//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_uid_generater.h
///  \brief     rt tps database factory
///
///  \version 1.0
///  \date    July 10,2015
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_UID_GENERATER_H_
#define RT_TPS_DATABASE_UID_GENERATER_H_

#include <string>
#include <memory>

#include "rt_tps_database_defs.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;


class RT_DB_EXPORT RtUidGenerater
{
public:

    static std::string GeneraterUid(void);
};

RT_TPS_DATABASE_END_NAMESPACE;


#endif 
