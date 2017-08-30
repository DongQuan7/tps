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

#ifndef TPS_PL_UTILITY_H_
#define TPS_PL_UTILITY_H_

#include "tps_ap_defs.h"
#include "tps_pl_datastructure.h"

#ifndef PI
#define PI          (3.141592654f)
#endif

namespace tps
{
    class TpsImage3DEntity;
    class RtContour;
}


TPS_PLUNC_BEGIN_NAMESPACE

typedef int (*SORT)(const void *, const void *);

TPS_EXPORT_API void transform_point(PNT3D pt, float T[4][4], PNT3D *tpt);

TPS_EXPORT_API void push(int index1, int index2, float x1, float y1, float x2, float y2);

TPS_EXPORT_API void pop(struct cord_stc *next_cord);

TPS_EXPORT_API float dist(float x, float y, struct cord_stc *this_cord);

TPS_EXPORT_API void cord_test(float criterion,
    float *xptr,
    float *yptr,
    int   count,
    float *new_xptr,
    float *new_yptr,
    int   *new_count);

TPS_EXPORT_API int find_next_point(char* pMask, int xDim, int index, int *dir);

// 该接口存在Bug 代码暂且保留 comment by wzd 2016-11-09
//TPS_EXPORT_API int make_point_list(char* pMask, // input mask
//    int xDim,
//    int yDim,
//    int index,
//    PNT3D *pOutline, // point to be added
//    int nOutline,
//    int value = 3);

// 该接口存在Bug 代码暂且保留 comment by wzd 2016-11-09
//TPS_EXPORT_API void mask_to_contour_discarded(
//    const char* pInputMask,
//    int xDim,
//    int yDim,
//    CONTOUR_STC* pContour);

TPS_EXPORT_API void mask_to_contour(
    char* pInputMask,
    int zCoord,
    int xDim,
    int yDim,
    int pixValue,
    CONTOUR_STC* pContour,
    float xStart = 0.0f,
    float yStart = 0.0f,
    float xScale = 1.0f,
    float yScale = 1.0f);

// connected component analysis (4-component)
// use seed filling algorithm  
// 1. begin with a foreground pixel and push its foreground neighbors into a stack;  
// 2. pop the top pixel on the stack and label it with the same label until the stack is empty  
// foreground pixel: pInputImg(x,y) = 1  
// background pixel: pInputImg(x,y) = 0  
// label start by 2,3,4...
// return count of area
TPS_EXPORT_API int SeedFill(const char* pInputImg, int xDim, int yDim, char* pOutputImg);

TPS_EXPORT_API void reduce_con(float criterion, CONTOUR_STC *con);

TPS_EXPORT_API int make_contour_from_map(
    char* pInputMask,
    int xDim,
    int yDim,
    int i,
    int j,
    int index,
    int offset,
    plunc::PNT3D *pt,
    float xStart = 0.0f,
    float yStart = 0.0f,
    float xScale = 1.0f,
    float yScale = 1.0f);

TPS_EXPORT_API float calc_angle_X(float x, float y);

TPS_EXPORT_API float calc_angle(float x1, float y1, float x2, float y2, float x3, float y3);

TPS_EXPORT_API int contour_sense(CONTOUR_STC *con);

TPS_EXPORT_API int fix_anastruct_sense(ANASTRUCT *ana);

TPS_EXPORT_API void resample_contours(ANASTRUCT *ana, int cnt);

TPS_EXPORT_API void hxy(ANASTRUCT *ana, int c, int v, float *hx, float *hy);

TPS_EXPORT_API PNT3D laplacian(float wxy, float wz, ANASTRUCT *ana, int *c, int v);

TPS_EXPORT_API void smooth_ana(double s, PNT3D *w, ANASTRUCT *a_in, int c[][3], ANASTRUCT *a_out);

TPS_EXPORT_API bool taubin(ANASTRUCT *ana, int c[][3], PNT3D *wts, float kbp, float lambda, int N);

TPS_EXPORT_API int contour_min_max(CONTOUR_STC *c);

TPS_EXPORT_API int anastruct_min_max(ANASTRUCT *ana);

TPS_EXPORT_API void smooth_ana_cb(ANASTRUCT *ana ,float xSize, float ySize, float zSize);

TPS_EXPORT_API float interp(
    int mode,
    float x1,
    float x2,
    float x,
    float y1,
    float y2,
    float *fx);

TPS_PLUNC_END_NAMESPACE

#endif
