////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  ZUO Kai kai.zuo@united-imaging.com
/// 
///  \file tps_rl_editdosegridboundaryoperation.cpp
/// 
///  \brief class TpsEditDoseGridBoundaryOperation implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/04/10
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_editdosegridboundaryoperation.h"
#include "RtTpsRenderLibrary/tps_rl_dosegridboundarygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderController/tps_rc_rendercontroller.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"



TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsEditDoseGridBoundaryOperation::TpsEditDoseGridBoundaryOperation(
const std::string& planUID, Mcsf::Point3f startPoint, Mcsf::Point3f endPoint, Mcsf::Point3f spacing) :  
    mPlanUID(planUID), mStartPoint(startPoint), mEndPoint(endPoint), mSpacing(spacing) {
}

TpsEditDoseGridBoundaryOperation::~TpsEditDoseGridBoundaryOperation(){

}

bool TpsEditDoseGridBoundaryOperation::ModifyGraphicObject() const{
    std::string doseGridBoundaryKey = mPlanUID + "|" + 
        GOTypeHelper::ToString(GO_TYPE_DOSE_GRID_BOUNDARY);
    auto go = mModelWarehouse->GetModelObject(doseGridBoundaryKey);
    auto doseGridBoundaryGo = std::dynamic_pointer_cast<TpsDoseGridBoundaryGraphicObject>(go);
    if (doseGridBoundaryGo == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get dose grid boundary go with key: "
            <<doseGridBoundaryKey;
        return false;
    }
    Mcsf::MedViewer3D::Point3D leftBottomPoint;
    Mcsf::MedViewer3D::Point3D rightTopPoint;
    Mcsf::MedViewer3D::Point3D spacing;
    leftBottomPoint.x = mStartPoint.GetX();
    leftBottomPoint.y = mStartPoint.GetY();
    leftBottomPoint.z = mStartPoint.GetZ();
    rightTopPoint.x = mEndPoint.GetX();
    rightTopPoint.y = mEndPoint.GetY();
    rightTopPoint.z = mEndPoint.GetZ();
    spacing.x = mSpacing.GetX();
    spacing.y = mSpacing.GetY();
    spacing.z = mSpacing.GetZ();
    doseGridBoundaryGo->SetLeftBottomPoint(leftBottomPoint);
    doseGridBoundaryGo->SetRightTopPoint(rightTopPoint);
    doseGridBoundaryGo->SetSpacing(spacing);
    doseGridBoundaryGo->SetType("DOSEGRID");
    doseGridBoundaryGo->SetDirty(true);
    return true;
}
TPS_END_NAMESPACE  // end namespace tps
