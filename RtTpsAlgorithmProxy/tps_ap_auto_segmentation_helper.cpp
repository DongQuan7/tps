////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_ap_auto_segmentation_helper.cpp
/// 
///  \brief class auto segmentation 
/// 
///  \version 1.0
/// 
///  \date    Dec.10,2013
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

#include <math.h>
#include <omp.h>
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"

//ZHENGHE
#include "McsfAlgorithms/McsfAppLib/McsfAlgoAutoContour.h"
#include "McsfAlgorithms/McsfAppLib/McsfAlgoBedBoardRemove.h"
#include "McsfGeometry/mcsf_geometry_plane.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfMedViewer3DArithmetic/point2d.h"
#include "McsfUtility/mcsf_progress_interface.h"

//TPS
#include "tps_logger.h"
#include "tps_defs.h"
#include "RtTpsFramework/tps_fw_mask.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsAlgorithmProxy/tps_pl_datastructure.h"
#include "RtTpsAlgorithmProxy/tps_pl_utility.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_contour.h"

TPS_BEGIN_NAMESPACE;

int SubVolumeHelperAPI::SkinSegmentation(
    const TpsImage3DEntity& image3DData, 
    SubVolume* pSubVolume,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::SkinSegmentation start.";
    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour)
    {
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    //Orientation flag, when the source data is supine, the flag is true; otherwise, the flag is false.
    //bool bFaceFront = image3DData.get()->m_Header.m_cPatientPosition[2] == 'S' ? true : false;

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->SkinSegmentation(*pSubVolume, pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: SKIN");
    if (!bSeg)
    {
        TPS_LOG_DEV_ERROR<<"SkinSegmentation failed.";
    }
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::SkinSegmentation end.";
    return bSeg ? TPS_ER_SUCCESS : TPS_ER_FAILURE;
}

int SubVolumeHelperAPI::LungsSegmentation(
    const TpsImage3DEntity& image3DData, 
    int lungType, 
    SubVolume* pLungs,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::LungsSegmentation start.";
    if (nullptr == pLungs){
        TPS_LOG_DEV_ERROR<<"pLungs is null.";
        return TPS_ER_FAILURE;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour){
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    int ret = TPS_ER_SUCCESS;

    SubVolume* leftLung = new SubVolume();
    SubVolume* rightLung = new SubVolume();

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->LungSegmentation(*leftLung, *rightLung, pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: LUNGS");
    
    if(!bSeg){
        TPS_LOG_DEV_ERROR<<"LungSegmentation failed";
        ret = TPS_ER_FAILURE;
    }
    else{
        if(0 == lungType){
            MergeVOIs(*leftLung, *rightLung, pLungs);
        }else {
            int x = 0, y = 0, z = 0, offsetX = 0, offsetY = 0, offsetZ = 0;
            char* pData = nullptr;
            if(1 == lungType){//left
                leftLung->GetSize(x, y, z);
                leftLung->GetOffSet(offsetX, offsetY, offsetZ);
                pLungs->SetSize(x, y, z);
                pLungs->SetOffSet(offsetX, offsetY, offsetZ);   
                pData = pLungs->GetData();
                memcpy(pData, leftLung->GetData(), sizeof(char) * x * y * z);
            }
            else {//right
                rightLung->GetSize(x, y, z);
                rightLung->GetOffSet(offsetX, offsetY, offsetZ);
                pLungs->SetSize(x, y, z);
                pLungs->SetOffSet(offsetX, offsetY, offsetZ);        
                pData = pLungs->GetData();
                memcpy(pData, rightLung->GetData(), sizeof(char) * x * y * z);
            }
        }
    }

    delete leftLung; leftLung = nullptr;
    delete rightLung; rightLung = nullptr; 
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::LungsSegmentation end.";
    return ret;
}

void SubVolumeHelperAPI::MergeVOIs(
    SubVolume& volume1, 
    SubVolume& volume2, 
    SubVolume* pVolumeMerged)
{
    if (nullptr == pVolumeMerged){
        TPS_LOG_DEV_ERROR<<"pVolumeMerged is null.";
        return;
    }
    
    int x1 = 0, y1 = 0, z1 = 0, offsetX1 = 0, offsetY1 = 0, offsetZ1 = 0;
    volume1.GetSize(x1, y1, z1);
    volume1.GetOffSet(offsetX1, offsetY1, offsetZ1);

    int x2 = 0, y2 = 0, z2 = 0, offsetX2 = 0, offsetY2 = 0, offsetZ2 = 0;
    volume2.GetSize(x2, y2, z2);
    volume2.GetOffSet(offsetX2, offsetY2, offsetZ2);

    
    int offsetX = min(offsetX1, offsetX2);
    int offsetY = min(offsetY1, offsetY2);
    int offsetZ = min(offsetZ1, offsetZ2);
    int maxX = max(x1 + offsetX1, x2 + offsetX2);
    int maxY = max(y1 + offsetY1, y2 + offsetY2);
    int maxZ = max(z1 + offsetZ1, z2 + offsetZ2);
    pVolumeMerged->SetOffSet(offsetX,offsetY,offsetZ);
    int newSizeX = maxX - offsetX;
    int newSizeY = maxY - offsetY;
    int newSizeZ = maxZ - offsetZ;
    pVolumeMerged->SetSize(newSizeX, newSizeY, newSizeZ);
    char * pData = (char*)pVolumeMerged->GetData();
    memset(pData, 0, sizeof(char)* newSizeX* newSizeY * newSizeZ);

    char * pData1 = (char*)volume1.GetData();
    char * pData2 = (char*)volume2.GetData();
    char v1(0), v2(0);
    int size1 = x1 * y1;
    int size2 = x2 * y2;
    int size = newSizeX * newSizeY;
    int shiftX(0),shiftY(0),shiftZ(0);
    for(int z = 0; z < newSizeZ; ++z){
        for(int y  = 0; y < newSizeY; ++y){
            for(int x = 0; x < newSizeX; ++x){
                v1 = 0; 
                shiftX = x + offsetX - offsetX1;
                shiftY = y + offsetY - offsetY1;
                shiftZ = z + offsetZ - offsetZ1;
                if((shiftX > 0) && ( shiftX < x1) &&
                   (shiftY > 0) && ( shiftY < y1) &&
                   (shiftZ > 0) && ( shiftZ < z1)){
                        v1 = pData1[shiftZ * size1 + shiftY * x1 + shiftX];
                }               
                
                v2 = 0;
                shiftX = x + offsetX - offsetX2;
                shiftY = y + offsetY - offsetY2;
                shiftZ = z + offsetZ - offsetZ2;
                if((shiftX > 0) && ( shiftX < x2) &&
                   (shiftY > 0) && ( shiftY < y2) &&
                   (shiftZ > 0) && ( shiftZ < z2)){
                        v2 = pData2[shiftZ * size2 + shiftY * x2 + shiftX];
                }

                pData[z * size + y * newSizeX + x] = (0 == (v1 + v2)) ? 0 : 1;
            }
        }
    }
}

int SubVolumeHelperAPI::SpinalCordSegmentation(
    const TpsImage3DEntity& image3DData, 
    SubVolume* pOutputMask,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::SpinalCordSegmentation start.";
    if (nullptr == pOutputMask){
        TPS_LOG_DEV_ERROR<<"pOutputMask is null.";
        return TPS_ER_FAILURE;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour)
    {
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    int ret = TPS_ER_SUCCESS;

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;

    if(!pAutoContour->CordSegmentation(*pOutputMask, pProgress))
    {
        TPS_LOG_DEV_ERROR<<"CordSegmentation failed";
        ret = TPS_ER_FAILURE;
    }
    TEST_PERFORMANCE_END("SpinalCordSegmentation: Cord");
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::SpinalCordSegmentation end.";
    return ret;
} 

int SubVolumeHelperAPI::HeartSegmentation(
    const TpsImage3DEntity& image3DData, 
    SubVolume* pOutputMask,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::HeartSegmentation start.";
    if (nullptr == pOutputMask){
        TPS_LOG_DEV_ERROR<<"pOutputMask is null.";
        return TPS_ER_FAILURE;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour){
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    int ret = TPS_ER_SUCCESS;

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->HeartSegmentation(*pOutputMask, pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: Heart");
    if(!bSeg){
        TPS_LOG_DEV_ERROR<<"HeartSegmentation failed";
        ret = TPS_ER_FAILURE;
    }
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::HeartSegmentation end.";
    return ret;
}

int SubVolumeHelperAPI::LiverSegmentation(
    const TpsImage3DEntity& image3DData, 
    SubVolume* pOutputMask,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::LiverSegmentation start.";
    TPS_PRINTF("LiverSegmentation start.");
    if (nullptr == pOutputMask)
    {
        TPS_LOG_DEV_ERROR<<"pOutputMask is null.";
        return TPS_ER_FAILURE;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour)
    {
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    int ret = TPS_ER_SUCCESS;
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->LiverSegmentation(*pOutputMask, pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: Liver");
    if(!bSeg)
    {
        TPS_LOG_DEV_ERROR<<"LiverSegmentation failed";
        ret = TPS_ER_FAILURE;
    }
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::LiverSegmentation end.";
    TPS_PRINTF("LiverSegmentation end.");
    return ret;
}

int SubVolumeHelperAPI::SpleenSegmentation(
    const TpsImage3DEntity& image3DData, 
    SubVolume* pOutputMask,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::SpleenSegmentation start.";
    if (nullptr == pOutputMask){
        TPS_LOG_DEV_ERROR<<"pOutputMask is null.";
        return TPS_ER_FAILURE;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour){
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    int ret = TPS_ER_SUCCESS;

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->SpleenSegmentation(*pOutputMask, pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: Spleen");
    if(!bSeg){
        TPS_LOG_DEV_ERROR<<"SpleenSegmentation failed";
        ret = TPS_ER_FAILURE;
    }
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::SpleenSegmentation end.";
    return ret;
}

int SubVolumeHelperAPI::KidneySegmentation(
    const TpsImage3DEntity& image3DData,
    int kidneyType,
    SubVolume* pKindney,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::KidneySegmentation start.";
    if (nullptr == pKindney){
        TPS_LOG_DEV_ERROR<<"pLungs is null.";
        return TPS_ER_FAILURE;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour){
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    int ret = TPS_ER_SUCCESS;

    SubVolume* leftKidney = new SubVolume();
    SubVolume* rightKidney = new SubVolume();

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->KidneySegmentation(*leftKidney, *rightKidney, pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: Kidney");

    if(!bSeg){
        TPS_LOG_DEV_ERROR<<"KidneySegmentation failed";
        ret = TPS_ER_FAILURE;
    }
    else{
        if(0 == kidneyType){
            MergeVOIs(*leftKidney, *rightKidney, pKindney);
        }else {
            int x = 0, y = 0, z = 0, offsetX = 0, offsetY = 0, offsetZ = 0;
            char* pData = nullptr;
            if(1 == kidneyType){//left
                leftKidney->GetSize(x, y, z);
                leftKidney->GetOffSet(offsetX, offsetY, offsetZ);
                pKindney->SetSize(x, y, z);
                pKindney->SetOffSet(offsetX, offsetY, offsetZ);   
                pData = pKindney->GetData();
                memcpy(pData, leftKidney->GetData(), sizeof(char) * x * y * z);
            }
            else {//right
                rightKidney->GetSize(x, y, z);
                rightKidney->GetOffSet(offsetX, offsetY, offsetZ);
                pKindney->SetSize(x, y, z);
                pKindney->SetOffSet(offsetX, offsetY, offsetZ);        
                pData = pKindney->GetData();
                memcpy(pData, rightKidney->GetData(), sizeof(char) * x * y * z);
            }
        }
    }

    delete leftKidney; leftKidney = nullptr;
    delete rightKidney; rightKidney = nullptr; 

    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::KidneySegmentation end.";
    return ret;
}

int SubVolumeHelperAPI::ParotidSegmentation(
    const TpsImage3DEntity& image3DData,
    int nType,
    SubVolume* pOutputMask,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::ParotidSegmentation start.";
    if (nullptr == pOutputMask){
        TPS_LOG_DEV_ERROR<<"pLungs is null.";
        return TPS_ER_FAILURE;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour){
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    int ret = TPS_ER_SUCCESS;

    SubVolume* left = new SubVolume();
    SubVolume* right = new SubVolume();

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->ParotidSegmentation(*left, *right, pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: Parotid");

    if(!bSeg){
        TPS_LOG_DEV_ERROR<<"ParotidSegmentation failed";
        ret = TPS_ER_FAILURE;
    }
    else{
        if(0 == nType){
            MergeVOIs(*left, *right, pOutputMask);
        }else {
            int x = 0, y = 0, z = 0, offsetX = 0, offsetY = 0, offsetZ = 0;
            char* pData = nullptr;
            if(1 == nType){//left
                left->GetSize(x, y, z);
                left->GetOffSet(offsetX, offsetY, offsetZ);
                pOutputMask->SetSize(x, y, z);
                pOutputMask->SetOffSet(offsetX, offsetY, offsetZ);   
                pData = pOutputMask->GetData();
                memcpy(pData, left->GetData(), sizeof(char) * x * y * z);
            }
            else {//right
                right->GetSize(x, y, z);
                right->GetOffSet(offsetX, offsetY, offsetZ);
                pOutputMask->SetSize(x, y, z);
                pOutputMask->SetOffSet(offsetX, offsetY, offsetZ);        
                pData = pOutputMask->GetData();
                memcpy(pData, right->GetData(), sizeof(char) * x * y * z);
            }
        }
    }

    delete left; left = nullptr;
    delete right; right = nullptr; 
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::ParotidSegmentation end.";
    return ret;
}


int SubVolumeHelperAPI::EyeballSegmentation(
    const TpsImage3DEntity& image3DData, 
    int EyeType, 
    SubVolume* pOutputMask,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::EyeballSegmentation start.";
    if (nullptr == pOutputMask){
        TPS_LOG_DEV_ERROR<<"pOutputMask is null.";
        return TPS_ER_FAILURE;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour){
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    int ret = TPS_ER_SUCCESS;

    SubVolume* left = new SubVolume();
    SubVolume* right = new SubVolume();

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->EyeballSegmentation(*left, *right, pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: Eyeball");

    if(!bSeg){
        TPS_LOG_DEV_ERROR<<"EyeballSegmentation failed";
        ret = TPS_ER_FAILURE;
    }
    else{
        if(0 == EyeType){
            MergeVOIs(*left, *right, pOutputMask);
        }else {
            int x = 0, y = 0, z = 0, offsetX = 0, offsetY = 0, offsetZ = 0;
            char* pData = nullptr;
            if(1 == EyeType){//left
                left->GetSize(x, y, z);
                left->GetOffSet(offsetX, offsetY, offsetZ);
                pOutputMask->SetSize(x, y, z);
                pOutputMask->SetOffSet(offsetX, offsetY, offsetZ);   
                pData = pOutputMask->GetData();
                memcpy(pData, left->GetData(), sizeof(char) * x * y * z);
            }
            else {//right
                right->GetSize(x, y, z);
                right->GetOffSet(offsetX, offsetY, offsetZ);
                pOutputMask->SetSize(x, y, z);
                pOutputMask->SetOffSet(offsetX, offsetY, offsetZ);        
                pData = pOutputMask->GetData();
                memcpy(pData, right->GetData(), sizeof(char) * x * y * z);
            }
        }
    }

    delete left; left = nullptr;
    delete right; right = nullptr; 
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::EyeballSegmentation end.";
    return ret;
}

int SubVolumeHelperAPI::BrainstemSegmentation(
    const TpsImage3DEntity& image3DData, 
    SubVolume* pOutputMask,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::BrainstemSegmentation start.";
    if (nullptr == pOutputMask){
        TPS_LOG_DEV_ERROR<<"pOutputMask is null.";
        return TPS_ER_FAILURE;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour){
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    int ret = TPS_ER_SUCCESS;

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->BrainstemSegmentation(*pOutputMask, pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: Brainstem");
    if(!bSeg){
        TPS_LOG_DEV_ERROR<<"BrainstemSegmentation failed";
        ret = TPS_ER_FAILURE;
    }
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::BrainstemSegmentation end.";
    return ret;
}

int SubVolumeHelperAPI::MandibleSegmentation(
    const TpsImage3DEntity& image3DData, 
    SubVolume* pOutputMask,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::MandibleSegmentation start.";
    if (nullptr == pOutputMask){
        TPS_LOG_DEV_ERROR<<"pOutputMask is null.";
        return TPS_ER_FAILURE;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour){
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    int ret = TPS_ER_SUCCESS;

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->MandibleSegmentation(*pOutputMask, pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: Mandible");
    if(!bSeg){
        TPS_LOG_DEV_ERROR<<"MandibleSegmentation failed";
        ret = TPS_ER_FAILURE;
    }
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::MandibleSegmentation end.";
    return ret;
}

int SubVolumeHelperAPI::FemoralheadSegmentation(
    const TpsImage3DEntity& image3DData, 
    SubVolume* pOutputMask,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::FemoralheadSegmentation start.";
    if (nullptr == pOutputMask){
        TPS_LOG_DEV_ERROR<<"pOutputMask is null.";
        return TPS_ER_FAILURE;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour){
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    int ret = TPS_ER_SUCCESS;

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->FemoralheadSegmentation(*pOutputMask, pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: Femoralhead");
    if(!bSeg){
        TPS_LOG_DEV_ERROR<<"FemoralheadSegmentation failed";
        ret = TPS_ER_FAILURE;
    }

    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::FemoralheadSegmentation end.";
    return ret;
}

int SubVolumeHelperAPI::BrainSegmentation(
    const TpsImage3DEntity& image3DData, 
    SubVolume* pOutputMask,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::BrainSegmentation start.";
    if (nullptr == pOutputMask){
        TPS_LOG_DEV_ERROR<<"pOutputMask is null.";
        return TPS_ER_FAILURE;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour){
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    int ret = TPS_ER_SUCCESS;

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->BrainSegmentation(*pOutputMask, pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: Brain");
    if(!bSeg){
        TPS_LOG_DEV_ERROR<<"BrainSegmentation failed";
        TPS_PRINTF_DEBUG("\nBrainSegmentation failed.\n");
        ret = TPS_ER_FAILURE;
    }
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::BrainSegmentation end.";
    return ret;
}

int SubVolumeHelperAPI::RectumSegmentation(
    const TpsImage3DEntity& image3DData, 
    SubVolume* pOutputMask,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::RectumSegmentation start.";
    if (nullptr == pOutputMask){
        TPS_LOG_DEV_ERROR<<"pOutputMask is null.";
        return TPS_ER_FAILURE;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour){
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    int ret = TPS_ER_SUCCESS;

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->RectumSegmentation(*pOutputMask, pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: Rectum");
    int sizeX=0, sizeY=0, sizeZ=0;
    pOutputMask->GetSize(sizeX,sizeY,sizeZ);
    int offSetX=0, offSetY=0, offSetZ=0;
    pOutputMask->GetOffSet(offSetX,offSetY,offSetZ);
    if(!bSeg || 
        sizeX < 0 ||
        sizeY < 0 ||
        sizeZ < 0 ||
        offSetX > 2000 ||
        offSetY > 2000 ||
        offSetZ > 2000){
        TPS_LOG_DEV_ERROR<<"RectumSegmentation failed";
        ret = TPS_ER_FAILURE;
    }

    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::RectumSegmentation end.";
    return ret;
}

int SubVolumeHelperAPI::ProstateSegmentation(
    const TpsImage3DEntity& image3DData, 
    bool bInputRectumMask, 
    SubVolume* pSubRectumMask,
    SubVolume* pOutputMask,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::ProstateSegmentation start.";
    if (nullptr == pOutputMask){
        TPS_LOG_DEV_ERROR<<"pOutputMask is null.";
        return TPS_ER_FAILURE;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour){
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    int ret = TPS_ER_SUCCESS;

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->ProstateSegmentation(
        bInputRectumMask, 
        *pSubRectumMask, 
        *pOutputMask, 
        pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: Prostate");

    int sizeX=0, sizeY=0, sizeZ=0;
    pOutputMask->GetSize(sizeX,sizeY,sizeZ);
    int offSetX=0, offSetY=0, offSetZ=0;
    pOutputMask->GetOffSet(offSetX,offSetY,offSetZ);
    if(!bSeg || 
        sizeX < 0 ||
        sizeY < 0 ||
        sizeZ < 0 ||
        offSetX > 2000 ||
        offSetY > 2000 ||
        offSetZ > 2000){
        TPS_LOG_DEV_ERROR<<"ProstateSegmentation failed";
        ret = TPS_ER_FAILURE;
    }
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::ProstateSegmentation end.";
    return ret;
}


int SubVolumeHelperAPI::JawJointSegmentation(
    const TpsImage3DEntity& image3DData,
    SubVolume* pOutputMask,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::JawJointSegmentation start.";
    if (nullptr == pOutputMask)
    {
        TPS_LOG_DEV_ERROR<<"pOutputMask is null.";
        return TPS_ER_FAILURE;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour)
    {
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    int ret = TPS_ER_SUCCESS;

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->JawJointSegmentation(*pOutputMask, pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: JawJoint");
    if(!bSeg){
        TPS_LOG_DEV_ERROR<<"JawJointSegmentation failed";
        ret = TPS_ER_FAILURE;
    }
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::JawJointSegmentation end.";
    return ret;
}

int SubVolumeHelperAPI::PancreasSegmentation(
    const TpsImage3DEntity& image3DData, 
    SubVolume* pOutputMask,
    Mcsf::IProgress* pProgress)
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::PancreasSegmentation start.";
    int ret = TPS_ER_FAILURE;
    if (nullptr == pOutputMask)
    {
        TPS_LOG_DEV_ERROR<<"pOutputMask is null.";
        return ret;
    }

    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour)
    {
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return ret;
    }

    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    bool bSeg = pAutoContour->PancreasSegmentation(*pOutputMask, pProgress);
    TEST_PERFORMANCE_END("Auto Segmentation: Pancreas");
    if(!bSeg)
    {
        TPS_LOG_DEV_ERROR<<"PancreasSegmentation failed";
    }
    else 
    {
        ret = TPS_ER_SUCCESS;
    }

    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::PancreasSegmentation end.";
    return ret;
}

float SubVolumeHelperAPI::AutoRemoveTable(const TpsImage3DEntity& image3DData) 
{
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::AutoRemoveTable start.";
    int tableTop;
    float result = 0.50f;
    Mcsf::McsfAlgoAutoContour* pAutoContour = image3DData.GetAutoContour();
    if (nullptr == pAutoContour)
    {
        TPS_LOG_DEV_ERROR<<"pAutoContour is null.";
        return TPS_ER_FAILURE;
    }

    RtImage3DHeader* header = nullptr;
    if(!image3DData.GetImage3DHeader(header)) {
        TPS_LOG_DEV_ERROR<<"Failed to get current image 3d header.";
        return false;
    }
    if(header == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get current image 3d header.";
        return false;
    }

    pAutoContour->BedBoardDetection(tableTop);
    if(header->m_iYDim > 0)
    {
        result = (float)tableTop / (float)header->m_iYDim; 
    }
    TPS_LOG_DEV_INFO<<"SubVolumeHelperAPI::AutoRemoveTable end.";
    return result;
}

TPS_END_NAMESPACE
