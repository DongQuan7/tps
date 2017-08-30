////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Xiaoqing.Shangguan Xiaoqing.Shangguan@united-imaging.com
/// 
///  \file    tps_pl_utility.h
/// 
///  \brief 
/// 
///  \version 1.0
/// 
///  \date    2014/06/06
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

//ZHENGHE
#include "McsfAlgorithms/McsfAppLib/McsfAlgoAutoContour.h"
#include "McsfGeometry/mcsf_geometry_plane.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

//TPS
#include "RtTpsAlgorithmProxy/tps_pl_utility.h"
#include "RtTpsFramework/tps_fw_common_datastructure.h"
#include "tps_logger.h"
#include "RtTpsFramework/tps_fw_mask.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_contour.h"

// std
#include <stack>
#include "tps_performance.h"
#define TAUBIN_PASSES 20


TPS_PLUNC_BEGIN_NAMESPACE


struct cord_stc
{
    int     pt_num_start;
    int     pt_num_end;
    float   xstart;
    float   ystart;
    float   xend;
    float   yend;
} *stack, *stack_ptr;

void push(int index1, int index2, float x1, float y1, float x2, float y2)
{
    stack_ptr->pt_num_start = index1;
    stack_ptr->pt_num_end = index2;
    stack_ptr->xstart = x1;
    stack_ptr->ystart = y1;
    stack_ptr->xend = x2;
    stack_ptr->yend = y2;
    stack_ptr++;
}

void pop(struct cord_stc *next_cord)
{
    *next_cord = *--stack_ptr;
}

/*
distance routine is stolen from Julian Rosenman's BASIC implementation
of this algorithm.  The distance is defined as the perpendicular distance
between the cord and the point if the perpendicular intersects the cord
between its endpoints.  Otherwise the distance is the distance between the
point and the nearest endpoint of the cord.  In short, it is the shortest
distance between the cord and the point in a very literal sense.
*/
float dist(float x, float y, struct cord_stc *this_cord)
{
    float c1, c2, c3, c4;
    float determinant, x_perp, y_perp, p, distance;

    c1 = this_cord->ystart - this_cord->yend;
    c2 = this_cord->xend - this_cord->xstart;
    c3 = this_cord->xend * this_cord->ystart - this_cord->xstart * this_cord->yend;
    c4 = x * c2 - y * c1;

    determinant = (float)(-c1*c1 - c2*c2);

    if (determinant != 0)
    {
        x_perp = ((float)(-c1 * c3 - c2 * c4)) / determinant;
        y_perp = ((float)(c1 * c4 - c3 * c2)) / determinant;
    }
    else
    {
        return(0.0);
    }

    if (c2) p = (x_perp - this_cord->xstart) / (float)c2;
    else p = (y_perp - this_cord->ystart) / (float)-c1;

    if (p < 0.0)
        distance = (x - this_cord->xstart) * (x - this_cord->xstart) +
        (y - this_cord->ystart) * (y - this_cord->ystart);
    else if (p > 1.0)
        distance = (x - this_cord->xend) * (x - this_cord->xend) +
        (y - this_cord->yend) * (y- this_cord->yend);
    else
        distance = (x - x_perp)*(x - x_perp) + (y - y_perp)*(y - y_perp);
    return(distance);
}

/*
Here's the guts.  Pull the next cord off the stack and find the point
which lies between the endpoints of the cord which is furthest away
(perpendicularly speaking).  If the distance to the furthest point
is greater than or equal to the criterion distance, create two new
cords and push them onto the stack.  If not, add the first endpoint of
the current cord to the list of contour points which have passed muster.
*/
void cord_test(float criterion,
    float *xptr,
    float *yptr,
    int   count,
    float *new_xptr,
    float *new_yptr,
    int   *new_count)
{
    struct cord_stc next_cord;
    int loop;
    int new_point;
    float max_dist;
    float perp;

    pop(&next_cord);

    max_dist = 0.0;
    new_point = -1;
    for (loop = (next_cord.pt_num_start + 1)%count; loop != next_cord.pt_num_end; loop = (loop + 1) % count)
    {
        perp = dist(xptr[loop], yptr[loop], &next_cord);
        if (perp > max_dist)
        {
            new_point = loop;
            max_dist = perp;
        }
    }

    if (new_point == -1 || max_dist < criterion)
    {
        new_xptr[*new_count] = next_cord.xstart;
        new_yptr[(*new_count)++] = next_cord.ystart;
    }
    else
    {
        push(new_point, next_cord.pt_num_end, xptr[new_point],
            yptr[new_point], next_cord.xend, next_cord.yend);
        push(next_cord.pt_num_start, new_point, next_cord.xstart,
            next_cord.ystart, xptr[new_point], yptr[new_point]);
    }
}

void reduce(float criterion, float *xptr, float *yptr, int count,
    float *new_xptr, float *new_yptr, int *new_count)
{
    int middle;

    stack = new cord_stc[count];
    stack_ptr = stack;
    //We have made the arbitrary decision to choose the first and
    //middle points to define the starting cords of the polygon.
    //I'm sure there are other equally valid choices.

    middle = count/2;
    push(middle, 0, xptr[middle], yptr[middle], xptr[0], yptr[0]);
    push(0, middle, xptr[0], yptr[0], xptr[middle], yptr[middle]);

    criterion *= criterion;
    *new_count = 0;

    while (stack_ptr != stack) {
        cord_test(criterion, xptr, yptr, count, new_xptr, new_yptr, new_count);
    }

    delete[] stack; 
    stack = nullptr;
}

#define NORTH (1)
#define SOUTH (2)
#define EAST (3)
#define WEST (4)

void find_next_ccw(
    char   *pInputMask,
    int    xDim,
    int    yDim,
    int    *index,
    int    *pi,
    int    *pj,
    int    *dir)
{
    xDim;
    yDim;
    int i;
    int *data;

    data = (int *)pInputMask;
    switch (*dir)
    {
    case WEST:
        /* check north */
        i = (*index)-1-xDim;
        if (data[i] & 3)
        {
            *dir = NORTH;
            *index += -1-xDim;
            *pi -= 1;
            *pj -= 1;
            return;
        }
        /* check west */
        i = (*index)-1;
        if (data[i] & 3)
        {
            *index += -1;
            *pi -= 1;
            return;
        }
        /* must turn south */
        *dir = SOUTH;
        return;
    case SOUTH:
        /* check west */
        i = (*index)-1+xDim;
        if (data[i] & 3)
        {
            *dir = WEST;
            *index += -1+xDim;
            *pi -= 1;
            *pj += 1;
            return;
        }
        /* check south */
        i = (*index)+xDim;
        if (data[i] & 3)
        {
            *index += xDim;
            *pj += 1;
            return;
        }
        /* must turn east */
        *dir = EAST;
        return;
    case EAST:
        /* check south */
        i = (*index)+1+xDim;
        if (data[i] & 3)
        {
            *index += 1+xDim;
            *pi += 1;
            *pj += 1;
            *dir = SOUTH;
            return;
        }
        /* check east */
        i = (*index)+1;
        if (data[i] & 3)
        {
            *index += 1;
            *pi += 1;
            return;
        }
        /* must turn north */
        *dir = NORTH;
        return;
    case NORTH:
        /* check east */
        i = (*index)+1-xDim;
        if (data[i] & 3)
        {
            *index += 1-xDim;
            *pi += 1;
            *pj -= 1;
            *dir = EAST;
            return;
        }
        /* check north */
        i = (*index)-xDim;
        if (data[i] & 3)
        {
            *index += -xDim;
            *pj -= 1;
            return;
        }
        /* must turn west */
        *dir = WEST;
        return;
    }
}

void find_next_clockwise(
    char *pInputMask, 
    int xDim,
    int yDim,
    int *index,
    int *pi,
    int *pj,
    int *dir)
{

    int    i;
    char   *data;

    data = pInputMask;
    char v = 3;
    switch (*dir)
    {
    case WEST:
        /* check north */
        i = (*index) - 1 - xDim;        //add these code because of no expanding of the volume
        if (((*index) - 1 - xDim) < 0)
        { //added
            v = 0;                  //added
            i = 0;                  //added
        }                           //added
        //if (data[i] & 3) {
        if( v & data[i])
        {
            *dir = NORTH;
            *index += -1 - xDim;
            *pi -= 1;
            *pj -= 1;
            return;
        }
        /* check west */
        i = (*index)-1;
        if ((*index) % xDim == 0)
        {
            //added
            v = 0;                 //added
            i = 0;                 //added
        }                          //added
        if (v & data[i])
        {
            *index += -1;
            *pi -= 1;
            return;
        }
        /* must turn south */
        *dir = SOUTH;
        return;
    case SOUTH:
        /* check west */
        i = (*index)-1 + xDim;
        if (((*index)-1 + xDim) >= xDim * yDim)
        { //added
            v = 0;                  //added
            i = 0;                  //added
        }                           //added
        if (v & data[i])
        {
            *dir = WEST;
            *index += -1 + xDim;
            *pi -= 1;
            *pj += 1;
            return;
        }
        /* check south */
        i = (*index) + xDim;
        if (((*index) + xDim) >= xDim * yDim)
        {
            //added
            v = 0;                  //added
            i = 0;                  //added
        }                           //added
        if (v & data[i])
        {
            *index += xDim;
            *pj += 1;
            return;
        }
        /* must turn east */
        *dir = EAST;
        return;
    case EAST:
        /* check south */
        i = (*index)+ 1 + xDim;
        if (((*index) + xDim + 1) >= xDim * yDim)
        { //added
            v = 0;                  //added
            i = 0;                  //added
        }                           //added
        if (v & data[i])
        {
            *index += 1 + xDim;
            *pi += 1;
            *pj += 1;
            *dir = SOUTH;
            return;
        }
        /* check east */
        i = (*index) + 1;
        if ((*index + 1) % xDim == 0)
        { //added
            v = 0;                 //added
            i = 0;                 //added
        }                          //added
        if (v & data[i])
        {
            *index += 1;
            *pi += 1;
            return;
        }
        /* must turn north */
        *dir = NORTH;
        return;
    case NORTH:
        /* check east */
        i = (*index) + 1 - xDim;
        if (((*index) + 1 - xDim) < 0)
        { //added
            v = 0;                  //added
            i = 0;                  //added
        }                           //added
        if (v & data[i])
        {
            *index += 1 - xDim;
            *pi += 1;
            *pj -= 1;
            *dir = EAST;
            return;
        }
        /* check north */
        i = (*index) - xDim;
        if (((*index) - xDim) < 0)
        {   //added
            v = 0;                  //added
            i = 0;                  //added
        }                           //added
        if (v & data[i])
        {
            *index += -xDim;
            *pj -= 1;
            return;
        }
        /* must turn west */
        *dir = WEST;
        return;
    }
}

int exterior_pixel(
    char	*data,
    int		index,
    int		xdim)
{
    if (data[index] != 1) return(0);
    if (data[index-xdim] == 0) return(1);
    if (data[index+xdim] == 0) return(2);
    return(0);
}

int find_next_point(char* pMask, int xDim, int index, int *dir)
{
    int i = index - 1;
    if (pMask[i] == 2)
    {
        pMask[i] = 3;
        *dir = 1;
        return(i);
    }

    i = index + 1;
    if (pMask[i] == 2)
    {
        pMask[i] = 3;
        *dir = 2;
        return(i);
    }

    i = index - xDim;
    if (pMask[i] == 2)
    {
        pMask[i] = 3;
        *dir = 3;
        return(i);
    }

    i = index + xDim;
    if (pMask[i] == 2)
    {
        pMask[i] = 3;
        *dir = 4;
        return(i);
    }

    i = index - 1 - xDim;
    if (pMask[i] == 2)
    {
        pMask[i] = 3;
        *dir = 5;
        return(i);
    }

    i = index - 1 + xDim;
    if (pMask[i] == 2) {
        pMask[i] = 3;
        *dir = 6;
        return(i);
    }

    i = index + 1 - xDim;
    if (pMask[i] == 2)
    {
        pMask[i] = 3;
        *dir = 7;
        return(i);
    }

    i = index + 1 + xDim;
    if (pMask[i] == 2)
    {
        pMask[i] = 3;
        *dir = 8;
        return(i);
    }
    return(-1);
}

//int make_point_list(char* pMask, int xDim, int yDim,     // input mask
//    int index,                           // point to be added
//    PNT3D *pOutline, int nOutline, int value)       // target list
//{
//    yDim;
//    int    count = 0;
//    int    first_index = index, next_index;
//    int    dir = -1, old_dir = 0;
//    PNT3D* pNewPoint = pOutline + nOutline;
//
//    pMask[index] = value;//default 3
//    while (1) {
//        if (dir != old_dir) {
//            old_dir = dir;
//            pNewPoint->x = index % xDim;
//            pNewPoint->y = index / xDim;
//            pNewPoint++;
//            count++;
//        }
//        next_index = find_next_point(pMask, xDim, index, &dir);
//        if (next_index == -1) {
//            pNewPoint->x = index % xDim;
//            pNewPoint->y = index / xDim;
//            pNewPoint++;
//            count++;
//            break;
//        }
//        if (dir != old_dir) {
//            old_dir = dir;
//            pNewPoint->x = index % xDim;
//            pNewPoint->y = index / xDim;
//            pNewPoint++;
//            count++;
//        }
//        index = next_index;
//    }
//    index = first_index;
//    pNewPoint->x = index % xDim;
//    pNewPoint->y = index / xDim;
//    pNewPoint++;
//    count++;
//
//    /* Remove nearly colinear points */
//    //pt -= count;
//    //for (i = 1; i < count - 1; i++) {
//    //    if (small_angle1(pt + i, margin)) {
//    //        count--;
//    //        for (j = i; j < count; j++) {
//    //            pt[j] = pt[j+1];
//    //        }
//    //        i--;
//    //    }
//    //}
//    //pt += count;
//
//    if (nOutline) {
//        pNewPoint->x = pOutline[nOutline].x;
//        pNewPoint->y = pOutline[nOutline].y;
//        pNewPoint++;
//        count++;
//        pNewPoint->x = pOutline->x;
//        pNewPoint->y = pOutline->y;
//        pNewPoint++;
//        count++;
//    }
//    return(count);
//}


//void mask_to_contour_dascarded(const char* pInputMask,
//    int xDim,
//    int yDim,
//    CONTOUR_STC* pContour)
//{
//    int i, j;
//    int index = 0;
//    // temp storage for contour
//    int    nOutline = 0;
//    const int imageSize = xDim * yDim * sizeof(PNT3D);
//    PNT3D* pOutline = new PNT3D[imageSize];
//    memset(pOutline, 0, imageSize * sizeof(PNT3D));
//
//    //edge detecting
//    unsigned int uiWidth = xDim;
//    unsigned int uiHeight = yDim;
//    unsigned int iSizeEdge = (uiWidth + 2) * (uiHeight + 2);
//    unsigned char *data = new unsigned char[iSizeEdge];
//    memset(data, 0, iSizeEdge * sizeof(unsigned char));
//    for(unsigned int j = 0; j < uiHeight; ++j){
//        memcpy(data + (j + 1 ) * (uiWidth + 2) + 1, pInputMask + j * uiWidth, uiWidth);
//    }
//
//#define test_four_point(j)         \
//    (data[j - 1] == 0 ||           \
//    data[j + 1] == 0 ||            \
//    data[j - uiWidth - 2] == 0 ||  \
//    data[j + uiWidth + 2] == 0)
//
//    //edge should not included in the loop
//    for (unsigned int i = uiWidth + 3; i < iSizeEdge - uiWidth -3; ++i) {
//        if (data[i] == 1) {
//            if (test_four_point(i)) {
//                data[i] = 2;
//            }
//        }
//    }
//
//    index = 0;
//    nOutline = 0;
//    for (j = 1; j < yDim-1; ++j) { //yDim
//        for (i = 1; i < xDim-1; ++i, ++index) {
//            if (data[index] == 2) {
//                data[index] = 0;
//                int num = make_point_list((char*)data, xDim + 2, yDim + 2, index, pOutline, nOutline, 3);
//                if (num > 5) nOutline += num;
//            }
//        }
//    }
//
//    delete[] data; data = nullptr;
//    pContour->x = nullptr;
//    pContour->y = nullptr;
//
//    pContour->vertex_count = nOutline;
//    if (nOutline > 0) {
//        pContour->max.x = -1000.0f;
//        pContour->max.y = -1000.0f;
//        pContour->max.z = -1000.0f;
//        pContour->min.x = 1000.0f;
//        pContour->min.y = 1000.0f;
//        pContour->min.z = 1000.0f;
//
//        //!should be released later!
//        pContour->x = new float[nOutline];
//        pContour->y = new float[nOutline];
//        for (i = 0; i < nOutline; i++){
//            pContour->x[i] = pOutline[i].x;
//            pContour->y[i] = pOutline[i].y;
//
//            if (pContour->max.x < pOutline[i].x) pContour->max.x = pOutline[i].x;
//            if (pContour->max.y < pOutline[i].y) pContour->max.y = pOutline[i].y;
//            if (pContour->min.x > pOutline[i].x) pContour->min.x = pOutline[i].x;
//            if (pContour->min.y > pOutline[i].y) pContour->min.y = pOutline[i].y;
//        }
//    }
//    delete[] pOutline; pOutline = nullptr;
//}



void mask_to_contour(
    char* pInputMask,
    int zCoord,
    int xDim,
    int yDim,
    int pixValue,
    CONTOUR_STC* pContour,
    float xStart,
    float yStart,
    float xScale,
    float yScale)
{
    int i;
    int index = 0;

    // temp storage for contour
    int iCurrVertexCount = 0;
    const int imageSize = xDim * yDim * sizeof(plunc::PNT3D);
    plunc::PNT3D* pOutline = new plunc::PNT3D[imageSize];
    memset(pOutline, 0, imageSize);
    int	iTmpVertexCount = 0;
    plunc::PNT3D* pTmpOutline = new plunc::PNT3D[imageSize];
    memset(pTmpOutline, 0, imageSize);

    //edge detecting
    int uiWidth = xDim;
    int uiHeight = yDim;
    const int iSize = sizeof(char) * uiWidth * uiHeight;
    char *data = new char[iSize];
    memset(data, 0, iSize);

    //edge should not included in the loop
    memset(pInputMask, 0, xDim);//top
    memset(pInputMask + (yDim - 1) * xDim, 0, xDim);//bottom
    for (i = 0; i < iSize; ++i)
    {
        if ((0 == i % xDim)//left
            || ((xDim - 1) == i % xDim))//right
        {
            pInputMask[i] = 0;
        }
    }

    for (i = xDim + 1; i < iSize - xDim - 1; ++i)
    {
        if (pInputMask[i] == pixValue)
        {   //edge detecting
            //include dilate for make_contour_from_map
            if (pInputMask[i - 1] != pixValue)
            {
                data[i - 1] = 1;
            }
            if (pInputMask[i + 1] != pixValue)
            {
                data[i + 1] = 1;
            }
            if (pInputMask[i - uiWidth] != pixValue)
            {
                data[i - uiWidth] = 1;
            }
            if (pInputMask[i + uiWidth] != pixValue)
            {
                data[i + uiWidth] = 1;
            }
        }
    }

    // test code
    /* int nX = xDim;
    int nY = yDim;
    unsigned char* buffer = (unsigned char*)data;
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

    for (int j = 0; j < yDim; ++j)
    {
        for (int i = 0; i < xDim; ++i)
        {
            index = j * xDim + i;
            if (data[index] == 1)
            {
                //get the max length contour
                iTmpVertexCount = make_contour_from_map(data, xDim, yDim, i, j, index,
                    0, pTmpOutline, xStart, yStart, xScale, yScale);
                if (iTmpVertexCount >=  xDim * yDim )
                {
                    delete []data;
                    data = nullptr;
                    delete [] pTmpOutline;
                    pTmpOutline = nullptr;

                    delete[] pOutline;
                    pOutline = nullptr;
                    TPS_LOG_DEV_ERROR<<"too many points:"<<iTmpVertexCount;
                    TPS_PRINTF("mask_to_contour too many points:%d\n",iTmpVertexCount);
                    return;
                }
                if (iTmpVertexCount > iCurrVertexCount)
                {
                    iCurrVertexCount = iTmpVertexCount;
                    swap(pTmpOutline, pOutline);
                }
            }
        }
    }
    delete []data;
    data = nullptr;
    delete [] pTmpOutline;
    pTmpOutline = nullptr;

    pContour->x = nullptr;
    pContour->y = nullptr;
    pContour->vertex_count = iCurrVertexCount;
    if (iCurrVertexCount > 0)
    {
        pContour->max.x = -1000.0f;
        pContour->max.y = -1000.0f;
        pContour->max.z = -1000.0f;
        pContour->min.x = 1000.0f;
        pContour->min.y = 1000.0f;
        pContour->min.z = 1000.0f;

        pContour->slice_number = zCoord;

        //!should be released later!
        pContour->x = (float*)malloc(iCurrVertexCount * sizeof(float));
        pContour->y = (float*)malloc(iCurrVertexCount * sizeof(float));
        for (i = 0; i < iCurrVertexCount; i++)
        {
            pContour->x[i] = pOutline[i].x;
            pContour->y[i] = pOutline[i].y;
            if (pContour->max.x < pOutline[i].x) pContour->max.x = pOutline[i].x;
            if (pContour->max.y < pOutline[i].y) pContour->max.y = pOutline[i].y;
            if (pContour->min.x > pOutline[i].x) pContour->min.x = pOutline[i].x;
            if (pContour->min.y > pOutline[i].y) pContour->min.y = pOutline[i].y;
        }
    }
    delete[] pOutline;
    pOutline = nullptr;
}

void reduce_con(float criterion, CONTOUR_STC *con)
{
    int count;
    float *xptr;
    float *yptr;
    int new_count;
    float *new_xptr;
    float *new_yptr;

    xptr = con->x;
    yptr = con->y;
    count = con->vertex_count;
    if (count < 4)
    {
        TPS_LOG_DEV_ERROR<<"vertex_count less than 4";
        return;
    }

    new_xptr = new float[count];
    new_yptr = new float[count];

    reduce(criterion, xptr, yptr, count, new_xptr, new_yptr, &new_count);

    if (new_count > count){
        delete[] new_xptr; new_xptr = nullptr;
        delete[] new_yptr; new_yptr = nullptr;

        TPS_LOG_DEV_ERROR<<"The new count after reducing is larger than old num.";
        return;
    }

    memcpy(con->x, new_xptr, new_count * sizeof(float));
    memcpy(con->y, new_yptr, new_count * sizeof(float));

    delete[] new_xptr; new_xptr = nullptr;
    delete[] new_yptr; new_yptr = nullptr;
    con->vertex_count = new_count;
}

#define push_stack(i, j)    \
{                           \
    iStack[stackIndex] = i; \
    jStack[stackIndex] = j; \
    stackIndex++;           \
}

#define pop_stack(i, j)         \
{                               \
    i = iStack[stackIndex - 1]; \
    j = jStack[stackIndex - 1]; \
    stackIndex--;               \
}

#define push_stack_if_1(w, v)                           \
    if (w >= 0 && w < xDim && v >= 0 && v < yDim)       \
{                                                       \
    if (pOutputImg[w + v * xDim] == 1)                  \
    push_stack(w, v);                                   \
}

#define push_stack_if_0(w, v)                            \
    if (w >= 0 && w < xDim && v >=0 && v < yDim)         \
{                                                        \
    if (pOutputImg[w + v * xDim] == 0)                   \
    push_stack(w, v);                                    \
}

int SeedFill(const char* pInputImg, int xDim, int yDim, char* pOutputImg)
{
    if (nullptr == pInputImg || nullptr == pOutputImg)
    {
        return 0;
    }

    int iSize = xDim * yDim;
    memcpy(pOutputImg, pInputImg, iSize);

    int* iStack = new int[iSize];
    int* jStack = new int[iSize];
    int stackIndex = 0;

    int label = 1 ;  // start by 2
    int x(0), y(0);

    int count = 0;

    for (int j = 0; j < yDim; ++j)
    {
        //ignore edge, so subvolume should larger 2 pixels than before
        //char* data= pInputImg;
        for (int i = 0; i < xDim; ++i)
        {
            if (/*pInputImg[j * xDim + i] == 0 &&*/ pOutputImg[j * xDim + i] == 0)
            {
                push_stack(i,j) ;     // pixel position: <i,j>

                if (label == 1)
                {
                    // ++label;
                }
                else if (j == yDim - 1 || i == xDim - 1)
                {
                    pOutputImg[i + j * xDim] = 2;//backgroud
                    continue;
                }
                ++label; // begin with a new label 

                while (stackIndex > 0)
                {
                    // get the top pixel on the stack and label it with the same label
                    int curX, curY;
                    pop_stack(curX, curY);

                    if (curX + curY * xDim < iSize)
                    {
                        pOutputImg[curX + curY * xDim] = label;
                    }

                    if (xDim-1 == curX || yDim-1 == curY) continue;

                    // push the 4-neighbors (foreground pixels)
                    x = curX; y = curY - 1;
                    push_stack_if_0(x, y);
                    x = curX; y = curY + 1;
                    push_stack_if_0(x, y);
                    x = curX - 1; y = curY;
                    push_stack_if_0(x, y);
                    x = curX + 1; y = curY;
                    push_stack_if_0(x, y);
                    count += 4;
                }
            }
            else if (/*pInputImg[j * xDim + i] == 1 &&*/ pOutputImg[j * xDim + i] == 1)
            {  
                push_stack(i,j) ;     // pixel position: <i,j>
                ++label ;  // begin with a new label

                while (stackIndex > 0)
                {
                    // get the top pixel on the stack and label it with the same label  
                    int curX, curY;
                    pop_stack(curX, curY);

                    if (curX + curY * xDim < iSize)
                    {
                        pOutputImg[curX + curY * xDim] = label;
                    }

                    if (0 == curX || 0 == curY) continue;

                    // push the 4-neighbors (foreground pixels)  
                    x = curX; y = curY - 1;
                    push_stack_if_1(x, y);
                    x = curX; y = curY + 1;
                    push_stack_if_1(x, y);
                    x = curX - 1; y = curY;
                    push_stack_if_1(x, y);
                    x = curX + 1; y = curY;
                    push_stack_if_1(x, y);
                    count += 4;
                }
            }
        }  
    }  
    const int contourNum = label - 2;
    delete[] iStack;
    delete[] jStack;
    return contourNum;
}

int make_contour_from_map(
    char* pInputMask,
    int xDim,
    int yDim,
    int i,
    int j,
    int index,
    int offset,
    plunc::PNT3D *pt,
    float xStart,
    float yStart,
    float xScale,
    float yScale)
{
    int count;
    int dir;
    int old_dir;
    int old_i;
    int old_j;

    void (*find_next_point)(char*, int, int, int*, int*, int*, int*);
    const int slicecnt = xDim * yDim;
    count = 0;
    const int first_index = index;
    pt +=  offset;
    //if (exterior_pixel(pInputMask, index, xDim) == 1) {//TODO if need
    //    find_next_point = find_next_ccw;
    //    old_dir = dir = WEST;
    //}
    //else
    {
        find_next_point = find_next_clockwise;
        dir = EAST;
        old_dir = WEST;
    }

    old_i = i;
    old_j = j;
    while (1)
    {
        pInputMask[index] = 2;
        if (count >= slicecnt)
        {
            // here means sth wrong!
            fprintf(stderr, "too many points\n");
            TPS_LOG_DEV_ERROR<<"too many points:"<<count;
            return (0);
        }
        if (dir != old_dir)
        {
            // 当old点和前一个点重复的时候，不存储这个点
            if (count > 0 && ((pt-1)->x != old_i || (pt -1)->y != old_j)) 
            {
                // 转向之前，将点添加
                pt->x = xStart + xScale * old_i;
                pt->y = yStart + yScale * old_j;
                pt++;
                count++;
            }
            // 转向后的新点，将其添加
            old_dir = dir;
            pt->x = xStart + xScale * i;
            pt->y = yStart + yScale * j;
            pt++;
            count++;
        }
        old_i = i;
        old_j = j;
        find_next_point(pInputMask, xDim, yDim, &index, &i, &j, &dir);

        // 最后一个点和第一个点重合时，可以跳出循环
        if (count > 1 && index == first_index)
        {
            pt->x = xStart + xScale * old_i;
            pt->y = yStart + yScale * old_j;
            pt++;
            count++;
            break;
        }
    }

    index = first_index % slicecnt;
    pt->x = xStart + xScale * (index % xDim);
    pt->y = yStart + yScale * (index / xDim);
    pt++;
    count++;

    return(count);
}

void transform_point(PNT3D pt, float T[4][4], PNT3D *tpt) {
    tpt->x = pt.x * T[0][0] + pt.y * T[1][0] + pt.z * T[2][0] + T[3][0];
    tpt->y = pt.x * T[0][1] + pt.y * T[1][1] + pt.z * T[2][1] + T[3][1];
    tpt->z = pt.x * T[0][2] + pt.y * T[1][2] + pt.z * T[2][2] + T[3][2];
}

//fix ana sense
float calc_angle_X(float x, float y)
{
    float	cos_theta, theta;
    float	len;

    len = sqrt(x*x + y*y);
    if (len == 0.0) return(0.0);
    cos_theta = x/len;
    theta = acos(cos_theta);
    if (y < 0.0) theta =  2.0 * PI - theta;
    return(theta);
}

float calc_angle(float x1, float y1, float x2, float y2, float x3, float y3) 
{
    float a1 = calc_angle_X(x2 - x1, y2 - y1);
    float a2 = calc_angle_X(x3 - x2, y3 - y2);
    double a = a2 - a1;
    if (a > PI) a -= 2.0f * PI;
    if (a < -PI) a += 2.0f * PI;
    return float(a);
}

int contour_sense(CONTOUR_STC *con) {
    int		i, j, k;
    float	x1, y1, x2, y2, x3, y3;
    float	angle, total_angle = 0.0;

    for (j = 0; j < con->vertex_count; j++) {
        i = j - 1;
        if (i < 0) i = con->vertex_count - 1;
        k = j + 1;
        if (k == con->vertex_count) k = 0;
        x1 = con->x[i]; y1 = con->y[i];
        x2 = con->x[j]; y2 = con->y[j];
        x3 = con->x[k]; y3 = con->y[k];
        angle = calc_angle(x1, y1, x2, y2, x3, y3);
        total_angle += angle;
    }
    return(total_angle > 0.0);
}

int fix_anastruct_sense(ANASTRUCT *ana) {
    int		i, j, k;
    int		sense;
    float	x, y;
    CONTOUR_STC	*con;

    if (ana->contour_count < 2) return(-1);
    sense = contour_sense(&ana->contours[0]);
    for (j = 1; j < ana->contour_count; j++) {
        if (sense != contour_sense(&ana->contours[j])) {
            con = ana->contours + j;
            for (i = 1; i <= con->vertex_count/2; i++) {
                k = con->vertex_count - i;
                if (k == i) break;
                x = con->x[i];
                y = con->y[i];
                con->x[i] = con->x[k];
                con->y[i] = con->y[k];
                con->x[k] = x;
                con->y[k] = y;
            }
        }
    }
    return(0);
}

//resample
void resample_contours(ANASTRUCT *ana, int cnt) {
    int		i, j, k;
    int		old_cnt;
    int         shifted, min_shift;
    float	cx, cy;
    float	dx, dy;
    float       ssd, min_ssd; /* sum of squared dists */
    float	frac;
    float	per, total_len, len, dlen;

    float	*x, *y, *t;
    CONTOUR_STC	*con, *prev_con = NULL;

    for (k = 0; k < ana->contour_count; k++) {
        /* Find the contour centroid and perimeter */
        con = &ana->contours[k];
        old_cnt = con->vertex_count;
        x = (float *)malloc(old_cnt*sizeof(float));
        y = (float *)malloc(old_cnt*sizeof(float));
        cx = cy = 0.0;
        per = 0.0;
        for (i = 0; i < old_cnt; i++) {
            if (i == old_cnt-1) j = 0;
            else j = i+1;
            x[i] = con->x[i];
            y[i] = con->y[i];
            cx += x[i];
            cy += y[i];
            dx = (con->x[j] - con->x[i]);
            dy = (con->y[j] - con->y[i]);
            per += sqrt(dx*dx + dy*dy);
        }
        cx /= old_cnt;
        cy /= old_cnt;
        /* Resample the contour */
        con->vertex_count = cnt;
        con->x = (float *)realloc(con->x, cnt*sizeof(float));
        con->y = (float *)realloc(con->y, cnt*sizeof(float));

        j = 0;
        dlen = per/cnt;
        len = total_len = 0.0;
        for (i = 0; i < cnt; i++) {
            int		v1, v2;
            float	seg_len;

            v1 = j;
            if (j == old_cnt-1) v2 = 0;
            else v2 = j + 1;
            dx = x[v2] - x[v1];
            dy = y[v2] - y[v1];
            seg_len = sqrt(dx*dx + dy*dy);
            if(seg_len == 0) {
                i--;
                j++;
                if (j == old_cnt) j = 0;
                continue;
            }
            frac = len/seg_len;

            if (frac > 1.0) {
                len -= seg_len;
                i--;
                j++;
                if (j == old_cnt) j = 0;
                continue;
            }
            con->x[i] = (1.0 - frac)*x[v1] + frac*x[v2];
            con->y[i] = (1.0 - frac)*y[v1] + frac*y[v2];
            len += dlen;
            total_len += dlen;
        }
        free(x);
        free(y);

        if (prev_con) {
            /* Find the shift giving the best correspondence */
            min_ssd = 100000;
            min_shift = 0;
            for (j = 0; j < cnt; j++) {
                ssd = 0;
                for (i = 0; i < cnt; i++) {
                    shifted = i + j;
                    if (shifted >= cnt) shifted -= cnt;
                    dx = con->x[shifted] - prev_con->x[i];
                    dy = con->y[shifted] - prev_con->y[i];
                    ssd += dx*dx + dy*dy;
                }
                if (ssd < min_ssd) {
                    min_ssd = ssd;
                    min_shift = j;
                }
            }

            /* Rotate the contour by this shift */
            t = (float *)malloc(2*cnt*sizeof(float));
            for (i = 0; i < cnt; i++) {
                j = i + min_shift;
                if (j >= cnt) j -= cnt;
                t[i] = con->x[j];
                t[i+cnt] = con->y[j];
            }
            for (i = 0; i < cnt; i++) {
                con->x[i] = t[i];
                con->y[i] = t[i+cnt];
            }
            free(t);
        }
        prev_con = con;
    }
}

//Begin Taubin Smoothing
//////////////////////////////////////////////////////////////////////////

void hxy(ANASTRUCT *ana, int c, int v, float *hx, float *hy) {
    CONTOUR_STC *con = ana->contours;
    int cnt2 = con[c].vertex_count;
    if (v < 0) v = cnt2-1;
    if (v == cnt2) v = 0;
    *hx = con[c].x[v]; 
    *hy = con[c].y[v];
}

PNT3D laplacian(float wxy, float wz, ANASTRUCT *ana, int *c, int v) {
    PNT3D p;
    float hx, hxcm1, hxcp1, hxvm1, hxvp1;
    float hy, hycm1, hycp1, hyvm1, hyvp1;
    hxy(ana, c[1], v, &hx, &hy);
    hxy(ana, c[0], v, &hxcm1, &hycm1);
    hxy(ana, c[2], v, &hxcp1, &hycp1);
    hxy(ana, c[1], v-1, &hxvm1, &hyvm1);
    hxy(ana, c[1], v+1, &hxvp1, &hyvp1);
    p.x = 0.25*(wz*(hxcm1 + hxcp1) + wxy*(hxvm1 + hxvp1)) - hx;
    p.y = 0.25*(wz*(hycm1 + hycp1) + wxy*(hyvm1 + hyvp1)) - hy;
    return(p);
}

void smooth_ana(double s, PNT3D *w, ANASTRUCT *a_in, int c[][3], ANASTRUCT *a_out)
{
    int		i, j;
    PNT3D	p;
    float	wxy, wz;

    // The weights must be doubled because the Laplacians are 2D
    wxy = 2.0f*(w->x + w->y);
    wz = w->z + w->z;

    // Smooth the contours
    for (i = 0; i < a_in->contour_count; i++) {
        for (j = 0; j < a_in->contours[i].vertex_count; j++) {
            p = laplacian(wxy, wz, a_in, c[i], j);
            a_out->contours[i].x[j] = a_in->contours[i].x[j] - p.x*s;
            a_out->contours[i].y[j] = a_in->contours[i].y[j] - p.y*s;
        }
    }
}

bool taubin(ANASTRUCT *ana, int c[][3], PNT3D *wts, float kbp, float lambda, int N)
{
    int i, n;
    ANASTRUCT tmp;
    float mu;
    PNT3D w;

    if (lambda >= 1.0) return(false);
    if (lambda*kbp - 1.0 >= -0.00001)  // Mu must be negative, but small
        return(false);

    mu = lambda/(lambda*kbp - 1.0);

    float s = wts->x + wts->y + wts->z;
    w.x = wts->x/s;
    w.y = wts->y/s;
    w.z = wts->z/s;

    tmp.contour_count = ana->contour_count;
    tmp.contours = (CONTOUR_STC *)malloc(ana->contour_count*sizeof(CONTOUR_STC));
    if (tmp.contours == NULL) {
        fprintf(stderr, "taubin failed malloc\n");
        exit(0);
    }
    for (i = 0; i < ana->contour_count; i++) {
        tmp.contours[i].x = (float *)
            malloc(ana->contours[i].vertex_count*sizeof(float));
        tmp.contours[i].y = (float *)
            malloc(ana->contours[i].vertex_count*sizeof(float));
        if (tmp.contours[i].y == NULL) {
            fprintf(stderr, "taubin failed malloc\n");
            exit(0);
        }
        tmp.contours[i].vertex_count = ana->contours[i].vertex_count;
        tmp.contours[i].z = ana->contours[i].z;
    }

    for (n = 0; n < N; n++) {
        smooth_ana(lambda, &w, ana, c, &tmp);
        smooth_ana(mu, &w, &tmp, c, ana);
    }

    for (i = 0; i < ana->contour_count; i++) {
        free(tmp.contours[i].x);
        free(tmp.contours[i].y);
    }
    free(tmp.contours);

    return(true);
}

int my_sort(CONTOUR_STC *c1, CONTOUR_STC *c2) {
    if ((c1)->slice_number < (c2)->slice_number) return(-1);
    if ((c1)->slice_number ==  (c2)->slice_number) return(0);
    return(1);
}

int contour_min_max(CONTOUR_STC *c) {
    float *ptrx, *ptry;
    int loop;

    if (c->vertex_count == 0) return(0);
    ptrx = c->x;
    c->min.x = c->max.x = *ptrx++;
    ptry = c->y;
    c->min.y = c->max.y = *ptry++;
    for (loop = 1; loop < c->vertex_count; loop++, ptrx++, ptry++) {
        if (*ptrx > c->max.x) c->max.x = *ptrx;
        if (*ptrx < c->min.x) c->min.x = *ptrx;
        if (*ptry > c->max.y) c->max.y = *ptry;
        if (*ptry < c->min.y) c->min.y = *ptry;
    }
    return(0);
}

int anastruct_min_max(ANASTRUCT *ana) {
    int loop;

    ana->min.x = 10000.0;
    ana->min.y = 10000.0;
    ana->min.z = 10000.0;
    ana->max.x = -10000.0;
    ana->max.y = -10000.0;
    ana->max.z = -10000.0;

    for (loop = 0; loop < ana->contour_count; loop++) {
        contour_min_max(ana->contours + loop);
        if (ana->contours[loop].min.x < ana->min.x)
            ana->min.x = ana->contours[loop].min.x;
        if (ana->contours[loop].min.y < ana->min.y)
            ana->min.y = ana->contours[loop].min.y;
        if (ana->contours[loop].min.z < ana->min.z)
            ana->min.z = ana->contours[loop].min.z;
        if (ana->contours[loop].max.x > ana->max.x)
            ana->max.x = ana->contours[loop].max.x;
        if (ana->contours[loop].max.y > ana->max.y)
            ana->max.y = ana->contours[loop].max.y;
        if (ana->contours[loop].max.z > ana->max.z)
            ana->max.z = ana->contours[loop].max.z;
    }
    return (0);
}


void smooth_ana_cb(ANASTRUCT *ana ,float xSize, float ySize, float zSize) {
    int			i, j, k, n, s1, s2, cnt;
    int			first_slice, last_slice;
    int			c[10000][3];
    int			c_list[100];
    float		cx1, cy1, cx2, cy2, dist, min_dist;
    float		lx1, hx1, lx2, hx2;
    float		ly1, hy1, ly2, hy2;
    CONTOUR_STC		*con, *c2;
    PNT3D		spacings;

    anastruct_min_max(ana); // Just to be safe

    cnt = ana->contour_count;
    if (cnt < 2) return;

    //copy_ana(TRUE);
    qsort(ana->contours, cnt, sizeof(CONTOUR_STC), (SORT)my_sort);
    first_slice = ana->contours[0].slice_number;
    last_slice = ana->contours[cnt-1].slice_number;

    // Force all contours to have the same winding number
    if (fix_anastruct_sense(ana)) return;

    // Find maximum number of vertices per contour
    n = 0;
    for (i = 0; i < cnt; i++) {
        con = ana->contours + i;
        if (n < con->vertex_count) n = con->vertex_count;
    }
    // Resample the contours to have n points each and rotate for
    // slice-to-slice correspondence.
    resample_contours(ana, n);

    // Find the previous and next contour for each contour
    for (i = 0; i < cnt; i++) {
        con = ana->contours + i;

        // Note: c[i][0], c[i][1], c[i][2] are contour indices:
        // c[i][0]: the contour above con in the mesh
        // c[i][1]: this contour (con)
        // c[i][2]: the contour below con in the mesh
        // It can get complicated when there's more than 1 contour/slice
        c[i][1] = i;

        // Find the "center" and bounds of this contour
        cx1 = 0.5*(con->min.x + con->max.x);
        cy1 = 0.5*(con->min.y + con->max.y);
        lx1 = con->min.x; ly1 = con->min.y;
        hx1 = con->max.x; hy1 = con->max.y;

        // Determine how many contours share this slice
        s1 = 0;
        for (k = i-1; k >= 0; --k){
            c2 = ana->contours + k;
            if (c2->slice_number == con->slice_number)
            {
                s1++;
            }
            else break;
        }
        for (k = i+1; k < cnt; ++k){
            c2 = ana->contours + k;
            if (c2->slice_number == con->slice_number)
            {
                s1++;
            }
            else break;
        }

        c[i][0] = c[i][2] = -1;
        // Find the first above slice that has at least 1 contour on it
        s2 = 0;
        for (k = 1; con->slice_number-k >= first_slice; k++) {
            for (j = i-1; j >= 0; j--) {
                c2 = ana->contours + j;
                if (c2->slice_number == con->slice_number-k) {
                    c_list[s2] = j;
                    s2++;
                }
            }
            if (s2) break;
        }
        if (s2 == 0) c[i][0] = i; // no contour above, link to self
        else { // possible contour(s) above to link to
            if (s1 == 1 && s2 == 1) // easy case, 1:1 correspondence
                c[i][0] = c_list[0];
            else {
                // Multiple contours to link to, pick "closest"
                min_dist = 1000.0;
                for (j = 0; j < s2; j++) {
                    c2 = ana->contours + c_list[j];
                    cx2 = 0.5*(c2->min.x + c2->max.x);
                    cy2 = 0.5*(c2->min.y + c2->max.y);
                    dist = (cx2 - cx1)*(cx2 - cx1) + (cy2 - cy1)*(cy2 - cy1);
                    if (min_dist > dist) {
                        k = c_list[j];
                        min_dist = dist;
                    }
                }
                c[i][0] = k;
            }
            // Sanity check that these two contours should be linked
            c2 = ana->contours + c[i][0];
            cx2 = 0.5*(c2->min.x + c2->max.x);
            cy2 = 0.5*(c2->min.y + c2->max.y);
            lx2 = c2->min.x; ly2 = c2->min.y;
            hx2 = c2->max.x; hy2 = c2->max.y;
            if (cx2 < lx1 || cx2 > hx1) c[i][0] = i;
            if (cy2 < ly1 || cy2 > hy1) c[i][0] = i;
        }

        // Find the first below slice that has at least 1 contour on it
        s2 = 0;
        for (k = 1; con->slice_number+k <= last_slice; k++) {
            for (j = i+1; j < cnt; j++) {
                c2 = ana->contours + j;
                if (c2->slice_number == con->slice_number+k) {
                    c_list[s2] = j;
                    s2++;
                }
            }
            if (s2) break;
        }
        if (s2 == 0) c[i][2] = i; // no contour below, link to self
        else {
            if (s2 == 1 && s1 == 1) // easy case, 1:1 correspondence
                c[i][2] = c_list[0];
            else {
                // Multiple contours to link to, pick "closest"
                min_dist = 1000.0;
                for (j = 0; j < s2; j++) {
                    c2 = ana->contours + c_list[j];
                    cx2 = 0.5*(c2->min.x + c2->max.x);
                    cy2 = 0.5*(c2->min.y + c2->max.y);
                    dist = (cx2 - cx1)*(cx2 - cx1) + (cy2 - cy1)*(cy2 - cy1);
                    if (min_dist > dist) {
                        k = c_list[j];
                        min_dist = dist;
                    }
                }
                c[i][2] = k;
            }
            // Sanity check that these two contours should be linked
            c2 = ana->contours + c[i][2];
            cx2 = 0.5*(c2->min.x + c2->max.x);
            cy2 = 0.5*(c2->min.y + c2->max.y);
            lx2 = c2->min.x; ly2 = c2->min.y;
            hx2 = c2->max.x; hy2 = c2->max.y;
            if (cx2 < lx1 || cx2 > hx1) c[i][2] = i;
            if (cy2 < ly1 || cy2 > hy1) c[i][2] = i;
        }
    }

    // Select the weights.  This is obviously ad hoc.  Since the
    // inter-slice distance is usually much greater than the intra-slice
    // pixel sizes, contouring irregularities between slices are usually
    // most apparent.  On this basis, the voxel sizes was selected and
    // then they are squared to intensify to effect.  Nevertheless,
    // using any weights seems to make very little difference. 
    spacings.x = xSize * xSize;
    spacings.y = ySize * xSize;
    spacings.z = zSize;
    spacings.z *= spacings.z;

    float kbp = 0.1f;	// Taubin band-pass cutoff frequency
    float lambda = 0.62f;    // A typical Taubin smoothing weight
    // Taubin's model situation must be scaled for this problem:
    float dc = 0.2f;	// Diffusion constant (factor), obtained
    // by trial and error.

    if (!taubin(ana, c, &spacings, kbp, dc*lambda, TAUBIN_PASSES)) {
        fprintf(stderr, "Internal error: incorrect smoothing parameters\n");
        return;
    }
    //     for (i = 0; i < ana->contour_count; i++) {
    //         con = &ana->contours[i];
    //         reduce_con(0.05f, con);
    //     }
    anastruct_min_max(ana);
    //post2_ana_stats(ana);
}

float interp(
    int mode,
    float x1,
    float x2,
    float x,
    float y1,
    float y2,
    float *fx)
{
    *fx = (x - x1) / (x2 - x1);
    if (mode)
    {
        if (*fx > 1.0)
            *fx = 1.0;
        if (*fx < 0.0)
            *fx = 0.0;
    }

    return(y1 + (y2 - y1) * *fx);
}
TPS_PLUNC_END_NAMESPACE
