//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_connect.h
///  \brief     rt tps database wrapper interface 
///
///  \version 1.0
///  \date    July 9,2015
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_CONNECT_H_
#define RT_TPS_DATABASE_CONNECT_H_

#include "tps_logger.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE

    /////////////////////////////////////////////////////////////////
    ///  \brief
    ///
    ///  \param[in]      std::string& sUser          
    ///                     std::string& sPassword      
    ///                     std::string& sDatabaseName  
    ///                     std::string& sHostName      
    ///                     unsigned int uiPort          
    ///  \param[out]   MYSQL*&
    ///  \return          bool
    ///  \pre \e  
    /////////////////////////////////////////////////////////////////
    inline bool create_database (
    std::string& sUser,
    std::string& sPassword,
    std::string& sDatabaseName,
    std::string& sHostName,
    unsigned int uiPort,
    MYSQL*& pMySQL)
{
    try
    {
        if (sUser.empty() 
            ||sPassword.empty() 
            ||sDatabaseName.empty() 
            ||sHostName.empty() 
            ||uiPort<1)
        {
            TPS_LOG_DEV_ERROR<<"create_database failed";
            return false;
        }

        //mysql connection
        pMySQL = mysql_init((MYSQL*)NULL);
        if (nullptr == pMySQL)
        {
            TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized mysql_init is null");
            return false;
        }
        //mysql_options(pMySQL,MYSQL_OPT_COMPRESS,0); enable compress may be slower than before
        
        //MYSQL_SET_CHARSET_NAME 20120717
        mysql_options(pMySQL, MYSQL_SET_CHARSET_NAME, "utf8");
        if(!mysql_real_connect(pMySQL,
            sHostName.c_str(),
            sUser.c_str(),
            sPassword.c_str(),
            sDatabaseName.c_str(),
            uiPort,
            NULL,0))
        {
            const std::string sLog = mysql_errno(pMySQL) ? std::string(mysql_error(pMySQL)) : "";
            TPS_LOG_DEV_ERROR<<("connect to DB failed! mysql_error: " +sLog);
            return false;
        }
        //The library calls this function automatically if mysql_set_local_infile_handler() has not been 
        //called or does not supply valid functions for each of its callbacks.
        mysql_set_local_infile_default(pMySQL);//todo
        TPS_LOG_DEV_INFO << ("connect to DB success!");
    }
    catch (const std::exception& e)
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized:" + std::string(e.what()));
        return false;
    }
    catch(...)
    {
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized");
        return false;
    }
    return true;
}
RT_TPS_DATABASE_END_NAMESPACE
#endif // MCSF_DATABASE_H_
