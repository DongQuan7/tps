//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  chengcheng.rong chengcheng.rong@united-imaging.com
/// 
///  \file 
/// 
///  \brief  

///  \version 1.0
///  \date    Jul 10, 2015
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RtTpsDataAccess/tps_da_datarepositoryfactory.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "tps_da_datarepository.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsDataRepositoryFactory::TpsDataRepositoryFactory()
{

}

TpsDataRepositoryFactory::~TpsDataRepositoryFactory()
{
}

ITpsDataRepository* TpsDataRepositoryFactory::CreateDataRepository()
{
    TpsDataRepository* dataRepository = new TpsDataRepository();
    //comment this because it may cause memory leak when the method is called more then once
    //qqzhou@20161109
    //if (!dataRepository->Initialize()) 
    //{
    //    // log error here
    //    return nullptr;
    //}
    return dataRepository;
}

TPS_END_NAMESPACE
