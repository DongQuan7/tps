////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_pl_autoshape_func.h
/// 
///  \brief header file for MLC autoshape 
/// 
///  \version 1.0
/// 
///  \date    2014/05/12
////////////////////////////////////////////////////////////////

#ifndef TPS_PL_AUTOSHAPE_FUNC_H
#define TPS_PL_AUTOSHAPE_FUNC_H

#include "tps_defs.h"
#include "tps_pl_datastructure.h"
#include "RtTpsAlgorithmProxy/tps_pl_utility.h"

#define MY_MAP_SIZE  128

#ifndef FALSE
    #define FALSE (0)
#endif

#ifndef TRUE
    #define TRUE (1)
#endif

#define bound_point(x1, y1, z1, mat)    \
    p1.x = x1; p1.y = y1; p1.z = z1;    \
    transform_point(p1, mat, &p2);      \
    p2.x = -1000.0f * p2.x / p2.z;      \
    p2.y = -1000.0f * p2.y / p2.z;      \
    if (p2.x < min_x) min_x = p2.x;     \
    if (p2.y < min_y) min_y = p2.y;     \
    if (p2.x > max_x) max_x = p2.x;     \
    if (p2.y > max_y) max_y = p2.y;


TPS_PLUNC_BEGIN_NAMESPACE  // begin namespace tps


TPS_EXPORT_API void cat_mat(float left[4][4], float right[4][4], float result[4][4]);

TPS_EXPORT_API void autoblock(ANASTRUCT *ana, float matImage2Beam[4][4],
    float margin_x, float margin_y,
    ANASTRUCT *anaOAR, float matImage2BeamOAR[4][4],
    float margin_oar_x, float margin_oar_y,
    float* maxRange, CONTOUR_STC* pContour);

TPS_EXPORT_API void autoblock_for_only_avoid(float* fieldRange, ANASTRUCT *anaOAR,
    float matImage2BeamOAR[4][4], 
    float margin_oar_x, float margin_oar_y, 
    CONTOUR_STC* pContour);

TPS_EXPORT_API void make_projection_map(ANASTRUCT *ana, float matImage2Beam[4][4],
    float margin_x, float margin_y,
    ANASTRUCT *anaOAR, float matImage2BeamOAR[4][4],
    float margin_oar_x, float margin_oar_y,
    float* maxRange, SCAN3D_MAP* pMap);


TPS_EXPORT_API void make_projection_map_for_only_avoid(float* fieldRange, ANASTRUCT *anaOAR,
    float matImage2BeamOAR[4][4], 
    float margin_oar_x, float margin_oar_y,
    SCAN3D_MAP* pMap);


TPS_EXPORT_API void get_bounding_box(ANASTRUCT *ana, float matImage2Beam[4][4],
    float margin_x, float margin_y, 
    ANASTRUCT *anaOAR, float matImage2BeamOAR[4][4],
    float margin_oar_x, float margin_oar_y,
    float* maxRange, float* boundingBox);

TPS_EXPORT_API void get_bounding_box_for_only_avoid(float* fieldRange, ANASTRUCT *anaOAR, 
    float matImage2BeamOAR[4][4],
    float margin_x, float margin_y,
    float* boundingBox);

TPS_EXPORT_API bool SaveToTrueColorBitmap(const std::string& path,
    unsigned char* buffer,
    const int& height,
    const int& width);

TPS_PLUNC_END_NAMESPACE  // end namespace tps

#endif //TPS_PL_AUTOSHAPE_FUNC_H
