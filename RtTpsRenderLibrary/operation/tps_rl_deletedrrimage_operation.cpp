////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Xiang.Liang xiang.liang@united-imaging.com
/// 
///  \file tps_rl_easyplan_seleteimage_operation.h
/// 
///  \brief class TpsDeleteDrrImageOperation
/// 
///  \version 1.0
/// 
///  \date    2017/7/30
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tps_logger.h"

#include "RtTpsFramework/tps_fw_operationbase.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"

#include "RtTpsRenderLibrary/tps_rl_deletedrrimage_operation.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgocollection.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"

TPS_BEGIN_NAMESPACE

TpsDeleteDrrImageOperation::TpsDeleteDrrImageOperation(const std::string &beamUID, const std::string &imageUID) :
mBeamUID(beamUID), mImageUID(imageUID)
{

}

TpsDeleteDrrImageOperation::~TpsDeleteDrrImageOperation() {

}

bool TpsDeleteDrrImageOperation::ModifyGraphicObject() const {
    try
    {
        RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(mBeamUID);
        if (beam == nullptr)
        {
            TPS_LOG_DEV_ERROR<<"Can't get beam from manager by beam uid:"<<mBeamUID;
            return false;
        }
        std::string planUID  = mDataRepository->GetNormgroupManager()->GetPlanUIDByNormGroupUID(beam->get_normgroupuid());
        if (planUID == "")
        {
            TPS_LOG_DEV_ERROR<<"Can't get planUID from manager by beam uid:"<<mBeamUID;
            return false;
        }

        auto go = mModelWarehouse->GetModelObject(
            planUID + "|" + GOTypeHelper::ToString(GO_TYPE_RT_IMAGE_COLLECTION));
        if(nullptr == go) {
            TPS_LOG_DEV_ERROR<<"failed to get easyplan drr go collection ! plan uid:"
                <<planUID;
            return nullptr;
        }

        auto drrCollectionGO = std::dynamic_pointer_cast<EasyPlanDrrGraphicObjectCollection> (go);
        drrCollectionGO->DeleteDrr(mImageUID);
        drrCollectionGO->SetDirty(true);
        return true;
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Exception happened in TpsDeleteDrrImageOperation::ModifyGraphicObject():"<<ex.what();
        return false;
    }
}

TPS_END_NAMESPACE