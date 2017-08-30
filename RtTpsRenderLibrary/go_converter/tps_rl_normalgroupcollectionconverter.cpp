////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_normalgroupcollectionconverter.cpp
/// 
///  \brief class NormalGroupCollectionConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/06/10
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollectionconverter.h"

#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamcollectiongraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

NormalGroupCollectionConverter::NormalGroupCollectionConverter(
GO_TYPE goType, TpsBeamCollectionGOConverter* converter)
    : TpsGOConverterBase(goType), mNormalGroupConverter(converter)/*, mPlan(nullptr)*/{
    
}

NormalGroupCollectionConverter::~NormalGroupCollectionConverter(){
    if (mNormalGroupConverter){
        delete mNormalGroupConverter;
        mNormalGroupConverter = nullptr;
    }
}

void NormalGroupCollectionConverter::SetModelWarehouse(ModelWarehouse* modelWarehouse){
    mModelWarehouse = modelWarehouse;
    if (mNormalGroupConverter){
        mNormalGroupConverter->SetModelWarehouse(modelWarehouse);
    }
}

void NormalGroupCollectionConverter::SetDataRepository(ITpsDataRepository* dataRepository){
    mDataRepository = dataRepository;
    if (mNormalGroupConverter){
        mNormalGroupConverter->SetDataRepository(dataRepository);
    }
}

GraphicObjectPtr NormalGroupCollectionConverter::CreateGraphicObject_i(){
    return std::shared_ptr<NormalGroupCollection>(new NormalGroupCollection());
}

bool NormalGroupCollectionConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(PLAN);
    if (it == para.data_source_map.end()) return false;

    mNormGroupUIDList = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(it->second);
    mPlan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(it->second, &mPlan) || mPlan == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get plan, uid: "<<it->second;
        return false;
    }
    return true;
}

bool NormalGroupCollectionConverter::Update_i(GraphicObjectPtr go){
    auto collection = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    if (mPlan == nullptr || collection == nullptr || mNormalGroupConverter == nullptr){
        return false;
    }

    collection->ClearAll();
    collection->SetSeriesUID(mPlan->get_imageseriesuid());

    GraphicObjectPtr normalGroup = nullptr;
    GOConverterParameter para;
    for (auto it = mNormGroupUIDList.begin(); it != mNormGroupUIDList.end(); ++it){
        para.obj_uid = *it;
        normalGroup = mNormalGroupConverter->Convert(para);
        if (normalGroup == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to convert normal group which uid: "<<para.obj_uid;
            return false;
        }
        collection->AddNormalGroup(para.obj_uid, 
            std::dynamic_pointer_cast<TpsBeamGraphicObjectCollection>(normalGroup));
    }
    return true;
}

TPS_END_NAMESPACE  // end namespace tps