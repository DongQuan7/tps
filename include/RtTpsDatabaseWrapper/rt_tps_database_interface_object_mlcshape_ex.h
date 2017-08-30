//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_interface_object_mlcshape_ex.h 
///  \brief     
///              
///
///  \version 1.0
///  \date    Oct. 17, 2016
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_INTERFACE_OBJECT_MLCSHAPE_EX_H_
#define RT_TPS_DATABASE_INTERFACE_OBJECT_MLCSHAPE_EX_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_data.h"

public:

    std::vector<db_Point2d> get_leafpositions() const;
    std::vector<db_Point2d> get_leafpositions();
    void set_leafpositions(const std::vector<db_Point2d>& vLeafPos);

#endif