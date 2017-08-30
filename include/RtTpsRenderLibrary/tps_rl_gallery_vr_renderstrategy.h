//////////////////////////////////////////////////////////////////////////
/// \defgroup RT TMS of Common Software Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang@united-imaging.com
///
///  \file    rt_tms_render_vrstrategy.h
///  \brief   
///
///  \version 1.0
///  \date    Jul. 10, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_RL_GALLERY_RENDER_VRSTRATEGY_H_
#define TPS_RL_GALLERY_RENDER_VRSTRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_render_strategy_gpu_vr_refactor.h"

namespace Viewer3D = Mcsf::MedViewer3D;
namespace Mcsf
{
    namespace MedViewer3D
    {
        class FrameBuffer;
        class GraphicObjVolume;
        class GraphicObjectVolumeRendering;
        class GraphicObjectVR_ColorTable;
        class RenderStrategyGPUVRRefactor;
    }
}

TPS_BEGIN_NAMESPACE

class VRRenderStrategy : public Mcsf::MedViewer3D::RenderStrategyGPUVRRefactor
{
public:
    VRRenderStrategy() : 
        Mcsf::MedViewer3D::RenderStrategyGPUVRRefactor(nullptr, nullptr,nullptr,nullptr, 0)
    {

    }

    virtual ~VRRenderStrategy() {}

    inline void SetVRGO(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjectVolumeRendering> spVR)
    {
        m_spVR = spVR;
    }

    inline std::shared_ptr<Mcsf::MedViewer3D::GraphicObjectVolumeRendering> GetVRGO() const
    {
        return m_spVR;
    }

    inline void SetColorTableGO(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjectVR_ColorTable> spColorTable)
    {
        m_spVRColorTable = spColorTable;
    }

    inline std::shared_ptr<Mcsf::MedViewer3D::GraphicObjectVR_ColorTable> GetColorTableGO() const
    {
        return m_spVRColorTable;
    }

    /// \brief Change the volume data to use
    /// \param[in] spVolume_VolumeTarget  Volume Graphic Object and Cube Graphic Object
    /// \return void 
    inline void SetVolumeGO(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spVolume_VolumeTarget, 
        std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spMask)
    {
        m_spVolume = spVolume_VolumeTarget;
        m_spMask = spMask;
    }

    inline std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> GetVolumeGO() const
    {
        return m_spVolume;
    }
};

class CameraGraphicObject;

class TPS_RL_DECLSPEC GalleryVRRenderStrategy : public TpsRenderStrategyBase
{
public:

    GalleryVRRenderStrategy(void);

    virtual ~GalleryVRRenderStrategy(void);

    /// \fun    Initialize
    /// \brief  For GPU implementation, any GL resources specific to the
    ///         strategy or the render context should be created in the function
    virtual void Initialize();

    /// \fun    Finalize
    /// \brief  For GPU implementation, the resources created in the Initialize
    ///         function should be cleaned up
    virtual void Finalize();

    virtual void Resize(int iWidth, int iHeight);

    virtual void SetFrameBuffer(unsigned int uiFbo);

    virtual void SetFBO(const std::shared_ptr<Mcsf::MedViewer3D::FrameBuffer> spFBO);

    virtual void SetDownSampleMode(bool bDownSample);

    virtual void SetWishRenderingTimePerFrame(unsigned int uims);

    virtual int CacheChanges();

    /// \brief Change the vr data to use
    /// \param[in] spVR  VolumeRending Graphic Object
    /// \return void 
    virtual void SetVRGO(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjectVolumeRendering> spVR);

    /// \brief Change the color table data to use
    /// \param[in] spColorTable  ColorTable of VR Graphic Object
    /// \return void 
    virtual void SetColorTableGO(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjectVR_ColorTable> spColorTable);

    /// \brief Change the volume data to use
    /// \param[in] spVolume_VolumeTarget  Volume Graphic Object and Cube Graphic Object
    /// \return void 
    virtual void SetVolumeGO(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spVolume_VolumeTarget, 
        std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spMask);

    virtual void SetCameraGO(std::shared_ptr<CameraGraphicObject> spCamera);

protected:
    virtual void Render_i();

    std::shared_ptr<CameraGraphicObject> m_spCamera;

    VRRenderStrategy* mVRStretegy;
};

TPS_END_NAMESPACE

#endif
