//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  zhendong.wang zhendong.wang@united-imaging.com
/// 
///  \file tps_tcpntcp_manager.h
/// 
///  \brief

///  \version 1.0
///  \date    August. 10, 2016
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_TCPNTCP_MANAGER_H_
#define TPS_TCPNTCP_MANAGER_H_

#include "tps_da_defs.h"
#include "RtTpsDataAccess/tps_da_datamanagerbase.h"

TPS_BEGIN_NAMESPACE;


class RtTcp;

class TPS_DA_DECLSPEC TpsTcpNTcpManager : public TpsDataManagerBase
{
public:

    TpsTcpNTcpManager(void);
    ~TpsTcpNTcpManager(void);

    bool AddTcp(RtTcp *pTcp);
    void AddTcpList(const std::vector<RtTcp*>& tcpList);
    std::vector<RtTcp*> GetAllTcps() const;
    bool GetTcpBySiteTemplateUid(const std::string& siteTemplateUid, std::vector<RtTcp*> &vecTcps);
    bool GetTcpBySitTemplateUidAndName(const std::string& siteTemplateUid, const std::string& name, RtTcp** tcp);


    void ClearPatientData();



private:
    std::map<std::string, RtTcp*>                   mTcpMap;
   
};

TPS_END_NAMESPACE

#endif
