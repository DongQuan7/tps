//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_interface_object_mlcshape_ex.cpp
///  \brief     
///       
///
///  \version 1.0
///  \date    Oct. 17, 2016
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_data.h"

//////////////////////////////////////////////////////////////////////////
RT_TPS_DATABASE_BEGIN_NAMESPACE;

//leafpositions
//dLeafLowerPos dLeafUpperPos
std::vector<tps::db_Point2d> RtMlcshape::get_leafpositions() const 
{
    return m_pImp->get_leafpositions();
}

std::vector<db_Point2d> RtMlcshape::get_leafpositions()
{ 
    return m_pImp->get_leafpositions();
}

void RtMlcshape::set_leafpositions(const std::vector<db_Point2d>& vLeafPos) 
{ 
    m_pImp->set_leafpositions(vLeafPos);
    enable_field(MLCSHAPE_LEAFPOSITIONS);
    setdirty_field(MLCSHAPE_LEAFPOSITIONS, true);
}
RT_TPS_DATABASE_END_NAMESPACE

