////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rc_bevrendercontroller.cpp
/// 
///  \brief class TpsBEVRenderController implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/01/20
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderController/tps_rc_bevrendercontroller.h"

//ZhengHe
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

//TPS
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_displaycache.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

//render strategy
//#include "RtTpsRenderLibrary/tps_rl_shadedvoistrategy.h"
#include "RtTpsRenderLibrary/tps_rl_voi3d_renderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_applicator_render_strategy.h"
#include "RtTpsRenderLibrary/tps_rl_drr_render_strategy.h"
//#include "RtTpsRenderLibrary/tps_rl_bev_dose_shaded_strategy.h"         // for BevDoseShadedRenderStrategy
#include "RtTpsRenderLibrary/tps_rl_fluence_map_render_strategy.h"
#include "RtTpsRenderLibrary/tps_rl_bev_window_tansparency_render_strategy.h"

//graphic object
#include "RtTpsRenderLibrary/tps_rl_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_statecontrol_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_ct2densitygocollection.h"
#include "RtTpsRenderLibrary/tps_rl_drrsettinggocollection.h"
#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobjectcollection.h"
#include "tps_logger.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE
TpsBEVRenderController::TpsBEVRenderController(WINDOW_TYPE type) :
    TpsRenderController(type),
    mFluenceMapRenderStrategy(nullptr),
    mDrrRenderStrategy(nullptr),
    mTransparencyRenderStrategy(nullptr)
{

}

TpsBEVRenderController::~TpsBEVRenderController()
{

}

bool TpsBEVRenderController::SetupScene_i()
{
    try
    {
        mDrrRenderStrategy.reset(new DrrRenderStrategy());
        RegisterRenderStrategy_i(ClsView::IMAGE, mDrrRenderStrategy);

        mFluenceMapRenderStrategy.reset(new FluenceMapRenderStrategy());
        RegisterRenderStrategy_i(ClsView::IMAGE, mFluenceMapRenderStrategy);

        mTransparencyRenderStrategy.reset(new BevWindowTransparencyRenderStrategy());
        RegisterRenderStrategy_i(ClsView::IMAGE, mTransparencyRenderStrategy);

        mVOIRenderStrategy.reset(new VOI3DRenderStrategy(mWindowType));
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mVOIRenderStrategy);

        mApplicatorRenderStrategy.reset(new ApplicatorRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mApplicatorRenderStrategy);
        return true;
    }
    catch (std::exception &ex)
    {
        TPS_LOG_DEV_ERROR << "Failed to setup scene:" << ex.what();
        return false;
    }
}

bool TpsBEVRenderController::ResetDataSource_i(DATA_SOURCE_ID id)
{

    bool bRes = false;
    switch (id)
    {
        case IMAGE:
            bRes = ResetImage_i();
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

bool TpsBEVRenderController::ResetImage_i()
{
    std::string imageUID = mDataSourceMap[IMAGE];

    IGraphicObjectPtr goVolume = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_VOLUME, goVolume))
    {
        TPS_LOG_DEV_ERROR << "Failed to check volume graphic object of image: " << imageUID;
        return false;
    }

    IGraphicObjectPtr goDRR = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_DRR, goDRR))
    {
        TPS_LOG_DEV_ERROR << "Failed to check DRR graphic object of image: " << imageUID;
        return false;
    }

    IGraphicObjectPtr goCt2DensityCollection = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_CT2DENSITYCollection, goCt2DensityCollection))
    {
        TPS_LOG_DEV_ERROR << "Failed to check Ct2Density graphic object of image: " << imageUID;
        return false;
    }

    IGraphicObjectPtr goStateControl = nullptr;
    if (!CheckGraphicObject_i(COMMON_BASE, GO_TYPE_BEV_DISPLAY_CONTROL, goStateControl))
    {
        TPS_LOG_DEV_ERROR << "Failed to get state control GO!";
        return false;
    }

    std::string mainPhaseImageUID = mDataRepository->GetImageDataManager(
        )->GetOperationImageUid(imageUID);

    //check voi graphic object collection & VOI Rendering Cache graphic object
    IGraphicObjectPtr goVOICollection = nullptr;
    if (!CheckGraphicObject_i(mainPhaseImageUID, GO_TYPE_VOI_COLLECTION, goVOICollection))
    {
        TPS_LOG_DEV_ERROR << "Failed to check VOI graphic object collection of image: " << mainPhaseImageUID;
        return false;
    }

    IGraphicObjectPtr goShadedSurface = nullptr;
    if (!CheckGraphicObject_i(mainPhaseImageUID, GO_TYPE_SHADEDSURFCE, goShadedSurface))
    {
        TPS_LOG_DEV_ERROR << "Failed to check shaded surface go:";
        return false;
    }


    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    auto drr = std::dynamic_pointer_cast<DrrGraphicObejct>(goDRR);
    auto stateControl = std::dynamic_pointer_cast<StateControlGraphicObject>(goStateControl);
    auto ctMap = std::dynamic_pointer_cast<Ct2DensityGraphicObejctCollection>(goCt2DensityCollection);
    auto voi = std::dynamic_pointer_cast<ShadedVoiGraphicObject>(goShadedSurface);
    auto voiCollection = std::dynamic_pointer_cast<TpsVOIGOCollection>(goVOICollection);
    if(volume == nullptr || drr == nullptr || voi == nullptr
        || ctMap == nullptr || stateControl == nullptr || voiCollection == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to dynamic cast IGraphicObjects.";
        return false;
    }


    mDrrRenderStrategy->SetVolumeGO(volume);
    mDrrRenderStrategy->SetDrrGO(drr);
    mDrrRenderStrategy->SetCt2DensityGOColletion(ctMap);
    mDrrRenderStrategy->SetStateControlGO(stateControl);

    mApplicatorRenderStrategy->SetVolumeGraphicObj(volume);
    mFluenceMapRenderStrategy->SetVolumeGO(volume);
    mFluenceMapRenderStrategy->SetStateControlGO(stateControl);

    mTransparencyRenderStrategy->SetVolumeGO(volume);
    mTransparencyRenderStrategy->SetShadedVOIGO(voi);
    mTransparencyRenderStrategy->SetVOICollectionGO(voiCollection);

    mVOIRenderStrategy->SetStateControlGO(stateControl);
    mVOIRenderStrategy->SetVolumeGO(volume);
    mVOIRenderStrategy->SetVOICollectionGO(voiCollection);

    return true;
}

bool TpsBEVRenderController::ResetPlan_i()
{
    std::string planUID = mDataSourceMap[PLAN];

    //check normal group collection graphic object
    IGraphicObjectPtr go = nullptr;
    if (!CheckGraphicObject_i(planUID, GO_TYPE_NORMALGROUP_COLLECTION, go))
    {
        TPS_LOG_DEV_ERROR << "Failed to check normal group collection of plan: " << planUID;
        return false;
    }

    IGraphicObjectPtr goCamera = nullptr;
    if (!CheckGraphicObject_i(planUID, GO_TYPE_BEV_CAMERA, goCamera, true))
    {
        TPS_LOG_DEV_ERROR << "Failed ti get bev camera";
        return false;
    }

    IGraphicObjectPtr doseMeshGo = nullptr;
    if (!CheckGraphicObject_i(planUID, GO_TYPE_DOSE_MESH_COLLECTION, doseMeshGo))
    {
        TPS_LOG_DEV_ERROR << "Failed to check DoseMesh graphic object of plan: " << planUID;
        return false;
    }

    IGraphicObjectPtr drrsettingGoCollection = nullptr;
    if (!CheckGraphicObject_i(planUID, GO_TYPE_DRRSETTING_COLLECTION, drrsettingGoCollection))
    {
        TPS_LOG_DEV_ERROR << "Failed to check drr setting graphic object collection of plan: " << planUID;
        return false;
    }

    auto goNGCollection = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    auto camera = std::dynamic_pointer_cast<CameraGraphicObject>(goCamera);
    auto spDoseMeshGO = std::dynamic_pointer_cast<DoseMeshGraphicObjectCollection>(doseMeshGo);
    auto goDrrsettingCollection = std::dynamic_pointer_cast<DRRSettingGraphicObjectCollection>(drrsettingGoCollection);

    mDrrRenderStrategy->SetNormalGroupCollectionGO(goNGCollection);
    mApplicatorRenderStrategy->SetNormalGroupCollection(goNGCollection);
    mFluenceMapRenderStrategy->SetNormalGroupCollectionGO(goNGCollection);
    mDrrRenderStrategy->SetCameraGO(camera);
    mDrrRenderStrategy->SetDrrSettingGoColletion(goDrrsettingCollection);
    mTransparencyRenderStrategy->SetCameraGO(camera);
    mTransparencyRenderStrategy->SetDoseMeshCollectionGO(spDoseMeshGO);
    mVOIRenderStrategy->SetCameraGO(camera);
    mFluenceMapRenderStrategy->SetCameraGO(camera);
    return true;
}

void TpsBEVRenderController::RemoveDataSource_i(DATA_SOURCE_ID id)
{
    switch (id)
    {
        case IMAGE:
            mDrrRenderStrategy->SetVolumeGO(nullptr);
            mDrrRenderStrategy->SetDrrGO(nullptr);
            mDrrRenderStrategy->SetCt2DensityGOColletion(nullptr);
            mDrrRenderStrategy->SetDrrSettingGoColletion(nullptr);
            mDrrRenderStrategy->SetStateControlGO(nullptr);
            mApplicatorRenderStrategy->SetVolumeGraphicObj(nullptr);
            mFluenceMapRenderStrategy->SetVolumeGO(nullptr);
            mFluenceMapRenderStrategy->SetStateControlGO(nullptr);
            mTransparencyRenderStrategy->SetVolumeGO(nullptr);
            mTransparencyRenderStrategy->SetShadedVOIGO(nullptr);
            mTransparencyRenderStrategy->SetVOICollectionGO(nullptr);
            mVOIRenderStrategy->SetStateControlGO(nullptr);
            mVOIRenderStrategy->SetVOICollectionGO(nullptr);
            mVOIRenderStrategy->SetVolumeGO(nullptr);
            break;
        case PLAN:
            mDrrRenderStrategy->SetNormalGroupCollectionGO(nullptr);
            mApplicatorRenderStrategy->SetNormalGroupCollection(nullptr);
            mFluenceMapRenderStrategy->SetNormalGroupCollectionGO(nullptr);
            mDrrRenderStrategy->SetCameraGO(nullptr);
            mTransparencyRenderStrategy->SetCameraGO(nullptr);
            mTransparencyRenderStrategy->SetDoseMeshCollectionGO(nullptr);
            mVOIRenderStrategy->SetCameraGO(nullptr);
            mFluenceMapRenderStrategy->SetCameraGO(nullptr);
            break;
        default:
            break;
    }
}

bool TpsBEVRenderController::TransformPoint(const Mcsf::MedViewer3D::Point2D& ptIn, 
    Mcsf::MedViewer3D::Point3D& ptOut)
{
    // check data source
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end())
    {
        TPS_LOG_DEV_ERROR << "The image data source is not ready.";
        return false;
    }
    if (mDataSourceMap.find(PLAN) == mDataSourceMap.end())
    {
        TPS_LOG_DEV_ERROR << "The plan data source is not ready.";
        return false;
    }
    std::string imageUID = mDataSourceMap[IMAGE];
    std::string planUID = mDataSourceMap[PLAN];

    //lock model warehouse
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);

    //get volume graphic object, get world 2 patient TM
    std::string volumeKey = imageUID + "|"+ GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto go = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(go);
    if (volume == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get volume go!" << imageUID;
        return false;
    }
    Matrix4x4 matW2P = volume->GetWorld2Patient();

    //get active segment graphic object, get collimator 2 patient TM
    go = mModelWarehouse->GetModelObject(
        planUID + "|" + GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    auto goNGCollection = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    if (nullptr == goNGCollection)
    {
        TPS_LOG_DEV_ERROR << "Failed to get normal group collection";
        return false;
    }
    std::string activeNGUID = goNGCollection->GetActiveNormalGroupUID();
    if (activeNGUID.empty()) return true;
    auto activeNG = goNGCollection->GetNormalGroup(activeNGUID);
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
    std::shared_ptr<TpsSegmentGraphicObject> activeSegment = beamGo->GetActiveSegmentGo();
    if (nullptr == activeSegment)
    {
        TPS_LOG_DEV_ERROR << "There is no active segment graphic object.";
        return false;
    }
    Mcsf::MedViewer3D::Matrix4x4 matC2P;
    activeSegment->GetStartCollimatorToPatMatrix(&matC2P);
    //get camera graphic object, translate screen point 2 view c.s. & get world 2 view TM
    go = mModelWarehouse->GetModelObject(
        planUID + "|" + GOTypeHelper::ToString(GO_TYPE_BEV_CAMERA));
    auto camera = std::dynamic_pointer_cast<CameraGraphicObject>(go);
    if (nullptr == camera)
    {
        TPS_LOG_DEV_ERROR << "The BEV camera does not exist.";
        return false;
    }

    int width, height;
    mView->GetDisplaySize(width, height);
    camera->AdaptFovViaAspect((double)width / height);
    Point3D eye = camera->GetEye();
    Point3D lookAt = camera->GetLookAt();
    double isoDist = (eye - lookAt).Magnitude();
    double nearDist = camera->GetNearClipDistance();
    double farDist = camera->GetFarClipDistance();
    double ratio = (isoDist - nearDist) / (farDist - nearDist);
    // convert to LookAt plane world
    const Point3D* corners = camera->GetWorldFrustumCorners();
    Mcsf::MedViewer3D::Point3D nearOut = corners[0];
    nearOut += ptIn.x * (corners[1] - corners[0]) + ptIn.y * (corners[3] - corners[0]);
    Mcsf::MedViewer3D::Point3D farOut = corners[4];
    farOut += ptIn.x * (corners[5] - corners[4]) + ptIn.y * (corners[7] - corners[4]);
    Mcsf::MedViewer3D::Point3D out = nearOut + ratio * (farOut - nearOut);
    // convert to collimator coordinate
    Mcsf::MedViewer3D::Matrix4x4 matWorld2Collimator = matW2P;
    matWorld2Collimator.Prepend(matC2P.Inverse());
    ptOut = matWorld2Collimator.Transform(out);
    //project to iso plane
    //ptOut *= (-1 * 1000.0 / ptOut.z);
    return true;
}

bool TpsBEVRenderController::GetCurrentPixelSpacingIn10MM(double& spacingX, double& spacingY)
{
    int width, height;
    mView->GetDisplaySize(width, height);
    if (width <= 0 || height <= 0)
    {
        TPS_LOG_DEV_ERROR << "The view size is not correct.";
        return false;
    }

    //check data source
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end())
    {
        TPS_LOG_DEV_ERROR << "The image data source is not ready.";
        return false;
    }
    if (mDataSourceMap.find(PLAN) == mDataSourceMap.end())
    {
        TPS_LOG_DEV_ERROR << "The plan data source is not ready.";
        return false;
    }
    std::string imageUID = mDataSourceMap[IMAGE];
    std::string planUID = mDataSourceMap[PLAN];
    //lock model warehouse
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    //get volume graphic object, get world 2 patient TM
    std::string volumeKey = imageUID + "|"+ GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto go = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(go);
    if (volume == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get volume go!" << imageUID;
        return false;
    }
    Matrix4x4 matW2P = volume->GetWorld2Patient();
    //get camera graphic object, translate screen point 2 view c.s. & get world 2 view TM
    go = mModelWarehouse->GetModelObject(
        planUID + "|" + GOTypeHelper::ToString(GO_TYPE_BEV_CAMERA));
    auto camera = std::dynamic_pointer_cast<CameraGraphicObject>(go);
    if (nullptr == camera)
    {
        TPS_LOG_DEV_ERROR << "The BEV camera does not exist.";
        return false;
    }

    //transform point from camera model to collimator c.s.
    camera->AdaptFovViaAspect((double)width / height);
    const Mcsf::MedViewer3D::Point3D* corners = camera->GetWorldFrustumCorners();
    double widthInMM = matW2P.Transform(corners[1] - corners[0]).Magnitude();
    Point3D eye = camera->GetEye();
    Point3D lookAt = camera->GetLookAt();
    double isoDist = (eye - lookAt).Magnitude();
    double nearDist = camera->GetNearClipDistance();
    widthInMM *= isoDist / nearDist;
    widthInMM = widthInMM <= 1e-6 ? 1e-6 : widthInMM;
    spacingY = spacingX = width / widthInMM * 10;
    
    ///handle zoom factor
    double zoomFactor = camera->GetZoomFactor();
    spacingX /= zoomFactor;
    spacingY /= zoomFactor;
    return true;
}

/*!\fn TpsBEVRenderController::GetBevViewDirecrion
* <key> \n
* PRA: Yes \n
* Name:bool GetBevViewDirecrion \n
* Traced from: 27421 DS_PRA_TPS_UBoyIndicator \n 
* Description: calculate the placement of U boy  \n
* Short Description: UBoy placement Indicator \n
* Component: TPS \n
* </key> \n
***********************************************************/
bool TpsBEVRenderController::GetBevViewDirecrion(Mcsf::MedViewer3D::Vector3D &vectorView, Mcsf::MedViewer3D::Vector3D &vectotUp) 
{
    std::string planUID = mDataSourceMap[PLAN];
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    IGraphicObjectPtr goCamera = nullptr;
    std::string goTypeStr = GOTypeHelper::ToString(GO_TYPE_BEV_CAMERA);
    std::string key = planUID + "|" + goTypeStr;
    goCamera = mModelWarehouse->GetModelObject(key);
    if (goCamera == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get bev camera";
        return false;
    }

    auto camera = std::dynamic_pointer_cast<CameraGraphicObject>(goCamera);
    Mcsf::MedViewer3D::Point3D ptLookAt = camera->GetLookAt();
    Mcsf::MedViewer3D::Point3D ptEye = camera->GetEye();
    vectotUp = camera->GetUpDirection();
    vectorView = ptLookAt - ptEye;
    return true;
}

void TpsBEVRenderController::DownSampleImage(float rate)
{
    if (mView == nullptr || rate <= 1e-6) return;

    int width, height;
    mView->GetDisplaySize(width, height);
    width = TPS_ROUND((float)width / rate);
    height = TPS_ROUND((float)height / rate);
    mView->SetDisplaySize(width, height, ClsView::IMAGE);
    TPS_LOG_DEV_INFO << "New rendering request commited";
    mView->Render(true);
}

void TpsBEVRenderController::ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid)
{
    if (id == IMAGE)
    {
        RemoveGraphicObject_i(uid, GO_TYPE_VOLUME);
        RemoveGraphicObject_i(uid, GO_TYPE_DRR);
        RemoveGraphicObject_i(uid, GO_TYPE_CT2DENSITYCollection);
        std::string mainPhaseImageUID = mDataRepository->GetImageDataManager(
            )->GetOperationImageUid(uid);
        RemoveGraphicObject_i(mainPhaseImageUID, GO_TYPE_VOI_COLLECTION);
        RemoveGraphicObject_i(mainPhaseImageUID, GO_TYPE_SHADEDSURFCE);
    }
    else if (id == PLAN)
    {
        RemoveGraphicObject_i(uid, GO_TYPE_NORMALGROUP_COLLECTION);
        RemoveGraphicObject_i(uid, GO_TYPE_BEV_CAMERA);
        RemoveGraphicObject_i(uid, GO_TYPE_DOSE_MESH_COLLECTION);
        RemoveGraphicObject_i(uid, GO_TYPE_DRRSETTING_COLLECTION);
    }
}
TPS_END_NAMESPACE
