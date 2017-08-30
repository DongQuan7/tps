////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  (mailto:maoliang.gong@united-imaging.com)
/// 
///  \file tps_rl_3d_window_render_strategy.cpp
/// 
///  \brief class Tps3DWindowRenderStrategy implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/10/18
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_3d_window_render_strategy.h"
#include "RtTpsRenderLibrary/tps_rl_transparency_scene_renderer.h"
#include "RtTpsRenderLibrary/tps_rl_dual_depth_peeling_algorithm.h"
#include "RtTpsRenderLibrary/tps_rl_front_depth_peeling_algorithm.h"
#include "RtTpsRenderLibrary/tps_rl_voi_transparency_rendering_node.h"
#include "RtTpsRenderLibrary/tps_rl_beam_transparency_rendering_node.h"
#include "RtTpsRenderLibrary/tps_rl_dose_transparency_rendering_node.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_transparency_rendering_node.h"
#include "RtTpsRenderLibrary/tps_rl_volume_cube_transparency_rendering_node.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

TPS_BEGIN_NAMESPACE

Tps3DWindowRenderStrategy::Tps3DWindowRenderStrategy()
    : mTSRenderer(new TransparencySceneRenderer())
    , mVOITRNode(nullptr), mBeamTRNode(nullptr)
    , mDoseTRNode(nullptr), mMprTRNode(nullptr)
{
    mVOITRNode = new VOITransparencyRenderingNode(VOITRNode);
    mBeamTRNode = new BeamTransparencyRenderingNode(BeamTRNode);
    mDoseTRNode = new DoseTransparencyRenderingNode(DoseTRNode);
    mMprTRNode = new MprTransparencyRenderingNode(MPRTRNode);
    mVolumeCubeNode = new VolumeCubeTRNode(CubeTRNode);
}

Tps3DWindowRenderStrategy::~Tps3DWindowRenderStrategy()
{
    TPS_DEL_PTR(mTSRenderer);
    TPS_DEL_PTR(mVOITRNode);
    TPS_DEL_PTR(mBeamTRNode);
    TPS_DEL_PTR(mDoseTRNode);
    TPS_DEL_PTR(mMprTRNode);
    TPS_DEL_PTR(mVolumeCubeNode);
}

void Tps3DWindowRenderStrategy::Initialize()
{
    //mTSRenderer->AddNode(mVolumeCubeNode);
    mTSRenderer->AddNode(mVOITRNode);
    mTSRenderer->AddNode(mBeamTRNode);
    mTSRenderer->AddNode(mDoseTRNode);
    mTSRenderer->AddNode(mMprTRNode);

    //if (!mTSRenderer->Initialize(new DualDepthPeelingAlgorithm()))
    if (!mTSRenderer->Initialize(new FrontDepthPeelingAlgorithm()))
    {
        TPS_LOG_DEV_ERROR<<"Failed to initialize transparency renderer.";
        return;
    }
}

void Tps3DWindowRenderStrategy::Finalize()
{
    mTSRenderer->Finalize();
}

void Tps3DWindowRenderStrategy::Render_i()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glClearColor(0.05f, 0.05f, 0.05f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    mTSRenderer->Render(m_uiFbo, GL_COLOR_ATTACHMENT0);
}

int Tps3DWindowRenderStrategy::CacheChanges()
{
    if(mVolumeGO == nullptr || mShadedVOIGO == nullptr || mCameraGO == nullptr)
    {
        return -1;
    }
    if(mVolumeGO->IsDirty() || mShadedVOIGO->IsDirty() || mCameraGO->IsDirty())
    {
        mDirty = true;
    }

    if (mVOICollectionGO != nullptr && mVOICollectionGO->IsDirty())
    {
        mDirty = true;
    }

    if (mDoseMeshCollectionGO != nullptr && mDoseMeshCollectionGO->IsDirty())
    {
        mDirty = true;
    }

    int mprSection = mShadedVOIGO->GetMprSection();
    if (mprSection == AXIAL && mMprAxialGO != nullptr && mMprAxialGO->IsDirty())
    {
        mDirty = true;
    }
    else if (mprSection == CORONAL && mMprCoronalGO != nullptr && mMprCoronalGO->IsDirty())
    {
        mDirty = true;
    }
    else if (mprSection == SAGITTAL && mMprSagittalGO != nullptr && mMprSagittalGO->IsDirty())
    {
        mDirty = true;
    }

    if(mNGCollectionGO == nullptr) return -1;
    std::string activeNGUid = mNGCollectionGO->GetActiveNormalGroupUID();
    auto activeNG = mNGCollectionGO->GetNormalGroup(activeNGUid);
    if (activeNG == nullptr){
        TPS_LOG_DEV_ERROR<<"There is no active normal group!";
        return -1;
    }

    std::map<std::string, std::shared_ptr<TpsBeamGraphicObject>> beamMap = activeNG->GetBeamMap();
    for(auto itr = beamMap.begin(); itr != beamMap.end(); ++itr)
    {
        std::shared_ptr<TpsBeamGraphicObject> beamGo = itr->second;
        if (beamGo == nullptr)
        {
            TPS_LOG_DEV_ERROR<<"TpsBeamGraphicObj is null!";
            continue;
        }
        if(beamGo->IsDirty())
        {
            mDirty = true;
            break;
        }
    }

    if(beamMap.size() == 0 || activeNG->IsDirty())
    {
        mDirty = true;
    }
    return 0;
}

void Tps3DWindowRenderStrategy::Resize(int iWidth, int iHeight)
{
    mWidth = iWidth;
    mHeight = iHeight;
    mTSRenderer->Resize(iWidth, iHeight);
    mDirty = true;
}

void Tps3DWindowRenderStrategy::SetVolumeGO(const VolumeGOPtr& volumeGO)
{
    if (mVolumeGO != volumeGO)
    {
        mVolumeGO = volumeGO;
        mVolumeCubeNode->SetVolumeGO(volumeGO);
        mVOITRNode->SetVolumeGO(volumeGO);
        mBeamTRNode->SetVolumeGO(volumeGO);
        mDoseTRNode->SetVolumeGO(volumeGO);
        mMprTRNode->SetVolumeGO(volumeGO);
        mDirty = true;
    }
}

void Tps3DWindowRenderStrategy::SetCameraGO(const CameraGOPtr& cameraGO)
{
    if (mCameraGO != cameraGO)
    {
        mCameraGO = cameraGO;
        mVolumeCubeNode->SetCameraGO(cameraGO);
        mVOITRNode->SetCameraGO(cameraGO);
        mBeamTRNode->SetCameraGO(cameraGO);
        mDoseTRNode->SetCameraGO(cameraGO);
        mMprTRNode->SetCameraGO(cameraGO);
        mDirty = true;
    }
}

void Tps3DWindowRenderStrategy::SetShadedVOIGO(const ShadedVOIGOPtr& shadedVOIGO)
{
    if (mShadedVOIGO != shadedVOIGO)
    {
        mShadedVOIGO = shadedVOIGO;
        mVolumeCubeNode->SetShadedVOIGO(shadedVOIGO);
        mVOITRNode->SetShadedVOIGO(shadedVOIGO);
        mBeamTRNode->SetShadedVOIGO(shadedVOIGO);
        mDoseTRNode->SetShadedVOIGO(shadedVOIGO);
        mMprTRNode->SetShadedVOIGO(shadedVOIGO);
        mDirty = true;
    }
}

void Tps3DWindowRenderStrategy::SetVOICollectionGO(const VOICollectionGOPtr& voiCollectionGO)
{
    if (mVOICollectionGO != voiCollectionGO)
    {
        mVOICollectionGO = voiCollectionGO;
        mVOITRNode->SetVOICollectionGO(voiCollectionGO);
        mDirty = true;
    }
}

void Tps3DWindowRenderStrategy::SetNGCollectionGO(const NGCollectionGOPtr& ngCollectionGO)
{
    if (mNGCollectionGO != ngCollectionGO)
    {
        mNGCollectionGO = ngCollectionGO;
        mBeamTRNode->SetNGCollectionGO(mNGCollectionGO);
        mDirty = true;
    }
}

void Tps3DWindowRenderStrategy::SetDoseMeshCollectionGO(const DoseMeshCollectionGOPtr& doseMeshCollectionGO)
{
    if (mDoseMeshCollectionGO != doseMeshCollectionGO)
    {
        mDoseMeshCollectionGO = doseMeshCollectionGO;
        mDoseTRNode->SetDoseMeshCollectionGO(mDoseMeshCollectionGO);
        mDirty = true;
    }
}

void Tps3DWindowRenderStrategy::SetMprGO(const MprGOPtr& axialGO, 
    const MprGOPtr& coronalGO, const MprGOPtr& sagittalGO)
{
    if (mMprAxialGO != axialGO && mMprCoronalGO != coronalGO && mMprSagittalGO != sagittalGO)
    {
        mMprAxialGO = axialGO;
        mMprCoronalGO = coronalGO;
        mMprSagittalGO = sagittalGO;
        mMprTRNode->SetMprGO(axialGO, coronalGO, sagittalGO);
        mDirty = true;
    }
}
TPS_END_NAMESPACE