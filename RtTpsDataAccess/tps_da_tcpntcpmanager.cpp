//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  zhendong.wang zhendong.wang@united-imaging.com
/// 
///  \file tps_tcpntcp_manager.cpp
/// 
///  \brief

///  \version 1.0
///  \date    August. 10, 2016
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RtTpsDataAccess/tps_da_tcpntcpmanager.h"

//TPS
#include "tps_logger.h"


#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_tcp.h"

TPS_BEGIN_NAMESPACE;
TpsTcpNTcpManager::TpsTcpNTcpManager(void) 
{

}

TpsTcpNTcpManager::~TpsTcpNTcpManager(void)
{

}

bool TpsTcpNTcpManager::AddTcp(RtTcp* tcp)
{
    if (tcp == nullptr) return false;

    const std::string tcpUid = tcp->get_uid();

    if (tcpUid.empty()) return false;

    auto it = mTcpMap.find(tcp->get_uid());
    if (it != mTcpMap.end())
    {
        TPS_LOG_DEV_WARNING << "The RtTable already exists, replace it.";
        TPS_DEL_PTR(it->second);
    }
    mTcpMap[tcp->get_uid()] = tcp;

    return true;
}

void TpsTcpNTcpManager::AddTcpList(const std::vector<RtTcp*>& tcpList)
{
    std::string uid;
    std::map<std::string, RtTcp*>::iterator it;
    for (int i = 0; i < tcpList.size(); ++i)
    {
        if (tcpList[i] == nullptr) continue;
        uid = tcpList[i]->get_uid();
        it = mTcpMap.find(uid);
        if (it != mTcpMap.end()) delete it->second;
        mTcpMap[uid] = tcpList[i];
    }
}

std::vector<RtTcp*> TpsTcpNTcpManager::GetAllTcps() const
{
    std::vector<RtTcp*> vecTcp;
    vecTcp.reserve(mTcpMap.size());
    for_each(mTcpMap.begin(), mTcpMap.end(), 
        [&](const std::pair<std::string, RtTcp*>& pr)
    {
        vecTcp.push_back(pr.second);
    });
    return vecTcp;
}

bool TpsTcpNTcpManager::GetTcpBySiteTemplateUid(const std::string& siteTemplateUid, std::vector<RtTcp*> &vecTcps)
{
    vecTcps.clear();
    for (auto it = mTcpMap.begin(); it != mTcpMap.end(); ++it)
    {
        if (it->second == nullptr) continue;
        if (it->second->get_sitetemplateuid() == siteTemplateUid)
        {
            vecTcps.push_back(it->second);
        }
    }
    return true;
}

bool TpsTcpNTcpManager::GetTcpBySitTemplateUidAndName(
    const std::string& siteTemplateUid, const std::string& name, RtTcp** tcp)
{
    *tcp = nullptr;
    if (siteTemplateUid.empty() || name.empty())
    {
        return false;
    }

    for (auto it = mTcpMap.begin(); it != mTcpMap.end(); ++it)
    {
        if (it->second == nullptr) continue;
        if (it->second->get_sitetemplateuid() == siteTemplateUid && it->second->get_name() == name)
        {
            *tcp = it->second;
            return true;
        }
    }
    return false;
}

void TpsTcpNTcpManager::ClearPatientData()
{
    for (auto it = mTcpMap.begin(); it != mTcpMap.end(); ++it)
    {
        delete it->second;
    }
    mTcpMap.clear();
}


TPS_END_NAMESPACE
