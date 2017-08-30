////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_block_graphic_object.cpp
/// 
///  \brief class BlockGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/03/17
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_block_graphic_object.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

BlockGraphicObject::BlockGraphicObject(){

}

BlockGraphicObject::~BlockGraphicObject(){
    
}

RtDbDef::BLOCK_TYPE BlockGraphicObject::GetBlockType() const{
    return mBlockType;
}

void BlockGraphicObject::SetBlockType(RtDbDef::BLOCK_TYPE type){
    mBlockType = type;
}

void BlockGraphicObject::SetBlockData(const std::vector<Mcsf::MedViewer3D::Point2D>& blockData){
    mBlockData = blockData;
}

void BlockGraphicObject::GetBlockData(std::vector<Mcsf::MedViewer3D::Point2D>*& blockData){
    blockData = &mBlockData;
}

TPS_END_NAMESPACE  // end namespace tps