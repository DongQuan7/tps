//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_client_tcp.h
///  \brief     client using tcp
///
///  \version 1.0
///  \date    Apr.5, 2017
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_CLIENT_TCP_H_
#define RT_TPS_DATABASE_CLIENT_TCP_H_

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include "rt_tps_database_client_protocol.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

class RtDatabaseClientTcp
{
public:

    // constructor
    RtDatabaseClientTcp(const std::string& host, unsigned int port);
    // destructor
    ~RtDatabaseClientTcp(void);

    void SetParameters(const std::string& host, unsigned int port) { m_sHostName = host; m_uiPort = port;}

    /////////////////////////////////////////////////////////////////
    ///  \brief         Send a requiest to TCP server
    ///                 
    ///  \param[in]
    ///                 protocol_method     sMethod,
    ///                 const std::string&  sDataToServer,
    ///  \param[out]    std::string&        sDataFromServer
    ///  \return        0:TRUE othes: FALSE
    ///  \pre \e  
    ///         e.g.    POST
    ///                 SendRequest(pro_POST, sDataToServer, sDataFromServer)
    ///                 sDataToServer is buffer sent to server,
    ///                 sDataFromServer is a full filepath in server;
    ///
    ///         e.g.    GET
    ///                 SendRequest(pro_GET, sDataToServer, sDataFromServer)
    ///                 sDataToServer is a full filepath in server,
    ///                 sDataFromServer is buffer sent from server;
    ///
    ///         e.g.    DELETE
    ///                 SendRequest(pro_DELETE, sDataToServer, sDataFromServer)
    ///                 sDataToServer is a full filepath in server,
    ///                 sDataFromServer is empth;
    /////////////////////////////////////////////////////////////////
    int SendRequest(protocol pro, const std::string& sDataToServer, std::string& sDataFromServer);

private:
    //host name
    std::string     m_sHostName;
    //port number
    unsigned int    m_uiPort;
};

RT_TPS_DATABASE_END_NAMESPACE;


#endif  //RT_TPS_DATABASE_CLIENT_TCP_H_