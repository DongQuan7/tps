//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_rl_edit_block_operation.cpp 
///  \brief   nudge block
///
///  \version 1.0
///  \date  2015/03/31  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_update_block_operation.h"

#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_graphic_object_converter_factory_base.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_block_graphic_object_converter.h"
#include "RtTpsRenderLibrary/tps_rl_block_graphic_object.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

UpdateBlockOperation::UpdateBlockOperation(const CertifiedMlcEditInfo &certNudgeInfo)
: mNudgeInfo(certNudgeInfo) {

}

UpdateBlockOperation::~UpdateBlockOperation() {

}

bool UpdateBlockOperation::ModifyGraphicObject() const {
    auto beam = mDataRepository->GetBeamManager()->GetBeam(mNudgeInfo.mBeamUid);

    if (beam == nullptr){
        TPS_LOG_DEV_ERROR<<"The beam does not exist with UID: "<<mNudgeInfo.mBeamUid;
        return false;
    }

    std::string normGroupUID = beam->get_normgroupuid();
    RtNormgroup *normGroup = nullptr;

    if(!mDataRepository->GetNormgroupManager()->GetNormGroup(normGroupUID, &normGroup)
        || normGroup == nullptr) {
            TPS_LOG_DEV_ERROR<<"Normgroup Not Found!";
            return false;
    }

    std::string planUID = normGroup->get_planuid();

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

    std::shared_ptr<TpsBeamGraphicObject> beamGO = goBeamCollection->SelectBeam(mNudgeInfo.mBeamUid);
    if(nullptr == beamGO) {
        TPS_LOG_DEV_ERROR<<"failed to get active easy plan beam go";
        return false;
    }

    GOConverterParameter para;
    para.obj_uid = mNudgeInfo.mBeamUid;
    auto blockApertureGOConverter = mConverterFactory->GetConverter(GO_TYPE_APERTURE_BLOCK);
    auto blockApertureGO = blockApertureGOConverter->Convert(para);
    if (blockApertureGO != nullptr) {
        beamGO->SetApertureBlock(std::dynamic_pointer_cast<BlockGraphicObject>(blockApertureGO));
    }
    else{
        TPS_LOG_DEV_WARNING<<"failed to convert aperture block go";
        //return false;
    }

    auto blockShieldingGOConverter = mConverterFactory->GetConverter(GO_TYPE_SHIELDING_BLOCK);
    auto blockShieldingGO = blockShieldingGOConverter->Convert(para);
    if (blockShieldingGO != nullptr) {
        beamGO->SetShieldingBlock(std::dynamic_pointer_cast<BlockGraphicObject>(blockShieldingGO));
    }
    else{
        TPS_LOG_DEV_WARNING<<"failed to convert shielding block go";
        //return false;
    }

    beamGO->SetDirty(true);
    return true;
}

void UpdateBlockOperation::Initialize() {

}

TPS_END_NAMESPACE