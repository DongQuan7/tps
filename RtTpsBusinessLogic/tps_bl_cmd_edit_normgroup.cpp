////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_cmd_edit_normgroup.cpp
/// 
///  \brief class for edit normgroup
/// 
///  \version 1.0
/// 
///  \date    Nov.6,2015
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_edit_normgroup.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsBusinessLogic/tps_bl_dosecalculationimpl.h"

#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h" 
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_poimanager.h" 


#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"


#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"




TPS_BEGIN_NAMESPACE;   // begin namespace tps

EditNormgroupCmd::EditNormgroupCmd(RtNormgroup*& pNormgroup) :
    mNormgroup(pNormgroup)
{

}

EditNormgroupCmd::~EditNormgroupCmd()
{

}

int EditNormgroupCmd::Execute()
{
    const std::string sNormgroupUid = mNormgroup->get_uid();
    RtNormgroup* pNg = nullptr;
    mDataRepository->GetNormgroupManager()->GetNormGroup(sNormgroupUid, &pNg);
    if (nullptr == pNg || nullptr == mNormgroup)
    {
        TPS_LOG_DEV_ERROR << "normgroup is null.";
        return TPS_ER_FAILURE;
    }

    std::string planUID = mNormgroup->get_planuid();
    RtPlan* pPlan = nullptr ;
    mDataRepository->GetPatientManager()->GetPlan(planUID, &pPlan);
    if (nullptr == pPlan)
    {
        TPS_LOG_DEV_ERROR << "GetPlan error with planUID: " << planUID;
        return TPS_ER_FAILURE;
    }


    double slopeMu = mNormgroup->get_doseperfraction() /
        mNormgroup->get_percentage() / pNg->get_doseperfraction() * pNg->get_percentage();
    bool goalMuChanged = fabs(slopeMu - 1.f) > 1e-6;
    bool goalFractionChanged = pNg->get_fraction() != mNormgroup->get_fraction();
    bool goaltargetChanged = (pNg->get_isnorm2voi() != mNormgroup->get_isnorm2voi());
    if (!goaltargetChanged)
    {
        goaltargetChanged = !mNormgroup->get_isnorm2voi() ?
            (pNg->get_normalpointpoiuid() != mNormgroup->get_normalpointpoiuid()) :
        (pNg->get_voiuid() != mNormgroup->get_voiuid()
            || pNg->get_isnorm2voimeandose() != mNormgroup->get_isnorm2voimeandose()
            || pNg->get_normvoipercentagevolume() != mNormgroup->get_normvoipercentagevolume());
    }
    bool isJoinedChanged = mNormgroup->get_isjoined() != pNg->get_isjoined();


    pNg->set_name(mNormgroup->get_name());
    pNg->set_isjoined(mNormgroup->get_isjoined());
    pNg->set_machineuid(mNormgroup->get_machineuid());
    pNg->set_radiationtype(mNormgroup->get_radiationtype());
    pNg->set_plantype(mNormgroup->get_plantype());
    pNg->set_doseperfraction(mNormgroup->get_doseperfraction());
    pNg->set_fraction(mNormgroup->get_fraction());
    pNg->set_isnorm2voi(mNormgroup->get_isnorm2voi());
    pNg->set_normalpointpoiuid(mNormgroup->get_normalpointpoiuid());
    pNg->set_voiuid(mNormgroup->get_voiuid());
    pNg->set_percentage(mNormgroup->get_percentage());
    pNg->set_normvoipercentagevolume(mNormgroup->get_normvoipercentagevolume());
    pNg->set_isnorm2voimeandose(mNormgroup->get_isnorm2voimeandose());
    pNg->set_prescriptionuid(mNormgroup->get_prescriptionuid());

    pNg->set_isgating(mNormgroup->get_isgating());
    pNg->set_binmode(mNormgroup->get_binmode());
    pNg->set_beamonrangestart(mNormgroup->get_beamonrangestart());
    pNg->set_beamonrangeend(mNormgroup->get_beamonrangeend());
    pNg->set_description(mNormgroup->get_description());
    pNg->set_tpsequipmentuid(mDatabaseWrapper->GetDefaultEquipmentUid());

    int iRet = mSaveObjects->SaveNormgroup(*pNg);
    if (TPS_ER_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "Failed to save normal group in database.";
        return iRet;
    }

    // Ng GraphicObject 还没创建的时候 不应该触发绘制
    if (!mRenderProxy->ContainsGraphicObject(planUID, GO_TYPE_NORMALGROUP_COLLECTION))
    {
        return TPS_ER_SUCCESS;
    }

    if (!mRenderProxy->UpdateNormalGroup(planUID, sNormgroupUid))
    {
        TPS_LOG_DEV_ERROR << "Failed to update normal group with UID: " << sNormgroupUid;
        return TPS_ER_FAILURE;
    }

    if (goaltargetChanged || goalMuChanged || goalFractionChanged)
    {
        if (GetRescaleDoseFlag_i(pNg, pPlan))
        {
            RescaleDosePara doseRescalePara;
            int iReasonType = 0;
            if (goalFractionChanged) iReasonType |= DOSE_FRACTION_CHANGED;
            if (goalMuChanged) iReasonType |=  DOSE_PERFRACTION_PERCENTAGE_PRODUCT_CHANGED;
            if (goaltargetChanged) iReasonType |= NORMALGROUP_GOAL_TARGET_CHANGED;
            doseRescalePara.doseRescaleReasonType = (DoseRescaleReasonType)iReasonType;
            doseRescalePara.muRescaleFactor = slopeMu;
            iRet = mDoseCalculator->RescaleDose(sNormgroupUid, doseRescalePara);
            if (TPS_ER_SUCCESS != iRet)
            {
                TPS_LOG_DEV_ERROR << "Failed to RescaleDose.";
                return iRet;
            }
        }
    }

    if (isJoinedChanged)
    {
        mRenderProxy->RefreshDoseLine(planUID);
    }

    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

bool EditNormgroupCmd::GetRescaleDoseFlag_i(RtNormgroup* normalgroup, RtPlan* plan)
{
    if (!normalgroup->get_dosegrid()->get_isdosevalid()) return false;

    if (plan->get_plansourcetype() != RtDbDef::TreatmentPlan) return false;

    if (!normalgroup->get_isnorm2voi())
    {
        std::string poiUid = normalgroup->get_normalpointpoiuid();
        RtPoi* poi = mDataRepository->GetPoiManager()->GetPOI(poiUid);
        if (poi != nullptr && poi->get_plancalcmethod(plan->get_uid()) != RtDbDef::Calculated) return false;
    }

    return true;
    
}





TPS_END_NAMESPACE  // end namespace tps
