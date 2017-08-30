////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_displaysegmentoutlineoperation.cpp
/// 
///  \brief DisplayCombinedSegmentOperation 
/// 
///  \version 1.0
/// 
///  \date    2014/4/18
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_displaysegmentoutlineoperation.h"

#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsFramework/tps_rl_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

DisplayCombinedSegmentOperation::DisplayCombinedSegmentOperation(const std::string &planUid,
const std::string &beamUid) :mPlanUid(planUid),mBeamUid(beamUid){

}

DisplayCombinedSegmentOperation::~DisplayCombinedSegmentOperation() {

}

bool DisplayCombinedSegmentOperation::ModifyGraphicObject()  const {

    auto beamGoGroup = ModelWarehouse::GetInstance()->GetModelObject(
        mPlanUid +"|"+ TPS_GO_BEAM_COLLECTION);
    auto beamGroup = dynamic_pointer_cast<TpsBeamGraphicObjectCollection>(beamGoGroup);
    if(nullptr == beamGoGroup || nullptr == beamGroup ) {
        TPS_LOG_DEV_ERROR<<"get beam go collection failed!"<<mPlanUid;
        return false;
    }
    auto beamGo = beamGroup->SelectBeam(mBeamUid);
    if(nullptr == beamGo) {
        TPS_LOG_DEV_ERROR<<"failed to get beam from beam collection!"<<mBeamUid;
        return false;
    }



    std::map<std::string,std::shared_ptr<TpsSegmentGraphicObj>> segmentList = beamGo->GetSegmentList();

    for(auto itr = segmentList.begin(); itr != segmentList.end(); ++itr) {

        


    }
    return true;

}


TPS_END_NAMESPACE