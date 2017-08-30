#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_setupptcrosshairstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_editsetuppoigraphicobject.h"

TPS_BEGIN_NAMESPACE
TpsSetupPtCrossHairStrategy::TpsSetupPtCrossHairStrategy() : mEditSetUpGo(nullptr){

}

TpsSetupPtCrossHairStrategy::~TpsSetupPtCrossHairStrategy() {

}

void TpsSetupPtCrossHairStrategy::Initialize() {

}

void TpsSetupPtCrossHairStrategy::Finalize() {

}

void TpsSetupPtCrossHairStrategy::Render_i() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glViewport(0,0,mWidth,mHeight);
    if(nullptr == mEditSetUpGo) {
        TPS_LOG_DEV_INFO<<"edit set up go is nullptr!";
        return;
    }

    Mcsf::MedViewer3D::Point2D position = mEditSetUpGo->GetPosition();

    if(!mEditSetUpGo->GetVisible()) {
        return;
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glEnable(GL_STENCIL_TEST);
    glStencilMask(0x01);
    glStencilFunc(GL_ALWAYS,0x01,0x01);
    glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);


    glColor4f(1.0f,0.0f,0.0f,1.0f);
    glBegin(GL_LINES);

    glVertex3d(-1.0,position.y*2-1,0.0);
    glVertex3d(1.0,position.y*2-1,0.0);

    glVertex3d(position.x*2-1,-1.0,0.0);
    glVertex3d(position.x*2-1,1.0,0.0);

    glEnd();

    glDisable(GL_STENCIL_TEST);
    glFinish();
    
}

void TpsSetupPtCrossHairStrategy::SetGraphicObject(std::shared_ptr<EditSetupPtGraphicObject> go) {
    if (mEditSetUpGo != go){
        mEditSetUpGo = go;
        mDirty = true;
    }
}

void TpsSetupPtCrossHairStrategy::Resize(int width, int height) {
    mWidth = width;
    mHeight = height;
}

int TpsSetupPtCrossHairStrategy::CacheChanges() {
    if(mEditSetUpGo == nullptr) {
        return -1;
    }
    if(mEditSetUpGo->IsDirty()) {
        mDirty = true;
    }
    return 0;
}
TPS_END_NAMESPACE
