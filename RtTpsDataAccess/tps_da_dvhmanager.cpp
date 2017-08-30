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
#include "stdafx.h"
#include "boost/thread/recursive_mutex.hpp"
#include "RtTpsDataAccess/tps_da_dvhmanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dvh.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE
TpsDVHManager::TpsDVHManager(void) 
{
    mReMutex = new boost::recursive_mutex();
}

TpsDVHManager::~TpsDVHManager(void) 
{
    delete mReMutex; mReMutex = nullptr;
    Dispose();
}

void TpsDVHManager::Dispose() {
    PlanDVHMap::iterator it = mDvhs.begin();
    for (;it!=mDvhs.end(); it++) {
        std::map<std::string, RtDvh*>& planDvhs = *(it->second);
        std::map<std::string, RtDvh*>::iterator it = planDvhs.begin();
        for (; it!=planDvhs.end(); it++) {
            delete it->second;
            it->second = nullptr;
        }
        planDvhs.clear();
    }
    mDvhs.clear();
}

void TpsDVHManager::ClearPatientData() {
    Dispose();
}

bool TpsDVHManager::AddDvh(const std::string sPlanUID, const RtDvh& dvh)
{
    boost::recursive_mutex::scoped_lock lg(*mReMutex);

    // if exist, failed to add
    if (HasPlanVoiDvh(sPlanUID, dvh.get_uid())) {
        TPS_LOG_DEV_ERROR<<"Failed to add dvh to manager. already exist";
        return false;
    }

    // 如果plan都没有DVH，加入plan
    PlanDvhIterator itrPlan = mDvhs.find(sPlanUID);
    if (itrPlan==mDvhs.end()) {
        PlanDVHDict* tmpDict = new PlanDVHDict();
        tmpDict->insert(std::make_pair(dvh.get_uid(), new RtDvh(dvh)));
        mDvhs[sPlanUID] = tmpDict;
        return true;
    }

    // 如果是plan有，voi没有，加入plan的子map
    PlanDVHDict::iterator dictIt = itrPlan->second->find(dvh.get_uid());
    if (dictIt!=itrPlan->second->end()) {
        TPS_LOG_DEV_ERROR<<"Failed to add dvh to manager. already exist";
        return false;
    }
    itrPlan->second->insert(std::make_pair(dvh.get_uid(), new RtDvh(dvh)));
    return true;
}

void TpsDVHManager::DeletePlanDvh(const std::string sPlanUID)
{
    boost::recursive_mutex::scoped_lock lg(*mReMutex);
    PlanDvhIterator it = mDvhs.find(sPlanUID);
    if (it != mDvhs.end()) {
        PlanDVHDict::iterator dictIt = it->second->begin();
        for (; dictIt!=it->second->end(); ++dictIt) {
            if(nullptr != dictIt->second) {
                delete dictIt->second;
                dictIt->second = nullptr;
            }
        }
        it->second->clear();

        mDvhs.erase(it);
    }
}

void TpsDVHManager::RemoveDvh(const std::string sPlanUID, const std::string sVoiUID)
{
    boost::recursive_mutex::scoped_lock lg(*mReMutex);
    if (!HasPlanVoiDvh(sPlanUID, sVoiUID)) {
        TPS_LOG_DEV_WARNING<<"dvh of plan["<<sPlanUID<<"],voi["
            <<sVoiUID<<"] not exist.";
        return;
    }

    PlanDvhIterator it = mDvhs.find(sPlanUID);
    if (it != mDvhs.end()){
        PlanDVHDict::iterator dictIt = it->second->find(sVoiUID);
        if (dictIt!=it->second->end()) {
            delete dictIt->second;
            it->second->erase(dictIt);
        }
    }
}

bool TpsDVHManager::HasPlanVoiDvh(const std::string& planUid, const std::string& voiUid)
{
    boost::recursive_mutex::scoped_lock lg(*mReMutex);
    PlanDvhIterator planDvhIt = mDvhs.find(planUid);
    if (planDvhIt==mDvhs.end()) {
        return false;
    }

    if (planDvhIt->second->find(voiUid)==planDvhIt->second->end()) {
        return false;
    }

    return true;
}

bool TpsDVHManager::UpdateDvh(const std::string planUid,  const RtDvh& dvh)
{
    boost::recursive_mutex::scoped_lock lg(*mReMutex);
	std::string voiUid = dvh.get_uid();
	if (!HasPlanVoiDvh(planUid, voiUid)) {
        TPS_LOG_DEV_ERROR<<"dvh of plan["<<planUid<<"],voi["
            <<voiUid<<"] not exist.";
        return false;
    }
    
    PlanDvhIterator planDvhIt = mDvhs.find(planUid);
    PlanDVHDict* planDvh = nullptr;
    if (planDvhIt != mDvhs.end()){
        planDvh = planDvhIt->second;
        
        PlanDVHDict::iterator dictIt = planDvh->find(voiUid);
        if (dictIt!=planDvh->end()) {
            delete dictIt->second;
            (*planDvh)[voiUid] = new RtDvh(dvh);
        }
        else {
            TPS_LOG_DEV_ERROR<<"dvh of plan["<<planUid<<"], voi"<<voiUid<<" not exist.";
            return false;
        }
        return true;
    }
    else{
        TPS_LOG_DEV_ERROR<<"dvh of plan["<<planUid<<" not exist";
        return false;
    }
}

std::vector<RtDvh*> TpsDVHManager::GetPlanDvhs(const std::string sPlanUid)
{
    boost::recursive_mutex::scoped_lock lg(*mReMutex);
    std::vector<RtDvh*> vDvh;
    PlanDvhIterator it = mDvhs.find(sPlanUid);
    if (it != mDvhs.end()){
        PlanDVHDict::iterator dictIt = it->second->begin();
        for (;dictIt!=it->second->end();dictIt++) {
            vDvh.push_back(dictIt->second);
        }
    }

    return vDvh;
}

bool TpsDVHManager::SetPlanDvhs(const std::string sPlanUid, 
    std::vector<RtDvh*>& vDvh)
{
    boost::recursive_mutex::scoped_lock lg(*mReMutex);

    DeletePlanDvh(sPlanUid);//delete old value

    for(int n=0; n<vDvh.size(); n++){
        //deep copy here!
        if (!AddDvh(sPlanUid, *vDvh[n])) {
            return false;
        }
    }

    return true;
}

bool TpsDVHManager::UpdatePlanDvhs(const std::string planUid, std::vector<RtDvh*>& planDvh)
{
    boost::recursive_mutex::scoped_lock lg(*mReMutex);

    if (mDvhs.find(planUid)==mDvhs.end()) {
        TPS_LOG_DEV_ERROR<<"plan["<<planUid<<" not exist in dvh manager";
        return false;
    }

    for (int n=0; n<planDvh.size(); n++) {
        if (UpdateDvh(planUid, *planDvh[n])) {
            TPS_LOG_DEV_ERROR<<"Failed to update dvh of plan["<<
                planUid<<"].";
            return false;
        }
    }
    return true;
}

bool TpsDVHManager::HasPlanDvhs(const std::string planUid) 
{
    boost::recursive_mutex::scoped_lock lg(*mReMutex);
    PlanDvhIterator it = mDvhs.find(planUid);
    return it != mDvhs.end();
}


void TpsDVHManager::AddVoiDoseVolume(std::string& strPlanUid, std::vector<int>& vecDoseVolumeToAdd)    // called when add new DoseVolume
{
    auto iter = m_mapPlanVoiDoseVolume.find(strPlanUid);
    // first time as not found
    if (iter == m_mapPlanVoiDoseVolume.end())
    {
        std::vector<int> vecDoseVolume;
        // Add default volume first
        int defaultDoseVolume[] = {99, 98, 95, 90, 50};
        for (int i = 0; i < sizeof(defaultDoseVolume) / sizeof(defaultDoseVolume[0]); i++)
        {
            vecDoseVolume.push_back(defaultDoseVolume[i]);
        }
        // Add new ones
        for (int i = 0; i < vecDoseVolumeToAdd.size(); i++)
        {
            vecDoseVolume.push_back(vecDoseVolumeToAdd[i]);
        }
        m_mapPlanVoiDoseVolume.insert(std::map<std::string, std::vector<int>>::value_type(strPlanUid, vecDoseVolume));
    }
    else  // append new ones
    {
        for (int i = 0; i < vecDoseVolumeToAdd.size(); i++)
        {
            iter->second.push_back(vecDoseVolumeToAdd[i]);
        }
    }
}

void TpsDVHManager::GetVoiDoseVolume(std::string& strPlanUid, std::vector<int>& vecDoseVolume)    // called when refresh all dose volume to FE
{
    int defaultDoseVolume[] = {99, 98, 95, 90, 50};
    auto iter = m_mapPlanVoiDoseVolume.find(strPlanUid);
    // return default array if not found
    if (iter == m_mapPlanVoiDoseVolume.end())
    {
        for (int i = 0; i < sizeof(defaultDoseVolume) / sizeof(defaultDoseVolume[0]); i++)
        {
            vecDoseVolume.push_back(defaultDoseVolume[i]);
        }
    }
    else 
    {
        vecDoseVolume = iter->second;
    }
}

        // actually
        //    Volume %
        //     ^    Left
        //     |  *(LeftVolume,LeftDose)
        //     |  
        //     |                                                      Right
        //     |                                                   *(RightVolume,RightDose)
        //     |---------------------------------------------------------------------------> Dose

void TpsDVHManager::GetVoiDoseVolume(std::string& strPlanUid, std::vector<std::string>& vecVoiUid, std::vector<int>& vecVolumePercent, std::vector<float>& vecDose)
{
    vecDose.clear();
    if (vecVoiUid.size() <= 0)
        return;
    if (vecVolumePercent.size() <= 0)
        return;
    vecDose.insert(vecDose.begin(), vecVoiUid.size() * vecVolumePercent.size(), 0.0f);

    //boost::recursive_mutex::scoped_lock lg(mReMutex);
    auto iter = mDvhs.find(strPlanUid);
    if (iter == mDvhs.end()) // not found DVH for the plan
    {
        return;
    }

    std::map<std::string, RtDvh*>* pVoiDvhMap = iter->second;       // <VoiUid, Dvh>
    if (pVoiDvhMap == nullptr)
    {
        return; // plan DVH is null
    }

    // for each VolumePercent, we will one left <Volume, Dose> pair and one right <Volume, Dose> pair.
    // then we can interpolate between these 2 pairs
    float* pLeftVolume = new float[vecVolumePercent.size()];       // x
    int* pLeftDose = new int[vecVolumePercent.size()];             // y
    float* pRightVolume = new float[vecVolumePercent.size()];
    int* pRightDose = new int[vecVolumePercent.size()];

    for (int i = 0; i < vecVoiUid.size(); i++)
    {
        auto iterDvh = pVoiDvhMap->find(vecVoiUid[i]);
        // not found DVH for this VOI
        if (iterDvh == pVoiDvhMap->end())
        {
            continue;
        }

        RtDvh* pDvh = iterDvh->second;
        if (pDvh == nullptr)    // VOI DVH is null
        {
            continue;
        }

        float fBinWidth = pDvh->get_binwidth(); // default to be 1 cGy 
        float fMinDose = pDvh->get_mindose();
        float fMaxDose = pDvh->get_maxdose();

        if (fBinWidth <= 0.0) 
            continue;
        if (fMaxDose <= 0.0)
            continue;

        //    Dose
        //     ^    Left
        //     |  *(LowVolume,HighDose)
        //     |  
        //     |                                                      Right
        //     |                                                   *(HighVolume,LowDose)
        //     |---------------------------------------------------------------------------> Volume

        for(int j = 0; j < vecVolumePercent.size(); j++)
        {
            pLeftVolume[j] = 0.0f;
            pLeftDose[j] = 0;

            pRightDose[j] = 0;
            pRightVolume[j] = 100.0f;
        }

        // find the near point to interpolate
        std::map<int, float> mapCumulativeData = pDvh->get_cumulative_dvhdata();
        std::map<int, float>::iterator iterDose;
        for (iterDose = mapCumulativeData.begin(); iterDose != mapCumulativeData.end(); iterDose++)
        {
            int nDose = iterDose->first;        // dose
            float fVolume = iterDose->second;   // volume
            for(int j = 0; j < vecVolumePercent.size(); j++)
            {
                if (pLeftVolume[j] < fVolume && fVolume <= vecVolumePercent[j]) // nearer low
                {
                    pLeftVolume[j] = fVolume;
                    pLeftDose[j] = nDose;
                }
                if (pRightVolume[j] > fVolume && fVolume >= vecVolumePercent[j]) // nearer high
                {
                    pRightVolume[j] = fVolume;
                    pRightDose[j] = nDose;
                }
            }
        }

        // interpolate for each volume point
        for(int j = 0; j < vecVolumePercent.size(); j++)
        {
            float fDose = 0.0f;
            if (pLeftVolume[j] == pRightVolume[j])  // exactly match
            {
                fDose = pLeftDose[j] * fBinWidth + pDvh->get_mindose();
            }
            else if( pLeftDose[j] == 0)   // left not found?
            {
                fDose = (vecVolumePercent[j] - pLeftVolume[j]) / (pRightVolume[j] - pLeftVolume[j]) * (pRightDose[j] * fBinWidth + fMinDose - fMaxDose) + fMaxDose;
            }
            else
            {
                fDose = (vecVolumePercent[j] - pLeftVolume[j]) / (pRightVolume[j] - pLeftVolume[j]) * (pRightDose[j] - pLeftDose[j]) + pLeftDose[j];
                fDose = fDose * fBinWidth + fMinDose;
            }
            vecDose[i * vecVolumePercent.size() + j] = fDose;
        }

    } // loop for each VOI

    // free memory on heap
    delete[] pLeftVolume;
    delete[] pLeftDose;
    delete[] pRightVolume;
    delete[] pRightDose;
}

TPS_END_NAMESPACE
