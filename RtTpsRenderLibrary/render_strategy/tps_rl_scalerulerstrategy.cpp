////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Shi Chunhui  chunhui.shi@united-imaging.com
/// 
///  \file tps_rl_scalerulerstrategy.cpp
/// 
///  \brief class TpsScaleRulerStrategy
/// 
///  \version 1.0
/// 
///  \date    2014/4/14
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
//#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_render_strategy_gpu.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_tag_types.h"
#include "McsfMedViewer3DArithmetic/matrix_4x4.h"

#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_scalerulerstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include <math.h>

#define CARRY_NUM 5
#define RULER_DISPLAY_RANGE 0.25
#define RULER_DISPLAY_VERTICAL_POS 0.075
#define RULER_STICK_HEIGHT 0.01

TPS_BEGIN_NAMESPACE
TpsScaleRulerStrategy::TpsScaleRulerStrategy() : 
    mMprGraphicObject(nullptr), mVolumeGraphicObject(nullptr), 
    mRulerTextGraphicObject(nullptr)
{

}

TpsScaleRulerStrategy::~TpsScaleRulerStrategy() 
{

}

void TpsScaleRulerStrategy::Initialize() 
{

}

void TpsScaleRulerStrategy::Finalize() 
{

}

void TpsScaleRulerStrategy::Render_i()
{
    using namespace Mcsf::MedViewer3D;
    if(nullptr == mMprGraphicObject || nullptr == mVolumeGraphicObject || 
        !mMprGraphicObject->RulerVisible())
    {
        return;
    }

    CalculatePixelSpacing_i();

    std::vector<float> ruler_pos;
    std::vector<float> ruler_sdw;
    std::vector<float> ruler_scale_val;

    int bigScaleCount = mTotalLen / mTickLen;
    int smallCarryNum = bigScaleCount < 5 ? ( mTickLen - 10.0 <= 1e-6 ? 5 : 4) : 2;
    double smallTickLen = mTickLen / smallCarryNum;
    double tickLenInPixel = smallTickLen / mPixelSpacing;
    int smallScaleCount = mTotalLen / smallTickLen;

    double x = 0.5 * (mWidth - smallScaleCount * tickLenInPixel);
    double y = mHeight * RULER_DISPLAY_VERTICAL_POS;
    double stickHeight = TPS_MAX(mHeight * RULER_STICK_HEIGHT, 5.0);
    double len(0), height;
    int idx(0);
    bool isBigScale;
    while (len - mTotalLen <= 1e-6)
    {
        isBigScale = idx % smallCarryNum == 0;
        height = isBigScale ? 1.75 * stickHeight : stickHeight;
        ruler_pos.push_back(x + idx * tickLenInPixel);
        ruler_pos.push_back(y);
        ruler_pos.push_back(x + idx * tickLenInPixel);
        ruler_pos.push_back(y + height);
        ruler_pos.push_back(x + idx * tickLenInPixel);
        ruler_pos.push_back(y);

        if (isBigScale)
        {
            ruler_scale_val.push_back(len / 10);//cm
        }
        len += smallTickLen;
        idx++;
    }

    for (int i = 0; i < ruler_pos.size(); i+=2){
        ruler_sdw.push_back(ruler_pos[i] + 1);
        ruler_sdw.push_back(ruler_pos[i + 1] - 1);
    }
    if (ruler_pos.empty()) return;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glViewport(0, 0, mWidth, mHeight);
    glUseProgram(0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, mWidth, 0.0f, mHeight, -1.0f, 1.0f);

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);

    float ruler_color[3] = {1.f, 1.f, 1.f};
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableClientState(GL_VERTEX_ARRAY);
    //draw shadow
    glColor4f(1.0 - ruler_color[0], 1.0 - ruler_color[1], 1.0 - ruler_color[2], 0.8f);
    glVertexPointer(2, GL_FLOAT, 0, ruler_sdw._Myfirst);
    glDrawArrays(GL_LINE_STRIP, 0, ruler_pos.size() / 2);
    //draw ruler
    glColor4f(ruler_color[0], ruler_color[1], ruler_color[2], 1.0f);
    glVertexPointer(2, GL_FLOAT, 0, ruler_pos._Myfirst);
    glDrawArrays(GL_LINE_STRIP, 0, ruler_pos.size() / 2);
    glDisableClientState(GL_VERTEX_ARRAY);

    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
    //draw text
    FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
    std::wstringstream wss;
    Pixel32* str_buffer = nullptr;
    int str_width, str_height;
    double interval = tickLenInPixel * smallCarryNum;
    int font_size = 12;

    wss.precision(4);
    std::wstring longest_text(L"");
    int str_len = 0;
    for (int i = 0; i < ruler_scale_val.size(); ++i){
        wss.clear();
        wss.str(L"");
        wss << ruler_scale_val[i];
        if (wss.str().length() > str_len)
        {
            str_len = wss.str().length();
            longest_text = wss.str();
        }
    }
    for (; font_size != 0; --font_size)
    {
        if (!pBufGen->GenerateRasterBuffer(longest_text, &str_buffer, 
            str_width, str_height, font_size, WHITE_VALUE)) 
        {
            TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer.";
            return;
        }
        if (interval > str_width * 1.5) break;
    }
    if (!font_size) return;

    double last_x = 0.0;
    //wss.setf(std::ios::fixed, std::ios::floatfield);
    for (int i = 0; i < ruler_scale_val.size(); ++i){
        wss.clear();
        wss.str(L"");
        wss << ruler_scale_val[i];
        if (!pBufGen->GenerateRasterBuffer(wss.str(), &str_buffer, 
            str_width, str_height, font_size, WHITE_VALUE)) {
            TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer.\n";
            return;
        }

        glRasterPos2d(x + i * interval - str_width * 0.5, TPS_MAX(y - str_height - 3, 0));
        glDrawPixels(str_width, str_height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)str_buffer);
        last_x = x + i * interval + str_width * 0.5;
        TPS_DEL_ARRAY(str_buffer);
    }

    x += (idx - 1) * tickLenInPixel;
    wss.clear();
    wss.str(L"cm");
    if (!pBufGen->GenerateRasterBuffer(wss.str(), &str_buffer, 
        str_width, str_height, font_size, WHITE_VALUE)) {
        TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer.\n";
        return;
    }
    glRasterPos2d(TPS_MAX(last_x, x), TPS_MAX(y - str_height - 3, 0));
    glDrawPixels(str_width, str_height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)str_buffer);
    TPS_DEL_ARRAY(str_buffer);

    glFlush();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void TpsScaleRulerStrategy::Resize(int width, int height) {
    if(mWidth != width || mHeight != height){
        mWidth = width;
        mHeight = height;
    }
}

int TpsScaleRulerStrategy::CacheChanges() {
    if(mVolumeGraphicObject == nullptr || mMprGraphicObject == nullptr) {
      //  printf("mVolumeGraphicObject or mMprGraphicObject is null.\n");
        return -1;
    }

    if (mVolumeGraphicObject->IsDirty()) {
        mDirty = true;
    }

    if(mMprGraphicObject->IsDirty()) {

        // Get the latest changed properties and flags
        std::map<unsigned int, int> mapChangedItems;
        mMprGraphicObject->GetPropertyInfo(NULL, &mapChangedItems);
        for (auto iter = mapChangedItems.begin(); iter != mapChangedItems.end(); ++iter){
            if (iter->first == Mcsf::MedViewer3D::kTagPrivate_Scale){
                mDirty = true;
                break;
            }
        }
    }

    return 0;
}

bool TpsScaleRulerStrategy::CalculatePixelSpacing_i(){

    if (mMprGraphicObject == nullptr){
        TPS_LOG_DEV_INFO<<"The mpr graphic object is empty.";
        return false;
    }
    if(mVolumeGraphicObject == nullptr) {
        TPS_LOG_DEV_INFO<<"The volume graphic object is empty";
        return false;
    }

    auto matW2P = mVolumeGraphicObject->GetWorld2Patient();
    Mcsf::MedViewer3D::Point3D vertices[4];
    mMprGraphicObject->GetVertices(vertices);
    double pixelLen = (mWidth + mHeight) / 2.0;//TPS_MIN(mWidth, mHeight);
    double worldLen = TPS_MAX((vertices[1] - vertices[0]).Magnitude(), 
        (vertices[3] - vertices[0]).Magnitude());
    double patientLen = matW2P.Transform(
        Mcsf::MedViewer3D::Vector3D(worldLen, 0.0, 0.0)).Magnitude();
    mPixelSpacing = patientLen / TPS_MIN(mWidth, mHeight);
    mTotalLen = pixelLen * RULER_DISPLAY_RANGE * mPixelSpacing;
    mTickLen = 10.0 * pow(2, -10.0);//初始参考值，1cm一大格
    while (mTotalLen / mTickLen - CARRY_NUM * 1.05 > 1e-6) {
        mTickLen *= 2;
    }
    mTotalLen = TPS_ROUND(mTotalLen / mTickLen) * mTickLen;
    return true;
}

void TpsScaleRulerStrategy::SetMprGraphicObject(std::shared_ptr<MPRGraphicObject> mprGo) {
    if (mMprGraphicObject != mprGo){
        mMprGraphicObject = mprGo;
        mDirty = true;
    }
}

void TpsScaleRulerStrategy::SetVolumeGraphicObject(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volume) {
    if (mVolumeGraphicObject != volume){
        mVolumeGraphicObject = volume;
        mDirty = true;
    }
}

void TpsScaleRulerStrategy::SetRulerTextInfoGraphicObject(std::shared_ptr<TpsTextOverlayGraphicObject> rulerText) {
    if (mRulerTextGraphicObject != rulerText){
        mRulerTextGraphicObject = rulerText;
        mDirty = true;
    }
}
TPS_END_NAMESPACE
