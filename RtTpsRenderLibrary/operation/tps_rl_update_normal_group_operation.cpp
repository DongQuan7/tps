////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_update_normal_group_operation.cpp
/// 
///  \brief class UpdateNormalGroup implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/08/28
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_update_normal_group_operation.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

UpdateNormalGroupOperation::UpdateNormalGroupOperation(
    const std::string& planUID, const std::string& ngUID) : 
    mPlanUID(planUID), mNormGroupUID(ngUID){

}

UpdateNormalGroupOperation::~UpdateNormalGroupOperation(){

}

bool UpdateNormalGroupOperation::ModifyGraphicObject() const{
    if (mNormGroupUID.empty() || mPlanUID.empty()){
        TPS_LOG_DEV_ERROR<<"The input parameter is invalid.";
        return false;
    }

    RtNormgroup* ng = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(mNormGroupUID, &ng) || ng == nullptr){
        TPS_LOG_DEV_ERROR<<"The normal group is empty.";
        return false;
    }

    auto go = mModelWarehouse->GetModelObject(
        mPlanUID + "|" + GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    auto ngCollection = dynamic_pointer_cast<NormalGroupCollection>(go);
    if (ngCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get normal group collection of try: "<<mPlanUID;
        return false;
    }

    NormalGroupPtr normalGroup = ngCollection->GetNormalGroup(mNormGroupUID);
    normalGroup->SetIsElectronNormalGroup(
        ng->get_radiationtype() == RtDbDef::RADIATION_ELETRON);
    normalGroup->SetIsJoined(ng->get_isjoined());

    RtMachine* machine = nullptr;
    mDataRepository->GetMachineManager()->GetMachine(ng->get_machineuid(), machine);
    double fieldLimits[4] =
    {
        machine->get_xmin_1(),
        machine->get_xmax_2(),
        machine->get_ymin_1(),
        machine->get_ymax_2()
    };


    normalGroup->SetFieldLimits(fieldLimits);

    ngCollection->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE  // end namespace tps
