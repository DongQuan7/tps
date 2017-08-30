////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Xiaoqing.Shangguan Xiaoqing.Shangguan@united-imaging.com
/// 
///  \file    tps_rl_changect2densitytableoperation.cpp
/// 
///  \brief 
/// 
///  \version 1.0
/// 
///  \date    Mar. 25, 2015
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_updatedrrsettingoperation.h"

#include "RtTpsRenderLibrary/tps_rl_ct2density_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_ct2density_graphicobjectcollectionconverter.h"
#include "RtTpsRenderLibrary/tps_rl_drrsettinggo.h"
#include "RtTpsRenderLibrary/tps_rl_drrsettinggocollection.h"
#include "RtTpsRenderLibrary/tps_rl_drrsettinggocollectionconverter.h"

#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobejctconverter.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

UpdateDrrSettingOperation::UpdateDrrSettingOperation(
const std::string& imageUid, const std::string& normUid, 
const float& ctthresholdMin, const float& ctthresholdMax, 
const std::string& ct2DensityTableUid, const float& electronEnergy) : 
    mImageUid(imageUid), mNormalgroupUid(normUid), mctThresholdMin(ctthresholdMin), 
    mctThresholdMax(ctthresholdMax), mCt2DensityTableUid(ct2DensityTableUid), mElectronEnergy(electronEnergy){
}

UpdateDrrSettingOperation::~UpdateDrrSettingOperation(){

}

bool UpdateDrrSettingOperation::ModifyGraphicObject() const{

    if (mImageUid.empty() || mCt2DensityTableUid.empty()){
        TPS_LOG_DEV_ERROR<<"The Image UID or VOI UID is empty.";
        return false;
    }

    std::string planUid = mDataRepository->GetNormgroupManager()->GetPlanUIDByNormGroupUID(mNormalgroupUid);

    GraphicObjectPtr go = mModelWarehouse->GetModelObject(
        planUid + "|" + GOTypeHelper::ToString(GO_TYPE_DRRSETTING_COLLECTION));

    auto drrsettingGoCollection = dynamic_pointer_cast<DRRSettingGraphicObjectCollection>(go);
    if (go == nullptr || drrsettingGoCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get drr setting collection of plan: "<<planUid;
        return false;
    }

    auto drrsettingGo = drrsettingGoCollection->GetDrrSettingGo(mNormalgroupUid);
    if(drrsettingGo == nullptr)
    {
        drrsettingGo = std::shared_ptr<DRRSettingGraphicObject>(
            new DRRSettingGraphicObject());
        drrsettingGoCollection->AddDrrSettingGo(drrsettingGo);
    }
    drrsettingGo->SetNormalgroupUid(mNormalgroupUid);
    drrsettingGo->SetCT2DensityTableUid(mCt2DensityTableUid);
    drrsettingGo->SetElectronEnergy(mElectronEnergy);
    drrsettingGo->SetCTThresholdMin(mctThresholdMin);
    drrsettingGo->SetCTThresholdMax(mctThresholdMax);

    go->SetDirty(true);

    return true;
}
TPS_END_NAMESPACE  // end namespace tps