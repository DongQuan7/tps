#include "StdAfx.h"
#include "RtTpsAlgorithmProxy/tps_ap_auto_shape_utility.h"
#include "RtTpsAlgorithmProxy/tps_pl_utility.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsAlgorithmProxy/tps_pl_autoshape_func.h"

TPS_BEGIN_NAMESPACE

using namespace plunc;

void AutoShapeUtility::GenerateOutlineFromVoi(
    const float matVol2Pat[4][4],
    ROIEntity* gtv,
    float margin_x,
    float margin_y,
    ROIEntity* oar,
    float margin_oar_x,
    float margin_oar_y,
    float matPat2Beam[4][4],
    float shappingField[4],
    CONTOUR_STC* pContour)
{
    if (gtv == nullptr) return;

    matVol2Pat;

    ANASTRUCT ana;
    memset(&ana, 0, sizeof(ANASTRUCT));
    SubVolumeHelperAPI::VOIToPluncAnastruct(gtv, &ana);

    ANASTRUCT anaOAR;
    memset(&anaOAR, 0, sizeof(ANASTRUCT));
    ANASTRUCT* pOAR = nullptr;
    if (nullptr != oar)
    {
        pOAR = &anaOAR;
        SubVolumeHelperAPI::VOIToPluncAnastruct(oar, &anaOAR);
    }

    autoblock(&ana, matPat2Beam, margin_x, margin_y, pOAR, matPat2Beam,
        margin_oar_x, margin_oar_y, shappingField, pContour);

    if (nullptr != gtv)
    {
        for (int i = 0; i < ana.contour_count; i++)
        {
            free(ana.contours[i].x);
            free(ana.contours[i].y);
        }
        free(ana.contours);
    }

    if (nullptr != oar)
    {
        for (int i = 0; i < anaOAR.contour_count; i++)
        {
            free(anaOAR.contours[i].x);
            free(anaOAR.contours[i].y);
        }
        free(anaOAR.contours);
    }
}

void AutoShapeUtility::GenerateOutlineForOnlyAvoid(
    const float matVol2Pat[4][4],
    ROIEntity* oarVoi,
    float margin_x,
    float margin_y,
    float matPat2Beam[4][4],
    float shappingField[4],
    CONTOUR_STC* pContour)
{
    if (oarVoi == nullptr)
    {
        return;
    }
    matVol2Pat;
    ANASTRUCT oar;
    memset(&oar, 0, sizeof(ANASTRUCT));
    SubVolumeHelperAPI::VOIToPluncAnastruct(oarVoi, &oar);

    autoblock_for_only_avoid(shappingField, &oar, matPat2Beam, margin_x, margin_y, pContour);

    for(int i = 0; i < oar.contour_count; i++)
    {
        free(oar.contours[i].x);
        free(oar.contours[i].y);
    }
    free(oar.contours);
}

void AutoShapeUtility::GenerateBoundingBoxFromVoi(
    const float matVol2Pat[4][4],
    ROIEntity* gtv,
    float margin_x,
    float margin_y,
    ROIEntity* oar,
    float margin_oar_x,
    float margin_oar_y,
    float matPat2Beam[4][4],
    float shappingField[4],
    float boundingBox[4])
{
    matVol2Pat;
    if (gtv == nullptr){
        return;
    }

    ANASTRUCT ana;
    memset(&ana, 0, sizeof(ANASTRUCT));
    SubVolumeHelperAPI::VOIToPluncAnastruct(gtv, &ana);

    ANASTRUCT anaOAR;
    memset(&anaOAR, 0, sizeof(ANASTRUCT));
    ANASTRUCT* pOAR = NULL;
    if(oar) {
        pOAR = &anaOAR;
        SubVolumeHelperAPI::VOIToPluncAnastruct(oar, &anaOAR);
    }

    get_bounding_box(&ana, matPat2Beam, margin_x,margin_y,
        pOAR, matPat2Beam, margin_oar_x, margin_oar_y, shappingField, boundingBox);

    if(gtv) {
        for(int i = 0; i < ana.contour_count; i++)
        {
            free(ana.contours[i].x);
            free(ana.contours[i].y);
        }
        free(ana.contours);
    }

    if(oar) {
        for(int i = 0; i < anaOAR.contour_count; i++)
        {
            free(anaOAR.contours[i].x);
            free(anaOAR.contours[i].y);
        }
        free(anaOAR.contours);
    }
}

void AutoShapeUtility::GenerateBoundingBoxForOnlyAvoid(
    const float matVol2Pat[4][4],
    ROIEntity* oarVoi,
    float margin_x,
    float margin_y,
    float matPat2Beam[4][4],
    float shappingField[4],
    float boundingBox[4])
{
    matVol2Pat;
    if (oarVoi == nullptr){
        return;
    }
    ANASTRUCT oar;
    memset(&oar, 0, sizeof(ANASTRUCT));
    SubVolumeHelperAPI::VOIToPluncAnastruct(oarVoi, &oar);

    get_bounding_box_for_only_avoid(shappingField, &oar, matPat2Beam, margin_x, margin_y, boundingBox);

    //if(oar) 
    {
        for(int i = 0; i < oar.contour_count; i++)
        {
            free(oar.contours[i].x);
            free(oar.contours[i].y);
        }
        free(oar.contours);
    }
}

void AutoShapeUtility::GenerateMapFromVoi(
    ROIEntity* gtv,
    float margin_x,
    float margin_y,
    ROIEntity* oar,
    float margin_oar_x,
    float margin_oar_y,
    float matPat2Beam[4][4],
    float shappingField[4],
    SCAN3D_MAP* pMap)
{
    if (gtv == nullptr) return;

    ANASTRUCT ana;
    memset(&ana, 0, sizeof(ANASTRUCT));
    SubVolumeHelperAPI::VOIToPluncAnastruct(gtv, &ana);

    ANASTRUCT anaOAR;
    memset(&anaOAR, 0, sizeof(ANASTRUCT));
    ANASTRUCT* pOAR = nullptr;
    if (nullptr != oar)
    {
        pOAR = &anaOAR;
        SubVolumeHelperAPI::VOIToPluncAnastruct(oar, &anaOAR);
    }

    make_projection_map(&ana, matPat2Beam, margin_x, margin_y, pOAR, matPat2Beam,
        margin_oar_x, margin_oar_y, shappingField, pMap);

    if (nullptr != gtv)
    {
        for (int i = 0; i < ana.contour_count; i++)
        {
            free(ana.contours[i].x);
            free(ana.contours[i].y);
        }
        free(ana.contours);
    }

    if (nullptr != oar)
    {
        for (int i = 0; i < anaOAR.contour_count; i++)
        {
            free(anaOAR.contours[i].x);
            free(anaOAR.contours[i].y);
        }
        free(anaOAR.contours);
    }
}

void AutoShapeUtility::GenerateMapForOnlyAvoid(
    ROIEntity* oarVoi,
    float margin_x,
    float margin_y,
    float matPat2Beam[4][4],
    float shappingField[4],
    SCAN3D_MAP* pMap)
{
    if (oarVoi == nullptr)
    {
        return;
    }

    ANASTRUCT oar;

    memset(&oar, 0, sizeof(ANASTRUCT));

    SubVolumeHelperAPI::VOIToPluncAnastruct(oarVoi, &oar);

    make_projection_map_for_only_avoid(shappingField, &oar, matPat2Beam, margin_x, margin_y, pMap);

    for (int i = 0; i < oar.contour_count; i++)
    {
        free(oar.contours[i].x);
        free(oar.contours[i].y);
    }
    free(oar.contours);
}

TPS_END_NAMESPACE