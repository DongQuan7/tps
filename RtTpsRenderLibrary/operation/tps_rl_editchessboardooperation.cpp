////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_editchessboardooperation.cpp
/// 
///  \brief class EditChessBoard implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/05/29
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_editchessboardooperation.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_fusionchessgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_fusiongraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

//#include "McsfMedViewer3DArithmetic/arithmetic.h"

#ifndef DISTANCE_BETWEEN_POINT2D
#define DISTANCE_BETWEEN_POINT2D(a, b) \
    (sqrt(((a.x) - (b.x)) * ((a.x) - (b.x)) + \
        ((a.y) - (b.y)) * ((a.y) - (b.y))))
#endif

TPS_BEGIN_NAMESPACE   // begin namespace tps

EditChessBoardOperation::EditChessBoardOperation(
double x, double y, int actionType, DISPLAY_SIZE displaySize)
: mX(x), mY(y), mMouseActionType(actionType), mDisplaySize(displaySize){

}

EditChessBoardOperation::~EditChessBoardOperation(){

}

bool EditChessBoardOperation::ModifyGraphicObject() const{
    auto go = mModelWarehouse->GetModelObject(mChessGoKey);
    auto chess = std::dynamic_pointer_cast<FusionChessGraphicObject>(go);
    if (chess == nullptr){
        TPS_LOG_DEV_ERROR<<"The fusion chess graphic object does not exist.";
        return false;
    }

    go = mModelWarehouse->GetModelObject(mFusionGoKey);
    auto fusionGo = std::dynamic_pointer_cast<FusionGraphicObject>(go);
    if (fusionGo == nullptr){
        TPS_LOG_DEV_ERROR<<"The fusion graphic object does not exist.";
        return false;
    }
    if (!fusionGo->CanEdit()){
        TPS_LOG_DEV_WARNING<<"The chess board can not be modified.";
        return false;
    }

    go = mModelWarehouse->GetModelObject(mMprKey);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get mpr graphic object using key:"<<mMprKey;
        return false;
    }

    switch(mMouseActionType){
    case MOVE:
        MouseOver_i(fusionGo, chess, mpr);
        break;
    case DOWN:
        BeginEdit_i(fusionGo, chess, mpr);
        break;
    case DRAG:
        EditChess_i(chess, mpr);
        break;
    case UP:
        EndEdit_i(chess);
        break;
    default:
        break;
    }
    return true;
}

HIT_TEST_NUM EditChessBoardOperation::HitTest_i(FusionGoPtr fusionGo, ChessGoPtr chess, MprGoPtr mpr) const{

    if (fusionGo->GetFusionType() == FUSION_TYPE_BLEND){
        return HIT_NUM_NONE;
    }

    double tolerance;
    double ratio[2] = {1.0, 1.0};
    if (mDisplaySize.width >= mDisplaySize.height){
        ratio[0] = double(mDisplaySize.width) / mDisplaySize.height;
        tolerance = 5.0 / mDisplaySize.height;
    }
    else{
        ratio[1] = double(mDisplaySize.height) / mDisplaySize.width;
        tolerance = 5.0 / mDisplaySize.width;
    }
    Mcsf::MedViewer3D::Point2D ptCurrent(mX - 0.5, mY -0.5);
    ptCurrent.x *= ratio[0];
    ptCurrent.y *= ratio[1];

    Mcsf::MedViewer3D::Matrix4x4 world2MprModel = mpr->GetModel2WorldMatrix().Inverse();
    //convert center & square size to mpr model
    Mcsf::MedViewer3D::Point3D center3D = chess->GetChessCenter();
    //need project to handle page turn
    center3D = mpr->ProjectToPlane(center3D);
    chess->SetChessCenter(center3D);
    center3D = world2MprModel.Transform(center3D);
    Mcsf::MedViewer3D::Point2D center(center3D.x, center3D.y);
    if (DISTANCE_BETWEEN_POINT2D(center, ptCurrent) <= tolerance){
        return HIT_NUM_CHESS_TRANSLATE;
    }

    //transform the square size to mpr model
    float squareSize = chess->GetSquareSize();
    Mcsf::MedViewer3D::Point3D vertices[4];
    mpr->GetVertices(vertices);
    Mcsf::MedViewer3D::Vector3D vecX = vertices[1] - vertices[0];
    vecX.Normalize();
    vecX *= squareSize;
    squareSize = world2MprModel.Transform(vecX).Magnitude();
    squareSize = TPS_MAX(MIN_SQUARE_SIZE, squareSize);
    Mcsf::MedViewer3D::Point2D ptZoom(center.x + squareSize, center.y - squareSize);
    if (DISTANCE_BETWEEN_POINT2D(ptZoom, ptCurrent) <= tolerance){
        return HIT_NUM_CHESS_SQUARE_SIZE;
    }

    Mcsf::MedViewer3D::Point2D ptRowOnly(center);
    ptRowOnly.x += 0.5 * squareSize;
    ptRowOnly.y -= squareSize;
    if (DISTANCE_BETWEEN_POINT2D(ptRowOnly, ptCurrent) <= tolerance){
        return HIT_NUM_CHESS_ROW_ONLY;
    }

    Mcsf::MedViewer3D::Point2D ptColumnOnly(center);
    ptColumnOnly.x += squareSize;
    ptColumnOnly.y -= 0.5 * squareSize;
    if (DISTANCE_BETWEEN_POINT2D(ptColumnOnly, ptCurrent) <= tolerance){
        return HIT_NUM_CHESS_COLUMN_ONLY;
    }
    return HIT_NUM_NONE;
}

void EditChessBoardOperation::MouseOver_i(FusionGoPtr fusionGo, ChessGoPtr chess, MprGoPtr mpr) const{
    HIT_TEST_NUM hitNum = HitTest_i(fusionGo, chess, mpr);
    chess->SetHit(hitNum);
}

void EditChessBoardOperation::BeginEdit_i(FusionGoPtr fusionGo, ChessGoPtr chess, MprGoPtr mpr) const{
    HIT_TEST_NUM hitNum = HitTest_i(fusionGo, chess, mpr);
    CHESS_DISPLAY_MODE mode = chess->GetDisplayMode();
    switch(hitNum){
    case HIT_NUM_CHESS_TRANSLATE:
        chess->SetEditMode(CHESS_TRANSLATE);
        break;
    case HIT_NUM_CHESS_SQUARE_SIZE:
        chess->SetEditMode(CHANGE_SQUARE_SIZE);
        break;
    case HIT_NUM_CHESS_ROW_ONLY:
        chess->SetEditMode(CHESS_IDLE);
        if (mode == ROW_ONLY) chess->SetDisplayMode(NORMAL_CHESS);
        else if(mode == NORMAL_CHESS) chess->SetDisplayMode(ROW_ONLY);
        else return;
        chess->SetDirty(true);
        break;
    case HIT_NUM_CHESS_COLUMN_ONLY:
        chess->SetEditMode(CHESS_IDLE);
        if (mode == COLUMN_ONLY) chess->SetDisplayMode(NORMAL_CHESS);
        else if(mode == NORMAL_CHESS) chess->SetDisplayMode(COLUMN_ONLY);
        else return;
        chess->SetDirty(true);
        break;
    default:
        break;
    }
}

void EditChessBoardOperation::EditChess_i(ChessGoPtr chess, MprGoPtr mpr) const{
    
    CHESS_EDIT_MODE editMode = chess->GetEditMode();
    if (editMode == CHESS_IDLE) return;
    double ratio[2] = {1.0, 1.0};
    if (mDisplaySize.width >= mDisplaySize.height){
        ratio[0] = double(mDisplaySize.width) / mDisplaySize.height;
    }
    else{
        ratio[1] = double(mDisplaySize.height) / mDisplaySize.width;
    }
    Mcsf::MedViewer3D::Point2D ptCurrent(
        TPS_CLAMP(mX, 0, 1) - 0.5, TPS_CLAMP(mY, 0, 1) -0.5);
    ptCurrent.x *= ratio[0];
    ptCurrent.y *= ratio[1];
    ptCurrent.x += 0.5; ptCurrent.y += 0.5;
    Mcsf::MedViewer3D::Point3D currentPt3D = mpr->GetWorldCoordinate(ptCurrent);

    if (editMode == CHESS_TRANSLATE){
        chess->SetChessCenter(currentPt3D);
        chess->SetDirty(true);
    }
    else if(editMode == CHANGE_SQUARE_SIZE){
        Mcsf::MedViewer3D::Point3D vertices[4];
        mpr->GetVertices(vertices);
        Mcsf::MedViewer3D::Vector3D vecDirct = vertices[1] - vertices[3];
        vecDirct.Normalize();

        Mcsf::MedViewer3D::Point3D center = chess->GetChessCenter();
        Mcsf::MedViewer3D::Vector3D vec(currentPt3D - center);
        double len = vec.DotProduct(vecDirct) * 0.5 * sqrt(2.0);
        chess->SetSquareSize(len);
        chess->SetDirty(true);
    }
}

void EditChessBoardOperation::EndEdit_i(ChessGoPtr chess) const{

    chess->SetEditMode(CHESS_IDLE);
}
TPS_END_NAMESPACE  // end namespace tps