////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  liu linqi  linqi.liu@united-imaging.com
/// 
///  \file tps_rl_doselineobjectconverter.h
/// 
///  \brief class TpsDoseLineGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/08/04
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_doselineobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_doselineobject.h"

#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_doseline.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"


TPS_BEGIN_NAMESPACE

TpsDoseLineGOConverter::TpsDoseLineGOConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType),
    mPlan(nullptr){

}

TpsDoseLineGOConverter::~TpsDoseLineGOConverter(){

}

GraphicObjectPtr TpsDoseLineGOConverter::CreateGraphicObject_i(){
    return std::shared_ptr<TpsDoseLineGraphicObject>(new TpsDoseLineGraphicObject());
}

bool TpsDoseLineGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    mDoseLineList.clear();
    mNormGroupList.clear();

    auto it = para.data_source_map.find(PLAN);
    if (it == para.data_source_map.end()) return false;

    if (!mDataRepository->GetDoseManager()->GetDoseLineList(it->second, mDoseLineList)) {
        TPS_LOG_DEV_INFO<<"No doseline in TpsDoseManager";
        return false;
    }

    mPlan = nullptr;
    if(!mDataRepository->GetPatientManager()->GetPlan(it->second, &mPlan)
        || mPlan == nullptr){
        TPS_LOG_DEV_INFO<<"Failed to get plan from manager.";
        return false;
    }

    /*mDoseCalcUid = DisplayStateManager::GetInstance()->GetDoseCalcUid();

    mDoseCalcLevel = DisplayStateManager::GetInstance()->GetDoseCalcLevel();*/

    mNormGroupList = mDataRepository->GetNormgroupManager()->GetNormalGroupList(it->second);
    return true;
}

bool DoseLineSorter(const RtDoseline* first, const RtDoseline* second);

bool TpsDoseLineGOConverter::Update_i(GraphicObjectPtr go){
    auto doseLineGo = std::dynamic_pointer_cast<TpsDoseLineGraphicObject>(go);
    if (doseLineGo == nullptr) {
        return false;
    }

    doseLineGo->Clear();

    std::sort(mDoseLineList.begin(), mDoseLineList.end(), DoseLineSorter);

    std::vector<DoseLineAttribute> doseLineAttrList;
    DoseLineAttribute doseLineAttr;

    float totalGoal = 0.f;
    for (int i = 0; i < mNormGroupList.size(); ++i)
    {
        if (mNormGroupList[i] == nullptr|| !mNormGroupList[i]->get_isjoined()) continue;
        totalGoal += mNormGroupList[i]->get_fraction() * 
            mNormGroupList[i]->get_doseperfraction();
    }
    float specfiedDose = mPlan->get_specifieddose();
    //if specified dose is invalid, follow the total goal value
    if (fabs(specfiedDose - INVALID_SPECIFIED_DOSE_VALUE) <= 1e-6)
    {
        specfiedDose = totalGoal;
    }

    for(auto iter = mDoseLineList.begin(); iter != mDoseLineList.end(); iter++){
        doseLineAttr.mDosePercent   = (*iter)->get_doseratio();
        doseLineAttr.mVisible       = (*iter)->get_isvisible();
        doseLineAttr.mColor[0]      = (*iter)->get_red();
        doseLineAttr.mColor[1]      = (*iter)->get_green();
        doseLineAttr.mColor[2]      = (*iter)->get_blue();

        doseLineAttrList.push_back(doseLineAttr);
    }

	doseLineGo->SetDisplayMode((DOSELINE_DISPLAYMODE)mPlan->get_displaymode());
    doseLineGo->SetDisplayAttributes(doseLineAttrList);
    doseLineGo->SetHundredPercentDoseMode((DOSELINE_CALCMODE)mPlan->get_hundredpercentdefinition());
    doseLineGo->SetTotalGoal(totalGoal);
    doseLineGo->SetMaxDose(mPlan->get_dosegrid()->get_max());
    doseLineGo->SetSpecifiedDose(specfiedDose);

	TpsDisplayStateManager* displayStateManager = mDataRepository->GetDisplayStateManager();
	doseLineGo->SetVisonMode(displayStateManager->GetDoselineRenderModeType());
	doseLineGo->SetTransparency(displayStateManager->GetDoselineTransparency());
	doseLineGo->SetTransparency3D(displayStateManager->GetDoselineTransparency3D());

    return true;
}

bool DoseLineSorter(const RtDoseline* first, const RtDoseline* second){

    return first->get_doseratio() < second->get_doseratio();
}
TPS_END_NAMESPACE

