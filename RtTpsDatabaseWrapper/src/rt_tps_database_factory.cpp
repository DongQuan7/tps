//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_factory.h
///  \brief     rt tps database factory
///
///  \version 1.0
///  \date    July 10,2015
///  \{
//////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_factory.h"
#include "rt_tps_database_wrapper.h"

//////////////////////////////////////////////////////////////////////////
RT_TPS_DATABASE_BEGIN_NAMESPACE


IDatabaseFactory::IDatabaseFactory()
{
}

IDatabaseFactory::~IDatabaseFactory()
{
}

////////////////////////////////////////////////////////////////////////////////
/// \brief  Create DBWrapper object
///
////////////////////////////////////////////////////////////////////////////////
IRtDatabaseWrapper* IDatabaseFactory::CreateDBWrapper(void) const
{
    RtDatabaseWrapper* p = nullptr;
    try
    {
        p = new RtDatabaseWrapper;
    }
    catch (...)
    {
        delete p;
        p = nullptr;
        TPS_LOG_DEV_ERROR<<("CreateDBWrapper failed!");
        return nullptr;
    }

    IRtDatabaseWrapper* pDBWrapper(p);
    return pDBWrapper;
}

RT_TPS_DATABASE_END_NAMESPACE

