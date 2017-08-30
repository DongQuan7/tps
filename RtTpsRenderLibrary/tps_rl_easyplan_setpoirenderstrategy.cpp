//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_rl_easyplan_setpoirenderstrategy.cpp 
///  \brief   set easyplan poi in drr image
///
///  \version 1.0
///  \date  2014/11/07  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setpoirenderstrategy.h"

#include "Mcsf3DArithmetic/mcsf_3d_matrix3x3.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"

#include "RtTpsFramework/tps_fw_image2dentity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"


TPS_BEGIN_NAMESPACE
TpsEasyPlanSetPoiRenderStrategy::TpsEasyPlanSetPoiRenderStrategy() 
: mPoiGraphicObejctColl(nullptr), mImagePlanPixelSpacingX(1.0f),mImagePlanPixelSpacingY(1.0f){

}

TpsEasyPlanSetPoiRenderStrategy::~TpsEasyPlanSetPoiRenderStrategy() {

}

void TpsEasyPlanSetPoiRenderStrategy::Initialize() {

}

void TpsEasyPlanSetPoiRenderStrategy::Finalize() {

}

int TpsEasyPlanSetPoiRenderStrategy::CacheChanges() {
    if(mPoiGraphicObejctColl == nullptr ) {
        return -1;
    }
    if (mPoiGraphicObejctColl->GetPoiMap().size() == 0) {
        return -1;
    }
    mDirty = true;
    return 0;
}
void TpsEasyPlanSetPoiRenderStrategy::Resize(int width, int height) {
    mWidth = width;
    mHeight = height;
}

void TpsEasyPlanSetPoiRenderStrategy::SetPixelSpaceData(float imagePlanPixelSpacingX, float imagePlanPixelSpacingY) {

    mImagePlanPixelSpacingX = imagePlanPixelSpacingX;
    mImagePlanPixelSpacingY = imagePlanPixelSpacingY;
}

void TpsEasyPlanSetPoiRenderStrategy::SetPoiGraphicObjectCollection(
    std::shared_ptr<TpsPoiGraphicObjectCollection> poiGoColl) {
    if (mPoiGraphicObejctColl != poiGoColl){
        mPoiGraphicObejctColl = poiGoColl;
        mDirty = true;
    }
}

void TpsEasyPlanSetPoiRenderStrategy::SetDrrGraphicObject(std::shared_ptr<EasyPlanDrrGraphicObejct> drrGo){
    if(mDrrGraphicObject != drrGo) {
        mDrrGraphicObject = drrGo;
        mDirty = true;
    }
}

void TpsEasyPlanSetPoiRenderStrategy::Render_i() {
    using namespace Mcsf::MedViewer3D;
    //drawPoint
    if (mPoiGraphicObejctColl == nullptr) {
        TPS_LOG_DEV_INFO << "poi GraphicObject is NULL !";
        return;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glViewport(0,0,mWidth,mHeight);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, mWidth, 0, mHeight, -0.01f, 0.01f);

    glEnable(GL_STENCIL_TEST);
    glStencilMask(0x01);
    glStencilFunc(GL_ALWAYS, 0x01, 0x01);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    std::map<std::string, std::shared_ptr<TpsPoiGraphicObject>> poiMap =
        mPoiGraphicObejctColl->GetPoiMap();

    if (poiMap.size() > 0) {

        //translate coordinate system
        std::string currentRTImage("");

        //////////////////////////////////////////////////////////////////////////
        //!!!!!!!!!!!to do by whom related!!!!!!!!!
        //1. remove current image uid interface
        //2. it's dangerous to access data in manager here
        //////////////////////////////////////////////////////////////////////////

        // get isocenter coordinate first
        float IsocenterX = 0.0f;
        float IsocenterY = 0.0f;
        for (auto itr = poiMap.begin(); itr != poiMap.end(); ++itr) {
            std::shared_ptr<TpsPoiGraphicObject> poiGo = itr->second;
            if (poiGo->GetPoiType() == RtDbDef::IsoCenter) {

                // if current go is plan opposed image go then the isocenter position should be also be opposed.
                /*Mcsf::MedViewer3D::Point3D isocenterPos = poiGo->GetPosition();
                if(mDrrGraphicObject != nullptr && mDrrGraphicObject->GetIsOpposedImage()) {
                    isocenterPos.x = 1 - isocenterPos.x;
                }*/
                Mcsf::MedViewer3D::Point3D isocenterPos;
                if(mDrrGraphicObject != nullptr) {
                    // The Isocenter position stored in POIGO is plan image(not opposed image) coordinate based.
                    /*if(mDrrGraphicObject->GetIsOpposedImage()) { 
                        isocenterPos.x = 1.0 - poiGo->GetPosition().x;
                        isocenterPos.y = poiGo->GetPosition().y;
                        isocenterPos.z = poiGo->GetPosition().z;
                    }
                    else {
                        isocenterPos.x = poiGo->GetPosition().x;
                        isocenterPos.y = poiGo->GetPosition().y;
                        isocenterPos.z = poiGo->GetPosition().z;
                    }*/

                    Mcsf::Point3f isopos = mDrrGraphicObject->GetIsocenterPosInImageModel();
                    isocenterPos.x = isopos.GetX();
                    isocenterPos.y = isopos.GetY();
                    isocenterPos.z = isopos.GetZ();
                }
                else
                {
                    isocenterPos.x = 0.5;
                    isocenterPos.y = 0.5;
                    isocenterPos.z = 0.0;
                }
                
                Mcsf::Matrix3f matImageModel2DrrModel(Mcsf::Vector3f(1.0f,0.0f,0.0f),Mcsf::Vector3f (0.0f,1.0f,0.0f),
                    Mcsf::Vector3f (0.0f,0.0f,1.0f));
                if(mDrrGraphicObject != nullptr) {
                    if(!mDrrGraphicObject->GetDrrModel2ImageModel(&matImageModel2DrrModel)) {
                        TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetPanOperation::ModifyGraphicObject() : "
                            <<"Failed to easyPlanDrrGO->SetDrrModel2ImageModel";
                        return;
                    }
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
                Mcsf::Vector3f a = Mcsf::Vector3f(isocenterPos.x,isocenterPos.y,1.0f);
                Mcsf::Vector3f b = matImageModel2Screen*(a);
                isocenterPos.x = b.GetX();
                isocenterPos.y = b.GetY();
                IsocenterX = isocenterPos.x;
                IsocenterY = isocenterPos.y;
                break;
            }
        }

        for (auto itr = poiMap.begin(); itr != poiMap.end(); ++itr) {
            std::shared_ptr<TpsPoiGraphicObject> poiGo = itr->second;
            if (!poiGo->IsVisible()) {
                continue;
            }

            Point3D pt = poiGo->GetPosition();
            float resultX = 0.0f;
            float resultY = 0.0f;
            //translate coordinate system

            // since this render strategy is just used for easyplan, so here if the point is norm point,
            // then the point coordinate is in Col coor system, so the actual poi coor need to be calculated according to isocenter.
            if (poiGo->GetPoiType() == RtDbDef::NormalPoint
                || poiGo->GetPoiType() == RtDbDef::Calculation) {

                // apply rotation based on DRR image table angle.
                if(mDrrGraphicObject != nullptr) {
                    Mcsf::Vector3f a(0.0f,0.0f,0.0f);
                    // if the image is opposed to plan image, then the poi point x should be reversed.
                    if(mDrrGraphicObject->GetIsOpposedImage()) {
                        a.SetX(-pt.x);
                        a.SetY(pt.y);
                        a.SetZ(0.0f);
                    }
                    else {
                        a.SetX(pt.x);
                        a.SetY(pt.y);
                        a.SetZ(0.0f);
                    }
                    Mcsf::Vector3f b(0.0f,0.0f,0.0f);
                        
                    // rotation
                    double imgCaptureAngle = mDrrGraphicObject->GetImageCaptureTableAngle();
                    double imgCurrentTableAngle = mDrrGraphicObject->GetImageTableAngle();
                    double angleToRotate = imgCaptureAngle - imgCurrentTableAngle ;
                    if(mDrrGraphicObject->GetIsOpposedImage()) {
                        angleToRotate = -1 * angleToRotate * PI / 180.0;
                    }
                    else {
                        angleToRotate = angleToRotate * PI / 180.0;
                    }
                    Mcsf::Vector3f col0(cos(angleToRotate),sin(angleToRotate),0.0);
                    Mcsf::Vector3f col1(-sin(angleToRotate),cos(angleToRotate),0.0);
                    Mcsf::Vector3f col2(0.0 ,0.0 ,1.0);
                    Mcsf::Matrix3f matrix3f;
                    matrix3f.SetCol0(col0);
                    matrix3f.SetCol1(col1);
                    matrix3f.SetCol2(col2);

                    // zoom
                    float zoomFactor = mDrrGraphicObject->GetZoomFactor();
                    // set new matrix
                    Mcsf::Vector3f col3(zoomFactor,0.0f,0.0f);
                    Mcsf::Vector3f col4(0.0f,zoomFactor,0.0f);
                    Mcsf::Vector3f col5(0.0f ,0.0f ,1.0f);
                    // fixed       
                    Mcsf::Matrix3f  zoomMatrix3f;
                    zoomMatrix3f.SetCol0(col3);
                    zoomMatrix3f.SetCol1(col4);
                    zoomMatrix3f.SetCol2(col5);
                    matrix3f.Append(zoomMatrix3f);

                    //matImageModel2Screen.Append(matrix3f);
                    b = matrix3f * a;
                    resultX = (b.GetX() / (mWidth*mImagePlanPixelSpacingX)) * 2 + IsocenterX;
                    resultY = (b.GetY() / (mWidth*mImagePlanPixelSpacingY)) * 2 + IsocenterY;
                }
                else {
                    // if there is no image, the first beam's coordinate system will be the POI's coordinate system.
                    //TpsImageDataManager* imageDataManager = mDataRepository->GetImageDataManager();
                    //TpsBeamManager* beamManager = mDataRepository->GetBeamManager();
                    //std::string easyPlanSelectedBeamUID = "";
                    //imageDataManager->GetEasyPlanSelectedBeamUID(&easyPlanSelectedBeamUID);
                    //RtBeam* beam = nullptr;
                    //beam = beamManager->GetBeam(easyPlanSelectedBeamUID);
                    //if(beam != nullptr){
                    //    //easyPlanSelectedBeamImageUID = beam->get_relatedrtimageuid();
                    //    beam->get_beamnumber()
                    //}

                    resultX = (pt.x / (mWidth*mImagePlanPixelSpacingX)) * 2 + IsocenterX;
                    resultY = (pt.y / (mWidth*mImagePlanPixelSpacingY)) * 2 + IsocenterY;
                }
                //normal point or calculation point: draw hollow point
                glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
                glBegin(GL_LINE_LOOP);
            }
            else if (poiGo->GetPoiType() == RtDbDef::IsoCenter) {
                resultX = IsocenterX;
                resultY = IsocenterY;
                //Isocenter: draw solid point
                glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
                glBegin(GL_TRIANGLE_FAN);
                glVertex3d((resultX + 1)/2.0 * mWidth, (resultY + 1)/2.0 * mHeight, 0.0);
                //glVertex3d(resultX , resultY, 0.0);
            }
            else
            {
                resultX = pt.x;
                resultY = pt.y;
                //Isocenter: draw solid point
                glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
                glBegin(GL_TRIANGLE_FAN);
                glVertex3d(resultX  * mWidth, resultY*mHeight, 0.0);
            }

            //draw point
            const int n = 100;
            const float r = 2.5;
            for (int i = 0; i <100; ++i) {
                glVertex3d((resultX + 1)/2.0 * mWidth + r *std::cos(2 * PI*i / n), (resultY +1)/2.0 * mHeight + r *std::sin(2 * PI *i / n), 0.0);
            }

            glEnd();
        }
    }
    glDisable(GL_STENCIL_TEST);

    glFinish();
}


TPS_END_NAMESPACE
