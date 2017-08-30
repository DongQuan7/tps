////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_graphic_object_converter_factory_base.cpp
/// 
///  \brief class GOConverterFactoryBase implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/07/13
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_graphic_object_converter_factory_base.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"


TPS_BEGIN_NAMESPACE

GOConverterFactoryBase::GOConverterFactoryBase() : 
mModelWarehouse(nullptr), mDataRepository(nullptr){

}

GOConverterFactoryBase::~GOConverterFactoryBase(){

    ClearAll();
}

TpsGOConverterBase* GOConverterFactoryBase::GetConverter(GO_TYPE goType){
    if (mModelWarehouse == nullptr || mDataRepository == nullptr){
        TPS_LOG_DEV_ERROR<<"Model warehouse or data repository is not ready.";
        return nullptr;
    }

    auto it = mConverterMap.find(goType);
    if (it != mConverterMap.end()){
        return it->second;
    }

    return CreateConverter_i(goType);
}

void GOConverterFactoryBase::SetModelWarehouse(ModelWarehouse* modelWarehouse){
    mModelWarehouse = modelWarehouse;
}

void GOConverterFactoryBase::SetDataRepository(ITpsDataRepository* dataRepository){
    mDataRepository = dataRepository;
}

void GOConverterFactoryBase::ClearAll()
{
    if (!mConverterMap.empty()){
        for_each(mConverterMap.begin(), mConverterMap.end(),
            [&](std::pair<GO_TYPE, TpsGOConverterBase*> pr){
                if (pr.second){
                    delete pr.second;
                    pr.second = nullptr;
                }
        });
        mConverterMap.clear();
    }
}
TPS_END_NAMESPACE