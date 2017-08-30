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
#ifndef TPS_DATA_REPOSITORY_FACTORY_H
#define TPS_DATA_REPOSITORY_FACTORY_H

#include "tps_da_defs.h"

TPS_BEGIN_NAMESPACE

class ITpsDataRepository;

class TPS_DA_DECLSPEC TpsDataRepositoryFactory
{
public:
    TpsDataRepositoryFactory();
    ~TpsDataRepositoryFactory();

    ITpsDataRepository* CreateDataRepository();
    
private:
    
};

TPS_END_NAMESPACE

#endif
