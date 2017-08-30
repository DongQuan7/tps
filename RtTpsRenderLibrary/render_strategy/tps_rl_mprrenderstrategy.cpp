////////////////////////////////////////////////////////////////////////////
///// \defgroup TPS of Radio Therapy Business Unit
/////  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
/////  All rights reserved.
/////
/////  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/////
/////  \file    tps_rl_mprrenderstrategy.cpp
/////  \brief   image windows
/////
/////  \version 1.0
/////  \date    Nov. 11, 2013
/////  \{
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_mprrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_algorithm.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3D/McsfViewer3DResource/mcsf_viewer3d_texture_3d.h"
#include "McsfMedViewer3DGraphicUtils/camera.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

double MPRRenderStrategy::MPR_TOLERANCE = 0.01;

MPRRenderStrategy::MPRRenderStrategy() : mMprAlgorithm(nullptr)
{

}


MPRRenderStrategy::~MPRRenderStrategy(void)
{

}

void MPRRenderStrategy::Render_i()
{
    if (mVolumeGO == nullptr || mMprGO == nullptr)
    {
        return;
    }

    if (!PrepareAlgorithm_i())
    {
        TPS_LOG_DEV_ERROR<<"Failed to prepare algorithm.";
        return;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glViewport(0, 0, mWidth, mHeight);

    glClearColor(0.05f, 0.05f, 0.05f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    mMprAlgorithm->Render();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

int MPRRenderStrategy::CacheChanges()
{
    if (mVolumeGO == nullptr || mMprGO == nullptr)
    {
        return -1;
    }

    bool dirty = mVolumeGO->IsDirty();
    dirty |= mMprGO->IsDirty();
    if (dirty) mDirty = true;

    return 0;
}

void MPRRenderStrategy::Initialize()
{
    mMprAlgorithm = new MprAlgorithm();
    mMprAlgorithm->Initialize();
}

void MPRRenderStrategy::Finalize()
{
    if (mMprAlgorithm)
    {
        mMprAlgorithm->Finalize();
        TPS_DEL_PTR(mMprAlgorithm);
    }
}

void MPRRenderStrategy::Resize(int iWidth, int iHeight)
{
    mWidth = iWidth;
    mHeight = iHeight;
    mDirty = true;
}

bool MPRRenderStrategy::PrepareAlgorithm_i()
{
    //set fixed image's world 2 volume model
    Matrix4x4 matWorld2VolumeM = mVolumeGO->GetModel2World().Inverse();

    //set view projection matrix & mpr model to world
    Point3D vertices[4];
    mMprGO->GetVertices(vertices);
    Vector3D normal;
    mMprGO->GetNormal(&normal);
    ProjectionCamera myCamera;
    myCamera.SetProjectionType(ProjectionCamera::PT_ORTHOGRAPHIC);
    myCamera.SetLookAt(mMprGO->GetCenter());
    myCamera.SetEye(mMprGO->GetCenter() + normal * 10.0);
    Vector3D vUp = vertices[3] - vertices[0];
    vUp.Normalize();
    myCamera.SetUpDirection(vUp);
    myCamera.SetNearClipDistance(8.0);
    myCamera.SetFarClipDistance(12.0);
    double dPlaneXinWorld = (vertices[1] - vertices[0]).Magnitude();
    double dPlaneYinWorld = (vertices[3] - vertices[0]).Magnitude();
    double dAspectRatioPlane = dPlaneXinWorld / dPlaneYinWorld;
    double dAspectRatioScreen = (double) mWidth / (double) mHeight;
    Matrix4x4 matRescale;
    if (dAspectRatioScreen > dAspectRatioPlane)
    {
        myCamera.SetAspectRatio(dAspectRatioScreen);
        myCamera.SetOrthoWindow(dPlaneYinWorld * dAspectRatioScreen, dPlaneYinWorld);
        matRescale = MakeScale(Vector3D(dAspectRatioScreen / dAspectRatioPlane, 1.0, 1.0));
    }
    else
    {
        myCamera.SetAspectRatio(dAspectRatioScreen);
        myCamera.SetOrthoWindow(dPlaneXinWorld, dPlaneXinWorld / dAspectRatioScreen);
        matRescale = MakeScale(Vector3D(1.0, dAspectRatioPlane / dAspectRatioScreen, 1.0));
    }
    Matrix4x4 matVP = myCamera.GetViewProjectionMatrix();
    Matrix4x4 matModel2World = mMprGO->GetModel2World();
    matModel2World.Append(matRescale);
    Matrix4x4 matMVP = matModel2World;
    matMVP.Prepend(matVP);
    Matrix4x4 matMprModel2VolumeModel = matWorld2VolumeM;
    matMprModel2VolumeModel.Append(matModel2World);
    mMprAlgorithm->SetMVPMatrix(matMVP);
    mMprAlgorithm->SetMprModel2VolumeModel(matMprModel2VolumeModel);

    //set casting vector
    double thickness = mMprGO->GetThickness();
    Vector3D castingVector = thickness * normal;
    mMprAlgorithm->SetCastingVector(castingVector);
    float fSteps = 1.0;
    double dMinThicknessInWorld = 0.0;
    TpsMprHelper::GetMinThickness(mVolumeGO, mMprGO, dMinThicknessInWorld);
    if (thickness < dMinThicknessInWorld *(1 +  MPR_TOLERANCE)){
        fSteps = 1.0;
    }
    else{
        double dVoxelSpacingInWorld = 0.0;
        double dVoxelSpacingP = 0.0;
        MPRAPI::GetMinSpacing(mVolumeGO, mMprGO, &dVoxelSpacingInWorld, &dVoxelSpacingP);
        fSteps = (float)std::max(1.0, std::ceil((thickness / dVoxelSpacingInWorld) - MPR_TOLERANCE));
    }
    mMprAlgorithm->SetCastingStepNum(fSteps);

    //set fixed image's volume data texture ID
    std::shared_ptr<Texture3D> texFixedVolume = mVolumeGO->GetTexture3D();
    if (texFixedVolume == nullptr){
        TPS_LOG_DEV_ERROR<<"The fixed volume's texture is null.";
        return false;
    }
    mMprAlgorithm->SetVolumeDataTexID(texFixedVolume->GetID());

    //set fixed image's window level
    double windowWidth, windowCenter;
    mMprGO->GetWindowing(windowWidth, windowCenter);
    mVolumeGO->RegulateWindowing(&windowWidth, &windowCenter);
    mMprAlgorithm->SetWindowLevel(windowCenter, windowWidth);

    //set pseudocolor table
    unsigned int pseudocolorTexID = mMprGO->GetPseudoColorTexID();
    mMprAlgorithm->SetPseudocolorTableTexID(pseudocolorTexID);
    unsigned int pseudocolorTableSize = mMprGO->GetPseudoColorTexSize();
    mMprAlgorithm->SetPseudocolorTableSize(pseudocolorTableSize);
    //set fusion weight
    //mFixedAlgorithm->SetAlpha(1.0f);
    //set mpr algorithm type
    int mprAlgorithm = 0;
    mMprGO->GetRenderAlgorithmType(&mprAlgorithm);
    mMprAlgorithm->SetMprAlgorithmType(MprAlgorithm::MprAlgorithmType(mprAlgorithm));
    //set color inverted
    mMprAlgorithm->SetIsColorInverted(mMprGO->IsColorInverted());

    //for passing only
    mMprAlgorithm->SetVPMat(matVP);
    mMprAlgorithm->SetModel2WorldMat(matModel2World);
    return true;
}
TPS_END_NAMESPACE

