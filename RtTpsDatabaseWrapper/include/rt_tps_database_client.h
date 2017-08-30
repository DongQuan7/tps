//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_client.h
///  \brief     client to download file from server
///
///  \version 1.0
///  \date    Jan.4, 2016
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_CLIENT_H_
#define RT_TPS_DATABASE_CLIENT_H_


#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;


//download file from server
//TODO local cache to improve performance
class  RtDatabaseClient
{
public:

    // constructor
    RtDatabaseClient(const std::string& host, unsigned int port);
    // destructor
    ~RtDatabaseClient(void);

    void SetParameters(const std::string& host, unsigned int port);
    void SetWebPageDownload(const std::string& page);
    void SetWebPageUpload(const std::string& page);

    // 0 : success others: failed
    // need delete buffer by caller!
    int DownloadFile(const std::string& path, char*& buffer, size_t* length);

    // 0 : success others: failed
    // input: file buffer & file size
    // output: file path on the server
    int UploadFile(const char* filebuffer, size_t filesize, std::string* filepath);

private:
    //host name
    std::string     m_sHostName;
    //port number
    unsigned int    m_uiPort;
    //web page
    std::string     m_sPageDownload;
    std::string     m_sPageUpload;
};

RT_TPS_DATABASE_END_NAMESPACE;

#endif  //RT_TPS_DATABASE_CLIENT_H_