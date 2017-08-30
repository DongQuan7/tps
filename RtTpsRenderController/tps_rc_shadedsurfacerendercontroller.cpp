////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rc_shadedsurfacerendercontroller.cpp
/// 
///  \brief class ShadedSurfaceRenderController definition 
/// 
///  \version 1.0
/// 
///  \date    2014/02/28
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderController/tps_rc_shadedsurfacerendercontroller.h"

#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_surface_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_voi3d_renderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_statecontrol_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobjectcollection.h"

#include "RtTpsRenderLibrary/tps_rl_3d_window_render_strategy.h"

#include "tps_logger.h"
TPS_BEGIN_NAMESPACE
ShadedSurfaceRenderController::ShadedSurfaceRenderController(WINDOW_TYPE type) :
    TpsRenderController(type),mLinedVoiStrategy(nullptr)
{

}

ShadedSurfaceRenderController::~ShadedSurfaceRenderController()
{

}

bool ShadedSurfaceRenderController::SetupScene_i() 
{
    try
    {
        m3DWindowRenderStrategy.reset(new Tps3DWindowRenderStrategy());
        RegisterRenderStrategy_i(Mcsf::MedViewer3D::ClsView::IMAGE, m3DWindowRenderStrategy);

        mLinedVoiStrategy.reset(new VOI3DRenderStrategy(mWindowType));
        RegisterRenderStrategy_i(Mcsf::MedViewer3D::ClsView::IMAGE, mLinedVoiStrategy);

        return true;
    }
    catch (std::exception &ex)
    {
        TPS_LOG_DEV_ERROR << "Failed to setup scene:" << ex.what();
        return false;
    }
}

bool ShadedSurfaceRenderController::ResetDataSource_i(DATA_SOURCE_ID id)
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

bool ShadedSurfaceRenderController::ResetImage_i()
{
    std::string imageUID = mDataSourceMap[IMAGE];
    IGraphicObjectPtr goVolume = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_VOLUME, goVolume))
    {
        TPS_LOG_DEV_ERROR << "Failed to check volume graphic object of image:"<<imageUID;
        return false;
    }

    IGraphicObjectPtr goMprs = nullptr;
    GO_TYPE goType = GO_TYPE_MPR_SAGITTAL;
    if (!CheckGraphicObject_i(imageUID, goType, goMprs))
    {
        TPS_LOG_DEV_ERROR << "Failed to check MPR graphic object of image: "<<imageUID;
        return false;
    }

    IGraphicObjectPtr goMprc = nullptr;
    goType = GO_TYPE_MPR_CORONAL;
    if (!CheckGraphicObject_i(imageUID, goType, goMprc))
    {
        TPS_LOG_DEV_ERROR << "Failed to check MPR graphic object of image: "<<imageUID;
        return false;
    }

    IGraphicObjectPtr goMpra = nullptr;
    goType = GO_TYPE_MPR_AXIAL;
    if (!CheckGraphicObject_i(imageUID, goType, goMpra))
    {
        TPS_LOG_DEV_ERROR << "Failed to check MPR graphic object of image: " << imageUID;
        return false;
    }

    // CX: Change GO_TYPE_SURFACE_CAMERA Relate With SeriesUID
    IGraphicObjectPtr goCamera = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_SURFACE_CAMERA, goCamera))
    {
        TPS_LOG_DEV_ERROR << "Failed to check surface camera";
        return false;
    }

    std::string mainPhaseImageUID = mDataRepository->GetImageDataManager(
        )->GetOperationImageUid(imageUID);
    //check voi graphic object collection & VOI Rendering Cache graphic object
    IGraphicObjectPtr goVOICollection = nullptr;
    if (!CheckGraphicObject_i(mainPhaseImageUID, GO_TYPE_VOI_COLLECTION, goVOICollection))
    {
        TPS_LOG_DEV_ERROR << "Failed to check VOI graphic object collection of image: "<<mainPhaseImageUID;
        return false;
    }

    IGraphicObjectPtr goShadedSurface = nullptr;
    if (!CheckGraphicObject_i(mainPhaseImageUID, GO_TYPE_SHADEDSURFCE, goShadedSurface))
    {
        TPS_LOG_DEV_ERROR << "Failed to check shaded surface go:";
        return false;
    }

    IGraphicObjectPtr goSurfaceVR = nullptr;
    if (!CheckGraphicObject_i(COMMON_BASE, GO_TYPE_VR, goSurfaceVR))
    {
        TPS_LOG_DEV_ERROR << "Failed to check vr go:";
        return false;
    }

    IGraphicObjectPtr goStateControl = nullptr;
    if (!CheckGraphicObject_i(COMMON_BASE, GO_TYPE_BEV_DISPLAY_CONTROL, goStateControl))
    {
        TPS_LOG_DEV_ERROR << "Failed to get state control GO!";
        return false;
    }

    auto volume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(goVolume);
    auto shadedSurface = std::dynamic_pointer_cast<ShadedVoiGraphicObject>(goShadedSurface);
    auto VRGo = std::dynamic_pointer_cast<ShadedSurfaceGraphicObject>(goSurfaceVR);
    auto cameraGO = std::dynamic_pointer_cast<CameraGraphicObject>(goCamera);
    auto mprGoa = std::dynamic_pointer_cast<MPRGraphicObject>(goMpra);
    auto mprGoc = std::dynamic_pointer_cast<MPRGraphicObject>(goMprc);
    auto mprGos = std::dynamic_pointer_cast<MPRGraphicObject>(goMprs);
    auto stateControl = std::dynamic_pointer_cast<StateControlGraphicObject>(goStateControl);
    auto voiCollection = std::dynamic_pointer_cast<TpsVOIGOCollection>(goVOICollection);
    if (volume == nullptr || VRGo == nullptr || cameraGO==nullptr || 
        shadedSurface == nullptr || voiCollection == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to dynamic cast IGraphicObjects.";
        return false;
    }

    m3DWindowRenderStrategy->SetVolumeGO(volume);
    m3DWindowRenderStrategy->SetShadedVOIGO(shadedSurface);
    m3DWindowRenderStrategy->SetVOICollectionGO(voiCollection);
    m3DWindowRenderStrategy->SetCameraGO(cameraGO);
    m3DWindowRenderStrategy->SetMprGO(mprGoa, mprGoc, mprGos);

    mLinedVoiStrategy->SetVolumeGO(volume);
    mLinedVoiStrategy->SetStateControlGO(stateControl);
    mLinedVoiStrategy->SetVOICollectionGO(voiCollection);
    mLinedVoiStrategy->SetCameraGO(cameraGO);

    return true;
}

bool ShadedSurfaceRenderController::ResetPlan_i()
{
    std::string planUID = mDataSourceMap[PLAN];
    //check normal group collection graphic object
    IGraphicObjectPtr go = nullptr;
    if (!CheckGraphicObject_i(planUID, GO_TYPE_NORMALGROUP_COLLECTION, go))
    {
        TPS_LOG_DEV_ERROR << "Failed to check normal group collection of try: " << planUID;
        return false;
    }

    IGraphicObjectPtr doseMeshGo = nullptr;
    if (!CheckGraphicObject_i(planUID, GO_TYPE_DOSE_MESH_COLLECTION, doseMeshGo))
    {
        TPS_LOG_DEV_ERROR << "Failed to check DoseMesh graphic object of image: " << planUID;
        return false;
    }
    auto spDoseMeshGO = std::dynamic_pointer_cast<DoseMeshGraphicObjectCollection>(doseMeshGo);
    auto goNGCollection = std::dynamic_pointer_cast<NormalGroupCollection>(go);

    m3DWindowRenderStrategy->SetNGCollectionGO(goNGCollection);
    m3DWindowRenderStrategy->SetDoseMeshCollectionGO(spDoseMeshGO);
    return true;
}

void ShadedSurfaceRenderController::RemoveDataSource_i(DATA_SOURCE_ID id){
    switch (id) {
    case IMAGE:
        mLinedVoiStrategy->SetVOICollectionGO(nullptr);
        //mLinedVoiStrategy->SetCameraGO(nullptr);
        //mLinedVoiStrategy->SetBevDisplayGo(nullptr);
        mLinedVoiStrategy->SetVolumeGO(nullptr);
        //mLinedVoiStrategy->SetStateControlGO(nullptr);

        m3DWindowRenderStrategy->SetVolumeGO(nullptr);
        m3DWindowRenderStrategy->SetShadedVOIGO(nullptr);
        m3DWindowRenderStrategy->SetVOICollectionGO(nullptr);
        m3DWindowRenderStrategy->SetMprGO(nullptr, nullptr, nullptr);
        //m3DWindowRenderStrategy->SetCameraGO(nullptr);
        break;
    case PLAN:
        m3DWindowRenderStrategy->SetNGCollectionGO(nullptr);
        m3DWindowRenderStrategy->SetDoseMeshCollectionGO(nullptr);
        break;
    default:
        break;
    }
}

bool ShadedSurfaceRenderController::GetShadedSurfaceViewDirecrion(Mcsf::MedViewer3D::Vector3D &vectorView,
    Mcsf::MedViewer3D::Vector3D &vectotUp)
{
    std::string imageUID = mDataSourceMap[IMAGE];
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    IGraphicObjectPtr goCamera = nullptr;
    std::string goTypeStr = GOTypeHelper::ToString(GO_TYPE_SURFACE_CAMERA);
    std::string key = imageUID + "|" + goTypeStr;
    goCamera = mModelWarehouse->GetModelObject(key);
    if (goCamera == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to shadedsurface camera";
        return false;
    }

    auto camera = std::dynamic_pointer_cast<CameraGraphicObject>(goCamera);
    Mcsf::MedViewer3D::Point3D ptLookAt = camera->GetLookAt();
    Mcsf::MedViewer3D::Point3D ptEye = camera->GetEye();
    vectotUp = camera->GetUpDirection();
    vectorView = ptLookAt - ptEye;
    return true;
}

void ShadedSurfaceRenderController::ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid)
{
    if (id == IMAGE)
    {
        RemoveGraphicObject_i(uid, GO_TYPE_VOLUME);
        GO_TYPE goType = GO_TYPE_MPR_SAGITTAL;
        RemoveGraphicObject_i(uid, goType);
        goType = GO_TYPE_MPR_CORONAL;
        RemoveGraphicObject_i(uid, goType);
        goType = GO_TYPE_MPR_AXIAL;
        RemoveGraphicObject_i(uid, goType);
        RemoveGraphicObject_i(uid, GO_TYPE_SURFACE_CAMERA);

        std::string mainPhaseImageUID = mDataRepository->GetImageDataManager(
            )->GetOperationImageUid(uid);
        RemoveGraphicObject_i(mainPhaseImageUID, GO_TYPE_VOI_COLLECTION);
        RemoveGraphicObject_i(mainPhaseImageUID, GO_TYPE_SHADEDSURFCE);
    }
    else if (id == PLAN)
    {
        RemoveGraphicObject_i(uid, GO_TYPE_NORMALGROUP_COLLECTION);
        RemoveGraphicObject_i(uid, GO_TYPE_DOSE_MESH_COLLECTION);
    }
}
TPS_END_NAMESPACE
