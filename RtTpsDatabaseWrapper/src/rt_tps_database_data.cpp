//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_data.cpp
///  \brief     basic data eg.point3f 
///
///  \version 1.0
///  \date    Nov. 2,2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_data.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;


float db_Vector3f::Magnitude() const
{
    return sqrt(x * x + y * y + z * z);
}

db_Vector3f db_Point3f::operator - (const db_Point3f& pt3f)
{
    return db_Vector3f(this->x - pt3f.x, this->y - pt3f.y, this->z - pt3f.z);
}

RT_TPS_DATABASE_END_NAMESPACE;