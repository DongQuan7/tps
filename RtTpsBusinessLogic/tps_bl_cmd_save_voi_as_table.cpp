////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_save_voi_as_table.cpp
/// 
///  \brief class SaveVOIAsTableCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/11/24
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_save_voi_as_table.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_uid_generater.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_tablecontour.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_contour.h"

#include "boost/lexical_cast.hpp"

TPS_BEGIN_NAMESPACE

SaveVOIAsTableCmd::SaveVOIAsTableCmd(const std::string& voiUID) : mVoiUid(voiUID)
{

}

SaveVOIAsTableCmd::~SaveVOIAsTableCmd()
{

}

int SaveVOIAsTableCmd::Execute()
{
    //auto voiMng = mDataRepository->GetVoiManager();
    //auto imgMng = mDataRepository->GetImageDataManager();
    //VOIEntity* voiEntity = nullptr;
    //if (!voiMng->GetVOI(mVoiUid, &voiEntity) || voiEntity == nullptr)
    //{
    //    TPS_LOG_DEV_ERROR<<"The voi does not exist, uid: "<<mVoiUid;
    //    return TPS_ER_FAILURE;
    //}

    //TpsImage3DEntity* image = nullptr;
    //std::string imageUID = voiEntity->GetSeriesUid();
    //if (!imgMng->GetImage3DData(imageUID, &image) || image == nullptr)
    //{
    //    TPS_LOG_DEV_ERROR<<"The image 3d does not exist, uid: "<<imageUID;
    //    return TPS_ER_FAILURE;
    //}

    //RtImage3DHeader* header = nullptr;
    //if (!image->GetImage3DHeader(header) || header == nullptr)
    //{
    //    TPS_LOG_DEV_ERROR<<"Failed to get current image 3d header.";
    //    return TPS_ER_FAILURE;
    //}
    //std::string sThickness = image->GetRtSeries()->get_slicethickness();
    //double dThickness = boost::lexical_cast<double>(sThickness);
    ////std::string sPatientPosition(header->m_cPatientPosition);

    //std::string tableName("UIH_SUPPORT");
    //std::string tableUid(RtUidGenerater::GeneraterUid());

    //RtTablecontour* tableContour;
    //std::vector<RtTablecontour*> vecTableContour;
    //std::vector<RtContour*> vecContour = voiEntity->GetContours();
    //std::vector<db_Point3d> pointList_d;
    //for (int i = 0; i < vecContour.size(); ++i)
    //{
    //    const std::vector<db_Point3f>& pointList = vecContour[i]->get_contour_points();
    //    if (pointList.size() < 3)
    //    {
    //        continue;
    //    }
    //    tableContour = new RtTablecontour(true);
    //    //WTF! float->double
    //    pointList_d.clear();
    //    for (int j = 0; j < pointList.size(); ++j)
    //    {
    //        pointList_d.push_back(db_Point3d(
    //            pointList[j].x, pointList[j].y, pointList[j].z));
    //    }
    //    tableContour->set_points(pointList_d);
    //    tableContour->set_slicenum(vecContour[i]->get_sliceindex());
    //    tableContour->set_tablename(tableName);
    //    tableContour->set_tableuid(tableUid);
    //    tableContour->set_thickness(dThickness);
    //    tableContour->set_contourid(0);
    //    vecTableContour.push_back(tableContour);
    //}

    ////临时代码：拷贝最后一层contour，将table扩到1m长，以后会删除
    //const std::vector<db_Point3f>& firstPoints = vecContour.front()->get_contour_points();
    //const std::vector<db_Point3f>& lastPoints = vecContour.back()->get_contour_points();
    //int firstSliceIndex = vecContour.front()->get_sliceindex();
    //int lastSliceIndex = vecContour.back()->get_sliceindex();
    //float len = lastPoints[0].z - firstPoints[0].z;
    //float spacing = len / (lastSliceIndex - firstSliceIndex);
    //int count = ceil((1000 - fabs(len)) / fabs(spacing));
    //for (int i = 0; i < count; ++i)
    //{
    //    tableContour = new RtTablecontour(true);
    //    pointList_d.clear();
    //    for (int j = 0; j < lastPoints.size(); ++j)
    //    {
    //        pointList_d.push_back(db_Point3d(
    //            lastPoints[j].x, lastPoints[j].y, lastPoints[j].z + (i+1) * spacing));
    //    }
    //    tableContour->set_points(pointList_d);
    //    tableContour->set_slicenum(lastSliceIndex + i + 1);
    //    tableContour->set_tablename(tableName);
    //    tableContour->set_tableuid(tableUid);
    //    tableContour->set_thickness(dThickness);
    //    tableContour->set_contourid(0);
    //    vecTableContour.push_back(tableContour);
    //}

    //int iRet = mSaveObjects->SaveTableContours(vecTableContour);
    //if (TPS_ER_SUCCESS != iRet)
    //{
    //    TPS_LOG_DEV_ERROR<<"Failed to save table contours.";
    //}
    //for (int i = 0; i < vecTableContour.size(); ++i)
    //{
    //    delete vecTableContour[i];
    //}
    //vecTableContour.clear();
    //return iRet;
    return TPS_ER_SUCCESS;
}
TPS_END_NAMESPACE
