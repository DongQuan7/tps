////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Xiaoqing.Shangguan  Xiaoqing.Shangguan@united-imaging.com
/// 
///  \file tps_pqaca_export_slice_dose_cmd.cpp
/// 
///  \brief 
/// 
///  \version 1.0
/// 
///  \date    2015/06/09
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_exportslicedose.h"

// TMS
#include "alg/RtAlgDataLayer/rt_alg_dl_object_dosegrid.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_normgroup.h"


#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
// MCSF
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

// TPS
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_datasourcegroup.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprdosegraphicobject.h"



#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "RtTpsDoseProxy/tps_dp_optobserver.h"
#include "RtTpsDoseProxy/tps_dp_doseproxy.h"

#include "tps_logger.h"

// Temp
#include <boost/lexical_cast.hpp>
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

using namespace Mcsf::MedViewer3D;
TPS_BEGIN_NAMESPACE

ExportSliceDoseCmd::ExportSliceDoseCmd(
    std::string planeType, std::string filePath, void* doseByte, int size) :
    m_planeType(planeType), m_filePath(filePath), m_pDoseByte(doseByte), m_size(size)
{

}

ExportSliceDoseCmd::~ExportSliceDoseCmd()
{
}

int ExportSliceDoseCmd::Execute()
{
    try
    {
        TpsFirstPostfixComponent section = EASYPLAN_IMAGE_SECTION;
        if ("CORONAL" == m_planeType)
        {
            section = CORONAL;
        }
        else if ("SAGITTAL" == m_planeType)
        {
            section = SAGITTAL;
        }
        else if ("AXIAL" == m_planeType)
        {
            section = AXIAL;
        }
        else
        {
            TPS_LOG_DEV_ERROR << "the plane type is not correct:" << m_planeType;
            return TPS_ER_FAILURE;
        }
        TpsDataSourceGroup *group = mRenderProxy->GetDataSourceGroup(SOURCE_GROUP_PLANQA);
        std::string imageUID, planUid;
        if (!group->GetDataSource(IMAGE, imageUID))
		{
            TPS_LOG_DEV_ERROR << "SOURCE_GROUP_PLANQA do not has IMAGE";
            return TPS_ER_FAILURE;
        }
        if (!group->GetDataSource(PLAN, planUid))
		{
            TPS_LOG_DEV_ERROR << "SOURCE_GROUP_PLANQA do not has PLAN";
            return TPS_ER_FAILURE;
        }

        //get dose graphic object
        RtDosegrid *doseGrid = nullptr;
        if (!mRenderProxy->GetSliceDoseGird(doseGrid, section, planUid, imageUID))
        {
            return TPS_ER_FAILURE;
        }

        if (!mDoseProxy->ExportSliceDose(doseGrid, m_filePath, m_pDoseByte, m_size))
        {
            TPS_DEL_PTR(doseGrid);
            return TPS_ER_FAILURE;
        }

        TPS_DEL_PTR(doseGrid);

        return TPS_ER_SUCCESS;

    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "ExportSliceDoseCmd failed:  " << ex.what();
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE