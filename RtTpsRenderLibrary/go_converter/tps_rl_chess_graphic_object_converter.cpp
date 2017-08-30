////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_chess_graphic_object_converter.cpp
/// 
///  \brief class ChessGoConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/12/16
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_chess_graphic_object_converter.h"

#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"

#include "Mcsf3DArithmetic/mcsf_3d_matrix4x4.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_fusionsecondarydatamanager.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_fusionchessgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

ChessGoConverter::ChessGoConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType), mMpr(nullptr){

}

ChessGoConverter::~ChessGoConverter(){

}

GraphicObjectPtr ChessGoConverter::CreateGraphicObject_i(){

    return std::shared_ptr<FusionChessGraphicObject>(new FusionChessGraphicObject());
}

bool ChessGoConverter::PrepareSourceData_i(const GOConverterParameter& para){
    mMpr = nullptr;
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end())
    {
        TPS_LOG_DEV_ERROR<<"Can not find IMAGE data source.";
        return false;
    }

    //get mpr graphic object
    FIRST_POSTFIX_COMPONENT section = GOTypeHelper::GetFirstPostfixComponent(mGOType);
    GO_TYPE mprGoType = GOTypeHelper::ComposeAsGOType(MPR, section);
    std::string mprKey = it->second + "|" + GOTypeHelper::ToString(mprGoType);
    auto go = mModelWarehouse->GetModelObject(mprKey);
    mMpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mMpr == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get mpr graphic object using key: "<<mprKey;
        return false;
    }

    return true;
}

bool ChessGoConverter::Update_i(GraphicObjectPtr go){
    auto chessGo = std::dynamic_pointer_cast<FusionChessGraphicObject>(go);
    if (mMpr == nullptr || chessGo == nullptr) return false;

    Mcsf::MedViewer3D::Point3D center = mMpr->GetCenter();

    Mcsf::MedViewer3D::Point3D vertices[4];
    if (!mMpr->GetVertices(vertices)){
        TPS_LOG_DEV_ERROR<<"Failed to get mpr vertices.";
        return false;
    }
    double squareSize = (vertices[1] - vertices[0]).Magnitude() * 0.5;
    chessGo->SetChessCenter(center);
    chessGo->SetSquareSize(squareSize);
    chessGo->SetDisplayMode(NORMAL_CHESS);
    chessGo->SetEditMode(CHESS_IDLE);
    return true;
}
TPS_END_NAMESPACE  // end namespace tps