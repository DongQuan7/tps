////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  zuo kai kai.zuo@united-imaging.com
/// 
///  \file tps_rl_dosegridboundarygraphicobjectconverter.cpp
/// 
///  \brief class TpsDoseGridBoundaryGOConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/04/11
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "McsfMedViewer3DArithmetic/point3d.h"
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"

#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsRenderLibrary/tps_rl_dosegridboundarygraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_dosegridboundarygraphicobject.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsDoseGridBoundaryGOConverter::TpsDoseGridBoundaryGOConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType), mDoseGrid(nullptr){

}

TpsDoseGridBoundaryGOConverter::~TpsDoseGridBoundaryGOConverter(){

}

GraphicObjectPtr TpsDoseGridBoundaryGOConverter::CreateGraphicObject_i(){
    return std::shared_ptr<TpsDoseGridBoundaryGraphicObject>(
        new TpsDoseGridBoundaryGraphicObject());
}

bool TpsDoseGridBoundaryGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    mDoseGrid = nullptr;

    auto it = para.data_source_map.find(PLAN);
    if (it == para.data_source_map.end()) return false;

    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(it->second, &plan) && plan == nullptr){
        TPS_LOG_DEV_ERROR<<"The plan does not exist, uid: "<<it->second;
        return false;
    }
    mDoseGrid = plan->get_dosegrid();
    if (mDoseGrid == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get dose grid of plan: "<<it->second;
        return false;
    }
    return true;
}

bool TpsDoseGridBoundaryGOConverter::Update_i(GraphicObjectPtr go){
    auto doseGridGo = std::dynamic_pointer_cast<TpsDoseGridBoundaryGraphicObject>(go);
    if (mDoseGrid == nullptr || doseGridGo == nullptr) return false;

    Mcsf::Matrix4f matG2P = TpsArithmeticConverter::ConvertToMatrix4f(mDoseGrid->get_grid_to_pat_t());
    Mcsf::Point3f points[2] =
    {
        matG2P.Transform(Mcsf::Point3f(-0.5f, -0.5f, -0.5f)),
        matG2P.Transform(Mcsf::Point3f(mDoseGrid->get_xcount() - 0.5f,
            mDoseGrid->get_ycount() - 0.5f,mDoseGrid->get_zcount() - 0.5f))
    };

    //Mcsf::Point3f lbPoint(
    //    TPS_MIN(points[0].GetX(), points[1].GetX()), 
    //    TPS_MIN(points[0].GetY(), points[1].GetY()), 
    //    TPS_MIN(points[0].GetZ(), points[1].GetZ()));
    //Mcsf::Point3f rtPoint(
    //    TPS_MAX(points[0].GetX(), points[1].GetX()),
    //    TPS_MAX(points[0].GetY(), points[1].GetY()),
    //    TPS_MAX(points[0].GetZ(), points[1].GetZ()));
    doseGridGo->SetLeftBottomPoint(
        TpsArithmeticConverter::ConvertToPoint3D(points[0]));
    doseGridGo->SetRightTopPoint(TpsArithmeticConverter::ConvertToPoint3D(points[1]));

    Mcsf::Point3f spacing(mDoseGrid->get_xcoordinate3dinc(),
        mDoseGrid->get_ycoordinate3dinc(),mDoseGrid->get_zcoordinate3dinc());
    doseGridGo->SetSpacing(TpsArithmeticConverter::ConvertToPoint3D(spacing));
    doseGridGo->SetVisible(false);
    return true;
}
TPS_END_NAMESPACE  // end namespace tps