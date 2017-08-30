////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
/// 
///  \author  Liang Xiang  xiang.liang@united-imaging.com
/// 
///  \file tps_rl_drrcollection_graphic_object_converter.cpp
/// 
///  \brief class TpsEasyPlanDrrCollectionGOConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2017/7/12
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrcollection_graphicobject_converter.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_easyplandrrgraphicobjectconverter.h"

#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"


TPS_BEGIN_NAMESPACE

TpsEasyPlanDrrCollectionGOConverter::TpsEasyPlanDrrCollectionGOConverter(
GO_TYPE goType, TpsEasyPlanDrrGOConverter* easyPlanDrrConverter)
: TpsGOConverterBase(goType), mEasyPlanDrrConverter(easyPlanDrrConverter){

}

TpsEasyPlanDrrCollectionGOConverter::~TpsEasyPlanDrrCollectionGOConverter(){
    if (mEasyPlanDrrConverter){
        delete mEasyPlanDrrConverter;
        mEasyPlanDrrConverter = nullptr;
    }
}

void TpsEasyPlanDrrCollectionGOConverter::SetModelWarehouse(ModelWarehouse* modelWarehouse){
    mModelWarehouse = modelWarehouse;
    if (mEasyPlanDrrConverter){
        mEasyPlanDrrConverter->SetModelWarehouse(modelWarehouse);
    }
}

void TpsEasyPlanDrrCollectionGOConverter::SetDataRepository(ITpsDataRepository* dataRepository){
    mDataRepository = dataRepository;
    if (mEasyPlanDrrConverter){
        mEasyPlanDrrConverter->SetDataRepository(dataRepository);
    }
}

GraphicObjectPtr TpsEasyPlanDrrCollectionGOConverter::CreateGraphicObject_i(){

    return std::shared_ptr<EasyPlanDrrGraphicObjectCollection>(
        new EasyPlanDrrGraphicObjectCollection());
}

bool TpsEasyPlanDrrCollectionGOConverter::PrepareSourceData_i(const DataSourceMap& dataSourceMap){

    auto it = dataSourceMap.find(PLAN);
    auto uid = it->second;
    if (uid.empty()){
        TPS_LOG_DEV_ERROR<<"Failed to get easyplan uid: "<<uid;
        return false;
    }

    //get BeamList
    mBeamList.clear(); 
    mBeamList = mDataRepository->GetBeamManager()->GetBeamsByPlan(uid);
    //EasyPlan现在不会存在没有beam的情况
    if (mBeamList.size() == 0)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get BeamList by  plan uid: "<<uid;
        return false;
    }

    //get RtImageList
    mRtImageMap.clear();
    std::vector<RtBeam*>::iterator itBeam = mBeamList.begin();
    for(; itBeam != mBeamList.end(); itBeam++)
    {
        RtBeam* beam = *itBeam;
        TpsImage2DEntity* image2DEntity = nullptr;
        bool iRet = mDataRepository->GetImageDataManager()->Get2DImageDataByBeamUID(beam->get_uid(), image2DEntity);
        //beam没有关联的drr是正常情况
        if (!iRet) break;

        mRtImageMap[beam->get_uid()] = image2DEntity;
    }
    return true;
}

bool TpsEasyPlanDrrCollectionGOConverter::Update_i(GraphicObjectPtr go)
{
    std::shared_ptr<EasyPlanDrrGraphicObjectCollection> easyPlanDrrCollection = 
        std::dynamic_pointer_cast<EasyPlanDrrGraphicObjectCollection>(go);
    if (easyPlanDrrCollection == nullptr || mEasyPlanDrrConverter == nullptr)
    {
        return false;
    }

    GraphicObjectPtr goPtr = nullptr;
    std::map<std::string , TpsImage2DEntity*>::iterator itImage2D = mRtImageMap.begin();
    for(; itImage2D != mRtImageMap.end(); itImage2D++)
    {
        TpsImage2DEntity* image2D = itImage2D->second;
        std::string image2DUID = "";
        if (!image2D->GetUID(image2DUID))
        {
            TPS_LOG_DEV_ERROR<<"Failed to get image2D uid. ";
            return false;
        }
        mEasyPlanDrrConverter->SetImage2DData(image2D);

        goPtr = mEasyPlanDrrConverter->Convert();
        if (goPtr == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to convert easyPlanDrr to easyPlanDrr GO, which uid: "<<image2DUID;
            return false;
        }
        auto easyPlanDrrGO = std::dynamic_pointer_cast<EasyPlanDrrGraphicObejct>(goPtr);
        easyPlanDrrGO->SetBeamUID(itImage2D->first);
        easyPlanDrrCollection->AddDrr(image2DUID, easyPlanDrrGO);
    }
    return true;
}

};