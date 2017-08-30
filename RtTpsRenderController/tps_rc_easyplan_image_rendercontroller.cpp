//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_rc_easyplan_image_rendercontroller.cpp 
///  \brief   easy plan image
///
///  \version 1.0
///  \date  2014/10/16  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderController/tps_rc_easyplan_image_rendercontroller.h"
#include "tps_logger.h"

//ZhengHe
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "Mcsf3DArithmetic/mcsf_3d_point2f.h"
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"

//TPS
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_measuregraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_measuredistancestrategy.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_measureanglestrategy.h"
#include "RtTpsRenderLibrary/tps_rl_editsetuppoigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_setupptcrosshairstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_easyplandrrrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setpoirenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"

#include "RtTpsFramework/tps_fw_displaycache.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"
#include "RtTpsFramework/tps_fw_rendercontrollerbase.h"



#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"

using namespace Mcsf::MedViewer3D;
TPS_BEGIN_NAMESPACE

TpsEasyPlanImageRenderController::TpsEasyPlanImageRenderController(WINDOW_TYPE type)
:   TpsRenderController(type),
    mDRRStrategy(nullptr),
    mEasyPlanMeasureDistanceStrategy(nullptr),
    mEasyPlanMeasureAngleStrategy(nullptr),
    mEasyPlanSetIsoCenterStrategy(nullptr)
{

}

TpsEasyPlanImageRenderController::~TpsEasyPlanImageRenderController()
{

}

bool TpsEasyPlanImageRenderController::SetupScene_i()
{
    try
    {
        mDRRStrategy.reset(new TpsEasyPlanDrrRenderStrategy());
        RegisterRenderStrategy_i(ClsView::IMAGE, mDRRStrategy);

        mEasyPlanMeasureDistanceStrategy.reset(new EasyPlanMeasureDistanceStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mEasyPlanMeasureDistanceStrategy);

        mEasyPlanMeasureAngleStrategy.reset(new EasyPlanMeasureAngleStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mEasyPlanMeasureAngleStrategy);

        mEasyPlanSetIsoCenterStrategy.reset(new TpsEasyPlanSetPoiRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mEasyPlanSetIsoCenterStrategy);

        return true;
    }
    catch (std::exception &ex)
    {
        TPS_LOG_DEV_ERROR << "Failed to setup scene:" << ex.what();
        return false;
    }
}

void TpsEasyPlanImageRenderController::SetPixelSpaceData(float mImagePlanPixelSpacingX, float mImagePlanPixelSpacingY)
{
    mEasyPlanSetIsoCenterStrategy->SetPixelSpaceData(mImagePlanPixelSpacingX, mImagePlanPixelSpacingY);
}

bool TpsEasyPlanImageRenderController::GetCurrentPixelSpacingIn10MM(double& spacingX, double& spacingY)
{
    spacingX = 0.0;
    spacingY = 0.0;
    double spacingx, spacingy;

    tps::DISPLAY_SIZE easyPlanBEVDiplaySize;
    this->GetDisplaySize(easyPlanBEVDiplaySize);
    if(easyPlanBEVDiplaySize.width == 0 || easyPlanBEVDiplaySize.height == 0) {
        TPS_LOG_DEV_ERROR << "TpsRenderProxy::GetPixelSpacingIn10MMOnBEV failed because display size contains 0.";
        return false;
    }

    // when there is no image, the background is equal to 40cm*40cm
    double const emptyImageWidth = 40.0; // in cm
    double const emptyImageHeight = 40.0; // in cm

    std::string beamUid;
    if (!GetEasyPlanActiveBeamUid(beamUid))
    {
        TPS_LOG_DEV_ERROR << "TpsRenderProxy::GetEasyPlanActiveBeamDrrImageUID failed.";
        return false;
    }
    TpsImage2DEntity *image2DData = nullptr;
    bool iRet = mDataRepository->GetImageDataManager()->Get2DImageDataByBeamUID(beamUid, image2DData);
    //beam没有关联的drr
    if (!iRet) 
    {
        spacingX = easyPlanBEVDiplaySize.width / emptyImageWidth;
        spacingY = easyPlanBEVDiplaySize.height / emptyImageHeight;
        return true;
    }
    std::string imageUid;
    image2DData->GetUID(imageUid);
    int imageWidth;
    int imageHeight;
    float convertedStartX = 0.0f;
    float convertedStartY = 0.0f;
    float convertedStopX = 0.0f;
    float convertedStopY = 0.0f;
    Mcsf::MedViewer3D::Point2D ptScreen;
    Mcsf::MedViewer3D::Point3D ptOut3D;
    ptScreen.x = 0.5;
    ptScreen.y = 0.5;
    this->TransformPoint(ptScreen, ptOut3D);
    convertedStartX = (float)ptOut3D.x;
    convertedStartY = (float)ptOut3D.y;
    ptScreen.x = 0.5 + 1.0/easyPlanBEVDiplaySize.width;
    ptScreen.y = 0.5 + 1.0/easyPlanBEVDiplaySize.height;
    this->TransformPoint(ptScreen, ptOut3D);
    convertedStopX = (float)ptOut3D.x;
    convertedStopY = (float)ptOut3D.y;
    image2DData->GetImageSize(imageWidth, imageHeight);
    float startX_InImage = convertedStartX * imageWidth;
    float startY_InImage = convertedStartY * imageHeight;
    float cursorX_InImage = convertedStopX * imageWidth;
    float cursorY_InImage = convertedStopY * imageHeight;

    std::pair<float, float> imagePlanePixalSpacing;
    imagePlanePixalSpacing.first = image2DData->get()->get_firstimageplanepixelspacing();
    imagePlanePixalSpacing.second = image2DData->get()->get_secondimageplanepixelspacing();

    double startX_InPatient = startX_InImage * imagePlanePixalSpacing.first;
    double startY_InPatient = startY_InImage * imagePlanePixalSpacing.second;
    double cursorX_InPatient = cursorX_InImage * imagePlanePixalSpacing.first;
    double cursorY_InPatient = cursorY_InImage * imagePlanePixalSpacing.second;

    double x_dis_InPatient = (cursorX_InPatient - startX_InPatient) * (cursorX_InPatient - startX_InPatient);
    double y_dis_InPatient = (cursorY_InPatient - startY_InPatient) * (cursorY_InPatient - startY_InPatient);

    spacingx = 10.0 / sqrt(x_dis_InPatient);
    spacingy = 10.0 / sqrt(y_dis_InPatient);

    float zoomFactor = 1.0f;
    if(this->GetDrrGoZoomFactor(imageUid, zoomFactor)){
        if(zoomFactor >= 0.0001f) {
            spacingx /= zoomFactor;
            spacingy /= zoomFactor;
        }
    }

    spacingX = spacingx;
    spacingY = spacingy;
    return true;
}


bool TpsEasyPlanImageRenderController::ResetDataSource_i( DATA_SOURCE_ID id )
{
    bool bRes = false;
    switch (id)
    {
        case IMAGE:
            break;
        case PLAN:
            bRes = ResetPlan_i();
            break;
        default:
            TPS_LOG_DEV_ERROR << "Unrecognized data source id: " << id;
            break;
    }
    return bRes;
}

bool TpsEasyPlanImageRenderController::ResetPlan_i()
{

    std::string planUID = mDataSourceMap[PLAN];

    IGraphicObjectPtr goPOI = nullptr;
    if (!CheckGraphicObject_i(planUID, GO_TYPE_POI_COLLECTION, goPOI))
    {
        TPS_LOG_DEV_ERROR << "Failed to check normal group collection of plan: " << planUID;
        return false;
    }

    //check normal group collection graphic object
    IGraphicObjectPtr goNormGroup = nullptr;
    if (!CheckGraphicObject_i(planUID, GO_TYPE_NORMALGROUP_COLLECTION, goNormGroup))
    {
        TPS_LOG_DEV_ERROR << "Failed to check normal group collection of plan: " << planUID;
        return false;
    }

    IGraphicObjectPtr goDRRCollection = nullptr;
    if (!CheckGraphicObject_i(planUID, GO_TYPE_RT_IMAGE_COLLECTION, goDRRCollection))
    {
        TPS_LOG_DEV_ERROR << "Failed to check Drr collection graphic object of image: "<<goDRRCollection;
        return false;
    }

    auto goNGCollection = std::dynamic_pointer_cast<NormalGroupCollection>(goNormGroup);
    auto goPOICollection = std::dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(goPOI);
    auto drrCollection = std::dynamic_pointer_cast<EasyPlanDrrGraphicObjectCollection>(goDRRCollection);

    if (goPOICollection == nullptr || goNGCollection == nullptr || nullptr == drrCollection)
    {
        TPS_LOG_DEV_ERROR << "Failed to dynamic cast IGraphicObjects.";
        return false;
    }

    mDRRStrategy->SetDRRGOCollection(drrCollection);
    mDRRStrategy->SetNGGOCollection(goNGCollection);
    mEasyPlanSetIsoCenterStrategy->SetPoiGraphicObjectCollection(goPOICollection);
    return true;
}

void TpsEasyPlanImageRenderController::RemoveDataSource_i(DATA_SOURCE_ID id)
{
    switch (id)
    {
        case IMAGE:
            break;
        case PLAN:
            mEasyPlanSetIsoCenterStrategy->SetPoiGraphicObjectCollection(nullptr);
            break;
        default:
            break;
    }
}

bool TpsEasyPlanImageRenderController::TransformPoint(const Point2D& ptIn, Point3D& ptOut)
{
    int drrWidth = 0;
    int drrHeight = 0;
    mView->GetDisplaySize(drrWidth,drrHeight);
    if (drrWidth ==0 || drrHeight == 0)
    {
        TPS_LOG_DEV_ERROR << "mDrrWidth ==0 || mDrrHeight == 0";
        return false;
    }
    double dScaleRatio = (drrWidth >= drrHeight) ? (double)drrWidth / (double)drrHeight :
        (double)drrHeight / (double)drrWidth;
    Mcsf::MedViewer3D::Point2D ptInput;
    if (drrWidth > drrHeight)
    {
        ptInput.x = (ptIn.x - 0.5) * dScaleRatio;
        ptInput.y = ptIn.y - 0.5;

    }
    else
    {
        ptInput.x = ptIn.x-0.5;
        ptInput.y = (ptIn.y - 0.5) * dScaleRatio;
    }
    Mcsf::Point3f drrModelCoorPoint((float)ptInput.x,(float)ptInput.y,1.0f);
    Mcsf::Point3f imageModelCoorPoint(0.0f,0.0f,0.0f);
    Mcsf::Matrix3f drrModel2ImageModelMatrix;

    std::string beamUid;
    if (!GetEasyPlanActiveBeamUid(beamUid))
    {
        TPS_LOG_DEV_ERROR << "TpsRenderProxy::GetEasyPlanActiveBeamDrrImageUID failed.";
        return false;
    }
    TpsImage2DEntity *image2DData = nullptr;
    bool iRet = mDataRepository->GetImageDataManager()->Get2DImageDataByBeamUID(beamUid, image2DData);
    //beam没有关联的drr
    if (!iRet) return true;

    std::string imageUID;
    image2DData->GetUID(imageUID);

    auto easyPlanDrrGO = GetEasyPlanDrrGoByImageUid(imageUID);

    if (nullptr == easyPlanDrrGO)
    {
        TPS_LOG_DEV_ERROR << "failed to dynamic cast to easyplan drr go in TpsEasyPlanImageRenderController::TransformPoint()";
        return false;
    }
    easyPlanDrrGO->GetDrrModel2ImageModel(&drrModel2ImageModelMatrix);
    imageModelCoorPoint = drrModel2ImageModelMatrix*drrModelCoorPoint;

    ptOut.x = imageModelCoorPoint.GetX();
    ptOut.y = imageModelCoorPoint.GetY();
    return true;
}

bool TpsEasyPlanImageRenderController::GetDrrGoZoomFactor(std::string imageUid, float& zoomFactor)
{
    auto easyPlanDrrGO = GetEasyPlanDrrGoByImageUid(imageUid);
    if (nullptr == easyPlanDrrGO)
    {
        TPS_LOG_DEV_ERROR << "failed to dynamic cast to easyplan drr go in TpsEasyPlanSetZoomOperation::ModifyGraphicObject()";
        return false;
    }
    zoomFactor = easyPlanDrrGO->GetZoomFactor();
    return true;
}

bool TpsEasyPlanImageRenderController::GetEasyPlanActiveBeamUid(std::string& beamUid)
{
    beamUid = "";

    std::string planUID;
    if (!this->GetDataSource(PLAN, planUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
        return false;
    }

    auto ngCollection = mModelWarehouse->GetModelObject(
        planUID + "|" + GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    auto goNormGroupCollection = std::dynamic_pointer_cast<NormalGroupCollection>(ngCollection);
    if (goNormGroupCollection == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed get normgroup collection.";
        return false;
    }

    if (nullptr == goNormGroupCollection)
    {
        TPS_LOG_DEV_ERROR << "Failed to get normal group collection";
        return false;
    }
    std::string activeNGUID = goNormGroupCollection->GetActiveNormalGroupUID();
    if (activeNGUID.empty()) return nullptr;
    auto activeNG = goNormGroupCollection->GetNormalGroup(activeNGUID);
    if (nullptr == activeNG)
    {
        TPS_LOG_DEV_ERROR << "There is no active normal group graphic object.";
        return false;
    }
    std::shared_ptr<TpsBeamGraphicObject> beamGo = activeNG->GetActiveBeamGo();
    if (nullptr == beamGo)
    {
        TPS_LOG_DEV_ERROR << "There is no active beam graphic object.";
        return false;
    }

    beamUid = beamGo->GetUID();
    return true;
}

std::shared_ptr<EasyPlanDrrGraphicObejct> TpsEasyPlanImageRenderController::GetEasyPlanDrrGoByImageUid(const std::string& imageUid)
{
    std::string planUID;
    if (!this->GetDataSource(PLAN, planUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
        return nullptr;
    }
    auto go = mModelWarehouse->GetModelObject(planUID + "|" 
        + GOTypeHelper::ToString(GO_TYPE_RT_IMAGE_COLLECTION));
    if(nullptr == go) {
        TPS_LOG_DEV_ERROR<<"failed to get easyplan drr go collection ! plan uid:"
            <<planUID;
        return nullptr;
    }
    auto drrCollection = std::dynamic_pointer_cast<EasyPlanDrrGraphicObjectCollection>(go);
    return drrCollection->GetDrr(imageUid);
}


void TpsEasyPlanImageRenderController::ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid)
{
    if (id == PLAN)
    {
        RemoveGraphicObject_i(uid, GO_TYPE_RT_IMAGE_COLLECTION);
        RemoveGraphicObject_i(uid, GO_TYPE_POI_COLLECTION);
        RemoveGraphicObject_i(uid, GO_TYPE_NORMALGROUP_COLLECTION);
    }
}
TPS_END_NAMESPACE
