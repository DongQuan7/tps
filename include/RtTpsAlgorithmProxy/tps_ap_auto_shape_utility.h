#ifndef TPS_AP_AUTO_SHAPE_UTILITY_H_
#define TPS_AP_AUTO_SHAPE_UTILITY_H_

#include "tps_ap_defs.h"
#include "tps_pl_datastructure.h"

TPS_BEGIN_NAMESPACE

class ROIEntity;

class TPS_AP_DECLSPEC AutoShapeUtility
{
public:
    static void GenerateOutlineFromVoi(
        const float matVol2Pat[4][4],
        ROIEntity* gtv,
        float margin_x,
        float margin_y,
        ROIEntity* oar,
        float margin_oar_x,
        float margin_oar_y,
        float matPat2Beam[4][4],
        float shappingField[4],
        plunc::CONTOUR_STC* pContour);

    static void GenerateOutlineForOnlyAvoid(
        const float matVol2Pat[4][4],
        ROIEntity* oar,
        float margin_x,
        float margin_y,
        float matPat2Beam[4][4],
        float shappingField[4],
        plunc::CONTOUR_STC* pContour);

    static void GenerateBoundingBoxFromVoi(
        const float matVol2Pat[4][4],
        ROIEntity* gtv,
        float margin_x,
        float margin_y,
        ROIEntity* oar,
        float margin_oar_x,
        float margin_oar_y,
        float matPat2Beam[4][4],
        float shappingField[4],
        float boundingBox[4]);

    static void GenerateBoundingBoxForOnlyAvoid(
        const float matVol2Pat[4][4],
        ROIEntity* oar,
        float margin_x,
        float margin_y,
        float matPat2Beam[4][4],
        float shappingField[4],
        float boundingBox[4]);

    static void GenerateMapFromVoi(
        ROIEntity* gtv,
        float margin_x,
        float margin_y,
        ROIEntity* oar,
        float margin_oar_x,
        float margin_oar_y,
        float matPat2Beam[4][4],
        float shappingField[4],
        plunc::SCAN3D_MAP* pMap);

    static void GenerateMapForOnlyAvoid(
        ROIEntity* oar,
        float margin_x,
        float margin_y,
        float matPat2Beam[4][4],
        float shappingField[4],
        plunc::SCAN3D_MAP* pMap);
};

TPS_END_NAMESPACE

#endif