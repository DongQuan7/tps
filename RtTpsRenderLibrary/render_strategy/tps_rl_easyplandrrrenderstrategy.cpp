//////////////////////////////////////////////////////////////////////////
///defgroup TDA of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  yilong.cheng mailto:yilong.cheng@united-imaging.com
///
///  \file    tda_rl_igrt2dqarenderstrategy.cpp
///  \brief   DRR RenderStrategy
///
///  \version 1.0
///  \date   2014/10/29
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_easyplandrrrenderstrategy.h"

//ZHENGHE

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_vr_colortable.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_vr.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_raii_utility.h"
#include "McsfMedViewer3DGraphicUtils/camera.h"
#include "McsfMedViewer3DArithmetic/point2d.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "RtTpsRenderLibrary/tps_rl_pixelbufferbmpalphawindowingconverter.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"


TPS_BEGIN_NAMESPACE   


TpsEasyPlanDrrRenderStrategy::TpsEasyPlanDrrRenderStrategy(void){

}

TpsEasyPlanDrrRenderStrategy::~TpsEasyPlanDrrRenderStrategy(void){

}

void TpsEasyPlanDrrRenderStrategy::Initialize() {
    glGenTextures(1,&mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TpsEasyPlanDrrRenderStrategy::Finalize() {
    glDeleteTextures(1,&mTextureID);
}

int TpsEasyPlanDrrRenderStrategy::CacheChanges() {

    if (mEasyPlanDrrGoCollection == nullptr || mNormalGroupGOCollection == nullptr) {
        return -1;
    }

    if (mEasyPlanDrrGoCollection->IsDirty() || mNormalGroupGOCollection->IsDirty()){
        mDirty = true;
    }
    return 0;
}

void TpsEasyPlanDrrRenderStrategy::CreateImage(const std::shared_ptr<EasyPlanDrrGraphicObejct>& drrGO)
{
    //get fixed image buffer
    char *dcmBuffer = drrGO->GetImageBuffer();

    //get image contrast value
    mImageWidth=drrGO->GetImageWidth();
    mImageHeight=drrGO->GetImageHight();
    int winCenter;
    int winWidth;
    DATATYPE dataType = drrGO->GetDataType();
    bool isInversed = drrGO->GetIsInversed();

    drrGO->GetWindowingWinWidthAndWinCenter(&winWidth,&winCenter);

    unsigned char* imageWindowing= new unsigned char[mImageWidth*mImageHeight];
    PixelBufferBmpAlphaAndWindowingConverter::PixelBufferWindowingConverter(
        dcmBuffer,mImageWidth,mImageHeight,winWidth,winCenter,dataType,imageWindowing);
    unsigned char* rgbaImage = new unsigned char[mImageWidth*mImageHeight*4];
    PixelBufferBmpAlphaAndWindowingConverter::WindowingBufferToRGBAConverter(
    imageWindowing,mImageWidth,mImageHeight,rgbaImage);

    // judge if the buffer should be inversed
    // if true: implement schema: "pixelValue = 255 - pixelValue"
    if(isInversed) {
        for(int i = 0; i <mImageHeight ; ++i) {
            for(int j = 0; j<mImageWidth; ++j) {
                rgbaImage[(mImageWidth *i+j)*4] = 255 - rgbaImage[(mImageWidth *i+j)*4];
                rgbaImage[(mImageWidth *i+j)*4 +1] = 255 - rgbaImage[(mImageWidth *i+j)*4 +1];
                rgbaImage[(mImageWidth *i+j)*4 + 2] = 255 - rgbaImage[(mImageWidth *i+j)*4 + 2];
                //rgbaImage[(mImageWidth *i+j)*4 + 3] =(unsigned char ) alpha*255;
            }
        }
    }

    delete imageWindowing; imageWindowing = nullptr;

    //load image
    glBindTexture(GL_TEXTURE_2D,mTextureID);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,mImageWidth,mImageHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,rgbaImage);
    glBindTexture(GL_TEXTURE_2D,0);

    delete rgbaImage; rgbaImage = nullptr;
}

void TpsEasyPlanDrrRenderStrategy::Render_i()
{
    if(mEasyPlanDrrGoCollection == nullptr || mNormalGroupGOCollection == nullptr)
    {
        return ;
    }

    auto activeDRR = GetCurrentDrrGO();
    if (activeDRR == nullptr)
        return;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glViewport(0,0,mWidth,mHeight);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    CreateImage(activeDRR);

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    DrawTexture(activeDRR);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void TpsEasyPlanDrrRenderStrategy::SetDRRGOCollection(std::shared_ptr<EasyPlanDrrGraphicObjectCollection> easyPlanDrrGoCollection) 
{
    if (mEasyPlanDrrGoCollection != easyPlanDrrGoCollection)
    {
        mEasyPlanDrrGoCollection = easyPlanDrrGoCollection;
        mDirty = true;
    }
}

void TpsEasyPlanDrrRenderStrategy::Resize(int iWidth, int iHeight) {
    mHeight = iHeight;
    mWidth = iWidth; 

}

void TpsEasyPlanDrrRenderStrategy::DrawTexture(const std::shared_ptr<EasyPlanDrrGraphicObejct>& drrGO) 
{
    Mcsf::Vector3f a(0.0f,0.0f,0.0f);
    Mcsf::Vector3f b(0.0f,0.0f,0.0f);
    
    glDisable(GL_BLEND);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,mTextureID);
    glEnable(GL_TEXTURE_2D);

    Mcsf::Matrix3f matImageModel2DrrModel(Mcsf::Vector3f(1.0f,0.0f,0.0f),Mcsf::Vector3f (0.0f,1.0f,0.0f),
        Mcsf::Vector3f (0.0f,0.0f,1.0f));
    if (!drrGO->GetDrrModel2ImageModel(&matImageModel2DrrModel)) {
        TPS_LOG_DEV_ERROR<<"Failed to  TpsEasyPlanDrrRenderStrategy::DrawTexture() :m_DRRFusionGO->GetDrrModel2ImageModel ";
        return;
    }
    matImageModel2DrrModel = matImageModel2DrrModel.Inverse();

    float scale[2];
    if (mWidth > mHeight) {
        scale[0] = float(mHeight) / mWidth * 2;
        scale[1] = 1 * 2;
    }
    else{
        scale[0] = 1 * 2;
        scale[1] = float(mWidth) / mHeight * 2;
    }

    Mcsf::Matrix3f matDrrModel2Screen(Mcsf::Vector3f(scale[0], 0, 0), 
        Mcsf::Vector3f(0, scale[1], 0), Mcsf::Vector3f(0, 0, 1));
    Mcsf::Matrix3f matImageModel2Screen = matImageModel2DrrModel;
    matImageModel2Screen.Prepend(matDrrModel2Screen);

    // apply the table angle deference between image capture table angle and current table angle
    Mcsf::Point3f isoPosInImgM = drrGO->GetIsocenterPosInImageModel();

    // 3: translate image back to original position
    Mcsf::Vector3f panBackCol0(1.0, 0.0, 0.0);
    Mcsf::Vector3f panBackCol1(0.0, 1.0, 0.0);
    Mcsf::Vector3f panBackCol2(isoPosInImgM.GetX(), isoPosInImgM.GetY(), 1.0);
    Mcsf::Matrix3f panBackMatrix3f;
    panBackMatrix3f.SetCol0(panBackCol0);
    panBackMatrix3f.SetCol1(panBackCol1);
    panBackMatrix3f.SetCol2(panBackCol2);
    matImageModel2Screen.Append(panBackMatrix3f);

    // 2: rotate image based on isocenter position
    double imgCaptureAngle = drrGO->GetImageCaptureTableAngle();
    double imgCurrentTableAngle = drrGO->GetImageTableAngle();
    double angleToRotate = imgCaptureAngle - imgCurrentTableAngle ;
    if(drrGO->GetIsOpposedImage()) {
        angleToRotate = angleToRotate * PI / 180.0;
    }
    else {
        angleToRotate = -1 * angleToRotate * PI / 180.0;
    }
    Mcsf::Vector3f col0(cos(angleToRotate),sin(angleToRotate),0.0);
    Mcsf::Vector3f col1(-sin(angleToRotate),cos(angleToRotate),0.0);
    Mcsf::Vector3f col2(0.0 ,0.0 ,1.0);
    Mcsf::Matrix3f matrix3f;
    matrix3f.SetCol0(col0);
    matrix3f.SetCol1(col1);
    matrix3f.SetCol2(col2);
    matImageModel2Screen.Append(matrix3f);

    // 1: translate image from original position to isocenter position
    Mcsf::Vector3f panCol0(1.0, 0.0, 0.0);
    Mcsf::Vector3f panCol1(0.0, 1.0, 0.0);
    Mcsf::Vector3f panCol2(-isoPosInImgM.GetX(), -isoPosInImgM.GetY(), 1.0);
    Mcsf::Matrix3f panMatrix3f;
    panMatrix3f.SetCol0(panCol0);
    panMatrix3f.SetCol1(panCol1);
    panMatrix3f.SetCol2(panCol2);
    matImageModel2Screen.Append(panMatrix3f);

    glColor3d(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    
    glTexCoord2f(0.05f,0.05f);
    a = Mcsf::Vector3f(.05f,.05f,1.0f);
    b = matImageModel2Screen*(a);
    glVertex3f(b.GetX(),b.GetY(),0.0f);

    glTexCoord2f(0.05f,0.95f);
    a = Mcsf::Vector3f(.05f,0.95f,1.0f);
    b = matImageModel2Screen.operator*(a);
    glVertex3f(b.GetX(),b.GetY(),0.0f);

    glTexCoord2f(0.95f,0.95f);
    a=Mcsf::Vector3f(0.95f,0.95f,1.0f);
    b = matImageModel2Screen.operator*(a);
    glVertex3f(b.GetX(),b.GetY(),0.0f);

    glTexCoord2f(0.95f,0.05f);
    a = Mcsf::Vector3f(0.95f,.05f,1.0f);
    b = matImageModel2Screen.operator*(a);
    glVertex3f(b.GetX(),b.GetY(),0.0f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D,0);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

std::shared_ptr<EasyPlanDrrGraphicObejct> TpsEasyPlanDrrRenderStrategy::GetCurrentDrrGO() const
{
    if (nullptr == mNormalGroupGOCollection)
    {
        TPS_LOG_DEV_ERROR << "Failed to get normal group collection";
        return nullptr;
    }
    std::string activeNGUID = mNormalGroupGOCollection->GetActiveNormalGroupUID();
    if (activeNGUID.empty()) return nullptr;
    auto activeNG = mNormalGroupGOCollection->GetNormalGroup(activeNGUID);
    if (nullptr == activeNG)
    {
        TPS_LOG_DEV_ERROR << "There is no active normal group graphic object.";
        return nullptr;
    }
    std::shared_ptr<TpsBeamGraphicObject> beamGo = activeNG->GetActiveBeamGo();
    if (nullptr == beamGo)
    {
        TPS_LOG_DEV_ERROR << "There is no active beam graphic object.";
        return nullptr;
    }
    std::string currentBeamUID = beamGo->GetUID();
    std::map<std::string,std::shared_ptr<EasyPlanDrrGraphicObejct>> drrMap = mEasyPlanDrrGoCollection->GetDrrMap();
    std::map<std::string,std::shared_ptr<EasyPlanDrrGraphicObejct>>::iterator itDrr = drrMap.begin();
    for (; itDrr!= drrMap.end(); itDrr++)
    {
        std::shared_ptr<EasyPlanDrrGraphicObejct> drrGO = itDrr->second;
        if (drrGO->GetBeamUID() == currentBeamUID)
        {
            return drrGO;
        }
    }
    return nullptr;
}

void TpsEasyPlanDrrRenderStrategy::SetNGGOCollection(std::shared_ptr<NormalGroupCollection> normalGroupGOCollection)
{
    mNormalGroupGOCollection = normalGroupGOCollection;
}

TPS_END_NAMESPACE

