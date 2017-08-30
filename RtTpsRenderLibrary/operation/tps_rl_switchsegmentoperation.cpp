////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_switchsegmentoperation.cpp
/// 
///  \brief class TpsSwithSegmentOperation
/// 
///  \version 1.0
/// 
///  \date    2014/4/15
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_switchsegmentoperation.h"

#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"


TPS_BEGIN_NAMESPACE

TpsSwithSegmentOperation::TpsSwithSegmentOperation(const std::string &beamUid, 
    int page) :  mBeamUid(beamUid),mPage(page) {

}

TpsSwithSegmentOperation::~TpsSwithSegmentOperation() {

}

bool TpsSwithSegmentOperation::ModifyGraphicObject() const {

    RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(mBeamUid);
    if (beam == nullptr){
        TPS_LOG_DEV_ERROR<<"The beam does not exist which uid: "<<mBeamUid;
        return false;
    }
    std::string normGroupUid = beam->get_normgroupuid();

    RtNormgroup* normalGroup = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(normGroupUid, &normalGroup) && normalGroup == nullptr){
        TPS_LOG_DEV_ERROR<<"The normal group does not exist which uid: "<<normGroupUid;
        return false;
    }
    std::string planUID = normalGroup->get_planuid();

    auto go = mModelWarehouse->GetModelObject(planUID + "|" + 
        GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    auto ngCollection = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    if (ngCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get normal group collection of try: "<<planUID;
        return false;
    }

    auto beamGroup = ngCollection->GetNormalGroup(normGroupUid);
    if (beamGroup == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get beam collection of plan: "<<normGroupUid;
        return false;
    }
    auto beamGo = beamGroup->SelectBeam(mBeamUid);
    //beamGroup->SetActiveBeam(mBeamUid);
    if(nullptr == beamGo) {
        TPS_LOG_DEV_ERROR<<"failed to get beam from beam collection!"<<mBeamUid;
        return false;
    }

    int currentIndex = mPage - 1;
    beamGo->SetActiveSegmentIndex(currentIndex);
    beamGo->CorrectActiveSegment();
    if (beamGo->GetBeamType() == ARC)
    {
        beamGo->UpdateCombinationFieldEdges();
    }
    beamGroup->SetDirty(true);
    return true;
}



TPS_END_NAMESPACE