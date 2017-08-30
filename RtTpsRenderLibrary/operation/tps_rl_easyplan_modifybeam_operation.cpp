////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \Author Cheng Yilong yilong.cheng@united-imaging.com
/// 
///  \file tps_rl_easyplan_modifybeam_operation.cpp
/// 
///  \brief class TpsEasyPlanModifyBeamOperation
/// 
///  \version 1.0
/// 
///  \date    2014/11/06
////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_modifybeam_operation.h"

#include "McsfMedViewer3DArithmetic/point3d.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"

#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_block_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedbeam.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

//#include <math>

TPS_BEGIN_NAMESPACE

TpsEasyPlanModifyBeamOperation::TpsEasyPlanModifyBeamOperation(const std::string& beamUID) {
    mBeamUID = beamUID;
    mDrrImageUID = "";
}

TpsEasyPlanModifyBeamOperation::~TpsEasyPlanModifyBeamOperation(){

}

bool TpsEasyPlanModifyBeamOperation::ModifyGraphicObject() const{
    // get beam
    RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(mBeamUID);
    if (beam == nullptr){
        TPS_LOG_DEV_ERROR<<"The beam does not exist with UID: "<<mBeamUID;
        return false;
    }

    // get the segment(in heap?)
    std::vector<RtBeamsegment*> vecSegments = beam->get_beamsegments();
    if (vecSegments.empty()){
        TPS_LOG_DEV_ERROR<<"Segment Not Found!";
        return false;
    }
	float jawPos[4];
	jawPos[0] = vecSegments[0]->get_startmlcshape()->get_xlowerjawpos();
	jawPos[1] = vecSegments[0]->get_startmlcshape()->get_xupperjawpos();
	jawPos[2] = vecSegments[0]->get_startmlcshape()->get_ylowerjawpos();
	jawPos[3] = vecSegments[0]->get_startmlcshape()->get_yupperjawpos();


    // update drr go
    if(mDrrImageUID != "") {
        auto go = mModelWarehouse->GetModelObject(mDrrImageUID + "|" + 
            GOTypeHelper::ToString(GO_TYPE_EASYPLAN_DRR));
        if(nullptr == go){
            TPS_LOG_DEV_ERROR<<"failed to get easyplan drr go ! image uid:"
                <<mDrrImageUID;
            return false;
        }
        auto easyPlanDrrGO = std::dynamic_pointer_cast<EasyPlanDrrGraphicObejct>(go);
        if(nullptr == easyPlanDrrGO){
            TPS_LOG_DEV_ERROR<<"failed to dynamic cast to easyplan drr go in "
                <<"TpsEasyPlanSetRotationOperation::ModifyGraphicObject()";
            return false;
        }
        easyPlanDrrGO->SetImageTableAngle(vecSegments[0]->get_starttablerotationangle());
        easyPlanDrrGO->SetDirty(true);
    }
    
    std::string normGroupUID = beam->get_normgroupuid();
    RtNormgroup *normGroup = nullptr;

    if(!mDataRepository->GetNormgroupManager()->GetNormGroup(normGroupUID, &normGroup)
        || normGroup == nullptr) {
        TPS_LOG_DEV_ERROR<<"Normgroup Not Found!";
        return false;
    }
    std::string planUID = normGroup->get_planuid();
    // get beam graphic object, so that we can get segment go list
    //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/~
    //get segment go
    auto ngCollection = mModelWarehouse->GetModelObject(
        planUID + "|" + GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    auto goNormGroupCollection = std::dynamic_pointer_cast<NormalGroupCollection>(ngCollection);
    if (goNormGroupCollection == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed get normgroup collection.";
        return false;
    }

    auto goBeamCollection = goNormGroupCollection->GetNormalGroup(normGroupUID);
    if (goBeamCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get easy plan beam collection of try: ";
        return false;
    }

    std::shared_ptr<TpsBeamGraphicObject> beamGO = goBeamCollection->SelectBeam(mBeamUID);
    if(nullptr == beamGO) {
        TPS_LOG_DEV_ERROR<<"failed to get active easy plan beam go";
        return false;
    }

    std::shared_ptr<TpsSegmentGraphicObject> segmentGO = beamGO->GetActiveSegmentGo();
    if(segmentGO == nullptr) {
        TPS_LOG_DEV_ERROR<<"failed to get active easy plan segment go";
        return false;
    }
    
    segmentGO->SetJawPosition(jawPos);
    segmentGO->SetStartGantryAngle((float)(vecSegments[0]->get_startgantryangle()));
    segmentGO->SetCollimatorAngle((float)(vecSegments[0]->get_startcollimatorangle()));
    segmentGO->SetDirty(true);
    goBeamCollection->SetDirty(true);
    return true;
}

void TpsEasyPlanModifyBeamOperation::SetDrrImageUID(const std::string &drrImageUID) {
    mDrrImageUID = drrImageUID;
}

std::string TpsEasyPlanModifyBeamOperation::GetDrrImageUID() const {
    return mDrrImageUID;
}
TPS_END_NAMESPACE
