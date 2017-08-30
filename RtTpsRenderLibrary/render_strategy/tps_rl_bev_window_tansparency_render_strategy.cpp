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
#include "RtTpsRenderLibrary/tps_rl_bev_window_tansparency_render_strategy.h"
#include "RtTpsRenderLibrary/tps_rl_transparency_scene_renderer.h"
#include "RtTpsRenderLibrary/tps_rl_dual_depth_peeling_algorithm.h"
#include "RtTpsRenderLibrary/tps_rl_front_depth_peeling_algorithm.h"
#include "RtTpsRenderLibrary/tps_rl_voi_transparency_rendering_node.h"
#include "RtTpsRenderLibrary/tps_rl_beam_transparency_rendering_node.h"
#include "RtTpsRenderLibrary/tps_rl_dose_transparency_rendering_node.h"

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

BevWindowTransparencyRenderStrategy::BevWindowTransparencyRenderStrategy()
    : mTSRenderer(new TransparencySceneRenderer())
    , mVOITRNode(nullptr), mDoseTRNode(nullptr)
{
    mVOITRNode = new VOITransparencyRenderingNode(VOITRNode);
    mDoseTRNode = new DoseTransparencyRenderingNode(DoseTRNode);
}

BevWindowTransparencyRenderStrategy::~BevWindowTransparencyRenderStrategy()
{
    TPS_DEL_PTR(mTSRenderer);
    TPS_DEL_PTR(mVOITRNode);
    TPS_DEL_PTR(mDoseTRNode);
}

void BevWindowTransparencyRenderStrategy::Initialize()
{
    mTSRenderer->AddNode(mVOITRNode);
    mTSRenderer->AddNode(mDoseTRNode);

    //if (!mTSRenderer->Initialize(new DualDepthPeelingAlgorithm()))
    if (!mTSRenderer->Initialize(new FrontDepthPeelingAlgorithm()))
    {
        TPS_LOG_DEV_ERROR<<"Failed to initialize transparency renderer.";
        return;
    }
}

void BevWindowTransparencyRenderStrategy::Finalize()
{
    mTSRenderer->Finalize();
}

void BevWindowTransparencyRenderStrategy::Render_i()
{
    mTSRenderer->Render(m_uiFbo, GL_COLOR_ATTACHMENT0);
}

int BevWindowTransparencyRenderStrategy::CacheChanges()
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
    return 0;
}

void BevWindowTransparencyRenderStrategy::Resize(int iWidth, int iHeight)
{
    mWidth = iWidth;
    mHeight = iHeight;
    mTSRenderer->Resize(iWidth, iHeight);
    mDirty = true;
}

void BevWindowTransparencyRenderStrategy::SetVolumeGO(const VolumeGOPtr& volumeGO)
{
    if (mVolumeGO != volumeGO)
    {
        mVolumeGO = volumeGO;
        mVOITRNode->SetVolumeGO(volumeGO);
        mDoseTRNode->SetVolumeGO(volumeGO);
        mDirty = true;
    }
}

void BevWindowTransparencyRenderStrategy::SetCameraGO(const CameraGOPtr& cameraGO)
{
    if (mCameraGO != cameraGO)
    {
        mCameraGO = cameraGO;
        mVOITRNode->SetCameraGO(cameraGO);
        mDoseTRNode->SetCameraGO(cameraGO);
        mDirty = true;
    }
}

void BevWindowTransparencyRenderStrategy::SetShadedVOIGO(const ShadedVOIGOPtr& shadedVOIGO)
{
    if (mShadedVOIGO != shadedVOIGO)
    {
        mShadedVOIGO = shadedVOIGO;
        mVOITRNode->SetShadedVOIGO(shadedVOIGO);
        mDoseTRNode->SetShadedVOIGO(shadedVOIGO);
        mDirty = true;
    }
}

void BevWindowTransparencyRenderStrategy::SetVOICollectionGO(const VOICollectionGOPtr& voiCollectionGO)
{
    if (mVOICollectionGO != voiCollectionGO)
    {
        mVOICollectionGO = voiCollectionGO;
        mVOITRNode->SetVOICollectionGO(voiCollectionGO);
        mDirty = true;
    }
}

void BevWindowTransparencyRenderStrategy::SetDoseMeshCollectionGO(const DoseMeshCollectionGOPtr& doseMeshCollectionGO)
{
    if (mDoseMeshCollectionGO != doseMeshCollectionGO)
    {
        mDoseMeshCollectionGO = doseMeshCollectionGO;
        mDoseTRNode->SetDoseMeshCollectionGO(mDoseMeshCollectionGO);
        mDirty = true;
    }
}
TPS_END_NAMESPACE