#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_backgroundrenderstrategy.h"

TPS_BEGIN_NAMESPACE

BackgroundRenderStrategy::BackgroundRenderStrategy(){

}

void BackgroundRenderStrategy::Initialize() {
    mDirty = true;
}

BackgroundRenderStrategy::~BackgroundRenderStrategy() {

}

void BackgroundRenderStrategy::Render_i() {

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_uiFbo);

    glViewport(0,0,mWidth,mHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0x01);
    glStencilFunc(GL_ALWAYS, 0x00, 0x00);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

    glColor4f(0.1f,0.1f,0.1f,1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-1.0f,-1.0f,0.0f);
    glVertex3f(-1.0f, 1.0f,0.0f);
    glVertex3f( 1.0f, 1.0f,0.0f);
    glVertex3f( 1.0f,-1.0f,0.0f);
    glEnd();

    glDisable(GL_STENCIL_TEST);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

}

void BackgroundRenderStrategy::Resize(int w, int h) {
    mHeight = h;
    mWidth = w;
}

void BackgroundRenderStrategy::Finalize() {

}

void BackgroundRenderStrategy::SetDirty(bool ) {

}

int BackgroundRenderStrategy::CacheChanges() {
    return 0;
}

TPS_END_NAMESPACE