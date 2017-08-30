////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Wang.Zhendong zhendong.wang@united-imaging.com
/// 
///  \file    tps_rl_change_ct2densitytable_operation.cpp
/// 
///  \brief 
/// 
///  \version 1.0
/// 
///  \date    Mar. 22, 2016
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_change_ct2densitytable_operation.h"

#include "RtTpsRenderLibrary/tps_rl_ct2density_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_ct2density_graphicobjectcollectionconverter.h"
#include "RtTpsRenderLibrary/tps_rl_drrsettinggocollection.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

ChangeCT2DensityTableOperation::ChangeCT2DensityTableOperation(
    const std::string& imageUid,
    const std::string& normUid,
    const std::string& ct2DensityTableUid) :
    mImageUid(imageUid),
    mNormalgroupUid(normUid),
    mCt2DensityTableUid(ct2DensityTableUid) {
}

ChangeCT2DensityTableOperation::~ChangeCT2DensityTableOperation() {

}

bool ChangeCT2DensityTableOperation::ModifyGraphicObject() const {
    if (mImageUid.empty() || mCt2DensityTableUid.empty()) {
        TPS_LOG_DEV_ERROR<<"The Image UID or VOI UID is empty.";
        return false;
    }

    std::string planUid = mDataRepository->GetNormgroupManager()->GetPlanUIDByNormGroupUID(mNormalgroupUid);

    GraphicObjectPtr go = mModelWarehouse->GetModelObject(
    planUid + "|" + GOTypeHelper::ToString(GO_TYPE_DRRSETTING_COLLECTION));

    auto drrsettingGoCollection = dynamic_pointer_cast<DRRSettingGraphicObjectCollection>(go);
    if (go == nullptr || drrsettingGoCollection == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed get drr setting collection of plan: "<<planUid;
        return false;
    }

    auto drrsettingGo = drrsettingGoCollection->GetDrrSettingGo(mNormalgroupUid);
    if (drrsettingGo == nullptr) {
        drrsettingGo = std::shared_ptr<DRRSettingGraphicObject>(
            new DRRSettingGraphicObject());
        drrsettingGoCollection->AddDrrSettingGo(drrsettingGo);
    }
    drrsettingGo->SetNormalgroupUid(mNormalgroupUid);
    drrsettingGo->SetCT2DensityTableUid(mCt2DensityTableUid);
    go->SetDirty(true);

    return true;
}
TPS_END_NAMESPACE  // end namespace tps