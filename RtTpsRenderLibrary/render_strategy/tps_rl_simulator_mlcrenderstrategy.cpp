//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_rl_simulator_mlcrenderstrategy.cpp 
///  \brief   render mlc shape
///
///  \version 1.0
///  \date  2015/02/10  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsRenderLibrary/tps_rl_simulator_mlcrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "tps_logger.h"

//#include "gl/glut.h"

TPS_BEGIN_NAMESPACE

TpsSimulatorMLCRenderStategy::TpsSimulatorMLCRenderStategy() : 
mNormGroupCollectionGO(nullptr){

}

TpsSimulatorMLCRenderStategy::~TpsSimulatorMLCRenderStategy() {

}

void TpsSimulatorMLCRenderStategy::Render_i() {
    using namespace Mcsf::MedViewer3D;

    //add by wzd simulator mlc 改成前端绘制
    return;

    if (mNormGroupCollectionGO == nullptr) {
        TPS_LOG_DEV_INFO<<"input go is nullptr!";
        return;
    }

    // Get Active segment
    std::string activeNGUid = mNormGroupCollectionGO->GetActiveNormalGroupUID();
    if (activeNGUid.empty()) return;
    auto activeNG = mNormGroupCollectionGO->GetNormalGroup(activeNGUid);
    if(nullptr == activeNG) {
        TPS_LOG_DEV_ERROR<<"There is no active normal group graphic object.";
        return;
    }
    std::shared_ptr<TpsBeamGraphicObject> beamGO = activeNG->GetActiveBeamGo();
    if(nullptr == beamGO){
        TPS_LOG_DEV_ERROR<<"There is no active beam graphic object.";
        return;
    }
    std::shared_ptr<TpsSegmentGraphicObject> segment = nullptr;
    segment = beamGO->GetActiveSegmentGo();

    if(segment == nullptr) {
#ifdef _DEBUG
        printf("\n[TPS][BE][RenderLibrary]segment is empty(one reason: burst arc is not finished, see in create beam cmd)");
#endif
        TPS_LOG_DEV_ERROR<<"get segment error ";
        return;
    }

    // Get MLC setting and positions
    StcMLC* pMLC = segment->GetMLC();
    const int nLeaf = pMLC->m_nLeafNum;
    plunc::LEAF_POSITION_STC* pMlcLeaf = pMLC->m_pLeaf;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_uiFbo);
    glViewport(0, 0, mWidth, mHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(-mWidth/2, mWidth/2, -mHeight/2, mHeight/2);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_STENCIL_TEST);
    glStencilMask(0x01);
    glStencilFunc(GL_ALWAYS, 0x01, 0x01);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

    const float minJawPosition = -200.0f;
    const float maxJawPosition =  200.0f;
    const float distS2A = 0.0f;

    for(int i = 0; i < nLeaf ; ++i) {
        //left side;
        glBegin(GL_LINE_LOOP);
        glVertex3f(minJawPosition, pMlcLeaf[i].min_w,-1 * distS2A);
        glVertex3f(pMlcLeaf[i].pos_1, pMlcLeaf[i].min_w, -1 * distS2A);
        glVertex3f(pMlcLeaf[i].pos_1, pMlcLeaf[i].max_w, -1 * distS2A);
        glVertex3f(minJawPosition,pMlcLeaf[i].max_w, -1 * distS2A);
        glEnd();
        //right side;
        glBegin(GL_LINE_LOOP);
        glVertex3f(pMlcLeaf[i].pos_2,pMlcLeaf[i].min_w, -1 * distS2A);
        glVertex3f(maxJawPosition, pMlcLeaf[i].min_w, -1 * distS2A);
        glVertex3f(maxJawPosition, pMlcLeaf[i].max_w, -1 * distS2A);
        glVertex3f(pMlcLeaf[i].pos_2,  pMlcLeaf[i].max_w, -1 * distS2A);
        glEnd();
    }

    //glDisable(GL_BLEND);

    glDisable(GL_STENCIL_TEST);
    glFlush();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void TpsSimulatorMLCRenderStategy::Initialize() {

}

void TpsSimulatorMLCRenderStategy::Finalize() {

}

int TpsSimulatorMLCRenderStategy::CacheChanges() {
    if (mNormGroupCollectionGO == nullptr) {
        return -1;
    }

    if (mNormGroupCollectionGO->IsDirty()) {
        mDirty = true;
    }

    return 0;
}

void TpsSimulatorMLCRenderStategy::Resize( int iWidth, int iHeight ) {
    mWidth = iWidth;
    mHeight = iHeight;
}

void TpsSimulatorMLCRenderStategy::SetNormalGroupCollection( std::shared_ptr<NormalGroupCollection> normGroupGO ) {
    if (mNormGroupCollectionGO != normGroupGO) {
        mNormGroupCollectionGO = normGroupGO;
        mDirty = true;
    }
}

TPS_END_NAMESPACE