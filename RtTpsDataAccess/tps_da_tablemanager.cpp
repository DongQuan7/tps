//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  Zuo Kai kai.zuo@united-imaging.com
///
///  \file    tps_da_tablemanager.cpp
///  \brief
///
///  \version 1.0
///  \date    01.29,2016
///  \{
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RtTpsDataAccess/tps_da_tablemanager.h"

// TPS
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_tablecontour.h"
#include "RtTpsFramework/tps_fw_table.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsTableManager::TpsTableManager(void)
{

}

TpsTableManager::~TpsTableManager(void)
{
    ClearPatientData();

    for (auto it = mOrgTableMap.begin(); it != mOrgTableMap.end(); ++it)
    {
        TPS_DEL_PTR(it->second);
    }
    mOrgTableMap.clear();
}

void TpsTableManager::AddRtTable(RtTable* table)
{
    if (table == nullptr) return;

    auto it = mOrgTableMap.find(table->GetUid());
    if (it != mOrgTableMap.end())
    {
        TPS_LOG_DEV_WARNING<<"The RtTable already exists, replace it.";
        TPS_DEL_PTR(it->second);
    }
    mOrgTableMap[table->GetUid()] = table;
}

RtTable* TpsTableManager::GetRtTable(const std::string& uid) const
{
    auto it = mOrgTableMap.find(uid);
    return it == mOrgTableMap.end() ? nullptr : it->second;
}

void TpsTableManager::RemoveRtTable(const std::string& uid)
{
    auto it = mOrgTableMap.find(uid);
    if (it != mOrgTableMap.end())
    {
        TPS_DEL_PTR(it->second);
        mOrgTableMap.erase(it);
    }
}

std::vector<RtTable*> TpsTableManager::GetAllRtTables() const
{
    std::vector<RtTable*> vec;
    vec.reserve(mOrgTableMap.size());
    for_each(mOrgTableMap.begin(), mOrgTableMap.end(), 
        [&](const std::pair<std::string, RtTable*>& pr)
    {
        vec.push_back(pr.second);
    });
    return vec;
}

void TpsTableManager::AddSeriesTable(const std::string& seriesUid, SeriesTable* seriesTable)
{
    if (seriesTable == nullptr) return;

    auto it = mSeriesTableMap.find(seriesUid);
    if (it != mSeriesTableMap.end())
    {
        TPS_LOG_DEV_WARNING<<"The SeriesTable already exists, replace it.";
        TPS_DEL_PTR(it->second);
    }
    mSeriesTableMap[seriesUid] = seriesTable;
}

SeriesTable* TpsTableManager::GetSeriesTable(const std::string& seriesUid) const
{
    auto it = mSeriesTableMap.find(seriesUid);
    if (it == mSeriesTableMap.end())
    {
        return nullptr;
    }
    return it->second;
}

void TpsTableManager::ClearPatientData()
{
    for (auto it = mSeriesTableMap.begin(); it != mSeriesTableMap.end(); ++it)
    {
        delete it->second;
    }
    mSeriesTableMap.clear();
}
TPS_END_NAMESPACE