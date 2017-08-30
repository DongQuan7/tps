////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_updatefusionregionoperation.cpp
/// 
///  \brief class UpdateFusionRegion implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/07/08
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_updatefusionregionoperation.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_fusionsecondarydatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_dosegridboundarygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

UpdateFusionRegionOperation::UpdateFusionRegionOperation(
FusionRegionUpdateType refreshType/* = AllRegion*/) : mRefreshType(refreshType){

}

UpdateFusionRegionOperation::~UpdateFusionRegionOperation(){

}

bool UpdateFusionRegionOperation::ModifyGraphicObject() const{
    auto manager = mDataRepository->GetFusionSecondaryDataManager();
    std::string fixedImageUID = manager->GetFixedImageUID();
    std::string floatImageUID = manager->GetFloatImageUID();
    if (fixedImageUID.empty() || floatImageUID.empty()){
        TPS_LOG_DEV_ERROR<<"Fixed image or float image hasn't set.";
        return false;
    }

    TpsImage3DEntity* fixedImage =nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(
        fixedImageUID, &fixedImage) || fixedImage == nullptr){
        TPS_LOG_DEV_ERROR<<"The fixed image is empty.";
        return false;
    }

    TpsImage3DEntity* floatImage =nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(
        floatImageUID, &floatImage) || floatImage == nullptr){
        TPS_LOG_DEV_ERROR<<"The float image is empty.";
        return false;
    }

    std::string key = COMMON_BASE + "|" + GOTypeHelper::ToString(GO_TYPE_FUSION_REGION_FIXED);
    auto go = mModelWarehouse->GetModelObject(key);
    auto fixedRegion = std::dynamic_pointer_cast<TpsDoseGridBoundaryGraphicObject>(go);
    if (fixedRegion == nullptr){
        TPS_LOG_DEV_ERROR<<"The fixed region graphic object does not exist.";
        return false;
    }

    key = COMMON_BASE + "|" + GOTypeHelper::ToString(GO_TYPE_FUSION_REGION_FLOAT);
    go = mModelWarehouse->GetModelObject(key);
    auto floatRegion = std::dynamic_pointer_cast<TpsDoseGridBoundaryGraphicObject>(go);
    if (floatRegion == nullptr){
        TPS_LOG_DEV_ERROR<<"The float region graphic object does not exist.";
        return false;
    }

    bool visible;
    const int* regionBuffer;
    manager->GetRegion(regionBuffer);
    if (mRefreshType != OnlyFloatRegion){
        float color[3] = {1.0f, 0.2f, 1.0f};
        Mcsf::Point3f fixedPt1(regionBuffer[0]-0.5f, regionBuffer[2]-0.5f, regionBuffer[4]-0.5f);
        Mcsf::Point3f fixedPt2(regionBuffer[1]+0.5f, regionBuffer[3]+0.5f, regionBuffer[5]+0.5f);
        visible = manager->IsRegionVisible(FUSION_ROLE_FIXED);
        Mcsf::Matrix4f matFixedV2P;
        fixedImage->GetVolumeToPatientMatrix(matFixedV2P);
        fixedPt1 = matFixedV2P.Transform(fixedPt1);
        fixedPt2 = matFixedV2P.Transform(fixedPt2);
        fixedRegion->SetLeftBottomPoint(TpsArithmeticConverter::ConvertToPoint3D(fixedPt1));
        fixedRegion->SetRightTopPoint(TpsArithmeticConverter::ConvertToPoint3D(fixedPt2));
        fixedRegion->SetVisible(visible);
        fixedRegion->SetColor(color);
        fixedRegion->SetDirty(true);
    }
    if (mRefreshType != OnlyFixedRegion){
        float color[3] = {0.2f, 1.0f, 1.0f};
        Mcsf::Point3f floatPt1(regionBuffer[6]-0.5f, regionBuffer[8]-0.5f, regionBuffer[10]-0.5f);
        Mcsf::Point3f floatPt2(regionBuffer[7]+0.5f, regionBuffer[9]+0.5f, regionBuffer[11]+0.5f);
        visible = manager->IsRegionVisible(FUSION_ROLE_FLOAT);
        Mcsf::Matrix4f matFloatV2P;
        floatImage->GetVolumeToPatientMatrix(matFloatV2P);
        floatPt1 = matFloatV2P.Transform(floatPt1);
        floatPt2 = matFloatV2P.Transform(floatPt2);
        floatRegion->SetLeftBottomPoint(TpsArithmeticConverter::ConvertToPoint3D(floatPt1));
        floatRegion->SetRightTopPoint(TpsArithmeticConverter::ConvertToPoint3D(floatPt2));
        floatRegion->SetVisible(visible);
        floatRegion->SetColor(color);
        floatRegion->SetDirty(true);
    }
    return true;
}

TPS_END_NAMESPACE  // end namespace tps
