////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_cmd_edit_plan.cpp
/// 
///  \brief class for edit plan
/// 
///  \version 1.0
/// 
///  \date    Nov.6,2015
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_edit_plan.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"

TPS_BEGIN_NAMESPACE;   // begin namespace tps

EditPlanCmd::EditPlanCmd(const std::string& sPlanUid, const std::string& sPlanName, const std::string& sPlanDescription, const std::string& sSetupPoiUid) : 
mPlanUid(sPlanUid), mPlanName(sPlanName), mPlanDescription(sPlanDescription), mSetupPoiUid(sSetupPoiUid)
{

}

EditPlanCmd::~EditPlanCmd()
{
}

int EditPlanCmd::Execute()
{
    RtPlan* pPlan = nullptr;
    mDataRepository->GetPatientManager()->GetPlan(mPlanUid, &pPlan);
    if (nullptr == pPlan){
        TPS_LOG_DEV_ERROR<<"plan is null.";
        return TPS_ER_FAILURE;
    }
    if (!mSetupPoiUid.empty())
    {
        RtPoi* pSetupPoiu = nullptr;
        pSetupPoiu = mDataRepository->GetPoiManager()->GetPOI(mSetupPoiUid);
        if (nullptr == pSetupPoiu){
            TPS_LOG_DEV_ERROR<<"setup poi is null.";
            return TPS_ER_FAILURE;
        }
        pPlan->set_setuppoiuid(mSetupPoiUid);
    }
    else
    {
        pPlan->disable_field(PLAN_SETUPPOIUID);
    }
    pPlan->set_name(mPlanName);
    pPlan->set_description(mPlanDescription);
    if (TPS_ER_SUCCESS != mSaveObjects->SavePlan(*pPlan)) {
        TPS_LOG_DEV_ERROR<<"Failed to save try in database.";
        //if (!TpsPatientManager::GetInstance()->RemovePlan(planUid)) {
        //    TPS_LOG_DEV_ERROR<<"Failed to remove dirty plan["<<planUid<<"] from patient manager.";
        //}
        return TPS_ER_DB_ERROR;
    }

    //update plan name for image text in MPR
    UpdateImageText(mPlanName);
    return TPS_ER_SUCCESS;
}

int EditPlanCmd::UpdateImageText(std::string strImageUID, 
    std::string sPlanUid, 
    WINDOW_TYPE nWindowType,
    std::string sPlanName)
{
    FIRST_POSTFIX_COMPONENT ucSectionType;
    switch(nWindowType)
    {
    case WINDOW_COMMON_MPR_AXIAL:
    case WINDOW_COMMON_MPR_AXIAL_PREV:
    case WINDOW_COMMON_MPR_AXIAL_POST:
    case WINDOW_UT_MPR_AXIAL:
        ucSectionType = AXIAL;
        break;
    //case IMAGEPANEL_SAGITTAL:
    //case ANASTRUCTEDITOR_SAGITTAL:
    //    ucSectionType = SAGITTAL;
    //    break;
    //case IMAGEPANEL_CORONAL:
    //case ANASTRUCTEDITOR_CORONAL:
    //    ucSectionType = CORONAL;
    //    break;
    default:
        return TPS_ER_FAILURE;
    }

    //Moved to font end.
    //std::shared_ptr<UpdateImageTextOperation> spUpdateImageTextOp(new UpdateImageTextOperation());
    //GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, ucSectionType);
    //std::string strMprKey = strImageUID + "|" + GOTypeHelper::ToString(goType);
    //goType = GOTypeHelper::ComposeAsGOType(MPR_CORNER_TEXT, ucSectionType);
    //std::string strMprCornerInfoKey = strImageUID + "|" + GOTypeHelper::ToString(goType);
    //spUpdateImageTextOp->SetGraphicObjectKeys(strMprKey, strMprCornerInfoKey, sPlanName);
    //ModelWarehouse::GetInstance()->ProcessOperation(spUpdateImageTextOp);
    //TpsStartOperations::Render();
    return 0;
}


int EditPlanCmd::UpdateImageText(const std::string& sPlanName)
{
    WINDOW_TYPE windowType = WINDOW_COMMON_MPR_AXIAL; 

    std::string strPlanUid = mPlanUid;
    std::string imageUid;//mPlan->get_imageseriesuid();
    int nResult = UpdateImageText(imageUid, strPlanUid, windowType, sPlanName);
    if (TPS_ER_SUCCESS != nResult){
        return nResult;
    }

    windowType = WINDOW_COMMON_MPR_SAGITTAL;
    nResult = UpdateImageText(imageUid, strPlanUid, windowType, sPlanName);
    if (TPS_ER_SUCCESS != nResult){
        return nResult;
    }  

    windowType = WINDOW_COMMON_MPR_CORONAL; 
    nResult = UpdateImageText(imageUid, strPlanUid, windowType, sPlanName);

    return nResult;
}

TPS_END_NAMESPACE  // end namespace tps