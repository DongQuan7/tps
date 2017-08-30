////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_graphicobjectconverterbase.cpp
/// 
///  \brief class TpsGOConverterBase implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/07/24
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/graphic_object_interface.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

class StatusResumer
{
public:
    StatusResumer(ConverterStatus& status) : mStatus(status) {}
    ~StatusResumer() { mStatus = Cvtr_Idle; }

private:
    ConverterStatus& mStatus;
};

TpsGOConverterBase::TpsGOConverterBase(GO_TYPE goType) 
    : mGOType(goType), mStatus(Cvtr_Idle)
    , mModelWarehouse(nullptr), mDataRepository(nullptr)
{

}

TpsGOConverterBase::~TpsGOConverterBase()
{

}

GraphicObjectPtr TpsGOConverterBase::Convert(const GOConverterParameter& para)
{
    StatusResumer resumer(mStatus = Cvtr_Convert);
    if (mModelWarehouse == nullptr || mDataRepository == nullptr){
        TPS_LOG_DEV_ERROR<<"Model warehouse or data repository is not ready.";
        return nullptr;
    }

    GraphicObjectPtr go = this->CreateGraphicObject_i();

    if (!this->PrepareSourceData_i(para)){
        TPS_LOG_DEV_ERROR<<"Failed to get source data.";
        return nullptr;
    }

    if (!this->Update_i(go)){
        TPS_LOG_DEV_ERROR<<"Failed to convert graphic object.";
        return nullptr;
    }
    go->SetDirty(true);
    return go;
}

GraphicObjectPtr TpsGOConverterBase::Convert()
{
    StatusResumer resumer(mStatus = Cvtr_Convert);
    if (mModelWarehouse == nullptr || mDataRepository == nullptr){
        TPS_LOG_DEV_ERROR<<"Model warehouse or data repository is not ready.";
        return nullptr;
    }

    GraphicObjectPtr go = this->CreateGraphicObject_i();

    if (!this->Update_i(go)){
        TPS_LOG_DEV_ERROR<<"Failed to convert graphic object.";
        return nullptr;
    }
    go->SetDirty(true);
    return go;
}

bool TpsGOConverterBase::Update(const GOConverterParameter& para, GraphicObjectPtr go) 
{
    StatusResumer resumer(mStatus = Cvtr_Update);
    if (mModelWarehouse == nullptr || mDataRepository == nullptr){
        TPS_LOG_DEV_ERROR<<"Model warehouse or data repository is not ready.";
        return false;
    }

    if (!this->PrepareSourceData_i(para)){
        TPS_LOG_DEV_ERROR<<"Failed to prepare source data.";
        return false;
    }

    if (!this->Update_i(go)){
        TPS_LOG_DEV_ERROR<<"Failed to update graphic object.";
        return false;
    }
    go->SetDirty(true);
    return true;
}

bool TpsGOConverterBase::Update(GraphicObjectPtr go)
{
    StatusResumer resumer(mStatus = Cvtr_Update);
    if (mModelWarehouse == nullptr || mDataRepository == nullptr){
        TPS_LOG_DEV_ERROR<<"Model warehouse or data repository is not ready.";
        return false;
    }

    bool bRes = Update_i(go);
    if (bRes) go->SetDirty(true);
    return bRes;
}

void TpsGOConverterBase::SetModelWarehouse(ModelWarehouse* modelWarehouse)
{
    mModelWarehouse = modelWarehouse;
}

void TpsGOConverterBase::SetDataRepository(ITpsDataRepository* dataRepository)
{
    mDataRepository = dataRepository;
}

bool TpsGOConverterBase::PrepareSourceData_i(const GOConverterParameter& /*para*/)
{
    return true;
}

bool TpsGOConverterBase::Update_i(GraphicObjectPtr /*go*/) 
{
    return true; 
}
TPS_END_NAMESPACE  // end namespace tps