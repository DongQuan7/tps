////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_volume4d_mpr_renderstrategy.cpp
/// 
///  \brief class TpsVolume4dMprRenderStrategy implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/07/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_volume4d_mpr_renderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_algorithm.h"
#include "RtTpsRenderLibrary/tps_rl_volume4d_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3D/McsfViewer3DResource/mcsf_viewer3d_texture_3d.h"
#include "McsfMedViewer3DGraphicUtils/camera.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"

#include "tps_logger.h"

//#include "gl/glut.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

static double MPR_TOLERANCE = 0.01;

Volume4dMprRenderStrategy::Volume4dMprRenderStrategy()
    : m_spVolume4dGO(nullptr), mIs4DEnabled(false)
{

}

Volume4dMprRenderStrategy::~Volume4dMprRenderStrategy()
{

}

void Volume4dMprRenderStrategy::Render_i()
{
    if (mIs4DEnabled)
    {
        if (m_spVolume4dGO == nullptr) return;
        this->SetVolumeGO(m_spVolume4dGO->GetCurrentVolume());
    }
    
    FusionRenderStrategy::Render_i();
}

int Volume4dMprRenderStrategy::CacheChanges()
{
    if (mIs4DEnabled)
    {
        if (m_spVolume4dGO == nullptr){
            return -1;
        }
        mDirty |= m_spVolume4dGO->IsDirty();
    }

    return FusionRenderStrategy::CacheChanges();
}
TPS_END_NAMESPACE  // end namespace tps
