////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_mprfusionrenderstrategy.cpp
/// 
///  \brief class TpsMprFusionRenderStrategy implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/05/09
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_fusionrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_algorithm.h"
#include "RtTpsRenderLibrary/tps_rl_fusiongraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_fusionchessgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_antialiased_circle_painter.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3D/McsfViewer3DResource/mcsf_viewer3d_texture_3d.h"
#include "McsfMedViewer3DGraphicUtils/camera.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"

//#include "gl/glut.h"
using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

static double MPR_TOLERANCE = 0.01;

FusionRenderStrategy::FusionRenderStrategy()
    : mFloatAlgorithm(nullptr)
    , mCirclePainter(nullptr)
    , mFusionGO(nullptr)
    , mFloatVolume(nullptr)
    , mFloatMpr(nullptr)
    , mChessGO(nullptr)
    , mIndexCount(-1)
    , mVertexBuffer(nullptr)
    , mIndexBuffer(nullptr)
    , mNeedRefreshChessBoard(true)
{
    mDisplayRectInChessModel[0] = -0.5f;
    mDisplayRectInChessModel[1] = 0.5f;
    mDisplayRectInChessModel[2] = -0.5f;
    mDisplayRectInChessModel[3] = 0.5f;
}

FusionRenderStrategy::~FusionRenderStrategy()
{
    TPS_DEL_ARRAY(mVertexBuffer);
    TPS_DEL_ARRAY(mIndexBuffer);
}

void FusionRenderStrategy::Render_i()
{
    mHitTestData.clear();

    if (mVolumeGO == nullptr || mMprGO == nullptr)
    {
        return;
    }

    bool isFusionEnabled = (mFusionGO != nullptr) && mFusionGO->IsFusionEnabled();
    if (!isFusionEnabled)
    {
        MPRRenderStrategy::Render_i();
        return;
    }

    if (mFloatVolume == nullptr || mFloatMpr == nullptr)
    {
        return;
    }

    bool isFusionChess = isFusionEnabled && (
        mFusionGO->GetFusionType() == FUSION_TYPE_CHESS);
    if (isFusionChess && mChessGO == nullptr){
        return;
    }

    if (!PrepareAlgorithm_i() || !PrepareFloatAlgorithm_i())
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

    FUSION_TYPE fusionType = mFusionGO->GetFusionType();
    switch (fusionType) {
    case FUSION_TYPE_BLEND:
        RenderBlend_i();
        break;
    case FUSION_TYPE_CHESS:
        RenderChess_i();
        break;
    default:
        TPS_LOG_DEV_ERROR<<"Unrecognized fusion type: "<<fusionType;
        break;
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

int FusionRenderStrategy::CacheChanges()
{
    bool isFusionEnabled = (mFusionGO != nullptr) && mFusionGO->IsFusionEnabled();
    if (isFusionEnabled && (mFloatVolume == nullptr || mFloatMpr == nullptr))
    {
        return -1;
    }

    bool isFusionChess = isFusionEnabled && (
        mFusionGO->GetFusionType() == FUSION_TYPE_CHESS);
    if (isFusionChess && mChessGO == nullptr)
    {
        return -1;
    }

    if (isFusionEnabled)
    {
        bool dirty = mFusionGO->IsDirty();
        dirty |= mFloatVolume->IsDirty();
        dirty |= mFloatMpr->IsDirty();
        if (isFusionChess) 
        {
            dirty |= mChessGO->IsDirty();
            mNeedRefreshChessBoard = true;
        }
        if (dirty) mDirty = true;
    }

    return MPRRenderStrategy::CacheChanges();
}

void FusionRenderStrategy::Initialize()
{
    MPRRenderStrategy::Initialize();

    mFloatAlgorithm = new MprAlgorithm();
    mFloatAlgorithm->Initialize();
    mCirclePainter = new AntialiasedCirclePainter();
    mCirclePainter->Initialize();
}

void FusionRenderStrategy::Finalize()
{
    MPRRenderStrategy::Finalize();

    if (mFloatAlgorithm)
    {
        mFloatAlgorithm->Finalize();
        TPS_DEL_PTR(mFloatAlgorithm);
    }
    TPS_DEL_PTR(mCirclePainter);
}

void FusionRenderStrategy::Resize(int iWidth, int iHeight)
{
    MPRRenderStrategy::Resize(iWidth, iHeight);

    mDisplayRectInChessModel[0] = -0.5f;
    mDisplayRectInChessModel[1] = 0.5f;
    mDisplayRectInChessModel[2] = -0.5f;
    mDisplayRectInChessModel[3] = 0.5f;
    if (mWidth >= mHeight){
        mDisplayRectInChessModel[0] = -0.5f * float(mWidth) / mHeight;
        mDisplayRectInChessModel[1] = 0.5f * float(mWidth) / mHeight;
    }
    else{
        mDisplayRectInChessModel[2] = -0.5f * float(mHeight) / mWidth;
        mDisplayRectInChessModel[3] = 0.5f * float(mHeight) / mWidth;
    }
    mNeedRefreshChessBoard = true;
}

void FusionRenderStrategy::HitTest(double x, double y, HitTestResult& hitRes)
{
    x *= mWidth; y *= mHeight;
    Vector2D vecInter;
    for (auto it = mHitTestData.begin(); it != mHitTestData.end(); ++it){
        vecInter.x = x - it->second.x;
        vecInter.y = y - it->second.y;
        if (vecInter.x * vecInter.x + vecInter.y * vecInter.y
            <= HIT_TOLERANCE * HIT_TOLERANCE){
            hitRes._hitNum = (HIT_TEST_NUM)it->first;
            return;
        }
    }
}

bool FusionRenderStrategy::PrepareFloatAlgorithm_i()
{
    Matrix4x4 matVP = mMprAlgorithm->GetVPMat();
    Matrix4x4 matModel2World = mMprAlgorithm->GetModel2WorldMat();
    Matrix4x4 matWorld2VolumeM;
    Vector3D castingVector;
    RegAlg regAlg = mFusionGO->GetRegAlg();
    if (regAlg == Rigid)
    {
        matWorld2VolumeM = mFloatVolume->GetModel2World().Inverse();
        //fixed world c.s. to float world c.s.
        Matrix4x4 matFixed2FloatW = mFloatVolume->GetWorld2Patient().Inverse();
        matFixed2FloatW.Append(mFusionGO->GetRegistrationMatrix());
        matFixed2FloatW.Append(mVolumeGO->GetWorld2Patient());
        //view projection matrix
        matVP.Append(matFixed2FloatW.Inverse());
        //model 2 world
        matModel2World.Prepend(matFixed2FloatW);
        //casting vector
        castingVector = matFixed2FloatW.Transform(castingVector);
    }

    Matrix4x4 matMVP = matModel2World;
    matMVP.Prepend(matVP);
    Matrix4x4 matMprModel2VolumeModel = matWorld2VolumeM;
    matMprModel2VolumeModel.Append(matModel2World);
    mFloatAlgorithm->SetMVPMatrix(matMVP);
    mFloatAlgorithm->SetMprModel2VolumeModel(matMprModel2VolumeModel);
    mFloatAlgorithm->SetCastingVector(castingVector);
    mFloatAlgorithm->SetCastingStepNum(mMprAlgorithm->GetCastingStepNum());
    

    //set float image's volume data texture ID
    std::shared_ptr<Texture3D> texFloatVolume = regAlg == Rigid ? 
        mFloatVolume->GetTexture3D() : mFusionGO->GetFusionVolumeTexture();
    if (texFloatVolume == nullptr){
        TPS_LOG_DEV_ERROR<<"The float volume's texture is null.";
        return false;
    }
    mFloatAlgorithm->SetVolumeDataTexID(texFloatVolume->GetID());

    //set float image's window level
    double windowWidth, windowCenter;
    mFloatMpr->GetWindowing(windowWidth, windowCenter);
    mFloatVolume->RegulateWindowing(&windowWidth, &windowCenter);
    mFloatAlgorithm->SetWindowLevel(windowCenter, windowWidth);

    //set float image's pseudocolor table
    auto pseudocolorTexID = mFloatMpr->GetPseudoColorTexID();
    mFloatAlgorithm->SetPseudocolorTableTexID(pseudocolorTexID);
    auto pseudocolorTableSize = mFloatMpr->GetPseudoColorTexSize();
    mFloatAlgorithm->SetPseudocolorTableSize(pseudocolorTableSize);
    //set fusion image's fusion weight
    //mFixedAlgorithm->SetAlpha(mFusionGO->GetFixedAlpha());
    //mFloatAlgorithm->SetAlpha(mFusionGO->GetFloatAlpha());
    //set color inverted
    mFloatAlgorithm->SetIsColorInverted(mFloatMpr->IsColorInverted());
    return true;
}

void FusionRenderStrategy::RenderBlend_i()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0x01, 0x01);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    mMprAlgorithm->Render();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    float alpha = mFusionGO->GetFloatAlpha();
    glBlendColor(1.0, 1.0, 1.0, alpha);
    glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
    mFloatAlgorithm->Render();
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
}

void FusionRenderStrategy::RenderChess_i()
{
    if (!WriteStencilForChess_i()){
        TPS_LOG_DEV_ERROR<<"Failed to prepare stencil buffer for chess board.";
        return;
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glStencilFunc(GL_EQUAL, 0x01, 0x01);
    mMprAlgorithm->Render();
    glStencilFunc(GL_NOTEQUAL, 0x01, 0x01);
    mFloatAlgorithm->Render();

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    if (!RenderChessLine_i()){
        TPS_LOG_DEV_ERROR<<"Failed to render chess line.";
        return;
    }
    RenderHotSpots_i();
}

bool FusionRenderStrategy::WriteStencilForChess_i()
{
    if (mNeedRefreshChessBoard && !PrepareChessBoard_i()){
        TPS_LOG_DEV_ERROR<<"Failed to prepare chess board.";
        return false;
    }

    if (mIndexBuffer == nullptr || mVertexBuffer == nullptr || mIndexCount <= 0){
        TPS_LOG_DEV_ERROR<<"The chess board buffer is not ready.";
        return false;
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(mDisplayRectInChessModel[0], mDisplayRectInChessModel[1], 
        mDisplayRectInChessModel[2], mDisplayRectInChessModel[3], -1, 1);

    //write stencil buffer for chess
    glStencilMask(0xff);
    glClearStencil(0x0);
    glClear(GL_STENCIL_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0x01, 0x01);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    
    //float* colors = new float[27];
    //memset(colors, 0, 27 * sizeof(float));
    //for (int i=0; i<27; i+=3){
    //    colors[i] = 1.0f;
    //}
    //
    /*glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertexBuffer);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glDrawElements(GL_QUADS, quadsCount, GL_UNSIGNED_INT, vertexIndexBuffer);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
*/
    //checkGLStatus();

    //glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    for (int i=0; i < mIndexCount; ++i){
        glVertex3f(mVertexBuffer[mIndexBuffer[i] * 3], 
            mVertexBuffer[mIndexBuffer[i] * 3 + 1], 
            mVertexBuffer[mIndexBuffer[i] * 3 + 2]);
    }
    glEnd();
    glDisable(GL_STENCIL_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    return true;
}

bool FusionRenderStrategy::PrepareChessBoard_i()
{
    Matrix4x4 world2MprModel = mMprGO->GetModel2WorldMatrix().Inverse();
    //convert center & square size to mpr model
    Point3D center3D = mChessGO->GetChessCenter();
    //need project to handle page turn
    center3D = mMprGO->ProjectToPlane(center3D);
    center3D = world2MprModel.Transform(center3D);
    Point2D center(center3D.x, center3D.y);
    //transform the square size to mpr model
    float squareSize = mChessGO->GetSquareSize();
    Mcsf::MedViewer3D::Point3D vertices[4];
    mMprGO->GetVertices(vertices);
    Mcsf::MedViewer3D::Vector3D vecX = vertices[1] - vertices[0];
    vecX.Normalize();
    vecX *= squareSize;
    squareSize = world2MprModel.Transform(vecX).Magnitude();
    squareSize = TPS_MAX(MIN_SQUARE_SIZE, squareSize);

    //calculate real start
    float realStart[2] = {mDisplayRectInChessModel[0], mDisplayRectInChessModel[2]};
    if(center.x - mDisplayRectInChessModel[0] < -1e-6 || 
       center.x - mDisplayRectInChessModel[1] > 1e-6 || 
       center.y - mDisplayRectInChessModel[2] < -1e-6 || 
       center.y - mDisplayRectInChessModel[3] > 1e-6){
        TPS_LOG_DEV_ERROR<<"The center of chess board is out of range.";
        return false;
    }
    
    float squareCountX = (center.x - realStart[0]) / squareSize;
    if (abs(squareCountX - floor(squareCountX)) > 1e-6){
        squareCountX = ceil(squareCountX);
    }
    realStart[0] = center.x - squareCountX * squareSize;
    float squareCountY = (center.y - realStart[1]) / squareSize;
    if (abs(squareCountY - floor(squareCountY)) > 1e-6){
        squareCountY = ceil(squareCountY);
    }
    realStart[1] = center.y - squareCountY * squareSize;
    //set fixed cube flag
    bool isFixedCubeFlag;
    CHESS_DISPLAY_MODE mode = mChessGO->GetDisplayMode();
    if (mode == NORMAL_CHESS){
        isFixedCubeFlag = (int(squareCountX - 1) % 2 == (int(squareCountY) % 2));
    }
    else if (mode == ROW_ONLY){
        isFixedCubeFlag = int(squareCountY) % 2 == 0;
    }
    else{
        isFixedCubeFlag = int(squareCountX - 1) % 2 == 0;
    }

    //set chess board
    mHorizonalPts.clear(); mVerticalPts.clear();
    mHorizonalPts.push_back(realStart[0]);
    realStart[0] += squareSize;
    while (realStart[0] - mDisplayRectInChessModel[1] < -1e-6) {
        if (mode != ROW_ONLY) mHorizonalPts.push_back(realStart[0]);
        realStart[0] += squareSize;
    }
    mHorizonalPts.push_back(realStart[0]);
    mVerticalPts.push_back(realStart[1]);
    realStart[1] += squareSize;
    while (realStart[1] - mDisplayRectInChessModel[3] < -1e-6) {
        if (mode != COLUMN_ONLY) mVerticalPts.push_back(realStart[1]);
        realStart[1] += squareSize;
    }
    mVerticalPts.push_back(realStart[1]);

    //set vertex buffer
    if (mVertexBuffer) delete[] mVertexBuffer;
    if (mIndexBuffer) delete[] mIndexBuffer;

    unsigned int horizonalSize = mHorizonalPts.size();
    unsigned int verticalSize = mVerticalPts.size();
    mVertexBuffer = new float[horizonalSize * verticalSize * 3];
    unsigned int currentIndex;
    for (unsigned int i=0; i < verticalSize; ++i){
        for (unsigned int j=0; j < horizonalSize; ++j){
            currentIndex = i * horizonalSize + j;
            currentIndex *= 3;
            mVertexBuffer[currentIndex] = mHorizonalPts[j];
            mVertexBuffer[currentIndex + 1] = mVerticalPts[i];
            mVertexBuffer[currentIndex + 2] = 0.0f;
        }
    }
    //set vertex index buffer
    unsigned int maxQuadsSize = (horizonalSize - 1) * (verticalSize - 1) / 2 + 1;
    mIndexBuffer = new unsigned int[maxQuadsSize * 4];
    mIndexCount = 0;
    for (unsigned int i=0; i < verticalSize - 1; ++i){
        for (unsigned int j=0; j < horizonalSize - 1; ++j){
            if ((mode == NORMAL_CHESS && isFixedCubeFlag == (i%2 == j%2)) ||
                (mode == ROW_ONLY && isFixedCubeFlag == (i%2 == 0)) || 
                (mode == COLUMN_ONLY && isFixedCubeFlag == (j%2 == 0))) {
                mIndexBuffer[mIndexCount] = i * horizonalSize + j;
                mIndexBuffer[mIndexCount + 1] = i * horizonalSize + j + 1;
                mIndexBuffer[mIndexCount + 2] = (i + 1) * horizonalSize + j + 1;
                mIndexBuffer[mIndexCount + 3] = (i + 1) * horizonalSize + j;
                mIndexCount += 4;
            }
        }
    }
    return true;
}

bool FusionRenderStrategy::RenderChessLine_i()
{
    if (mHorizonalPts.empty() || mVerticalPts.empty()){
        TPS_LOG_DEV_ERROR<<"The chess positions is empty.";
        return false;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(mDisplayRectInChessModel[0], mDisplayRectInChessModel[1], 
        mDisplayRectInChessModel[2], mDisplayRectInChessModel[3], -1, 1);

    glLineWidth(1.0f);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
    //glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    //render chess line, color: white
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    for (int i = 1; i<mVerticalPts.size() - 1; ++i){
        glVertex3f(mDisplayRectInChessModel[0], mVerticalPts[i], 0.0f);
        glVertex3f(mDisplayRectInChessModel[1], mVerticalPts[i], 0.0f);
    }
    for (int i = 1; i<mHorizonalPts.size() - 1; ++i){
        glVertex3f(mHorizonalPts[i], mDisplayRectInChessModel[2], 0.0f);
        glVertex3f(mHorizonalPts[i], mDisplayRectInChessModel[3], 0.0f);
    }
    glEnd();
    //glDisable(GL_BLEND);
    //glDisable(GL_LINE_SMOOTH);
    //glLineWidth(1.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    return true;
}

void FusionRenderStrategy::RenderHotSpots_i()
{
    if (!mFusionGO->CanEdit()) return;

    Matrix4x4 world2MprModel = mMprGO->GetModel2WorldMatrix().Inverse();
    //convert center & square size to mpr model
    Point3D center3D = mChessGO->GetChessCenter();
    //need project to handle page turn
    center3D = mMprGO->ProjectToPlane(center3D);
    center3D = world2MprModel.Transform(center3D);
    Point2D center(center3D.x, center3D.y);
    //transform the square size to mpr model
    float squareSize = mChessGO->GetSquareSize();
    Mcsf::MedViewer3D::Point3D vertices[4];
    mMprGO->GetVertices(vertices);
    Mcsf::MedViewer3D::Vector3D vecX = vertices[1] - vertices[0];
    vecX.Normalize();
    vecX *= squareSize;
    squareSize = world2MprModel.Transform(vecX).Magnitude();
    squareSize = TPS_MAX(MIN_SQUARE_SIZE, squareSize);

    float radius = TPS_MAX(5.0f, 0.02 * squareSize * TPS_MIN(mWidth, mHeight));
    float ratio[2] = {
        mWidth >= mHeight ? float(mHeight) / mWidth : 1,
        mWidth >= mHeight ? 1 : float(mWidth) / mHeight};
    float radises[4] = { radius, radius, radius, radius };
    float centers[8], colors[12];
    bool isFilled[4] = { false, false, false, false };
    //pan hot spot

    centers[0] = (center.x * ratio[0] + 0.5) * mWidth;
    centers[1] = (center.y * ratio[1] + 0.5) * mHeight;
    mHitTestData[HIT_NUM_CHESS_TRANSLATE] = Mcsf::MedViewer3D::Point2D(centers[0], centers[1]);
    colors[0] = 0.f; colors[1] = 1.f; colors[2] = 0.f;
    //square size hot spot
    centers[2] = ((center.x + squareSize) * ratio[0] + 0.5) * mWidth;
    centers[3] = ((center.y - squareSize) * ratio[1] + 0.5) * mHeight;
    colors[3] = 1.f; colors[4] = 1.f; colors[5] = 0.f;
    mHitTestData[HIT_NUM_CHESS_SQUARE_SIZE] = Mcsf::MedViewer3D::Point2D(centers[2], centers[3]);
    int count = 2;
    //row only hot spot
    CHESS_DISPLAY_MODE mode = mChessGO->GetDisplayMode();
    if (mode != COLUMN_ONLY){
        centers[2 * count] = ((center.x + 0.5 * squareSize) * ratio[0] + 0.5) * mWidth;
        centers[2 * count + 1] = ((center.y - squareSize) * ratio[1] + 0.5) * mHeight;
        colors[3 * count] = 1.f; colors[3 * count + 1] = 0.f; colors[3 * count + 2] = 0.f;
        mHitTestData[HIT_NUM_CHESS_ROW_ONLY] = Mcsf::MedViewer3D::Point2D(
            centers[2 * count], centers[2 * count + 1]);
        count++;
    }
    //column only hot spot
    if (mode != ROW_ONLY){
        centers[2 * count] = ((center.x + squareSize) * ratio[0] + 0.5) * mWidth;
        centers[2 * count + 1] = ((center.y - 0.5 * squareSize) * ratio[1] + 0.5) * mHeight;
        colors[3 * count] = 1.f; colors[3 * count + 1] = 0.f; colors[3 * count + 2] = 0.f;
        mHitTestData[HIT_NUM_CHESS_COLUMN_ONLY] = Mcsf::MedViewer3D::Point2D(
            centers[2 * count], centers[2 * count + 1]);
        count++;
    }
    mCirclePainter->SetParas(centers, radises, colors, isFilled, count);
    mCirclePainter->Render();
}

void FusionRenderStrategy::DrawCircle_i(float cx, float cy, float r, int num_segments) 
{
    float theta = 2 * 3.1415926 / float(num_segments); 
    float c = cosf(theta);//precalculate the sine and cosine
    float s = sinf(theta);
    float t;

    float x = r;//we start at angle = 0 
    float y = 0; 

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    glBegin(GL_LINE_LOOP); 
    for(int ii = 0; ii < num_segments; ii++) 
    { 
        glVertex2f(x + cx, y + cy);//output vertex 

        //apply the rotation matrix
        t = x;
        x = c * x - s * y;
        y = s * t + c * y;
    } 
    glEnd(); 
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
}

void FusionRenderStrategy::CircleMidPoint_i(int x, int y, int radius)
{
    int p = 1 - radius;
    int deltaX = 0;
    int deltaY = radius;

    CirclePlotPoints_i(x, y, deltaX, deltaY);
    while(deltaX < deltaY){
        deltaX++;
        if (p < 0){
            p += 2 * deltaX + 1;
        }
        else{
            deltaY--;
            p += 2 * (deltaX - deltaY) + 1;
        }
        CirclePlotPoints_i(x, y, deltaX, deltaY);
    }
}

void FusionRenderStrategy::CirclePlotPoints_i(int x, int y, int deltaX, int deltaY)
{
    glBegin(GL_POINTS);
    glVertex2i(x + deltaX, y + deltaY);
    glVertex2i(x - deltaX, y + deltaY);
    glVertex2i(x + deltaX, y - deltaY);
    glVertex2i(x - deltaX, y - deltaY);
    glVertex2i(x + deltaY, y + deltaX);
    glVertex2i(x - deltaY, y + deltaX);
    glVertex2i(x + deltaY, y - deltaX);
    glVertex2i(x - deltaY, y - deltaX);
    glEnd();
}

//void checkGLStatus() 
//{
//    GLenum err =  glGetError(); 
//    switch (err) 
//    { 
//    case GL_NO_ERROR: 
//        printf("No error.\n"); 
//        break; 
//    case GL_INVALID_ENUM: 
//        printf("Invalid enum.\n"); 
//        break; 
//    case GL_INVALID_VALUE: 
//        printf("Invalid value.\n"); 
//        break; 
//
//    case GL_INVALID_OPERATION: 
//        printf("Invalid operation.\n"); 
//        break; 
//    case GL_INVALID_FRAMEBUFFER_OPERATION: 
//        printf("Invalid frame operation.\n"); 
//        break; 
//    case GL_OUT_OF_MEMORY: 
//        printf("Out of memory.\n"); 
//        break; 
//    case GL_STACK_UNDERFLOW: 
//        printf("Stack underflow.\n"); 
//        break; 
//    case GL_STACK_OVERFLOW: 
//        printf("Stack overflow.\n"); 
//        break; 
//    default: 
//        printf("Unknown error.\n"); 
//        break;
//    }
//}
TPS_END_NAMESPACE  // end namespace tps
