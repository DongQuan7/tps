//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    
///  \brief   
///
///  \version 1.0
///  \date    Apr. 17, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderController/tps_rc_gallery_shadedsurfacerendercontroller.h"

#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DDataRepresentation/data_manager.h" 
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_vr.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_vr_colortable.h"

#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_fw_displaycache.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_surface_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_statecontrol_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_vr_renderstrategy.h" 


#include "tps_logger.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

TpsGalleryShadedSurfaceRenderController::TpsGalleryShadedSurfaceRenderController(WINDOW_TYPE type) :
    TpsRenderController(type),
    m_spVRRenderStrategy(nullptr)
{

}

TpsGalleryShadedSurfaceRenderController::~TpsGalleryShadedSurfaceRenderController()
{

}

bool TpsGalleryShadedSurfaceRenderController::SetupScene_i()
{
    try
    {
        m_spVRRenderStrategy.reset(new GalleryVRRenderStrategy());
        RegisterRenderStrategy_i(Mcsf::MedViewer3D::ClsView::IMAGE, m_spVRRenderStrategy);
        return true;
    }
    catch (std::exception &ex)
    {
        TPS_LOG_DEV_ERROR << "Failed to setup scene:" << ex.what();
        return false;
    }
}

bool TpsGalleryShadedSurfaceRenderController::ResetDataSource_i(DATA_SOURCE_ID id)
{

    bool bRes = false;
    switch (id)
    {
    case IMAGE:
        bRes = ResetImage_i();
        break;
    default:
        TPS_LOG_DEV_ERROR << "Unrecognized data source id: " << id;
        break;
    }
    return bRes;
}

bool TpsGalleryShadedSurfaceRenderController::ResetImage_i()
{

    std::string imageUID = mDataSourceMap[IMAGE];
    IGraphicObjectPtr goVolume = nullptr;
    if (!CheckGraphicObject_i(imageUID,GO_TYPE_GALLERY_VOLUME,goVolume))
    {
        TPS_LOG_DEV_ERROR << "Failed to check volume graphic object of image:" << imageUID;
        return false;
    }

    IGraphicObjectPtr goCamera = nullptr;
    if (!CheckGraphicObject_i(imageUID,GO_TYPE_GALLERY_SURFACE_CAMERA,goCamera))
    {
        TPS_LOG_DEV_ERROR << "Failed to check surface camera of Image:" << imageUID;
        return false;
    }

    IGraphicObjectPtr goVR = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_GALLERY_VR, goVR))
    {
        TPS_LOG_DEV_ERROR << "Failed to check gallery vr of Image:" << imageUID;
        return false;
    }

    IGraphicObjectPtr goColorTable = nullptr;
    if (!CheckGraphicObject_i(COMMON_BASE, GO_TYPE_GALLERY_COLORTABLE, goColorTable))
    {
        TPS_LOG_DEV_ERROR << "Failed to check gallery colortable of Image:" << imageUID;
        return false;
    } 

    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    auto colortable = std::dynamic_pointer_cast<GraphicObjectVR_ColorTable>(goColorTable);
    auto volumeRendering = std::dynamic_pointer_cast<GraphicObjectVolumeRendering>(goVR);
    auto cameraGO = std::dynamic_pointer_cast<CameraGraphicObject>(goCamera);

    if (volume == nullptr || volumeRendering == nullptr || colortable == nullptr || cameraGO==nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to dynamic cast IGraphicObjects.";
        return false;
    }

    int iMaskLabel = 1;
    int iRenderType = -1;
    double aWindow[2] = {0.0, 0.0};
    colortable->GetCurrentWindowLevel(aWindow[0], aWindow[1], iMaskLabel);    
    volumeRendering->GetRenderAlgorithmType(&iRenderType);
    volumeRendering->UpdateBrickInfo();
    //volumeRendering->SetWindowing(aWindow[0], aWindow[1], iRenderType, (unsigned char)iMaskLabel); 
    volumeRendering->SetWindowing(3000, aWindow[1], iRenderType, (unsigned char)iMaskLabel);
    size_t uiDim[3] = {0,0,0};
    volume->GetDimension(uiDim);
    volumeRendering->SetTargetVolume(Point3D(0,0,0) , Point3D(uiDim[0],uiDim[1],uiDim[2]));

    m_spVRRenderStrategy->SetColorTableGO(colortable);
    m_spVRRenderStrategy->SetVRGO(volumeRendering);
    m_spVRRenderStrategy->SetVolumeGO(volume, nullptr);
    m_spVRRenderStrategy->SetCameraGO(cameraGO);

    return true;

}

void TpsGalleryShadedSurfaceRenderController::RemoveDataSource_i(DATA_SOURCE_ID id)
{
    switch (id)
    {
    case IMAGE:
        m_spVRRenderStrategy->SetVolumeGO(nullptr, nullptr);
        m_spVRRenderStrategy->SetCameraGO(nullptr);
        m_spVRRenderStrategy->SetColorTableGO(nullptr);
        m_spVRRenderStrategy->SetVRGO(nullptr);
        break;
    default:
        break;
    }
}

void TpsGalleryShadedSurfaceRenderController::RemoveGraphicObjects(const std::string& seriesUID)
{
    if (seriesUID.empty())
    {
        return;
    }

    //remove camera graphic object
    RemoveGraphicObject_i(seriesUID, GO_TYPE_GALLERY_SURFACE_CAMERA);
    // remove vr go
    RemoveGraphicObject_i(seriesUID, GO_TYPE_GALLERY_VR);
    // remove volume graphic object
    RemoveGraphicObject_i(seriesUID, GO_TYPE_GALLERY_VOLUME);
    // remove volume graphic object
    RemoveGraphicObject_i(COMMON_BASE, GO_TYPE_GALLERY_COLORTABLE);
}

void TpsGalleryShadedSurfaceRenderController::RemoveGraphicObject_i(const std::string& seriesUID, GO_TYPE goType)
{
    std::string goTypeStr = GOTypeHelper::ToString(goType);
    std::string key = seriesUID + "|" + goTypeStr;
    auto go = mModelWarehouse->GetModelObject(key);
    if (nullptr != go)
    {
        mModelWarehouse->RemoveModelObject(key);
    }
}     

bool TpsGalleryShadedSurfaceRenderController::GetShadedSurfaceViewDirecrion(Mcsf::MedViewer3D::Vector3D &vectorView, 
    Mcsf::MedViewer3D::Vector3D &vectotUp)
{
    std::string imageUID = mDataSourceMap[IMAGE];
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    IGraphicObjectPtr goCamera = nullptr;
    std::string goTypeStr = GOTypeHelper::ToString(GO_TYPE_GALLERY_SURFACE_CAMERA);
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

TPS_END_NAMESPACE
