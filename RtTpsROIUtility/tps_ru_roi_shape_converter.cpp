#include "StdAfx.h"
#include "RtTpsROIUtility/tps_ru_roi_shape_converter.h"

#include "RtTpsROIUtility/tps_ru_marching_cubes_tables.h"
#include "RtTpsROIUtility/tps_ru_polygon_utility.h"
#include "RtTpsFramework/tps_fw_contour.h"
#include "RtTpsFramework/tps_fw_contour_set.h"
#include "RtTpsFramework/tps_fw_signed_distance_field.h"
#include "RtTpsFramework/tps_fw_arithmetic.h"
#include "tps_ru_common.h"

#include "cuda.h"
#include "cuda_runtime.h"
#include "helper_cuda.h"

#include <iostream>
#include <utility>

#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "ipp/ippcv.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

extern "C"
void Launch_ContoursOverlappingLabelling(dim3 grid, dim3 threads, 
    contour* contours, int2* page_contours_info, int page_count, bool only_dirty);

extern "C"
void Launch_SDFLabelling(dim3 grid, dim3 threads, contour* contours, 
    int2* pci, int3 dim, float3 spacing, cudaPitchedPtr label, bool only_dirty);

extern "C"
void Launch_CalculateSDF(dim3 grid, dim3 threads, contour* contours, 
    int2* pci, int3 dim, float3 spacing, cudaPitchedPtr label, cudaPitchedPtr sdf);

extern "C"
void Launch_CalculateSDF_SingleSlice(dim3 grid, dim3 threads, contour* contours, 
    int2* pci, int3 dim, float3 spacing, cudaPitchedPtr label, int slice_num, float* plane_sdf);

extern "C"
void Launch_MarchingSquareLabelling(dim3 grid, dim3 threads, cudaTextureObject_t texObj, 
    double3 start, double3 vx, double3 vy, float iso_level, MS_MAP ms_map);

extern "C"
void Launch_CalculateSDFWithoutHat_SingleSlice(dim3 grid, dim3 threads, contour* contours, 
    int count, int2 dim, float3 spacing, float* plane_sdf);

void CalcPageContoursInfo(const ContourSet& cs, std::vector<contour>& contours,
    int* pci, double* bdBx, int& min_slice, int& max_slice)
{
    int slice_range[2];
    int slice_count = cs.GetSliceRange(slice_range[0], slice_range[1]);
    bdBx[0] = std::numeric_limits<float>::max();
    bdBx[1] = std::numeric_limits<float>::lowest();
    bdBx[2] = std::numeric_limits<float>::max();
    bdBx[3] = std::numeric_limits<float>::lowest();
    min_slice = slice_range[1];
    max_slice = slice_range[0];

    int axis[2];
    int idx = 0;
    for (int i = 0; i < 3; ++i)
    {
        if (i == cs.axis) continue;
        axis[idx++] = i;
    }

    PageContours* pc;
    PT3 p;
    contour* ctr = nullptr;
    for (int i = 0; i < slice_count; ++i)
    {
        pci[2*i] = contours.size();

        int id = i + slice_range[0];
        auto it = cs.page_contours.find(id);
        if (it == cs.page_contours.end())
        {
            continue;
        }

        pc = it->second;
        pci[2*i+1] = 0;
        for (int j = 0; j < pc->contour_list.size(); ++j)
        {
            int vc = pc->contour_list[j].VertexCount();
            if (vc < 3)
            {
                continue;
            }

            contours.push_back(contour());
            ctr = &contours.back();
            ctr->count = vc;
            ctr->hat_flag = 0;
            ctr->points = new float2[vc];
            ctr->is_dirty = pc->is_dirty;
            pci[2*i+1]++;

            for (int k = 0; k < vc; ++k)
            {
                p = cs.pat_to_vol.Transform(pc->contour_list[j].pt[k]);
                ctr->points[k].x = p.x * cs.spacing[0];
                ctr->points[k].y = p.y * cs.spacing[1];

                bdBx[0] = TPS_MIN(p.x, bdBx[0]);
                bdBx[1] = TPS_MAX(p.x, bdBx[1]);
                bdBx[2] = TPS_MIN(p.y, bdBx[2]);
                bdBx[3] = TPS_MAX(p.y, bdBx[3]);
            }
            min_slice = TPS_MIN(id, min_slice);
            max_slice = TPS_MAX(id, max_slice);
        }
    }
}

bool WholeContourSetToSDF(const ContourSet& cs, SignedDistanceField& sdf)
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    sdf.Clear();
    if (cs.IsEmpty())
    {
        return true;
    }

    //step 1: prepare contours information
    //get sdf start, size
    int slice_range[2];
    int slice_count = cs.GetSliceRange(slice_range[0], slice_range[1]);
    //page contour info
    int* pci = new int[slice_count * 2];
    memset(pci, 0, sizeof(int) * 2 * slice_count);
    std::vector<contour> contours;
    double bdBx[4];
    int min_slice, max_slice;
    CalcPageContoursInfo(cs, contours, pci, bdBx, min_slice, max_slice);
    if (min_slice > max_slice)
    {
        for (int i = 0; i < contours.size(); ++i)
        {
            TPS_DEL_ARRAY(contours[i].points);
        }
        TPS_DEL_ARRAY(pci);
        return false;
    }

    int start[3] = 
    {
        int(floor(bdBx[0]-1.0)), int(floor(bdBx[2]-1.0)), min_slice-1
    };
    sdf.SetStart(start);
    int dim[3] = 
    {
        int(ceil(bdBx[1] + 1)) - start[0] + 1, 
        int(ceil(bdBx[3] + 1)) - start[1] + 1, 
        max_slice - min_slice + 3, 
    };
    for (int i = 0; i < contours.size(); ++i)
    {
        for (int j = 0; j < contours[i].count; ++j)
        {
            contours[i].points[j].x -= start[0] * cs.spacing[0];
            contours[i].points[j].y -= start[1] * cs.spacing[1];
        }
    }

    int* real_pci = new int[2 * dim[2]];
    real_pci[0] = 0; real_pci[1] = 0;
    memcpy(real_pci + 2, pci + 2 * (min_slice - slice_range[0]), 
        (dim[2] - 2) * 2 * sizeof(int));
    real_pci[2 * dim[2] - 2] = real_pci[2 * dim[2] - 4] + real_pci[2 * dim[2] - 3];
    real_pci[2 * dim[2] - 1] = 0;
    delete[] pci;
    //TEST_PERFORMANCE_END("Prepare contours information");

    //step 2: contours overlapping labelling
    float2* ptr;
    for (int i = 0; i < contours.size(); ++i)
    {
        if (contours[i].count == 0) continue;
        ptr = contours[i].points;
        checkCudaErrors(cudaMalloc((void**)&contours[i].points, contours[i].count * sizeof(float2)));
        checkCudaErrors(cudaMemcpy(contours[i].points, ptr, 
            contours[i].count * sizeof(float2), cudaMemcpyHostToDevice));
        TPS_DEL_ARRAY(ptr);
    }
    contour* d_contours;
    checkCudaErrors(cudaMalloc((void**)&d_contours, contours.size() * sizeof(contour)));
    checkCudaErrors(cudaMemcpy(d_contours, contours._Myfirst, 
        contours.size() * sizeof(contour), cudaMemcpyHostToDevice));
    int2* d_pci;
    checkCudaErrors(cudaMalloc((void**)&d_pci, dim[2] * sizeof(int2)));
    checkCudaErrors(cudaMemcpy(d_pci, real_pci, 
        dim[2] * sizeof(int2), cudaMemcpyHostToDevice));

    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 dimGrid((contours.size() + BLOCK_SIZE * BLOCK_SIZE - 1) / (BLOCK_SIZE * BLOCK_SIZE));
    Launch_ContoursOverlappingLabelling(dimGrid, dimBlock, d_contours, d_pci, dim[2], false);
    //cudaDeviceSynchronize();
    //TEST_PERFORMANCE_END("ContoursOverlappingLabelling");

    //step 3: sdf labelling
    //prepare label buffer
    cudaExtent lbl_extent = make_cudaExtent(dim[0] * sizeof(char), dim[1], dim[2]); 
    cudaPitchedPtr lbl_ptr; 
    checkCudaErrors(cudaMalloc3D(&lbl_ptr, lbl_extent)); 

    int3 d_dim = make_int3(dim[0], dim[1], dim[2]);
    float3 d_spacing = make_float3(cs.spacing[0], cs.spacing[1], cs.spacing[2]);

    dimGrid.x = (dim[0] + BLOCK_SIZE - 1) / BLOCK_SIZE;
    dimGrid.y = (dim[1] + BLOCK_SIZE - 1) / BLOCK_SIZE;
    dimGrid.z = dim[2];
    Launch_SDFLabelling(dimGrid, dimBlock, d_contours, d_pci, d_dim, d_spacing, lbl_ptr, false);
    //cudaDeviceSynchronize();
    //TEST_PERFORMANCE_END("Launch_SDFLabelling");

    //step 4: calculate the signed distance field
    sdf.SetSize(dim);
    sdf.SetIsoLevel(0.f);
    float* data = sdf.GetData();
    MAT4 matModel2SDF = cs.pat_to_vol;
    matModel2SDF.Prepend(MakeTranslate(VEC3(-start[0], -start[1], -start[2])));
    sdf.SetPatToSDF(matModel2SDF);
  
    cudaExtent sdf_extent = make_cudaExtent(dim[0] * sizeof(float), dim[1], dim[2]); 
    cudaPitchedPtr sdf_ptr; 
    checkCudaErrors(cudaMalloc3D(&sdf_ptr, sdf_extent)); 
    Launch_CalculateSDF(dimGrid, dimBlock, d_contours, d_pci, 
        d_dim, d_spacing, lbl_ptr, sdf_ptr);
    //cudaDeviceSynchronize();
    //TEST_PERFORMANCE_END("Launch_CalculateSDF");
  
    cudaMemcpy3DParms parms = {0};
    parms.dstPtr = make_cudaPitchedPtr(data, 
        dim[0] * sizeof(float), dim[0], dim[1]);
    parms.extent = sdf_extent;
    parms.srcPtr = sdf_ptr;
    parms.kind = cudaMemcpyDeviceToHost;
    checkCudaErrors(cudaMemcpy3D(&parms));
    checkCudaErrors(cudaFree(sdf_ptr.ptr));
    checkCudaErrors(cudaFree(lbl_ptr.ptr));
    for (int i = 0; i < contours.size(); ++i)
    {
        if (contours[i].count == 0) continue;
        checkCudaErrors(cudaFree(contours[i].points));
    }
    checkCudaErrors(cudaFree(d_contours));
    checkCudaErrors(cudaFree(d_pci));
    delete[] real_pci;

    cs.ClearDirty();
    //cudaDeviceSynchronize();
    TEST_PERFORMANCE_END("WholeContourSetToSDF");
    return true;
}

void ResizeSDFViaNewBdBx(double bdBx[6], SignedDistanceField& sdf)
{
    int old_start[3];
    int old_dim[3];
    sdf.GetStart(old_start);
    sdf.GetSize(old_dim);
    double old_bdbx[6] = 
    {
        old_start[0], old_start[0] + old_dim[0] - 1,
        old_start[1], old_start[1] + old_dim[1] - 1,
        old_start[2], old_start[2] + old_dim[2] - 1,
    };
    int offset[6];
    for (int i = 0; i < 3; ++i)
    {
        offset[2*i] = static_cast<int>(floor(bdBx[2*i] - old_bdbx[2*i]));
        offset[2*i+1] = static_cast<int>(ceil(bdBx[2*i+1] - old_bdbx[2*i+1]));
    }

    if (offset[0] == 0 && offset[1] == 0 && offset[2] == 0 && 
        offset[3] == 0 && offset[4] == 0 && offset[5] == 0)
    {
        return;
    }

    int start[3] = 
    {
        old_start[0] + offset[0], old_start[1] + offset[2], old_start[2] + offset[4]
    };
    int dim[3] = 
    {
        old_dim[0] + offset[1] - offset[0],
        old_dim[1] + offset[3] - offset[2],
        old_dim[2] + offset[5] - offset[4],
    };

    int overlap[6] = 
    {
        old_bdbx[0] + TPS_MAX(0, offset[0]), old_bdbx[1] + TPS_MIN(0, offset[1]),
        old_bdbx[2] + TPS_MAX(0, offset[2]), old_bdbx[3] + TPS_MIN(0, offset[3]),
        old_bdbx[4] + TPS_MAX(0, offset[4]), old_bdbx[5] + TPS_MIN(0, offset[5]),
    };
    float* old_data = new float[old_dim[0] * old_dim[1] * old_dim[2]];
    memcpy(old_data, sdf.GetData(), old_dim[0] * old_dim[1] * old_dim[2] * sizeof(float));
    sdf.SetStart(start);
    sdf.SetSize(dim);
    int len = overlap[1] - overlap[0] + 1;

    float* slice = sdf.GetData() + (overlap[4] - start[2]) * dim[0] * dim[1];
    float* old_slice = old_data + (overlap[4] - old_start[2]) * old_dim[0] * old_dim[1];
    for (int i = overlap[4]; i <= overlap[5]; ++i)
    {
        for (int j = overlap[2]; j <= overlap[3]; ++j)
        {
            memcpy(slice + (j - start[1]) * dim[0], 
                old_slice + (j - old_start[1]) * old_dim[0], len * sizeof(float));
        }
        slice += dim[0] * dim[1];
        old_slice += old_dim[0] * old_dim[1];
    }
    TPS_DEL_ARRAY(old_data);
}

bool DirtySlicesToSDF(const ContourSet& cs, SignedDistanceField& sdf)
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    //nothing to be updated
    if (!cs.IsAnyDirty()) return true;

    //get sdf start, size
    //page contour info
    int slice_range[2];
    int slice_count = cs.GetSliceRange(slice_range[0], slice_range[1]);
    int* pci = new int[slice_count * 2];
    memset(pci, 0, sizeof(int) * 2 * slice_count);
    std::vector<contour> contours;
    double bdBx[4];
    int min_slice, max_slice;
    CalcPageContoursInfo(cs, contours, pci, bdBx, min_slice, max_slice);
    if (min_slice > max_slice)
    {
        for (int i = 0; i < contours.size(); ++i)
        {
            TPS_DEL_ARRAY(contours[i].points);
        }
        TPS_DEL_ARRAY(pci);
        return false;
    }

    double bdBx3d[6] = 
    {
        bdBx[0] - 1.0, 
        bdBx[0] + ceil(bdBx[1] - bdBx[0]) + 2.0,
        bdBx[2] - 1.0, 
        bdBx[2] + ceil(bdBx[3] - bdBx[2]) + 2.0,
        min_slice - 1, 
        max_slice + 1,
    };
    ResizeSDFViaNewBdBx(bdBx3d, sdf);

    int start[3];
    sdf.GetStart(start);
    int dim[3];
    sdf.GetSize(dim);

    for (int i = 0; i < contours.size(); ++i)
    {
        for (int j = 0; j < contours[i].count; ++j)
        {
            contours[i].points[j].x -= start[0] * cs.spacing[0];
            contours[i].points[j].y -= start[1] * cs.spacing[1];
        }
    }
    int* real_pci = new int[2 * dim[2]];
    real_pci[0] = 0; real_pci[1] = 0;
    memcpy(real_pci + 2, pci + 2 * (min_slice - slice_range[0]), 
        (dim[2] - 2) * 2 * sizeof(int));
    real_pci[2 * dim[2] - 2] = real_pci[2 * dim[2] - 4] + real_pci[2 * dim[2] - 3];
    real_pci[2 * dim[2] - 1] = 0;
    delete[] pci;
    //TEST_PERFORMANCE_END("Prepare segments information");

    float2* ptr;
    for (int i = 0; i < contours.size(); ++i)
    {
        if (contours[i].count == 0) continue;
        ptr = contours[i].points;
        checkCudaErrors(cudaMalloc((void**)&contours[i].points, contours[i].count * sizeof(float2)));
        checkCudaErrors(cudaMemcpy(contours[i].points, ptr, 
            contours[i].count * sizeof(float2), cudaMemcpyHostToDevice));
        TPS_DEL_ARRAY(ptr);
    }
    contour* d_contours;
    checkCudaErrors(cudaMalloc((void**)&d_contours, contours.size() * sizeof(contour)));
    checkCudaErrors(cudaMemcpy(d_contours, contours._Myfirst, 
        contours.size() * sizeof(contour), cudaMemcpyHostToDevice));
    int2* d_pci;
    checkCudaErrors(cudaMalloc((void**)&d_pci, dim[2] * sizeof(int2)));
    checkCudaErrors(cudaMemcpy(d_pci, real_pci, 
        dim[2] * sizeof(int2), cudaMemcpyHostToDevice));

    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 dimGrid((contours.size() + BLOCK_SIZE * BLOCK_SIZE - 1) / BLOCK_SIZE / BLOCK_SIZE);
    Launch_ContoursOverlappingLabelling(dimGrid, dimBlock, d_contours, d_pci, dim[2], true);
    //cudaDeviceSynchronize();
    //TEST_PERFORMANCE_END("ContoursOverlappingLabelling");

    //step 3: sdf labelling
    //prepare label buffer
    cudaExtent lbl_extent = make_cudaExtent(dim[0] * sizeof(char), dim[1], dim[2]); 
    cudaPitchedPtr lbl_ptr; 
    cudaMalloc3D(&lbl_ptr, lbl_extent); 

    int3 d_dim = make_int3(dim[0], dim[1], dim[2]);
    float3 d_spacing = make_float3(cs.spacing[0], cs.spacing[1], cs.spacing[2]);

    dimGrid.x = (dim[0] + BLOCK_SIZE - 1) / BLOCK_SIZE;
    dimGrid.y = (dim[1] + BLOCK_SIZE - 1) / BLOCK_SIZE;
    dimGrid.z = dim[2];
    Launch_SDFLabelling(dimGrid, dimBlock, d_contours, d_pci, d_dim, d_spacing, lbl_ptr, true);
    //cudaDeviceSynchronize();
    //TEST_PERFORMANCE_END("Launch_SDFLabelling");

    MAT4 matPat2SDF = cs.pat_to_vol;
    matPat2SDF.Prepend(MakeTranslate(VEC3(-start[0], -start[1], -start[2])));
    sdf.SetPatToSDF(matPat2SDF);

    int plane_size = dim[0] * dim[1];
    float* d_plane_sdf;
    checkCudaErrors(cudaMalloc((void**)&d_plane_sdf, plane_size * sizeof(float)));

    dimGrid.z = 1;
    float* data = sdf.GetData();
    for (int i = 0; i < dim[2]; ++i)
    {
        if (real_pci[2*i+1] == 0) continue;
        //check neighbor slice dirty flag
        //if (contours[real_pci[2*i]].is_dirty) continue;
        bool neighbor_dirty = false;
        for (int j = -1; j <= 1; ++j)
        {
            int idx = i + j;
            if (idx < 0 || idx >= dim[2] || real_pci[2*idx+1] == 0)
            {
                continue;
            }
            if (contours[real_pci[2*idx]].is_dirty)
            {
                neighbor_dirty = true;
                break;
            }
        }
        if (!neighbor_dirty) continue;

        Launch_CalculateSDF_SingleSlice(dimGrid, dimBlock, d_contours, 
            d_pci, d_dim, d_spacing, lbl_ptr, i, d_plane_sdf);
        checkCudaErrors(cudaMemcpy(data + i*plane_size, d_plane_sdf, 
            plane_size * sizeof(float), cudaMemcpyDeviceToHost));
    }
    //cudaDeviceSynchronize();
    //TEST_PERFORMANCE_END("Launch_CalculateSDF_SingleSlice");

    //clear resources
    checkCudaErrors(cudaFree(d_plane_sdf));
    checkCudaErrors(cudaFree(lbl_ptr.ptr));
    for (int i = 0; i < contours.size(); ++i)
    {
        if (contours[i].count == 0) continue;
        checkCudaErrors(cudaFree(contours[i].points));
    }
    checkCudaErrors(cudaFree(d_contours));
    checkCudaErrors(cudaFree(d_pci));
    delete[] real_pci;
    cs.ClearDirty();
    //cudaDeviceSynchronize();
    TEST_PERFORMANCE_END("DirtySlicesToSDF");
    return true;
}

bool ROIShapeConverter::ContourSetToSDF(
    const ContourSet& cs, 
    SignedDistanceField& sdf,
    bool onlyDirtySlices/* = true*/)
{
    return onlyDirtySlices ? DirtySlicesToSDF(cs, sdf) : WholeContourSetToSDF(cs, sdf);
}

int ms_segment_table[16][4] = 
{
    { -1, -1, -1, -1 },
    { 0, 3, -1, -1 },
    { 0, 1, -1, -1 },
    { 1, 3, -1, -1 },
    { 1, 2, -1, -1 },
    { 0, 1, 2, 3 },//5
    { 0, 2, -1, -1 },
    { 2, 3, -1, -1 },
    { 2, 3, -1, -1 },
    { 0, 2, -1, -1 },
    { 0, 3, 1, 2 },//10
    { 1, 2, -1, -1 },
    { 1, 3, -1, -1 },
    { 0, 1, -1, -1 },
    { 0, 3, -1, -1 },
    { -1, -1, -1, -1 },
};

int ms_edge_neiggbor[4] = { 2, 3, 0, 1 };

struct point3f
{
    float x;
    float y;
    float z;

    point3f(float a, float b, float c) : x(a), y(b), z(c){}
    point3f(){}
};

int FindNextEdge(int square_id, int incidence_edge, bool central_flag)
{
    if (square_id == 0 || square_id == 15) return -1;
    //handle ambiguous case
    if (square_id == 5 && !central_flag)
    {
        square_id = 10;
    }
    else if (square_id == 10 && !central_flag)
    {
        square_id = 5;
    }

    int next = -1;
    int* table = &ms_segment_table[square_id][0];
    for (int i = 0; i < 3; i+=2)
    {
        if (table[i] == -1)
            break;
        if (table[i] == incidence_edge)
        {
            next = table[i+1];
        }
        else if(table[i+1] == incidence_edge)
        {
            next = table[i];
        }
    }
    return next;
}

void TraceSquare(char* label, point3f* row_vertex, point3f* column_vertex, 
    int stride, int x0, int y0, int incidence_edge, std::vector<point3f>& contour)
{
    int idx;
    char square_id;
    bool central_flag;
    while(true)
    {
        idx = y0 * stride + x0;
        square_id = (label[idx] >> 4) & 0x0f;
        central_flag = (label[idx] & 0x04) != 0;
        int next = FindNextEdge(square_id, incidence_edge, central_flag);
        if (next == -1) return;

        switch(next)
        {
        case 2:
            idx += stride;
        case 0:
            {
                y0 = next ? y0+1 : y0-1;
                if ((label[idx] & 0x01) == 0) return;
                contour.push_back(row_vertex[idx]);
                label[idx] &= 0xfe;
            }
            break;
        case 1:
            idx++;
        case 3:
            {
                x0 = next == 1 ? x0+1 : x0-1;
                if ((label[idx] & 0x02) == 0) return;
                contour.push_back(column_vertex[idx]);
                label[idx] &= 0xfd;
            }
            break;
        }
        incidence_edge = ms_edge_neiggbor[next];
    }
}

void FindContours(char* label, point3f* row_vertex, point3f* column_vertex, 
    int width, int height, std::vector<std::vector<point3f>>& contours)
{
    int idx, incidence_edge;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            idx = y * width + x;
            if ((label[idx] & 0x03)== 0)
            {
                continue;
            }

            std::vector<point3f> contour;
            if (label[idx] & 0x01)
            {
                contour.push_back(row_vertex[idx]);
                label[idx] &= 0xfe;
                incidence_edge = 0;
            }
            else
            {
                contour.push_back(column_vertex[idx]);
                label[idx] &= 0xfd;
                incidence_edge = 3;
            }
            TraceSquare(label, row_vertex, column_vertex, width, 
                x, y, incidence_edge, contour);
            if (contour.size() > 2)
            {
                contours.push_back(contour);
            }
        }
    }
}

int edge_index[24] = 
{
    0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7,
};

PT3 VertexInterpolation(const PT3* vertices, int index)
{
    PT3 start, end;
    switch(index)
    {
    case 0:
        start = vertices[0];
        end = vertices[1];
        break;
    case 1:
        start = vertices[1];
        end = vertices[2];
        break;
    case 2:
        start = vertices[2];
        end = vertices[3];
        break;
    case 3:
        start = vertices[0];
        end = vertices[3];
        break;
    case 4:
        start = vertices[4];
        end = vertices[5];
        break;
    case 5:
        start = vertices[5];
        end = vertices[6];
        break;
    case 6:
        start = vertices[6];
        end = vertices[7];
        break;
    case 7:
        start = vertices[4];
        end = vertices[7];
        break;
    case 8:
        start = vertices[0];
        end = vertices[4];
        break;
    case 9:
        start = vertices[1];
        end = vertices[5];
        break;
    case 10:
        start = vertices[2];
        end = vertices[6];
        break;
    case 11:
        start = vertices[3];
        end = vertices[7];
        break;
    }

    VEC3 vec = (0.0 - start.z) / (end.z - start.z) * (end - start);
    return start + vec;
}

//if no intersection, return false
bool AdaptPlaneToMinBounding(const SignedDistanceField& sdf, PLANE& plane)
{
    PT3 corners[4];
    plane.GetVertices(corners);
    VEC3 axis[3] = 
    {
        corners[1] - corners[0],
        corners[3] - corners[0],
        plane.GetNormal(),
    };
    for (int i = 0; i < 3; ++i)
    {
        axis[i].Normalize();
    }
    MAT4 matPlane2Pat(
        axis[0].x, axis[0].y, axis[0].z, 0.0, 
        axis[1].x, axis[1].y, axis[1].z, 0.0, 
        axis[2].x, axis[2].y, axis[2].z, 0.0, 
        corners[0].x, corners[0].y, corners[0].z, 1.0);
    MAT4 matPat2Plane = matPlane2Pat.Inverse();
    MAT4 matSDFToPlane = sdf.GetPatToSDF().Inverse();
    matSDFToPlane.Prepend(matPat2Plane);

    int dim[3];
    sdf.GetSize(dim);
    PT3 vertices[8] = 
    {
        PT3(-0.5, -0.5, -0.5), 
        PT3(dim[0]-0.5, -0.5, -0.5), 
        PT3(dim[0]-0.5, dim[1]-0.5, -0.5), 
        PT3(-0.5, dim[1]-0.5, -0.5),
        PT3(-0.5, -0.5, dim[2]-0.5), 
        PT3(dim[0]-0.5, -0.5, dim[2]-0.5), 
        PT3(dim[0]-0.5, dim[1]-0.5, dim[2]-0.5), 
        PT3(-0.5, dim[1]-0.5, dim[2]-0.5),
    };

    int cubeIndex = 0;
    for (int i = 0; i < 8; ++i)
    {
        vertices[i] = matSDFToPlane.Transform(vertices[i]);
        if (vertices[i].z > 0.0)
        {
            cubeIndex |= (0x01 << i);
        }
    }
    if (cubeIndex == 0 || cubeIndex == 255) return false;

    std::vector<PT3> contours;
    for (int i = 0; i < 16; ++i)
    {
        int idx = MarchingCubesTables::mTriTable[cubeIndex][i];
        if (idx == -1)
        {
            break;
        }
        contours.push_back(VertexInterpolation(vertices, idx));
    }

    if (contours.empty()) return false;
    double bdry[4] = 
    {
        contours[0].x, contours[0].x, contours[0].y, contours[0].y
    };
    for (int i = 1; i < contours.size(); ++i)
    {
        bdry[0] = TPS_MIN(bdry[0], contours[i].x);
        bdry[1] = TPS_MAX(bdry[1], contours[i].x);
        bdry[2] = TPS_MIN(bdry[2], contours[i].y);
        bdry[3] = TPS_MAX(bdry[3], contours[i].y);
    }
    corners[0] = matPlane2Pat.Transform(PT3(bdry[0], bdry[2], 0.0));
    corners[1] = matPlane2Pat.Transform(PT3(bdry[1], bdry[2], 0.0));
    corners[3] = matPlane2Pat.Transform(PT3(bdry[0], bdry[3], 0.0));
    plane = PLANE(corners[0], corners[3], corners[1]);
    return true;
}

void ROIShapeConverter::SliceSDFToContours(
    const SignedDistanceField& sdf, 
    const PLANE& rect, 
    std::vector<Contour>& contours)
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    contours.clear();

    //upload sdf buffer to gpu
    int dim[3];
    sdf.GetSize(dim);

    cudaExtent cuExtent = make_cudaExtent(dim[0], dim[1], dim[2]);

    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<float>();
    cudaArray_t cuArray = 0;
    checkCudaErrors(cudaMalloc3DArray(&cuArray, &channelDesc, cuExtent));
    cudaMemcpy3DParms parms = {0};
    parms.dstArray = cuArray;
    parms.extent = cuExtent;
    parms.srcPtr = make_cudaPitchedPtr(sdf.GetData(), 
        dim[0] * sizeof(float), dim[0], dim[1]);
    parms.kind = cudaMemcpyHostToDevice;
    checkCudaErrors(cudaMemcpy3D(&parms));

    cudaResourceDesc resDesc;
    memset(&resDesc, 0, sizeof(cudaResourceDesc));
    resDesc.resType = cudaResourceTypeArray;
    resDesc.res.array.array = cuArray;

    cudaTextureDesc texDesc;
    memset(&texDesc, 0, sizeof(cudaTextureDesc));
    texDesc.addressMode[0] = cudaAddressModeClamp;
    texDesc.addressMode[1] = cudaAddressModeClamp;
    texDesc.addressMode[2] = cudaAddressModeClamp;
    texDesc.filterMode = cudaFilterModeLinear;
    texDesc.readMode = cudaReadModeElementType;
    texDesc.normalizedCoords = 1;

    cudaTextureObject_t texObj = 0;
    checkCudaErrors(cudaCreateTextureObject(&texObj, &resDesc, &texDesc, NULL));

    PT3 corners[4];
    rect.GetVertices(corners);
    //
    auto matSDF2NSDF = MAT4(dim[0], 0, 0, 0, 
        0, dim[1], 0, 0, 0, 0, dim[2], 0, -0.5, -0.5, -0.5, 1).Inverse();
    auto matPat2SDF = sdf.GetPatToSDF();
    auto matPat2NSDF = matSDF2NSDF;
    matPat2NSDF.Append(matPat2SDF);

    auto start = matPat2NSDF.Transform(corners[0]);
    auto p1 = matPat2NSDF.Transform(corners[1]);
    auto p3 = matPat2NSDF.Transform(corners[3]);
    auto vx = p1 - start;
    auto vy = p3 - start;

    auto matVol2Pat = sdf.GetPatToSDF().Inverse();
    double spacing_p = 
        matVol2Pat.Transform(VEC3(1, 0, 0)).Magnitude();
    spacing_p = TPS_MIN(spacing_p, 
        matVol2Pat.Transform(VEC3(0, 1, 0)).Magnitude());
    spacing_p = TPS_MIN(spacing_p, 
        matVol2Pat.Transform(VEC3(0, 0, 1)).Magnitude());

    auto vx_p = matVol2Pat.Transform(vx);
    vx_p.Normalize();
    vx_p *= spacing_p;
    auto vy_p = matVol2Pat.Transform(vy);
    vy_p.Normalize();
    vy_p *= spacing_p;
    double spacing[2] = 
    {
        matPat2SDF.Transform(vx_p).Magnitude(),
        matPat2SDF.Transform(vy_p).Magnitude(),
    };

    int size[2] = 
    {
        (int)ceil(vx.Magnitude() / spacing[0]) + 1, 
        (int)ceil(vy.Magnitude() / spacing[1]) + 1,
    };
    vx.Normalize();
    vy.Normalize();
    vx *= spacing[0];
    vy *= spacing[1];

    double3 d_start = make_double3(start.x, start.y, start.z);
    double3 d_vx = make_double3(vx.x, vx.y, vx.z);
    double3 d_vy = make_double3(vy.x, vy.y, vy.z);

    MS_MAP ms_map;
    ms_map.map_size = make_int2(size[0], size[1]);
    //initialize map memory on device
    int w_b = size[0] * sizeof(char);
    checkCudaErrors(cudaMallocPitch(&ms_map.label_map, &ms_map.l_pitch, w_b, size[1]));
    checkCudaErrors(cudaMemset2D(ms_map.label_map, ms_map.l_pitch, 0, w_b, size[1]));
    checkCudaErrors(cudaMallocPitch(&ms_map.row_vertex_map, 
        &ms_map.v_pitch, size[0] * sizeof(float3), size[1]));
    checkCudaErrors(cudaMallocPitch(&ms_map.column_vertex_map, 
        &ms_map.v_pitch, size[0] * sizeof(float3), size[1]));

    dim3 blockDim(BLOCK_SIZE, BLOCK_SIZE);
    dim3 gridDim((size[0] + BLOCK_SIZE - 2) / BLOCK_SIZE, 
        (size[1] + BLOCK_SIZE - 2) / BLOCK_SIZE);

    Launch_MarchingSquareLabelling(gridDim, blockDim, texObj, d_start, 
        d_vx, d_vy, sdf.GetIsoLevel(), ms_map);

    char* label_map = new char[size[0] * size[1]];
    point3f* row_vertex = new point3f[size[0] * size[1]];
    point3f* column_vertex = new point3f[size[0] * size[1]];
    checkCudaErrors(cudaMemcpy2D(label_map, w_b, ms_map.label_map, ms_map.l_pitch, 
        w_b, size[1], cudaMemcpyDeviceToHost));
    w_b = size[0] * sizeof(point3f);
    checkCudaErrors(cudaMemcpy2D(row_vertex, w_b, ms_map.row_vertex_map, ms_map.v_pitch, 
        w_b, size[1], cudaMemcpyDeviceToHost));
    checkCudaErrors(cudaMemcpy2D(column_vertex, w_b, ms_map.column_vertex_map, ms_map.v_pitch, 
        w_b, size[1], cudaMemcpyDeviceToHost));

    //release gpu resources
    checkCudaErrors(cudaFree(ms_map.label_map));
    checkCudaErrors(cudaFree(ms_map.column_vertex_map));
    checkCudaErrors(cudaFree(ms_map.row_vertex_map));
    checkCudaErrors(cudaDestroyTextureObject(texObj));
    checkCudaErrors(cudaFreeArray(cuArray));

    std::vector<std::vector<point3f>> _contours;
    FindContours(label_map, row_vertex, column_vertex, size[0], size[1], _contours);

    delete[] label_map;
    delete[] row_vertex;
    delete[] column_vertex;

    MAT4 matrix = matPat2NSDF.Inverse();
    //convert data structure
    for (int i = 0; i < _contours.size(); ++i)
    {
        contours.push_back(Contour());
        Contour& contour = contours.back();
        contour.pt.resize(_contours[i].size());

        for (int j = 0; j < _contours[i].size(); ++j)
        {
            contour.pt[j].x = _contours[i][j].x;
            contour.pt[j].y = _contours[i][j].y;
            contour.pt[j].z = _contours[i][j].z;
            contour.pt[j] = matrix.Transform(contour.pt[j]);
        }
    }
    TEST_PERFORMANCE_END("SliceSDFToContours_GPU");
}

void ROIShapeConverter::SliceSDFToContours_CPU(
    const SignedDistanceField& sdf, 
    const PLANE& rect,
    std::vector<Contour>& contours)
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    contours.clear();

    PLANE plane = rect;
    if (!AdaptPlaneToMinBounding(sdf, plane))
    {
        //no intersection
        return;
    }

    MAT4 matSDF2P = sdf.GetPatToSDF().Inverse();
    double spacing[3] = 
    {
        matSDF2P.Transform(VEC3(1, 0, 0)).Magnitude(),
        matSDF2P.Transform(VEC3(0, 1, 0)).Magnitude(),
        matSDF2P.Transform(VEC3(0, 0, 1)).Magnitude(),
    };
    double spacing_p = spacing[0];
    spacing_p += spacing[1];
    spacing_p += spacing[2];
    spacing_p /= 3;

    int dim[3];
    sdf.GetSize(dim);
    auto matSDF2NSDF = MAT4(dim[0], 0, 0, 0, 
        0, dim[1], 0, 0, 0, 0, dim[2], 0, -0.5, -0.5, -0.5, 1).Inverse();
    auto matPat2NSDF = sdf.GetPatToSDF();
    matPat2NSDF.Prepend(matSDF2NSDF);

    PT3 corners[4];
    plane.GetVertices(corners);
    auto start = matPat2NSDF.Transform(corners[0]);
    VEC3 vx = corners[1] - corners[0];
    vx.Normalize(); 
    VEC3 vy = corners[3] - corners[0];
    vy.Normalize();
    vx = matPat2NSDF.Transform(vx * spacing_p/*spacing[0]*/);
    vy = matPat2NSDF.Transform(vy * spacing_p/*spacing[1]*/);
    int size[2] = 
    {
        (int)ceil(1.0 / vx.Magnitude()) + 1, 
        (int)ceil(1.0 / vy.Magnitude()) + 1,
    };

    PT3 p;
    float* plane_buffer = new float[(size[0] + 1) * (size[1] + 1)];
    float iso_level = sdf.GetIsoLevel();
    for (int i = 0; i < size[1] + 1; ++i)
    {
        for (int j = 0; j < size[0] + 1; ++j)
        {
            p = start + i * vy + j * vx;
            p.x = p.x * dim[0] - 0.5;
            p.y = p.y * dim[1] - 0.5;
            p.z = p.z * dim[2] - 0.5;
            plane_buffer[i * (size[0] + 1) + j] = sdf(p.x, p.y, p.z) - iso_level;
        }
    }

    int plane_size = size[0] * size[1];
    char* label_map = new char[plane_size];
    memset(label_map, 0, plane_size * sizeof(char));
    auto row_vertex = new point3f[plane_size];
    auto column_vertex = new point3f[plane_size];

    float sd_square[4];
    int idx = 0;
    PT3 p0, p1;
    for (int i = 0; i < size[1]; ++i)
    {
        for (int j = 0; j < size[0]; ++j)
        {
            idx = i * (size[0] + 1) + j;
            sd_square[0] = plane_buffer[idx];
            sd_square[1] = plane_buffer[idx + 1];
            sd_square[2] = plane_buffer[idx + size[0] + 2];
            sd_square[3] = plane_buffer[idx + size[0] + 1];
            idx -= i;

            char id = 0;
            for(int k = 0; k < 4; ++k)
            {
                if(sd_square[k] >= 0) id |= (0x01<<k);
            }

            label_map[idx] |= char(id << 0x04);
            if(id == 0 || id == 15 || id == 4 || id == 11) continue;

            p0 = start + i * vy + j * vx;
            if(abs(sd_square[0]) <= 1e-6)
            {
                row_vertex[idx] = point3f(p0.x, p0.y, p0.z);
                column_vertex[idx] = row_vertex[idx];
                label_map[idx] |= 0x03;
            }
            else 
            {
                if(sd_square[0] * sd_square[1] <= 1e-6)
                {
                    p1 = p0 + -sd_square[0] / (sd_square[1] - sd_square[0]) * vx;
                    row_vertex[idx] = point3f(p1.x, p1.y, p1.z);
                    label_map[idx] |= 0x01;
                }
                if(sd_square[0] * sd_square[3] <= 1e-6)
                {
                    p1 = p0 + -sd_square[0] / (sd_square[3] - sd_square[0]) * vy;
                    column_vertex[idx] = point3f(p1.x, p1.y, p1.z);
                    label_map[idx] |= 0x02;
                }
            }

            //ambiguous case
            if (id == 5 || id == 10)
            {
                float sum = 0;
                for (int k = 0; k < 4; ++k)
                {
                    sum += sd_square[k];
                }
                float central_sd = sum / 4;
                if (central_sd >= 0)
                {
                    label_map[idx] |= 0x04;
                }
            }
        }
    }

    std::vector<std::vector<point3f>> _contours;
    FindContours(label_map, row_vertex, column_vertex, size[0], size[1], _contours);

    delete[] label_map;
    delete[] row_vertex;
    delete[] column_vertex;
    delete[] plane_buffer;

    //convert data structure
    MAT4 matrix = matPat2NSDF.Inverse();
    Contour ctr;
    for (int i = 0; i < _contours.size(); ++i)
    {
        ctr.pt.resize(_contours[i].size());
        for (int j = 0; j < _contours[i].size(); ++j)
        {
            ctr.pt[j] = matrix.Transform(PT3(
                _contours[i][j].x, _contours[i][j].y, _contours[i][j].z));
        }
        contours.push_back(ctr);
    }
    TEST_PERFORMANCE_END("SliceSDFToContours_CPU");
}

void ROIShapeConverter::SDFToContourSet(
    const SignedDistanceField& sdf, 
    int axis, ContourSet& cs)
{
    TEST_PERFORMANCE_INIT
    TEST_PERFORMANCE_BEGIN
    cs.Clear();

    int sdf_start[3];
    sdf.GetStart(sdf_start);
    auto matPat2SDF = sdf.GetPatToSDF();
    cs.pat_to_vol = matPat2SDF;
    cs.pat_to_vol.Prepend(MakeTranslate(VEC3(sdf_start[0], sdf_start[1], sdf_start[2])));
    switch (axis)
    {
    case 0:
        {
            QUAT myQuat(-PI / 2, VEC3(0, 1, 0));
            cs.pat_to_vol.Prepend(myQuat.ToMatrix());
            myQuat = QUAT(-PI / 2, VEC3(0, 0, 1));
            cs.pat_to_vol.Prepend(myQuat.ToMatrix());
        }
        break;
    case 1:
        {
            QUAT myQuat(PI / 2, VEC3(1, 0, 0));
            cs.pat_to_vol.Prepend(myQuat.ToMatrix());
            myQuat = QUAT(PI / 2, VEC3(0, 0, 1));
            cs.pat_to_vol.Prepend(myQuat.ToMatrix());
        }
        break;
    }
    auto matCSVol2Pat = cs.pat_to_vol.Inverse();
    VEC3 vecAxis[3] = 
    {
        matCSVol2Pat.Transform(VEC3(1, 0, 0)),
        matCSVol2Pat.Transform(VEC3(0, 1, 0)),
        matCSVol2Pat.Transform(VEC3(0, 0, 1)),
    };
    for (int i = 0; i < 3; ++i)
    {
        cs.spacing[i] = vecAxis[i].Magnitude();
        vecAxis[i].Normalize();
    }
    cs.normal = vecAxis[2];
//     double spacing_p = cs.spacing[0];
//     spacing_p += cs.spacing[1];
//     spacing_p += cs.spacing[2];
//     spacing_p /= 3;
    //double spacing_p = TPS_MIN(cs.spacing[0], TPS_MIN(cs.spacing[1], cs.spacing[2]));

    int dim[3];
    sdf.GetSize(dim);
    auto matSDF2NSDF = MAT4(dim[0], 0, 0, 0, 
        0, dim[1], 0, 0, 0, 0, dim[2], 0, -0.5, -0.5, -0.5, 1).Inverse();
    auto matPat2NSDF = matPat2SDF;
    matPat2NSDF.Prepend(matSDF2NSDF);
    auto vx = matPat2NSDF.Transform(vecAxis[0] * cs.spacing[0]);
    auto vy = matPat2NSDF.Transform(vecAxis[1] * cs.spacing[1]);
    auto normal = matPat2NSDF.Transform(vecAxis[2] * cs.spacing[2]);
    int size[2] = 
    {
        (int)ceil(1.0 / vx.Magnitude()) + 1, 
        (int)ceil(1.0 / vy.Magnitude()) + 1,
    };
    PT3 start = matSDF2NSDF.Transform(PT3(0, 0, 0));

    cudaExtent cuExtent = make_cudaExtent(dim[0], dim[1], dim[2]);
    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<float>();
    cudaArray_t cuArray = 0;
    checkCudaErrors(cudaMalloc3DArray(&cuArray, &channelDesc, cuExtent));
    cudaMemcpy3DParms parms = {0};
    parms.dstArray = cuArray;
    parms.extent = cuExtent;
    parms.srcPtr = make_cudaPitchedPtr(sdf.GetData(), 
        dim[0] * sizeof(float), dim[0], dim[1]);
    parms.kind = cudaMemcpyHostToDevice;
    checkCudaErrors(cudaMemcpy3D(&parms));
 
    cudaResourceDesc resDesc;
    memset(&resDesc,0,sizeof(cudaResourceDesc));
    resDesc.resType = cudaResourceTypeArray;
    resDesc.res.array.array = cuArray;
 
    cudaTextureDesc texDesc;
    memset(&texDesc, 0, sizeof(cudaTextureDesc));
    texDesc.addressMode[0] = cudaAddressModeClamp;
    texDesc.addressMode[1] = cudaAddressModeClamp;
    texDesc.addressMode[2] = cudaAddressModeClamp;
    texDesc.filterMode = cudaFilterModeLinear;
    texDesc.readMode = cudaReadModeElementType;
    texDesc.normalizedCoords = 1;
 
    cudaTextureObject_t texObj = 0;
    checkCudaErrors(cudaCreateTextureObject(&texObj, &resDesc, &texDesc, NULL));
 
    double3 d_vx = make_double3(vx.x, vx.y, vx.z);
    double3 d_vy = make_double3(vy.x, vy.y, vy.z);
 
    MS_MAP ms_map;
    ms_map.map_size = make_int2(size[0], size[1]);
    //initialize map memory on device
    int w_b_char = size[0] * sizeof(char);
    int w_b_point = size[0] * sizeof(point3f);
    checkCudaErrors(cudaMallocPitch(&ms_map.label_map, &ms_map.l_pitch, w_b_char, size[1]));
    checkCudaErrors(cudaMallocPitch(&ms_map.row_vertex_map, 
        &ms_map.v_pitch, size[0] * sizeof(float3), size[1]));
    checkCudaErrors(cudaMallocPitch(&ms_map.column_vertex_map, 
        &ms_map.v_pitch, size[0] * sizeof(float3), size[1]));

    char* label_map = new char[size[0] * size[1]];
    point3f* row_vertex = new point3f[size[0] * size[1]];
    point3f* column_vertex = new point3f[size[0] * size[1]];

    dim3 blockDim(BLOCK_SIZE, BLOCK_SIZE);
    dim3 gridDim((size[0] + BLOCK_SIZE - 2) / BLOCK_SIZE, 
        (size[1] + BLOCK_SIZE - 2) / BLOCK_SIZE);

    //calculate reduction epsilon
    float epsilon = sin(atan(1.0 / 3)) * TPS_MIN(cs.spacing[0], cs.spacing[1])/*spacing_p*/;

    auto matNSDF2Pat = matPat2NSDF.Inverse();
    int slice_id;
    float iso_level = sdf.GetIsoLevel();

    for (int z = 0; z < dim[axis]; ++z, start += normal)
    {
        slice_id = z + sdf_start[axis];
        //使用double3避免由于float3引起的精度问题，在kernel代码中调用tex时再转float访问纹理
        double3 d_start = make_double3(start.x, start.y, start.z);
        //checkCudaErrors(cudaMemset2D(ms_map.label_map, ms_map.l_pitch, 0, w_b_char, size[1]));
        Launch_MarchingSquareLabelling(gridDim, blockDim, texObj, d_start, 
            d_vx, d_vy, iso_level, ms_map);

        checkCudaErrors(cudaMemcpy2D(label_map, w_b_char, ms_map.label_map, ms_map.l_pitch, 
            w_b_char, size[1], cudaMemcpyDeviceToHost));
        checkCudaErrors(cudaMemcpy2D(row_vertex, w_b_point, ms_map.row_vertex_map, ms_map.v_pitch, 
            size[0] * sizeof(float3), size[1], cudaMemcpyDeviceToHost));
        checkCudaErrors(cudaMemcpy2D(column_vertex, w_b_point, ms_map.column_vertex_map, ms_map.v_pitch, 
            size[0] * sizeof(float3), size[1], cudaMemcpyDeviceToHost));

        std::vector<std::vector<point3f>> _contours;
        FindContours(label_map, row_vertex, column_vertex, size[0], size[1], _contours);

        if (_contours.empty()) continue;
        std::vector<Contour> contour_list;
        
        for (int j = 0; j < _contours.size(); ++j)
        {
            if (_contours[j].size() < 3) continue;

            contour_list.push_back(Contour());
            Contour& ctr = contour_list.back();
            ctr.slice_number = slice_id;
            ctr.pt.resize(_contours[j].size());

            for (int k = 0; k < ctr.VertexCount(); ++k)
            {
                ctr.pt[k].x = _contours[j][k].x;
                ctr.pt[k].y = _contours[j][k].y;
                ctr.pt[k].z = _contours[j][k].z;
                ctr.pt[k] = matNSDF2Pat.Transform(ctr.pt[k]);
            }
            PolygonUtility::RDPPolygonReduce(ctr.pt, cs.normal, epsilon);
        }
        if (contour_list.empty()) continue;
        PageContours* pc = new PageContours();
        pc->contour_list = contour_list;
        pc->is_dirty = false;
        pc->slice_number = slice_id;
        cs.page_contours[slice_id] = pc;
    }

    //release memory
    checkCudaErrors(cudaFree(ms_map.label_map));
    checkCudaErrors(cudaFree(ms_map.column_vertex_map));
    checkCudaErrors(cudaFree(ms_map.row_vertex_map));
    checkCudaErrors(cudaDestroyTextureObject(texObj));
    checkCudaErrors(cudaFreeArray(cuArray));
    delete[] label_map;
    delete[] row_vertex;
    delete[] column_vertex;
    TEST_PERFORMANCE_END("SDFToContourSet")
}

void ROIShapeConverter::SDFToContourSet_CPU(
    const SignedDistanceField& sdf, 
    int axis, ContourSet& cs)
{
    TEST_PERFORMANCE_INIT
    TEST_PERFORMANCE_BEGIN
    cs.Clear();

    int sdf_start[3];
    sdf.GetStart(sdf_start);
    int dim[3];
    sdf.GetSize(dim);
    auto matPat2SDF = sdf.GetPatToSDF();
    cs.pat_to_vol = matPat2SDF;
    cs.pat_to_vol.Prepend(MakeTranslate(VEC3(sdf_start[0], sdf_start[1], sdf_start[2])));
    switch (axis)
    {
    case 0:
        {
            QUAT myQuat(-PI / 2, VEC3(0, 1, 0));
            cs.pat_to_vol.Prepend(myQuat.ToMatrix());
            myQuat = QUAT(-PI / 2, VEC3(0, 0, 1));
            cs.pat_to_vol.Prepend(myQuat.ToMatrix());
        }
        break;
    case 1:
        {
            QUAT myQuat(PI / 2, VEC3(1, 0, 0));
            cs.pat_to_vol.Prepend(myQuat.ToMatrix());
            myQuat = QUAT(PI / 2, VEC3(0, 0, 1));
            cs.pat_to_vol.Prepend(myQuat.ToMatrix());
        }
        break;
    }
    auto matCSVol2Pat = cs.pat_to_vol.Inverse();
    VEC3 vecAxis[3] = 
    {
        matCSVol2Pat.Transform(VEC3(1, 0, 0)),
        matCSVol2Pat.Transform(VEC3(0, 1, 0)),
        matCSVol2Pat.Transform(VEC3(0, 0, 1)),
    };
    for (int i = 0; i < 3; ++i)
    {
        cs.spacing[i] = vecAxis[i].Magnitude();
        vecAxis[i].Normalize();
    }
    cs.normal = vecAxis[2];

    auto matSDF2Pat = matPat2SDF.Inverse();
    PT3 org = matSDF2Pat.Transform(PT3(0, 0, 0));
    PT3 px = org + vecAxis[0];
    PT3 py = org + vecAxis[1];
    VEC3 step = vecAxis[2] * cs.spacing[2];

    int slice_id;
    std::vector<Contour> contours;
    for (int z = 0; z < dim[axis]; ++z, org += step, px += step, py += step)
    {
        slice_id = z + sdf_start[axis];
        PLANE plane (org, py, px);
        SliceSDFToContours_CPU(sdf, plane, contours);
        if (contours.empty()) continue;

        PageContours* pc = new PageContours();
        pc->contour_list = contours;
        pc->is_dirty = false;
        pc->slice_number = slice_id;
        cs.page_contours[slice_id] = pc;
    }
    TEST_PERFORMANCE_END("SDFToContourSet_CPU")
}

bool ROIShapeConverter::SubVolumeToSDF(
    McsfGeometry::SubVolume& sv, 
    const MAT4& matP2V,
    SignedDistanceField& sdf)
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;

    char* sv_data = sv.GetData();
    int start[3];
    sv.GetOffSet(start[0], start[1], start[2]);
    int size[3];
    sv.GetSize(size[0], size[1], size[2]);

    MAT4 matV2P = matP2V.Inverse();
    double spacing[3] = 
    {
        matV2P.Transform(VEC3(1, 0, 0)).Magnitude(),
        matV2P.Transform(VEC3(0, 1, 0)).Magnitude(),
        matV2P.Transform(VEC3(0, 0, 1)).Magnitude()
    };

    int src_step = size[0] * sizeof(unsigned char); 
    int dst_step = size[0] * sizeof(float); 
    int plane_size = size[0] * size[1];
    IppStatus status = ippStsNoErr;
    auto outer_buffer = new unsigned char[plane_size];
    auto outer_dst = new float[plane_size];
    IppiSize roi_size = { size[0], size[1] };
    Ipp32f metrics[2] = { spacing[0], spacing[1] };
    float* sdf_data = new float[plane_size * size[2]];
    for (int i = 0; i < size[2]; ++i)
    {
        float* dst = sdf_data + i * plane_size;
        auto inner_buffer = (unsigned char*)sv_data + i * plane_size;
        status = ippiDistanceTransform_3x3_8u32f_C1R(
            inner_buffer, src_step, dst, dst_step, roi_size, metrics);
        if (status != ippStsNoErr)
        {
            printf("\nippiDistanceTransform_3x3_8u32f_C1R error: %d", status);
            TPS_DEL_ARRAY(outer_buffer);
            TPS_DEL_ARRAY(outer_dst);
            TPS_DEL_ARRAY(sdf_data);
            return false;
        }

        for (int j = 0; j < plane_size; ++j)
        {
            outer_buffer[j] = 1.f - inner_buffer[j];
        }
        status = ippiDistanceTransform_3x3_8u32f_C1R(
            outer_buffer, src_step, outer_dst, dst_step, roi_size, metrics);
        if (status != ippStsNoErr)
        {
            printf("\nippiDistanceTransform_3x3_8u32f_C1R error: %d", status);
            TPS_DEL_ARRAY(outer_buffer);
            TPS_DEL_ARRAY(outer_dst);
            TPS_DEL_ARRAY(sdf_data);
            return false;
        }

        for (int j = 0; j < plane_size; ++j)
        {
            dst[j] -= outer_dst[j];
        }
    }
    TPS_DEL_ARRAY(outer_buffer);
    TPS_DEL_ARRAY(outer_dst);

    //每个维度扩张1个体素，距离值为-1
    int sdf_size[3];
    for (int i = 0; i < 3; ++i)
    {
        start[i] -= 1;
        sdf_size[i] = size[i] + 2;
    }
    sdf.Clear();
    sdf.SetIsoLevel(0.f);
    sdf.SetStart(start);
    sdf.SetSize(sdf_size);

    float* expanded_sdf_data = sdf.GetData();
    MAT4 matModel2SDF = matP2V;
    matModel2SDF.Prepend(MakeTranslate(VEC3(-start[0], -start[1], -start[2])));
    sdf.SetPatToSDF(matModel2SDF);
    //
    int sdf_plane_size = sdf_size[0] * sdf_size[1];
    for (int i = 0; i < size[2]; ++i)
    {
        float* tgt = expanded_sdf_data + (i+1) * sdf_plane_size + sdf_size[0] + 1;
        float* src = sdf_data + i * plane_size;
        for (int j = 0; j < size[1]; ++j, tgt += sdf_size[0], src += size[0])
        {
            memcpy(tgt, src, size[0] * sizeof(float));
        }
    }
    TPS_DEL_ARRAY(sdf_data);

    //该接口只用于自动分割，对于hat的处理采取较简单的方式，只检查首尾两头
    int head = 0, tail = sdf_size[2]-1;
    for (int i = 1; i < sdf_size[2]-1; ++i)
    {
        float* buff = expanded_sdf_data + i * sdf_plane_size;
        for (int j = 0; j < sdf_plane_size; ++j)
        {
            if (buff[j] >= 0)
            {
                head = i;
                break;
            }
        }
        if (head > 0) break;
    }
    for (int i = sdf_size[2]-2; i >= 1; --i)
    {
        float* buff = expanded_sdf_data + i * sdf_plane_size;
        for (int j = 0; j < sdf_plane_size; ++j)
        {
            if (buff[j] >= 0)
            {
                tail = i;
                break;
            }
        }
        if (tail < sdf_size[2]-1) break;
    }
    if (head == 0 || tail == sdf_size[2]-1) return true;
    float* buffs[4] = 
    {
        expanded_sdf_data + (head-1) * sdf_plane_size,
        expanded_sdf_data + head * sdf_plane_size,
        expanded_sdf_data + tail * sdf_plane_size,
        expanded_sdf_data + (tail+1) * sdf_plane_size
    };
    for (int i = 0; i < sdf_plane_size; ++i)
    {
        buffs[0][i] = -0.5f * spacing[2];
        buffs[1][i] = TPS_MIN(buffs[1][i], 0.5f * spacing[2]);
        buffs[2][i] = TPS_MIN(buffs[2][i], 0.5f * spacing[2]);
        buffs[3][i] = -0.5f * spacing[2];
    }

    TEST_PERFORMANCE_END("SubVolumeToSDF");
    return true;
}

bool ROIShapeConverter::SDFToSubVolume(
    const SignedDistanceField& sdf, 
    const MAT4& matP2V,
    const size_t* volDim,
    McsfGeometry::SubVolume& sv)
{
    int size[3];
    sdf.GetSize(size);
    int start[3];
    sdf.GetStart(start);
    auto sdf_data = sdf.GetData();

    MAT4 sdf2vol = sdf.GetPatToSDF().Inverse();
    sdf2vol.Prepend(matP2V);

    VEC3 vecZ = sdf2vol.Transform(VEC3(0, 0, 1));
    vecZ.Normalize();
    int axis = -1;
    if (fabs(fabs(vecZ.x) - 1) <= 1e-6)
    {
        axis = 0;
        sv.SetOffSet(start[2], start[0], start[1]);
        sv.SetSize(size[2], size[0], size[1]);
    }
    else if (fabs(fabs(vecZ.y) - 1) <= 1e-6)
    {
        axis = 1;
        sv.SetOffSet(start[1], start[2], start[0]);
        sv.SetSize(size[1], size[2], size[0]);
    }
    else if (fabs(fabs(vecZ.z) - 1) <= 1e-6)
    {
        axis = 2;
        sv.SetOffSet(start[0], start[1], start[2]);
        sv.SetSize(size[0], size[1], size[2]);
    }
    if (axis == -1)
    {
        TPS_LOG_DEV_ERROR<<"oblique case unsupported.";
        return false;
    }

    auto sv_data = sv.GetData();
    if (axis == 2)
    {
        for (int i = 0; i < size[0] * size[1] * size[2]; ++i)
        {
            sv_data[i] = sdf_data[i] > 1e-6 ? 1 : 0;
        }
    }
    else if (axis == 0)
    {
        int sdf_idx = 0;
        int sv_idx = 0;
        for (int k = 0; k < size[2]; ++k)
        {
            for (int j = 0; j < size[1]; ++j)
            {
                for (int i = 0; i < size[0]; ++i)
                {
                    sv_idx = j * size[2] * size[0] + i * size[2] + k;
                    sv_data[sv_idx] = sdf_data[sdf_idx++] > 1e-6 ? 1 : 0;
                }
            }
        }
    }
    else if (axis == 1)
    {
        int sdf_idx = 0;
        int sv_idx = 0;
        for (int k = 0; k < size[2]; ++k)
        {
            for (int j = 0; j < size[1]; ++j)
            {
                for (int i = 0; i < size[0]; ++i)
                {
                    sv_idx = i * size[1] * size[2] + k * size[1] + j;
                    sv_data[sv_idx] = sdf_data[sdf_idx++] > 1e-6 ? 1 : 0;
                }
            }
        }
    }

    //TODO:自动分割接口、剂量相关接口现在都不支持SubVolume超出体数据，
    //作为临时方案，我们暂时进行切割，以后需要让他们改掉
    sv.GetOffSet(start[0], start[1], start[2]);
    sv.GetSize(size[0], size[1], size[2]);
    int bdryBox[6] = 
    {
        start[0], start[0] + size[0] - 1, 
        start[1], start[1] + size[1] - 1, 
        start[2], start[2] + size[2] - 1
    };
    if (bdryBox[0] >= 0 && bdryBox[1] < volDim[0] && 
        bdryBox[2] >= 0 && bdryBox[3] < volDim[1] && 
        bdryBox[4] >= 0 && bdryBox[5] < volDim[2])
    {
        return true;
    }

    int new_bdry[6] = 
    {
        TPS_MAX(0, bdryBox[0]), TPS_MIN(volDim[0]-1, bdryBox[1]), 
        TPS_MAX(0, bdryBox[2]), TPS_MIN(volDim[1]-1, bdryBox[3]), 
        TPS_MAX(0, bdryBox[4]), TPS_MIN(volDim[2]-1, bdryBox[5]), 
    };
    int new_start[3] = { new_bdry[0], new_bdry[2], new_bdry[4] };
    int new_size[3] = 
    {
        new_bdry[1] - new_bdry[0] + 1, 
        new_bdry[3] - new_bdry[2] + 1, 
        new_bdry[5] - new_bdry[4] + 1, 
    };
    char* temp_buffer = new char[size[0] * size[1] * size[2]];
    memcpy(temp_buffer, sv_data, size[0] * size[1] * size[2] * sizeof(char));
    sv.SetOffSet(new_start[0], new_start[1], new_start[2]);
    sv.SetSize(new_size[0], new_size[1], new_size[2]);
    sv_data = sv.GetData();
    for (int i = new_bdry[4]; i <= new_bdry[5]; ++i)
    {
        for (int j = new_bdry[2]; j <= new_bdry[3]; ++j)
        {
            memcpy(
                sv_data + (i - new_start[2]) * new_size[0] * new_size[1] + 
                (j - new_start[1]) * new_size[0], 
                temp_buffer + (i - start[2]) * size[0] * size[1] + 
                (j - start[1]) * size[0], new_size[0] * sizeof(char));
        }
    }
    TPS_DEL_ARRAY(temp_buffer);
    return true;
}

void ROIShapeConverter::InterpolateROI(char* interFlags, SignedDistanceField& sdf, ContourSet& cs)
{
    int size[3];
    sdf.GetSize(size);
    int start[3];
    sdf.GetStart(start);
    float* sdf_buff = sdf.GetData();

    int plane_size = size[0] * size[1];
    float* start_buff = new float[plane_size];
    float* end_buff = new float[plane_size];
    float* plane_buff;

    float* d_plane_sdf;
    checkCudaErrors(cudaMalloc((void**)&d_plane_sdf, plane_size * sizeof(float)));
    int2 d_dim = make_int2(size[0], size[1]);
    float3 d_spacing = make_float3(cs.spacing[0], cs.spacing[1], cs.spacing[2]);
    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 dimGrid((size[0] + BLOCK_SIZE - 1) / BLOCK_SIZE, 
                 (size[1] + BLOCK_SIZE - 1) / BLOCK_SIZE);

    //0非插值， 1插值
    int pre = 0, curr;
    int start_slice = -1, end_slice = -1, slice;
    std::vector<contour> contours;
    PageContours* pc;
    contour* ctr;
    PT3 p;
    for (int i = 0; i < size[2]; ++i)
    {
        curr = (interFlags[i / 8] & (0x01 << (i % 8))) == 0 ? 0 : 1;
        if (pre == curr) continue;
        bool is_start_slice = pre == 0;
        pre = curr;

        if (is_start_slice)
        {
            start_slice = slice = i - 1;
            if (start_slice == end_slice)
            {
                swap(start_buff, end_buff);
                continue;
            }
            plane_buff = start_buff;
        }
        else
        {
            end_slice = slice = i;
            plane_buff = end_buff;
        }

        contours.clear();
        auto it = cs.page_contours.find(slice + start[2]);
        if (it != cs.page_contours.end())
        {
            pc = it->second;
            //设置为true，之后对边界page的sd进行更新
            pc->is_dirty = true;
            for (int j = 0; j < pc->contour_list.size(); ++j)
            {
                int vc = pc->contour_list[j].VertexCount();
                if (vc < 3)
                {
                    continue;
                }

                contours.push_back(contour());
                ctr = &contours.back();
                ctr->count = vc;
                ctr->points = new float2[vc];
                for (int k = 0; k < vc; ++k)
                {
                    p = cs.pat_to_vol.Transform(pc->contour_list[j].pt[k]);
                    ctr->points[k].x = (p.x - start[0]) * cs.spacing[0];
                    ctr->points[k].y = (p.y - start[1]) * cs.spacing[1];
                }
            }
        }

        if (contours.empty())
        {
            for (int n = 0; n < plane_size; ++n)
            {
                plane_buff[n] = -0.5f * cs.spacing[2];
            }
            continue;
        }

        float2* ptr;
        for (int n = 0; n < contours.size(); ++n)
        {
            if (contours[n].count == 0) continue;
            ptr = contours[n].points;
            checkCudaErrors(cudaMalloc((void**)&contours[n].points, contours[n].count * sizeof(float2)));
            checkCudaErrors(cudaMemcpy(contours[n].points, ptr, 
                contours[n].count * sizeof(float2), cudaMemcpyHostToDevice));
            TPS_DEL_ARRAY(ptr);
        }
        contour* d_contours;
        checkCudaErrors(cudaMalloc((void**)&d_contours, contours.size() * sizeof(contour)));
        checkCudaErrors(cudaMemcpy(d_contours, contours._Myfirst, 
            contours.size() * sizeof(contour), cudaMemcpyHostToDevice));

        Launch_CalculateSDFWithoutHat_SingleSlice(dimGrid, dimBlock, d_contours, 
            contours.size(), d_dim, d_spacing, d_plane_sdf);
        //cudaDeviceSynchronize();
        //TEST_PERFORMANCE_END("Launch_SDFLabelling");
        checkCudaErrors(cudaMemcpy(plane_buff, d_plane_sdf, 
            plane_size * sizeof(float), cudaMemcpyDeviceToHost));

        for (int n = 0; n < contours.size(); ++n)
        {
            if (contours[n].count == 0) continue;
            checkCudaErrors(cudaFree(contours[n].points));
        }
        checkCudaErrors(cudaFree(d_contours));

        //if all ready, do interpolation
        if (is_start_slice) continue;
        for (int j = start_slice + 1; j < end_slice; ++j)
        {
            float lamda = abs(float(j - start_slice) / (end_slice - start_slice + 1));
            float* data = sdf_buff + j * plane_size;
            for (int k = 0; k < plane_size; ++k)
            {
                data[k] = (1 - lamda) * start_buff[k] + lamda * end_buff[k];
            }
        }
    }
    //clear resources
    checkCudaErrors(cudaFree(d_plane_sdf));
    TPS_DEL_ARRAY(start_buff);
    TPS_DEL_ARRAY(end_buff);

    auto matSDF2Pat = sdf.GetPatToSDF().Inverse();
    PT3 org = matSDF2Pat.Transform(PT3(0, 0, 0));
    PT3 px = org + matSDF2Pat.Transform(VEC3(1, 0, 0));
    PT3 py = org + matSDF2Pat.Transform(VEC3(0, 1, 0));
    VEC3 step = matSDF2Pat.Transform(VEC3(0, 0, 1));
    for (int i = 0; i < size[2]; ++i, org += step, px += step, py += step)
    {
        if ((interFlags[i / 8] & (0x01 << (i % 8))) == 0) continue;

        int slice_no = i + start[2];
        auto it = cs.page_contours.find(slice_no);
        if (it == cs.page_contours.end())
        {
            pc = new PageContours();
            pc->is_dirty = false;
            pc->slice_number = slice_no;
            cs.page_contours[slice_no] = pc;
        }
        else pc = it->second;

        PLANE plane(org, py, px);
        SliceSDFToContours_CPU(sdf, plane, pc->contour_list);
    }
    ContourSetToSDF(cs, sdf, true);
}
TPS_END_NAMESPACE