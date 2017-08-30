#include "RtTpsROIUtility/tps_ru_defs.h"
#include "cuda_runtime.h"
#include "helper_math.h"
#include "helper_cuda.h"

#include "math_constants.h"
#include "device_launch_parameters.h"
#include "device_functions.h"
#include "texture_fetch_functions.h"

#include "tps_ru_common.h"

TPS_BEGIN_NAMESPACE

__device__ float2 project(contour* ctr, float2 axis)
{
    float dot_proj = dot(axis, ctr->points[0]);
    float2 proj = make_float2(dot_proj, dot_proj);
    for (int i = 1; i < ctr->count; ++i)
    {
        dot_proj = dot(axis, ctr->points[i]);
        proj.x = min(proj.x, dot_proj);
        proj.y = max(proj.y, dot_proj);
    }
    return proj;
}

__device__ bool has_overlap(contour* ctr0, contour* ctr1)
{
    if (ctr0->count < 3 || ctr1->count < 3) return false;

    float4 bdry0 = make_float4(
        ctr0->points[0].x, ctr0->points[0].x, 
        ctr0->points[0].y, ctr0->points[0].y);
    for (int i = 1; i < ctr0->count; ++i)
    {
        bdry0.x = min(bdry0.x, ctr0->points[i].x);
        bdry0.y = max(bdry0.y, ctr0->points[i].x);
        bdry0.z = min(bdry0.z, ctr0->points[i].y);
        bdry0.w = max(bdry0.w, ctr0->points[i].y);
    }

    float4 bdry1 = make_float4(
        ctr1->points[0].x, ctr1->points[0].x, 
        ctr1->points[0].y, ctr1->points[0].y);
    for (int i = 1; i < ctr1->count; ++i)
    {
        bdry1.x = min(bdry1.x, ctr1->points[i].x);
        bdry1.y = max(bdry1.y, ctr1->points[i].x);
        bdry1.z = min(bdry1.z, ctr1->points[i].y);
        bdry1.w = max(bdry1.w, ctr1->points[i].y);
    }

    if (bdry0.x >= bdry1.y || bdry1.x >= bdry0.y || 
        bdry0.z >= bdry1.w || bdry1.z >= bdry0.w)
    {
        return false;
    }
    return true;

    //这里只做粗判断，SAT性能太差

    //using separating axis theorem to detect overlapping
    //float2 axis, proj0, proj1;
    //for (int i = 0; i < ctr0->count; ++i)
    //{
    //    axis = ctr0->points[(i+1) % ctr0->count] - ctr0->points[i];
    //    if (length(axis) <= 1e-6) continue;
    //    //project contour 0
    //    proj0 = project(ctr0, axis);
    //    //project contour 1
    //    proj1 = project(ctr1, axis);

    //    if (proj0.x >= proj1.y || proj1.x >= proj0.y)
    //    {
    //        return false;
    //    }
    //}

    //for (int i = 0; i < ctr1->count; ++i)
    //{
    //    axis = ctr1->points[(i+1) % ctr1->count] - ctr1->points[i];
    //    if (length(axis) <= 1e-6) continue;
    //    //project contour 0
    //    proj0 = project(ctr0, axis);
    //    //project contour 1
    //    proj1 = project(ctr1, axis);

    //    if (proj0.x >= proj1.y || proj1.x >= proj0.y)
    //    {
    //        return false;
    //    }
    //}
    //return true;
}

__device__ bool has_overlap(contour* pc, int count, contour* ctr)
{
    for (int i = 0; i < count; ++i)
    {
        if (has_overlap(pc+i, ctr))
        {
            return true;
        }
    }
    return false;
}

__device__ bool is_on_segment(float2 p0, float2 p1, float2 p)
{
    float2 v_seg = p1 - p0;
    float2 v = p - p0;

    float cross_proj = v.y * v_seg.x - v.x * v_seg.y;
    if (abs(cross_proj) > 1e-6) return false;

    float dot_proj = dot(v, v_seg);
    if (dot_proj < 0) return false;

    float squr_len = v_seg.x * v_seg.x + v_seg.y * v_seg.y;
    if (dot_proj > squr_len) return false;

    return true;
}

__device__ char calc_sdf_label(contour* contours, int count, float2 point)
{
    int cn = 0, vc;
    float lambda;
    float2 p0, p1;
    char sdf_label = 0;
    for (int i = 0; i < count; ++i)
    {
        vc = contours[i].count;
        if (vc < 3) continue;

        int n = 0;
        p0 = contours[i].points[vc-1];
        for (int j = 0; j < vc; ++j)
        {
            p1 = contours[i].points[j];
            if (length(p1 - p0) <= 1e-6) continue;

            if (is_on_segment(p0, p1, point))
            {
                //on the contour
                sdf_label |= (contours[i].hat_flag<<2);
                sdf_label |= 0x01;
                return sdf_label;
            }

            if ((p0.y <= point.y && p1.y > point.y) || 
                (p1.y <= point.y && p0.y > point.y))
            {
                lambda = __fdividef(__fsub_rn(point.y, p0.y), __fsub_rn(p1.y, p0.y));
                if (__fadd_rn(p0.x, __fmul_rn(lambda, __fsub_rn(p1.x, p0.x))) > point.x)
                    ++n;
            }
            p0 = p1;
        }
        //if in the contour, dump the hat flag
        if (n % 2 != 0)
        {
            sdf_label |= (contours[i].hat_flag<<2);
        }
        cn += n;
    }
    if (cn % 2 != 0) sdf_label |= 0x02;
    return sdf_label;
}

__device__ float calc_dist_to_segment(float2 start, float2 end, float2 point)
{
    float2 v0 = end - start;
    float seg_len = length(v0);
    if (seg_len <= 1e-6)
    {
        return length(point - start);
    }
    float2 norm = normalize(v0);
    float2 v1 = point - start;
    float ratio = dot(v1, norm) / seg_len;
    if (ratio <= 0)
    {
        return length(v1);
    }
    else if (ratio >= 1)
    {
        return length(point - end);
    }
    else
    {
        return length(ratio * v0 - v1);
    }
}

__device__ bool is_neighbor_all_same(int3 p, char* curr, int slice_pitch, int pitch, int3 dim)
{
    bool curr_is_out = (*curr & 0x03) == 0;
    for (int k = -NEIGHBOR_SHIFT; k <= NEIGHBOR_SHIFT; ++k)
    {
        if (p.z + k < 0 || p.z + k > dim.z - 1)
        {
            if (!curr_is_out) return false;
            continue;
        }
        for (int j = -NEIGHBOR_SHIFT; j <= NEIGHBOR_SHIFT; ++j)
        {
            if (p.y + j < 0 || p.y + j > dim.y - 1)
            {
                if (!curr_is_out) return false;
                continue;
            }
            for (int i = -NEIGHBOR_SHIFT; i <= NEIGHBOR_SHIFT; ++i)
            {
                if (p.x + i < 0 || p.x + i > dim.x - 1)
                {
                    if (!curr_is_out) return false;
                    continue;
                }
                char* ch = curr + k * slice_pitch + j * pitch + i;
                if (((*ch & 0x03) == 0) ^ curr_is_out) return false;
            }
        }
    }
    return true;
}

__device__ bool is_neighbor_dirty(contour* contours, 
    int2* pci, int page_count, int curr_page)
{
    int shift = NEIGHBOR_SHIFT + 1;
    bool any_neighbor_dirty = false;
    for (int i = -shift; i <= shift; ++i)
    {
        int idx = curr_page + i;
        if (idx < 0 || idx >= page_count || pci[idx].y == 0) continue;
        if (contours[pci[idx].x].is_dirty)
        {
            any_neighbor_dirty = true;
            break;
        }
    }
    return any_neighbor_dirty;
}

__device__ float calc_min_dist(contour* contours, int count, float2 point)
{
    int vc;
    float2 p0, p1;
    float min_dist = CUDART_MAX_NORMAL_F;
    for (int i = 0; i < count; ++i)
    {
        vc = contours[i].count;
        if (vc < 3) continue;

        p0 = contours[i].points[vc-1];
        for (int j = 0; j < vc; ++j)
        {
            p1 = contours[i].points[j];
            min_dist = min(min_dist, calc_dist_to_segment(p0, p1, point));
            p0 = p1;
        }
    }
    return min_dist;
}


__global__ void ContoursOverlappingLabelling(contour* contours, 
    int2* pci, int page_count, bool only_dirty)
{
    int ctr_count = pci[page_count-1].x + pci[page_count-1].y;
    int ctr_idx = blockIdx.x * blockDim.x * blockDim.y + 
        threadIdx.y * blockDim.x + threadIdx.x;
    if (ctr_idx >= ctr_count) return;

    int curr_page = -1;
    contour* ctr = contours + ctr_idx;
    for (int i = 0; i < page_count; ++i)
    {
        if (ctr_idx < pci[i].x + pci[i].y)
        {
            curr_page = i;
            break;
        }
    }
    if (curr_page == -1) return;

    ctr->hat_flag = 0;

    //if only dirty slices, check neighbor slice dirty flag
    if (only_dirty && !is_neighbor_dirty(contours, pci, page_count, curr_page))
    {
        return;
    }

    //pre page
    if (curr_page == 0 || 
        !has_overlap(contours + pci[curr_page-1].x, pci[curr_page-1].y, ctr))
    {
        ctr->hat_flag |= 0x01;
    }
    //post page
    if (curr_page == page_count-1 || 
        !has_overlap(contours + pci[curr_page+1].x, pci[curr_page+1].y, ctr))
    {
        ctr->hat_flag |= 0x02;
    }
}

__global__ void SDFLabelling(contour* contours, int2* pci, 
    int3 dim, float3 spacing, cudaPitchedPtr label, bool only_dirty)
{
    int3 point = make_int3(
        blockIdx.x * blockDim.x + threadIdx.x, 
        blockIdx.y * blockDim.y + threadIdx.y,
        blockIdx.z);
    if (point.x >= dim.x || point.y >= dim.y)
        return;

    if (only_dirty && !is_neighbor_dirty(contours, pci, dim.z, point.z))
    {
        return;
    }

    size_t pitch = label.pitch;
    size_t slice_pitch = pitch * dim.y;
    char* tgt = (char*)label.ptr + point.z * slice_pitch + 
        point.y * pitch + point.x;

    int start = pci[point.z].x;
    int count = pci[point.z].y;
    if (count == 0)
    {
        *tgt = 0;
        return;
    }

    float2 fpt = make_float2(point.x * spacing.x, point.y * spacing.y);
    contour* seg_start = contours + start;
    *tgt = calc_sdf_label(seg_start, count, fpt);
}

__device__ float calc_sdf(contour* contours, int2* pci, int3 dim, 
    float3 spacing, cudaPitchedPtr label, int3 point)
{
    float default_sdf = spacing.z * 0.5f;
    int count = pci[point.z].y;
    if (count == 0)
    {
        return -default_sdf;
    }

    size_t lbl_pitch = label.pitch;
    size_t lbl_slice_pitch = lbl_pitch * dim.y;
    char* curr_lbl = (char*)label.ptr + point.z * lbl_slice_pitch + 
        point.y * lbl_pitch + point.x;

    //if on contour, sdf is 0
    if ((*curr_lbl & 0x01) != 0)
    {
        return 0.f;
    }

    int sign = 0;
    bool limited_to_half_spacing = false;
    //if out of contour
    if ((*curr_lbl & 0x03) == 0)
    {
        char* down_lbl = point.z == 0 ? 0 : (curr_lbl - lbl_slice_pitch);
        char* up_lbl = point.z == dim.z - 1 ? 0 : (curr_lbl + lbl_slice_pitch);
        sign = -1;
        //if up is in down-hatted contour or down is in up-hatted contour, set limit flag
        if (((*up_lbl & 0x04) != 0) || ((*down_lbl & 0x08) != 0)) 
        {
            limited_to_half_spacing = true;
        }
    }
    //if in a contour
    else
    {
        sign = 1;
        // if current is in hatted contour, set limit flag
        if ((*curr_lbl & 0x0c) != 0)
        {
            limited_to_half_spacing = true;
        }
    }
    if (limited_to_half_spacing)
    {
        return sign * default_sdf;
    }

    //check 5*5*5 cube, if all in or all out
//     if (is_neighbor_all_same(point, curr_lbl, lbl_slice_pitch, lbl_pitch, dim))
//     {
//         return sign * length(spacing) * 0.5f;
//     }

    float2 fp = make_float2(point.x * spacing.x, point.y * spacing.y);
    contour* ctr_start = contours + pci[point.z].x;
    float min_dist = calc_min_dist(ctr_start, count, fp);
    return min_dist * sign;
}

__device__ float calc_sdf_without_hat(contour* contours, int count, float2 point)
{
    int cn = 0;
    float lambda;
    float2 p0, p1;
    float min_dist = CUDART_MAX_NORMAL_F;
    for (int i = 0; i < count; ++i)
    {
        int vc = contours[i].count;
        if (vc < 3) continue;

        p0 = contours[i].points[vc-1];
        for (int j = 0; j < vc; ++j)
        {
            p1 = contours[i].points[j];
            if (length(p1 - p0) <= 1e-6) continue;

            if ((p0.y <= point.y && p1.y > point.y) || 
                (p1.y <= point.y && p0.y > point.y))
            {
                lambda = __fdividef(__fsub_rn(point.y, p0.y), __fsub_rn(p1.y, p0.y));
                if (__fadd_rn(p0.x, __fmul_rn(lambda, __fsub_rn(p1.x, p0.x))) > point.x)
                    ++cn;
            }
            min_dist = min(min_dist, calc_dist_to_segment(p0, p1, point));
            p0 = p1;
        }
    }
    int sign = (cn % 2 == 0) ? -1: 1;
    return sign * min_dist;
}

__global__ void CalculateSDF(contour* contours, int2* pci, int3 dim, 
    float3 spacing, cudaPitchedPtr label, cudaPitchedPtr sdf)
{
    int3 p = make_int3(
        blockIdx.x * blockDim.x + threadIdx.x, 
        blockIdx.y * blockDim.y + threadIdx.y,
        blockIdx.z);
    if (p.x >= dim.x || p.y >= dim.y) return;

    size_t sdf_pitch = sdf.pitch;
    size_t sdf_slice_pitch = sdf_pitch * dim.y;
    float* tgt = (float*)((char*)sdf.ptr + p.z * sdf_slice_pitch + 
        p.y * sdf_pitch) + p.x;

    *tgt = calc_sdf(contours, pci, dim, spacing, label, p);
}

__global__ void CalculateSDF_SingleSlice(contour* contours, int2* pci, int3 dim, 
    float3 spacing, cudaPitchedPtr label, int slice_num, float* plane_sdf)
{
    int3 p = make_int3(
        blockIdx.x * blockDim.x + threadIdx.x, 
        blockIdx.y * blockDim.y + threadIdx.y,
        slice_num);
    if (p.x >= dim.x || p.y >= dim.y) return;

    plane_sdf[p.y * dim.x + p.x] = calc_sdf(contours, pci, dim, spacing, label, p);
}

__global__ void CalculateSDFWithoutHat_SingleSlice(contour* contours, 
    int count, int2 dim, float3 spacing, float* plane_sdf)
{
    int2 p = make_int2(
        blockIdx.x * blockDim.x + threadIdx.x, 
        blockIdx.y * blockDim.y + threadIdx.y);
    if (p.x >= dim.x || p.y >= dim.y) return;

    if (count == 0)
    {
        plane_sdf[p.y * dim.x + p.x] = -spacing.z / 2;
        return;
    }

    float2 fp = make_float2(p.x * spacing.x, p.y * spacing.y);
    plane_sdf[p.y * dim.x + p.x] = calc_sdf_without_hat(contours, count, fp);
}

extern "C"
void Launch_ContoursOverlappingLabelling(dim3 grid, dim3 threads, contour* contours, 
    int2* page_contours_info, int page_count, bool only_dirty)
{
    ContoursOverlappingLabelling<<<grid, threads>>>(contours, page_contours_info, page_count, only_dirty);
    getLastCudaError("ContoursOverlappingLabelling failed");
}

extern "C"
void Launch_SDFLabelling(dim3 grid, dim3 threads, contour* contours, 
    int2* pci, int3 dim, float3 spacing, cudaPitchedPtr label, bool only_dirty)
{
    SDFLabelling<<<grid, threads>>>(contours, pci, dim, spacing, label, only_dirty);
    getLastCudaError("SDFLabelling failed");
}

extern "C"
void Launch_CalculateSDF(dim3 grid, dim3 threads, contour* contours, 
    int2* pci, int3 dim, float3 spacing, cudaPitchedPtr label, cudaPitchedPtr sdf)
{
    CalculateSDF<<<grid, threads>>>(contours, pci, dim, spacing, label, sdf);
    getLastCudaError("CalculateSDF failed");
}

extern "C"
void Launch_CalculateSDF_SingleSlice(dim3 grid, dim3 threads, contour* contours, 
    int2* pci, int3 dim, float3 spacing, cudaPitchedPtr label, int slice_num, float* plane_sdf)
{
    CalculateSDF_SingleSlice<<<grid, threads>>>(
        contours, pci, dim, spacing, label, slice_num, plane_sdf);
    getLastCudaError("Launch_CalculateSDF_SinglePlane failed");
}

extern "C"
void Launch_CalculateSDFWithoutHat_SingleSlice(dim3 grid, dim3 threads, contour* contours, 
    int count, int2 dim, float3 spacing, float* plane_sdf)
{
    CalculateSDFWithoutHat_SingleSlice<<<grid, threads>>>(
        contours, count, dim, spacing, plane_sdf);
    getLastCudaError("CalculateSDFWithoutHat_SingleSlice failed");
}

__global__ void MarchingSquareLabelling(cudaTextureObject_t texObj, 
    double3 start, double3 vx, double3 vy, float iso_level, MS_MAP ms_map)
{
    int2 square_idx;
    square_idx.x = blockIdx.x * blockDim.x + threadIdx.x;
    square_idx.y = blockIdx.y * blockDim.y + threadIdx.y;
    if(square_idx.x >= ms_map.map_size.x || 
       square_idx.y >= ms_map.map_size.y)
    {
        return;
    }

    double3 p0 = start + vx * square_idx.x + vy * square_idx.y;
    double3 corners[4] = { p0, p0 + vx, p0 + vx + vy, p0 + vy };
    float sd_square[4];
    for(int i = 0; i < 4; ++i)
    {
        //use double here to avoid float precision loss problem 
        sd_square[i] = tex3D<float>(
            texObj, corners[i].x, corners[i].y, corners[i].z) - iso_level;
    }

    char id = 0;
    for(int i = 0; i < 4; ++i)
    {
        if(sd_square[i] >= -1e-6) id |= (0x01<<i);
    }
    char* label = ms_map.label_map + 
        square_idx.y * ms_map.l_pitch + square_idx.x;
    *label = char(id << 0x04);
    if(id == 0 || id == 15 || id == 4 || id == 11) return;

    float3* r_v = (float3*)((char*)ms_map.row_vertex_map + 
        square_idx.y * ms_map.v_pitch) + square_idx.x;
    float3* c_v = (float3*)((char*)ms_map.column_vertex_map + 
        square_idx.y * ms_map.v_pitch) + square_idx.x;
    if(abs(sd_square[0]) <= 1e-6)
    {
        *r_v = make_float3(p0);
        *c_v = make_float3(p0);
        *label |= 0x03;
    }
    else 
    {
        if(sd_square[0] * sd_square[1] <= 1e-6)
        {
            *r_v = make_float3(p0 - sd_square[0] / (sd_square[1] - sd_square[0]) * vx);
            *label |= 0x01;
        }
        if(sd_square[0] * sd_square[3] <= 1e-6)
        {
            *c_v = make_float3(p0 - sd_square[0] / (sd_square[3] - sd_square[0]) * vy);
            *label |= 0x02;
        }
    }

    //ambiguous case
    if (id == 5 || id == 10)
    {
        float3 center = make_float3(p0 + 0.5 * vx + 0.5 * vy);
        float central_sd = tex3D<float>(texObj, center.x, center.y, center.z);
        if (central_sd >= 0)
        {
            *label |= 0x04;
        }
    }
}

extern "C"
void Launch_MarchingSquareLabelling(dim3 grid, dim3 threads, cudaTextureObject_t texObj, 
    double3 start, double3 vx, double3 vy, float iso_level, MS_MAP ms_map)
{
    MarchingSquareLabelling<<<grid, threads>>>(texObj, start, vx, vy, iso_level, ms_map);
    getLastCudaError("Launch_MarchingSquareLabelling failed!");
}

TPS_END_NAMESPACE