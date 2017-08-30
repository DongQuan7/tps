////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_da_imagedatamanager_group4d.cpp
/// 
///  \brief class TpsImageDataManager 
/// 
///  \version 1.0
/// 
///  \date    Oct.22,2015
////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <boost/thread/mutex.hpp>
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

//TPS
#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_series_group4d.h"

#include "McsfDataHeader/mcsf_data_header_element_map_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_seriesgroup.h"

#include "boost/date_time/posix_time/posix_time.hpp"

TPS_BEGIN_NAMESPACE

static bool SortSeries(RtSeries* pSeries1, RtSeries* pSeries2) //注意：本函数的参数的类型一定要与vector中元素的类型一致  
{  
    return pSeries1->get_seriesnumber() < pSeries2->get_seriesnumber(); //升序排列  
}

int TpsImageDataManager::GroupingSeries(const std::string& courseUid,const std::vector<RtSeriesgroup*>& vecSeriesGroup)
{
    std::vector<RtSeries*> vecSeries;
    for (auto it = mMapImage3DEntity.begin(); it != mMapImage3DEntity.end(); ++it)
    {
        if (it->second != nullptr && it->second->GetRtSeries() != nullptr && 
            it->second->GetRtSeries()->get_courseuid() == courseUid)
        {
            vecSeries.push_back(it->second->GetRtSeries());
        }
    }
    return this->GroupingSeries(vecSeries,vecSeriesGroup);
}

// divide series into groups
int TpsImageDataManager::GroupingSeries(
    const std::vector<RtSeries*>& vecSeriesInCourse,
const std::vector<RtSeriesgroup*>& vecSeriesGroup)
{
    vecSeriesGroup;
    std::map<std::string, std::vector<RtSeries*>> mapSeriesGroupTemp;    // <group name, series vector>
    int nSeries = vecSeriesInCourse.size();
    for (int i = 0; i < nSeries; i++)
    {
        RtSeries* pSeries = vecSeriesInCourse[i];
        if (pSeries == nullptr)
        {
            continue;
        }

        if (pSeries->get_isqa() || pSeries->get_isphantom())    // ignore phantom series or qa series
            continue;
        if (pSeries->get_modality() != "CT") continue;
        //std::string strGroupUid = pSeries->get_seriesaprgroupid();
        //if (strGroupUid == "")  // new series or belonging-no-group
        //{
        //    auto acquisition_time = pSeries->get_acquisitiontime();
        //    std::string strReferenceUid = pSeries->get_frameofreferenceuid();
        //    std::string strManufacturerModel = pSeries->get_manufacturermodelname();
        //    //bool isAllTagsValid = !acquisition_time.is_special() && 
        //    //    !strReferenceUid.empty() && !strManufacturerModel.empty();
        //    //if (!isAllTagsValid)
        //    //{
        //    //    continue;
        //    //}
        //    std::string strAcquisitionTime = boost::posix_time::to_simple_string(acquisition_time);
        //    //根据数据库三个字段拼出strGroupRule
        //    std::string strGroupRule = strReferenceUid + strAcquisitionTime + strManufacturerModel;
        //    auto it = std::find_if(m_mapSeriesGroup4d.begin(), m_mapSeriesGroup4d.end(), 
        //        [&](std::pair<std::string, std::shared_ptr<TpsSeriesGroup4d>> pr)
        //    {
        //        return (pr.second != nullptr && pr.second->m_strGroupingRule == strGroupRule);
        //    });
        //    //查找m_mapSeriesGroup4d中是否存在Rule跟他相同的
        //    if (it != m_mapSeriesGroup4d.end())
        //    {
        //        //存在相同的话将series放到第二个参数m_vecSeries中
        //        it->second->m_vecSeries.push_back(pSeries);
        //    }
        //    else
        //    {
        //        //不存在的话,在mapSeriesGroupTemp中搜索是否存在rule
        //        auto iter = mapSeriesGroupTemp.find(strGroupRule);
        //        if (iter == mapSeriesGroupTemp.end())
        //        {
        //            //如果不存在将键值对插入mapSeriesGroupTemp
        //            //键值对第一个参数为groupRule，第二个为std::vector<RtSeries*>
        //            std::vector<RtSeries*> vecTemp;
        //            vecTemp.push_back(pSeries);
        //            mapSeriesGroupTemp.insert(std::map<std::string, std::vector<RtSeries*>>::value_type(strGroupRule, vecTemp));
        //        }
        //        else
        //        {
        //            //如果存在插入第二个参数中
        //            iter->second.push_back(pSeries);
        //        }
        //    }
        //}
        //else    // already grouped
        //{
        //    //groupUid不为空
        //    //在m_mapSeriesGroup4d中查找strGroupUid
        //    auto iter = m_mapSeriesGroup4d.find(strGroupUid);
        //    if (iter == m_mapSeriesGroup4d.end())
        //    {
        //        // error
        //    }
        //    else
        //    {
        //        //找到的话，检查该series是否已经被加入到m_mapSeriesGroup4d中了
        //        //已加入不做任何处理
        //        //没有加入将它加入到最后面
        //        std::shared_ptr<TpsSeriesGroup4d> spSeriesGroup4d = iter->second;
        //        const std::vector<std::string>& vecSeriesUid = spSeriesGroup4d->m_vecSeriesUid;
        //        if (spSeriesGroup4d != nullptr)
        //        {
        //            //check if the series is already in the group
        //            auto it = std::find(vecSeriesUid.begin(), 
        //                vecSeriesUid.end(), pSeries->get_uid());
        //            if (it == vecSeriesUid.end()) 
        //            {
        //                spSeriesGroup4d->m_vecSeries.push_back(pSeries);
        //            }
        //        }
        //    }
        //}



        std::string fourDCTGroupUid = pSeries->get_seriesgroupuid();
        if (fourDCTGroupUid != "")
        {
            auto iter = mMapSeriesGroup4d.find(fourDCTGroupUid);
            if (iter == mMapSeriesGroup4d.end())
            {
                //键值对第一个参数为4DCTGroupUid，第二个为std::vector<RtSeries*>
                //std::vector<RtSeries*> vecTemp;
                //vecTemp.push_back(pSeries);
                //mapSeriesGroupTemp.insert(std::map<std::string, std::vector<RtSeries*>>::value_type(fourDCTGroupUid, vecTemp));

                std::shared_ptr<TpsSeriesGroup4d> spSeriesGroup4d(new TpsSeriesGroup4d());
                if (spSeriesGroup4d == nullptr)
                    return -1;
                spSeriesGroup4d->Init();
                spSeriesGroup4d->m_strGroupUid = fourDCTGroupUid;
                spSeriesGroup4d->m_vecSeries.push_back(pSeries);
                mMapSeriesGroup4d[fourDCTGroupUid] = spSeriesGroup4d;
            }
            else
            {
                iter->second->m_vecSeries.push_back(pSeries);
            }
        }
    }


    // new series or belonging-no-group
    //for (auto iter = mapSeriesGroupTemp.begin(); iter != mapSeriesGroupTemp.end(); ++iter)
    //{
    //    std::vector<RtSeries*> vecSeries = iter->second;
    //    if (vecSeries.size() > 1)
    //    {
    //        //RtSeriesgroup seriesGroup(true);
    //        //这里strNewGroupUid不要自己生成，拿数据库series表里面的，已经有数值了
    //        std::string strGroupUid = vecSeries[0]->get_seriesgroupuid();
    //        std::shared_ptr<TpsSeriesGroup4d> spSeriesGroup4d(new TpsSeriesGroup4d());
    //        if (spSeriesGroup4d == nullptr)
    //            return -1;
    //        spSeriesGroup4d->Init();
    //        spSeriesGroup4d->m_strGroupUid = strGroupUid;
    //        spSeriesGroup4d->m_vecSeries = vecSeries;
    //        //spSeriesGroup4d->m_strGroupingRule = iter->first;
    //        m_mapSeriesGroup4d[strGroupUid] = spSeriesGroup4d;
    //        //for(int i = 0; i < vecSeries.size(); i++)
    //        //{
    //        //    vecSeries[i]->set_seriesaprgroupid(strGroupUid);
    //        //}
    //    }
    //}

    // (a) sort by RtSeries series number (b) build the link
    for (auto iter = mMapSeriesGroup4d.begin(); iter != mMapSeriesGroup4d.end(); ++iter)
    {
        std::shared_ptr<TpsSeriesGroup4d> spSeriesGroup4d = iter->second;
        if (spSeriesGroup4d != nullptr)
        {
            std::vector<RtSeries*> vecSeries = spSeriesGroup4d->m_vecSeries;
            std::sort(vecSeries.begin(), vecSeries.end(), SortSeries);
            spSeriesGroup4d->m_vecSeries = vecSeries;
            spSeriesGroup4d->m_vecSeriesUid.clear();
            for (int i = 0; i < vecSeries.size(); i++)
            {
                RtSeries* pSeries = vecSeries[i];
                if (pSeries == nullptr)
                {
                    continue;
                }

                std::string strSeriesUid = pSeries->get_uid();
                TpsImage3DEntity* pImage3d = nullptr;
                bool bFound = GetImage3DData(strSeriesUid, &pImage3d);
                if (!bFound)
                {
                    continue;
                }
                if (pImage3d == nullptr)
                {
                    continue;
                }

                spSeriesGroup4d->m_vecSeriesUid.push_back(strSeriesUid);
                // In Image3dEntity, keep a weak ptr to this group4d
                pImage3d->SetSeriesGroup4d(spSeriesGroup4d);
            }
            if(spSeriesGroup4d->m_vecSeriesUid.size() > 0)
            {
                //if(spSeriesGroup4d->m_strMainSeriesUid == "")
                //{

                    for (int i = 0; i < vecSeriesGroup.size(); i++)
                    {
                        if (vecSeriesGroup[i]->get_uid() == spSeriesGroup4d->m_strGroupUid)
                        {
                            spSeriesGroup4d->m_strGroupName = (vecSeriesGroup[i]->get_name());
                            spSeriesGroup4d->m_strMainSeriesUid = vecSeriesGroup[i]->get_mainseriesuid();
                            spSeriesGroup4d->m_strCurrentSeriesUid = vecSeriesGroup[i]->get_mainseriesuid();
                        }
                    }

                    //GroupName在数据库里已经有了
                    //spSeriesGroup4d->m_strGroupName = "Main Phase " + spSeriesGroup4d->m_vecSeries[0]->get_seriesdescription();
                //}
                spSeriesGroup4d->m_bValid = true;
            }
            else    // exclude SeriesGroup belongs to other patient
            {
                spSeriesGroup4d->m_bValid = false;
            }
        }
    }

    return 0;
}

int TpsImageDataManager::GetAllSeriesGroup(std::vector<std::shared_ptr<TpsSeriesGroup4d>>& vecSeriesGroup)
{
    vecSeriesGroup.clear();
    for (auto iter = mMapSeriesGroup4d.begin(); iter != mMapSeriesGroup4d.end(); ++iter)
    {
        std::shared_ptr<TpsSeriesGroup4d> spSeriesGroup4d = iter->second;
        if (spSeriesGroup4d != nullptr)
        {
            if (spSeriesGroup4d->m_bValid)
            {
                vecSeriesGroup.push_back(spSeriesGroup4d);
            }
        }
    }
    return 0;
}

int TpsImageDataManager::SetSeriesGroup(const std::string& strGroupUid, const std::vector<std::string>& vecSeriesUid)
{
    std::shared_ptr<TpsSeriesGroup4d> spSeriesGroup4d(new TpsSeriesGroup4d());
    if (spSeriesGroup4d == nullptr)
        return -1;
    spSeriesGroup4d->Init();

    int nSeries = vecSeriesUid.size();
    int nSeriesValid = 0;
    for (int i = 0; i < nSeries; i++)
    {
        TpsImage3DEntity* pImage3d = nullptr;
        bool bFound = GetImage3DData(vecSeriesUid[i], &pImage3d);
        if (!bFound)
            continue;
        if (pImage3d == nullptr)
            continue;
        // Add to series uid vector
        spSeriesGroup4d->m_vecSeriesUid.push_back(vecSeriesUid[i]);
        // In Image3dEntity, keep a weak ptr to this group4d
        pImage3d->SetSeriesGroup4d(spSeriesGroup4d);
        // Valid series count++
        nSeriesValid++;
    }
    if (nSeriesValid > 0)
    {
        mMapSeriesGroup4d[strGroupUid] = spSeriesGroup4d;
        spSeriesGroup4d->m_bValid = true;
    }
    if (nSeries != nSeriesValid)
    {

    }
    return 0;
}

std::shared_ptr<TpsSeriesGroup4d> TpsImageDataManager::GetSeriesGroup4d(const std::string& strSeriesUid)
{
    TpsImage3DEntity* pImage3d = nullptr;
    bool bFound = GetImage3DData(strSeriesUid, &pImage3d);
    if (!bFound)
        return nullptr;
    if (pImage3d == nullptr)
        return nullptr;
    return pImage3d->GetSeriesGroup4d();
}

std::string TpsImageDataManager::GetGroupUid(const std::string& strSeriesUid)
{
    TpsImage3DEntity* pImage3d = nullptr;
    bool bFound = GetImage3DData(strSeriesUid, &pImage3d);
    if (!bFound)
        return "";
    if (pImage3d == nullptr)
        return "";
    std::shared_ptr<TpsSeriesGroup4d> spSeriesGroup = pImage3d->GetSeriesGroup4d();
    if (spSeriesGroup == nullptr)
        return "";
    return spSeriesGroup->m_strGroupUid;
}


// Will change the VOI collection and Plan later
void TpsImageDataManager::SetMainPhase(const std::string& strGroupUid, const std::string& strSeriesUid)
{
    auto iter = mMapSeriesGroup4d.find(strGroupUid);
    if (iter != mMapSeriesGroup4d.end())
    {
        std::shared_ptr<TpsSeriesGroup4d> spSeriesGroup4d = iter->second;
        if (spSeriesGroup4d != nullptr)
        {
            spSeriesGroup4d->m_strMainSeriesUid = strSeriesUid;
            spSeriesGroup4d->m_strCurrentSeriesUid = strSeriesUid;
        }
    }
}

// for 4D CT, VOI, POI is operated on MainPhase Series
std::string TpsImageDataManager::GetOperationImageUid(const std::string& strSeriesUid)
{
    std::shared_ptr<TpsSeriesGroup4d> spSeriesGroup4d = GetSeriesGroup4d(strSeriesUid);
    if (spSeriesGroup4d == nullptr)
        return strSeriesUid;

    return spSeriesGroup4d->m_strMainSeriesUid;
}

TPS_END_NAMESPACE
