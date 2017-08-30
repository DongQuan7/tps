////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Zuo kai kai.zuo@united-imaging.com
/// 
///  \file tps_rl_drrsettinggocollectionconverter.cpp
/// 
///  \brief class TpsDrrSettingGOCollectionConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/01/20
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_drrsettinggocollectionconverter.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsRenderLibrary/tps_rl_drrsettinggo.h"
#include "RtTpsRenderLibrary/tps_rl_drrsettinggocollection.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_drrsetting.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsDrrSettingGOCollectionConverter::TpsDrrSettingGOCollectionConverter(
    GO_TYPE goType)
    : TpsGOConverterBase(goType){

}

TpsDrrSettingGOCollectionConverter::~TpsDrrSettingGOCollectionConverter(){

}

void TpsDrrSettingGOCollectionConverter::SetModelWarehouse(ModelWarehouse* modelWarehouse){
    mModelWarehouse = modelWarehouse;
}

void TpsDrrSettingGOCollectionConverter::SetDataRepository(ITpsDataRepository* dataRepository){
    mDataRepository = dataRepository;
}

GraphicObjectPtr TpsDrrSettingGOCollectionConverter::CreateGraphicObject_i(){
    return std::shared_ptr<DRRSettingGraphicObjectCollection>(new DRRSettingGraphicObjectCollection());
}

bool TpsDrrSettingGOCollectionConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(PLAN);
    if (it == para.data_source_map.end()) return false;
    mPlanUid = it->second;
    return true;
}

bool TpsDrrSettingGOCollectionConverter::Update_i(GraphicObjectPtr go){
    auto drrsettingcollectionGO = std::dynamic_pointer_cast<DRRSettingGraphicObjectCollection>(go);
    if (drrsettingcollectionGO == nullptr) return false;
    TpsNormGroupManager *normalGroupManager = mDataRepository->GetNormgroupManager();
    std::vector<std::string> normalGroupUids = normalGroupManager->GetNormGroupUIDListByPlan(mPlanUid);
    RtDrrsetting *drrsetting = nullptr;
    for(int i = 0; i < normalGroupUids.size(); ++i)
    {
        normalGroupManager->GetDrrSetting(normalGroupUids[i], &drrsetting);
        if(drrsetting == nullptr) continue;
        auto drrsettingGo = std::shared_ptr<DRRSettingGraphicObject>(
            new DRRSettingGraphicObject());
        drrsettingGo->SetCT2DensityTableUid(drrsetting->get_ct2densityuid());
        drrsettingGo->SetCTThresholdMax(drrsetting->get_ctthresholdmaxvalue());
        drrsettingGo->SetCTThresholdMin(drrsetting->get_ctthresholdminvalue());
        drrsettingGo->SetElectronEnergy(drrsetting->get_electronenergy());
        drrsettingGo->SetNormalgroupUid(drrsetting->get_normalgroupuid());
        drrsettingcollectionGO->AddDrrSettingGo(drrsettingGo);
    }
    return true;
}

TPS_END_NAMESPACE  // end namespace tps