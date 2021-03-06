﻿//////////////////////////////////////////////////////////////////////////
/// \defgroup McsfDatabase of Common Software Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  qiangqiang.zhou@united-imaging.com
///
///  \file       mcsf_database_interface_object.h
///  \brief   This file was generated by CodeGenerater.exe 
///              which translates database script into classes supported
///
///  \version 1.0
///  \date    9/5/2013
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_INTERFACE_OBJECTS_H_
#define RT_TPS_DATABASE_INTERFACE_OBJECTS_H_

#include <boost/date_time/gregorian/gregorian_types.hpp>// for posix_time::ptime --DATETIME
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "rt_tps_database_interface_object_accessory.h"
#include "rt_tps_database_interface_object_applicator.h"
#include "rt_tps_database_interface_object_approverecord.h"
#include "rt_tps_database_interface_object_patient.h"
#include "rt_tps_database_interface_object_study.h"
#include "rt_tps_database_interface_object_course.h"
#include "rt_tps_database_interface_object_prescription.h"
#include "rt_tps_database_interface_object_series.h"
#include "rt_tps_database_interface_object_imagetransformation.h"
#include "rt_tps_database_interface_object_image.h"
#include "rt_tps_database_interface_object_image3d.h"
#include "rt_tps_database_interface_object_ct2density.h"
#include "rt_tps_database_interface_object_plan.h"
#include "rt_tps_database_interface_object_normgroup.h"
#include "rt_tps_database_interface_object_beam.h"
#include "rt_tps_database_interface_object_beamsegment.h"
#include "rt_tps_database_interface_object_deliverybeamgroup.h"

#include "rt_tps_database_interface_object_machine.h"
#include "rt_tps_database_interface_object_commissionedunit.h"
#include "rt_tps_database_interface_object_ct2density.h"

#include "rt_tps_database_interface_object_contour.h"
#include "rt_tps_database_interface_object_dosegrid.h"
#include "rt_tps_database_interface_object_doseline.h"

#include "rt_tps_database_interface_object_structureset.h"
#include "rt_tps_database_interface_object_voi.h"
#include "rt_tps_database_interface_object_voisetting.h"
#include "rt_tps_database_interface_object_rtimage.h"

#include "rt_tps_database_interface_object_block.h"
#include "rt_tps_database_interface_object_mlcshape.h"
#include "rt_tps_database_interface_object_dvh.h"
#include "rt_tps_database_interface_object_beam_poi.h"
#include "rt_tps_database_interface_object_doseoptgoalbase.h"

#include "rt_tps_database_interface_object_template.h"
#include "rt_tps_database_interface_object_commissionedunit_tray.h"
#include "rt_tps_database_interface_object_kerneldata.h"
#include "rt_tps_database_interface_object_kerneldatalist.h"
#include "rt_tps_database_interface_object_tray.h"
#include "rt_tps_database_interface_object_kernelweight.h"
#include "rt_tps_database_interface_object_mcphocommissionedunit.h"
#include "rt_tps_database_interface_object_measureddata.h"
#include "rt_tps_database_interface_object_measuredprofile.h"
#include "rt_tps_database_interface_object_wedgefactor.h"
#include "rt_tps_database_interface_object_goldenstt.h"
#include "rt_tps_database_interface_object_accessorychunk.h"
#include "rt_tps_database_interface_object_commissionedunit_tray.h"
#include "rt_tps_database_interface_object_mcphomonoenergyweight.h"
#include "rt_tps_database_interface_object_monocompressedphasespacedata.h"
#include "rt_tps_database_interface_object_tablecontour.h"
#include "rt_tps_database_interface_object_seriestable.h"
#include "rt_tps_database_interface_object_drrsetting.h"
#include "rt_tps_database_interface_object_tcp.h"
#include "rt_tps_database_interface_object_ntcp.h"
#include "rt_tps_database_interface_object_site.h"
#include "rt_tps_database_interface_object_norm_voi.h"
#include "rt_tps_database_interface_object_plan_voi.h"
#include "rt_tps_database_interface_object_equipment.h"
#include "rt_tps_database_interface_object_seriesgroup.h"
#include "rt_tps_database_interface_object_poi.h"

//////////////////////////////////////////////////////////////////////////
RT_TPS_DATABASE_BEGIN_NAMESPACE;

typedef boost::posix_time::ptime DATETIME_BOOST;
typedef boost::posix_time::time_duration TIME_BOOST;
typedef boost::gregorian::date DATE_BOOST;


RT_TPS_DATABASE_END_NAMESPACE
#endif // MCSF_DATABASE_INTERFACE_OBJECT_H
