//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  xiaoqing.shangguan xiaoqing.shangguan@united-imaging.com
///
///  \file    tps_da_dvhmanager.h
///  \brief
///
///  \version 1.0
///  \date    Apr. 18, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_DVH_MANAGER_H_
#define TPS_DVH_MANAGER_H_

#include "tps_da_defs.h"
//#include "boost/thread/recursive_mutex.hpp"

#include "RtTpsDataAccess/tps_da_datamanagerbase.h"

namespace boost
{
    class recursive_mutex;
}


TPS_BEGIN_NAMESPACE

class RtDvh;
    
typedef std::map<std::string, RtDvh*> PlanDVHDict;
typedef std::map<std::string, PlanDVHDict*>::iterator PlanDvhIterator;
typedef std::map<std::string, RtDvh*>::iterator PlanDvhDictIterator;
typedef std::map<std::string, PlanDVHDict*> PlanDVHMap;

class TPS_DA_DECLSPEC TpsDVHManager : public TpsDataManagerBase
{
public:
    TpsDVHManager(void);
    ~TpsDVHManager(void);

    bool AddDvh(const std::string planUid, const RtDvh& dvh);

    void DeletePlanDvh(const std::string sPlanUID);
    void RemoveDvh(const std::string sPlanUID, const std::string sVoiUID);

    bool UpdateDvh(const std::string planUid,  const RtDvh& dvh);

    std::vector<RtDvh*> GetPlanDvhs(const std::string sPlanUid);
    bool SetPlanDvhs(const std::string sPlanUid, std::vector<RtDvh*>& vDvh);

    bool UpdatePlanDvhs(const std::string planUid, std::vector<RtDvh*>& planDvh);
    bool HasPlanDvhs(const std::string planUid);
    bool HasPlanVoiDvh(const std::string& planUid, const std::string& voiUid);

    // Voi Dose Volume Value
    void AddVoiDoseVolume(std::string& strPlanUid, std::vector<int>& vecVoiVolume);    // called when add new DoseVolume
    void GetVoiDoseVolume(std::string& strPlanUid, std::vector<int>& vecVoiVolume);    // called when refresh all dose volume to FE
    void GetVoiDoseVolume(std::string& strPlanUid, std::vector<std::string>& vecVoiUid, std::vector<int>& vecVoiVolume, std::vector<float>& vecDose);
    void ClearPatientData();

private:
    void Dispose();

private:
    PlanDVHMap mDvhs;
    boost::recursive_mutex *mReMutex;

    // Voi Dose Volume Value
    std::map<std::string, std::vector<int>> m_mapPlanVoiDoseVolume;
};

TPS_END_NAMESPACE

#endif //TPS_BEAM_MANAGER
