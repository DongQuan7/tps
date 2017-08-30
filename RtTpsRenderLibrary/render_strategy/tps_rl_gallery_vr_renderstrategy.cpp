//////////////////////////////////////////////////////////////////////////
/// \defgroup RT TMS of Common Software Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang@united-imaging.com
///
///  \file    rt_tms_render_vrstrategy.cpp
///  \brief   
///
///  \version 1.0
///  \date    Jul. 10, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_vr_renderstrategy.h"

#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
//ZHENGHE
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"                  //GraphicObjVolume
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_vr.h"                      // GraphicObjectVolumeRendering
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_vr_colortable.h"           //GraphicObjectVR_ColorTable
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_vr_api.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_raii_utility.h"                           //Scopped_SetValue

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

GalleryVRRenderStrategy::GalleryVRRenderStrategy(void)
{
    mVRStretegy = new VRRenderStrategy();
}

GalleryVRRenderStrategy::~GalleryVRRenderStrategy(void)
{
    TPS_DEL_PTR(mVRStretegy);
}

void GalleryVRRenderStrategy::Initialize()
{
    mVRStretegy->Initialize();
}

void GalleryVRRenderStrategy::Finalize()
{
    mVRStretegy->Finalize();
}

void GalleryVRRenderStrategy::SetFrameBuffer(unsigned int uiFbo)
{
    mVRStretegy->SetFrameBuffer(uiFbo);
}

void GalleryVRRenderStrategy::SetFBO(const std::shared_ptr<Mcsf::MedViewer3D::FrameBuffer> spFBO)
{
    mVRStretegy->SetFBO(spFBO);
}

void GalleryVRRenderStrategy::SetDownSampleMode(bool bDownSample)
{
    mVRStretegy->SetDownSampleMode(bDownSample);
}

void GalleryVRRenderStrategy::SetWishRenderingTimePerFrame(unsigned int uims)
{
    mVRStretegy->SetWishRenderingTimePerFrame(uims);
}

void GalleryVRRenderStrategy::Resize(int iWidth, int iHeight){

    mHeight = iHeight;
    mWidth = iWidth;
    mDirty = true;
    mVRStretegy->Resize(iWidth, iHeight);
}

void GalleryVRRenderStrategy::Render_i() {

    mDirty = false;
    auto volumeGO = mVRStretegy->GetVolumeGO();
    auto vrGO = mVRStretegy->GetVRGO();
    auto colorTableGO = mVRStretegy->GetColorTableGO();
    if(nullptr == volumeGO || nullptr == vrGO || nullptr == colorTableGO || nullptr == m_spCamera) {
        return;
    }

    // 按照当前的TPS RenderStrategy的Add方式， Resize时，m_spVR未null， 接口直接返回，没有调用SetDisplaySize
    // 如果没有做下面的判断， VR图像不在View的中心
    // Resize方法check m_spVolume以及m_spVR
    int iWidth, iHeight;
    vrGO->GetDisplaySize(&iWidth, &iHeight);
    if (iWidth != mWidth || iHeight != mHeight){
        vrGO->SetDisplaySize(mWidth, mHeight);
        Resize(mWidth, mHeight);
        //return;
    }

    Point3D ptEye = m_spCamera->GetEye();
    Point3D ptLookAt = m_spCamera->GetLookAt();
    double angle = m_spCamera->GetFovY();
    double dDistance = (ptLookAt - ptEye).Magnitude();
    Vector3D ptUp = m_spCamera->GetUpDirection();
    double halfH = dDistance * std::tan(angle * 0.5);
    Vector3D vecUp = ptUp * halfH;
    VRAPI::PlaceVRCamera(vrGO, ptEye, ptLookAt, vecUp);  

    mVRStretegy->Render();
    return;
}

void GalleryVRRenderStrategy::SetVRGO(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjectVolumeRendering> spVR){
    if (mVRStretegy->GetVRGO() != spVR) {
        mVRStretegy->SetVRGO(spVR);
        mDirty = true;
    }
}

void GalleryVRRenderStrategy::SetColorTableGO(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjectVR_ColorTable> spColorTable){
    if (mVRStretegy->GetColorTableGO() != spColorTable){
        mVRStretegy->SetColorTableGO(spColorTable);
        mDirty = true;
    }
}

void GalleryVRRenderStrategy::SetVolumeGO(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spVolume,
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spMask){
    if (mVRStretegy->GetVolumeGO() != spVolume) {
        mVRStretegy->SetVolumeGO(spVolume, spMask);
        mDirty = true;
    }       
}

void GalleryVRRenderStrategy::SetCameraGO(std::shared_ptr<CameraGraphicObject> spCamera){

    if (m_spCamera != spCamera) {
        m_spCamera = spCamera;
        mDirty = true;
    }
}

int GalleryVRRenderStrategy::CacheChanges() 
{
    auto volumeGO = mVRStretegy->GetVolumeGO();
    auto vrGO = mVRStretegy->GetVRGO();
    auto colorTableGO = mVRStretegy->GetColorTableGO();
    if(nullptr == volumeGO || 
       nullptr == vrGO || 
       nullptr == colorTableGO || 
       nullptr == m_spCamera) 
    {
        return -1;
    }

    // m_spVR->IsDirty()返回值一直是205，无法用作判断
    if(volumeGO->IsDirty() || /*m_spVR->IsDirty() ||*/ colorTableGO->IsDirty() || m_spCamera->IsDirty()) {
        mDirty = true;
    }
    return 0;

}

TPS_END_NAMESPACE

