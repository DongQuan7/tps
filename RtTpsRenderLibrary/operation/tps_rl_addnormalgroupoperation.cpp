////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_addnormalgroupoperation.cpp
/// 
///  \brief class AddNormalGroup implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/06/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_addnormalgroupoperation.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamcollectiongraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

AddNormalGroupOperation::AddNormalGroupOperation(
const std::string& planUID, const std::string& normGroupUID)
: mPlanUID(planUID), mNormGroupUID(normGroupUID){

}

AddNormalGroupOperation::~AddNormalGroupOperation(){

}

bool AddNormalGroupOperation::ModifyGraphicObject() const{
    
    auto go = mModelWarehouse->GetModelObject(
        mPlanUID + "|" + GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    auto ngCollection = dynamic_pointer_cast<NormalGroupCollection>(go);
    if (ngCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get normal group collection of try: "<<mPlanUID;
        return false;
    }

    NormalGroupPtr normalGroup = ngCollection->GetNormalGroup(mNormGroupUID);

    auto converter = mConverterFactory->GetConverter(GO_TYPE_BEAM_COLLECTION);
    if (converter == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get beam collection converter.";
        return false;
    }

    GOConverterParameter para;
    para.obj_uid = mNormGroupUID;
    normalGroup = std::dynamic_pointer_cast<
        TpsBeamGraphicObjectCollection>(converter->Convert(para));
    if (normalGroup == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed convert beam collection.";
        return false;
    }
    ngCollection->AddNormalGroup(mNormGroupUID, normalGroup);

    ngCollection->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE  // end namespace tps
