////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Xiaoqing.Shangguan Xiaoqing.Shangguan@united-imaging.com
/// 
///  \file rt_tps_certifiedunit.h
/// 
///  \brief struct RT_TPS_Unit declaration 
/// 
///  \version 1.0
/// 
///  \date    Jan 10, 2014
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIEDUNIT_H_
#define RT_TPS_CERTIFIEDUNIT_H_

#include "tps_defs.h"
#include <vector>

TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto{
    class RT_TPS_Unit;
    class RT_TPS_Applicator;
}

//required string applicator_uid		= 1;
//optional string unit_uid			= 2;
//optional string applicator_name		= 3;
struct CertifiedApplicator
{
    std::string mApplicatorUid;
    std::string mUnitUid;
    std::string mApplicatorName;

    void Convert(const proto::RT_TPS_Applicator& protoApplicator);

    void ConvertBack(proto::RT_TPS_Applicator* protoApplicator) const;
};

struct TrayFactor
{
    std::string m_strTrayUid;
    float       m_fTrayFactor;
};

struct CertifiedUnit
{
    std::string mUnitUID;
    std::string mMachineName;
    std::string mMachineUID;
    std::string mUnitEnergy;
    int mRadiationType;
    std::vector<int> mTechniqueTypeList;
    int mFlatteningFilterMode;
    double mGantryMin;
    double mGantryMax;
    double mTableMin;
    double mTableMax;
    double mCollimatorMin;
    double mCollimatorMax;
    double mJawX1Min;
    double mJawX1Max;
    double mJawX2Min;
    double mJawX2Max;
    double mJawY1Min;
    double mJawY1Max;
    double mJawY2Min;
    double mJawY2Max;

    std::vector<CertifiedApplicator> mApplicatorList;
	std::vector<float> mDoseRateList;
    std::vector<std::string> mTrayUIDList;
    std::vector<TrayFactor> m_vecTrayFactor;
    CertifiedUnit();

    void Convert(const proto::RT_TPS_Unit& protoUnit);

    void ConvertBack(proto::RT_TPS_Unit* protoUnit) const;
};

TPS_END_NAMESPACE  // end namespace tps

#endif // RT_TPS_CertifiedNormGroup_H_