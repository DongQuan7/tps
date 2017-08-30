////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_fluncemaprenderstrategy.cpp
/// 
///  \brief class FlunceMapRenderStrategy 
/// 
///  \version 1.0
/// 
///  \date    2014/07/17
////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_fluncemaprenderstrategy.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_raii_utility.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"

#include "RtTpsRenderLibrary/tps_rl_statecontrolforbevgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_fluencemapgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_drrgraphicobject.h"


#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

FlunceMapRenderStrategy::FlunceMapRenderStrategy() : Mcsf::MedViewer3D::RenderStrategyGPU(0){

}

FlunceMapRenderStrategy::~FlunceMapRenderStrategy() {

}

void FlunceMapRenderStrategy::Initialize() {

}

void FlunceMapRenderStrategy::Finalize() {

}

void FlunceMapRenderStrategy::Render() {

    using namespace Mcsf::MedViewer3D;
    Scopped_SetValue<bool, false> setFalse(&mDirty);
    if(nullptr == mFlunceMapGO || nullptr == mStateGO) {
        TPS_LOG_TRACE_INFO<<"Input go is null";
        return;
    }
    if(StateControlForBEVGraphicObject::DRR ==
        mStateGO->GetType()) {
        TPS_LOG_TRACE_INFO<<"DRR ! Not FlunceMap";
        return;
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_uiFbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glViewport(0,0,mWidth,mHeight);
    /*std::string activeNGUid = mNormalGroupCollection->GetActiveNormalGroupUID();
    auto activeNG = mNormalGroupCollection->GetNormalGroup(activeNGUid);
    if(activeNG == nullptr) {
    TPS_LOG_DEV_ERROR<<"There is no active normal group";
    return;
    }
    std::shared_ptr<TpsBeamGraphicObj> beamGO = activeNG->GetActiveBeam();
    if(nullptr == beamGO) {
    TPS_LOG_DEV_ERROR<<"beamGO is null!";
    return;
    }
    std::shared_ptr<TpsSegmentGraphicObj> segment = nullptr;
    segment = beamGO->GetActiveSegment();
    Matrix4x4 matBeam2Pat;
    segment->GetBeamToPatMatrix(&matBeam2Pat);

    Matrix4x4 matW2P = mVolumeGO->GetWorld2Patient();

    Matrix4x4 matModelView = matBeam2Pat;
    matModelView.Prepend(matW2P.Inverse());*/

    //Matrix4x4 matViewProjection; //= mDRRGO->GetViewProjectionMat(mWidth,mHeight);

    glMatrixMode(GL_MODELVIEW);
    //glLoadMatrixd(matModelView._m);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glOrtho(-250.0f,250.f,-250.f,250.f,-0.01f,0.01f);

    int xDim, yDim;
    mFlunceMapGO->GetDim(&xDim,&yDim);

    float xScale, yScale;
    mFlunceMapGO->GetScale(&xScale, &yScale);

    float *data = mFlunceMapGO->GetBuffer();

    float minValue,maxValue;
    mFlunceMapGO->GetMinMaxValue(&minValue,&maxValue);

    float xStart,yStart;
    mFlunceMapGO->GetStartPosition(&xStart,&yStart);

    float *color = mFlunceMapGO->GetColor();

 /*   glDisable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0x01);
    glStencilFunc(GL_ALWAYS, 0x01, 0x01);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);*/

    for(int i = 0; i < xDim; ++i) {
        for(int j = 0; j < yDim; ++j) {
           float temp = data[j * xDim +i];
           unsigned int index = (unsigned int)((temp - minValue) / (maxValue - minValue) * 254 + 0.49f);
           glColor4f(color[3*index],color[3*index+1],color[3*index+2],1.0f);
           float xCoord = xStart + i*xScale;
           float yCoord = yStart + j*yScale;
           glBegin(GL_QUADS);
           glVertex3f(xCoord - 0.5f * xScale, yCoord - 0.5f * yScale, 0.0f);
           glVertex3f(xCoord + 0.5f * xScale, yCoord - 0.5f * yScale, 0.0f);
           glVertex3f(xCoord + 0.5f * xScale, yCoord + 0.5f * yScale, 0.0f);
           glVertex3f(xCoord - 0.5f * xScale, yCoord + 0.5f * yScale, 0.0f);
           glEnd();
        }
    }
    delete [] color;
    //glDisable(GL_STENCIL_TEST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);


}

void FlunceMapRenderStrategy::Resize(int iWidth, int iHeight) {
    mWidth = iWidth;
    mHeight = iHeight;
}

void FlunceMapRenderStrategy::SetDirty(bool flag) {
    mDirty = flag;
}

int FlunceMapRenderStrategy::CacheChanges() {
    if(mStateGO == nullptr || mFlunceMapGO == nullptr || mVolumeGO==nullptr) {
        return -1;
    }
    if(mStateGO->IsDirty() || mFlunceMapGO->IsDirty() || mVolumeGO->IsDirty()) {
        mDirty = true;
    }
    return 0;

}

bool FlunceMapRenderStrategy::IsToUpdate() const {
    return mDirty;
}

void FlunceMapRenderStrategy::SetFlunceMapGO( 
      std::shared_ptr<FluenceMapGraphicObject> flunceGO) {
    mFlunceMapGO = flunceGO;
}

void FlunceMapRenderStrategy::SetStateGO(
    std::shared_ptr<StateControlForBEVGraphicObject> stateGO) {
    mStateGO = stateGO;
}

void FlunceMapRenderStrategy::SetVolumeGO(
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volumeGO) {
    mVolumeGO = volumeGO;
}




TPS_END_NAMESPACE

