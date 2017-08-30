////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_pl_collimator.h
/// 
///  \brief header file for beam collimator
/// 
///  \version 1.0
/// 
///  \date    2014/05/12
////////////////////////////////////////////////////////////////

#ifndef TPS_PL_COLLIMATOR_H
#define TPS_PL_COLLIMATOR_H

#include "tps_defs.h"
#include "tps_pl_datastructure.h"

#define TPS_FI_LIMIT    (50)

#ifndef PI
#define PI          (3.141592654)
#endif

#ifndef MAX
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#define Free free
#define Malloc malloc

TPS_PLUNC_BEGIN_NAMESPACE  // begin namespace tps::plunc

TPS_EXPORT_API int FindNtrsc( CONTOUR_STC *Contour,
               float Point1[2],
               float Point2[2],
               float Intersects[2][TPS_FI_LIMIT],
               float Distances[TPS_FI_LIMIT]);

// ͨ�������ĵ㼯��ҶƬ����
TPS_EXPORT_API int CollimatorEnclosure(int nVertex,
                        float* pVertexX, 
                        float* pVertexY, 
                        int nLeafNum,
                        LEAF_POSITION_STC* pLeaf,
                        double minBoundary,
                        double maxBoundary,
                        int nMlcEditMode = 0);

// ͨ��BeVͶӰ��Mapֱ����ҶƬ����
TPS_EXPORT_API int LeafAdaptForProjectionMap(SCAN3D_MAP* pMap,
                              int nLeafNum,
                              LEAF_POSITION_STC* pLeaf,
                              double minBoundary,
                              double maxBoundary,
                              int nMlcEditMode = 0);

TPS_PLUNC_END_NAMESPACE  // end namespace tps::plunc

#endif //TPS_PL_COLLIMATOR_H
