////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_calc_jump_to_step_operation.cpp
/// 
///  \brief class CalcJumpToStep implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/08/03
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_calc_jump_to_step_operation.h"

#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

CalcJumpToStepOperation::CalcJumpToStepOperation(
    const std::string& seriesUid, 
    float x, float y, float z, 
    FIRST_POSTFIX_COMPONENT section, 
    SECOND_POSTFIX_COMPONENT extension) : 
    mSeriesUid(seriesUid), mX(x), mY(y), mZ(z), 
    mSection(section), mExtension(extension), mStep(0)
{

}

CalcJumpToStepOperation::~CalcJumpToStepOperation()
{

}

bool CalcJumpToStepOperation::ModifyGraphicObject() const
{
    std::string goKey = mSeriesUid + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto go = mModelWarehouse->GetModelObject(goKey);
    auto volume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(go);
    if (volume == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get volume go with key: "<<goKey;
        return false;
    }

    GO_TYPE type = GOTypeHelper::ComposeAsGOType(MPR, mSection, mExtension);
    goKey = mSeriesUid + "|" + GOTypeHelper::ToString(type);
    go = mModelWarehouse->GetModelObject(goKey);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get MPR go with key: "<<goKey;
        return false;
    }

    Mcsf::MedViewer3D::Vector3D normal;
    mpr->GetNormal(&normal);
    Mcsf::MedViewer3D::Point3D vertices[4];
    mpr->GetVertices(vertices);
    double spacing;
    TpsMprHelper::CalcSpacingInWorld(volume, mpr, spacing);

    auto matP2W = volume->GetWorld2Patient().Inverse();
    auto point = matP2W.Transform(Mcsf::MedViewer3D::Point3D(mX, mY, mZ));
    double dist = normal.DotProduct(point - vertices[0]);
    mStep = -1 * TPS_ROUND(dist / spacing);
    return true;
}

int CalcJumpToStepOperation::GetStep() const
{
    return mStep;
}
TPS_END_NAMESPACE  // end namespace tps