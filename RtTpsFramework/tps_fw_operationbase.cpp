//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_fw_operationbase.cpp
///
/// \brief   OperationBase
/// 
/// \version 1.0
/// 
/// \date    2013/12/11
/////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

#include "McsfMedViewer3DGraphicObject/graphic_object_interface.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"

#include "tps_logger.h"
TPS_BEGIN_NAMESPACE

OperationBase::OperationBase() : mModelWarehouse(nullptr), 
mDataRepository(nullptr), mConverterFactory(nullptr){
}

OperationBase::~OperationBase() {
}

void OperationBase::SetModelWarehouse(ModelWarehouse* model) {
    mModelWarehouse = model;
}

void OperationBase::SetDataRepository(ITpsDataRepository* dataRepository){
    mDataRepository = dataRepository;
}

void OperationBase::SetConverterFactory(GOConverterFactoryBase* converterFactory){
    mConverterFactory = converterFactory;
}

void OperationBase::Initialize(ModelWarehouse* model, 
    ITpsDataRepository* dataRepository, 
    GOConverterFactoryBase* converterFactory)
{
    mModelWarehouse = model;
    mDataRepository = dataRepository;
    mConverterFactory = converterFactory;
}

bool OperationBase::IsReady() const{
    return mModelWarehouse != nullptr && mDataRepository != nullptr && 
        mConverterFactory != nullptr;
}

bool OperationBase::Execute() const {
    if (!this->IsReady()){
        TPS_LOG_DEV_ERROR<<"[ERROR][OperationBase][Execute]: "
            <<"The operation is not ready to execute.";
        return false;
    }

    bool result = false;

    mModelWarehouse->Lock();

    result = ModifyGraphicObject();

    mModelWarehouse->Unlock();

    return result;
}

TPS_END_NAMESPACE

