//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Wang Zhendong  zhendong.wang@united-imaging.com
/// 
///  \file block_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2016/05/05
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "block_converter.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

bool BlockConverter::ConvertToALG(const RtBlock& tpsBlock, std::shared_ptr<RTFWK::RTFWKDLDOBlock>* algBlock)
{
    algBlock->reset(new RTFWK::RTFWKDLDOBlock(false));
    (*algBlock)->SetAvoidMarginX(tpsBlock.get_avoidmarginx());
    (*algBlock)->SetAvoidMarginY(tpsBlock.get_avoidmarginy());
    (*algBlock)->SetAvoidVoiUid(tpsBlock.get_avoidvoiuid());
    (*algBlock)->SetBeamUID(tpsBlock.get_beamuid());
    (*algBlock)->SetBlockDivergence((RTFWK::BLOCK_DIVERGENCE)tpsBlock.get_blockdivergence());
    (*algBlock)->SetBlockMountingPosition((RTFWK::BLOCK_MOUNTING_POSITION)tpsBlock.get_blockmountingposition());
    (*algBlock)->SetBlockName(tpsBlock.get_blockname());
    (*algBlock)->SetBlockNumber(tpsBlock.get_blocknumber());
    (*algBlock)->SetBlockThickness(tpsBlock.get_blockthickness());
    (*algBlock)->SetBlockType((RTFWK::BLOCK_TYPE)tpsBlock.get_blocktype());
//    (*algBlock)->SetIsApertureBlock(tpsBlock.get_isapertureblock());
    (*algBlock)->SetDBUID(tpsBlock.get_uid());
    (*algBlock)->SetIsAutoShape(tpsBlock.get_isautoshape());
    (*algBlock)->SetMaterialID(tpsBlock.get_materialid());

    std::vector<RTFWK::RTFWKDLDOPoint2D*> algBlockPoits;
    std::vector<db_Point2f> blockPoints = tpsBlock.get_points();
    RTFWK::RTFWKDLDOPoint2D *algBlockPoint = nullptr;
    std::vector<db_Point2f>::iterator itblockPoint = blockPoints.begin();
    for (; itblockPoint != blockPoints.end(); ++itblockPoint)
    {
        db_Point2f blockPt = *itblockPoint;
        algBlockPoint = new RTFWK::RTFWKDLDOPoint2D();
        algBlockPoint->x = blockPt.x;
        algBlockPoint->y = blockPt.y;
        algBlockPoits.push_back(algBlockPoint);
    }
    (*algBlock)->SetPoints(algBlockPoits);
    (*algBlock)->SetSourceToTrayDistance(tpsBlock.get_sourcetotraydistance());
    (*algBlock)->SetTargetMarginX(tpsBlock.get_targetmarginx());
    (*algBlock)->SetTargetMarginY(tpsBlock.get_targetmarginy());
    (*algBlock)->SetTargetVoiUid(tpsBlock.get_targetvoiuid());
    return true;
}


bool BlockConverter::CreateALGBlockList(
    std::vector<RtBlock*>& tpsBlockList,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOBlock>>* algBlockList)
{
    std::vector<RtBlock*>::iterator itTpsBlock = tpsBlockList.begin();
    for (; itTpsBlock != tpsBlockList.end(); ++itTpsBlock)
    {
        if (nullptr == *itTpsBlock)
        {
            TPS_LOG_DEV_ERROR << "tpsBlock is null";
            return false;
        }

        std::shared_ptr<RTFWK::RTFWKDLDOBlock> algBlock;

        BlockConverter::ConvertToALG(*(*itTpsBlock), &algBlock);
        algBlockList->push_back(algBlock);
    }
    return true;
}



TPS_END_NAMESPACE