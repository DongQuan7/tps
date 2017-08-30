////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  xi.chen xi.chen@united-imaging.com
/// 
///  \file tps_rl_highlightbeamoperation.cpp
/// 
///  \brief class HighlightBeamOperation implementation
/// 
///  \version 1.0
/// 
///  \date    2014/06/10
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_highlightbeamoperation.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

HighlightBeamOperation::HighlightBeamOperation(
    const std::string &beamUID, 
    const std::string &normGroupUID,
    const std::string &planUID,
    const std::string &imageUID) : 
    mBeamUID(beamUID), mNormGroupUID(normGroupUID), 
    mPlanUID(planUID), mImageUID(imageUID){
}

HighlightBeamOperation::~HighlightBeamOperation(){

}

bool HighlightBeamOperation::ModifyGraphicObject() const{
    if (mPlanUID.empty()) {
        TPS_LOG_DEV_ERROR<<"The plan uid is empty!";
        return false;
    }

    auto go = mModelWarehouse->GetModelObject(
        mPlanUID + "|" + GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    auto ngCollection = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    if (ngCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get normal group collection of try: "<<mPlanUID;
        return false;
    }
    ngCollection->SetActiveNormalGroup(mNormGroupUID);
    ngCollection->SetDirty(true);

    auto beamCollection = ngCollection->GetNormalGroup(mNormGroupUID);
    if (beamCollection != nullptr)
    {
        beamCollection->SetActiveBeam(mBeamUID);
        beamCollection->SetDirty(true);
    }
    return true;
}
TPS_END_NAMESPACE  // end namespace tps
