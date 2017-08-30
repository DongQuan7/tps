////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_applicator_render_strategy.cpp
/// 
///  \brief class ApplicatorRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/05/06
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_applicator_render_strategy.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_raii_utility.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_applicator_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_antialiased_circle_painter.h"

#define SAD 1000.0 //mm

TPS_BEGIN_NAMESPACE  // begin namespace tps

ApplicatorRenderStrategy::ApplicatorRenderStrategy() : 
mVolumeGO(nullptr), mCameraGO(nullptr), 
mNormalGroupCollection(nullptr), mCirclePainter(nullptr){

}

ApplicatorRenderStrategy::~ApplicatorRenderStrategy(){

}

void ApplicatorRenderStrategy::Render_i(){
    using namespace Mcsf::MedViewer3D;
    if (mVolumeGO == nullptr || mNormalGroupCollection == nullptr || mCameraGO == nullptr){
        return;
    }

    auto activeNG = mNormalGroupCollection->GetActiveNormalGroup();
    if (activeNG == nullptr){
        TPS_LOG_DEV_WARNING<<"There is no active normal group.";
        return;
    }
    std::shared_ptr<TpsBeamGraphicObject> beamGO = activeNG->GetActiveBeamGo();
    if(nullptr == beamGO) {
        TPS_LOG_DEV_WARNING<<"There is no active beam.";
        return;
    }
    if (!beamGO->GetVisible()) return;

    std::shared_ptr<ApplicatorGraphicObject> applicator = beamGO->GetApplicator();
    if (applicator == nullptr) return;

    std::shared_ptr<TpsSegmentGraphicObject> segment = beamGO->GetActiveSegmentGo();
    if (segment == nullptr){
        TPS_LOG_DEV_ERROR<<"The active segment is empty.";
        return;
    }

    Matrix4x4 matBeam2Pat;
    segment->GetStartCollimatorToPatMatrix(&matBeam2Pat);
    Matrix4x4 matBeam2World = matBeam2Pat;   // Beam2World
    Matrix4x4 matWorld2Patient = mVolumeGO->GetWorld2Patient();
    matBeam2World.Prepend(matWorld2Patient.Inverse());
    mCameraGO->AdaptFovViaAspect((double)mWidth / mHeight);
    Matrix4x4 matViewProjection = mCameraGO->GetViewProjectionMatrix();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_uiFbo); 
    glViewport(0, 0, mWidth, mHeight);

    float color[3] = {0.0f, 1.0f, 1.0f};
    RtDbDef::APPLICATOR_APERTURE_SHAPE shapeType = applicator->GetShapeType();
    //draw rectangle or square
    if (shapeType != RtDbDef::SYM_CIRCULAR){
        double board[4];
        if (shapeType == RtDbDef::SYM_SQUARE){
            double opening = applicator->GetOpening();
            board[0] = -0.5 * opening;
            board[1] = 0.5 * opening;
            board[2] = -0.5 * opening;
            board[3] = 0.5 * opening;
        }
        else {
            double openingX = applicator->GetOpeningX();
            double openingY = applicator->GetOpeningY();
            board[0] = -0.5 * openingX;
            board[1] = 0.5 * openingX;
            board[2] = -0.5 * openingY;
            board[3] = 0.5 * openingY;
        }
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixd(matBeam2World._m);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixd(matViewProjection._m);
        glColor4f(color[0], color[1], color[2], 1.0f);

        glBegin(GL_LINE_LOOP);
        glVertex3d(board[0], board[2], -1 * SAD);
        glVertex3d(board[1], board[2], -1 * SAD);
        glVertex3d(board[1], board[3], -1 * SAD);
        glVertex3d(board[0], board[3], -1 * SAD);
        glEnd();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    }
    //draw circular
    else {
        if (mCirclePainter == nullptr){
            TPS_LOG_DEV_ERROR<<"The circle painter is nullptr;";
            return;
        }
        double opening = applicator->GetOpening();
        Matrix4x4 mvp = matBeam2World;
        mvp.Prepend(matViewProjection);
        Point3D ptTmp = mvp.Transform(Point3D(0.0, 0.0, -SAD));
        float center[2] = {(ptTmp.x + 1) * 0.5 * mWidth, 
                           (ptTmp.y + 1) * 0.5 * mHeight};
        Vector3D vecTmp = mvp.Transform(Vector3D(opening, 0.0, 0.0));
        float radius = std::sqrt(vecTmp.x * 0.5 * mWidth * vecTmp.x * 0.5 * mWidth + 
            vecTmp.y * 0.5 * mHeight * vecTmp.y * 0.5 * mHeight);
        bool isFilled = false;
        mCirclePainter->SetParas(center, &radius, color, &isFilled, 1);
        mCirclePainter->Render();
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

int ApplicatorRenderStrategy::CacheChanges(){
    if (mVolumeGO == nullptr || mNormalGroupCollection == nullptr ||
        mCameraGO == nullptr){
        return -1;
    }

    if (mNormalGroupCollection->IsDirty() || mCameraGO->IsDirty() || 
        mVolumeGO->IsDirty()){
        mDirty = true;
    }
    return 0;
}

void ApplicatorRenderStrategy::Initialize(){
    mCirclePainter = new AntialiasedCirclePainter();
    mCirclePainter->Initialize();
}

void ApplicatorRenderStrategy::Finalize(){
    if (mCirclePainter){
        delete mCirclePainter;
        mCirclePainter = nullptr;
    }
}

void ApplicatorRenderStrategy::Resize(int iWidth, int iHeight){
    mWidth = iWidth;
    mHeight = iHeight;
}

void ApplicatorRenderStrategy::SetVolumeGraphicObj(
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volume){

    if (mVolumeGO != volume){
        mVolumeGO = volume;
        mDirty = true;
    }
}

void ApplicatorRenderStrategy::SetCameraGO(std::shared_ptr<CameraGraphicObject> go){
    if (mCameraGO != go){
        mCameraGO = go;
        mDirty = true;
    }
}

void ApplicatorRenderStrategy::SetNormalGroupCollection(
    std::shared_ptr<NormalGroupCollection> normalGroupCollection){

    if (mNormalGroupCollection != normalGroupCollection){
        mNormalGroupCollection = normalGroupCollection;
        mDirty = true;
    }
}
TPS_END_NAMESPACE  // end namespace tps