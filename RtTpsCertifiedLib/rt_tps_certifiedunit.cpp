////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Xiaoqing.Shangguan Xiaoqing.Shangguan@united-imaging.com
/// 
///  \file rt_tps_certifiedunit.cpp
/// 
///  \brief struct RT_TPS_Unit implementation 
/// 
///  \version 1.0
/// 
///  \date    Jan 10, 2014
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifiedunit.h"
#include "RtTpsProtoLib/rt_tps_unit.pb.h"

TPS_BEGIN_NAMESPACE;  // begin namespace tps

void CertifiedApplicator::Convert(const proto::RT_TPS_Applicator& protoApplicator)
{
    mApplicatorUid = protoApplicator.applicator_uid();
    mUnitUid = protoApplicator.unit_uid();
    mApplicatorName = protoApplicator.applicator_name();
}

void CertifiedApplicator::ConvertBack(proto::RT_TPS_Applicator* protoApplicator) const
{
    protoApplicator->set_applicator_uid(mApplicatorUid);
    protoApplicator->set_unit_uid(mUnitUid);
    protoApplicator->set_applicator_name(mApplicatorName);
}


CertifiedUnit::CertifiedUnit()
{
    mUnitUID="";
    mMachineName="";
    mMachineUID="";
    mUnitEnergy="";
    mRadiationType = 0;
    //std::vector<int> mTechniqueTypeList;
    mFlatteningFilterMode=0;
    mGantryMin = 0.;
    mGantryMax = 0.;
    mTableMin = 0.;
    mTableMax = 0.;
    mCollimatorMin= 0.;
    mCollimatorMax= 0.;
    mJawX1Min  = 0.;
    mJawX1Max  = 0.;
    mJawX2Min  = 0.;
    mJawX2Max  = 0.;
    mJawY1Min  = 0.;
    mJawY1Max  = 0.;
    mJawY2Min  = 0.;
    mJawY2Max  = 0.;
    //std::vector<CertifiedApplicator> mApplicatorList;
    //std::vector<std::string> mTrayUIDList;
}


void CertifiedUnit::Convert(const proto::RT_TPS_Unit& protoUnit) 
{
    mUnitUID = protoUnit.unituid();
    mMachineName = protoUnit.machinename();
    mMachineUID = protoUnit.machineuid();
    mUnitEnergy = protoUnit.unitenergy();
    mRadiationType = protoUnit.radiationtype();
    mGantryMin = protoUnit.gantry_min();
    mGantryMax = protoUnit.gantry_max();
    mTableMin = protoUnit.table_min();
    mTableMax = protoUnit.table_max();
    mCollimatorMin = protoUnit.collimator_min();
    mCollimatorMax = protoUnit.collimator_min();
    mJawX1Min  = protoUnit.jaw_x1_min();
    mJawX1Max  = protoUnit.jaw_x1_max();
    mJawX2Min  = protoUnit.jaw_x2_min();
    mJawX2Max  = protoUnit.jaw_x2_max();
    mJawY1Min  = protoUnit.jaw_y1_min();
    mJawY1Max  = protoUnit.jaw_y1_max();
    mJawY2Min  = protoUnit.jaw_y2_min();
    mJawY2Max  = protoUnit.jaw_y2_max();

    mTechniqueTypeList.clear();
    for (int n=0; n<protoUnit.techniquetype_size(); n++) {
        mTechniqueTypeList.push_back(protoUnit.techniquetype(n));
    }
    mFlatteningFilterMode = protoUnit.flatteningfiltermode();

    for (int i=0; i<protoUnit.applicators_size(); ++i){
        CertifiedApplicator app;
        app.Convert(protoUnit.applicators(i));
        mApplicatorList.push_back(app);
    }

    for(int i=0; i<protoUnit.trayuid_size(); i++) {
        mTrayUIDList.push_back(protoUnit.trayuid(i));
    }

    for(int i=0; i<protoUnit.tray_factor_size(); i++) {
        TrayFactor stcTrayFactor;
        stcTrayFactor.m_strTrayUid = protoUnit.tray_factor(i).tray_uid();
        stcTrayFactor.m_fTrayFactor = protoUnit.tray_factor(i).tray_factor();
        m_vecTrayFactor.push_back(stcTrayFactor);
    }

    for(int i = 0; i < protoUnit.doserates_size(); i++){
        mDoseRateList.push_back(protoUnit.doserates(i));
    }
}

void CertifiedUnit::ConvertBack(proto::RT_TPS_Unit* protoUnit) const
{
    protoUnit->set_machinename(mMachineName);
    protoUnit->set_machineuid(mMachineUID);
    protoUnit->set_unituid(mUnitUID);
    protoUnit->set_radiationtype(mRadiationType);
    protoUnit->clear_techniquetype();
    for (int n=0; n<mTechniqueTypeList.size(); n++) {
        protoUnit->add_techniquetype(mTechniqueTypeList[n]);
    }
    protoUnit->set_unitenergy(mUnitEnergy);
    protoUnit->set_gantry_min(mGantryMin);
    protoUnit->set_gantry_max(mGantryMax);
    protoUnit->set_table_min(mTableMin);
    protoUnit->set_table_max(mTableMax);
    protoUnit->set_collimator_min(mCollimatorMin);
    protoUnit->set_collimator_max(mCollimatorMax);
    protoUnit->set_jaw_x1_min(mJawX1Min);
    protoUnit->set_jaw_x1_max(mJawX1Max);
    protoUnit->set_jaw_x2_min(mJawX2Min);
    protoUnit->set_jaw_x2_max(mJawX2Max);
    protoUnit->set_jaw_y1_min(mJawY1Min);
    protoUnit->set_jaw_y1_max(mJawY1Max);
    protoUnit->set_jaw_y2_min(mJawY2Min);
    protoUnit->set_jaw_y2_max(mJawY2Max);

    protoUnit->set_flatteningfiltermode(mFlatteningFilterMode);

    for (int i=0; i<mApplicatorList.size(); ++i){
        proto::RT_TPS_Applicator* app = protoUnit->add_applicators();
        mApplicatorList[i].ConvertBack(app);
    }

    for (int i=0; i<mTrayUIDList.size(); i++) {
        protoUnit->add_trayuid(mTrayUIDList[i]);
    }

    for (int i=0; i<m_vecTrayFactor.size(); i++) {
        proto::RT_TPS_Tray_Factor* pTrayFactor = protoUnit->add_tray_factor();
        pTrayFactor->set_tray_uid(m_vecTrayFactor[i].m_strTrayUid);
        pTrayFactor->set_tray_factor(m_vecTrayFactor[i].m_fTrayFactor);
    }

    for (int i = 0; i < mDoseRateList.size(); i++){
        protoUnit->add_doserates(mDoseRateList[i]);
    }
}

TPS_END_NAMESPACE  // end namespace tps