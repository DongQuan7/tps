//////////////////////////////////////////////////////////////////////////
/// \defgroup TDA of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  wenlong.zhao mailto:wenlong.zhao@united-imaging.com
///
///  \file    tps_rl_easyplan_measuredistancestrategy.cpp
///  \brief   tps easyplan measure distance strategy class
///
///  \version 1.0
///  \date    2014-11-10
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_measuredistancestrategy.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_measuregraphicobject.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

//MCSF
#include "Mcsf3DArithmetic/mcsf_3d_matrix3x3.h"
#include "Mcsf3DArithmetic/mcsf_3d_vector3f.h"

//OPENGL
#include "gl\glew.h"
#include "gl\GL.h"
#include "gl\GLU.h"
#include <cmath>

TPS_BEGIN_NAMESPACE

EasyPlanMeasureDistanceStrategy::EasyPlanMeasureDistanceStrategy()
{

}

EasyPlanMeasureDistanceStrategy::~EasyPlanMeasureDistanceStrategy()
{

}
void EasyPlanMeasureDistanceStrategy::Render_i()
{
    if(mEasyPlanMeasureDistanceGraphicObject == nullptr) {
        return;
    }

    bool actionStarted = false;
    bool actionDone = false;
    float startX = 0.0f;
    float startY = 0.0f;
    float cursorX = 0.0f;
    float cursorY = 0.0f;

    mEasyPlanMeasureDistanceGraphicObject->GetMeasureDistanceStartPoint(&startX, &startY);
    mEasyPlanMeasureDistanceGraphicObject->GetMeasureDistanceStopPoint(&cursorX, &cursorY);
    mEasyPlanMeasureDistanceGraphicObject->GetMeasureDistanceActionStatus(&actionStarted, &actionDone);
    RenderMeasureDistance(startX, startY, cursorX, cursorY, actionStarted, actionDone);

    return;
}

bool EasyPlanMeasureDistanceStrategy::TransformEasyPlanImageModelPointToScreenPoint( 
    Mcsf::MedViewer3D::Point2D& ptIn, Mcsf::MedViewer3D::Point2D& ptOut) {
        Mcsf::Matrix3f drrModel2ImageModelMatrix;
        Mcsf::Matrix3f matImageModel2DrrModel;
        //int easyPlanDisplayWidth = 0;
        //int easyPlanDisplayHeight = 0;
        
        //todo: it's dangerous to get data from data manager in rendering thread
        //TpsImageDataManager::GetInstance()->GetDrrModel2ImageModelMatrix(drrModel2ImageModelMatrix);

        //TpsImageDataManager::GetInstance()->GetEasyPlanDisplaySize(easyPlanDisplayWidth,
        //    easyPlanDisplayHeight);
        matImageModel2DrrModel = drrModel2ImageModelMatrix.Inverse();
        float scale[2];
        if (mWidth > mHeight/*easyPlanDisplayWidth > easyPlanDisplayHeight*/) {
            //scale[0] = float(easyPlanDisplayHeight) / easyPlanDisplayWidth * 2;
            scale[0] = float(mHeight) / mWidth * 2;
            scale[1] = 2;
        } else {
            scale[0] = 2;
            //scale[1] = float(easyPlanDisplayWidth) / easyPlanDisplayHeight * 2;
            scale[1] = float(mWidth) / mHeight * 2;
        }
        Mcsf::Matrix3f matDrrModel2Screen(Mcsf::Vector3f(scale[0], 0, 0), 
            Mcsf::Vector3f(0, scale[1], 0), Mcsf::Vector3f(0, 0, 1));
        Mcsf::Matrix3f matImageModel2Screen = matImageModel2DrrModel;
        matImageModel2Screen.Prepend(matDrrModel2Screen);
        Mcsf::Vector3f a = Mcsf::Vector3f((float)ptIn.x,(float)ptIn.y,1.0f);
        Mcsf::Vector3f b = matImageModel2Screen*(a);
        ptOut.x= b.GetX();
        ptOut.y = b.GetY();
        return true;
}

void EasyPlanMeasureDistanceStrategy::Initialize()
{

}

void EasyPlanMeasureDistanceStrategy::Finalize()
{

}

void EasyPlanMeasureDistanceStrategy::RenderMeasureDistance( float startX, float startY, 
    float cursorX, float cursorY, bool actionStarted, bool actionDone )
{
    if(actionStarted && !actionDone)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_uiFbo);
        glViewport(0, 0, mWidth, mHeight);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();


        glEnable(GL_STENCIL_TEST);
        glStencilMask(0x01);
        glStencilFunc(GL_ALWAYS, 0x01, 0x01);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

        glBegin(GL_LINES);
        glVertex3f(startX * 2 - 1.0f, startY * 2 - 1.0f, 0.0f);
        glVertex3f(cursorX * 2 - 1.0f, cursorY * 2 - 1.0f, 0.0f);

        glEnd();

        glDisable(GL_STENCIL_TEST);
        glFlush();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
}

void EasyPlanMeasureDistanceStrategy::SetEasyPlanMeasureDistanceGraphicObject( 
    std::shared_ptr<EasyPlanMeasureGraphicObject> easyPlanMeasureDistanceGo )
{
    if (mEasyPlanMeasureDistanceGraphicObject != easyPlanMeasureDistanceGo){
        mEasyPlanMeasureDistanceGraphicObject = easyPlanMeasureDistanceGo;
        mDirty = true;
    }
}

void EasyPlanMeasureDistanceStrategy::Resize( int iWidth, int iHeight ){
    mHeight = iHeight;
    mWidth = iWidth;
}

int EasyPlanMeasureDistanceStrategy::CacheChanges(){
    if(mEasyPlanMeasureDistanceGraphicObject == nullptr) {
        return -1;
    }

    mDirty = mEasyPlanMeasureDistanceGraphicObject->IsDirty();
    //mDirty |= m_spMPR->IsDirty();
    return 0;
}



TPS_END_NAMESPACE