////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_addbeamoperation.cpp
/// 
///  \brief class TpsAddBeamOperation implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/28
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_addbeamoperation.h"

#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"



TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsAddBeamOperation::TpsAddBeamOperation(
    const std::string& planUID, const std::string& beamUID) : 
    mNormGroupUID(planUID), mBeamUID(beamUID){

}

TpsAddBeamOperation::~TpsAddBeamOperation(){

}

bool TpsAddBeamOperation::ModifyGraphicObject() const{

    if (mNormGroupUID.empty()){
        TPS_LOG_DEV_ERROR<<"The plan UID is empty.";
        return false;
    }

    RtNormgroup* normGroup = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(mNormGroupUID, &normGroup) || normGroup == nullptr){
        TPS_LOG_DEV_ERROR<<"The norm group does not exist which uid: "<<mNormGroupUID;
        return false;
    }
    std::string planUID = normGroup->get_planuid();

    auto go = mModelWarehouse->GetModelObject(
        planUID + "|" + GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    auto ngCollection = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    if (ngCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get normal group collection of try: "<<mNormGroupUID;
        return false;
    }
    ngCollection->SetActiveNormalGroup(mNormGroupUID);
    auto beamCollection = ngCollection->GetNormalGroup(mNormGroupUID);
    if (beamCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get beam collection of plan: "<<mNormGroupUID;
        return false;
    }

    auto converter = mConverterFactory->GetConverter(GO_TYPE_BEAM);
    if (converter == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get beam graphic object converter.";
        return false;
    }

    GOConverterParameter para;
    para.obj_uid = mBeamUID;
    std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> beamGO = converter->Convert(para);
    if (beamGO == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed convert beam graphic object.";
        return false;
    }
    std::shared_ptr<tps::TpsBeamGraphicObject> tpsBeamGO = dynamic_pointer_cast<TpsBeamGraphicObject>(beamGO);
    tpsBeamGO->SetActiveSegmentIndex(0);
    beamCollection->AddBeam(mBeamUID, dynamic_pointer_cast<TpsBeamGraphicObject>(beamGO));
    beamCollection->SetActiveBeam(mBeamUID);
    beamCollection->SetDirty(true);
    return true;
}
TPS_END_NAMESPACE  // end namespace tps