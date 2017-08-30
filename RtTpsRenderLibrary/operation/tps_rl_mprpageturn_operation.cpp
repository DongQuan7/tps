//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
///
///  \file    tps_rl_mprpageturn_operation.h
///  \brief   MPR page turn implementation
///
///  \version 1.0
///  \date    Dec. 20, 2013
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

// TPS
#include "RtTpsRenderLibrary/tps_rl_mprpageturn_operation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_movement_operation.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

MPRPageTurnOperation::MPRPageTurnOperation(
    int turnStep, const std::string& strImageUID, 
    FIRST_POSTFIX_COMPONENT ucSectionType, 
    SECOND_POSTFIX_COMPONENT ucSectionExtendType, 
    bool bIsCyclic) : 
    mTurnStep(turnStep),
    mImageUID(strImageUID),
    mSectionType(ucSectionType),
    mSectionExtendType(ucSectionExtendType),
    mIsCyclic(bIsCyclic)
{

}

MPRPageTurnOperation::~MPRPageTurnOperation()
{

}

bool MPRPageTurnOperation::ModifyGraphicObject() const 
{
    try
    {
        auto goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType, mSectionExtendType);
        std::string strKey = mImageUID + "|" + GOTypeHelper::ToString(goType);
        auto go = mModelWarehouse->GetModelObject(strKey);
        auto spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
        if (spMprGO == nullptr)
        {
            TPS_LOG_DEV_ERROR << "[ERROR][MPRPageTurnOperation][ModifyGraphicObject]: "
                << "Failed to dynamic cast IGraphicObjects.";
            return false;
        }

        strKey = mImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
        go = mModelWarehouse->GetModelObject(strKey);
        auto spVolumeGO = std::dynamic_pointer_cast<GraphicObjVolume>(go);
        if (spVolumeGO == nullptr)
        {
            TPS_LOG_DEV_ERROR << "[ERROR][MPRPageTurnOperation][ModifyGraphicObject]: "
                << "Failed to dynamic cast IGraphicObjects.";
            return false;
        }

        auto matP2W = spVolumeGO->GetModel2PatientMat().Inverse();
        auto matM2W = spVolumeGO->GetModel2World();
        matP2W.Prepend(matM2W);

        Vector3D vStepLen;
        spMprGO->GetNormal(&vStepLen);
        vStepLen.Normalize();
        //计算出翻转1一页需要的偏移距离
        double stepLen;
        TpsMprHelper::CalcSpacingInWorld(spVolumeGO, spMprGO, stepLen);
        //翻页正方向为视角方向，即面法向的反方向
        vStepLen *= -1 * stepLen;

        int currentSlice, totalSlice;
        TpsMprHelper::GetSliceNumberInViewOrder(spVolumeGO, spMprGO, currentSlice, totalSlice);
        int targetSlice = currentSlice + mTurnStep;
        if (mIsCyclic)
        {
            targetSlice = (targetSlice - 1) % totalSlice + 1;
            if (targetSlice <= 0) targetSlice += totalSlice;
        }
        else
        {
            targetSlice = TPS_CLAMP(targetSlice, 1, totalSlice);
        }
        int arctualStep = targetSlice - currentSlice;
        spMprGO->Translate(arctualStep * vStepLen);
        spMprGO->SetDirty(true);
    }
    catch(std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "MPRPageTurnOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }

    return true;
}

TPS_END_NAMESPACE
