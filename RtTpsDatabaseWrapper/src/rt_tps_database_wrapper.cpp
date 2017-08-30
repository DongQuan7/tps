//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_wrapper.cpp
///  \brief     rt tps Database Wrapper
///
///
///  \version 1.0
///  \date   July 8, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "rt_tps_database_wrapper.h"

// c++ header
#include <cctype>       // for string toupper
#include <algorithm>    // for algorithm transform
#include <vector>
#include <ctime>        //for get system time

// third library header
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/filesystem.hpp"//create directory
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_uid_generater.h"

#include "mcsf_systemenvironment_factory.h"//systemenvironment
#include "rt_tps_database_connect.h" // create_database
#include "rt_tps_database_object_helper.h"
#include "rt_tps_database_sql_generater.h"
#include "rt_tps_database_client.h"
#include "rt_tps_database_client_tcp.h"

#include "tps_performance.h"
#pragma comment(lib, "libmysql.lib")

//memory leak detected
//#include "vld.h"

//for pc-lint
//lint --e{1702}   (Info -- operator 'operator!=' is both an ordinary function 'operator!=(const struct 
//////////////////////////////////////////////////////////////////////////
RT_TPS_DATABASE_BEGIN_NAMESPACE;

//boost::recursive_mutex* g_DBInitRecMutex = new boost::recursive_mutex();

RtDatabaseWrapper::RtDatabaseWrapper(void)
    : IRtDatabaseWrapper(),
    m_sUser(""),
    m_sPassword(""),
    m_sDatabaseName(""),
    m_sHostName(""),
    m_uiPort(0),
    m_sImagesFolder(""),
    m_pMySql(nullptr),
    m_pDBUtils(nullptr),
    m_pHelper(nullptr),
    m_pClient(nullptr),
    m_pClientTcp(nullptr),
    m_bRollBack(false),
    m_iTransCount(0),
    m_sTpsVersion(""),
    m_sDefaultEquipmentUid(""),
    m_bIsTcp(true)
{
    m_dbrecmutext = new boost::recursive_mutex();
    if (m_bIsTcp)
    {
        m_pClientTcp = new RtDatabaseClientTcp(m_sHostName,0);
    } 
    else
    { 
        m_pClient = new RtDatabaseClient(m_sHostName,0);
    }

}

RtDatabaseWrapper::~RtDatabaseWrapper()
{
    this->Finalize();
}

/////////////////////////////////////////////////////////////////
///  \brief        
///                 As a user I want a new interface of Initialize,
///                 so that I can get the status about the connection to DB 
///                 before using the other interfaces of DBWapper. 
///                 This interface will use McsfFileParser to parse McsfDatabaseConfig.xml and connect to DB 
//                  then return the connection status.
///  \param[in]     None
///
///  \param[out]    None
///  \return    bool 
///                 true: successful connection to DB
///                 false: failed connection to DB 
///  \pre \e  
/////////////////////////////////////////////////////////////////
bool RtDatabaseWrapper::Initialize ()
{
    //boost::recursive_mutex::scoped_lock lock(*g_DBInitRecMutex);
    // pointer to file parser
    Mcsf::IFileParser* pFileParser=NULL;
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper initialize start!";

    TPS_LOG_TRACE_INFO<<"[TRACE]RtDatabaseWrapper initialize start!";

    // get xml file parser object
    pFileParser = Mcsf::ConfigParserFactory::Instance()->GetXmlFileParser();
    if (NULL == pFileParser)
    {
        TPS_LOG_DEV_ERROR<<("GetXmlFileParser output pFileParser is null!");
        return false;
    }       
    // initialize IFileParser resource
    pFileParser->Initialize();
    const bool bRet = GetConfigFile(pFileParser);

    // clean up IFileParser resource
    pFileParser->Terminate();
    delete pFileParser;
    pFileParser = NULL;

    if (!bRet)
    {
        TPS_LOG_DEV_ERROR<<("Database Initialize Failed!");
        return bRet;
    }

    RtEquipment* pEqu = nullptr;
    int iRet = this->GetDefaultEquipment(pEqu);
    if (DB_CODES_SUCCESS == iRet && nullptr != pEqu)
    {                                     
        m_sDefaultEquipmentUid = pEqu->get_uid();
    }
    else
    {
        TPS_LOG_DEV_WARNING<<"No default equipment, now create one.";
        RtEquipment equ(true);         
        equ.set_manufacturer("UIH");
        equ.set_softwareversion(m_sTpsVersion);
        m_sDefaultEquipmentUid = equ.get_uid();
        iRet = InsertEquipment(equ);
        if (DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR<<"Insert equipment failed with uid:"<<m_sDefaultEquipmentUid;
            DEL_PTR(pEqu);
            return false;
        }
        else
        {          
            TPS_LOG_DEV_WARNING<<"Create default equipment with uid:"<<m_sDefaultEquipmentUid;
        }
    }

    DEL_PTR(pEqu);

    TPS_LOG_DEV_INFO <<"RtDatabaseWrapper initialize end!";
    TPS_LOG_TRACE_INFO<<"[TRACE]RtDatabaseWrapper initialize end!";
   // this->AutoTransOff();
    return bRet;
}

bool RtDatabaseWrapper::Finalize()
{
    TPS_LOG_DEV_WARNING<<("RtDatabaseWrapper start finalizing");
    if (nullptr != m_pMySql)
    {
        mysql_close(m_pMySql);
        m_pMySql = nullptr;
    }

    DEL_PTR(m_pDBUtils);
    DEL_PTR(m_dbrecmutext);
    DEL_PTR(m_pHelper);
    DEL_PTR(m_pClient);
    DEL_PTR(m_pClientTcp);
    return true;
}

void RtDatabaseWrapper::BeginTrans()
{
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized");
        return;
    }
    if (0==m_iTransCount){
        const std::string sSQL = "BEGIN";
        if(!m_pHelper->MysqlRealQuery(sSQL)){
            TPS_LOG_DEV_ERROR<<(sSQL);
            return;
        }
    }
    m_iTransCount++;
}

bool RtDatabaseWrapper::Commit()
{
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized";
        return false;
    }

    if (0== --m_iTransCount)
    {
        std::string sSql;
        if (!m_bRollBack)
        {
            sSql = "COMMIT";
        }else
        {
            sSql = "ROLLBACK";
        }
        if(!m_pHelper->MysqlRealQuery(sSql))
        {
            TPS_LOG_DEV_ERROR<<(sSql);
            return false;
        }
    }

    return true;
}

void RtDatabaseWrapper::RollBack()
{
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized");
        return;
    }
    if (!m_bRollBack){
        m_bRollBack = true;
    }

    if (0==--m_iTransCount){
        const std::string sSQL = "ROLLBACK";
        if(!m_pHelper->MysqlRealQuery(sSQL)){
            TPS_LOG_DEV_ERROR<<(sSQL);
            return;
        }
        m_bRollBack = false;
    }
}

void RtDatabaseWrapper::AutoTransOff()
{
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized");
        return;
    }
    const std::string sSQL = "set autocommitment = 0";
    if(!m_pHelper->MysqlRealQuery(sSQL))
    {
        TPS_LOG_DEV_ERROR<<(sSQL);
        return;
    }
}

void RtDatabaseWrapper::ClearAllPatientLocks()
{
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized");
        return;
    }
    const std::string sSql = "delete from tmspatientlock";
    if(!m_pHelper->MysqlRealQuery(sSql)){
        TPS_LOG_DEV_ERROR<<(sSql);
        return;
    }
}

bool RtDatabaseWrapper::GetConfigFile(Mcsf::IFileParser* const pFileParser)
{
    if (nullptr == pFileParser)
    {
        TPS_LOG_DEV_ERROR<<"pFileParser is null.";
        return false;
    }

    m_sUser="";
    m_sPassword="";
    m_sDatabaseName="";
    m_sHostName="";
    int iPort = 0;
    m_sImagesFolder ="";

    //SystemEnvironmentConfig
        Mcsf::ISystemEnvironmentConfig* pSystemEnvironmentConfig
        = Mcsf::ConfigSystemEnvironmentFactory::Instance()->GetSystemEnvironment();
    if ( nullptr == pSystemEnvironmentConfig )
    {
        TPS_LOG_DEV_ERROR<<"GetSystemEnvironment pSystemEnvironmentConfig is null!";
        return false;
    }

    //get version of TPS
    //D:/UIH/version.xml
    const std::string sUihRootPath = pSystemEnvironmentConfig->GetRootPath();
    const std::string versionXml = sUihRootPath + RT_TPS_VERSION;
    DEL_PTR(pSystemEnvironmentConfig);
    if (sUihRootPath.empty())
    {
        TPS_LOG_DEV_ERROR<<"SystemEnvironment :: sUihRootPath failed:"<<sUihRootPath;
        return false;
    }
    bool bRet = pFileParser->ParseByURI(versionXml);
    if (!bRet)
    {
        TPS_LOG_DEV_ERROR<<"Failed to parse version_xml file:" << versionXml;
        TPS_PRINTF_DEBUG("\nFailed to parse version_xml file: %s\n", versionXml.c_str());
        return false;
    }
    if(!pFileParser->GetAttributeStringValue("/ClientComponents/TPS", "Version", &m_sTpsVersion))
    {
        TPS_LOG_DEV_ERROR<<"FileParser TPS version failed! Version:" << m_sTpsVersion;
        return false;
    }
    //std::cout<< m_sTpsVersion << std::endl;
    //TPS_A001.03.0.0.19937.Re.20160328
    //TODO
    //m_sTpsVersion = m_sTpsVersion.substr(0,11);

    //////////////////////////////////////////////////////////////////////////
    //get host name
    //D:/UIH/appdata/rt/config/RTClient.xml
    const std::string sClientXml = sUihRootPath + "appdata/rt/config/" + RT_TPS_CLIENT;
    bRet = pFileParser->ParseByURI(sClientXml);
    if (!bRet){
        TPS_LOG_DEV_ERROR<<"Failed to parse RTClient.xml:" << sClientXml;
        TPS_PRINTF_DEBUG("\nFailed to parse RTClient.xml: %s\n", sClientXml.c_str());
        return false;
    }
    if( !pFileParser->GetStringValueByTag("Ip", &m_sHostName)){
        TPS_LOG_DEV_ERROR<<"FileParser HostName failed! HostName:" << m_sHostName;
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    std::string sFileName(RT_TPS_DATABASE_CONFIG_NAME);
    sFileName = sUihRootPath + "appdata/rt/tps/config/" + sFileName;
    bRet = pFileParser->ParseByURI(sFileName);
    if (!bRet)
    {
        TPS_LOG_DEV_ERROR<<"Failed to parse database config file:" << sFileName;
        TPS_PRINTF_DEBUG("\nFailed to parse database config file: %s\n", sFileName.c_str());
        return false;
    }

    //{ parse xml
    if( !pFileParser->GetStringValueByTag("UserName", &m_sUser))
    {
        TPS_LOG_DEV_ERROR<<("UserName failed!" + m_sUser);
        return false;
    }

    if( !pFileParser->GetStringValueByTag("Password", &m_sPassword))
    {
        TPS_LOG_DEV_ERROR<<("FileParser Password failed! Password:" + m_sPassword);
        return false;
    }

    if( !pFileParser->GetStringValueByTag("DatabaseName", &m_sDatabaseName))
    {
        TPS_LOG_DEV_ERROR<<("FileParser DatabaseName failed! DatabaseName:" + m_sDatabaseName);
        return false;
    }

    //get cuurent version
    std::string sCurrentVersion;
    if( !pFileParser->GetStringValueByTag("Version", &sCurrentVersion))
    {
        TPS_LOG_DEV_ERROR<<"FileParser current version failed! version:" << m_sDatabaseName;
        return false;
    }

    std::string sPreviousVersion;
    if( !pFileParser->GetStringValueByTag("Version_Previous", &sPreviousVersion))
    {
        TPS_LOG_DEV_ERROR<<"FileParser previous version failed! previous version:" << sPreviousVersion;
        return false;
    }

    //if( !pFileParser->GetStringValueByTag("HostName", &m_sHostName)){
    //    TPS_LOG_DEV_ERROR<<("FileParser HostName failed! HostName:" + m_sHostName);
    //    return false;
    //}

    bRet = pFileParser->GetIntValueByTag("Port", &iPort);
    bRet = pFileParser->GetStringValueByTag("ImagesFolder", &m_sImagesFolder);
    //accepted two versions sometimes if no differences for TPS 
    int iAcceptedVersions = 0;
    bRet = pFileParser->GetIntValueByTag("AcceptedVersions", &iAcceptedVersions);
    //} parse xml

    m_uiPort = static_cast<unsigned int>(iPort);

    //{ test connect to DB
    std::string sError = boost::lexical_cast<std::string>(m_uiPort);
    sError = "User:"+m_sUser +",Password:******,DatabaseName:" 
        + m_sDatabaseName +",HostName:" + m_sHostName + ",Port:" + sError;
    std::string sDescription = "----RtDatabaseWrapper Initialize: " + sError + "\nDBWrapper Version:";
    sDescription += sCurrentVersion;
    TPS_LOG_DEV_INFO<<(sDescription);


    if(!bRet
        ||m_sUser.empty()
        ||m_sPassword.empty()
        ||m_sDatabaseName.empty()
        ||m_sHostName.empty()
        ||iPort<1
        ||m_sImagesFolder.empty()){
        TPS_LOG_DEV_ERROR<<("pFileParser->GetStringValueByTag Error!");
        return false;
    }

    if (!create_database (m_sUser, m_sPassword,m_sDatabaseName,m_sHostName,m_uiPort,
        m_pMySql))
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized probably caused by wrong config or mysql dead");
        return false;
    }

    DEL_PTR(m_pDBUtils);
    m_pDBUtils = new RtDatabaseUtils();
    DEL_PTR(m_pHelper);
    m_pHelper = new RtDatabaseHelper(m_pMySql);

    const unsigned short max_retries = 5;
    for (unsigned short retry_count (0); ; retry_count++)
    {
        try
        {
            std::string sSQL = "SET GLOBAL innodb_flush_log_at_trx_commit = 0 ";
            if(!m_pHelper->MysqlRealQuery(sSQL)){
                TPS_LOG_DEV_ERROR<<(sSQL);
                return false;
            }

            //options
            //sSQL = "set global wait_timeout=28800";
            //if(!m_pHelper->MysqlRealQuery(sSQL))
            //{
            //    TPS_LOG_DEV_ERROR<<(sSQL);
            //    return false;
            //}

            //sSQL = "set global interactive_timeout=28800";
            //if(!m_pHelper->MysqlRealQuery(sSQL))
            //{
            //    TPS_LOG_DEV_ERROR<<(sSQL);
            //    return false;
            //}

            //sSQL = "set global max_allowed_packet=10M";
            //if(!m_pHelper->MysqlRealQuery(sSQL))
            //{
            //    TPS_LOG_DEV_ERROR<<(sSQL);
            //    return false;
            //}

            sSQL = GET_DBVERSION;
            if(!m_pHelper->MysqlRealQuery(sSQL)){
                TPS_LOG_DEV_ERROR<<(sSQL);
                return false;
            }

            MYSQL_RES *myquery = mysql_store_result(m_pMySql);
            if (nullptr == myquery){
                return false;
            }

            MYSQL_ROW row = nullptr;
            const unsigned int num_fields = mysql_num_fields(myquery);
            std::string DBVersion("");
            if (num_fields >0 ){
                if(nullptr != (row = mysql_fetch_row(myquery))){
                    DBVersion = row[0] ? row[0] : "";
                }
            }
            //mysql_free_result
            if (nullptr != myquery)
            {
                mysql_free_result(myquery);
                myquery = nullptr;
            }

            if (2 == iAcceptedVersions )
            {
                if (0 != DBVersion.compare(sCurrentVersion) 
                    && 0 != DBVersion.compare(sPreviousVersion))
                {
                        std::string sVersion(sCurrentVersion);
                        sVersion += " OR ";
                        sVersion += sPreviousVersion; 
                        const std::string sLog = "RtTpsDatabaseWrapper version(" + sVersion + ") is not compatible with Database tables("+ DBVersion +")!";
                        TPS_LOG_DEV_ERROR<<(sLog);
                        TPS_PRINTF("%s\n",sLog.c_str());//to be deleted
                        return false;
                }
            } 
            else
            {
                if (0 != DBVersion.compare(sCurrentVersion))
                {
                    const std::string sVersion(sCurrentVersion);
                    const std::string sLog = "RtTpsDatabaseWrapper version(" + sVersion + ") is not compatible with Database tables("+ DBVersion +")!";
                    TPS_LOG_DEV_ERROR<<(sLog);
                    TPS_PRINTF("%s\n",sLog.c_str());//to be deleted
                    return false;
                }
            }
            break;
        }
        catch (const std::exception& e)
        {
            //connect_timeout
            if (retry_count > max_retries)
            {
                TPS_LOG_DEV_ERROR<<("retry_limit_exceeded or timeout:" + std::string(e.what()));
                return false;
            }
            else
            {
                continue;
            }
        }
    }
    //} test connect to DB

    m_IsLocalhost = (m_sHostName == "localhost" ||  m_sHostName == "127.0.0.1");
//    m_IsLocalhost = false;
    if (m_bIsTcp)
    {
        m_pClientTcp->SetParameters(m_sHostName, 8731);
    } 
    else
    { 
        m_pClient->SetParameters(m_sHostName,12350);
        m_pClient->SetWebPageDownload("/FileTransferService/DownloadFile");
        m_pClient->SetWebPageUpload("/FileTransferService/UploadDCMFile/null");
    }
    return true;
}

bool RtDatabaseWrapper::SetConnectionParameters(
    const std::string& sUser,
    const std::string& sPassword,
    const std::string& sDatabaseName,
    const std::string& sHostName,
    const unsigned int& uiPort)
{
    //boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    // test connect to DB

    m_sUser = sUser;
    m_sPassword = sPassword;
    m_sDatabaseName = sDatabaseName;
    m_sHostName = sHostName;
    m_uiPort=uiPort;

    std::string sError = boost::lexical_cast<std::string>(uiPort);
    sError = "User:"+sUser +",Password:" + sPassword +",DatabaseName:" 
        + sDatabaseName +",HostName:" + sHostName + ",Port:" + sError;
    std::string sDescription("----SetConnectionParameters: ");
    sDescription += sError + "\nDBVersion:";
    //sDescription += RT_TPS_DATABASE_VERSION_STR;

    TPS_LOG_DEV_INFO<<(sDescription);
    if (nullptr != m_pMySql)
    {
        mysql_close(m_pMySql);
        m_pMySql = nullptr;
    }

    //test connect to DB
    if (!create_database (m_sUser, m_sPassword,m_sDatabaseName,m_sHostName,m_uiPort, m_pMySql)){
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized");
        return false;
    }

    DEL_PTR(m_pDBUtils);
    m_pDBUtils = new RtDatabaseUtils();
    /*
    {
    if (nullptr != m_pDBUtils)
    {
    delete m_pDBUtils;
    m_pDBUtils = nullptr;
    }

    m_pDBUtils = new RtDatabaseUtils(m_pMySql);
    const std::string sSQL ="select VersionUID from DBVersionTable WHERE VersionUID IS NOT NULL";
    if(!m_pHelper->MysqlRealQuery(sSQL))
    {
    TPS_LOG_DEV_ERROR<<(sSQL);
    return false;
    }

    MYSQL_RES *myquery = mysql_store_result(m_pMySql);
    if (nullptr == myquery)
    {
    return false;
    }

    MYSQL_ROW row = nullptr;
    const unsigned int num_fields = mysql_num_fields(myquery);
    std::string DBVersion("");
    if (num_fields >0 )
    {
    if(nullptr != (row = mysql_fetch_row(myquery)))
    {
    DBVersion = row[0] ? row[0] : "";
    }
    }
    //mysql_free_result
    if (nullptr != myquery)
    {
    mysql_free_result(myquery);
    myquery = nullptr;
    }

    if (RT_TPS_DATABASE_VERSION_STR != DBVersion)
    {
    const std::string sVersion(RT_TPS_DATABASE_VERSION_STR);
    const std::string sLog = "McsfDBWrapper version(" + sVersion + ") is not compatible with Database tables!";
    TPS_LOG_DEV_ERROR<<(sLog);
    TPS_PRINTF_DEBUG("%s\n",sLog.c_str());//to be deleted
    return false;
    }  
    }
    */

    m_IsLocalhost = (m_sHostName == "localhost" ||  m_sHostName == "127.0.0.1");
    if (m_bIsTcp)
    {
        m_pClientTcp->SetParameters(m_sHostName, 8731);
    } 
    else
    { 
        m_pClient->SetParameters(m_sHostName,12350);
        m_pClient->SetWebPageDownload("/FileTransferService/DownloadFile");
        m_pClient->SetWebPageUpload("/FileTransferService/UploadDCMFile/null");
    }
    return true;
}

const std::string RtDatabaseWrapper::ReturnCodeToString(const int returnCode) const
{
    std::string sReturn = "Invalid Return Code!";
    if (returnCode >= DB_CODES_SUCCESS && returnCode <= DB_CODES_UNKNOWN){
        ENUM_TO_STRING_ERROR_CODES[returnCode];
    }
    return sReturn;
}

int RtDatabaseWrapper::EraseObject(const std::string& sUid, RtDbDef::OBJECT_TYPE objType) const
{
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    std::string sSql("");
    const std::string sTableName = std::string(RtDbDef::ENUM_TO_STRING_OBJECT_TYPE[objType]);
    //for series
    switch(objType)
    {
    //case RtDbDef::TYPE_SERIES:
    //    {
    //        sSql = "DELETE FROM tmslongblob WHERE uid = (select seriespixeldatauid from tmsseries where ";
    //        sSql += "uid ='" + sUid + "')";
    //    }
    //    break;
    case RtDbDef::TYPE_SERIESTABLE:
        {
            sSql = "DELETE FROM " + sTableName + " WHERE ";
            sSql += "seriesuid ='" + sUid + "'";
        }
        break;
    default:
        {
            sSql = "DELETE FROM " + sTableName + " WHERE ";
            sSql += "uid ='" + sUid + "'";
        }
        break;
    }

    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if(!m_pHelper->MysqlRealQuery(sSql))
    {
        TPS_LOG_DEV_ERROR<<sSql;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"Erase table:"<<sTableName << " with uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::EraseObject(const std::string& sFirstUid, const std::string& sSecondUid,
    RtDbDef::OBJECT_TYPE objType) const
{
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    //delete record
    const std::string sTableName = std::string(RtDbDef::ENUM_TO_STRING_OBJECT_TYPE[objType]);
    std::string sSql = "DELETE FROM " + sTableName + " WHERE ";
    if(RtDbDef::TYPE_IMAGETRANSFORMATION == objType)
    {
        sSql += "originalseriesuid = '" + sFirstUid + "' AND transfomationseriesuid ='" +sSecondUid +"'";
    }
    else if(RtDbDef::TYPE_BEAM_POI == objType)
    {
        sSql += "beamuid = '" + sFirstUid + "' AND poiuid = '" +sSecondUid +"'";
    }
    else if(RtDbDef::TYPE_SERIESTABLE == objType)
    {
        sSql += "seriesuid = '" + sFirstUid + "' AND tableuid = '" +sSecondUid +"'";
    }

    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if(!m_pHelper->MysqlRealQuery(sSql))
    {
        TPS_LOG_DEV_ERROR<<sSql;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"Erase table:"<< sTableName << " with sFirstUid:"<<sFirstUid<<"SecondUid:"<<sSecondUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::EraseObjectUnderCondition(const std::string& sCondition, RtDbDef::OBJECT_TYPE objType) const
{
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    //delete record               
    const std::string sTableName = std::string(RtDbDef::ENUM_TO_STRING_OBJECT_TYPE[objType]);
    const std::string sSql = "DELETE FROM " + sTableName + " WHERE " + sCondition;
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if(!m_pHelper->MysqlRealQuery(sSql))
    {
        TPS_LOG_DEV_ERROR<<sSql;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"Erase table:"<< sTableName << " with condition:"<< sSql;
    return DB_CODES_SUCCESS;
}


std::string RtDatabaseWrapper::CalcHash(const void* pObject, RtDbDef::OBJECT_TYPE objType) const
{
    std::string sMD5Value("");
    switch(objType)
    {
    case RtDbDef::TYPE_MACHINE:
        {
            RtMachine* pObj = (RtMachine*)pObject;
            if (nullptr != pObj)
            {
                m_pHelper->CalcHash(*pObj, &sMD5Value);
            }
        }
        break;
    case RtDbDef::TYPE_PATIENT:
        {
            RtPatient* pObj = (RtPatient*)pObject;
            if (nullptr != pObj)
            {
                m_pHelper->CalcHash(*pObj, &sMD5Value);
            }
        }
        break;
    default:
        break;
    }
    return sMD5Value;
}

bool RtDatabaseWrapper::QueryDBInTable(const std::string& sUid, RtDbDef::OBJECT_TYPE objType) const
{
    return m_pHelper->QueryDBInTable(sUid, objType);
}

int RtDatabaseWrapper::SaveDataFromXmltoDB(const std::string& xmlPath, const std::string& tableName) const
{
    std::string sql = "LOAD XML LOCAL INFILE '" + xmlPath + "' INTO TABLE " + tableName + ";";
    if (!m_pHelper->MysqlRealQuery(sql)){
        TPS_LOG_DEV_ERROR << sql;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO << ("ExcuteSQLScript:" + sql);
    return DB_CODES_SUCCESS;
}

bool RtDatabaseWrapper::QueryDBInTableViaSpecifiedKey(const std::string& sKey, const std::string& sValue, 
    RtDbDef::OBJECT_TYPE objType) const
{
    return m_pHelper->QueryDBInTableViaSpecifiedKey(sKey, sValue, objType);
}

bool RtDatabaseWrapper::QueryDBInTable(const std::string& sFirstUid, const std::string& sSecondUid, 
    RtDbDef::OBJECT_TYPE objType) const
{
    return m_pHelper->QueryDBInTable(sFirstUid, sSecondUid, objType);
}

int RtDatabaseWrapper::UpdatePatientObject(RtPatient* pPatient)  const
{ 
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql || !pPatient)
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized or pPatient is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(pPatient->get_uid());
    if (!pPatient->is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    if (!m_pHelper->QueryDBInPatient(sUid)){
        TPS_LOG_DEV_ERROR<<("ERROR_DB_PATIENT_UID_NOT_EXIST:"   + sUid);
        return DB_CODES_UID_NOT_EXIST;
    }

    std::string sSQL("");
    //UpdatePatientTableFromObject(pPatient, sSQL);
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    pPatient->set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(*pPatient, false)){
        TPS_LOG_DEV_ERROR<<(sSQL);
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<("UpdatePatientObject:" + sUid);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdatePoiObject(RtPoi* pPoi) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized or pMlcshape is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(pPoi->get_uid());
    if (!pPoi->is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    if(!m_pHelper->QueryDBInTable(sUid, TABLE_TPSPOI))
    {
        TPS_LOG_DEV_ERROR <<"DB_CODES_UID_NOT_EXIST:" << sUid;
        return DB_CODES_UID_NOT_EXIST;
    }

    pPoi->setdirty_field(POI_ROINUMBERINDCM, false);

    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    pPoi->set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(*pPoi, false)){
        TPS_LOG_DEV_ERROR<<"UpdatePoiObject failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper UpdatePoiObject success with Uid:" + sUid);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateDosegridObject(RtDosegrid* pDosegrid) 
{
   boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
   if (nullptr == m_pMySql || nullptr == pDosegrid) 
   {
       TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized or pDosegrid is null");
       return DB_CODES_MEMORY_ALLOCATION_FAILURE;
   }

   const std::string sUid(pDosegrid->get_uid());
   if (!pDosegrid->is_any_dirty())
   {
       TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
       return DB_CODES_INPUT_VIOLATION;
   }

   if (pDosegrid->get_isdosevalid())
   {
       if (DB_CODES_SUCCESS != this->SaveDosegridBufferToFile(pDosegrid))
       {
           TPS_LOG_DEV_ERROR << "SaveDosegrid BufferToFile failed with uid:" << sUid;
           return DB_CODES_EXCEPTION;
       }
   }
   else
   {
       //delete file
       this->DeleteFileByPath(pDosegrid->get_dosegridfilepath());
       //pDosegrid->set_dosegridfilepath("");
   }

   const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
   pDosegrid->set_updatetime(ptimeLocalTimeInstance);

   if(!m_pHelper->InsertUpdateDBFromObject(*pDosegrid, false))
   {
       TPS_LOG_DEV_ERROR<<"UpdateDosegrid failed with uid:"<<sUid;
       return DB_CODES_EXCEPTION;
   }

   //DATETIME_BOOST dt = m_pHelper->GetUpdatetime(sUID, RtDbDef::TYPE_DOSEGRID);
   //if (!dt.is_not_a_date_time()){
   //    pDosegrid->set_updatetime(dt);
   //}

   TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Update Dosegrid Object success with Uid:" << sUid;
   return DB_CODES_SUCCESS;
}

// update voi include contours
int RtDatabaseWrapper::UpdateVoiObject(RtVoi& voi) 
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized.");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(voi.get_uid());
    if (!voi.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    if(!m_pHelper->QueryDBInTable(sUid, TABLE_VOI)) {
        TPS_LOG_DEV_ERROR <<"DB_CODES_UID_NOT_EXIST:" << sUid;
        return DB_CODES_UID_NOT_EXIST;
    }

    voi.setdirty_field(VOI_ROINUMBERINDCM, false);//for export only

    BeginTrans();

    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    voi.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(voi, false)){
        RollBack();
        TPS_LOG_DEV_ERROR<<"UpdateVoiObject failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    //delete old contours 
    const std::string sCondition = "voiuid='" + sUid + "'";
    if(DB_CODES_SUCCESS != EraseObjectUnderCondition(sCondition, RtDbDef::TYPE_CONTOUR)){
        RollBack();
        TPS_LOG_DEV_ERROR<<"UpdateVoiObject failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }

    const std::vector<RtContour*>& vContours = voi.get_contours();
    for (auto itr=vContours.begin(); itr!=vContours.end(); ++itr){
        if(!m_pHelper->InsertUpdateDBFromObject(*(*itr), true)){//always insert
            RollBack();
            TPS_LOG_DEV_ERROR<<"UpdateVoiObject failed with uid:"<<sUid;
            return DB_CODES_EXCEPTION;
        }
    }
    if(!Commit()){
        return DB_CODES_EXCEPTION;
    }

    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper UpdateVoiObject success with Uid:" + sUid);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateContourObject(RtContour& contour) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized.");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid=contour.get_uid();
    if (!contour.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    contour.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(contour, false))
    {
        TPS_LOG_DEV_ERROR<<"UpdateVoiObject failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper UpdateVoiObject success with Uid:" + sUid);
    return DB_CODES_SUCCESS;
}

// update voi attributes exclude contours
int RtDatabaseWrapper::UpdateVoiAttributes(RtVoi& voi) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized or pMlcshape is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(voi.get_uid());
    if (!voi.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    if(!m_pHelper->QueryDBInTable(sUid, TABLE_VOI))
    {
        TPS_LOG_DEV_ERROR <<"DB_CODES_UID_NOT_EXIST:" << sUid;
        return DB_CODES_UID_NOT_EXIST;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    voi.set_updatetime(ptimeLocalTimeInstance);
    voi.setdirty_field(VOI_ROINUMBERINDCM, false);//for export only
    if(!m_pHelper->InsertUpdateDBFromObject(voi, false)){
        TPS_LOG_DEV_ERROR<<"UpdateVoiAttributes failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper UpdateVoiAttributes success with Uid:" + sUid);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateMlcshapeObject(RtMlcshape& mlcshape) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized or pMlcshape is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(mlcshape.get_uid());
    if (!mlcshape.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    if(!m_pHelper->QueryDBInTable(sUid, TABLE_MLCSHAPE)) 
    {
        TPS_LOG_DEV_ERROR <<"DB_CODES_UID_NOT_EXIST:" << sUid;
        return DB_CODES_UID_NOT_EXIST;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    mlcshape.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(mlcshape, false)){
        TPS_LOG_DEV_ERROR<<"UpdateMlcshapeObject failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper UpdateMlcshapeObject success with Uid:" + sUid);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateCt2densityObject(RtCt2density& ct2density) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized or pMlcshape is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(ct2density.get_uid());
    if (!ct2density.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    if(!m_pHelper->QueryDBInTable(sUid, TABLE_CT2DENSITY)) 
    {
        TPS_LOG_DEV_ERROR <<"DB_CODES_UID_NOT_EXIST:" << sUid;
        return DB_CODES_UID_NOT_EXIST;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    ct2density.set_updatetime(ptimeLocalTimeInstance);

    if(!m_pHelper->InsertUpdateDBFromObject(ct2density, false)){
        TPS_LOG_DEV_ERROR<<"UpdateCt2densityObject failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }

    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper UpdateCt2densityObject success with Uid:" + sUid);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateDoseoptgoalbaseObject(RtDoseoptgoalbase& doseoptgoalbase) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(doseoptgoalbase.get_uid());
    if (!doseoptgoalbase.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    if(!m_pHelper->QueryDBInTable(sUid, TABLE_DOSEOPTGOALBASE)) 
    {
        TPS_LOG_DEV_ERROR <<"DB_CODES_UID_NOT_EXIST:" << sUid;
        return DB_CODES_UID_NOT_EXIST;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    doseoptgoalbase.set_updatetime(ptimeLocalTimeInstance);
    doseoptgoalbase.setdirty_field(DOSEOPTGOALBASE_ID,false);
    if(!m_pHelper->InsertUpdateDBFromObject(doseoptgoalbase, false))
    {
        TPS_LOG_DEV_ERROR<<"Update DoseoptgoalbaseObject failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Update DoseoptgoalbaseObject success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateDrrsetting(RtDrrsetting& drrSetting) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(drrSetting.get_uid());
    if (!drrSetting.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    if(!m_pHelper->QueryDBInTable(sUid, TABLE_DRRSETTING)) 
    {
        TPS_LOG_DEV_ERROR <<"DB_CODES_UID_NOT_EXIST:" << sUid;
        return DB_CODES_UID_NOT_EXIST;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    drrSetting.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(drrSetting, false)){
        TPS_LOG_DEV_ERROR<<"Update Drrsetting failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Update Drrsetting success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetPatientByUid(const std::string& sPatientUid, RtPatient*& pPatient)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_PATIENT \
        "WHERE uid ='" +sPatientUid +"'";

    std::vector<RtPatient*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList))
    {
        TPS_LOG_DEV_ERROR<<"GetPatient failed with uid:" <<sPatientUid;
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty())
    {
        return DB_CODES_UID_NOT_EXIST;
    }
    pPatient = vList.front();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetDefaultEquipment(RtEquipment*& pEquipment)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_EQUIPMENT \
        "WHERE softwareversion='" + m_sTpsVersion + "' limit 1";

    std::vector<RtEquipment*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList))
    {
        TPS_LOG_DEV_ERROR<<"GetDefault Equipment failed.";
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty())
    {
        return DB_CODES_UID_NOT_EXIST;
    }
    pEquipment = vList.front();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetDosegridObjectByNormgroupUid(const std::string& sNormgroupUid, RtDosegrid* pDosegrid)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_UNKNOWN;
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized");
        iRet = DB_CODES_MEMORY_ALLOCATION_FAILURE;
        return iRet;
    }
    if (nullptr == pDosegrid){
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_DOSEGRID \
        "WHERE normgroupuid ='" +sNormgroupUid +"'";

    std::vector<RtDosegrid*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList))
    {
        TPS_LOG_DEV_ERROR<<("GetDosegridObjectBy NormgroupUid failed with seriesuid:" + sNormgroupUid);
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty()){
        return DB_CODES_UID_NOT_EXIST;
    }
    *pDosegrid = *(vList.front());

    //build dosegrid data
    if (pDosegrid->get_isdosevalid())
    {
        iRet = this->BuildDosegridFromFile(pDosegrid);
        if (DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR<<"failed to BuildDosegrid FromFile for dosegrid belong to NormgroupUid:"<<sNormgroupUid;
            return iRet;
        } 
    }

    return DB_CODES_SUCCESS;
}


int RtDatabaseWrapper::GetDosegridObjectByBeamUid(const std::string& sBeamUid, RtDosegrid*& pDosegrid)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_UNKNOWN;

    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized");
        iRet = DB_CODES_MEMORY_ALLOCATION_FAILURE;
        return iRet;
    }

    const std::string sSql = SELECT_TABLE_DOSEGRID \
        "WHERE beamuid ='" +sBeamUid +"'";

    std::vector<RtDosegrid*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList))
    {
        TPS_LOG_DEV_ERROR<<("GetDosegrid ObjectByBeamUid failed with seriesuid:" + sBeamUid);
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty())
    {
        TPS_LOG_DEV_ERROR << "RtDosegrid is empty.";
        return DB_CODES_UID_NOT_EXIST;
    }
    if (vList.size() > 1)
    {
        TPS_LOG_DEV_ERROR << "RtDosegrid size > 1";
        for (auto itr = vList.begin(); itr != vList.end(); ++itr)
        {
            DEL_PTR(*itr);
        }
        return DB_CODES_UID_NOT_UNIQUE;
    }
    pDosegrid = vList.front();

    //build dosegrid data
    if (pDosegrid->get_isdosevalid())
    {
        iRet = this->BuildDosegridFromFile(pDosegrid);
        if (DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR<<"failed to BuildDosegrid FromFile for dosegrid belong to beamUid:"<<sBeamUid;
            return iRet;
        } 
    }

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetDosegridObjects(const std::vector<std::string>& dosegridUidList, 
    std::vector<RtDosegrid*>& vRtDosegridList)
{
    vRtDosegridList.clear();
    if (dosegridUidList.empty()) return DB_CODES_SUCCESS;
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_UNKNOWN;

    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized";
        iRet = DB_CODES_MEMORY_ALLOCATION_FAILURE;
        return iRet;
    }

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_DOSEGRID);
    std::for_each(dosegridUidList.begin(), dosegridUidList.end(),
        [&sqlGenerater](std::string dosegridUid) 
    {
            sqlGenerater.SetFilter("uid", dosegridUid, OR);
    });
    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    if (!m_pHelper->FetchDataFromDB(sSql, vRtDosegridList))
    {
        TPS_LOG_DEV_ERROR << ("FetchDataFromDB failed when get dose grid.");
        return DB_CODES_EXCEPTION;
    }

    for (auto itr=vRtDosegridList.begin(); itr!=vRtDosegridList.end(); ++itr)
    {
        if (!(*itr)->get_isdosevalid())
        {
            TPS_LOG_DEV_WARNING << "dose grid is not valid of UID:"<< (*itr)->get_uid();
            continue;
        }
        //build dosegrid data
        iRet = this->BuildDosegridFromFile(*itr);
        if (DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR<<"failed to BuildDosegrid FromFile for dosegrid belong to beamUid:"<<(*itr)->get_uid();
            break;
        } 
    }
    if (DB_CODES_SUCCESS != iRet)
    {
        for (auto itr=vRtDosegridList.begin(); itr!=vRtDosegridList.end(); ++itr)
        {
            DEL_PTR(*itr);
        }
        vRtDosegridList.clear();
    }

    return iRet;
}

int RtDatabaseWrapper::GetSeriesObjectByUid(const std::string& seriesuid, RtSeries*& pSeries)
{
	boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
	if (nullptr == m_pMySql){
		TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized";
		return DB_CODES_MEMORY_ALLOCATION_FAILURE;
	}

	const std::string sSql = SELECT_TABLE_SERIES \
		"WHERE uid ='" +seriesuid +"'";

	std::vector<RtSeries*> vList;
	if(!m_pHelper->FetchDataFromDB(sSql, vList)){
		TPS_LOG_DEV_ERROR<<"GetSeriesObjectByUid failed with seriesuid:" << seriesuid;
		return DB_CODES_EXCEPTION;
	}
	if (vList.empty()) return DB_CODES_UID_NOT_EXIST;
	pSeries = vList.front();

	char* buffer = nullptr;
	std::size_t fileSize(0);
	const std::string sFilePath = pSeries->get_seriespixeldatafilepath();
	int iRet = GetFile(sFilePath, buffer, &fileSize);
	if(0 != iRet|| nullptr == buffer || fileSize < 1)
	{
		TPS_LOG_DEV_ERROR<<"GetSeriesObjectByUid ReadBuffer FromFile failed:"<<seriesuid;
		DEL_PTR(pSeries);
		return iRet;
	}
	pSeries->set_imagedata(buffer, (unsigned long)fileSize);
	DEL_ARRAY(buffer);

	std::vector<RtImage*> imageList;
	if(DB_CODES_SUCCESS != GetImageListBySeriesUid(seriesuid, imageList))
	{
		TPS_LOG_DEV_ERROR<<"GetSeriesObjectByUid GetImageListBySeriesUid failed:"<<seriesuid;
		DEL_PTR(pSeries);
		for (auto itrImg=imageList.begin(); itrImg != imageList.end(); ++itrImg)
		{
			DEL_PTR(*itrImg);
		}
		return iRet;
	}
	RtImage* pFirstImage = *(imageList.cbegin());
	if (nullptr == pFirstImage){
		TPS_LOG_DEV_ERROR<<"pFirstImage is null.";
		return false;
	}
	RtImage3DHeader* imageHeader = pSeries->get_header();
	imageHeader->m_iSliceCount = (const int)imageList.size();
	imageHeader->m_iXDim = pFirstImage->get_columns();
	imageHeader->m_iYDim = pFirstImage->get_rows();
	imageHeader->m_iBitsAllocated = pFirstImage->get_bitsallocated();

	for (auto itrImg=imageList.begin(); itrImg != imageList.end(); ++itrImg)
	{
		DEL_PTR(*itrImg);
	}
	//////////////////////////////////////////////////////////////////////////
	//for more information about the image set 
	iRet = GetSeriesInformation(pSeries);
	if (DB_CODES_SUCCESS != iRet)
	{
		TPS_LOG_DEV_ERROR<<"GetSeriesObjectByUid Get Series Information failed:"<<seriesuid;
		DEL_PTR(pSeries);
		return iRet;
	}

	return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetSeriesByUid(const std::string& seriesuid, RtSeries*& pSeries)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sSql = SELECT_TABLE_SERIES \
        "WHERE uid ='" +seriesuid +"'";

    std::vector<RtSeries*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetSeriesByUid failed with seriesuid:" << seriesuid;
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty()) return DB_CODES_UID_NOT_EXIST;
    pSeries = vList.front();

	std::vector<RtImage*> imageList;
	int iRet = GetImageListBySeriesUid(seriesuid, imageList);
    if(DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR<<"GetImageListBySeriesUid failed:"<<seriesuid;
        DEL_PTR(pSeries);
        for (auto itrImg=imageList.begin(); itrImg != imageList.end(); ++itrImg)
        {
            DEL_PTR(*itrImg);
        }
        return iRet;
    }

  
    char* buffer = nullptr;
    std::size_t fileSize(0);
    const std::string sFilePath = pSeries->get_seriespixeldatafilepath();
    iRet = GetFile(sFilePath, buffer, &fileSize);

    if(0 != iRet|| nullptr == buffer || fileSize < 1)
    {
        TPS_LOG_DEV_ERROR<<"ReadBuffer FromFile failed:"<<seriesuid;

        DEL_PTR(pSeries);
        for (auto itrImg=imageList.begin(); itrImg != imageList.end(); ++itrImg)
        {
            DEL_PTR(*itrImg);
        }
    }

    pSeries->set_imagedata(buffer, (unsigned long)fileSize);
    DEL_ARRAY(buffer);

    if(!m_pDBUtils->BuildImage3DFromSeries(pSeries, imageList, (unsigned long)fileSize))
    {
        TPS_LOG_DEV_ERROR<<"BuildImage3D from Series failed:"<<seriesuid;

        DEL_PTR(pSeries);
        for (auto itrImg=imageList.begin(); itrImg != imageList.end(); ++itrImg)
        {
            DEL_PTR(*itrImg);
        }
        return DB_CODES_EXCEPTION;
    }

    for (auto itrImg=imageList.begin(); itrImg != imageList.end(); ++itrImg)
    {
        DEL_PTR(*itrImg);
    }

    //////////////////////////////////////////////////////////////////////////
    //for more information about the image set 
    iRet = GetSeriesInformation(pSeries);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR<<"Get Series Information failed:"<<seriesuid;
        DEL_PTR(pSeries);
        return iRet;
    }

    return DB_CODES_SUCCESS;
}

bool RtDatabaseWrapper::BuildImage3DFromSeries(RtSeries* pRtSeries, std::vector<RtImage*> imageList, unsigned long ulSize)
{
    return m_pDBUtils->BuildImage3DFromSeries(pRtSeries, imageList, ulSize);

}
int RtDatabaseWrapper::GetSeriesInformation(RtSeries* pSeries)
{
    if (nullptr == pSeries) return DB_CODES_EXCEPTION;

    RtStudy* pStudy = nullptr;
    const std::string sStudyUid = pSeries->get_studyuid();
    int iRet = GetStudyByUid(sStudyUid, pStudy);
    if(DB_CODES_SUCCESS == iRet && nullptr != pStudy)
	{ //phantom series dose not have a patient
        pSeries->set_studyid(pStudy->get_studyid());
        pSeries->set_studydescription(pStudy->get_studydescription());
        const std::string sPatientUid = pStudy->get_patientuid();
        DEL_PTR(pStudy);
        RtPatient* pPatient = nullptr;
        iRet = GetPatientByUid(sPatientUid, pPatient);
        if(DB_CODES_SUCCESS != iRet || nullptr == pPatient){
            TPS_LOG_DEV_ERROR<<"get patient failed:" << sPatientUid;
            DEL_PTR(pPatient);
            return iRet;
        }

        pSeries->set_patientid(pPatient->get_patientid());
        pSeries->set_patientbirthdate(m_pHelper->ConvertDateTime(pPatient->get_patientbirthdate()));
        //pSeries->set_patientage(std::to_string((long long)pPatient->get_age()));
        pSeries->set_patientsex(std::to_string((long long)pPatient->get_patientsex()));
        pSeries->set_patientuid(pPatient->get_uid());
        DEL_PTR(pPatient);
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetStudyByUid(const std::string& studyuid, RtStudy*& pStudy)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_STUDY \
        "WHERE uid ='" +studyuid +"'";
    std::vector<RtStudy*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetStudyByUid failed with studyuid:" << studyuid;
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty()) return DB_CODES_UID_NOT_EXIST;
    pStudy = vList.front();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetCourseListByPatientUid(const std::string& sPatientUid, std::vector<RtCourse*>& RtCourseList) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    RtCourseList.clear();//clear

    const std::string sSql = SELECT_TABLE_COURSE \
        "WHERE patientuid = '" +sPatientUid +"'";

    if(!m_pHelper->FetchDataFromDB(sSql, RtCourseList)){
        TPS_LOG_DEV_ERROR<<("GetCourseListByPatientUid failed with patient uid:" + sPatientUid);
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetPatientListByCondition(const std::string& sCondition, std::vector<RtPatient*>& vList) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vList.clear();

    const std::string sSql = SELECT_TABLE_PATIENT \
        "WHERE " + sCondition;

    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetPatientListBy Condition failed:" << sCondition;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetSeriesList(const std::vector<std::string>& vSeriesList, std::vector<RtSeries*>& seriesList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    //  int iRet = DB_CODES_SUCCESS;

    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR << ("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    seriesList.clear();//clear

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_SERIES);

    std::for_each(vSeriesList.begin(), vSeriesList.end(),
        [&sqlGenerater](std::string seriesUid) {
        sqlGenerater.SetFilter("uid", seriesUid, OR);
    });

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    std::vector<RtSeries*> tmpSeriesList;
    if (!m_pHelper->FetchDataFromDB(sSql, tmpSeriesList)){
        TPS_LOG_DEV_ERROR << ("GetAllPatientObjects failed.");
        return DB_CODES_EXCEPTION;
    }

    //get image list
    std::string sUid;
    int iRet;
    std::vector<RtImage*> imageList;
    char* buffer = nullptr;
    std::size_t fileSize(0);
    for (auto itr=tmpSeriesList.begin(); itr!=tmpSeriesList.end(); ++itr)
    {
        sUid = (*itr)->get_uid();
        iRet = GetImageListBySeriesUid(sUid, imageList);
        if(DB_CODES_SUCCESS != iRet){
            TPS_LOG_DEV_ERROR<<"GetImageListBySeriesUid failed:"<<sUid;
            DEL_PTR(*itr);
            continue;
        }
 
        const std::string sFilePath = (*itr)->get_seriespixeldatafilepath();
        int iRet = GetFile(sFilePath, buffer, &fileSize);
        if(0 != iRet|| nullptr == buffer || fileSize < 1)
        {
            TPS_LOG_DEV_ERROR<<"GetLongblob failed:"<<sUid;
            DEL_PTR(*itr);
            continue;
        }

        (*itr)->set_imagedata(buffer, (unsigned long)fileSize);
        DEL_ARRAY(buffer);

        if(!m_pDBUtils->BuildImage3DFromSeries((*itr), imageList, (unsigned long)fileSize))
        {
            TPS_LOG_DEV_ERROR<<"BuildImage3D From Series failed:"<<sUid;
            DEL_PTR(*itr);
            continue;
        }

        //////////////////////////////////////////////////////////////////////////
        //for more information about the image set 
        iRet = GetSeriesInformation(*itr);
        if (DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR<<"Get Series Information failed:"<<sUid;
            DEL_PTR(*itr);
            continue;
        }

        seriesList.push_back(*itr);
    }
    return DB_CODES_SUCCESS;
}

int  RtDatabaseWrapper::GetSeriesListByCourseUid(const std::string& sCoureseUid, std::vector<RtSeries*>& RtSeriesList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    RtSeriesList.clear();//clear

    const std::string sSql = SELECT_TABLE_SERIES \
        "WHERE courseuid = '" +sCoureseUid +"'";

    std::vector<RtSeries*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<("GetSeriesListByCourseUid failed with seriesuid:" + sCoureseUid);
        return DB_CODES_EXCEPTION;
    }

    //get image list
    int iRet = DB_CODES_SUCCESS;
    char* buffer = nullptr;
    std::size_t fileSize(0);
    for (auto itr=vList.begin(); itr!=vList.end(); ++itr)
    {
        std::vector<RtImage*> imageList;
        const std::string sUid = (*itr)->get_uid();
        iRet = GetImageListBySeriesUid(sUid, imageList);
        if(DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR<<"GetImageListBySeriesUid failed:"<<sUid;
            DEL_PTR(*itr);
            for (auto itrImg=imageList.begin(); itrImg != imageList.end(); ++itrImg)
            {
                DEL_PTR(*itrImg);
            }
            continue;
        }

        TEST_PERFORMANCE_INIT;
        TEST_PERFORMANCE_BEGIN;
        const std::string sFilePath = (*itr)->get_seriespixeldatafilepath();
        int iRet = GetFile(sFilePath, buffer, &fileSize);
        
        TEST_PERFORMANCE_END("ReadBuffer FromFile");
        if(0 != iRet || nullptr == buffer || fileSize < 1)
        {
            TPS_LOG_DEV_ERROR<<"ReadBuffer FromFile failed:"<<sUid;
            DEL_PTR(*itr);
            for (auto itrImg=imageList.begin(); itrImg != imageList.end(); ++itrImg)
            {
                DEL_PTR(*itrImg);
            }
            continue;
        }

        (*itr)->set_imagedata(buffer, (unsigned long)fileSize);
        DEL_ARRAY(buffer);

        if(!m_pDBUtils->BuildImage3DFromSeries((*itr), imageList, (unsigned long)fileSize))
        {
            TPS_LOG_DEV_ERROR<<"BuildImage3D From Series failed:"<<sUid;
            DEL_PTR(*itr);
            iRet = DB_CODES_EXCEPTION;
            for (auto itrImg=imageList.begin(); itrImg != imageList.end(); ++itrImg)
            {
                DEL_PTR(*itrImg);
            }
            continue;
        }

        for (auto itrImg=imageList.begin(); itrImg != imageList.end(); ++itrImg)
        {
            DEL_PTR(*itrImg);
        }

        //////////////////////////////////////////////////////////////////////////
        //for more information about the image set 
        iRet = GetSeriesInformation(*itr);
        if (DB_CODES_SUCCESS != iRet){
            TPS_LOG_DEV_ERROR<<"Get Series Information failed:"<<sUid;
            DEL_PTR(*itr);
            continue;
        }

        RtSeriesList.push_back(*itr);
    }

    if (DB_CODES_SUCCESS != iRet)
    {
        for (int i = 0; i < RtSeriesList.size(); ++i)
        {
            DEL_PTR(RtSeriesList[i]);
        }
        RtSeriesList.clear();
    }
    return iRet;
}

int RtDatabaseWrapper::GetAllPatientObjects(std::vector<RtPatient*>& vRtPatientList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtPatientList.clear();//clear
    const std::string sSql = SELECT_TABLE_PATIENT;

    if(!m_pHelper->FetchDataFromDB(sSql, vRtPatientList)){
        TPS_LOG_DEV_ERROR<<("GetAllPatientObjects failed.");
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetAllCommissionedunitObjects(std::vector<RtCommissionedunit*>& vRtCommissionedunitList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;

    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtCommissionedunitList.clear();//clear

    const std::string sSql = SELECT_TABLE_COMMISSIONEDUNIT;

    //std::vector<RtCommissionedunit*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vRtCommissionedunitList))
    {
        TPS_LOG_DEV_ERROR<<("GetAllCommissionedunitObjects failed.");
        return DB_CODES_EXCEPTION;
    }

    for (auto itr=vRtCommissionedunitList.begin(); itr!=vRtCommissionedunitList.end(); ++itr){
        // read discrete trayfactor value from Commissionedunit_tray table
        std::vector<RtCommissionedunit_tray*> vList;
        GetCommissionedunit_trayListByUnitUid((*itr)->get_uid(), vList);
        std::map<std::string, float> UnitTrayMap;
        for (auto itrMap = vList.begin();itrMap != vList.end(); ++itrMap){
            UnitTrayMap.insert(std::make_pair((*itrMap)->get_trayuid(),(*itrMap)->get_trayfactor()));
            DEL_PTR(*itrMap);
        }
        (*itr)->set_discrete_trayfactor(UnitTrayMap);
    }
    iRet = DB_CODES_SUCCESS;
    return iRet;
}

int RtDatabaseWrapper::GetAllMcphocommissionedunits(std::vector<RtMcphocommissionedunit*>& vMcphocommissionedunit)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vMcphocommissionedunit.clear();
    const std::string sSql = SELECT_TABLE_MCPHOCOMMISSIONEDUNIT;

    if (!m_pHelper->FetchDataFromDB(sSql, vMcphocommissionedunit)) {
        TPS_LOG_DEV_ERROR<<("GetAllMcphocommissionedunits failed.");
        return DB_CODES_EXCEPTION;
    }

    iRet = DB_CODES_SUCCESS;
    return iRet;
}

int RtDatabaseWrapper::GetAllKerneldata(std::vector<RtKerneldata*>& vKerneldata)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vKerneldata.clear();//clear
    const std::string sSql = SELECT_TABLE_KERNELDATA;
    if(!m_pHelper->FetchDataFromDB(sSql, vKerneldata)){
        TPS_LOG_DEV_ERROR<<"GetAllKerneldata failed.";
        return DB_CODES_EXCEPTION;
    }
    iRet = DB_CODES_SUCCESS;
    return iRet;
}

int RtDatabaseWrapper::GetAllKerneldatalists(std::vector<RtKerneldatalist*>& vKerneldatalist)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vKerneldatalist.clear();//clear
    const std::string sSql = SELECT_TABLE_KERNELDATALIST;
    if(!m_pHelper->FetchDataFromDB(sSql, vKerneldatalist)){
        TPS_LOG_DEV_ERROR<<"GetAllKerneldatalists failed.";
        return DB_CODES_EXCEPTION;
    }
    iRet = DB_CODES_SUCCESS;
    return iRet;
}

int RtDatabaseWrapper::GetAllMeasureDatas(std::vector<RtMeasureddata*>& vMeasureData)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR << "DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vMeasureData.clear();//clear
    const std::string sSql = SELECT_TABLE_MEASUREDDATA;
    if (!m_pHelper->FetchDataFromDB(sSql, vMeasureData)){
        TPS_LOG_DEV_ERROR << "GetAllKerneldatalists failed.";
        return DB_CODES_EXCEPTION;
    }
    iRet = DB_CODES_SUCCESS;
    return iRet;
}
int RtDatabaseWrapper::GetAllKernelWeights(std::vector<RtKernelweight*>& vKernelweight)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vKernelweight.clear();
    const std::string sSql = SELECT_TABLE_KERNELWEIGHT;
    if (!m_pHelper->FetchDataFromDB(sSql, vKernelweight))
    {
        TPS_LOG_DEV_ERROR<<"GetAllKernelWeights failed.";
        return DB_CODES_EXCEPTION;
    }
    iRet = DB_CODES_SUCCESS;
    return iRet;
}

int RtDatabaseWrapper::GetAllApplicators(std::vector<RtApplicator*>& vApplicator)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR << "DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vApplicator.clear();
    const std::string sSql = SELECT_TABLE_APPLICATOR;
    if (!m_pHelper->FetchDataFromDB(sSql, vApplicator)){
        TPS_LOG_DEV_ERROR << "GetAllApplicators failed.";
        return DB_CODES_EXCEPTION;
    }
    iRet = DB_CODES_SUCCESS;
    return iRet;
}

int RtDatabaseWrapper::GetAllSeriesgroups(std::vector<RtSeriesgroup*>& vSeriesgroup)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR << "DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vSeriesgroup.clear();
    const std::string sSql = SELECT_TABLE_SERIESGROUP;
    if (!m_pHelper->FetchDataFromDB(sSql, vSeriesgroup))
    {
        TPS_LOG_DEV_ERROR << "GetAll Seriesgroups failed.";
        return DB_CODES_EXCEPTION;
    }
    iRet = DB_CODES_SUCCESS;
    return iRet;
}


int RtDatabaseWrapper::GetAllAccessorys(std::vector<RtAccessory*>& vAccessory)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR << "DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vAccessory.clear();
    const std::string sSql =SELECT_TABLE_ACCESSORY;
    if (!m_pHelper->FetchDataFromDB(sSql, vAccessory)){
        TPS_LOG_DEV_ERROR << "GetAllAccessorys failed.";
        return DB_CODES_EXCEPTION;
    }
    iRet = DB_CODES_SUCCESS;
    return iRet;
}

int RtDatabaseWrapper::GetAllMeasuredprofile(std::vector<RtMeasuredprofile*>& vMeasuredprofile)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR << "DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vMeasuredprofile.clear();
    const std::string sSql = SELECT_TABLE_MEASUREDPROFILE;

    if (!m_pHelper->FetchDataFromDB(sSql, vMeasuredprofile)) {
        TPS_LOG_DEV_ERROR << ("GetAllMeasuredprofile failed.");
        return DB_CODES_EXCEPTION;
    }

    iRet = DB_CODES_SUCCESS;
    return iRet;
}

int RtDatabaseWrapper::GetAllCommissionedunit_trays(std::vector<RtCommissionedunit_tray*>& vCommissionedunit_tray)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR << "DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vCommissionedunit_tray.clear();
    const std::string sSql = SELECT_TABLE_COMMISSIONEDUNIT_TRAY;

    if (!m_pHelper->FetchDataFromDB(sSql, vCommissionedunit_tray)) {
        TPS_LOG_DEV_ERROR << ("GetAllCommissionedunit_trays failed.");
        return DB_CODES_EXCEPTION;
    }

    iRet = DB_CODES_SUCCESS;
    return iRet;
}

int RtDatabaseWrapper::GetAllAccessorychunk(std::vector<RtAccessorychunk*>& vAccessorychunk)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR << "DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vAccessorychunk.clear();
    const std::string sSql = SELECT_TABLE_ACCESSORYCHUNK;

    if (!m_pHelper->FetchDataFromDB(sSql, vAccessorychunk)) {
        TPS_LOG_DEV_ERROR << ("GetAllAccessorychunk failed.");
        return DB_CODES_EXCEPTION;
    }

    iRet = DB_CODES_SUCCESS;
    return iRet;
}
int RtDatabaseWrapper::GetAllWedgefactors(std::vector<RtWedgefactor*>& vWedgefactor)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR << "DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vWedgefactor.clear();
    const std::string sSql = SELECT_TABLE_WEDGEFACTOR;

    if (!m_pHelper->FetchDataFromDB(sSql, vWedgefactor)) {
        TPS_LOG_DEV_ERROR << ("GetAllWedgefactors failed.");
        return DB_CODES_EXCEPTION;
    }

    iRet = DB_CODES_SUCCESS;
    return iRet;
}


int RtDatabaseWrapper::GetAllTrays(std::vector<RtTray*>& vTray)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vTray.clear();
    const std::string sSql = SELECT_TABLE_TRAY;
    if(!m_pHelper->FetchDataFromDB(sSql, vTray)){
        TPS_LOG_DEV_ERROR<<"GetAllTrays failed.";
        return DB_CODES_EXCEPTION;
    }
    iRet = DB_CODES_SUCCESS;
    return iRet;
}

int RtDatabaseWrapper::GetAllMachines(std::vector<RtMachine*>& vRtMachineList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
  //  int iRet = DB_CODES_SUCCESS;

    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtMachineList.clear();//clear

    //按更新时间排序
    std::string sSql = SELECT_TABLE_MACHINE \
         " ORDER BY UPDATETIME";

    //std::vector<RtMachine*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vRtMachineList)){
        TPS_LOG_DEV_ERROR<<("GetAllPatientObjects failed.");
        return DB_CODES_EXCEPTION;
    }

    //for (auto itr=vList.begin(); itr!=vList.end(); ++itr){
    //    // read leaf boundary value from file
    //    if(DB_CODES_SUCCESS != ReadOutLeafBoundaryFromFile(*itr)){
    //        TPS_LOG_DEV_ERROR<<"ReadOutLeafBoundaryFromFile failed:"<<(*itr)->get_uid();
    //        DEL_PTR(*itr);
    //        continue;
    //    }

    //    vRtMachineList.push_back(*itr);
    //}

    return DB_CODES_SUCCESS;
}

/////////////////////////////////////////////////////////////////
///  \brief             Get all Ct2density  
///  \param[in]    
///     NOTE:   If failed, the output will be empty.
///             new inside, should be delete outside!!!
///  \param[out]    std::vector<RtCt2density*>& vRtCt2densityList
///  \return            int
///         DB_CODES_SUCCESS-- success
///         DB_CODES_MEMORY_ALLOCATION_FAILURE-- DBWrapper has not been initialized
///         DB_CODES_EXCEPTION--Exception
///         DB_CODES_UNKNOWN-- catch(...)
///  \pre \e  
/////////////////////////////////////////////////////////////////
int RtDatabaseWrapper::GetAllCt2densityObjects(std::vector<RtCt2density*>& vRtCt2densityList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtCt2densityList.clear();//clear

    const std::string sSql = SELECT_TABLE_CT2DENSITY;

    if(!m_pHelper->FetchDataFromDB(sSql, vRtCt2densityList)){
        TPS_LOG_DEV_ERROR<<("GetAllCt2densityObjects failed");
        return DB_CODES_EXCEPTION;
    }

    // read data from file
    //using blob
    //for(auto itr = vRtCt2densityList.begin(); itr!=vRtCt2densityList.end(); ++itr) {
    //    if (!m_pDBUtils->BuildCt2densityMapFromFile(*itr)) {
    //        TPS_LOG_DEV_ERROR<<"Failed to get ct2densitymap";
    //        continue;
    //    }
    //}
    return DB_CODES_SUCCESS;
}


int RtDatabaseWrapper::GetCt2densityByUid(const std::string& uid, RtCt2density*& pCt2Density)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_CT2DENSITY \
        "WHERE uid = '" +uid +"'";
    std::vector<RtCt2density*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetCt2density ByUid failed";
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty()){
        return DB_CODES_UID_NOT_EXIST;
    }
    pCt2Density = vList.front();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetSeriesListByPatientUid(const std::string& sPatientUid, std::vector<RtSeries*>& RtSeriesList)
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;

    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    RtSeriesList.clear();//clear

    const std::string sSql = SELECT_TABLE_SERIES \
        "WHERE studyuid IN (SELECT uid FROM tmsstudy WHERE patientuid = '" +sPatientUid +"')";


    std::vector<RtSeries*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList))
    {
        TPS_LOG_DEV_ERROR<<("GetSeriesListByPatientUid failed with seriesuid:" + sPatientUid);
        return DB_CODES_EXCEPTION;
    }

    //get series list
    char* buffer = nullptr;
    std::size_t fileSize(0);
    for (auto itr=vList.begin(); itr!=vList.end(); ++itr)
    {
        std::vector<RtImage*> imageList;
        const std::string sUid = (*itr)->get_uid();
        int iRet = GetImageListBySeriesUid(sUid, imageList);
        if(DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR<<"GetImageListBySeriesUid failed:"<<sUid;
            DEL_PTR(*itr);
            for (auto itrImg=imageList.begin(); itrImg != imageList.end(); ++itrImg)
            {
                DEL_PTR(*itrImg);
            }
            continue;
        }

        const std::string sFilePath = (*itr)->get_seriespixeldatafilepath();
        iRet = GetFile(sFilePath, buffer, &fileSize);
        if(0 != iRet|| nullptr == buffer || fileSize < 1)
        {
            TPS_LOG_DEV_ERROR<<"GetLongblob failed:"<<sUid;
            DEL_PTR(*itr);
            for (auto itrImg=imageList.begin(); itrImg != imageList.end(); ++itrImg)
            {
                DEL_PTR(*itrImg);
            }
            continue;
        }

        (*itr)->set_imagedata(buffer, (unsigned long)fileSize);
        DEL_ARRAY(buffer);

        if(!m_pDBUtils->BuildImage3DFromSeries((*itr), imageList, (unsigned long)fileSize))
        {
            TPS_LOG_DEV_ERROR<<"BuildImage3D From Series failed:"<<sUid;
            DEL_PTR(*itr);
            for (auto itrImg=imageList.begin(); itrImg != imageList.end(); ++itrImg)
            {
                DEL_PTR(*itrImg);
            }
            continue;
        }
        for (auto itrImg=imageList.begin(); itrImg != imageList.end(); ++itrImg)
        {
            DEL_PTR(*itrImg);
        }
        //////////////////////////////////////////////////////////////////////////
        //for more information about the image set 
        iRet = GetSeriesInformation(*itr);
        if (DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR<<"Get Series Information failed:"<<sUid;
            DEL_PTR(*itr);
            continue;
        }

        RtSeriesList.push_back(*itr);
    }

    TEST_PERFORMANCE_END("BE RtDatabaseWrapper::GetSeriesListByPatientUid");
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetImagetransformationListBySeriesUid(const std::string& sSeriesUid, 
    std::vector<RtImagetransformation*>& vList) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vList.clear();//clear

    const std::string sSql = SELECT_TABLE_IMAGETRANSFORMATION \
        "WHERE originalseriesuid = '" +sSeriesUid +"'";

    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetImagetransformationListBySeriesUid failed with SeriesUid:"<<sSeriesUid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetImagetransformation(const std::string& sFixedUid, const std::string& sFloatUid, 
    RtImagetransformation*& pImageTrans) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sSql = SELECT_TABLE_IMAGETRANSFORMATION \
        "WHERE originalseriesuid = '" +sFixedUid +"' AND transfomationseriesuid = '" + sFloatUid +"'";

    std::vector<RtImagetransformation*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetImagetransformation failed with sFixedUid:"<<sFixedUid <<" sFloatUid:"<< sFloatUid;
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty()){
        return DB_CODES_UID_NOT_EXIST;
    }
    pImageTrans = vList.front();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetCommissionedunit_trayListByUnitUid(const std::string& sUnitUid, std::vector<RtCommissionedunit_tray*>& vList) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vList.clear();//clear

    const std::string sSql = SELECT_TABLE_COMMISSIONEDUNIT_TRAY \
        "WHERE commissionedunituid = '" +sUnitUid +"'";

    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetCommissionedunit_trayListByUnitUid failed with UnitUid:"<<sUnitUid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetCommissionedunit_tray(const std::string& sUnitUid, const std::string& sTrayUid, 
    RtCommissionedunit_tray*& pUnitTray) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sSql = SELECT_TABLE_COMMISSIONEDUNIT_TRAY \
        "WHERE commissionedunituid = '" +sUnitUid +"' AND trayuid = '" + sTrayUid +"'";

    std::vector<RtCommissionedunit_tray*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetCommissionedunit_tray failed with commissionedunituid:"<<sUnitUid <<" trayuid:"<< sTrayUid;
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty()){
        return DB_CODES_UID_NOT_EXIST;
    }
    pUnitTray = vList.front();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateImagetransformation(RtImagetransformation& imageTrans) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized.");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    //const std::string sUid(imageTrans.get_uid());
    //if(!m_pHelper->QueryDBInTable(sUid, TABLE_IMAGETRANSFORMATION)) {
    //    TPS_LOG_DEV_ERROR <<"DB_CODES_UID_NOT_EXIST:" << sUid;
    //    return DB_CODES_UID_NOT_EXIST;
    //}
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    imageTrans.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(imageTrans, false)){
        TPS_LOG_DEV_ERROR<<"UpdateImagetransformation failed ";
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper UpdateImagetransformation success.");
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdatePoiDose(RtBeam_poi& beam_poi) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized.");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    beam_poi.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(beam_poi, false)){
        TPS_LOG_DEV_ERROR<<"UpdatePoiDose failed ";
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper UpdatePoiDose success.");
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetPoiDoseListByPoiUid(const std::string& sPoiUid, std::vector<RtBeam_poi*>& vList) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vList.clear();//clear

    const std::string sSql = SELECT_TABLE_BEAM_POI \
        "WHERE poiuid = '" +sPoiUid +"'";

    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<("GetPoiDoseListByPoiUid failed with PoiUid:" + sPoiUid);
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetPoiDoseListByBeamUid(const std::string& sBeamUid, std::vector<RtBeam_poi*>& vList) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vList.clear();//clear

    const std::string sSql = SELECT_TABLE_BEAM_POI \
        "WHERE beamuid = '" +sBeamUid +"'";

    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<("GetPoiDoseListByBeamUid failed with BeamUid:" + sBeamUid);
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetPoiDose(const std::string& poiUid, const std::string& beamUid, RtBeam_poi*& beam_poi) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_BEAM_POI \
        "WHERE poiuid = '" +poiUid +"' AND beamuid='" + beamUid + "'";
    std::vector<RtBeam_poi*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<("GetPoiDose failed with PoiUid:" + poiUid);
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty()){
        TPS_LOG_DEV_ERROR<<("GetPoiDose failed with PoiUid:" + poiUid);
        return DB_CODES_UID_NOT_EXIST;
    }
    beam_poi = vList.front();

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetPlanListByCourseUid(const std::string& sCourseUid, std::vector<RtPlan*>& vRtPlanList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtPlanList.clear();//clear

    const std::string sSql = SELECT_TABLE_PLAN \
        "WHERE courseuid = '" +sCourseUid +"'";

    if(!m_pHelper->FetchDataFromDB(sSql, vRtPlanList))
    {
        TPS_LOG_DEV_ERROR<<("GetPlanListByCourseUid failed with course uid:" + sCourseUid);
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetPlanByUid(const std::string& sPlanUid, RtPlan*& pPlan)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sSql = SELECT_TABLE_PLAN \
        "WHERE uid = '" + sPlanUid +"'";
    std::vector<RtPlan*> vRtPlanList;
    if(!m_pHelper->FetchDataFromDB(sSql, vRtPlanList))
    {
        TPS_LOG_DEV_ERROR<<"GetPlan ByUid failed with uid:" << sPlanUid;
        return DB_CODES_EXCEPTION;
    }
    if (!vRtPlanList.empty())
    {
        pPlan = vRtPlanList.front();
    }
    else 
    {
        TPS_LOG_DEV_ERROR<<"GetPlan ByUid failed with uid:" << sPlanUid;
        return DB_CODES_UID_NOT_EXIST;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetSiteListByCourseUid(const std::string& sCourseUid, std::vector<RtSite*>& vSiteList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vSiteList.clear();//clear

    const std::string sSql = SELECT_TABLE_SITE \
        "WHERE courseuid = '" +sCourseUid +"'";

    if(!m_pHelper->FetchDataFromDB(sSql, vSiteList)){
        TPS_LOG_DEV_ERROR<<"GetSiteList ByCourseUid failed with course uid:" << sCourseUid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetPlanList(const std::vector<std::string>& vPlanUidList, std::vector<RtPlan*>& vRtPlanList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    //  int iRet = DB_CODES_SUCCESS;

    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR << ("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtPlanList.clear();//clear

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_PLAN);

    std::for_each(vPlanUidList.begin(), vPlanUidList.end(),
        [&sqlGenerater](std::string planUid) {
        sqlGenerater.SetFilter("uid", planUid, OR);
    });

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    if (!m_pHelper->FetchDataFromDB(sSql, vRtPlanList)){
        TPS_LOG_DEV_ERROR << ("GetAllPatientObjects failed.");
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetPrescriptionListByCourseUid(
    const std::string& sCourseUid, 
    std::vector<RtPrescription*>& vPresriptionList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vPresriptionList.clear();
    const std::string sSql = SELECT_TABLE_PRESCRIPTION \
        "WHERE siteuid IN (SELECT uid FROM tmssite WHERE courseuid = '" +sCourseUid +"')";
    if(!m_pHelper->FetchDataFromDB(sSql, vPresriptionList)){
        TPS_LOG_DEV_ERROR<<"GetPlanList ByCourseUid failed with course uid:" << sCourseUid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetDeliverybeamgroupListByPrescriptionUid(
    const std::string& prescriptionuid, 
    std::vector<RtDeliverybeamgroup*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vList.clear();
    const std::string sSql = SELECT_TABLE_DELIVERYBEAMGROUP \
        "WHERE prescriptionuid = '" + prescriptionuid + "'";
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetDeliverybeamgroup List By PrescriptionUid failed with prescriptionuid:" << prescriptionuid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetNormgroupListByPlanUid(const std::string& sPlanUid, std::vector<RtNormgroup*>& vRtNormgroupList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtNormgroupList.clear();//clear
    const std::string sSql = SELECT_TABLE_NORMGROUP \
        "WHERE planuid = '" +sPlanUid +"'";

    if(!m_pHelper->FetchDataFromDB(sSql, vRtNormgroupList)){
        TPS_LOG_DEV_ERROR<<("GetNormgroupListByPlanUid failed with sPlanUid:" + sPlanUid);
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetPoiListBySeriesUid(const std::string& sSeriesUid, std::vector<RtPoi*>& vRtPoiList) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtPoiList.clear();
    const std::string sSql = SELECT_TABLE_TPSPOI \
        "WHERE seriesuid = '" +sSeriesUid +"'";
    if(!m_pHelper->FetchDataFromDB(sSql, vRtPoiList))
    {
        TPS_LOG_DEV_ERROR<<"GetPoiList BySeriesUid failed with plan uid:" << sSeriesUid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetDoselineListByPlanUid(const std::string& sPlanUid, std::vector<RtDoseline*>& vRtDoselineList) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);

    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtDoselineList.clear();//clear

    const std::string sSql = SELECT_TABLE_DOSELINE \
        "WHERE planuid = '" +sPlanUid +"'";

    if(!m_pHelper->FetchDataFromDB(sSql, vRtDoselineList)){
        TPS_LOG_DEV_ERROR<<("GetDoselineListByPlanUid failed with planuid:" + sPlanUid);
        return DB_CODES_EXCEPTION;
    }

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetBeamListByNormgroupUid(const std::string& sNormgroupUid, std::vector<RtBeam*>& vRtBeamList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtBeamList.clear();
    if (sNormgroupUid.empty()){
        TPS_LOG_DEV_ERROR<<"DB_CODES_INPUT_VIOLATION";
        return DB_CODES_INPUT_VIOLATION;
    }
    const std::string sSql = SELECT_TABLE_BEAM \
        "WHERE normgroupuid = '" +sNormgroupUid +"' ORDER BY beamnumber";

    if(!m_pHelper->FetchDataFromDB(sSql, vRtBeamList)){
        TPS_LOG_DEV_ERROR<<("GetBeamListByNormgroupUid failed with sNormgroupUid:" + sNormgroupUid);
        return DB_CODES_EXCEPTION;
    }

    //get segments
    for (auto itrBeam=vRtBeamList.begin(); itrBeam!=vRtBeamList.end(); ++itrBeam)
    {
        const std::string sBeamuid= (*itrBeam)->get_uid();
        //get dose grid
        RtDosegrid* pDoseFromDB = nullptr;
        int iRet = this->GetDosegridObjectByBeamUid(sBeamuid, pDoseFromDB);
        if (DB_CODES_SUCCESS == iRet)
        {
            RtDosegrid* pDose =  (*itrBeam)->get_dosegrid();
            *pDose = *pDoseFromDB;
            DEL_PTR(pDoseFromDB);
        }
        else if (DB_CODES_UID_NOT_EXIST == iRet)
        {
            TPS_LOG_DEV_INFO<<" dose gird object dose not exists with beamuid:"<<sBeamuid;
        }
        else
        {
            TPS_LOG_DEV_ERROR<<" get dose gird object failed with beamuid:"<<sBeamuid;
            return iRet;
        }

        //get beamsegments
        std::vector<RtBeamsegment*> vSegments;// = (*itrBeam)->get_beamsegments();
        iRet = this->GetBeamsegmentListByBeamUid(sBeamuid, vSegments);
        if(DB_CODES_SUCCESS != iRet)
		{
            //delete vSegments
            for (auto itr=vSegments.begin(); itr!=vSegments.end(); ++itr) DEL_PTR(*itr);
            vSegments.clear();
            return iRet;
        }
        (*itrBeam)->set_beamsegments(vSegments);
		for (auto itr=vSegments.begin(); itr!=vSegments.end(); ++itr) DEL_PTR(*itr);

        //get blocks
        std::vector<RtBlock*> vBlockList;
        iRet = GetBlockListByBeamuid(sBeamuid, vBlockList);
        if(DB_CODES_SUCCESS != iRet){
            TPS_LOG_DEV_ERROR<<" get block list failed with beamuid:"<<sBeamuid;
            return iRet;
        }
        for (auto itrBlock=vBlockList.begin(); itrBlock!=vBlockList.end(); ++itrBlock){
            if (RtDbDef::APERTURE == (*itrBlock)->get_blocktype()){
                (*itrBeam)->create_aperture_block();
                RtBlock* pBlock = (*itrBeam)->get_aperture_block();
                *pBlock = *(*itrBlock);
            }
            else{
                (*itrBeam)->create_shield_block();
                RtBlock* pBlock = (*itrBeam)->get_shield_block();
                *pBlock = *(*itrBlock);
            }
            DEL_PTR(*itrBlock);
        }

        ////get bolus
        //const std::string sBolusUid = (*itrBeam)->get_bolusvoiuid();
        //RtVoi* pBolus = nullptr;
        //iRet = GetVoi(sBolusUid, pBolus);
        //if (DB_CODES_SUCCESS != iRet || nullptr == pBolus){
        //    return iRet;
        //}
        //(*itrBeam)->set_bolus(pBolus);
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetDoseoptgoalbaseListByNormgroupUid(const std::string& sNormgroupUid,
    std::vector<RtDoseoptgoalbase*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vList.clear();//clear
    if (sNormgroupUid.empty()){
        TPS_LOG_DEV_ERROR<<"DB_CODES_INPUT_VIOLATION";
        return DB_CODES_INPUT_VIOLATION;
    }
    const std::string sSql = SELECT_TABLE_DOSEOPTGOALBASE \
        "WHERE normgroupuid = '" +sNormgroupUid +"'";

    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<("GetDoseoptgoalbaseListByNormgroupUid failed with sNormgroupUid:" + sNormgroupUid);
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetBeamsegmentListByBeamUid(const std::string& sBeamUid, std::vector<RtBeamsegment*>& vRtBeamsegmentList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);

    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtBeamsegmentList.clear();//clear

    std::string sSql = SELECT_TABLE_BEAMSEGMENT "WHERE beamuid = '" +sBeamUid + "'";
    sSql += " ORDER BY sequencenumber";

    if(!m_pHelper->FetchDataFromDB(sSql, vRtBeamsegmentList)){
        TPS_LOG_DEV_ERROR<<("GetBeamsegmentListByBeamUid failed with segmentuid:" + sBeamUid);
        return DB_CODES_EXCEPTION;
    }
    //get mlcshapes
    for (auto itrSeg=vRtBeamsegmentList.begin(); itrSeg!=vRtBeamsegmentList.end(); ++itrSeg){

        std::vector<RtMlcshape*> vMlcshapes;
        int iRet = this->GetMlcshapeListByBeamsegmentUid((*itrSeg)->get_uid(), vMlcshapes);
        if(DB_CODES_SUCCESS != iRet){
            //delete vRtBeamList
            for (auto itr=vRtBeamsegmentList.begin(); itr!=vRtBeamsegmentList.end(); ++itr){
                DEL_PTR(*itr);
            }
            vRtBeamsegmentList.clear();
            return iRet;
        }

        for(auto itrMlc=vMlcshapes.begin(); itrMlc!=vMlcshapes.end(); ++itrMlc) {
            if (nullptr != *itrMlc){
                if ((*itrMlc)->get_isstartmlcshape()){ 
                    RtMlcshape* pMlc = (*itrSeg)->get_startmlcshape();
                    *pMlc = *(*itrMlc);//deep copy
                }
                else{
                    RtMlcshape* pMlc = (*itrSeg)->get_endmlcshape();
                    *pMlc = *(*itrMlc);
                }
                DEL_PTR(*itrMlc);
            }
        }
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetAccessoryListByBeamUid(const std::string& sBeamUid, std::vector<std::string>& vAccessoryUid)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vAccessoryUid.clear();

    MYSQL_RES *myquery = nullptr;
    const std::string sSQL = SELECT_TABLE_BEAM_ACCESSORY \
        "WHERE beamuid = '" +sBeamUid +"'";
    if(!m_pHelper->MysqlRealQuery(sSQL)){
        TPS_LOG_DEV_ERROR<<(sSQL);
        return DB_CODES_EXCEPTION;
    }

    myquery = mysql_store_result(m_pMySql);
    if (nullptr == myquery){
        return DB_CODES_SUCCESS;
    }

    MYSQL_ROW row = nullptr;
    const unsigned int num_fields = mysql_num_fields(myquery);
    if (num_fields >0 ){
        while (nullptr != (row = mysql_fetch_row(myquery))){
            vAccessoryUid.push_back(std::string(row[1]));
        }
    }
    //mysql_free_result
    if (nullptr != myquery){
        mysql_free_result(myquery);
        myquery = nullptr;
    }

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetMlcshapeListByBeamsegmentUid(const std::string& sSegmentUid, std::vector<RtMlcshape*>& vRtMlcshapeList)
{  
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtMlcshapeList.clear();//clear

    const std::string sSql = SELECT_TABLE_MLCSHAPE \
        "WHERE segmentuid = '" +sSegmentUid +"'";
    if(!m_pHelper->FetchDataFromDB(sSql, vRtMlcshapeList)){
        TPS_LOG_DEV_ERROR<<"GetMlcshapeListByBeamsegmentUid failed with segmentuid:" << sSegmentUid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetBlockListByBeamuid(const std::string& beamuid, std::vector<RtBlock*>& vRtBlockList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtBlockList.clear();
    const std::string sSql = SELECT_TABLE_BLOCK \
        "WHERE beamuid = '" +beamuid +"'";
    if(!m_pHelper->FetchDataFromDB(sSql, vRtBlockList)){
        TPS_LOG_DEV_ERROR<<"Get BlockList By BeamUid failed with beamuid:" << beamuid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetRtImageListByPlanUid(const std::string& sPlanUid, std::vector<RtRtimage*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vList.clear();//clear

    const std::string sSql = SELECT_TABLE_RTIMAGE \
        "WHERE planuid = '" +sPlanUid +"'";
   // std::vector<RtRtimage*> tempList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList))
    {
        TPS_LOG_DEV_ERROR<<("GetRtImageListByPlanUid failed with sPlanUid:" + sPlanUid);
        return DB_CODES_EXCEPTION;
    }
    //for (auto itr=tempList.begin(); itr!=tempList.end(); ++itr)
    //{
    //    char* pBuffer = nullptr;
    //    size_t size(0);
    //    size;
    //    if(!m_pDBUtils->ReadBufferFromFile((*itr)->get_pixeldatafilepath(), pBuffer, &size))
    //    {
    //        DEL_PTR(*itr);
    //        continue;
    //    }
    //    (*itr)->set_pixel_data_buffer(pBuffer, (unsigned long)size);
    //    DEL_ARRAY(pBuffer);
    //    vList.push_back(*itr);
    //}
    return DB_CODES_SUCCESS;
}

//sBeamUid = "1.2.3";
//sConditon = "beamuid='" + sBeamUid + "'";
int RtDatabaseWrapper::GetRtImageListByCondition(const std::string& sCondition, std::vector<RtRtimage*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vList.clear();//clear

    const std::string sSql = SELECT_TABLE_RTIMAGE \
        "WHERE " + sCondition ;
    if(!m_pHelper->FetchDataFromDB(sSql, vList))
    {
        TPS_LOG_DEV_ERROR<<"GetRtImageListBy Condition failed with condition:" << sCondition;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetDrrsettingListByNormgroupUid(const std::string& sNormgroupUid, std::vector<RtDrrsetting*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vList.clear();
    if (sNormgroupUid.empty()){
        TPS_LOG_DEV_ERROR<<"DB_CODES_INPUT_VIOLATION";
        return DB_CODES_INPUT_VIOLATION;
    }
    const std::string sSql = SELECT_TABLE_DRRSETTING \
        "WHERE normalgroupuid = '" +sNormgroupUid +"'";
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetDrrsettingList ByNormgroupUid failed with sNormgroupUid:" << sNormgroupUid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetRtImageByUid(const std::string& sUid, RtRtimage*& pRtImage)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sSql = SELECT_TABLE_RTIMAGE \
        "WHERE uid = '" +sUid +"'";
    std::vector<RtRtimage*> tempList;
    if(!m_pHelper->FetchDataFromDB(sSql, tempList)){
        TPS_LOG_DEV_ERROR<<"GetRtImageByUid failed with sUid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    if (tempList.empty()){
        return DB_CODES_UID_NOT_EXIST;
    }
    pRtImage = tempList.front();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetRtImageListByBeamUid(const std::string& sBeamUid, std::vector<RtRtimage*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vList.clear();//clear

    const std::string sSql = SELECT_TABLE_RTIMAGE \
        "WHERE beamuid = '" +sBeamUid +"'";
    if(!m_pHelper->FetchDataFromDB(sSql, vList))
    {
        TPS_LOG_DEV_ERROR<<("GetRtImageListByBeamUid failed with sBeamUid:" + sBeamUid);
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

 int RtDatabaseWrapper::GetContourByUid(const std::string& sContourUid, RtContour*& pContour)
 {
     boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
     if (nullptr == m_pMySql){
         TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
         return DB_CODES_MEMORY_ALLOCATION_FAILURE;
     }
     const std::string sSql = SELECT_TABLE_CONTOUR \
         "WHERE uid = '" +sContourUid +"'";
     std::vector<RtContour*> tempList;
     if(!m_pHelper->FetchDataFromDB(sSql, tempList)){
         TPS_LOG_DEV_ERROR<<"GetContour ByUid failed with sUid:" <<sContourUid;
         return DB_CODES_EXCEPTION;
     }
     if (tempList.empty()){
         return DB_CODES_UID_NOT_EXIST;
     }
     pContour = tempList.front();
     return DB_CODES_SUCCESS;
 }

 int RtDatabaseWrapper::GetNormgroupByUid(const std::string& uid, RtNormgroup*& pNormgroup)
 {
     boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
     if (nullptr == m_pMySql){
         TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
         return DB_CODES_MEMORY_ALLOCATION_FAILURE;
     }
     const std::string sSql = SELECT_TABLE_NORMGROUP \
         "WHERE uid = '" + uid +"'";
     std::vector<RtNormgroup*> tempList;
     if(!m_pHelper->FetchDataFromDB(sSql, tempList)){
         TPS_LOG_DEV_ERROR<<"GetNormgroup ByUid failed with sUid:" <<uid;
         return DB_CODES_EXCEPTION;
     }
     if (tempList.empty()){
         return DB_CODES_UID_NOT_EXIST;
     }
     pNormgroup = tempList.front();
     return DB_CODES_SUCCESS;
 }

int RtDatabaseWrapper::GetAccessoryByUid(const std::string& uid, RtAccessory*& pAccessory)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_ACCESSORY \
        "WHERE uid = '" + uid +"'";
    std::vector<RtAccessory*> tempList;
    if(!m_pHelper->FetchDataFromDB(sSql, tempList)){
        TPS_LOG_DEV_ERROR<<"GetAccessory ByUid failed with sUid:" <<uid;
        return DB_CODES_EXCEPTION;
    }
    if (tempList.empty()){
        return DB_CODES_UID_NOT_EXIST;
    }
    pAccessory = tempList.front();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetRelatedPatientCountByCt2DensityUid(
    const std::string& ct2DesnsityUid, std::vector<std::string>& relatedPatientNameList)
{
    int iRet = 0;
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    MYSQL_RES *myquery = nullptr;
    const std::string sSQL = "select lastname,firstname from tmspatient where "
        "uid in (select patientuid from tmsstudy where uid in (select studyuid from tmsseries where ct2densityuid='"
        +ct2DesnsityUid +"'))";
    if(!m_pHelper->MysqlRealQuery(sSQL)){
        TPS_LOG_DEV_ERROR<<sSQL;
        return DB_CODES_EXCEPTION;
    }

    myquery = mysql_store_result(m_pMySql);
    if (nullptr == myquery){
        return DB_CODES_SUCCESS;
    }

    MYSQL_ROW row = nullptr;
    const unsigned int num_fields = mysql_num_fields(myquery);
    if (num_fields >0 )
    {
        while (nullptr != (row = mysql_fetch_row(myquery)))
        {
            relatedPatientNameList.push_back(std::string(nullptr == row[0] ? "" : row[0]) + std::string(nullptr == row[1] ? "" : row[1]));
        }
    }
    //mysql_free_result
    if (nullptr != myquery){
        mysql_free_result(myquery);
        myquery = nullptr;
    }

    return iRet;
}


std::string RtDatabaseWrapper::GetOneItemBySql(const std::string& sSql) const
{
    std::string relatedPatientUid="";
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql || sSql.empty() )
    {
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized or sSql is empty.";
        return relatedPatientUid;
    }

    MYSQL_RES *myquery = nullptr;
    //mysql> select uid from tmspatient where uid in (select patientuid from tmscourse where uid in (select courseuid from tms
    //    plan where uid in (select planuid from tmsnormgroup where uid in (select normgroupuid from tmsbeam where uid in (select
    //    beamuid from tmsdosegrid where uid='1.2.156.112605.14038004228065.20170222071234.50.14892.3')))));
    if(!m_pHelper->MysqlRealQuery(sSql))
    {
        TPS_LOG_DEV_ERROR<<sSql;
        return relatedPatientUid;
    }

    myquery = mysql_store_result(m_pMySql);
    if (nullptr == myquery)
    {
        return relatedPatientUid;
    }

    MYSQL_ROW row = nullptr;
    const unsigned int num_fields = mysql_num_fields(myquery);
    if (num_fields >0 )
    {
        if(nullptr != (row = mysql_fetch_row(myquery)))
        {
            relatedPatientUid = std::string(nullptr == row[0] ? "" : row[0]);
        }
    }
    //mysql_free_result
    if (nullptr != myquery)
    {
        mysql_free_result(myquery);
        myquery = nullptr;
    }

    return relatedPatientUid;
}

int RtDatabaseWrapper::BuildDosegridFromFile(RtDosegrid* pDosegrid)
{
    if (nullptr == pDosegrid)
    {
        TPS_LOG_DEV_ERROR<<"pDosegrid is null.";
        return DB_CODES_INPUT_VIOLATION;
    }

    const int iLen = pDosegrid->get_xcount() * pDosegrid->get_ycount() * pDosegrid->get_zcount();
    if (iLen < 1)
    {
        TPS_LOG_DEV_ERROR<<"wrong dosegrid size";
        return DB_CODES_INPUT_VIOLATION;
    }

    char* buffer = nullptr;
    std::size_t lBufferLen(0);

    const std::string sFilePath = pDosegrid->get_dosegridfilepath();
    int iRet = GetFile(sFilePath, buffer, &lBufferLen);
    if (0 != iRet)
    {
        DEL_ARRAY(buffer);
        TPS_LOG_DEV_ERROR<<"get file buffer failed with uid:"<<pDosegrid->get_uid();
        return DB_CODES_EXCEPTION;
    }

    const double dAccuracy = pDosegrid->get_accuracy();
    const int iBitsAllocated = pDosegrid->get_bitsallocated();
    if (16 != iBitsAllocated && 32 != iBitsAllocated)
    {
        DEL_ARRAY(buffer);
        TPS_LOG_DEV_ERROR<<"Invalid dosegrid as BitsAllocated is not 16 or 32";
        return DB_CODES_INPUT_VIOLATION;
    }

    float* pBuffer = pDosegrid->create_dosegrid_buffer();
    //memcpy((void*)pBuffer, buffer, lBufferLen);

    if (16 == iBitsAllocated)
    {
        uint32_t* pTmp = reinterpret_cast<uint32_t*>(buffer);
        size_t iElemSize = lBufferLen/4;
        if (iElemSize>0 && iElemSize<= iLen)
        {
            float fValue = 0.0f;
            for (int i = 0;i< iElemSize;++i, ++pTmp)
            {
                fValue = static_cast<float>(*pTmp * dAccuracy);
                pBuffer[i] = fValue;
            }
        }
    }
    else if(32 == iBitsAllocated)
    {
        int* pTmp = reinterpret_cast<int*>(buffer);
        size_t iElemSize = lBufferLen/sizeof(int);
        if (iElemSize>0 && iElemSize<= iLen)
        {
            float fValue = 0.0f;
            for (int i = 0;i< iElemSize;++i,++pTmp)
            {
                fValue = static_cast<float>(*pTmp * dAccuracy);
                pBuffer[i] = fValue;
            }
        }
    }

    DEL_ARRAY(buffer);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetImageListBySeriesUid(const std::string& seriesuid, std::vector<RtImage*>& RtImageList, bool bWithDcm)
{
    //boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    RtImageList.clear();//clear

    const std::string sSql = SELECT_TABLE_IMAGE \
        "WHERE seriesuid = '" +seriesuid +"' ORDER BY instancenumber";

    if(!m_pHelper->FetchDataFromDB(sSql, RtImageList))
    {
        TPS_LOG_DEV_ERROR<<("GetImageListBySeriesUid failed with segmentuid:" + seriesuid);
        return DB_CODES_EXCEPTION;
    }
    // for performance reason disable this
    if (bWithDcm)
    { 
        char* buffer = nullptr;
        std::size_t fileSize(0);
        std::string sFilePath;
        for (auto itr=RtImageList.begin(); itr!=RtImageList.end(); ++itr)
        {
            const std::string sFilePath = (*itr)->get_dcmfilepath();
            int iRet = GetFile(sFilePath, buffer, &fileSize);

            if(0 != iRet|| nullptr == buffer || fileSize < 1)
            {
                TPS_LOG_DEV_ERROR<<"Get dcm file failed:"<<(*itr)->get_dcmfilepath();
                continue;
            }

            (*itr)->set_dcmfiledata(buffer, (unsigned long)fileSize);
            DEL_ARRAY(buffer);

        }
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetContourListByVoiUid(const std::string& sVoiUid, std::vector<RtContour*>& RtContourList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    RtContourList.clear();//clear

    //select voiuid,name from tmsvoi where structuresetuid in 
    //(select uid from tmsstructureset where seriesuid='1.2.156.112605.124191480579577.20150720053938.50.2716.19') ;
    const std::string sSql = SELECT_TABLE_CONTOUR \
        "WHERE voiuid = '" +sVoiUid +"' ORDER BY sliceindex" ; //order by

    if(!m_pHelper->FetchDataFromDB(sSql, RtContourList)){
        TPS_LOG_DEV_ERROR<<("GetContourListByVoiUid failed with voiuid:" + sVoiUid);
        return DB_CODES_EXCEPTION;
    }

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetContourListByVoiList(const std::vector<RtVoi*>& VoiList, std::vector<RtContour*>& RtContourList)
{
    RtContourList.clear();//clear
    if (VoiList.empty()) return DB_CODES_SUCCESS;

    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    std::string voiuidlist("");
    int iCount = 0;
    for (auto itr=VoiList.cbegin(); itr!=VoiList.cend(); ++itr)
    {
        if (0 == iCount)
        {
            voiuidlist += "voiuid='" + (*itr)->get_uid() + "'";
        } 
        else{
            voiuidlist += " OR voiuid='" + (*itr)->get_uid() +"'";
        }
        ++iCount;
    }

    const std::string sSql = SELECT_TABLE_CONTOUR \
        "WHERE " + voiuidlist +" ORDER BY voiuid, sliceindex" ; //order by

    if(!m_pHelper->FetchDataFromDB(sSql, RtContourList)){
        TPS_LOG_DEV_ERROR<<("GetContourListByVoiUid failed.");
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetVoiList(const std::vector<std::string>& vStructureSetUids, std::vector<RtVoi*>& RtVoiList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    //  int iRet = DB_CODES_SUCCESS;

    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR << ("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    RtVoiList.clear();//clear

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_STRUCTURESET);
    std::for_each(vStructureSetUids.begin(), vStructureSetUids.end(),
        [&sqlGenerater](std::string uid) {
            sqlGenerater.SetFilter("uid", uid, OR);
    });

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }
    std::vector<RtStructureset*> strctSetList;
    if (!m_pHelper->FetchDataFromDB(sSql, strctSetList)){
        TPS_LOG_DEV_ERROR << ("FetchDataFromDB failed.");
        return DB_CODES_EXCEPTION;
    }
    if (vStructureSetUids.size() != strctSetList.size())
    {
        TPS_LOG_DEV_ERROR << "The structure set size is uncorrect.";
        return DB_CODES_EXCEPTION;
    }

    sqlGenerater.Clear();
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_VOI);
    std::for_each(vStructureSetUids.begin(), vStructureSetUids.end(),
        [&sqlGenerater](std::string voiUid) {
        sqlGenerater.SetFilter("structuresetuid", voiUid, OR);
    });

    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    if (!m_pHelper->FetchDataFromDB(sSql, RtVoiList)){
        TPS_LOG_DEV_ERROR << ("FetchDataFromDB failed.");
        return DB_CODES_EXCEPTION;
    }

    for (int i = 0; i < RtVoiList.size(); ++i){
        if(DB_CODES_SUCCESS != FillInContoursIntoVoi_i(RtVoiList[i])){
            TPS_LOG_DEV_ERROR<<"FillInContoursIntoVoi failed:"<<RtVoiList[i]->get_uid();
            DEL_PTR(RtVoiList[i]);
            return DB_CODES_EXCEPTION;
        }
    }

    for (int i = 0; i < RtVoiList.size(); ++i)
    {
        for (int j = 0; j < strctSetList.size(); ++j)
        {
            if (RtVoiList[i]->get_structuresetuid() == strctSetList[j]->get_uid())
            {
                RtVoiList[i]->set_seriesuid(strctSetList[j]->get_seriesuid());
            }
        }
    }
    return DB_CODES_SUCCESS;
}

int  RtDatabaseWrapper::GetVoiListBySeriesUid(const std::string& sSeriesUid, std::vector<RtVoi*>& RtVoiList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    RtVoiList.clear();//clear

    //select voiuid,name from tmsvoi where structuresetuid in 
    //(select uid from tmsstructureset where seriesuid='1.2.156.112605.124191480579577.20150720053938.50.2716.19') ;
    const std::string sSql = SELECT_TABLE_VOI \
        "WHERE structuresetuid IN (SELECT uid FROM tmsstructureset WHERE seriesuid = '" +sSeriesUid +"')";

    if(!m_pHelper->FetchDataFromDB(sSql, RtVoiList)){
        TPS_LOG_DEV_ERROR<<("GetVoiListBySeriesUid failed with uid:" + sSeriesUid);
        return DB_CODES_EXCEPTION;
    }
    if (RtVoiList.empty()) return DB_CODES_SUCCESS;

    //////////////////////////////////////////////////////////////////////////
    // rcc:下面的查询语句，可以优化
    // fill in the contours into each voi object
    std::vector<RtContour*> vecAllOrderedContours;
    if (DB_CODES_SUCCESS != GetContourListByVoiList(RtVoiList, vecAllOrderedContours))
    {
        TPS_LOG_DEV_ERROR<<("GetContourListByVoiList failed!");
        return DB_CODES_EXCEPTION;
    }

    std::map<std::string, std::vector<RtContour*>> voiContourMap;
    std::string voiUid;
    RtContour* contour = nullptr;
    for (int i = 0; i < vecAllOrderedContours.size(); ++i)
    {
        contour = vecAllOrderedContours[i];
        if (voiUid != contour->get_voiuid())
        {
            voiUid = contour->get_voiuid();
            voiContourMap[voiUid] = std::vector<RtContour*>();
        }
        voiContourMap[voiUid].push_back(contour);
    }
    
    std::map<std::string, std::vector<RtContour*>>::iterator it;
    for (int i = 0; i < RtVoiList.size(); ++i)
    {
        it = voiContourMap.find(RtVoiList[i]->get_uid());
        if (it != voiContourMap.end())
        {
            RtVoiList[i]->set_contours(it->second);
        }
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetStructuresetListBySeriesUid(const std::string& seriesuid, std::vector<RtStructureset*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vList.clear();
    const std::string sSql = SELECT_TABLE_STRUCTURESET \
        "WHERE seriesuid = '" + seriesuid + "'";
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetStructuresetList BySeriesUid failed with uid:" << seriesuid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetVoi(const std::string& sVoiUid, RtVoi*& pVoi)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_VOI \
        "WHERE uid = '" +sVoiUid +"'";

    std::vector<RtVoi*> RtVoiList;
    if(!m_pHelper->FetchDataFromDB(sSql, RtVoiList)){
        TPS_LOG_DEV_ERROR<<("GetVoi failed with uid:" + sVoiUid);
        return DB_CODES_EXCEPTION;
    }
    if (RtVoiList.empty()){
        TPS_LOG_DEV_ERROR<<"Failed to GetVoi as DB_CODES_UID_NOT_EXIST:"<<sVoiUid;
        return DB_CODES_UID_NOT_EXIST;
    }

    //////////////////////////////////////////////////////////////////////////
    // rcc:下面的查询语句，可以优化
    // fill in the contours into each voi object
    pVoi = RtVoiList.front();
    if(DB_CODES_SUCCESS != FillInContoursIntoVoi_i(pVoi)){
        TPS_LOG_DEV_ERROR<<"FillInContoursIntoVoi failed:"<<sVoiUid;
        DEL_PTR(pVoi);
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}


int RtDatabaseWrapper::GetVoiSettingList(std::vector<RtVoisetting*>& vVoiSettingList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_VOISETTING;
    if(!m_pHelper->FetchDataFromDB(sSql, vVoiSettingList)){
        TPS_LOG_DEV_ERROR<<("GetVoiSettingList failed.");
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetSeriesCountByPatientUid(const std::string& patientUid, int* count)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql || nullptr == count){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    *count = 0;
    if(!QueryDBInTable(patientUid, RtDbDef::TYPE_PATIENT)){
        return DB_CODES_FK_UID_NOT_EXIST;
    }

    const std::string sSql = "SELECT count(*) FROM tmsseries " 
        "WHERE studyuid IN (SELECT uid FROM tmsstudy WHERE patientuid = '" +patientUid +"')";
    MYSQL_RES *myquery = nullptr;
    if (!m_pHelper->MysqlRealQuery(sSql)){
        return DB_CODES_EXCEPTION;
    }

    myquery = mysql_store_result(m_pMySql);
    if (nullptr == myquery){
        return DB_CODES_SUCCESS;
    }

    MYSQL_ROW row = nullptr;
    const unsigned int num_fields = mysql_num_fields(myquery);
    if (num_fields >0 ){
        while (nullptr != (row = mysql_fetch_row(myquery))){
            *count = atoi(row[0]);
            break;
        }
    }
    //mysql_free_result
    if (nullptr != myquery){
        mysql_free_result(myquery);
        myquery = nullptr;
    }

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetVoiCountByStructuresetuid(const std::string& structuresetuid, int* count)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql || nullptr == count){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    *count = 0;
    const std::string sSql = "SELECT count(*) FROM tmsvoi " \
        "WHERE structuresetuid = '" + structuresetuid + "'";
    MYSQL_RES *myquery = nullptr;
    if (!m_pHelper->MysqlRealQuery(sSql)) return DB_CODES_EXCEPTION;

    myquery = mysql_store_result(m_pMySql);
    if (nullptr == myquery){
        TPS_LOG_DEV_ERROR<<"mysql_store_result failed.";
        return DB_CODES_SUCCESS;
    }

    MYSQL_ROW row = nullptr;
    const unsigned int num_fields = mysql_num_fields(myquery);
    if (num_fields >0 ){
        while (nullptr != (row = mysql_fetch_row(myquery))){
            *count = atoi(row[0]);
            break;
        }
    }
    //mysql_free_result
    if (nullptr != myquery){
        mysql_free_result(myquery);
        myquery = nullptr;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::FillInContoursIntoVoi_i(RtVoi* pRtVoi)
{
    if (pRtVoi==nullptr) {
        TPS_LOG_DEV_ERROR<<"pRtVoi is nullptr.";
        return DB_CODES_INPUT_VIOLATION;
    }

    std::string voiUid = pRtVoi->get_uid();
    std::vector<RtContour*> vecContours;
    // query all contours of this voi by voi uid
    if (GetContourListByVoiUid(voiUid, vecContours) != DB_CODES_SUCCESS) {
        return DB_CODES_EXCEPTION;
    }
    pRtVoi->set_contours(vecContours);

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertPatient(RtPatient& patient) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(patient.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    patient.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(patient, true)){
        TPS_LOG_DEV_ERROR<<"Insert Patient failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    //temp solution
    //need to update patientbarcode because Mysql auto increacement
    RtPatient *lastestPatient = nullptr;
    const std::string sSql = SELECT_TABLE_PATIENT \
        "WHERE uid ='" +sUid +"'";
    std::vector<RtPatient*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper::InsertPatient Failed to get patientbarcode after insert which uid: "<<sUid;
        DEL_PTRVECTOR(vList);
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty()) return DB_CODES_UID_NOT_EXIST;
    lastestPatient = vList.front();
    patient.set_patientbarcode(lastestPatient->get_patientbarcode());
    DEL_PTRVECTOR(vList);

    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper InsertPatient success with Uid:"<<sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertStudy(RtStudy& study) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(study.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    study.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(study, true)){
        TPS_LOG_DEV_ERROR<<"Insert Study failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper InsertStudy success with Uid:"<<sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertCourse(RtCourse& course) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(course.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    course.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(course, true)){
        TPS_LOG_DEV_ERROR<<"InsertCourse failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper InsertCourse success with Uid:"<<sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertStructureset(RtStructureset& structureset)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(structureset.get_uid());
    if (!CheckUID(sUid)){
        TPS_LOG_DEV_ERROR<<"DB_CODES_INPUT_VIOLATION SOPInstanceUID:" << sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    if(!m_pHelper->QueryDBInTable(structureset.get_seriesuid(), TABLE_SERIES)) {
        TPS_LOG_DEV_ERROR <<"ERROR_DB_FK_UID_NOT_EXIST:" << structureset.get_seriesuid();
        return DB_CODES_FK_UID_NOT_EXIST;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    structureset.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(structureset, true)){
        TPS_LOG_DEV_ERROR<<"Insert structureset failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert structureset success with Uid:"<<sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertVoi(RtVoi& voi)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized or pVoi is null";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUID(voi.get_uid());
    if (!CheckUID(sUID)){
        TPS_LOG_DEV_ERROR<<"DB_CODES_INPUT_VIOLATION SOPInstanceUID:" << sUID;
        return DB_CODES_INPUT_VIOLATION;
    }
    if(!m_pHelper->QueryDBInTable(voi.get_seriesuid(), TABLE_SERIES)) {
        TPS_LOG_DEV_ERROR <<"ERROR_DB_COURSE_UID_NOT_EXIST:" << voi.get_seriesuid();
        return DB_CODES_FK_UID_NOT_EXIST;
    }

    //query
    if(m_pHelper->QueryDBInTable(sUID, TABLE_VOI)) {
        TPS_LOG_DEV_ERROR<<"DB_CODES_UID_NOT_UNIQUE:"<<sUID;
        return DB_CODES_UID_NOT_UNIQUE;
    }

    BeginTrans();
    //insert tmsstructureset
    //{
    //    const std::string sInsertStructureSet = "INSERT INTO tmsstructureset (uid,seriesuid) VALUES ('"
    //        + sUID +"','" + voi.get_seriesuid() +"')";
    //    if(!m_pHelper->MysqlRealQuery(sInsertStructureSet)){
    //        RollBack();
    //        TPS_LOG_DEV_ERROR<<sInsertStructureSet;
    //        return DB_CODES_EXCEPTION;
    //    }
    //}
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    voi.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(voi, true)){
        RollBack();
        TPS_LOG_DEV_ERROR<<"InsertVoiObject failed with uid:"<<sUID;
        return DB_CODES_EXCEPTION;
    }

    int iRet = DB_CODES_UNKNOWN;
    const std::vector<RtContour*>& vContours = voi.get_contours();
    for (auto itr=vContours.begin(); itr!=vContours.end(); ++itr){
        iRet = InsertContour(*(*itr));
        if (DB_CODES_SUCCESS != iRet){
            RollBack();
            TPS_LOG_DEV_ERROR<<"InsertContourObject failed!";
            return iRet;
        }
    }

    if(!Commit()){
        return DB_CODES_EXCEPTION;
    }

    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper InsertNormgroupObject success with Uid:" + sUID);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertPoiObject(RtPoi& poi)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized.");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sUid(poi.get_uid());
    if (!CheckUID(sUid) ) {
        TPS_LOG_DEV_ERROR<<("DB_CODES_INPUT_VIOLATION SOPInstanceUID:" + sUid);
        return DB_CODES_INPUT_VIOLATION;
    }

    //if(!m_pHelper->QueryDBInTable(poi.get_planuid(), TABLE_PLAN)) {
    //    TPS_LOG_DEV_ERROR <<"ERROR_DB_PLAN_UID_NOT_EXIST:" << poi.get_planuid();
    //    return DB_CODES_FK_UID_NOT_EXIST;
    //}

    //query
    //if(m_pHelper->QueryDBInTable(sUID, TABLE_TPSPOI)) {
    //    TPS_LOG_DEV_ERROR<<"DB_CODES_UID_NOT_UNIQUE:"<<sUID;
    //    return DB_CODES_UID_NOT_UNIQUE;
    //}
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    poi.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(poi, true)){
        TPS_LOG_DEV_ERROR<<"InsertPoiObject failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }

    /*
    //save poi dose map
    std::map<std::string, float> dose_map = pPoi->get_poidosemap();
    for (auto itr=dose_map.begin(); itr!=dose_map.end(); ++itr){
        RtBeam_poi poi_dose;
        poi_dose.set_poiuid(sUid);
        poi_dose.set_beamuid((*itr).first);
        poi_dose.set_dosecontribution((*itr).second);
        bool bExist = QueryDBInTable((*itr).first, sUid, RtDbDef::TYPE_BEAM_POI);//true update
        if(!m_pHelper->InsertUpdateDBFromObject(poi_dose, !bExist)){
            TPS_LOG_DEV_ERROR<<"Failed to save poi dose map for poi uid:"<<sUid;
            return DB_CODES_EXCEPTION;
        }
    }
    */

    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper InsertPoiObject success with Uid:"<<sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertBlock(RtBlock& block)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(block.get_uid());
    if (!CheckUID(sUid) ) {
        TPS_LOG_DEV_ERROR<<"DB_CODES_INPUT_VIOLATION SOPInstanceUID:" << sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    if(!m_pHelper->QueryDBInTable(block.get_beamuid(), TABLE_BEAM)) {
        TPS_LOG_DEV_ERROR <<"ERROR_DB_FK_UID_NOT_EXIST:" << block.get_beamuid();
        return DB_CODES_FK_UID_NOT_EXIST;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    block.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(block, true)){
        TPS_LOG_DEV_ERROR<<"Insert block failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert block success with Uid:"<<sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateBlock(RtBlock& block)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(block.get_uid());
    if (!block.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }

    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    block.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(block, false)){
        TPS_LOG_DEV_ERROR<<"update block failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert block success with Uid:"<<sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::SaveFile(
    enDataType datatype, const std::string& sUid, const char* filebuffer, size_t filesize,
    std::string* filepath)
{
    int iRet = DB_CODES_SUCCESS;
    if (nullptr == filebuffer || filesize < 1)
    {
        iRet = DB_CODES_INPUT_VIOLATION;
        return iRet;
    }
    if (m_IsLocalhost)
    {
        //get current date
        const boost::posix_time::ptime ptimeLocalTimeInstance = boost::posix_time::microsec_clock::local_time();
        const boost::gregorian::date dtDateInstance = ptimeLocalTimeInstance.date();
        std::string sDate = boost::gregorian::to_iso_string(dtDateInstance);    // DA(Date) format: YYYYMMDD  
        std::string sFolder = m_sImagesFolder + "/" +sDate;
        switch (datatype)
        {
        case DT_DOSEGRID:
            {
                sFolder += "/dosegrid";
            }
            break;
        case DT_IMAGE:
            {
                sFolder += "/image";
            }
            break;
        case DT_SERIES:
            { 
                sFolder += "/series";
            }
            break;
        default:
            TPS_LOG_DEV_ERROR<<"wrong datatype:"<<datatype;
            return DB_CODES_INPUT_VIOLATION;
        }

        if(!m_pDBUtils->CreateMultiFolder(sFolder))
        {
            TPS_LOG_DEV_ERROR << "RtDatabaseWrapper::Upload File CreateMultiFolder failed with uid:" << sUid;
            return DB_CODES_EXCEPTION;
        }


        std::string sFilePath = sFolder + "/" + sUid;
        bool bRet = m_pDBUtils->WriteFileInBinary(sFilePath, filebuffer, filesize);
        if (!bRet)
        {
            TPS_LOG_DEV_ERROR << "RtDatabaseWrapper::Upload File WriteFileInBinary failed with uid:" << sUid;
            return DB_CODES_EXCEPTION;
        }
        *filepath = sFilePath;
    } 
    else
    {
        if (m_bIsTcp)
        {
            protocol pro;
            pro.method = TCP_POST;
            pro.filename = sUid;
            const std::string sDataToServer(filebuffer, filesize);
            iRet = m_pClientTcp->SendRequest(pro, sDataToServer, *filepath);
        } 
        else
        {    
            iRet = m_pClient->UploadFile(filebuffer, filesize, filepath);
        }
    }
    return iRet;
}

int RtDatabaseWrapper::GetFile(const std::string& path, char*& filebuffer, size_t* filesize)
{
    int iRet = DB_CODES_SUCCESS;
    if (path.empty())
    {
        iRet = DB_CODES_INPUT_VIOLATION;
        return iRet;
    }
    if (m_IsLocalhost)
    {
        if(!m_pDBUtils->ReadBufferFromFile(path, filebuffer, filesize))
        {
            iRet = DB_CODES_EXCEPTION;
            TPS_LOG_DEV_ERROR<<"ReadBufferFromFile failed.";
        }
    } 
    else
    {
        if (m_bIsTcp)
        {
            protocol pro;
            pro.method = TCP_GET;
            std::string sDataFromServer;
            iRet = m_pClientTcp->SendRequest(pro, path, sDataFromServer);
            *filesize = sDataFromServer.size();
            filebuffer = new char[*filesize + 1];
            memcpy(filebuffer, sDataFromServer.c_str(), *filesize + 1);
        } 
        else
        {    
            iRet = m_pClient->DownloadFile(path, filebuffer, filesize);
        }
    }
    return iRet;
}

int RtDatabaseWrapper::InsertSeriesObject(RtSeries& series) 
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized or series is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sUid(series.get_uid());
    if (!CheckUID(sUid) ) 
    {
        TPS_LOG_DEV_ERROR<<("DB_CODES_INPUT_VIOLATION SOPInstanceUID:" + sUid);
        return DB_CODES_INPUT_VIOLATION;
    }

    std::string sSeriesFilePath(""); 
    unsigned long ulSize(0);
    char* pData = series.get_imagedata(&ulSize);

    int iRet = SaveFile(DT_SERIES, sUid, pData, ulSize, &sSeriesFilePath);
    if (0 != iRet)
    {
        TPS_LOG_DEV_ERROR<<"Failed to upload file for series uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }

    series.set_seriespixeldatafilepath(sSeriesFilePath);

    //BeginTrans();
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    series.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(series, true))
    {
        TPS_LOG_DEV_ERROR<<"InsertPoiObject failed with uid:"<<sUid;
       // RollBack();
        return DB_CODES_EXCEPTION;
    }
    //temp solution
    //need to update seriesname because Mysql trigger create it automatically
    RtSeries *lastestSeries = nullptr;
    const std::string sSql = SELECT_TABLE_SERIES \
        "WHERE uid ='" +sUid +"'";
    std::vector<RtSeries*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper::InsertSeriesObject Failed to get series after insert which uid: "<<sUid;
        DEL_PTRVECTOR(vList);
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty()) return DB_CODES_UID_NOT_EXIST;
    lastestSeries = vList.front();
    series.set_seriesname(lastestSeries->get_seriesname());
    DEL_PTRVECTOR(vList);

   // Commit();
    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper InsertSeriesObject success with Uid:" + sUid);
    TEST_PERFORMANCE_END("RtDatabaseWrapper::InsertSeriesObject");
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertImageObject(RtImage& image)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized or series is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sUid(image.get_uid());
    if (!CheckUID(sUid) ) 
    {
        TPS_LOG_DEV_ERROR<<("DB_CODES_INPUT_VIOLATION SOPInstanceUID:" + sUid);
        return DB_CODES_INPUT_VIOLATION;
    }

    std::string sImageFilePath("");

    const std::string sDcmUid = image.get_uid();
    if (!sDcmUid.empty())
    {
        unsigned long ulSize(0);
        char* pData = image.get_dcmfiledata(&ulSize);
        if (nullptr == pData || ulSize < 1)
        {
            TPS_LOG_DEV_WARNING<<"No dcm file data.";
        }
        else
        {
            bool bRet = false;
            if (m_IsLocalhost)
            {
                const std::string sImage= m_sImagesFolder + "/image";
                if(!m_pDBUtils->CreateMultiFolder(sImage))
                {
                    TPS_LOG_DEV_ERROR << "RtDatabaseWrapper::InsertImageObject CreateMultiFolder failed with uid:" << sUid;
                    return DB_CODES_EXCEPTION;
                }

                sImageFilePath = sImage + "/" + sDcmUid + ".DCM";
                bRet = m_pDBUtils->WriteFileInBinary(sImageFilePath, pData, ulSize);
                if (!bRet)
                {
                    TPS_LOG_DEV_ERROR << "Insert WriteFileInBinary failed with uid:" << sDcmUid;
                    return DB_CODES_EXCEPTION;
                }
            }
            else
            {
                int iRet = SaveFile(DT_IMAGE, sUid, pData, ulSize, &sImageFilePath);
                if (0 != iRet)
                {
                    TPS_LOG_DEV_ERROR << "RtDatabaseWrapper::InsertImage Object Upload File failed with uid:" << sUid;
                    return DB_CODES_EXCEPTION;

                }
            }
        }
    }
    image.set_dcmfilepath(sImageFilePath);

    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    image.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(image, true))
    {
        TPS_LOG_DEV_ERROR<<"InsertPoiObject failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertContour(RtContour& contour) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized or pContour is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sUID(contour.get_uid());
    if (!CheckUID(sUID) ) {
        TPS_LOG_DEV_ERROR<<("DB_CODES_INPUT_VIOLATION:" + sUID);
        return DB_CODES_INPUT_VIOLATION;
    }

    //query
    //if(m_pHelper->QueryDBInTable(sUID, TABLE_CONTOUR)) {
    //    TPS_LOG_DEV_ERROR<<"DB_CODES_UID_NOT_UNIQUE:"<<sUID;
    //    return DB_CODES_UID_NOT_UNIQUE;
    //}
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    contour.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(contour, true)){
        TPS_LOG_DEV_ERROR<<"Insert Contour failed with uid:"<<sUID;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper InsertContourObject success with Uid:" + sUID);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertDoselineObject(RtDoseline* pDoseline)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql || nullptr == pDoseline) {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized or pVoi is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sUID(pDoseline->get_uid());
    if (!CheckUID(sUID) ) {
        TPS_LOG_DEV_ERROR<<("DB_CODES_INPUT_VIOLATION SOPInstanceUID:" + sUID);
        return DB_CODES_INPUT_VIOLATION;
    }

    if(!m_pHelper->QueryDBInTable(pDoseline->get_planuid(), TABLE_PLAN)) {
        TPS_LOG_DEV_ERROR <<"ERROR_DB_COURSE_UID_NOT_EXIST:" << pDoseline->get_planuid();
        return DB_CODES_FK_UID_NOT_EXIST;
    }

    //query
    if(m_pHelper->QueryDBInTable(sUID, TABLE_DOSELINE)) {
        TPS_LOG_DEV_ERROR<<"DB_CODES_UID_NOT_UNIQUE:"<<sUID;
        return DB_CODES_UID_NOT_UNIQUE;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    pDoseline->set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(*pDoseline, true)){
        TPS_LOG_DEV_ERROR<<"Insert DoselineObject failed with uid:"<<sUID;
        return DB_CODES_EXCEPTION;
    }

    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper InsertNormgroupObject success with Uid:" + sUID);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertPlanObject(RtPlan* pPlan) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql || nullptr == pPlan) {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized or pPlan is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sUID(pPlan->get_uid());
    if (!CheckUID(sUID) ) {
        TPS_LOG_DEV_ERROR<<("DB_CODES_INPUT_VIOLATION SOPInstanceUID:" + sUID);
        return DB_CODES_INPUT_VIOLATION;
    }

    //query
    if(!m_pHelper->QueryDBInTable(pPlan->get_courseuid(), TABLE_COURSE)) {
        TPS_LOG_DEV_ERROR <<"ERROR_DB_COURSE_UID_NOT_EXIST:" << pPlan->get_courseuid();
        return DB_CODES_FK_UID_NOT_EXIST;
    }

    if(m_pHelper->QueryDBInTable(sUID, TABLE_PLAN)) {
        TPS_LOG_DEV_ERROR<<("DB_CODES_UID_NOT_UNIQUE:"   + sUID);
        return DB_CODES_UID_NOT_UNIQUE;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    pPlan->set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(*pPlan, true)){
        TPS_LOG_DEV_ERROR<<"failed to InsertPlanObject "<< sUID;
        return DB_CODES_EXCEPTION;
    }

    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper InsertPlan  success with PatientUID:" + sUID);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertNormgroupObject(RtNormgroup* pNormgroup) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);

    if (nullptr == m_pMySql || nullptr == pNormgroup) {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized or pNormgroup is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    RtDosegrid* pDose = pNormgroup->get_dosegrid();
    if (nullptr == pDose){
        TPS_LOG_DEV_ERROR<<("RtDosegrid of the Normgroup is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    } 

    const std::string sUID(pNormgroup->get_uid());
    if (!CheckUID(sUID) ) {
        TPS_LOG_DEV_ERROR<<("DB_CODES_INPUT_VIOLATION SOPInstanceUID:" + sUID);
        return DB_CODES_INPUT_VIOLATION;
    }

    //query
    if(!m_pHelper->QueryDBInTable(pNormgroup->get_planuid(), TABLE_PLAN)) {
        TPS_LOG_DEV_ERROR <<"DB_CODES_FK_UID_NOT_EXIST:" << pNormgroup->get_planuid();
        return DB_CODES_FK_UID_NOT_EXIST;
    }

    if(m_pHelper->QueryDBInTable(sUID, TABLE_NORMGROUP)) 
    {
        TPS_LOG_DEV_ERROR<<"DB_CODES_UID_NOT_UNIQUE:"<<sUID;
        return DB_CODES_UID_NOT_UNIQUE;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    pNormgroup->set_updatetime(ptimeLocalTimeInstance);
    if (pNormgroup->get_tpsequipmentuid().empty())
    {
        pNormgroup->disable_field(NORMGROUP_TPSEQUIPMENTUID);
    }
    if (pNormgroup->get_normalpointpoiuid().empty())//database design issue
    {
        pNormgroup->disable_field(NORMGROUP_NORMALPOINTPOIUID);
    }
    if(!m_pHelper->InsertUpdateDBFromObject(*pNormgroup, true))
    {
        TPS_LOG_DEV_ERROR<<"failed to InsertNormgroupObject:"<<sUID;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper InsertNormgroupObject success with Uid:" + sUID);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertBeamObject(RtBeam& beam)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized or pBeam is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sUid(beam.get_uid());
    if (!CheckUID(sUid)) 
    {
        TPS_LOG_DEV_ERROR<<"DB_CODES_INPUT_VIOLATION SOPInstanceUID:" << sUid;
        return DB_CODES_INPUT_VIOLATION;
    }

    //query
    if(!m_pHelper->QueryDBInTable(beam.get_normgroupuid(), TABLE_NORMGROUP)) {
        TPS_LOG_DEV_ERROR <<"DB_CODES_FK_UID_NOT_EXIST:" << beam.get_normgroupuid();
        return DB_CODES_FK_UID_NOT_EXIST;
    }

    if(m_pHelper->QueryDBInTable(sUid, TABLE_BEAM)) 
    {
        TPS_LOG_DEV_ERROR<<"DB_CODES_UID_NOT_UNIQUE:"<<sUid;
        return DB_CODES_UID_NOT_UNIQUE;
    }

    BeginTrans();
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    beam.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(beam, true))
    {
        RollBack();
        TPS_LOG_DEV_ERROR<<"failed to InsertBeamObject:"<<sUid;
        return DB_CODES_EXCEPTION;
    }

    int iRet = DB_CODES_UNKNOWN;
    std::vector<RtBeamsegment*> vSegments = beam.get_beamsegments();
    for (auto itr=vSegments.begin(); itr!=vSegments.end(); ++itr)
    {
        iRet = InsertBeamsegmentObject(*(*itr));
        if (DB_CODES_SUCCESS != iRet){
            RollBack();
            TPS_LOG_DEV_ERROR<<"Insert BeamsegmentObject failed!";
            return iRet;
        }
    }

    //pDose->set_normgroupuid("");
    //pDose->set_beamuid(sUID);//same as the beam's uid
    //qqzhou@20170207 saved dose grid after dose calculation to improve performance

    //insert blocks
    RtBlock* pApertureBlock = beam.get_aperture_block();
    if (nullptr != pApertureBlock){
        pApertureBlock->set_beamuid(sUid);
        iRet = InsertBlock(*pApertureBlock);
        if (DB_CODES_SUCCESS != iRet){
            RollBack();
            TPS_LOG_DEV_ERROR<<"Insert block failed!";
            return iRet;
        }
    }
    RtBlock* pShieldBlock = beam.get_shield_block();
    if (nullptr != pShieldBlock){
        pShieldBlock->set_beamuid(sUid);
        iRet = InsertBlock(*pShieldBlock);
        if (DB_CODES_SUCCESS != iRet){
            RollBack();
            TPS_LOG_DEV_ERROR<<"Insert block failed!";
            return iRet;
        }
    }

    //insert bolus
    //const std::string sBolusUid = beam.get_bolusvoiuid();
    //RtVoi* pBolus = beam.get_bolus();
    //if (!sBolusUid.empty() && nullptr != pBolus){
    //    iRet = InsertVoi(*pBolus);
    //    if (DB_CODES_SUCCESS != iRet){
    //        return iRet;
    //    }
    //}

    if(!Commit())
    {
        TPS_LOG_DEV_ERROR<<"Commit failed!";
        return DB_CODES_EXCEPTION;
    }

    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper InsertNormgroupObject success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertBeamsegmentObject(RtBeamsegment& beamsegment)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized or pBeamsegment is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    RtMlcshape* pStartMlcshape = beamsegment.get_startmlcshape();
    if (nullptr == pStartMlcshape){
        TPS_LOG_DEV_ERROR<<("pStartMlcshape of the pBeamsegment is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    } 

    RtMlcshape* pEndMlcshape = beamsegment.get_endmlcshape();
    if (nullptr == pEndMlcshape){
        TPS_LOG_DEV_ERROR<<("pEndMlcshape of the pBeamsegment is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    } 

    const std::string sUID(beamsegment.get_uid());
    if (!CheckUID(sUID) ) {
        TPS_LOG_DEV_ERROR<<("DB_CODES_INPUT_VIOLATION SOPInstanceUID:" + sUID);
        return DB_CODES_INPUT_VIOLATION;
    }

    //query
    if(!m_pHelper->QueryDBInTable(beamsegment.get_beamuid(), TABLE_BEAM)) {
        TPS_LOG_DEV_ERROR <<"DB_CODES_FK_UID_NOT_EXIST:" << beamsegment.get_beamuid();
        return DB_CODES_FK_UID_NOT_EXIST;
    }

    if(m_pHelper->QueryDBInTable(sUID, TABLE_BEAMSEGMENT)) {
        TPS_LOG_DEV_ERROR<<"DB_CODES_UID_NOT_UNIQUE:"<<sUID;
        return DB_CODES_UID_NOT_UNIQUE;
    }

    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    beamsegment.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(beamsegment, true)){
        TPS_LOG_DEV_ERROR<<"failed to Insert BeamsegmentObject:"<<sUID;
        return DB_CODES_EXCEPTION;
    }

    pStartMlcshape->set_segmentuid(sUID);
    pEndMlcshape->set_segmentuid(sUID);
    pStartMlcshape->setdirty_all_fields(true);
    int iRet = InsertMlcshapeObject(*pStartMlcshape);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR<<"Insert MlcshapeObject failed!";
        return iRet;
    }

    pEndMlcshape->setdirty_all_fields(true);
    iRet = InsertMlcshapeObject(*pEndMlcshape);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR<<"Insert MlcshapeObject failed!";
        return iRet;
    }

    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper InsertNormgroupObject success with Uid:" + sUID);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertDosegridObject(RtDosegrid* pDosegrid)
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;

    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);

    if (nullptr == m_pMySql || nullptr == pDosegrid) 
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized or pDosegrid is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sUid(pDosegrid->get_uid());
    if (!CheckUID(sUid))
    {
        TPS_LOG_DEV_ERROR<<"DB_CODES_INPUT_VIOLATION SOPInstanceUID:" << sUid;
        return DB_CODES_INPUT_VIOLATION;
    }

    //query
    const std::string sBeamUid = pDosegrid->get_beamuid();
    //const std::string sNormgroupUid = pDosegrid->get_normgroupuid();
    if (!m_pHelper->QueryDBInTable(sBeamUid, TABLE_BEAM)) 
    {
        TPS_LOG_DEV_ERROR <<"DB_CODES_FK_UID_NOT_EXIST:" << sBeamUid;
        return DB_CODES_FK_UID_NOT_EXIST;
    }

    if(m_pHelper->QueryDBInTableViaSpecifiedKey("beamuid", sBeamUid, RtDbDef::TYPE_DOSEGRID))
    {
        TPS_LOG_DEV_ERROR <<"DB_CODES_UID_NOT_UNIQUE in dose grid table with beamuid:" << sBeamUid;
        return DB_CODES_UID_NOT_UNIQUE;
    }

    //for the latest requirement save beam level only!
    pDosegrid->disable_field(DOSEGRID_PLANUID);
    pDosegrid->disable_field(DOSEGRID_NORMGROUPUID);

    if (pDosegrid->get_isdosevalid())
    {
        if (DB_CODES_SUCCESS != this->SaveDosegridBufferToFile(pDosegrid))
        {
            TPS_LOG_DEV_ERROR << "SaveDosegrid BufferToFile failed with beamuid:" << sBeamUid;
            return DB_CODES_EXCEPTION;
        }
    }
    else
    {
        pDosegrid->set_dosegridfilepath("");
    }

    if(m_pHelper->QueryDBInTable(sUid, TABLE_DOSEGRID))
    {
        TPS_LOG_DEV_ERROR<<"DB_CODES_UID_NOT_UNIQUE:"<<sUid;
        return DB_CODES_UID_NOT_UNIQUE;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    pDosegrid->set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(*pDosegrid, true))
    {
        TPS_LOG_DEV_ERROR<<"failed to Insert DosegridObject.";
        return DB_CODES_EXCEPTION;
    }

    //DATETIME_BOOST dt = m_pHelper->GetUpdatetime(sUID, RtDbDef::TYPE_DOSEGRID);
    //if (!dt.is_not_a_date_time()){
    //    pDosegrid->set_updatetime(dt);
    //}

    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert Dosegrid Object success with Uid:" << sUid;  
    TEST_PERFORMANCE_END("RtDatabaseWrapper::Insert Dosegrid Object");
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::SaveDosegridBufferToFile(RtDosegrid* pDose)
{
    if (nullptr == pDose)
    {
        TPS_LOG_DEV_ERROR << "pDose is null.";
        return DB_CODES_INPUT_VIOLATION;
    }
    //save dosegrid to file
    std::string sSeriesFilePath(""); 
    unsigned long ulSize(0);
    std::string sUid = pDose->get_uid();
    const int iLen = pDose->get_xcount()* pDose->get_ycount() * pDose->get_zcount();
    const double dAccuracy = pDose->get_accuracy();
    const float* pBuffer = pDose->get_dosegrid_buffer();
    if (iLen < 1 || pBuffer == nullptr)
    {
        TPS_LOG_DEV_ERROR<<" len < 1 or pBuffer is null.";
        return DB_CODES_INPUT_VIOLATION;
    }

    uint32_t* data_dosegridvalue = new uint32_t[iLen];
    const int iMutiple = static_cast<int>(1/dAccuracy);
    for (int x = 0;x < iLen; ++x)
    {
        data_dosegridvalue[x] = static_cast<uint32_t>(pBuffer[x] * iMutiple);
    }
    ulSize = (unsigned long)(iLen*sizeof(uint32_t));
    std::string sDosegridFilePath;
    int iRet = SaveFile(DT_DOSEGRID, sUid, (const char*)data_dosegridvalue, ulSize, &sDosegridFilePath);
    if (0 != iRet)
    {
        TPS_LOG_DEV_ERROR << "RtDatabaseWrapper::SaveDosegrid BufferToFile Save File failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }

    pDose->set_dosegridfilepath(sDosegridFilePath);

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertRtImageObject(RtRtimage& rtimage)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sUid(rtimage.get_uid());
    if (!CheckUID(sUid)) 
    {
        TPS_LOG_DEV_ERROR<<("DB_CODES_INPUT_VIOLATION SOPInstanceUID:" + sUid);
        return DB_CODES_INPUT_VIOLATION;
    }

//     std::string sFilePath = m_sImagesFolder + "/rtimage";
//     if (!m_pDBUtils->CreateMultiFolder(sFilePath)){
//         TPS_LOG_DEV_ERROR<<"Failed to create folder";
//         return DB_CODES_EXCEPTION;
//     }
//     sFilePath += "/" +sUid;

//    rtimage.set_pixeldatafilepath(sFilePath);
//     unsigned long ulSize(0);
//     char* pBuffer = rtimage.get_pixel_data_buffer(&ulSize);
//     if(!m_pDBUtils->WriteFileInBinary(sFilePath, pBuffer, ulSize)){
//         TPS_LOG_DEV_ERROR<<"DB_CODES_UID_NOT_UNIQUE:"<<sUid;
//         return DB_CODES_INPUT_VIOLATION;
//     }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    rtimage.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(rtimage, true))
    {
        TPS_LOG_DEV_ERROR<<"failed to Insert RtImageObject.";
        return DB_CODES_EXCEPTION;
    }

    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper InsertRtImageObject success with Uid:" + sUid);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertDrrsetting(RtDrrsetting& drrSetting) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(drrSetting.get_uid());
    if (!CheckUID(sUid) ) {
        TPS_LOG_DEV_ERROR<<"DB_CODES_INPUT_VIOLATION SOPInstanceUID:" << sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    drrSetting.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(drrSetting, true)){
        TPS_LOG_DEV_ERROR<<"failed to Insert Drrsetting.";
        return DB_CODES_EXCEPTION;
    }

    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert Drrsetting success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertMlcshapeObject(RtMlcshape& mlcshape)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized.");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sUID(mlcshape.get_uid());
    if (!CheckUID(sUID) ) {
        TPS_LOG_DEV_ERROR<<("DB_CODES_INPUT_VIOLATION SOPInstanceUID:" + sUID);
        return DB_CODES_INPUT_VIOLATION;
    }

    //query
    const std::string sBeamsegmentUid = mlcshape.get_segmentuid();
    if(!m_pHelper->QueryDBInTable(sBeamsegmentUid, TABLE_BEAMSEGMENT)) {
        TPS_LOG_DEV_ERROR <<"DB_CODES_FK_UID_NOT_EXIST:" << sBeamsegmentUid;
        return DB_CODES_FK_UID_NOT_EXIST;
    }

    if(m_pHelper->QueryDBInTable(sUID, TABLE_MLCSHAPE)) {
        TPS_LOG_DEV_ERROR<<"DB_CODES_UID_NOT_UNIQUE:"<<sUID;
        return DB_CODES_UID_NOT_UNIQUE;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    mlcshape.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(mlcshape, true)){
        TPS_LOG_DEV_ERROR<<"Insert MlcshapeObject failed with uid:"<<sUID;
        return DB_CODES_EXCEPTION;
    }

    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper Insert MlcshapeObject success with Uid:" + sUID);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertDoseoptgoalbaseObject(RtDoseoptgoalbase& doseoptgoalbase) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized.");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(doseoptgoalbase.get_uid());
    if (!CheckUID(sUid) ) {
        TPS_LOG_DEV_ERROR<<("DB_CODES_INPUT_VIOLATION :" + sUid);
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    doseoptgoalbase.set_updatetime(ptimeLocalTimeInstance);
    doseoptgoalbase.setdirty_field(DOSEOPTGOALBASE_ID, false);
    if(!m_pHelper->InsertUpdateDBFromObject(doseoptgoalbase, true))
    {
        TPS_LOG_DEV_ERROR<<"Insert DoseoptgoalbaseObject failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper InsertDoseoptgoalbaseObject success with Uid:" + sUid);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertImagetransformationObject(RtImagetransformation& Imagetransformation) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized.");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    //query
    const std::string sSeriesUid = Imagetransformation.get_originalseriesuid();
    if(!m_pHelper->QueryDBInTable(sSeriesUid, TABLE_SERIES)) {
        TPS_LOG_DEV_ERROR <<"DB_CODES_FK_UID_NOT_EXIST:" << sSeriesUid;
        return DB_CODES_FK_UID_NOT_EXIST;
    }
    const std::string sTransSeriesUid = Imagetransformation.get_transfomationseriesuid();
    if(!m_pHelper->QueryDBInTable(sTransSeriesUid, TABLE_SERIES)) {
        TPS_LOG_DEV_ERROR <<"DB_CODES_FK_UID_NOT_EXIST:" << sTransSeriesUid;
        return DB_CODES_FK_UID_NOT_EXIST;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    Imagetransformation.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(Imagetransformation, true)){
        TPS_LOG_DEV_ERROR<<"InsertImagetransformationObject failed with series uid:"<<sSeriesUid;
        return DB_CODES_EXCEPTION;
    }

    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper InsertImagetransformationObject success with Uid:" + sSeriesUid);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertPoiDose(RtBeam_poi& beam_poi) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized.");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    //query
    const std::string sBeamUid = beam_poi.get_beamuid();
    if(!m_pHelper->QueryDBInTable(sBeamUid, TABLE_BEAM)) {
        TPS_LOG_DEV_ERROR <<"DB_CODES_FK_UID_NOT_EXIST:" << sBeamUid;
        return DB_CODES_FK_UID_NOT_EXIST;
    }
    const std::string sPoiUid = beam_poi.get_poiuid();
    if(!m_pHelper->QueryDBInTable(sPoiUid, TABLE_TPSPOI)) {
        TPS_LOG_DEV_ERROR <<"DB_CODES_FK_UID_NOT_EXIST:" << sPoiUid;
        return DB_CODES_FK_UID_NOT_EXIST;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    beam_poi.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(beam_poi, true)){
        TPS_LOG_DEV_ERROR<<"InsertPoiDose failed with poi uid:"<<sPoiUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper InsertPoiDose success with poi uid:" + sPoiUid);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertNormgroupVoiDose(RtNorm_voi& norm_voi) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    //query
    const std::string sNgUid = norm_voi.get_normgroupuid();
    if(!m_pHelper->QueryDBInTable(sNgUid, TABLE_NORMGROUP)) 
    {
        TPS_LOG_DEV_ERROR <<"DB_CODES_FK_UID_NOT_EXIST:" << sNgUid;
        return DB_CODES_FK_UID_NOT_EXIST;
    }
    const std::string sVoiUid = norm_voi.get_voiuid();
    if(!m_pHelper->QueryDBInTable(sVoiUid, TABLE_VOI)) 
    {
        TPS_LOG_DEV_ERROR <<"DB_CODES_FK_UID_NOT_EXIST:" << sVoiUid;
        return DB_CODES_FK_UID_NOT_EXIST;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    norm_voi.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(norm_voi, true))
    {
        TPS_LOG_DEV_ERROR<<"InsertNormgroupVoiDose failed with normgroup uid:"<<sNgUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper InsertNormgroupVoiDose success with normgroup uid:" << sNgUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertPlanVoiDose(RtPlan_voi& plan_voi) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    //query
    const std::string sPlanUid = plan_voi.get_planuid();
    if(!m_pHelper->QueryDBInTable(sPlanUid, TABLE_PLAN)) 
    {
        TPS_LOG_DEV_ERROR <<"DB_CODES_FK_UID_NOT_EXIST:" << sPlanUid;
        return DB_CODES_FK_UID_NOT_EXIST;
    }
    const std::string sVoiUid = plan_voi.get_voiuid();
    if(!m_pHelper->QueryDBInTable(sVoiUid, TABLE_VOI)) 
    {
        TPS_LOG_DEV_ERROR <<"DB_CODES_FK_UID_NOT_EXIST:" << sVoiUid;
        return DB_CODES_FK_UID_NOT_EXIST;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    plan_voi.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(plan_voi, true))
    {
        TPS_LOG_DEV_ERROR<<"InsertPlanVoiDose failed with plan uid:"<<sPlanUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper InsertPlanVoiDose success with plan uid:" << sPlanUid;
    return DB_CODES_SUCCESS;
}
int RtDatabaseWrapper::InsertCt2densityObject(RtCt2density& ct2density) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized.");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sUid = ct2density.get_uid();
    //if(m_pHelper->QueryDBInTable(sSeriesUid, TABLE_CT2DENSITY)) {
    //    TPS_LOG_DEV_ERROR <<"DB_CODES_UID_NOT_UNIQUE:" << sUid;
    //    return DB_CODES_UID_NOT_UNIQUE;
    //}
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    ct2density.set_updatetime(ptimeLocalTimeInstance);

    if(!m_pHelper->InsertUpdateDBFromObject(ct2density, true)){
        TPS_LOG_DEV_ERROR<<"InsertCt2densityObject failed with uid:"<<sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<("RtDatabaseWrapper InsertCt2densityObject success with Uid:" + sUid);
    return DB_CODES_SUCCESS;
}

bool RtDatabaseWrapper::CheckSQLQuery(const std::string& sSQLQuery) const
{ 
    std::string sSQL(sSQLQuery);
    if (sSQLQuery.empty())
    {
        return false;
    }
    //try
    //{
    //    std::string sSQL(sSQLQuery);
    //    if (sSQLQuery.empty())
    //    {
    //        return false;
    //    }

    //    //lint -e864
    //    (void)std::transform(sSQLQuery.begin(), sSQLQuery.end(), sSQL.begin(),
    //        static_cast<int(*)(int)>(std::toupper));
    //    //lint +e864

    //    return (sSQL.find("DROP") != std::string::npos ||
    //        sSQL.find("DELETE") != std::string::npos ||
    //        //sSQL.find("SELECT") != std::string::npos ||
    //        sSQL.find("UPDATE") != std::string::npos ||
    //        //sSQL.find("WHERE") != std::string::npos ||
    //        sSQL.find("EXECUTE") != std::string::npos
    //        ?false:true);
    //}
    //catch (...)
    //{
    //    TPS_LOG_DEV_ERROR<<("CheckSQLQuery Error! \n");
    //    return false;
    //}
    TPS_LOG_DEV_INFO<<(sSQLQuery);
    return true;
} 

/////////////////////////////////////////////////////////////////
///  \brief        Check UID in order to insert invalid UID to DB
///                 UID should be "0"~"9" and ".", 
///                 eg. 2.16.124.113543.6003.1126150362.65359.16760.710820283
///
///  \param[in]     const std::string& sUID
///
///  \param[out]    None
///  \return            bool
///  \pre \e  
/////////////////////////////////////////////////////////////////
bool RtDatabaseWrapper::CheckUID(const std::string& sUID) const
{
    //validity check (UID length<=64, contain only number and ".")
    if (sUID.length() <= 64 && !sUID.empty()) {
        //NOT allowed ".123.456" and "123.456."
        if ('.' == sUID[0] || '.' == sUID[sUID.length()-1]) {
            return false;
        }

        for (std::size_t i(0); i<sUID.length(); i++) {
            if ( (!std::isdigit(sUID[i])) && ( '.' != sUID[i] )) {
                return false;
            }
        }
    }
    else {
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
std::string RtDatabaseWrapper::CheckValidateString(const std::string& sInput) const
{
    if (sInput.empty())
    {
        return sInput;
    }

    std::string sOutPut("");
    for (int i = 0; i < sInput.length(); ++i)
    {
        switch (sInput[i])
        {
        case '\'':
            sOutPut += "\\'";
            break;
        case '\\':
            sOutPut += "\\\\";
            break;
        default:
            sOutPut += sInput[i];
            break;
        }
    }
    return sOutPut;
}

int RtDatabaseWrapper::ReadOutLeafBoundaryFromFile(RtMachine* pMachine)
{
    if (pMachine==nullptr)
    {
        TPS_LOG_DEV_ERROR<<"pMachine==nullptr";
        return DB_CODES_INPUT_VIOLATION;
    }

    std::string leafboundsFilePath = "";//pMachine->get_leafboundariesfilepath();
    ifstream dataFile(leafboundsFilePath, std::ios::in);
    if (!dataFile) {
        TPS_LOG_DEV_ERROR<<"Failed to open file: ["<<leafboundsFilePath<<"]";
        return DB_CODES_UNKNOWN;
    }

    stringstream textStringStream;
    textStringStream<<dataFile.rdbuf();
    string textString;
    textStringStream>>textString;
    std::vector<std::string> vecBoundsString;
    (void)boost::split(vecBoundsString, textString, boost::is_any_of("\\\\"));
    std::vector<double> vecBoundsDouble;
    for (auto itStr=vecBoundsString.begin(); itStr!=vecBoundsString.end(); ++itStr) {
        if ((*itStr).empty()) {
            continue;
        }
        float bound = boost::lexical_cast<float>(*itStr);
        vecBoundsDouble.push_back(bound);
    }
    
    pMachine->set_leafboundaries_list(vecBoundsDouble);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::ReadOutDoserateFromFile(RtCommissionedunit* pUnit)
{
    if (pUnit==nullptr) {
        TPS_LOG_DEV_ERROR<<"pUnit==nullptr";
        return DB_CODES_INPUT_VIOLATION;
    }
    std::string doseRateFilePath = "";//pUnit->get_discretedoseratefilepath();
    if (doseRateFilePath.empty()) {
        return DB_CODES_SUCCESS;
    }

    ifstream dataFile(doseRateFilePath, std::ios::in);
    if (!dataFile) {
        TPS_LOG_DEV_ERROR<<"Failed to open file: ["<<doseRateFilePath<<"]";
        return DB_CODES_UNKNOWN;
    }

    stringstream textStringStream;
    textStringStream<<dataFile.rdbuf();
    string textString;
    textStringStream>>textString;
    std::vector<std::string> vecDoserateString;
    (void)boost::split(vecDoserateString, textString, boost::is_any_of(","));
    std::vector<float> vecDoserateFloat;
    for (auto itStr=vecDoserateString.begin(); itStr!=vecDoserateString.end(); ++itStr) {
        float bound = boost::lexical_cast<float>(*itStr);
        vecDoserateFloat.push_back(bound);
    }

    pUnit->set_discretedoserate(vecDoserateFloat);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::ReadOutTrayfactorFromFile(RtCommissionedunit* pUnit)
{
    if (pUnit == nullptr) {
        TPS_LOG_DEV_ERROR << "pUnit==nullptr";
        return DB_CODES_INPUT_VIOLATION;
    }

    std::string trayfactorFilePath = "";//pUnit->get_trayfactorfilepath();
    if (trayfactorFilePath.empty()) {
        return DB_CODES_SUCCESS;
    }

    ifstream dataFile(trayfactorFilePath, std::ios::in);
    if (!dataFile) {
        TPS_LOG_DEV_ERROR << "Failed to open file: [" << trayfactorFilePath << "]";
        return DB_CODES_UNKNOWN;
    }

    stringstream textStringStream;
    textStringStream << dataFile.rdbuf();
    string textString;
    textStringStream >> textString;
    std::vector<std::string> vecTrayfactorString;
    (void)boost::split(vecTrayfactorString, textString, boost::is_any_of("|"));

    std::map<std::string, float> mapTrayfactor;
    std::for_each(vecTrayfactorString.begin(), vecTrayfactorString.end(),
        [&](std::string & trayfactorString) {

        std::vector<std::string> vecTrayfactorValue;
        (void)boost::split(vecTrayfactorValue, trayfactorString, boost::is_any_of(","));
        if (vecTrayfactorValue.size() == 2) {
            std::string sTrayID = vecTrayfactorValue[0];
            float factor = boost::lexical_cast<float>(vecTrayfactorValue[1]);

            mapTrayfactor.insert(std::make_pair<std::string, float>(sTrayID, factor));
        }
    });

    pUnit->set_discrete_trayfactor(mapTrayfactor);

    return DB_CODES_SUCCESS;
}

std::string RtDatabaseWrapper::GeneraterUid() const
{       
    return RtUidGenerater::GeneraterUid();
}

int RtDatabaseWrapper::GetAllPhantomSeries(std::vector<RtSeries*>& RtPhantomList) 
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    RtPhantomList.clear();//clear

    const std::string sSql = SELECT_TABLE_SERIES \
        "WHERE isphantom = '1'";

    std::vector<RtSeries*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<("GetAllPhantomSeries.");
        return DB_CODES_EXCEPTION;
    }

    //get image list
    char* buffer = nullptr;
    std::size_t fileSize(0);
    for (auto itr=vList.begin(); itr!=vList.end(); ++itr)
    {
        std::vector<RtImage*> imageList;
        const std::string sUid = (*itr)->get_uid();
        int iRet = GetImageListBySeriesUid(sUid, imageList);
        if(DB_CODES_SUCCESS != iRet){
            TPS_LOG_DEV_ERROR<<"GetImageListBySeriesUid failed:"<<sUid;
            DEL_PTR(*itr);
            continue;
        }

        const std::string sFilePath = (*itr)->get_seriespixeldatafilepath();
        iRet = GetFile(sFilePath, buffer, &fileSize);

        if(0 != iRet || nullptr == buffer || fileSize < 1)
        {
            TPS_LOG_DEV_ERROR<<"GetLongblob failed:"<<sUid;
            DEL_PTR(*itr);
            continue;
        }
        (*itr)->set_imagedata(buffer, (unsigned long)fileSize);
        DEL_ARRAY(buffer);

        if(!m_pDBUtils->BuildImage3DFromSeries(*itr, imageList, (unsigned long)fileSize))
        {
            TPS_LOG_DEV_ERROR<<"BuildImage3D From Series failed:"<<sUid;
            DEL_PTR(*itr);
            continue;
        }

        //////////////////////////////////////////////////////////////////////////
        //for more information about the image set 
        iRet = GetSeriesInformation(*itr);
        if (DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR<<"Get Series Information failed:"<<sUid;
            DEL_PTR(*itr);
            continue;
        }

        RtPhantomList.push_back(*itr);
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetCourseListByPatientUidList(std::vector<std::string>& vPatientUidList, std::vector<RtCourse*>& vRtCourseList) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR << ("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtCourseList.clear();//clear

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_COURSE);

    for (auto itPatientUid = vPatientUidList.begin(); itPatientUid != vPatientUidList.end(); ++itPatientUid)
    {
        sqlGenerater.SetFilter("patientuid", *itPatientUid, OR);
    }

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    if (!m_pHelper->FetchDataFromDB(sSql, vRtCourseList)){
        TPS_LOG_DEV_ERROR << ("GetCourseListByPatientUidList failed.");
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

std::string RtDatabaseWrapper::GetSavePath()
{
    return this->m_sImagesFolder;
}

std::string RtDatabaseWrapper::GetTpsVersion()
{
    return this->m_sTpsVersion;
}

std::string RtDatabaseWrapper::GetDefaultEquipmentUid()
{

    return this->m_sDefaultEquipmentUid;
}

bool RtDatabaseWrapper::GetDICOMImageBySeriesUid(const std::string& seriesuid, char*& pFile, size_t& length)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return false;
    }
    // ReadBuffer From local file
    const std::string sSql =  "select dcmfilepath from tmsimage WHERE seriesuid = '" 
        + seriesuid +"' ORDER BY instancenumber LIMIT 1";
    

    std::string sFilePath="";
    if(!m_pHelper->MysqlRealQuery(sSql))
    {
        TPS_LOG_DEV_ERROR<<(sSql);
        return false;
    }

    MYSQL_RES *myquery = mysql_store_result(m_pMySql);
    if (nullptr == myquery){
        return false;
    }

    MYSQL_ROW row = nullptr;
    const unsigned int num_fields = mysql_num_fields(myquery);
    if (num_fields >0 ){
        if(nullptr != (row = mysql_fetch_row(myquery))){
            sFilePath = row[0] ? row[0] : "";
        }
    }
    //mysql_free_result
    if (nullptr != myquery)
    {
        mysql_free_result(myquery);
        myquery = nullptr;
    }

    //file 
    if (sFilePath.empty()){
        return false;
    }

    bool bRet = 0 == GetFile(sFilePath, pFile, &length);
    return bRet;
}

bool RtDatabaseWrapper::CreatePath(const std::string& sPath)
{
    return m_pDBUtils->CreateMultiFolder(sPath);
}

bool RtDatabaseWrapper::CopyFileInBinary(const std::string& sFilePathSrc, const std::string& sFilePathDst) const
{
    return m_pDBUtils->CopyFileInBinary(sFilePathSrc, sFilePathDst);
}

int RtDatabaseWrapper::GetSystemconfigList(std::vector<RtTemplate*>& vRtSystemconfigList) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_TEMPLATE);

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    if (!m_pHelper->FetchDataFromDB(sSql, vRtSystemconfigList))
    {
        TPS_LOG_DEV_ERROR << ("GetSystemconfigList failed.");
        return DB_CODES_EXCEPTION;
    }

    return DB_CODES_SUCCESS;
}

bool RtDatabaseWrapper::IsObjectApproved(const std::string& uid) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_APPROVERECORD);
    sqlGenerater.SetFilter("objectuid", uid);

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return false;
    }

    std::vector<RtApproverecord*> record;
    if (!m_pHelper->FetchDataFromDB(sSql, record))
    {
        TPS_LOG_DEV_ERROR << ("Failed to get approve record.");
        return false;
    }
    return !record.empty() && record[0]->get_isapproved();
}

int RtDatabaseWrapper::InsertApproveRecord(RtApproverecord& approveRecord) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR << "RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(approveRecord.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    approveRecord.set_updatetime(ptimeLocalTimeInstance);
    if (!m_pHelper->InsertUpdateDBFromObject(approveRecord, true))
    {
        TPS_LOG_DEV_ERROR << "Insert accessory failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO << "RtDatabaseWrapper Insert accessory success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetApproveRecordList(std::vector<RtApproverecord*>& vRtApproverecordList) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_APPROVERECORD);

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    if (!m_pHelper->FetchDataFromDB(sSql, vRtApproverecordList))
    {
        TPS_LOG_DEV_ERROR << ("GetSeriesListByCourseUid failed.");
        return DB_CODES_EXCEPTION;
    }

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetMachineList(std::vector<std::string> vMachineUidList, std::vector<RtMachine*>& vRtMachineList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    //  int iRet = DB_CODES_SUCCESS;

    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR << ("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtMachineList.clear();//clear

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_MACHINE);

    std::for_each(vMachineUidList.begin(), vMachineUidList.end(),
        [&sqlGenerater](std::string machineUid) {

        sqlGenerater.SetFilter("uid", machineUid, OR);
    });

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

  //  std::vector<RtMachine*> vList;
    if (!m_pHelper->FetchDataFromDB(sSql, vRtMachineList)){
        TPS_LOG_DEV_ERROR << ("GetAllPatientObjects failed.");
        return DB_CODES_EXCEPTION;
    }

    //for (auto itr = vList.begin(); itr != vList.end(); ++itr){
    //    // read leaf boundary value from file
    //    if (DB_CODES_SUCCESS != ReadOutLeafBoundaryFromFile(*itr)){
    //        TPS_LOG_DEV_ERROR << "ReadOutLeafBoundaryFromFile failed:" << (*itr)->get_uid();
    //        DEL_PTR(*itr);
    //        continue;
    //    }

    //    vRtMachineList.push_back(*itr);
    //}

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetMachine(const std::string& machineUid, RtMachine*& machine)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    //  int iRet = DB_CODES_SUCCESS;

    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR << ("DBWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_MACHINE);
    sqlGenerater.SetFilter("uid", machineUid);

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    std::vector<RtMachine*> vList;
    if (!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR << ("GetAllPatientObjects failed.");
        return DB_CODES_EXCEPTION;
    }

    if (vList.size() != 1){
        TPS_LOG_DEV_ERROR<<"The machine does not exist.";
        return DB_CODES_UID_NOT_EXIST;
    }
    machine = vList.front();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetCommissionedunitListByMachineUidList(
    std::vector<std::string> vMachineUidList, std::vector<RtCommissionedunit*>& vRtCommissionedunitList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;

    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR << "DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vRtCommissionedunitList.clear();//clear
    if (vMachineUidList.empty()) return DB_CODES_SUCCESS;

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_COMMISSIONEDUNIT);

    std::for_each(vMachineUidList.begin(), vMachineUidList.end(),
        [&sqlGenerater](std::string machineUid) {

        sqlGenerater.SetFilter("machineuid", machineUid, OR);
    });

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    //std::vector<RtCommissionedunit*> vList;
    if (!m_pHelper->FetchDataFromDB(sSql, vRtCommissionedunitList)){
        TPS_LOG_DEV_ERROR << ("GetAllCommissionedunitObjects failed.");
        return DB_CODES_EXCEPTION;
    }

    for (auto itr=vRtCommissionedunitList.begin(); itr!=vRtCommissionedunitList.end(); ++itr){
        // read discrete trayfactor value from Commissionedunit_tray table
        std::vector<RtCommissionedunit_tray*> vList;
        GetCommissionedunit_trayListByUnitUid((*itr)->get_uid(), vList);
        std::map<std::string, float> UnitTrayMap;
        for (auto itrMap = vList.begin();itrMap != vList.end(); ++itrMap){
            UnitTrayMap.insert(std::make_pair((*itrMap)->get_trayuid(),(*itrMap)->get_trayfactor()));
            DEL_PTR(*itrMap);
        }
        (*itr)->set_discrete_trayfactor(UnitTrayMap);
    }
    iRet = DB_CODES_SUCCESS;
    return iRet;
}

int RtDatabaseWrapper::GetMcphocommissionedunitByUnituid(const std::string& commissionedunituid, 
    RtMcphocommissionedunit*& mcphocommissionedunit)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_MCPHOCOMMISSIONEDUNIT \
        "WHERE commissionedunituid = '" +commissionedunituid +"'";
    std::vector<RtMcphocommissionedunit*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetMcphocommissionedunit By Unituid failed with commissionunituid:" << commissionedunituid;
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty()){
        return DB_CODES_UID_NOT_EXIST;
    }

    mcphocommissionedunit = vList.front();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetMeasureddataByUnituid(const std::string& commissionedunituid, 
    RtMeasureddata*& measureddata)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_MEASUREDDATA \
        "WHERE commissionedunituid = '" +commissionedunituid +"'";
    std::vector<RtMeasureddata*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetMeasureddata By Unituid failed with commissionunituid:" << commissionedunituid;
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty()){
        return DB_CODES_UID_NOT_EXIST;
    }
    measureddata = vList.front();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetMeasuredprofileListByUnituid(const std::string& commissionedunituid, 
    std::vector<RtMeasuredprofile*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_MEASUREDPROFILE \
        "WHERE commissionedunituid = '" +commissionedunituid +"'";
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetMeasureddata By Unituid failed with commissionunituid:" << commissionedunituid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetKernelweightListByUnituid(const std::string& commissionedunituid, 
    std::vector<RtKernelweight*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_KERNELWEIGHT \
        "WHERE commissionedunituid = '" + commissionedunituid +"'";
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetMeasureddata By Unituid failed with commissionunituid:" << commissionedunituid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetWedgefactorListByUnituid(const std::string& commissionedunituid, 
    std::vector<RtWedgefactor*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_WEDGEFACTOR \
        "WHERE commissionedunituid = '" + commissionedunituid +"'";
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetRtWedgefactor By Unituid failed with commissionedunituid:" << commissionedunituid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetApplicatorListByMachineuid(const std::string& machineuid, 
    std::vector<RtApplicator*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_APPLICATOR \
        "WHERE machineuid = '" + machineuid +"'";
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"Get Applicator list by machineuid failed with machineuid:" << machineuid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetAccessoryListByMachineuid(const std::string& machineuid, 
    std::vector<RtAccessory*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_ACCESSORY \
        "WHERE machineuid = '" + machineuid +"'";
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"Get Accessory list by machineuid failed with machineuid:" << machineuid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetTrayListByMachineuid(const std::string& machineuid, 
    std::vector<RtTray*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_TRAY \
        "WHERE machineuid = '" + machineuid +"'";
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"Get Tray list by machineuid failed with machineuid:" << machineuid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetAllMonocompressedphasespacedataList(
    std::vector<RtMonocompressedphasespacedata*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_MONOCOMPRESSEDPHASESPACEDATA;
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetAll MonocompressedphasespacedataList.";
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertTray(RtTray& tray) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(tray.get_uid());
    //if (!CheckUID(sUid)) {
    //    TPS_LOG_DEV_ERROR<<"DB_CODES_INPUT_VIOLATION SOPInstanceUID:" << sUid;
    //    return DB_CODES_INPUT_VIOLATION;
    //}
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    tray.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(tray, true)){
        TPS_LOG_DEV_ERROR<<"Insert tray failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert tray success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateTray(RtTray& tray) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(tray.get_uid());
    if (!tray.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    tray.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(tray, false)){
        TPS_LOG_DEV_ERROR<<"update tray failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper update tray success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertMachine(RtMachine& machine) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(machine.get_uid());
    //if (!CheckUID(sUid)) {
    //    TPS_LOG_DEV_ERROR<<"DB_CODES_INPUT_VIOLATION SOPInstanceUID:" << sUid;
    //    return DB_CODES_INPUT_VIOLATION;
    //}
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    machine.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(machine, true)){
        TPS_LOG_DEV_ERROR<<"Insert machine failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert machine success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}
int RtDatabaseWrapper::UpdateMachine(RtMachine& machine) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(machine.get_uid());
    if (!machine.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    machine.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(machine, false)){
        TPS_LOG_DEV_ERROR<<"Update machine failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Update machine success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertCommissionedunit(RtCommissionedunit& commissionedunit) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(commissionedunit.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    commissionedunit.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(commissionedunit, true)){
        TPS_LOG_DEV_ERROR<<"Insert commissionedunit failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert commissionedunit success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}
int RtDatabaseWrapper::UpdateCommissionedunit(RtCommissionedunit& commissionedunit) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(commissionedunit.get_uid());
    if (!commissionedunit.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    commissionedunit.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(commissionedunit, false)){
        TPS_LOG_DEV_ERROR<<"Update commissionedunit failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Update commissionedunit success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertAccessory(RtAccessory& accessory) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(accessory.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    accessory.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(accessory, true))
    {
        TPS_LOG_DEV_ERROR<<"Insert accessory failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert accessory success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertEquipment(const RtEquipment& equipment) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(equipment.get_uid());
    if(!m_pHelper->InsertUpdateDBFromObject(equipment, true))
    {
        TPS_LOG_DEV_ERROR<<"Insert accessory failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert accessory success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateAccessory(RtAccessory& accessory) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(accessory.get_uid());
    if (!accessory.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    accessory.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(accessory, false)){
        TPS_LOG_DEV_ERROR<<"Update accessory failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Update accessory success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertAccessorychunk(RtAccessorychunk& accessorychunk) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(accessorychunk.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    accessorychunk.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(accessorychunk, true)){
        TPS_LOG_DEV_ERROR<<"Insert accessorychunk failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert accessorychunk success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}
int RtDatabaseWrapper::UpdateAccessorychunk(RtAccessorychunk& accessorychunk) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(accessorychunk.get_uid());
    if (!accessorychunk.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    accessorychunk.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(accessorychunk, false)){
        TPS_LOG_DEV_ERROR<<"Update accessorychunk failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Update accessorychunk success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertApplicator(RtApplicator& applicator) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(applicator.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    applicator.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(applicator, true)){
        TPS_LOG_DEV_ERROR<<"Insert applicator failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert applicator success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}
int RtDatabaseWrapper::UpdateApplicator(RtApplicator& applicator) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(applicator.get_uid());
    if (!applicator.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    applicator.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(applicator, false)){
        TPS_LOG_DEV_ERROR<<"Update applicator failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Update applicator success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertCommissionedunit_tray(RtCommissionedunit_tray& commissionedunit_tray) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(commissionedunit_tray.get_commissionedunituid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    commissionedunit_tray.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(commissionedunit_tray, true)){
        TPS_LOG_DEV_ERROR<<"Insert commissionedunit_tray failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert commissionedunit_tray success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}
int RtDatabaseWrapper::UpdateCommissionedunit_tray(RtCommissionedunit_tray& commissionedunit_tray) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(commissionedunit_tray.get_commissionedunituid());
    //if (!commissionedunit_tray.is_any_dirty())
    //{
    //    TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
    //    return DB_CODES_INPUT_VIOLATION;
    //}
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    commissionedunit_tray.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(commissionedunit_tray, false)){
        TPS_LOG_DEV_ERROR<<"Update commissionedunit_tray failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Update commissionedunit_tray success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertMeasuredprofile(RtMeasuredprofile& measuredprofile) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(measuredprofile.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    measuredprofile.set_updatetime(ptimeLocalTimeInstance);

    if (measuredprofile.get_accessoryuid().empty())
    {
        measuredprofile.disable_field(MEASUREDPROFILE_ACCESSORYUID);
    }
    if (measuredprofile.get_commissionedunituid().empty())
    {
        measuredprofile.disable_field(MEASUREDPROFILE_COMMISSIONEDUNITUID);
    }
    if (measuredprofile.get_applicatoruid().empty())
    {
        measuredprofile.disable_field(MEASUREDPROFILE_APPLICATORUID);
    }

    if(!m_pHelper->InsertUpdateDBFromObject(measuredprofile, true))
    {
        TPS_LOG_DEV_ERROR<<"Insert measuredprofile failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert measuredprofile success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}
int RtDatabaseWrapper::UpdateMeasuredprofile(RtMeasuredprofile& measuredprofile) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(measuredprofile.get_uid());
    if (!measuredprofile.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    measuredprofile.set_updatetime(ptimeLocalTimeInstance);
    if (measuredprofile.get_accessoryuid().empty())
    {
        measuredprofile.disable_field(MEASUREDPROFILE_ACCESSORYUID);
    }
    if (measuredprofile.get_commissionedunituid().empty())
    {
        measuredprofile.disable_field(MEASUREDPROFILE_COMMISSIONEDUNITUID);
    }
    if (measuredprofile.get_applicatoruid().empty())
    {
        measuredprofile.disable_field(MEASUREDPROFILE_APPLICATORUID);
    }
    if(!m_pHelper->InsertUpdateDBFromObject(measuredprofile, false))
    {
        TPS_LOG_DEV_ERROR<<"Update measuredprofile failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Update measuredprofile success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertMeasureddata(RtMeasureddata& measureddata) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(measureddata.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    measureddata.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(measureddata, true)){
        TPS_LOG_DEV_ERROR<<"Insert measureddata failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert measureddata success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}
int RtDatabaseWrapper::UpdateMeasureddata(RtMeasureddata& measureddata) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(measureddata.get_uid());
    if (!measureddata.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    measureddata.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(measureddata, false)){
        TPS_LOG_DEV_ERROR<<"Update measureddata failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Update measureddata success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertWedgefactor(RtWedgefactor& wedgefactor) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(wedgefactor.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    wedgefactor.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(wedgefactor, true)){
        TPS_LOG_DEV_ERROR<<"Insert wedgefactor failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert wedgefactor success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}
int RtDatabaseWrapper::UpdateWedgefactor(RtWedgefactor& wedgefactor) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(wedgefactor.get_uid());
    if (!wedgefactor.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    wedgefactor.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(wedgefactor, false)){
        TPS_LOG_DEV_ERROR<<"Insert wedgefactor failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert wedgefactor success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertKernelweight(RtKernelweight& kernelweight) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(kernelweight.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    kernelweight.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(kernelweight, true)){
        TPS_LOG_DEV_ERROR<<"Insert kernelweight failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert kernelweight success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}
int RtDatabaseWrapper::UpdateKernelweight(RtKernelweight& kernelweight) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(kernelweight.get_uid());
    if (!kernelweight.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    kernelweight.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(kernelweight, false)){
        TPS_LOG_DEV_ERROR<<"Insert kernelweight failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert kernelweight success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertGoldenstt(RtGoldenstt& Goldenstt) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(Goldenstt.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    Goldenstt.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(Goldenstt, true)){
        TPS_LOG_DEV_ERROR<<"Insert RtGoldenstt failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert RtGoldenstt success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}
int RtDatabaseWrapper::UpdateGoldenstt(RtGoldenstt& Goldenstt) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(Goldenstt.get_uid());
    if (!Goldenstt.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    Goldenstt.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(Goldenstt, false))
    {
        TPS_LOG_DEV_ERROR<<"Insert RtGoldenstt failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert RtGoldenstt success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertMcphocommissionedunit(RtMcphocommissionedunit& mcphocommissionedunit) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(mcphocommissionedunit.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    mcphocommissionedunit.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(mcphocommissionedunit, true)){
        TPS_LOG_DEV_ERROR<<"Insert mcphocommissionedunit failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert mcphocommissionedunit success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateMcphocommissionedunit(RtMcphocommissionedunit& mcphocommissionedunit) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(mcphocommissionedunit.get_uid());
    if (!mcphocommissionedunit.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    mcphocommissionedunit.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(mcphocommissionedunit, false)){
        TPS_LOG_DEV_ERROR<<"Insert mcphocommissionedunit failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert mcphocommissionedunit success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertMcphomonoenergyweight(RtMcphomonoenergyweight& Mcphomonoenergyweight) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(Mcphomonoenergyweight.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    Mcphomonoenergyweight.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(Mcphomonoenergyweight, true)){
        TPS_LOG_DEV_ERROR<<"Insert RtMcphomonoenergyweight failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert RtMcphomonoenergyweight success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}
int RtDatabaseWrapper::UpdateMcphomonoenergyweight(RtMcphomonoenergyweight& Mcphomonoenergyweight) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(Mcphomonoenergyweight.get_uid());
    if (!Mcphomonoenergyweight.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    Mcphomonoenergyweight.set_updatetime(ptimeLocalTimeInstance);
    if(!m_pHelper->InsertUpdateDBFromObject(Mcphomonoenergyweight, false)){
        TPS_LOG_DEV_ERROR<<"Insert RtMcphomonoenergyweight failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"RtDatabaseWrapper Insert RtMcphomonoenergyweight success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetAccessorychunkListByAccUidAndUnitUid(
    const std::string& sAccUid, 
    const std::string& sUnitUid,
    std::vector<RtAccessorychunk*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vList.clear();
    const std::string sSql = SELECT_TABLE_ACCESSORYCHUNK \
        "WHERE accessoryuid = '" + sAccUid +"' AND commissionedunituid = '" + sUnitUid + "'";
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetAccessorychunkList By AccUid AndUnitUid failed with sAccUid:" <<sAccUid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetGoldensttListByAccUidAndUnitUid(
    const std::string& sAccUid, 
    const std::string& sUnitUid,
    std::vector<RtGoldenstt*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_GOLDENSTT \
        "WHERE accessoryuid = '" + sAccUid +"' AND commissionedunituid = '" + sUnitUid + "'";
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetGoldensttList By AccUid And UnitUid failed with sAccUid:" <<sAccUid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetContourByAccessorychunkuid(
    const std::string& accessorychunkuid, 
    RtContour*& pContour)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_CONTOUR \
        "WHERE accessorychunkuid = '" + accessorychunkuid + "'";
    std::vector<RtContour*> tempList;
    if(!m_pHelper->FetchDataFromDB(sSql, tempList)){
        TPS_LOG_DEV_ERROR<<"GetContour By Accessorychunkuid failed with accessorychunkuid:" <<accessorychunkuid;
        return DB_CODES_EXCEPTION;
    }
    if (tempList.empty()){
        return DB_CODES_UID_NOT_EXIST;
    }
    pContour = tempList.front();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetMcphomonoenergyweightListByMcphocommissionedunituid(
    const std::string& mcphocommissionedunituid, 
    std::vector<RtMcphomonoenergyweight*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_MCPHOMONOENERGYWEIGHT \
        "WHERE mcphocommissionedunituid = '" + mcphocommissionedunituid + "'";
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetContour By GetMcphomonoenergyweightList By Mcphocommissionedunituid failed with accessorychunkuid:" <<mcphocommissionedunituid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetAccessoryListByAccessoryUidList(std::vector<std::string> vAccessoryUidList, std::vector<RtAccessory*>& vRtAccessoryList)
{
    vRtAccessoryList.clear();
    if (vAccessoryUidList.empty()) return DB_CODES_SUCCESS;
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_ACCESSORY);

    std::for_each(vAccessoryUidList.begin(), vAccessoryUidList.end(),
        [&sqlGenerater](std::string accessoryUid) {
        sqlGenerater.SetFilter("uid", accessoryUid, OR);
    });

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql)){
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }
    vRtAccessoryList.clear();
    if (!m_pHelper->FetchDataFromDB(sSql, vRtAccessoryList)){
        TPS_LOG_DEV_ERROR << "GetSeriesListByCourseUid failed.";
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetAccessorychunkListByCommissionedunitUid(
    std::string commissionedunitUid,
    std::vector<RtAccessorychunk*>& vRtAccessorychunkList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR << "DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_ACCESSORYCHUNK);
    sqlGenerater.SetFilter("commissionedunituid", commissionedunitUid);

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    if (!m_pHelper->FetchDataFromDB(sSql, vRtAccessorychunkList))
    {
        TPS_LOG_DEV_ERROR << "GetAccessorychunkList ByCommissionedunitUidList failed.";
        return DB_CODES_EXCEPTION;
    }

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetAccessorychunkListByCommissionedunitUidList(
    std::vector<std::string> vCommissionedunitUidList, 
    std::vector<RtAccessorychunk*>& vRtAccessorychunkList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    vRtAccessorychunkList.clear();
    if (vCommissionedunitUidList.empty()) return DB_CODES_SUCCESS;

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_ACCESSORYCHUNK);

    std::for_each(vCommissionedunitUidList.begin(), vCommissionedunitUidList.end(),
        [&sqlGenerater](std::string commissionedunitUid) {

        sqlGenerater.SetFilter("commissionedunituid", commissionedunitUid, OR);
    });

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    if (!m_pHelper->FetchDataFromDB(sSql, vRtAccessorychunkList))
    {
        TPS_LOG_DEV_ERROR << "GetAccessorychunkList ByCommissionedunitUidList failed.";
        return DB_CODES_EXCEPTION;
    }

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetGoldensttListByCommissionedunitUidList(
    std::vector<std::string> vCommissionedunitUidList, 
    std::vector<RtGoldenstt*>& vList)
{
    vList.clear();
    if (vCommissionedunitUidList.empty()) return DB_CODES_SUCCESS;
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_GOLDENSTT);

    std::for_each(vCommissionedunitUidList.begin(), vCommissionedunitUidList.end(),
        [&sqlGenerater](std::string commissionedunitUid) {
            sqlGenerater.SetFilter("commissionedunituid", commissionedunitUid, OR);
    });
    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql)){
        TPS_LOG_DEV_ERROR << " GeneraterSql failed:"<<sSql;
        return DB_CODES_EXCEPTION;
    }
    vList.clear();
    if (!m_pHelper->FetchDataFromDB(sSql, vList))
    {
        TPS_LOG_DEV_ERROR << "GetGoldensttList ByCommissionedunitUidList failed.";
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetAllGoldenstt(std::vector<RtGoldenstt*>& vGoldenstt)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_SUCCESS;
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR << "DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vGoldenstt.clear();
    const std::string sSql = SELECT_TABLE_GOLDENSTT;

    if (!m_pHelper->FetchDataFromDB(sSql, vGoldenstt))
    {
        TPS_LOG_DEV_ERROR << ("GetAllGoldenstt failed.");
        return DB_CODES_EXCEPTION;
    }

    iRet = DB_CODES_SUCCESS;
    return iRet;
}

int RtDatabaseWrapper::GetAllTcp(std::vector<RtTcp*>& vTcp)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_UNKNOWN;
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR << "DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vTcp.clear();
    const std::string sSql = SELECT_TABLE_TCP;
    if (!m_pHelper->FetchDataFromDB(sSql, vTcp))
    {
        TPS_LOG_DEV_ERROR << "get all tcp failed.";
        return DB_CODES_EXCEPTION;
    }
    iRet = DB_CODES_SUCCESS;
    return iRet;
}

int RtDatabaseWrapper::GetAllNtcp(std::vector<RtNtcp*>& vNtcp)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    int iRet = DB_CODES_UNKNOWN;
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR << "DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vNtcp.clear();
    const std::string sSql = SELECT_TABLE_NTCP;
    if (!m_pHelper->FetchDataFromDB(sSql, vNtcp))
    {
        TPS_LOG_DEV_ERROR << "get all ntcp failed.";
        return DB_CODES_EXCEPTION;
    }
    iRet = DB_CODES_SUCCESS;
    return iRet;
}

//!!NOTE without image data
int RtDatabaseWrapper::UpdateSeriesObject(RtSeries& series) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR << ("RtDatabaseWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(series.get_uid());
    if (!series.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    if (!m_pHelper->QueryDBInTable(sUid, TABLE_SERIES)) 
    {
        TPS_LOG_DEV_ERROR << "DB_CODES_UID_NOT_EXIST:" << sUid;
        return DB_CODES_UID_NOT_EXIST;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    series.set_updatetime(ptimeLocalTimeInstance);
    if (!m_pHelper->InsertUpdateDBFromObject(series, false))
    {
        TPS_LOG_DEV_ERROR << "UpdateSeriesObject failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO << ("RtDatabaseWrapper UpdateSeriesObject success with Uid:" + sUid);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdatePlanObject(RtPlan& plan) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR << "RtDatabaseWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(plan.get_uid());
    if (!plan.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    if (!m_pHelper->QueryDBInTable(sUid, TABLE_PLAN)) {
        TPS_LOG_DEV_ERROR << "DB_CODES_UID_NOT_EXIST:" << sUid;
        return DB_CODES_UID_NOT_EXIST;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    plan.set_updatetime(ptimeLocalTimeInstance);
    if (!m_pHelper->InsertUpdateDBFromObject(plan, false)){
        TPS_LOG_DEV_ERROR << "UpdatePlanObject failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO << ("RtDatabaseWrapper UpdatePlanObject success with Uid:" + sUid);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateNormgroupObject(RtNormgroup& normgroup) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR << ("RtDatabaseWrapper has not been initialized or plan is null");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(normgroup.get_uid());
    if (!normgroup.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        return DB_CODES_INPUT_VIOLATION;
    }
    if (!m_pHelper->QueryDBInTable(sUid, TABLE_NORMGROUP)) 
    {
        TPS_LOG_DEV_ERROR << "DB_CODES_UID_NOT_EXIST:" << sUid;
        return DB_CODES_UID_NOT_EXIST;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    normgroup.set_updatetime(ptimeLocalTimeInstance);
    if (normgroup.get_normalpointpoiuid().empty())//database design issue
    {
        normgroup.disable_field(NORMGROUP_NORMALPOINTPOIUID);
    }
    if (normgroup.get_tpsequipmentuid().empty())
    {
        normgroup.disable_field(NORMGROUP_TPSEQUIPMENTUID);
    }
    if (!m_pHelper->InsertUpdateDBFromObject(normgroup, false))
    {
        TPS_LOG_DEV_ERROR << "UpdateNormgroupObject failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO << ("RtDatabaseWrapper UpdateNormgroupObject success with Uid:" + sUid);
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateBeamObject(RtBeam& beam)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR << "RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUid(beam.get_uid());
    if (!m_pHelper->QueryDBInTable(sUid, TABLE_BEAM))
    {
        TPS_LOG_DEV_ERROR << "DB_CODES_UID_NOT_EXIST:" << sUid;
        return DB_CODES_UID_NOT_EXIST;
    }
    if (!beam.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<sUid;
        //return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    beam.set_updatetime(ptimeLocalTimeInstance);
    if (!m_pHelper->InsertUpdateDBFromObject(beam, false))
    {
        TPS_LOG_DEV_ERROR << "UpdateBeamObject failed with uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }

    //delete old segments to be removed later as that causes memory leak for segmgents
    const std::string sCondition = "beamuid='" + sUid + "'";
    if(DB_CODES_SUCCESS != EraseObjectUnderCondition(sCondition, RtDbDef::TYPE_BEAMSEGMENT))
    {
        TPS_LOG_DEV_ERROR << "EraseObjectUnderCondition failed with beam uid:" << sUid;
        return DB_CODES_EXCEPTION;
    }

    //save beam segments
    int iRet = DB_CODES_UNKNOWN;
    std::vector<RtBeamsegment*> vSegments = beam.get_beamsegments();
    for (auto itr=vSegments.begin(); itr!=vSegments.end(); ++itr)
    {
        (*itr)->setdirty_all_fields(true);
        iRet = InsertBeamsegmentObject(*(*itr));
        if (DB_CODES_SUCCESS != iRet){
            //RollBack();
            TPS_LOG_DEV_ERROR<<"Insert BeamsegmentObject failed!";
            return iRet;
        }
    }
    if (DB_CODES_SUCCESS != iRet)
    {
        //      RollBack();
        TPS_LOG_DEV_ERROR<<"Update BeamObject failed!";
        return iRet;
    }

    //insert blocks
    RtBlock* pApertureBlock = beam.get_aperture_block();
    if (nullptr != pApertureBlock)
    {
        pApertureBlock->setdirty_all_fields(true);
        pApertureBlock->set_beamuid(sUid);
        iRet = InsertBlock(*pApertureBlock);
        if (DB_CODES_SUCCESS != iRet){
            //      RollBack();
            TPS_LOG_DEV_ERROR<<"Insert block failed!";
            return iRet;
        }
    }
    RtBlock* pShieldBlock = beam.get_shield_block();
    if (nullptr != pShieldBlock)
    {
        pShieldBlock->setdirty_all_fields(true);
        pShieldBlock->set_beamuid(sUid);
        iRet = InsertBlock(*pShieldBlock);
        if (DB_CODES_SUCCESS != iRet){
            //      RollBack();
            TPS_LOG_DEV_ERROR<<"Insert block failed!";
            return iRet;
        }
    }

    TPS_LOG_DEV_INFO << "RtDatabaseWrapper UpdateBeamObject success with Uid:" << sUid;
    return DB_CODES_SUCCESS;
}

//update beam only exclude segments and other objects
int RtDatabaseWrapper:: UpdateBeamAttributes(RtBeam& beam) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR << "RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUID(beam.get_uid());
    if (!m_pHelper->QueryDBInTable(sUID, TABLE_BEAM)) {
        TPS_LOG_DEV_ERROR << "DB_CODES_UID_NOT_EXIST:" << sUID;
        return DB_CODES_UID_NOT_EXIST;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    beam.set_updatetime(ptimeLocalTimeInstance);
    if (!m_pHelper->InsertUpdateDBFromObject(beam, false)){
        TPS_LOG_DEV_ERROR << "UpdateBeam Attributes failed with uid:" << sUID;
        return DB_CODES_EXCEPTION;
    }

    TPS_LOG_DEV_INFO << "RtDatabaseWrapper UpdateBeam Attributes success with Uid:" << sUID;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateBeamSegments(std::vector<RtBeamsegment*> beamSegments) const
{
    for (auto it = beamSegments.begin(); it != beamSegments.end(); ++it)
    {
        if (!m_pHelper->QueryDBInTable((*it)->get_uid(), TABLE_BEAMSEGMENT))
        {
            TPS_LOG_DEV_ERROR << "DB_CODES_UID_NOT_EXIST:" << (*it)->get_uid();
            return DB_CODES_UID_NOT_EXIST;
        }

        if (!(*it)->is_any_dirty())
        {
            TPS_LOG_DEV_INFO << "input violation as no any dirty filed:"<<(*it)->get_uid();
            return DB_CODES_SUCCESS;
        }

        if (!m_pHelper->InsertUpdateDBFromObject(**it, false))
        {
            TPS_LOG_DEV_ERROR << "UpdateSegment Attributes failed with uid:" << (*it)->get_uid();
            return DB_CODES_EXCEPTION;
        }
    }

    TPS_LOG_DEV_INFO << "RtDatabaseWrapper UpdateBeamSegments success";
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::EraseDoselineByPlanUid(const std::string planUid)
{
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR << ("RtDatabaseWrapper has not been initialized");
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    //delete record
    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_DELETE);
    sqlGenerater.SetTable(TABLE_DOSELINE);
    sqlGenerater.SetFilter("planuid", planUid);

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (!m_pHelper->MysqlRealQuery(sSql)){
        TPS_LOG_DEV_ERROR << sSql;
        return DB_CODES_EXCEPTION;
    }

    TPS_LOG_DEV_INFO << ("EraseDoselineByPlanUid:" + planUid);

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertTemplate(RtTemplate& systemconfig)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR << "RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUID(systemconfig.get_uid());
    if (!CheckUID(sUID)) {
        TPS_LOG_DEV_ERROR << "DB_CODES_INPUT_VIOLATION SOPInstanceUID:" << sUID;
        return DB_CODES_INPUT_VIOLATION;
    }
    //query
    if (m_pHelper->QueryDBInTable(sUID, TABLE_TEMPLATE)) {
        TPS_LOG_DEV_ERROR << "DB_CODES_UID_NOT_UNIQUE:" << sUID;
        return DB_CODES_UID_NOT_UNIQUE;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    systemconfig.set_updatetime(ptimeLocalTimeInstance);
    if (!m_pHelper->InsertUpdateDBFromObject(systemconfig, true)){
        TPS_LOG_DEV_ERROR << "failed to Insert SystemconfigObject " << sUID;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO << "RtDatabaseWrapper Insert SystemconfigObject success with UID:" << sUID;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateTemplate(RtTemplate& systemconfig)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR << "RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUID(systemconfig.get_uid());
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    systemconfig.set_updatetime(ptimeLocalTimeInstance);
    if (!m_pHelper->InsertUpdateDBFromObject(systemconfig, false))
    {
        TPS_LOG_DEV_ERROR << "failed to Update Template " << sUID;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO << "RtDatabaseWrapper Update Template success with UID:" << sUID;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetConstraintListByPrescriptionUidList(std::vector<std::string> vPrescriptionUidList,
    std::vector<RtConstraint*>& vRtConstraintList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_CONSTRAINT);

    std::for_each(vPrescriptionUidList.begin(), vPrescriptionUidList.end(),
        [&sqlGenerater](std::string prescriptionUid) {

        sqlGenerater.SetFilter("prescriptionuid", prescriptionUid, OR);
    });

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    if (!m_pHelper->FetchDataFromDB(sSql, vRtConstraintList))
    {
        TPS_LOG_DEV_ERROR << ("GetSeriesListByCourseUid failed.");
        return DB_CODES_EXCEPTION;
    }

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateItems(RtDbDef::OBJECT_TYPE objType, 
    //<attribute name, value>
    const std::map<std::string, std::string>& attrToValueMap, 
    //<filter attribute name, filter value>
    const std::map<std::string, std::vector<std::string>>& filtersMap) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
 
    const std::string sTableName = std::string(RtDbDef::ENUM_TO_STRING_OBJECT_TYPE[objType]);
    std::string sSql = "UPDATE " + sTableName;
    std::string sValue = " ";
    for (auto it = attrToValueMap.begin(); it != attrToValueMap.end(); ++it)
    {
        if (it != attrToValueMap.begin())
        {
            sValue += ", ";
        }
        sValue += "set " + it->first + " = '" + it->second + "' ";
    }
    std::string sWhere = " WHERE ";
    bool hasValidFilter = false;
    for (auto it = filtersMap.begin(); it != filtersMap.end(); ++it)
    {
        if (!it->second.empty())
        {
            if (hasValidFilter)
            {
                sWhere += " AND ";
            }
            sWhere += "( ";
            for (int i = 0; i < it->second.size(); ++i)
            {
                if (i) sWhere += " OR ";
                sWhere += it->first + " = '" + it->second[i] + "' ";
            }
            sWhere += " )";
            hasValidFilter = true;
        }
    }
    sSql += sValue + sWhere;

    if(!m_pHelper->MysqlRealQuery(sSql))
    {
        TPS_LOG_DEV_ERROR<<sSql;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO<<"Update table:"<<sTableName <<" with sql:"<<sSql;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertTablecontour(RtTablecontour& tablecontour)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR << "RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sUID(tablecontour.get_uid());
    if (!CheckUID(sUID)) {
        TPS_LOG_DEV_ERROR << "DB_CODES_INPUT_VIOLATION SOPInstanceUID:" << sUID;
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    tablecontour.set_updatetime(ptimeLocalTimeInstance);
    if (!m_pHelper->InsertUpdateDBFromObject(tablecontour, true)){
        TPS_LOG_DEV_ERROR << "failed to Insert Tablecontour " << sUID;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO << "RtDatabaseWrapper Insert Tablecontour success with :" << sUID;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetTablecontourByUid(const std::string& uid, RtTablecontour*& pTablecontour)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_TABLECONTOUR \
        "WHERE uid = '" + uid + "'";
    std::vector<RtTablecontour*> tempList;
    if(!m_pHelper->FetchDataFromDB(sSql, tempList)){
        TPS_LOG_DEV_ERROR<<"GetTablecontour ByUid failed with uid:" << uid;
        return DB_CODES_EXCEPTION;
    }
    if (tempList.empty()){
        return DB_CODES_UID_NOT_EXIST;
    }
    pTablecontour = tempList.front();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetTablecontourListByTableUid(const std::string& tableuid, std::vector<RtTablecontour*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_TABLECONTOUR \
        "WHERE tableuid = '" + tableuid + "'";
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetTablecontour By TableUid failed with tableuid:" << tableuid;
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetAllTablecontourList(std::vector<RtTablecontour*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_TABLECONTOUR;
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetAll Tablecontour List.";
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertSeriestable(RtSeriestable& seriestable) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) {
        TPS_LOG_DEV_ERROR << "RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    seriestable.set_updatetime(ptimeLocalTimeInstance);
    if (!m_pHelper->InsertUpdateDBFromObject(seriestable, true))
    {
        TPS_LOG_DEV_ERROR << "Failed to insert Seriestable with seriesuid " << seriestable.get_seriesuid() ;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO << "RtDatabaseWrapper Insert Seriestable success with seriesuid:" << seriestable.get_seriesuid();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::UpdateSeriestable(RtSeriestable& seriestable) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql) 
    {
        TPS_LOG_DEV_ERROR << "RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    if (!seriestable.is_any_dirty())
    {
        TPS_LOG_DEV_ERROR << "input violation as no any dirty filed:"<<seriestable.get_seriesuid();
        return DB_CODES_INPUT_VIOLATION;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    seriestable.set_updatetime(ptimeLocalTimeInstance);
    if (!m_pHelper->InsertUpdateDBFromObject(seriestable, false))
    {
        TPS_LOG_DEV_ERROR << "Failed to update Seriestable with seriesuid " << seriestable.get_seriesuid() ;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO << "RtDatabaseWrapper update Seriestable success with seriesuid:" << seriestable.get_seriesuid();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::InsertSeriesgroup(RtSeriesgroup& seriesgroup) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR << "RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    seriesgroup.set_updatetime(ptimeLocalTimeInstance);
    const std::string sUid = seriesgroup.get_uid();
    if (!m_pHelper->InsertUpdateDBFromObject(seriesgroup, true))
    {
        TPS_LOG_DEV_ERROR << "Failed to insert seriesgroup with uid " << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO << "RtDatabaseWrapper Insert seriesgroup success with uid:" << sUid;
    return DB_CODES_SUCCESS;
}
int RtDatabaseWrapper::UpdateSeriesgroup(RtSeriesgroup& seriesgroup) const
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR << "RtDatabaseWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const boost::posix_time::ptime ptimeLocalTimeInstance= boost::posix_time::microsec_clock::local_time();
    seriesgroup.set_updatetime(ptimeLocalTimeInstance);
    const std::string sUid = seriesgroup.get_uid();
    if (!m_pHelper->InsertUpdateDBFromObject(seriesgroup, false))
    {
        TPS_LOG_DEV_ERROR << "Failed to update seriesgroup with uid " << sUid;
        return DB_CODES_EXCEPTION;
    }
    TPS_LOG_DEV_INFO << "RtDatabaseWrapper update seriesgroup success with uid:" << sUid;
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetSeriesgroupByUid(const std::string& uid, RtSeriesgroup*& pSeriesgroup)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"RtDatabaseWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }

    const std::string sSql = SELECT_TABLE_SERIESGROUP "WHERE uid ='" +uid +"'";
    std::vector<RtSeriesgroup*> vList;
    if(!m_pHelper->FetchDataFromDB(sSql, vList))
    {
        TPS_LOG_DEV_ERROR<<"GetSeriesgroup ByUid failed with uid:" << uid;
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty()) return DB_CODES_UID_NOT_EXIST;
    pSeriesgroup = vList.front();
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetAllSeriestableList(std::vector<RtSeriestable*>& vList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_SERIESTABLE;
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"GetAll Seriestable List failed.";
        return DB_CODES_EXCEPTION;
    }
    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetSeriestable(const std::string& seriesUID, RtSeriestable*& seriesTable)
{
    std::vector<RtSeriestable*> vList;
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized.";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    const std::string sSql = SELECT_TABLE_SERIESTABLE \
        "WHERE seriesuid = '" + seriesUID + "'";
    if(!m_pHelper->FetchDataFromDB(sSql, vList)){
        TPS_LOG_DEV_ERROR<<"Get RtSeriestable failed.";
        return DB_CODES_EXCEPTION;
    }
    if (vList.empty()){
        return DB_CODES_UID_NOT_EXIST;
    }
    seriesTable = vList.front();
    return DB_CODES_SUCCESS;
}


void RtDatabaseWrapper::DeleteFileByPath(const std::string sPath) const
{
    if (m_IsLocalhost)
    {
        if(m_pDBUtils->DeleteLocalFile(sPath))
        {
            TPS_LOG_DEV_INFO << "Local file was deleted:" << sPath;
        }
        else
        {
           TPS_LOG_DEV_ERROR << "Failed to delete local file :" << sPath;
        }
    }
    else//TODO
    {
        //delete file on server
        if (m_bIsTcp)
        {
            protocol pro;
            pro.method = TCP_DELETE;
            std::string sOut;
            (void) m_pClientTcp->SendRequest(pro, sPath, sOut);
        } 
        else
        {
            //TODO
        }
        
    }
}

RtDatabaseHelper& RtDatabaseWrapper::GetDatabaseHelper()
{
	return *m_pHelper;
}

#pragma region [New Load]
int RtDatabaseWrapper::GetPartialSeriesListByPatientUid(const std::string& sPatientUid, std::vector<RtSeries*>& vRtSeriesList)
{
    if (sPatientUid.empty())
    {
        return DB_CODES_SUCCESS;
    }

    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);

    vRtSeriesList.clear();

    const std::string sSql = "SELECT uid, seriesname, isqa, isphantom, patientname, ct2densityuid FROM tmsseries" \
        " WHERE studyuid IN (SELECT uid FROM tmsstudy WHERE patientuid = '" +sPatientUid +"')";

    if(!m_pHelper->MysqlRealQuery(sSql)){
        TPS_LOG_DEV_ERROR<<(sSql);
        return DB_CODES_EXCEPTION;
    }

    MYSQL_RES* myquery = mysql_store_result(m_pMySql);
    if (nullptr == myquery){
        return DB_CODES_SUCCESS;
    }

    MYSQL_ROW row = nullptr;
    const unsigned int num_fields = mysql_num_fields(myquery);
    if (num_fields >0 ){
        while (nullptr != (row = mysql_fetch_row(myquery))){
            RtSeries* pRtSeries = new RtSeries();
            if (row[0] != nullptr)
            {
                pRtSeries->set_uid(std::string(row[0]));
            }

            pRtSeries->set_patientuid(sPatientUid);

            if (row[1] != nullptr)
            {
                pRtSeries->set_seriesname(std::string(row[1]));
            }

            if (row[2] != nullptr)
            {
                pRtSeries->set_isqa(atoi(row[2]) == 1);
            }

            if (row[3] != nullptr)
            {
                pRtSeries->set_isphantom(atoi(row[3]) == 1);
            }

            if (row[4] != nullptr)
            {
                pRtSeries->set_patientname(std::string(row[4]));
            }

			if (row[5] != nullptr)
			{
				pRtSeries->set_ct2densityuid(std::string(row[5]));
			}

            vRtSeriesList.push_back(pRtSeries);
        }
    }

    //mysql_free_result
    if (nullptr != myquery){
        mysql_free_result(myquery);
        myquery = nullptr;
    }

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetPartialPlanListByCourseUidList(const std::vector<string>& vCourseUidList, std::vector<RtPlan*>& vRtPlanList)
{
    if (vCourseUidList.empty())
    {
        return DB_CODES_SUCCESS;
    }

    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);

    vRtPlanList.clear();

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_PLAN);
    sqlGenerater.SetField("uid");
    sqlGenerater.SetField("courseuid");
    sqlGenerater.SetField("imageseriesuid");
    sqlGenerater.SetField("name");
    sqlGenerater.SetField("plansourcetype");

    std::for_each(vCourseUidList.begin(), vCourseUidList.end(),
        [&sqlGenerater](std::string courseUid) {
        sqlGenerater.SetFilter("courseuid", courseUid, OR);
    });

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    if(!m_pHelper->MysqlRealQuery(sSql)){
        TPS_LOG_DEV_ERROR<<(sSql);
        return DB_CODES_EXCEPTION;
    }

    MYSQL_RES* myquery = mysql_store_result(m_pMySql);
    if (nullptr == myquery){
        return DB_CODES_SUCCESS;
    }

    MYSQL_ROW row = nullptr;
    const unsigned int num_fields = mysql_num_fields(myquery);
    if (num_fields >0 ){
        while (nullptr != (row = mysql_fetch_row(myquery))){
            RtPlan* pRtPlan = new RtPlan();
            if (row[0] != nullptr)
            {
                pRtPlan->set_uid(std::string(row[0]));
            }
            if (row[1] != nullptr)
            {            
                pRtPlan->set_courseuid(std::string(row[1]));
            }
            if (row[2] != nullptr)
            {
                pRtPlan->set_imageseriesuid(std::string(row[2]));
            }
            if (row[3] != nullptr)
            {
                pRtPlan->set_name(std::string(row[3]));
            }
            if (row[4] != nullptr)
            {
                pRtPlan->set_plansourcetype(atoi(row[4]));
            }
            vRtPlanList.push_back(pRtPlan);
        }
    }

    //mysql_free_result
    if (nullptr != myquery){
        mysql_free_result(myquery);
        myquery = nullptr;
    }

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetSiteListByCourseUidList(const std::vector<std::string>& vCourseUidList, std::vector<RtSite*>& vSiteList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vSiteList.clear();//clear

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_SITE);
    std::for_each(vCourseUidList.begin(), vCourseUidList.end(),
        [&sqlGenerater](std::string courseUid) {
            sqlGenerater.SetFilter("courseuid", courseUid, OR);
    });

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    if(!m_pHelper->FetchDataFromDB(sSql, vSiteList)){
        TPS_LOG_DEV_ERROR<<"GetSiteList ByCourseUidList failed.";
        return DB_CODES_EXCEPTION;
    }

    return DB_CODES_SUCCESS;
}

int RtDatabaseWrapper::GetPrescriptionListBySiteUidList(const std::vector<std::string>& vSiteUidList, std::vector<RtPrescription*>& vPresriptionList)
{
    boost::recursive_mutex::scoped_lock lock(*m_dbrecmutext);
    if (nullptr == m_pMySql)
    {
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return DB_CODES_MEMORY_ALLOCATION_FAILURE;
    }
    vPresriptionList.clear();//clear

    RtSqlGenerater sqlGenerater;
    sqlGenerater.SetSqlOperator(SQL_SELECT);
    sqlGenerater.SetTable(TABLE_PRESCRIPTION);
    std::for_each(vSiteUidList.begin(), vSiteUidList.end(),
        [&sqlGenerater](std::string siteUid) {
            sqlGenerater.SetFilter("siteuid", siteUid, OR);
    });

    std::string sSql;
    if (!sqlGenerater.GeneraterSql(sSql))
    {
        TPS_LOG_DEV_ERROR << " GeneraterSql failed.";
        return DB_CODES_EXCEPTION;
    }

    if(!m_pHelper->FetchDataFromDB(sSql, vPresriptionList)){
        TPS_LOG_DEV_ERROR<<"GetPrescriptionList BySiteUidList failed.";
        return DB_CODES_EXCEPTION;
    }

    return DB_CODES_SUCCESS;
}

#pragma endregion

RT_TPS_DATABASE_END_NAMESPACE;
    //////////////////////////////////////////////////////////////////////////
