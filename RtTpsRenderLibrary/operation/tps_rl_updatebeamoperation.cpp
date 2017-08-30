////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_updatebeamoperation.cpp
/// 
///  \brief class TpsUpdateBeamOperation implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/01/02
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_updatebeamoperation.h"

#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"

TPS_BEGIN_NAMESPACE

TpsUpdateBeamOperation::TpsUpdateBeamOperation(
    const std::string& ngUID, const std::string& beamUID)
    : mNgUid(ngUID), mBeamUID(beamUID){

}

TpsUpdateBeamOperation::~TpsUpdateBeamOperation(){

}

bool TpsUpdateBeamOperation::ModifyGraphicObject() const{

    RtNormgroup* ng = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(mNgUid, &ng) && ng == nullptr){
        TPS_LOG_DEV_ERROR<<"The plan does not exist which uid: "<<mNgUid;
        return false;
    }
    std::string planUID = ng->get_planuid();

    auto go = mModelWarehouse->GetModelObject(planUID + "|" + 
        GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    auto ngCollection = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    if (ngCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get normal group collection of try: "<<planUID;
        return false;
    }

    auto beamCollection = ngCollection->GetNormalGroup(mNgUid);
    if (beamCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get beam collection of plan: "<<mNgUid;
        return false;
    }

    auto beam = beamCollection->SelectBeam(mBeamUID);
    //beamCollection->SetActiveBeam(mBeamUID);

    auto converter = mConverterFactory->GetConverter(GO_TYPE_BEAM);
    if (converter == nullptr){
        TPS_LOG_DEV_ERROR<<"The cache collection converter does not exist.";
        return false;
    }

    GOConverterParameter para;
    para.obj_uid = mBeamUID;
    if (!converter->Update(para, beam)){
        TPS_LOG_DEV_ERROR<<"The cache collection converter does not exist.";
        return false;
    }
    beam->SetDirty(true);
    beamCollection->SetDirty(true);
    return true;

}

TPS_END_NAMESPACE