////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_pl_autoshape_func.cpp
/// 
///  \brief implementations for MLC auto-shaping, mostly borrowed 
///         from PLUNC 
/// 
///  \version 1.0
/// 
///  \date    2014/05/12
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsAlgorithmProxy/tps_pl_autoshape_func.h"
#include "tps_logger.h"

TPS_PLUNC_BEGIN_NAMESPACE  // begin namespace tps

//static edge_type  first_edge_temp[1000];
//static edge_type  *edge_table[MY_MAP_SIZE];
//static edge_type  *aet[200];
//static int    min_y,max_y;
//static int    global_num_edges;


static void scan_anastruct(ANASTRUCT *anastruct, SCAN3D_MAP *map, float T[4][4],
    float depth, char val);
static void build_prism_face(CONTOUR_STC *contour, SCAN3D_MAP *map,  float T[4][4],
    float depth, int face, edge_type* first_edge, edge_type *edge_table[MY_MAP_SIZE], int& minY, int& maxY);
static void build_prism_edge(CONTOUR_STC *contour, SCAN3D_MAP *map, float T[4][4],
    float depth, int num, edge_type* first_edge, edge_type *edge_table[MY_MAP_SIZE], int& minY, int& maxY);
static void  project_point(float x, float y, float z, float T[4][4], float depth, PNT3D *pt);

static void init_edge_table(edge_type* first_edge, int num_edges, int ydim, edge_type *edge_table[MY_MAP_SIZE], int& minY, int& maxY);
static void insert_edge(edge_type **edge_list, edge_type *edge);
static void scan_poly(SCAN3D_MAP *map, char val, edge_type *edge_table[MY_MAP_SIZE], int minY, int maxY);
static void release_contour_edges(edge_type *edge_table[MY_MAP_SIZE], int minY, int maxY);
static void mark_exterior_pixels(SCAN3D_MAP *map, int slice);
static void remove_single_pixels(SCAN3D_MAP *map);


static void generate_contour(SCAN3D_MAP *map, float margin_x, float margin_y, 
    float margin_oar_x, float margin_oar_y, CONTOUR_STC* pContour);

static void generate_bounding_box(SCAN3D_MAP *map, float margin_x, float margin_y, 
    float margin_oar_x, float margin_oar_y, float* boundingRect);

static void dilate(SCAN3D_MAP *map, int slice, float margin_x, float margin_y);

static int  small_angle(PNT3D *list, float margin_x, float margin_y);

bool SaveToTrueColorBitmap(const std::string& path, 
    unsigned char* buffer,
    const int& height,
    const int& width)
{

#ifdef WIN32
    //todo: check whether the file is existed

    // calculate the pad size of whole image
    int nStride = (width*32+7)/8;
    int paddedsize = nStride*height;

    // declare bmp structures 
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER info;

    // andinitialize them to zero
    memset ( &bmfh, 0, sizeof (BITMAPFILEHEADER ) );
    memset ( &info, 0, sizeof (BITMAPINFOHEADER ) );

    // fill the fileheader with data
    bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paddedsize;
    bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits

    // fill the infoheader

    info.biSize = sizeof(BITMAPINFOHEADER);
    info.biWidth = width;
    info.biHeight = height;
    info.biPlanes = 1;			// we only have one bitplane
    info.biBitCount = 32;		// RGB mode is 24 bits
    info.biCompression = BI_RGB;	
    info.biSizeImage = 0;		// can be 0 for 24 bit images
    info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
    info.biYPelsPerMeter = 0x0ec4;     
    info.biClrUsed = 0;			// we are in RGB mode and have no palette
    info.biClrImportant = 0;    // all colors are important

    // now we open the file to write to
    //#ifdef _DEBUG
    //    HANDLE file = CreateFileW(sPath.c_str() , GENERIC_WRITE, FILE_SHARE_READ,
    //        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    //#else
    std::wstring wsPath(path.begin(), path.end());
    HANDLE file = CreateFile(wsPath.c_str() , GENERIC_WRITE, FILE_SHARE_READ,
        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    //#endif

    if ( file == NULL )
    {
        CloseHandle ( file );
        return false;
    }

    // write file header
    unsigned long bwritten;
    if ( WriteFile ( file, &bmfh, sizeof ( BITMAPFILEHEADER ), &bwritten, NULL ) == false )
    {	
        CloseHandle ( file );
        return false;
    }
    // write infoheader
    if ( WriteFile ( file, &info, sizeof ( BITMAPINFOHEADER ), &bwritten, NULL ) == false )
    {	
        CloseHandle ( file );
        return false;
    }
    // write image data
    if ( WriteFile ( file, buffer, paddedsize, &bwritten, NULL ) == false )
    {	
        CloseHandle ( file );
        return false;
    }

    // and close file
    CloseHandle ( file );
#else
    path;
    buffer;
    width;
    height;
#endif // _WINDOWS

    return true;
}

//for debug
void SaveToGrayBitmap(const std::string& path, 
    unsigned char* buffer,
    const int& height,
    const int& width, int color = 255)
{

    const int size = width * height;
    color = color < 0 ? 0 : color;
    color = color > 255 ? 255 : color;
    unsigned char* bmpBuffer = new unsigned char[size * 4];
    memset(bmpBuffer, 0, sizeof(unsigned char) * size * 4);
    for (int j=0; j < size; ++j){
        if(buffer[j]){
            bmpBuffer[4 * j] = color;
            bmpBuffer[4 * j + 1] = color;
            bmpBuffer[4 * j + 2] = color;
            bmpBuffer[4 * j + 3] = 255;
        }
    }

    SaveToTrueColorBitmap(path, bmpBuffer, height, width);

    delete []bmpBuffer;
}

void cat_mat(float left[4][4], float right[4][4], float result[4][4])
{   
    int r, c;       // row and column of output
    int loop;       // inner most loop of multiply routine
    float T[4][4];  // needed in case left or right is result

    for (r = 0; r < 4; r++) {       // for each row of output
        for (c = 0; c < 4; c++) {   // each col of output, only compute 4x3
            T[r][c] = 0.0;
            for (loop = 0; loop < 4; loop++) {
                T[r][c] += left[r][loop]*right[loop][c];
            }
        }
        //T[r][3] = 0.0;  // last column must end up being (0, 0, 0, 1)
    }

    //T[3][3] = 1.0;      // last entry of last column to get (0, 0, 0, 1)

    // After computation is done it is safe to copy to result
    memcpy(result, T, 16*sizeof(float));
}

/******************************************************************************/
/**
Use polygon scan conversion to generate a 2D binary map which is then
used to generate the beam contour on the border of that map.

\bug state.autoshape_anastruct must be set to the anastruct to use..why??

\param ana     Anastruct to use as contour
\param anaOAR  Anastruct to be avoided
\param margin  Margin around ana to use

*/
void autoblock(ANASTRUCT *ana, float matImage2Beam[4][4], float margin_x, float margin_y,
    ANASTRUCT *anaOAR, float matImage2BeamOAR[4][4], float margin_oar_x, float margin_oar_y, 
    float* maxRange, CONTOUR_STC* pContour)
{
    int      i, j;
    float    min_x, max_x;
    float    min_y, max_y;
    float    total_margin_x, total_margin_y;
    float    depth;
    char     val;
    PNT3D    p1, p2;

    SCAN3D_MAP  autoshape_map;
    memset(&autoshape_map, 0, sizeof(autoshape_map));
    SCAN3D_MAP    *map = &autoshape_map;
    map->x_dim = MY_MAP_SIZE;
    map->y_dim = MY_MAP_SIZE;
    map->z_dim = 3;
    map->slicecnt = map->x_dim * map->y_dim;
    map->data = (char *)malloc(map->z_dim * map->slicecnt * sizeof(char));
    memset(map->data, 0, map->z_dim * map->slicecnt * sizeof(char));

    min_x = min_y =  10000.0;
    max_x = max_y = -10000.0;
    bound_point(ana->min.x, ana->min.y, ana->min.z, matImage2Beam);
    bound_point(ana->max.x, ana->min.y, ana->min.z, matImage2Beam);
    bound_point(ana->min.x, ana->max.y, ana->min.z, matImage2Beam);
    bound_point(ana->max.x, ana->max.y, ana->min.z, matImage2Beam);
    bound_point(ana->min.x, ana->min.y, ana->max.z, matImage2Beam);
    bound_point(ana->max.x, ana->min.y, ana->max.z, matImage2Beam);
    bound_point(ana->min.x, ana->max.y, ana->max.z, matImage2Beam);
    bound_point(ana->max.x, ana->max.y, ana->max.z, matImage2Beam);

    /* Force x and y to cover the range (square pixels) */
    if (min_x < min_y) min_y = min_x;
    else min_x = min_y;
    if (max_x > max_y) max_y = max_x;
    else max_x = max_y;

    //total_margin = fabs(margin) + fabs(marginOAR);
    total_margin_x = fabs(margin_x) + fabs(margin_oar_x);
    total_margin_y = fabs(margin_y) + fabs(margin_oar_y);

    min_x -= total_margin_x; min_y -= total_margin_y;
    max_x += total_margin_x; max_y += total_margin_y;
    //get the overlap
    if (maxRange != nullptr)
    {
        min_x = std::max(min_x, maxRange[0]);
        max_x = std::min(max_x, maxRange[1]);
        min_y = std::max(min_y, maxRange[2]);
        max_y = std::min(max_y, maxRange[3]);
    }
    if (max_x - min_x <= 1e-6 || max_y - min_y <= 1e-6)
    {
        TPS_LOG_TRACE_ERROR << "No target voi has no overlap with current field region.";
        return;
    }

    /* Keep the extreme points two pixels into the map on either side */
    map->x_scale = (max_x - min_x) / (map->x_dim - 5);
    map->y_scale = (max_y - min_y) / (map->y_dim - 5);
    map->x_start = min_x - 2.0f * map->x_scale;
    map->y_start = min_y - 2.0f * map->y_scale;
    map->inv_x_scale = 1.0f / map->x_scale;
    map->inv_y_scale = 1.0f / map->y_scale;

    val = 1;
    depth = -1000.0;
    scan_anastruct(ana, map, matImage2Beam, depth, val);

    if (anaOAR != NULL)
    {
        map->data += map->slicecnt;
        map->slice_valid[1] = 1;
        scan_anastruct(anaOAR, map, matImage2BeamOAR, depth, val);
        /*
        Zero out the border pixels since the sensitive
        structure could extend into the map boundaries
        */
        for (j = 0; j < map->y_dim; j++)
        {
            map->data[j * map->x_dim] = 0;
            map->data[j * map->x_dim + map->x_dim - 1] = 0;
        }
        for (i = 0; i < map->x_dim; i++)
        {
            map->data[i] = 0;
            map->data[i + map->x_dim*(map->y_dim-1)] = 0;
        }
        map->data -= map->slicecnt;
    }

    // Finally generate the beam shape around this map
    generate_contour(map, margin_x, margin_y, margin_oar_x, margin_oar_y, pContour);

    free(map->data);
}

void autoblock_for_only_avoid(float* fieldRange, ANASTRUCT *anaOAR, 
    float matImage2BeamOAR[4][4], float margin_x, float margin_y, CONTOUR_STC* pContour) 
{
    if (fieldRange == nullptr || anaOAR == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Invalid parameters!";
        return;
    }

    int      i, j;
    float    min_x, max_x;
    float    min_y, max_y;
    float    total_margin;
    float    depth;
    char    val;

    SCAN3D_MAP  autoshape_map;
    memset(&autoshape_map, 0, sizeof(autoshape_map));
    SCAN3D_MAP    *map = &autoshape_map;
    map->x_dim = MY_MAP_SIZE;
    map->y_dim = MY_MAP_SIZE;
    map->z_dim = 3;
    map->slicecnt = map->x_dim * map->y_dim;
    map->data = (char *)malloc(map->z_dim * map->slicecnt * sizeof(char));
    memset(map->data, 0, map->z_dim * map->slicecnt * sizeof(char));

    min_x = fieldRange[0];
    max_x = fieldRange[1];
    min_y = fieldRange[2];
    max_y = fieldRange[3];
    /* Force x and y to cover the range (square pixels) */
    if (min_x < min_y) min_y = min_x;
    else min_x = min_y;
    if (max_x > max_y) max_y = max_x;
    else max_x = max_y;

    total_margin = sqrt((double)(margin_x*margin_x + margin_y*margin_y));
    min_x -= total_margin; min_y -= total_margin;
    max_x += total_margin; max_y += total_margin;
    if (max_x - min_x <= 1e-6 || max_y - min_y <= 1e-6)
    {
        TPS_LOG_TRACE_ERROR << "No target voi has no overlap with current field region.";
        return;
    }

    /* Keep the extreme points two pixels into the map on either side */
    map->x_scale = (max_x - min_x) / (map->x_dim - 5);
    map->y_scale = (max_y - min_y) / (map->y_dim - 5);
    map->x_start = min_x - 2.0f * map->x_scale;
    map->y_start = min_y - 2.0f * map->y_scale;
    map->inv_x_scale = 1.0f / map->x_scale;
    map->inv_y_scale = 1.0f / map->y_scale;

    val = 1;
    depth = -1000.0;
    //set all points in the field to 1
    for (int j = 2; j < map->y_dim - 2; ++j)
    {
        for (int i = 2; i < map->x_dim - 2; ++i)
        {
            map->data[j * map->x_dim + i] = 1;
        }
    }

    map->data += map->slicecnt;
    map->slice_valid[1] = 1;
    scan_anastruct(anaOAR, map, matImage2BeamOAR, depth, val);
    /*
    Zero out the border pixels since the sensitive
    structure could extend into the map boundaries
    */
    for (j = 0; j < map->y_dim; j++)
    {
        map->data[j * map->x_dim] = 0;
        map->data[j * map->x_dim + map->x_dim - 1] = 0;
    }
    for (i = 0; i < map->x_dim; i++)
    {
        map->data[i] = 0;
        map->data[i + map->x_dim*(map->y_dim-1)] = 0;
    }
    map->data -= map->slicecnt;

    // Finally generate the beam shape around this map
    generate_contour(map, 0.f, 0.f, margin_x,margin_y, pContour);

    free(map->data);
}

void make_projection_map(ANASTRUCT *ana, float matImage2Beam[4][4],
    float margin_x, float margin_y,
    ANASTRUCT *anaOAR, float matImage2BeamOAR[4][4],
    float margin_oar_x, float margin_oar_y,
    float* maxRange, SCAN3D_MAP* pMap)
{
    int      i, j;
    float    min_x, max_x;
    float    min_y, max_y;
    float    total_margin_x, total_margin_y;
    float    depth;
    char     val;
    PNT3D    p1, p2;

    pMap->x_dim = MY_MAP_SIZE;
    pMap->y_dim = MY_MAP_SIZE;
    pMap->z_dim = 3;
    pMap->slicecnt = pMap->x_dim * pMap->y_dim;
    pMap->data = (char *)malloc(pMap->z_dim * pMap->slicecnt * sizeof(char));
    memset(pMap->data, 0, pMap->z_dim * pMap->slicecnt * sizeof(char));

    min_x = min_y =  10000.0;
    max_x = max_y = -10000.0;
    bound_point(ana->min.x, ana->min.y, ana->min.z, matImage2Beam);
    bound_point(ana->max.x, ana->min.y, ana->min.z, matImage2Beam);
    bound_point(ana->min.x, ana->max.y, ana->min.z, matImage2Beam);
    bound_point(ana->max.x, ana->max.y, ana->min.z, matImage2Beam);
    bound_point(ana->min.x, ana->min.y, ana->max.z, matImage2Beam);
    bound_point(ana->max.x, ana->min.y, ana->max.z, matImage2Beam);
    bound_point(ana->min.x, ana->max.y, ana->max.z, matImage2Beam);
    bound_point(ana->max.x, ana->max.y, ana->max.z, matImage2Beam);

    /* Force x and y to cover the range (square pixels) */
    if (min_x < min_y) min_y = min_x;
    else min_x = min_y;
    if (max_x > max_y) max_y = max_x;
    else max_x = max_y;

    //total_margin = fabs(margin) + fabs(marginOAR);
    total_margin_x = fabs(margin_x) + fabs(margin_oar_x);
    total_margin_y = fabs(margin_y) + fabs(margin_oar_y);

    min_x -= total_margin_x; min_y -= total_margin_y;
    max_x += total_margin_x; max_y += total_margin_y;
    //get the overlap
    if (maxRange != nullptr)
    {
        min_x = std::max(min_x, maxRange[0]);
        max_x = std::min(max_x, maxRange[1]);
        min_y = std::max(min_y, maxRange[2]);
        max_y = std::min(max_y, maxRange[3]);
    }
    if (max_x - min_x <= 1e-6 || max_y - min_y <= 1e-6)
    {
        TPS_LOG_TRACE_ERROR << "No target voi has no overlap with current field region.";
        return;
    }

    /* Keep the extreme points two pixels into the map on either side */
    pMap->x_scale = (max_x - min_x) / (pMap->x_dim - 5);
    pMap->y_scale = (max_y - min_y) / (pMap->y_dim - 5);
    pMap->x_start = min_x - 2.0f * pMap->x_scale;
    pMap->y_start = min_y - 2.0f * pMap->y_scale;
    pMap->inv_x_scale = 1.0f / pMap->x_scale;
    pMap->inv_y_scale = 1.0f / pMap->y_scale;

    val = 1;
    depth = -1000.0;
    scan_anastruct(ana, pMap, matImage2Beam, depth, val);

    if (anaOAR != NULL)
    {
        pMap->data += pMap->slicecnt;
        pMap->slice_valid[1] = 1;
        scan_anastruct(anaOAR, pMap, matImage2BeamOAR, depth, val);
        /*
        Zero out the border pixels since the sensitive
        structure could extend into the map boundaries
        */
        for (j = 0; j < pMap->y_dim; j++)
        {
            pMap->data[j * pMap->x_dim] = 0;
            pMap->data[j * pMap->x_dim + pMap->x_dim - 1] = 0;
        }
        for (i = 0; i < pMap->x_dim; i++)
        {
            pMap->data[i] = 0;
            pMap->data[i + pMap->x_dim * (pMap->y_dim - 1)] = 0;
        }
        pMap->data -= pMap->slicecnt;
    }

    // slice_valid[1]为1表示存在Avoid VOI，用Target的VOI的Mask减去Avoid VOI的Mask
    if (pMap->slice_valid[1])
    {
        //mark_exterior_pixels(map, 1);
        dilate(pMap, 1, margin_oar_x, margin_oar_y);
        for (i = 0, j = pMap->slicecnt; i < pMap->slicecnt; i++, j++)
        {
            if (pMap->data[j] != 0) pMap->data[i] = 0;
        }
        remove_single_pixels(pMap);
    }

    dilate(pMap, 0, margin_x, margin_y);

    // 膨胀之后再做一次Target减去Avoid的Mask
    if (pMap->slice_valid[1])
    {
        for (i = 0, j = pMap->slicecnt; i < pMap->slicecnt; i++, j++)
        {
            if (pMap->data[j] != 0) pMap->data[i] = 0;
        }
        remove_single_pixels(pMap);
    }


    // test code
    /* int nX = pMap->x_dim;
    int nY = pMap->y_dim;
    unsigned char* buffer = (unsigned char*)pMap->data;
    unsigned char* bmpBuffer = new unsigned char[nX * nY * 4];
    for (int j = 0; j < nX * nY; ++j)
    {
    bmpBuffer[4 * j] = buffer[j] * 255;
    bmpBuffer[4 * j + 1] = buffer[j] * 255;
    bmpBuffer[4 * j + 2] = buffer[j] * 255;
    bmpBuffer[4 * j + 3] = 255;
    }
    char fileName[256];
    sprintf_s(fileName, "d:\\temp\\map_afterailate_1.bmp");
    SaveToTrueColorBitmap(fileName, bmpBuffer, nY, nX);
    delete []bmpBuffer;*/
}


void make_projection_map_for_only_avoid(float* fieldRange,
    ANASTRUCT *anaOAR,
    float matImage2BeamOAR[4][4],
    float margin_x, float margin_y,
    SCAN3D_MAP* pMap)
{
    if (fieldRange == nullptr || anaOAR == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Invalid parameters!";
        return;
    }

    int      i, j;
    float    min_x, max_x;
    float    min_y, max_y;
    float    total_margin;
    float    depth;
    char     val;

    pMap->x_dim = MY_MAP_SIZE;
    pMap->y_dim = MY_MAP_SIZE;
    pMap->z_dim = 3;
    pMap->slicecnt = pMap->x_dim * pMap->y_dim;
    pMap->data = (char *)malloc(pMap->z_dim * pMap->slicecnt * sizeof(char));
    memset(pMap->data, 0, pMap->z_dim * pMap->slicecnt * sizeof(char));

    min_x = fieldRange[0];
    max_x = fieldRange[1];
    min_y = fieldRange[2];
    max_y = fieldRange[3];

    /* Force x and y to cover the range (square pixels) */
    if (min_x < min_y) min_y = min_x;
    else min_x = min_y;
    if (max_x > max_y) max_y = max_x;
    else max_x = max_y;

    total_margin = sqrt((double)(margin_x * margin_x + margin_y * margin_y));
    min_x -= total_margin; min_y -= total_margin;
    max_x += total_margin; max_y += total_margin;
    if (max_x - min_x <= 1e-6 || max_y - min_y <= 1e-6)
    {
        TPS_LOG_TRACE_ERROR << "No target voi has no overlap with current field region.";
        return;
    }

    /* Keep the extreme points two pixels into the map on either side */
    pMap->x_scale = (max_x - min_x) / (pMap->x_dim - 5);
    pMap->y_scale = (max_y - min_y) / (pMap->y_dim - 5);
    pMap->x_start = min_x - 2.0f * pMap->x_scale;
    pMap->y_start = min_y - 2.0f * pMap->y_scale;
    pMap->inv_x_scale = 1.0f / pMap->x_scale;
    pMap->inv_y_scale = 1.0f / pMap->y_scale;

    val = 1;
    depth = -1000.0;
    //set all points in the field to 1
    for (int j = 2; j < pMap->y_dim - 2; ++j)
    {
        for (int i = 2; i < pMap->x_dim - 2; ++i)
        {
            pMap->data[j * pMap->x_dim + i] = 1;
        }
    }

    pMap->data += pMap->slicecnt;
    pMap->slice_valid[1] = 1;
    scan_anastruct(anaOAR, pMap, matImage2BeamOAR, depth, val);
    /*
    Zero out the border pixels since the sensitive
    structure could extend into the map boundaries
    */
    for (j = 0; j < pMap->y_dim; j++)
    {
        pMap->data[j * pMap->x_dim] = 0;
        pMap->data[j * pMap->x_dim + pMap->x_dim - 1] = 0;
    }
    for (i = 0; i < pMap->x_dim; i++)
    {
        pMap->data[i] = 0;
        pMap->data[i + pMap->x_dim * (pMap->y_dim - 1)] = 0;
    }
    pMap->data -= pMap->slicecnt;


    // slice_valid[1]为1表示存在Avoid VOI，用Target的VOI的Mask减去Avoid VOI的Mask
    if (pMap->slice_valid[1])
    {
        dilate(pMap, 1, margin_x, margin_y);
        for (i = 0, j = pMap->slicecnt; i < pMap->slicecnt; i++, j++)
        {
            if (pMap->data[j] != 0) pMap->data[i] = 0;
        }
        remove_single_pixels(pMap);
    }

    dilate(pMap, 0, margin_x, margin_y);

    // 膨胀之后再做一次Target减去Avoid的Mask
    if (pMap->slice_valid[1])
    {
        for (i = 0, j = pMap->slicecnt; i < pMap->slicecnt; i++, j++)
        {
            if (pMap->data[j] != 0) pMap->data[i] = 0;
        }
        remove_single_pixels(pMap);
    }

}

void get_bounding_box(ANASTRUCT *ana, float matImage2Beam[4][4], float margin_x, float margin_y,
    ANASTRUCT *anaOAR, float matImage2BeamOAR[4][4], float margin_oar_x, float margin_oar_y, 
    float* maxRange, float* boundingBox)
{
    int      i, j;
    float    min_x, max_x;
    float    min_y, max_y;
    float    total_margin_x, total_margin_y;
    float    depth;
    char    val;
    PNT3D    p1, p2;

    SCAN3D_MAP  autoshape_map;
    memset(&autoshape_map, 0, sizeof(autoshape_map));
    SCAN3D_MAP    *map = &autoshape_map;
    map->x_dim = MY_MAP_SIZE;
    map->y_dim = MY_MAP_SIZE;
    map->z_dim = 3;
    map->slicecnt = map->x_dim * map->y_dim;
    map->data = (char *)malloc(map->z_dim * map->slicecnt * sizeof(char));
    memset(map->data, 0, map->z_dim * map->slicecnt * sizeof(char));

    min_x = min_y =  10000.0;
    max_x = max_y = -10000.0;
    bound_point(ana->min.x, ana->min.y, ana->min.z, matImage2Beam);
    bound_point(ana->max.x, ana->min.y, ana->min.z, matImage2Beam);
    bound_point(ana->min.x, ana->max.y, ana->min.z, matImage2Beam);
    bound_point(ana->max.x, ana->max.y, ana->min.z, matImage2Beam);
    bound_point(ana->min.x, ana->min.y, ana->max.z, matImage2Beam);
    bound_point(ana->max.x, ana->min.y, ana->max.z, matImage2Beam);
    bound_point(ana->min.x, ana->max.y, ana->max.z, matImage2Beam);
    bound_point(ana->max.x, ana->max.y, ana->max.z, matImage2Beam);

    /* Force x and y to cover the range (square pixels) */
    if (min_x < min_y) min_y = min_x;
    else min_x = min_y;
    if (max_x > max_y) max_y = max_x;
    else max_x = max_y;

    total_margin_x = fabs(margin_x) + fabs(margin_oar_x);
    total_margin_y = fabs(margin_y) + fabs(margin_oar_y);

    min_x -= total_margin_x; min_y -= total_margin_y;
    max_x += total_margin_x; max_y += total_margin_y;
    //get the overlap
    if (maxRange != nullptr){
        min_x = std::max(min_x, maxRange[0]);
        max_x = std::min(max_x, maxRange[1]);
        min_y = std::max(min_y, maxRange[2]);
        max_y = std::min(max_y, maxRange[3]);
    }
    if(max_x - min_x <= 1e-6 || max_y - min_y <= 1e-6){
        TPS_LOG_TRACE_ERROR<<"No target voi has no overlap with current field region.";
        return;
    }

    /* Keep the extreme points two pixels into the map on either side */
    map->x_scale = (max_x - min_x) / (map->x_dim - 5);
    map->y_scale = (max_y - min_y) / (map->y_dim - 5);
    map->x_start = min_x - 2.0f * map->x_scale;
    map->y_start = min_y - 2.0f * map->y_scale;
    map->inv_x_scale = 1.0f / map->x_scale;
    map->inv_y_scale = 1.0f / map->y_scale;

    val = 1;
    depth = -1000.0;
    scan_anastruct(ana, map, matImage2Beam, depth, val);

    if (anaOAR != NULL) {
        map->data += map->slicecnt;
        map->slice_valid[1] = 1;
        scan_anastruct(anaOAR, map, matImage2BeamOAR, depth, val);
        /*
        Zero out the border pixels since the sensitive
        structure could extend into the map boundaries
        */
        for (j = 0; j < map->y_dim; j++) {
            map->data[j * map->x_dim] = 0;
            map->data[j * map->x_dim + map->x_dim - 1] = 0;
        }
        for (i = 0; i < map->x_dim; i++) {
            map->data[i] = 0;
            map->data[i + map->x_dim*(map->y_dim-1)] = 0;
        }
        map->data -= map->slicecnt;
    }

    generate_bounding_box(map, margin_x, margin_y, margin_oar_x, margin_oar_y, boundingBox);

    free(map->data);
}

void get_bounding_box_for_only_avoid(float* fieldRange, ANASTRUCT *anaOAR, 
    float matImage2BeamOAR[4][4], float margin_x, float margin_y, float* boundingBox)
{
    if (fieldRange == nullptr || anaOAR == nullptr){
        TPS_LOG_DEV_ERROR<<"Invalid parameters!";
        return;
    }

    int      i, j;
    float    min_x, max_x;
    float    min_y, max_y;
    float    total_margin;
    float    depth;
    char    val;

    SCAN3D_MAP  autoshape_map;
    memset(&autoshape_map, 0, sizeof(autoshape_map));
    SCAN3D_MAP    *map = &autoshape_map;
    map->x_dim = MY_MAP_SIZE;
    map->y_dim = MY_MAP_SIZE;
    map->z_dim = 3;
    map->slicecnt = map->x_dim * map->y_dim;
    map->data = (char *)malloc(map->z_dim * map->slicecnt * sizeof(char));
    memset(map->data, 0, map->z_dim * map->slicecnt * sizeof(char));

    min_x = fieldRange[0];
    max_x = fieldRange[1];
    min_y = fieldRange[2];
    max_y = fieldRange[3];
    /* Force x and y to cover the range (square pixels) */
    if (min_x < min_y) min_y = min_x;
    else min_x = min_y;
    if (max_x > max_y) max_y = max_x;
    else max_x = max_y;

    total_margin = sqrt((double)(margin_x*margin_x + margin_y*margin_y));
    min_x -= total_margin; min_y -= total_margin;
    max_x += total_margin; max_y += total_margin;
    if(max_x - min_x <= 1e-6 || max_y - min_y <= 1e-6){
        TPS_LOG_TRACE_ERROR<<"No target voi has no overlap with current field region.";
        return;
    }

    /* Keep the extreme points two pixels into the map on either side */
    map->x_scale = (max_x - min_x) / (map->x_dim - 5);
    map->y_scale = (max_y - min_y) / (map->y_dim - 5);
    map->x_start = min_x - 2.0f * map->x_scale;
    map->y_start = min_y - 2.0f * map->y_scale;
    map->inv_x_scale = 1.0f / map->x_scale;
    map->inv_y_scale = 1.0f / map->y_scale;

    val = 1;
    depth = -1000.0;
    //set all points in the field to 1
    for (int j = 2; j < map->y_dim - 2; ++j){
        for (int i = 2; i < map->x_dim - 2; ++i){
            map->data[j * map->x_dim + i] = 1;
        }
    }

    map->data += map->slicecnt;
    map->slice_valid[1] = 1;
    scan_anastruct(anaOAR, map, matImage2BeamOAR, depth, val);
    /*
    Zero out the border pixels since the sensitive
    structure could extend into the map boundaries
    */
    for (j = 0; j < map->y_dim; j++) {
        map->data[j * map->x_dim] = 0;
        map->data[j * map->x_dim + map->x_dim - 1] = 0;
    }
    for (i = 0; i < map->x_dim; i++) {
        map->data[i] = 0;
        map->data[i + map->x_dim*(map->y_dim-1)] = 0;
    }
    map->data -= map->slicecnt;

    // Finally generate the beam shape around this map
    generate_bounding_box(map, 0.f,0.f, margin_x, margin_y, boundingBox);

    free(map->data);
}

/******************************checked*****************************************/
void normalize_vector(PNT3D *p) {   
    float len, inv;
    len = sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
    if (len == 0.0) return;
    inv = 1.0f / len;
    p->x *= inv;
    p->y *= inv;
    p->z *= inv;
}

/**********************************checked************************************/
/**
This routine scan converts a 3D ANASTRUCT represented by
a series of contours (that represent prisms) into two 2D
pixel maps.
\param anastruct   anastruct to provide contours
\param map         array of 2d maps
\param T           coordinate system transformation matrix
\param depth       depth in cm.
\param val         value to write to those parts of the image
within the contour
*/
static void scan_anastruct(ANASTRUCT *anastruct, SCAN3D_MAP *map, float T[4][4],
    float depth, char val) 
{  
    int      i, j;
    CONTOUR_STC    *contour = anastruct->contours;

    int nEdge = 1000;
    edge_type*  pEdge = (edge_type*)malloc(nEdge * sizeof(edge_type));
    memset(pEdge, 0, nEdge * sizeof(edge_type));
    for (i = 0; i < anastruct->contour_count; i++, contour++) {
        if (contour->vertex_count <= 0) continue;
        if (contour->vertex_count > nEdge)
        {
            pEdge = (edge_type*)realloc(pEdge, contour->vertex_count * sizeof(edge_type));
            memset(pEdge + nEdge, 0, (contour->vertex_count - nEdge) * sizeof(edge_type));
            nEdge = contour->vertex_count;
        }
        /* first the prism faces at the maximum z depth */
        int minY = 0;
        int maxY = 0;
        edge_type  *edge_table[MY_MAP_SIZE];
        build_prism_face(contour, map, T, depth, 1, pEdge, edge_table, minY, maxY);
        scan_poly(map, val, edge_table, minY, maxY);
        release_contour_edges(edge_table, minY, maxY);

        /* now the prism faces at the minimum z depth */
        build_prism_face(contour, map, T, depth, 0, pEdge, edge_table, minY, maxY);
        scan_poly(map, val, edge_table, minY, maxY);
        release_contour_edges(edge_table, minY, maxY);

        /* finally the prism edges */
        for (j = 0; j < contour->vertex_count; j++)
        {
            build_prism_edge(contour, map, T, depth, j, pEdge, edge_table, minY, maxY);
            scan_poly(map, val, edge_table, minY, maxY);
            release_contour_edges(edge_table, minY, maxY);
        }
    }

    free(pEdge);
}

/******************************************************************************/
/**
Creates the edges representing either of the two flat faces
each contour prism has.
\param contour  which contour to use
\param map      image array
\param T        coordinate system transformation matrix
\param depth    depth in cm
\param face     0 for bottom, 1 for top in Z
*/
static void build_prism_face(CONTOUR_STC *contour, SCAN3D_MAP *map,  float T[4][4],
    float depth, int face, edge_type* first_edge, edge_type *edge_table[MY_MAP_SIZE], int& minY, int& maxY)
{  
    int i, j, num_edges; 
    PNT3D pt0, pt1;
    edge_type *edge;
    float z;

    num_edges = contour->vertex_count;
    if (face) z = contour->max.z;
    else z = contour->min.z;
    project_point(contour->x[0], contour->y[0], z, T, depth, &pt0);
    for (edge = first_edge, i = 0; i < num_edges; i++, edge++) {
        j = i+1;
        if (j == num_edges) j = 0;
        project_point(contour->x[j], contour->y[j], z, T, depth, &pt1);
        edge->xo = (pt0.x - map->x_start)*map->inv_x_scale; 
        edge->xf = (pt1.x - map->x_start)*map->inv_x_scale; 
        edge->yo = (pt0.y - map->y_start)*map->inv_y_scale;
        edge->yf = (pt1.y - map->y_start)*map->inv_y_scale;
        edge->zo = (pt0.z);
        edge->zf = (pt1.z);
        pt0 = pt1;
    }
    init_edge_table(first_edge, num_edges, map->y_dim, edge_table, minY, maxY);
}

/******************************************************************************/
/**
The prism also has a 4 sided polygon for each edge of the
contour (going from min to max z). This routine builds
the edges of that polygon for a given edge of the contour.

\param contour  which contour to use
\param map      image array
\param T        coordinate system transformation matrix
\param depth    depth in cm
\param num      contour vertex number
*/
static void build_prism_edge(CONTOUR_STC *contour, SCAN3D_MAP *map, float T[4][4],
    float depth, int num, edge_type* first_edge, edge_type *edge_table[MY_MAP_SIZE], int& minY, int& maxY)
{
        int i, j, num_edges;
        PNT3D temp_pt[2], pt[4];
        edge_type *edge;

        num_edges = 4;
        i = num;
        j = num+1;
        if (j == contour->vertex_count) j = 0;
        project_point(contour->x[i], contour->y[i], contour->min.z, T, depth, temp_pt);
        project_point(contour->x[j], contour->y[j], contour->min.z, T, depth, temp_pt + 1);
        pt[0] = temp_pt[0];
        pt[1] = temp_pt[1];
        project_point(contour->x[i], contour->y[i], contour->max.z, T, depth, temp_pt);
        project_point(contour->x[j], contour->y[j], contour->max.z, T, depth, temp_pt + 1);
        pt[3] = temp_pt[0];
        pt[2] = temp_pt[1];

        for (edge = first_edge, i = 0; i < 4; i++, edge++) {
            j = i+1;
            if (j == 4) j = 0;
            edge->xo = (pt[i].x - map->x_start)*map->inv_x_scale; 
            edge->xf = (pt[j].x - map->x_start)*map->inv_x_scale; 
            edge->yo = (pt[i].y - map->y_start)*map->inv_y_scale;
            edge->yf = (pt[j].y - map->y_start)*map->inv_y_scale;
            edge->zo = (pt[i].z);
            edge->zf = (pt[j].z);
        }
        init_edge_table(first_edge, num_edges, map->y_dim, edge_table, minY, maxY);
}

/**********************checked*************************************************/
/*                 | T[0][0]   01   02   03  |
[ x y z 1] *  | T[1][0]   11   12   13  |
| T[2][0]   21   22   23  |
| T[3][0]   31   32   33  |
*/

static void project_point(float x, float y, float z, float T[4][4], float depth, PNT3D *pt) {  
        float scale;
        pt->x = x * T[0][0] + y * T[1][0] + z * T[2][0] + T[3][0];
        pt->y = x * T[0][1] + y * T[1][1] + z * T[2][1] + T[3][1];
        pt->z = x * T[0][2] + y * T[1][2] + z * T[2][2] + T[3][2];
        pt->z = 1.0f / pt->z;
        scale = depth * pt->z;
        pt->x *= scale;
        pt->y *= scale;
}

/******************************************************************************/
/**
This routine initializes the edge table by:
walking along the edge list and making sure that each edge
has yo <= yf (interchanges them if not).
Then it calculates the proper delta x_step.
Finally it inserts the edge into the edge table.
\param num_edges  how many edges in table
\param xdim   X dimension
\param ydim   Y dimension
*/
static void init_edge_table(edge_type* first_edge, int num_edges, int ydim, edge_type *edge_table[MY_MAP_SIZE], int& minY, int& maxY)
{

    //if (num_edges > MY_MAP_SIZE || num_edges < 0)
    //{
    //    TPS_LOG_DEV_ERROR << "num_edges value is not valid";
    //    return;
    //}

    int i;
    float temp;
    edge_type *edge;
    minY = 10000;
    maxY = 0;
    int edgeIndex = 0;
    memset(edge_table, 0, MY_MAP_SIZE * sizeof(edge_type *));

    for (edge = first_edge, i = 0; i < num_edges; i++, edge++)
    {
        if (edge->yo > edge->yf)
        {
            temp = edge->yo;
            edge->yo = edge->yf;
            edge->yf = temp;
            temp = edge->xo; 
            edge->xo = edge->xf;
            edge->xf = temp;
            temp = edge->zo;
            edge->zo = edge->zf;
            edge->zf = temp;
        }

        /* Turn the float Y's into int Y's in a consistent manner */
        /* iyo must be >= yo,  and yf must be < yf */
        edge->iyo = (int)edge->yo;
        if ((edge->yo > 0.0) && (edge->iyo != edge->yo)) edge->iyo++;
        edge->iyf = (int)edge->yf;
        if ((edge->yf < 0.0) || (edge->iyf == edge->yf)) edge->iyf--;

        /* Check for out of bounds */
        if (edge->iyo >= ydim) continue;
        if (edge->iyo > edge->iyf) continue;

        /* Clip to window */
        if (edge->iyo < 0) edge->iyo = 0;
        if (edge->iyf >= ydim) edge->iyf = ydim - 1;

        /* Adjust the X and Z to the new integer Y */
        edge->dx = (edge->xf - edge->xo) / (edge->yf - edge->yo);
        edge->xo += (edge->iyo - edge->yo) * edge->dx;
        edge->xf += (edge->iyf - edge->yf) * edge->dx;
        edge->dz = (edge->zf - edge->zo) / (edge->yf - edge->yo);
        edge->zo += (edge->iyo - edge->yo) * edge->dz;
        edge->zf += (edge->iyf - edge->yf) * edge->dz;
        if (edgeIndex + 1 < MY_MAP_SIZE)
        {
            insert_edge(&edge_table[edge->iyo], edge);
            edgeIndex ++;
        }
        else
        {
            TPS_LOG_DEV_ERROR << "edgeIndex value is large than MY_MAP_SIZE";
        }

        if (edge->iyo < minY) minY = edge->iyo;
        if (edge->iyf > maxY) maxY = edge->iyf;
    }
}

/******************************************************************************/
/**
This routine inserts an edge into an edge table list simply
by putting it at the end of the list
\param edge_list   current list of edges
\param edge        the edge to be added to the end of the list

*/
static void insert_edge(edge_type **edge_list, edge_type *edge)
{
    edge_type *next_edge;      // ptr to walk the list with

    edge->next = NULL;         // We are going to be the end of the list
    if (*edge_list == NULL)
    {
        // if list is empty, this is it
        *edge_list = edge;
        return;
    }
    // Walk the list until the end
    for (next_edge = *edge_list; next_edge->next; next_edge = next_edge->next);

    // add new edge to the end
    next_edge->next = edge;
}

/******************************************************************************/
/**
This routine turns the contour into a filled polygon
by working from the top down.
the contour is given by the contents of the edge array
\param map the image to fill in
\param val the value to write to those parts of the image within the contour

*/
static void scan_poly(SCAN3D_MAP *map, char val, edge_type *edge_table[MY_MAP_SIZE], int minY, int maxY)
{
    int    y;
    int    i, j;
    int    xdim, ydim;
    int    aet_num;
    int    flag;
    char  *data;
    edge_type  *edge, *t;
    int    xo, xf;
    int    y_x_xdim;

    data = (char *)map->data;
    xdim = map->x_dim;
    ydim = map->y_dim;
    y_x_xdim = minY * xdim;

    int aet_count = 0;
    for (y = minY; y <= maxY; y++)
    {
        if (edge_table[y] != NULL)
        {
            for (edge = edge_table[y]; edge; edge = edge->next)
            {
                aet_count++;
            }
        }
    }
    edge_type** aet = new edge_type*[aet_count];
    memset(aet, 0, aet_count * sizeof(edge_type *));

    aet_num = 0;
    for (y = minY; y <= maxY; y++, y_x_xdim += xdim)
    {
        flag = FALSE;
        /* add_bucket */
        if (edge_table[y] != NULL)
        {
            flag = TRUE;
            for (edge = edge_table[y]; edge; edge = edge->next)
            {
                aet[aet_num++] = edge;
            }
        }
        /* clean_aet */
        for (i = 0; i < aet_num; i++)
        {
            if (aet[i]->iyf < y)
            {
                flag = TRUE;
                aet[i] = aet[--aet_num];
                i--;
            }
        }
        /* sort_aet */
        //if (TRUE) {
        for (i = 0; i < aet_num; i++)
        {
            for (j = i + 1; j < aet_num; j++)
            {
                if (aet[i]->xo > aet[j]->xo)
                {
                    t = aet[i];
                    aet[i] = aet[j];
                    aet[j] = t;
                }
                else if (aet[i]->xo == aet[j]->xo)
                {
                    if (aet[i]->dx > aet[j]->dx)
                    {
                        t = aet[i];
                        aet[i] = aet[j];
                        aet[j] = t;
                    }
                }
            }
        }
        //}

        /* fill_line */
        for (i = 0; i < aet_num - 1; i += 2)
        {
            /* Need to adjust the float X's to int X's in consistent way */
            xo = (int)aet[i]->xo;
            if ((aet[i]->xo > 0.0) && (xo != aet[i]->xo)) xo++;
            xf = (int)aet[i + 1]->xo;
            if ((aet[i + 1]->xo < 0.0) || (xf == aet[i+1]->xo)) xf--;
            if (xo < 0) xo = 0;
            if (xf >= xdim) xf = xdim - 1;

            /* Ready the aet for next scan line */
            aet[i]->xo += aet[i]->dx;
            aet[i + 1]->xo += aet[i + 1]->dx;
            /* Fill in the horizontal line from xo to xf */
            /* Index into the proper scan line of the data array */
            xo += y_x_xdim;
            xf += y_x_xdim;
            while (xo <= xf)
            {
                data[xo] = val;
                xo++;
            }
        }
    }

    delete[] aet;
}

/******************************************************************************/
/**
Walk the edge table and set all edges to NULL
*/
static void release_contour_edges(edge_type *edge_table[MY_MAP_SIZE], int minY, int maxY)
{
    for (int i = minY; i <= maxY; i++)
    {
        edge_table[i] = NULL;
    }
}


/******************************************************************************/
/**
Generate the beam shape around this map
relys on state.sensitive_anastruct having been set if needed
\param map to provide shape
\param margin margin around shape for beam to use
*/
static void generate_contour(SCAN3D_MAP *map, float margin_x, float margin_y, 
    float margin_oar_x, float margin_oar_y, CONTOUR_STC* pContour)
{
    int    i, j;

    // slice_valid[1]为1表示存在Avoid VOI，用Target的VOI的Mask减去Avoid VOI的Mask
    if (map->slice_valid[1])
    {
        //mark_exterior_pixels(map, 1);
        dilate(map, 1, margin_oar_x, margin_oar_y);
        for (i = 0, j = map->slicecnt; i < map->slicecnt; i++, j++)
        {
            if (map->data[j] != 0) map->data[i] = 0;
        }
        remove_single_pixels(map);
    }

    // 标记Mask的边缘 mask_to_contour中存在标记边缘的算法此处不用调用
    //mark_exterior_pixels(map, 0);

    dilate(map, 0, margin_x, margin_y);

    // 膨胀之后再做一次Target减去Avoid的Mask
    if (map->slice_valid[1])
    {
        for (i = 0, j = map->slicecnt; i < map->slicecnt; i++, j++)
        {
            if (map->data[j] != 0) map->data[i] = 0;
        }
        remove_single_pixels(map);
    }
    //mark_exterior_pixels(map, 0);

    plunc::mask_to_contour(map->data, 0, map->x_dim, map->y_dim, 1, pContour,
        map->x_start, map->y_start, map->x_scale, map->y_scale);

}

static void generate_bounding_box(SCAN3D_MAP *map, float margin_x, float margin_y, 
    float margin_oar_x, float margin_oar_y, float* boundingRect)
{
    mark_exterior_pixels(map, 0);
    dilate(map, 0, margin_x, margin_y);
    int boundingBox[4] = { map->x_dim - 1, 0, map->y_dim - 1, 0};
    int count = 0;
    for(int j = 0; j < map->y_dim; ++j){
        for (int i = 0; i < map->x_dim; ++i){
            if (map->data[j * map->x_dim + i]){
                count++;
                boundingBox[0] = min(boundingBox[0], i);
                boundingBox[1] = max(boundingBox[1], i);
                boundingBox[2] = min(boundingBox[2], j);
                boundingBox[3] = max(boundingBox[3], j);
            }
        }
    }

    if (!count){
        TPS_LOG_DEV_ERROR<<"There is no point in target map.";
        boundingRect[0] = 0.f;
        boundingRect[1] = 0.f;
        boundingRect[2] = 0.f;
        boundingRect[3] = 0.f;
        return;
    }

    if (map->slice_valid[1]) {
        mark_exterior_pixels(map, 1);
        dilate(map, 1, margin_oar_x, margin_oar_y);
        int avoidBB[4] = { map->x_dim - 1, 0, map->y_dim - 1, 0};
        count = 0;
        char* data = map->data + map->slicecnt;
        for(int j = 0; j < map->y_dim; ++j){
            for (int i = 0; i < map->x_dim; ++i){
                if (data[j * map->x_dim + i]){
                    count++;
                    avoidBB[0] = min(avoidBB[0], i);
                    avoidBB[1] = max(avoidBB[1], i);
                    avoidBB[2] = min(avoidBB[2], j);
                    avoidBB[3] = max(avoidBB[3], j);
                }
            }
        }
        //if there is overlap
        if (count != 0 && !(avoidBB[0] > boundingBox[1] || avoidBB[1] < boundingBox[0] || 
            avoidBB[2] > boundingBox[3] || avoidBB[3] < boundingBox[2])){
                //if all target points are inside the avoid region
                if(avoidBB[0] <= boundingBox[0] && avoidBB[1] >= boundingBox[1] && 
                    avoidBB[2] <= boundingBox[2] && avoidBB[3] >= boundingBox[3]){
                        boundingRect[0] = 0.f;
                        boundingRect[1] = 0.f;
                        boundingRect[2] = 0.f;
                        boundingRect[3] = 0.f;
                        return;
                }
                else {
                    //calculate the pixel loss if any boundary replaced, choose the one with smallest cost
                    int dim[2] = { boundingBox[1] - boundingBox[0] + 1, 
                        boundingBox[3] - boundingBox[2] + 1};
                    int pixelLoss[4] = {
                        dim[1] * (boundingBox[1] - avoidBB[0]), 
                        dim[1] * (avoidBB[1] - boundingBox[0]),
                        dim[0] * (boundingBox[3] - avoidBB[2]), 
                        dim[0] * (avoidBB[3] - boundingBox[2])};
                        int index = 0;
                        int pxLoss = pixelLoss[0];
                        for (int i = 1; i < 4; ++i){
                            if (pxLoss > pixelLoss[i]){
                                pxLoss = pixelLoss[i];
                                index = i;
                            }
                        }
                        boundingBox[(index % 2 == 0) ? (index + 1) : (index - 1)] = avoidBB[index];
                }
        }
    }

    boundingRect[0] = map->x_start + map->x_scale * boundingBox[0];
    boundingRect[1] = map->x_start + map->x_scale * boundingBox[1];
    boundingRect[2] = map->y_start + map->y_scale * boundingBox[2];
    boundingRect[3] = map->y_start + map->y_scale * boundingBox[3];
}

/******************************************************************************/
/** 

\param  map     to provide shape
\param  index   int
\param  pt      starting point
\param  offset  how many points to skip over
\param  margin  points within the margin are treated as one?
\return         number of points found
\bug            Above comments are mostly guesses
*/
//static int
//make_point_list(SCAN3D_MAP *map, int index, PNT3D *pt,
//                int offset, float margin_x, float margin_y) 
// {   
//    int    i, j;
//    int    count;
//    int    first_index;
//    int    next_index;
//    int    dir, old_dir;
//    PNT3D  *base;
//    int    done;
//
//    base = pt;
//    pt += offset;
//    count = 0;
//    map->data[index] = 3;
//    first_index = index;
//
//    done = FALSE;
//    old_dir = 0;
//    dir = -1;
//    while (!done) {
//        if (dir != old_dir) {
//            old_dir = dir;
//            pt->x = map->x_start + map->x_scale * (index % map->x_dim);
//            pt->y = map->y_start + map->y_scale * (index / map->y_dim);
//            pt++;
//            count++;
//        }
//        //next_index = find_next_point(map, index, &dir);
//        next_index = plunc::find_next_point(map->data, map->x_dim, index, &dir);
//        if (next_index == -1) {
//            pt->x = map->x_start + map->x_scale * (index % map->x_dim);
//            pt->y = map->y_start + map->y_scale * (index / map->y_dim);
//            pt++;
//            count++;
//            break;
//        }
//        if (dir != old_dir) {
//            old_dir = dir;
//            pt->x = map->x_start + map->x_scale * (index % map->x_dim);
//            pt->y = map->y_start + map->y_scale * (index / map->y_dim);
//            pt++;
//            count++;
//        }
//        index = next_index;
//    }
//    index = first_index;
//    pt->x = map->x_start + map->x_scale * (index % map->x_dim);
//    pt->y = map->y_start + map->y_scale * (index / map->y_dim);
//    pt++;
//    count++;
//
//
//    /* Remove nearly colinear points */
//    pt -= count;
//    for (i = 1; i < count - 1; i++) {
//        if (small_angle(pt + i, margin_x, margin_y)) {
//            count--;
//            for (j = i; j < count; j++) {
//                pt[j] = pt[j+1];
//            }
//            i--;
//        }
//    }
//    pt += count;
//
//    if (offset) {
//        pt->x = (base + offset)->x;
//        pt->y = (base + offset)->y;
//        pt++;
//        count++;
//        pt->x = base->x;
//        pt->y = base->y;
//        pt++;
//        count++;
//    }
//    return(count);
//}


/*
zero out pixels where at least three adjacent pixels are zero 
*/
static void remove_single_pixels(SCAN3D_MAP *map) {
    int count = 0;
    for (int i = 0; i < map->slicecnt; i++) {
        if (map->data[i] == 1) {
            count  = map->data[i-1] == 0;
            count += map->data[i+1] == 0;
            count += map->data[i-map->x_dim] == 0;
            count += map->data[i+map->x_dim] == 0;
            if (count > 2) {
                map->data[i] = 0;
                /* Now back the index up */
                i = i - 1 - map->x_dim;
                if (i < 0) i = 0;
            }
        }
    }
}

/******************************************************************************/
/** 
walk a map, expected precondition is that maps is zeros with a shape 
defined by ones. After this routine completes, pixels on the exterior
of the shape are set to 2
\param  map     to provide shape
\param  slice   which slice in Z
*/
static void mark_exterior_pixels(SCAN3D_MAP *map, int slice)
{
    char  *data = map->data + slice * map->slicecnt;
    for (int i = 0; i < map->slicecnt; i++)
    {
        if (data[i] == 1) 
        {
            if (data[i - 1] == 0 ||
                data[i + 1] == 0 ||
                data[i - map->x_dim] == 0 ||
                data[i + map->x_dim] == 0)
            {
                data[i] = 2;
            }
        }
    }
}

//dilate with margin x
static void dilate_margin_x(SCAN3D_MAP *map, int slice, float margin_x) 
{   
    int    i, j, k;
    int    ii, jj;
    int    index, index1;
    int    fill_value = 1;
    int    idiameterX = 2, idiameterY = 2;
    int    *index_list, *x_list, *y_list;
    int    num_indices;
    float  cx, cy;
    float  x, y;
    float  radius_sq;
    float  dist;
    char  *data1, *data2;

    /* margin < 0 => erode rather than dilate */
    if (margin_x == 0.0)
    {
        return;
    }
    else if (margin_x < 0.0)
    {
        margin_x = -margin_x;
        fill_value = 0;
    }
    else
    {
        fill_value = 1;
    }

    // 计算膨胀直径
    idiameterX = (int)(2.0 * (margin_x / map->x_scale));
    idiameterX = idiameterX < 2 ? 2 : idiameterX;

    radius_sq = margin_x / map->x_scale;
    radius_sq *= radius_sq;

    cx = 0.5f * idiameterX;
    cy = 1;

    /* Compute various margin related parameters for the dilator */
    int iRadiusX = idiameterX >> 1;
    int iRadiusY = 1;

    /* Determine the indices for the dialation kernel */
    index_list = (int *)malloc(idiameterX * idiameterY * sizeof(int));
    x_list = (int *)malloc(idiameterX * idiameterY * sizeof(int));
    y_list = (int *)malloc(idiameterX * idiameterY * sizeof(int));
    num_indices = 0;
    for (j = 0; j < idiameterY; j++)
    {
        index = -(iRadiusX + (j - iRadiusY) * map->x_dim);
        for (i = 0; i < idiameterX; i++, index++)
        {
            x = i - cx;
            y = j - cy;
            dist = x * x + y * y;
            if (dist <= radius_sq)
            {
                index_list[num_indices] = index;
                x_list[num_indices] = i - iRadiusX;
                y_list[num_indices] = iRadiusY - j;
                num_indices++;
            }
        }
    }

    /* Dilate the image */
    data1 = map->data + slice * map->slicecnt;
    data2 = map->data + 2 * map->slicecnt;
    memcpy(data2, data1, map->slicecnt * sizeof(char));
    index = 0;
    for (k = 0; k < map->y_dim; k++)
    {
        for (i = 0; i < map->x_dim; i++, index++)
        {
            if (data1[index] > 0)
            {
                for (j = 0; j < num_indices; j++)
                {
                    ii = i + x_list[j];
                    if (ii < 0 || ii >= map->x_dim) continue;
                    jj = k + y_list[j];
                    if (jj < 0 || jj >= map->y_dim) continue;
                    index1 = index + index_list[j];
                    if (index1 < 0 || index1 >= map->slicecnt)
                    {
                        printf("bad index_list entry: %d\n", index1);
                    }
                    data2[index1] = (char)fill_value;
                }
            }
        }
    }
    memcpy(data1, data2, map->slicecnt * sizeof(char));

    free(y_list);
    free(x_list);
    free(index_list);
}

//dilate with margin y
static void dilate_margin_y(SCAN3D_MAP *map, int slice, float margin_y) 
{
    int    i, j, k;
    int    ii, jj;
    int    index, index1;
    int    fill_value = 1; // 1表示膨胀 0表示腐蚀
    int    idiameterX = 2, idiameterY = 2;
    int    *index_list, *x_list, *y_list;
    int    num_indices;
    float  cx, cy;
    float  x, y;
    float  radius_sq;
    float  dist;
    char  *data1, *data2;

    /* margin < 0 => erode rather than dilate */
    if (margin_y == 0.0)
    {
        return;
    }
    else if (margin_y < 0.0)
    {
        margin_y = -margin_y;
        fill_value = 0;
    }
    else
    {
        fill_value = 1;
    }

    /* Compute various margin related parameters for the dilator */
    idiameterY = (int)(2.0 * (margin_y / map->y_scale));
    idiameterY = idiameterY < 2 ? 2 : idiameterY; 
    radius_sq = margin_y / map->y_scale;
    radius_sq *= radius_sq;

    cx = 1;
    cy = 0.5f * idiameterY;

    /* Compute various margin related parameters for the dilator */
    int iRadiusX = 1;
    int iRadiusY = idiameterY >> 1;

    /* Determine the indices for the dialation kernel */
    index_list = (int *)malloc(idiameterX * idiameterY * sizeof(int));
    x_list = (int *)malloc(idiameterX * idiameterY * sizeof(int));
    y_list = (int *)malloc(idiameterX * idiameterY * sizeof(int));
    num_indices = 0;
    for (j = 0; j < idiameterY; j++)
    {
        index = -(iRadiusX + (j - iRadiusY) * map->x_dim);
        for (i = 0; i < idiameterX; i++, index++)
        {
            x = i - cx;
            y = j - cy;
            dist = x * x + y * y;
            if (dist <= radius_sq)
            {
                index_list[num_indices] = index;
                x_list[num_indices] = i - iRadiusX;
                y_list[num_indices] = iRadiusY-j;
                num_indices++;
            }
        }
    }

    /* Dilate the image */
    data1 = map->data + slice * map->slicecnt;
    data2 = map->data + 2 * map->slicecnt;
    memcpy(data2, data1, map->slicecnt * sizeof(char));
    index = 0;
    for (k = 0; k < map->y_dim; k++)
    {
        for (i = 0; i < map->x_dim; i++, index++)
        {
            if (data1[index] > 0)
            {
                for (j = 0; j < num_indices; j++)
                {
                    ii = i + x_list[j];
                    if (ii < 0 || ii >= map->x_dim) continue;
                    jj = k + y_list[j];
                    if (jj < 0 || jj >= map->y_dim) continue;
                    index1 = index + index_list[j];
                    if (index1 < 0 || index1 >= map->slicecnt)
                    {
                        printf("bad index_list entry: %d\n", index1);
                    }
                    data2[index1] = (char)fill_value;
                }
            }
        }
    }
    memcpy(data1, data2, map->slicecnt * sizeof(char));

    free(y_list);
    free(x_list);
    free(index_list);
}


/******************************************************************************/
/** 
walk a map, expected precondition is that maps is zeros with a shape 
defined by ones. After this routine completes, 
the size of the shape has been increased by margin.
Note: margin can be negative, in which case the size will be reduced.
//dilate with margin x &margin y
//marginX>0 && marginY>0

\param  map     to provide shape
\param  slice   which slice in Z
\param  margin  margin around shape, can be positive or negative
\bug            Above comments are mostly guesses
*/

static void dilate(SCAN3D_MAP *map, int slice, float marginX, float marginY) 
{
    int    i, j, k;
    int    ii, jj;
    int    index, index1;
    const int    fill_value = 1;
    int    idiameterX, idiameterY/*, iradius*/;
    int    *index_list, *x_list, *y_list;
    int    num_indices;
    float  cx, cy;
    float  x, y;
    float  radius_sq;
    float  dist;
    char  *data1, *data2;

    /* margin < 0 => erode rather than dilate */
    if (marginX == 0.0 && marginY == 0.0)
    {
        return;
    }
    else if (marginX == 0.0)
    {
        dilate_margin_y(map, slice, marginY);
        return;
    } 
    else if (marginY == 0.0)
    {
        dilate_margin_x(map, slice, marginX);
        return;
    }

    /* Compute various margin related parameters for the dilator */
    idiameterX = (int)(2.0 * (marginX / map->x_scale));
    idiameterX = idiameterX < 2 ? 2 : idiameterX;
    idiameterY = (int)(2.0 * (marginY / map->y_scale));
    idiameterY = idiameterY < 2 ? 2 : idiameterY;

    radius_sq = marginX * marginY/(map->x_scale * map->y_scale);
    cx = 0.5f * idiameterX;
    cy = 0.5f * idiameterY;

    /* Compute various margin related parameters for the dilator */
    int iRadiusX = idiameterX >> 1;
    int iRadiusY = idiameterY >> 1;

    /* Determine the indices for the dialation kernel */
    index_list = (int *)malloc(idiameterX * idiameterY * sizeof(int));
    x_list = (int *)malloc(idiameterX * idiameterY * sizeof(int));
    y_list = (int *)malloc(idiameterX * idiameterY * sizeof(int));
    num_indices = 0;
    for (j = 0; j < idiameterY; j++)
    {
        index = -(iRadiusX + (j - iRadiusY) * map->x_dim);
        for (i = 0; i < idiameterX; i++, index++)
        {
            x = i - cx;
            y = j - cy;
            dist = x * x + y * y;
            if (dist <= radius_sq)
            {
                index_list[num_indices] = index;
                x_list[num_indices] = i - iRadiusX;
                y_list[num_indices] = iRadiusY-j;
                num_indices++;
            }
        }
    }

    /* Dilate the image */
    data1 = map->data + slice * map->slicecnt;
    data2 = map->data + 2 * map->slicecnt;
    memcpy(data2, data1, map->slicecnt * sizeof(char));
    index = 0;
    for (k = 0; k < map->y_dim; k++)
    {
        for (i = 0; i < map->x_dim; i++, index++)
        {
            if (data1[index] > 0)
            {
                for (j = 0; j < num_indices; j++)
                {
                    ii = i + x_list[j];
                    if (ii < 0 || ii >= map->x_dim) continue;
                    jj = k + y_list[j];
                    if (jj < 0 || jj >= map->y_dim) continue;
                    index1 = index + index_list[j];
                    if (index1 < 0 || index1 >= map->slicecnt)
                    {
                        printf("bad index_list entry: %d\n", index1);
                    }
                    data2[index1] = (char)fill_value;
                }
            }
        }
    }
    memcpy(data1, data2, map->slicecnt * sizeof(char));

    free(y_list);
    free(x_list);
    free(index_list);
}

/*
Takes a list of 3 points. returns true if all 3 points are close together.
returns false otherwise.
\param  list    list of 3 points
\param  margin  margin defines what is considered close, but see code for
exceptions.
\return         int as bool: true the the points are less than 
the margin and less than 3 mm away from each other.
*/
static int small_angle(PNT3D *list, float margin_x, float margin_y) 
{
    const int MarginAsNear = 3; // distance < 3mm is treated as near
    float  dist_sq;
    float  dot;
    PNT3D  d1, d2;

    // First if this point and the previous previous point are more 
    // than margin away, return FALSE.
    list -= 1;
    dist_sq = (list[0].x - list[1].x) * (list[0].x - list[1].x) +
        (list[0].y - list[1].y) * (list[0].y - list[1].y);
    if (dist_sq > margin_x * margin_y) return (false);

    // First if they are less than 3 mm away, return TRUE.
    if (dist_sq < MarginAsNear * MarginAsNear) return (true);

    // If the two line segments connecting this point and the previous
    // and next points are not nearly colinear, return FALSE.
    d1.x = list[1].x - list[0].x;
    d1.y = list[1].y - list[0].y;
    d1.z = 0.0;
    d2.x = list[2].x - list[1].x;
    d2.y = list[2].y - list[1].y;
    d2.z = 0.0;
    normalize_vector(&d1);
    normalize_vector(&d2);
    dot = d1.x * d2.x + d1.y * d2.y;
    if (dot < 0.9) return (false);      // arccos(0.9)=25.84 degree

    return(true);
}
TPS_PLUNC_END_NAMESPACE  // end namespace tps
