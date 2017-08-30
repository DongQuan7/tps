////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Shi Chunhui  chunhui.shi@united-imaging.com
/// 
///  \file tps_rl_colorspectrarenderstrategy.cpp
/// 
///  \brief class TpsColorSpectraRenderStrategy
/// 
///  \version 1.0
/// 
///  \date    2014/4/30
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_colorspectrarenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mprdosegraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_doselineobject.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "tps_logger.h"


TPS_BEGIN_NAMESPACE
TpsColorSpectraRenderStrategy::TpsColorSpectraRenderStrategy() : 
    mOriginX(-0.95f), mOriginY(-0.6f), mOffsetX(0.03f), mOffsetY(0.06f){
}

TpsColorSpectraRenderStrategy::~TpsColorSpectraRenderStrategy() {

}

void TpsColorSpectraRenderStrategy::Initialize() {
    
}

void TpsColorSpectraRenderStrategy::Finalize() {

}

void TpsColorSpectraRenderStrategy::Render_i(){
    using namespace Mcsf::MedViewer3D;
    if (mDoseGO == nullptr) {
        TPS_LOG_TRACE_INFO << "[TpsColorSpectraRenderStrategy]The dose go is empty!";
        return;
    }

    // get Ratio and color
    std::vector<DoseLineAttribute> vecDoseLine;
    mDoseGO->GetDisplayAttributes(vecDoseLine);
    if (vecDoseLine.size() <= 0) {
        TPS_LOG_TRACE_INFO << "[TpsColorSpectraRenderStrategy]There are not dose ratio color yet";
        return;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glViewport(0, 0, mWidth, mHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_STENCIL_TEST);
    glStencilMask(0x01);
    glStencilFunc(GL_ALWAYS, 0x01, 0x01);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glShadeModel(GL_FLAT);

    float margin = 15.0f / mHeight;

    float *color = vecDoseLine[0].mColor;
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glLineWidth(1.0f);
    for(int j = 0, k = j + 1; j < vecDoseLine.size(); ++j, ++k){
        if (vecDoseLine[j].mVisible == true) {
        glBegin(GL_QUADS);
        color = vecDoseLine[j].mColor;
        glColor3f(color[0], color[1], color[2]);
        glVertex2f(mOriginX, mOriginY + (k-1) * (mOffsetY + margin));
        glVertex2f(mOriginX + mOffsetX, mOriginY+ (k-1) * (mOffsetY + margin));
        glVertex2f(mOriginX + mOffsetX, mOriginY + k * mOffsetY + (k-1) * margin);
        glVertex2f(mOriginX, mOriginY + k * mOffsetY + (k-1) * margin);
        glEnd();
        }
    }
    for(int j = 0, k = j + 1; j < vecDoseLine.size(); ++j, ++k){
        glBegin(GL_LINE_LOOP);
        color = vecDoseLine[j].mColor;
        glColor3f(color[0], color[1], color[2]);
        glVertex2f(mOriginX, mOriginY + (k-1) * (mOffsetY + margin));
        glVertex2f(mOriginX + mOffsetX, mOriginY+ (k-1) * (mOffsetY + margin));
        glVertex2f(mOriginX + mOffsetX, mOriginY + k * mOffsetY + (k-1) * margin);
        glVertex2f(mOriginX, mOriginY + k * mOffsetY + (k-1) * margin);
        glEnd();
    }
    glPopAttrib();

    FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();

    Pixel32* pStrBuffer = nullptr;
    int strBufWidth = 0;
    int strBufHeight = 0;
    std::wstringstream wss;

    double x_start = 5 + 0.5 * (1 + (mOriginX + mOffsetX)) * mWidth;
    double y_start = 0.5 * (1 + mOriginY) * mHeight;

    double deltaY = 0.5 * (mOffsetY + margin) * mHeight;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, (double)mWidth, 0.0f, (double)mHeight, -1.0f, 10.0f);

    for(int i = 0, k = i; i < vecDoseLine.size(); ++i, ++k){
        //if (vecDoseLine[i].mVisible == false){
        //    //TPS_LOG_TRACE_INFO << "[TpsColorSpectraRenderStrategy]The dose line is not visible!";
        //    --k;
        //    continue;
        //}
        wss.clear();
        wss.str(L"");
        if(mDoseGO->GetDisplayMode() == ABSOLUTE_VALUE){
            //wss<<vecDoseLine[i].mDosePercent * vecDoseLine[i].mReferenceDose<<" cGy";
        }else{
            wss<<vecDoseLine[i].mDosePercent * 100 <<"%";
        }

        if (!pBufGen->GenerateRasterBuffer(wss.str(), &pStrBuffer, strBufWidth, strBufHeight)){
            TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for slice.\n";
            return;
        }

        glRasterPos2d(x_start, y_start + k * deltaY);
        glDrawPixels(strBufWidth, strBufHeight, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)pStrBuffer);
        if( pStrBuffer != nullptr){
            delete[] pStrBuffer;
            pStrBuffer = nullptr;
        }
    }
    vecDoseLine.clear();
    std::vector<DoseLineAttribute>().swap(vecDoseLine);

    glDisable(GL_STENCIL_TEST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

}

void TpsColorSpectraRenderStrategy::Resize(int width, int height) {
    mWidth = width;
    mHeight = height;
}

int TpsColorSpectraRenderStrategy::CacheChanges() {
    return 0;
}

void TpsColorSpectraRenderStrategy::SetDoseGraphicObject(
    std::shared_ptr<TpsDoseLineGraphicObject> doseGO) {
    if (mDoseGO != doseGO){
        mDoseGO = doseGO;
        mDirty = true;
    }
}


TPS_END_NAMESPACE
