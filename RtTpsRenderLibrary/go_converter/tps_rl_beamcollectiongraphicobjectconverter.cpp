////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_beamcollectiongraphicobjectconverter.cpp
/// 
///  \brief class TpsBeamCollectionGOConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/25
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_beamcollectiongraphicobjectconverter.h"

#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"

#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobjectconverter.h"
//#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"


TPS_BEGIN_NAMESPACE

TpsBeamCollectionGOConverter::TpsBeamCollectionGOConverter(
    GO_TYPE goType, TpsBeamGOConverter* beamConverter)
    : TpsGOConverterBase(goType), mBeamConverter(beamConverter)
    , mNormalGroup(nullptr), mMachine(nullptr){

}

TpsBeamCollectionGOConverter::~TpsBeamCollectionGOConverter(){

    if (mBeamConverter){
        delete mBeamConverter;
        mBeamConverter = nullptr;
    }
}

void TpsBeamCollectionGOConverter::SetModelWarehouse(ModelWarehouse* modelWarehouse){
    mModelWarehouse = modelWarehouse;
    if (mBeamConverter){
        mBeamConverter->SetModelWarehouse(modelWarehouse);
    }
}

void TpsBeamCollectionGOConverter::SetDataRepository(ITpsDataRepository* dataRepository){
    mDataRepository = dataRepository;
    if (mBeamConverter){
        mBeamConverter->SetDataRepository(dataRepository);
    }
}

GraphicObjectPtr TpsBeamCollectionGOConverter::CreateGraphicObject_i(){

    return std::shared_ptr<TpsBeamGraphicObjectCollection>(
        new TpsBeamGraphicObjectCollection());
}

bool TpsBeamCollectionGOConverter::PrepareSourceData_i(const GOConverterParameter& para){

    if (para.obj_uid.empty()){
        TPS_LOG_DEV_ERROR<<"Failed to get normal group which uid: "<<para.obj_uid;
        return false;
    }

    mNormalGroup = nullptr; 
    mBeamList.clear(); 
    mIecCSList.clear(); 
    mMachine = nullptr; 
    mFluenceMapList.clear();
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(para.obj_uid, &mNormalGroup) || mNormalGroup == nullptr){
        TPS_LOG_DEV_ERROR<<"The normal group is empty.";
        return false;
    }

    auto beamMng = mDataRepository->GetBeamManager();
    std::vector<std::string> beamList = beamMng->GetBeamUIDListByNormgroupUID(para.obj_uid);
    RtBeam* beam;
    FluenceMapEntity* fluenceMap;
    TpsIECCoordinateSystem* iecCs;
    for (int i = 0; i < beamList.size(); ++i){
        beam = beamMng->GetBeam(beamList[i]);
        fluenceMap = beamMng->GetFluenceMap(beamList[i]);
        iecCs = beamMng->GetBeamCS(beamList[i]);
        if (beam != nullptr && iecCs != nullptr) {
            mBeamList.push_back(beam);
            mIecCSList.push_back(iecCs);
            mFluenceMapList.push_back(fluenceMap);
        }
    }

    mMachine = nullptr;
    if (!mDataRepository->GetMachineManager()->GetMachine(
        mNormalGroup->get_machineuid(), mMachine) || mMachine == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get machine.";
        return false;
    }
    return true;
}

bool TpsBeamCollectionGOConverter::Update_i(GraphicObjectPtr go){

    std::shared_ptr<TpsBeamGraphicObjectCollection> beamCollection = 
        std::dynamic_pointer_cast<TpsBeamGraphicObjectCollection>(go);
    if (beamCollection == nullptr || mBeamConverter == nullptr || 
        mBeamList.size() != mFluenceMapList.size() || mBeamList.size() != mIecCSList.size()){
        return false;
    }

    //beamCollection->SetIsJoined(mNormalGroup->GetIsJoined());
    // rcc：这里需要等强强的enum定义完成后，替换成新的
    beamCollection->SetIsElectronNormalGroup(
        mNormalGroup->get_radiationtype() == RtDbDef::RADIATION_ELETRON);
    beamCollection->SetIsJoined(mNormalGroup->get_isjoined());
    //todo, 后端没有approve状态
    beamCollection->SetIsApproved(false);
    beamCollection->SetSAD(mMachine->get_sad());
    double fieldLimits[4] = {
        mMachine->get_xmin_1(),
        mMachine->get_xmax_2(),
        mMachine->get_ymin_1(),
        mMachine->get_ymax_2()};
    beamCollection->SetFieldLimits(fieldLimits);
    beamCollection->RemoveAll();

    GraphicObjectPtr beam = nullptr;
    std::string beamUID;
    mBeamConverter->SetNormalGroup(mNormalGroup);
    mBeamConverter->SetMachine(mMachine);
    for (int i = 0; i < mBeamList.size(); ++i){
        beamUID = std::move(mBeamList[i]->get_uid());
        mBeamConverter->SetBeam(mBeamList[i]);
        mBeamConverter->SetIecCS(mIecCSList[i]);
        mBeamConverter->SetFluenceMapEntity(mFluenceMapList[i]);
        beam = mBeamConverter->Convert();
        if (beam == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to convert beam to beam GO, which uid: "<<beamUID;
            return false;
        }
        beamCollection->AddBeam(beamUID, std::dynamic_pointer_cast<TpsBeamGraphicObject>(beam));
    }
    return true;
}
TPS_END_NAMESPACE
