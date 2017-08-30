////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_block_graphic_object_converter.cpp
/// 
///  \brief class BlockGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/03/17
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_block_graphic_object_converter.h"

#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_block.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_block_graphic_object.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

BlockGOConverter::BlockGOConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType), mApertureBlock(nullptr), mShieldingBlock(nullptr), mGoType(goType)
{
}

BlockGOConverter::~BlockGOConverter(){

}

GraphicObjectPtr BlockGOConverter::CreateGraphicObject_i(){

    return std::shared_ptr<BlockGraphicObject>(new BlockGraphicObject());
}

bool BlockGOConverter::PrepareSourceData_i(const GOConverterParameter& para){
    if (nullptr == mDataRepository){
        TPS_LOG_DEV_ERROR<<"mDataRepository is null.";
        return false;
    }
    RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(para.obj_uid);
    if (beam == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get beam with uid: "<<para.obj_uid;
        return false;
    }
    if (GO_TYPE_APERTURE_BLOCK == mGoType){
        mApertureBlock = beam->get_aperture_block();
        mShieldingBlock = nullptr;
    }
    else{
        mApertureBlock = nullptr;
        mShieldingBlock = beam->get_shield_block();
    }

    if (nullptr == mApertureBlock && nullptr == mShieldingBlock){
        TPS_LOG_DEV_ERROR<<"The beam has no block.";
        return false;
    }
    return true;
}

bool BlockGOConverter::Update_i(GraphicObjectPtr go)
{
    auto block = std::dynamic_pointer_cast<BlockGraphicObject>(go);
    if (block == nullptr){
        return false;
    }
    if (nullptr == mApertureBlock && nullptr == mShieldingBlock){
        return false;
    }
    //block->SetBlockType((RtDbDef::BLOCK_TYPE)mBlock->get_blocktype());

    std::vector<db_Point2f> points = 
        nullptr != mApertureBlock ? mApertureBlock->get_points() : mShieldingBlock->get_points();
    std::vector<Mcsf::MedViewer3D::Point2D> vecData;
    vecData.reserve(points.size());
    for (size_t i = 0; i < points.size(); ++i){
        vecData.push_back(Mcsf::MedViewer3D::Point2D(points[i].x, points[i].y));
    }
    block->SetBlockData(vecData);
    return true;
}
TPS_END_NAMESPACE  // end namespace tps