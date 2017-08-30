//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_object_helper.cpp
///
///  \version 1.0
///  \date    Oct.23 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "rt_tps_database_object_helper.h"
// third library header
#include "boost/date_time/posix_time/posix_time.hpp"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

RtDatabaseHelper::RtDatabaseHelper(MYSQL* pMySql) : m_pMySql(pMySql)
{

}

RtDatabaseHelper::~RtDatabaseHelper() 
{

}

bool RtDatabaseHelper::QueryDBInTable(const std::string& sUid, RtDbDef::OBJECT_TYPE objType) const
{
    return QueryDBInTableViaSpecifiedKey("uid", sUid, objType);
}

bool RtDatabaseHelper::QueryDBInTable(const std::string& sFirstUid, const std::string& sSecondUid, 
    RtDbDef::OBJECT_TYPE objType) const
{
    //query
    std::string sTable = RtDbDef::ENUM_TO_STRING_OBJECT_TYPE[objType];
    std::string sSQL = "";
    if (RtDbDef::TYPE_BEAM_POI == objType)
    {
        sSQL = "SELECT beamuid FROM " + sTable + " WHERE beamuid = '" +sFirstUid +"' AND poiuid='" + sSecondUid + "'";
    }
    else if (RtDbDef::TYPE_NORM_VOI == objType)
    {
        sSQL = "SELECT normgroupuid FROM " + sTable + " WHERE normgroupuid = '" +sFirstUid +"' AND voiuid='" + sSecondUid + "'";
    }
    else if (RtDbDef::TYPE_PLAN_VOI == objType)
    {
        sSQL = "SELECT planuid FROM " + sTable + " WHERE planuid = '" +sFirstUid +"' AND voiuid='" + sSecondUid + "'";
    }
    //TODO for more relationships

    if(!MysqlRealQuery(sSQL)){
        TPS_LOG_DEV_ERROR<<sSQL;
        return false;
    }

    MYSQL_RES *myquery = mysql_store_result(m_pMySql);
    if (nullptr == myquery){
        return false;
    }

    MYSQL_ROW row = nullptr;
    const unsigned int num_fields = mysql_num_fields(myquery);
    bool bFind(false);
    if (num_fields >0 ){
        if (nullptr != (row = mysql_fetch_row(myquery))){
            bFind = true;
        }
    }

    //mysql_free_result
    if (nullptr != myquery){
        mysql_free_result(myquery);
        myquery = nullptr;
    }
    return bFind;
}

bool RtDatabaseHelper::QueryDBInTable(const std::string& sUid, const std::string& sTable) const
{ 
    //query
    std::string sSQL = "SELECT uid FROM " + sTable + " WHERE uid = '" +sUid +"'";
    if(!MysqlRealQuery(sSQL)){
        TPS_LOG_DEV_ERROR<<sSQL;
        return false;
    }

    MYSQL_RES *myquery = mysql_store_result(m_pMySql);
    if (nullptr == myquery){
        return false;
    }

    MYSQL_ROW row = nullptr;
    const unsigned int num_fields = mysql_num_fields(myquery);
    bool bFind(false);
    if (num_fields >0 ){
        if (nullptr != (row = mysql_fetch_row(myquery))){
            bFind = true;
        }
    }

    //mysql_free_result
    if (nullptr != myquery){
        mysql_free_result(myquery);
        myquery = nullptr;
    }
    return bFind;
}

bool RtDatabaseHelper::QueryDBInPatient(const std::string& sPatientUID) const
{   
    return QueryDBInTable(sPatientUID, TABLE_PATIENT);
}

bool RtDatabaseHelper::QueryDBInTableViaSpecifiedKey(
    const std::string& key, const std::string& value, RtDbDef::OBJECT_TYPE objType) const
{
    //query
    std::string sTable = RtDbDef::ENUM_TO_STRING_OBJECT_TYPE[objType];
    const std::string sSQL = "SELECT " + key + " FROM " + sTable + " WHERE " + key + " = '" +value +"'";
    if(!MysqlRealQuery(sSQL))
    {
        TPS_LOG_DEV_ERROR<<sSQL;
        return false;
    }

    MYSQL_RES *myquery = mysql_store_result(m_pMySql);
    if (nullptr == myquery){
        return false;
    }

    MYSQL_ROW row = nullptr;
    const unsigned int num_fields = mysql_num_fields(myquery);
    bool bFind(false);
    if (num_fields >0 ){
        if (nullptr != (row = mysql_fetch_row(myquery))){
            bFind = true;
        }
    }

    //mysql_free_result
    if (nullptr != myquery){
        mysql_free_result(myquery);
        myquery = nullptr;
    }
    return bFind;
}

bool RtDatabaseHelper::MysqlRealQuery(const std::string& sSQL) const
{
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<("RtDatabaseWrapper has not been initialized");
        return false;
    }

    TPS_LOG_DEV_INFO << ("Executing: " + sSQL);
    if (0 != mysql_real_query(m_pMySql, sSQL.c_str(), static_cast<unsigned int>(sSQL.length()))){ //mysql_real_query() failed due to an error
        const unsigned int errorNo = mysql_errno(m_pMySql);
        std::string sLog("");
        if (errorNo>0){
            sLog = "mysql_error: " + std::to_string(long long(errorNo))+":" +
                mysql_error(m_pMySql)+" with SQL is \n"+sSQL.c_str();
            TPS_LOG_DEV_ERROR<<(sLog);
            TPS_PRINTF_DEBUG("MysqlRealQuery error:\n%s\n", sLog.c_str());
        }
        return false;
    }

    return true;
}

std::string RtDatabaseHelper::ConvertDateTime(const DATETIME_BOOST& datetime)
{
    std::string sDatetime("");
    if (datetime.is_not_a_date_time()){
        return sDatetime;
    }
    sDatetime = boost::posix_time::to_iso_extended_string(datetime);
    const std::string t("T");
    sDatetime.replace(sDatetime.find(t), t.size(), " ");
    //sDatetime = "'" + sDatetime + "'"; //for database
    return sDatetime;
}

std::string RtDatabaseHelper::ConvertDateTime(const DATE_BOOST& date)
{
    std::string sDate("");
    if (date.is_not_a_date()){
        return sDate;
    }
    //YYYYMMDDTHHMMSS
    sDate = boost::gregorian::to_iso_extended_string(date);
    //         (void)sDate.insert(4,"-");
    //         (void)sDate.insert(7,"-");
    //sDate = "'" + sDate + "'";//for database
    return sDate;
}

std::string RtDatabaseHelper::ConvertDateTime(const TIME_BOOST& time)
{
    std::string sTime("");
    if (time.is_not_a_date_time()){
        return sTime;
    }
    sTime = boost::posix_time::to_simple_string(time);
    sTime = "'" + sTime + "'";
    return sTime;
}

void RtDatabaseHelper::ConvertDateTime2DB(const DATETIME_BOOST& datetime, MYSQL_TIME* my_datetime)
{
    if (datetime.is_not_a_date_time() || nullptr == my_datetime) return;
    my_datetime->year = datetime.date().year();
    my_datetime->month = datetime.date().month();
    my_datetime->day = datetime.date().day();

    my_datetime->hour = datetime.time_of_day().hours();
    my_datetime->minute = datetime.time_of_day().minutes();
    my_datetime->second = datetime.time_of_day().seconds();
    return;
}

void RtDatabaseHelper::ConvertDateTime2DB(const DATE_BOOST& date, MYSQL_TIME* my_datetime)
{
    if (date.is_not_a_date() || nullptr == my_datetime) return;
    my_datetime->year = date.year();
    my_datetime->month = date.month();
    my_datetime->day = date.day();
}

void RtDatabaseHelper::ConvertDateTime2DB(const TIME_BOOST& time, MYSQL_TIME* my_datetime)
{
    if (time.is_not_a_date_time() || nullptr == my_datetime) return;
    //fix bug 48812
    my_datetime->neg = false;
    my_datetime->day = 0;
    my_datetime->hour = time.hours();
    my_datetime->minute = time.minutes();
    my_datetime->second = time.seconds();
    //my_datetime->time_type = MYSQL_TIMESTAMP_TIME;
}

void RtDatabaseHelper::ConvertDateTime2Boost(const MYSQL_TIME& my_datetime, DATETIME_BOOST* datetime)
{
    if (nullptr == datetime || my_datetime.year < 1000) return;
    DATETIME_BOOST p(DATE_BOOST(
        (unsigned short)my_datetime.year, 
        (unsigned short)my_datetime.month,
        (unsigned short)my_datetime.day), 
        boost::posix_time::hours(my_datetime.hour)
        + boost::posix_time::minutes(my_datetime.minute)
        + boost::posix_time::seconds(my_datetime.second)); 
    *datetime = p; 
}

void RtDatabaseHelper::ConvertDateTime2Boost(const MYSQL_TIME& my_datetime, DATE_BOOST* datetime)
{
    if (nullptr == datetime || my_datetime.year < 1000) return;
    DATE_BOOST p(DATE_BOOST(
        (unsigned short)my_datetime.year, 
        (unsigned short)my_datetime.month,
        (unsigned short)my_datetime.day)); 
    *datetime = p; 
}

void RtDatabaseHelper::ConvertDateTime2Boost(const MYSQL_TIME& my_datetime, TIME_BOOST* datetime)
{
    if (nullptr == datetime) return;
    TIME_BOOST p(boost::posix_time::hours(my_datetime.hour)
        + boost::posix_time::minutes(my_datetime.minute)
        + boost::posix_time::seconds(my_datetime.second)); 
    *datetime = p; 
}


DATETIME_BOOST RtDatabaseHelper::GetUpdatetime(const std::string& sUid, RtDbDef::OBJECT_TYPE objType)
{
    DATETIME_BOOST dt(boost::date_time::not_a_date_time);
    if (nullptr == m_pMySql){
        TPS_LOG_DEV_ERROR<<"DBWrapper has not been initialized";
        return dt;
    }

    //query
    std::string sTable = RtDbDef::ENUM_TO_STRING_OBJECT_TYPE[objType];
    const std::string sSql = "SELECT updatetime FROM " + sTable + " WHERE uid = '" +sUid +"'";
    MYSQL_RES *myquery = nullptr;
    if (!this->MysqlRealQuery(sSql)){
        TPS_LOG_DEV_ERROR<<sSql;
        return dt;
    }

    myquery = mysql_store_result(m_pMySql);
    if (nullptr == myquery){
        TPS_LOG_DEV_ERROR<<"mysql_store_result error.";
        return dt;
    }

    MYSQL_ROW row = nullptr;
    const unsigned int num_fields = mysql_num_fields(myquery);
    std::string sDatetime("");
    if (num_fields >0 ){
        while (nullptr != (row = mysql_fetch_row(myquery))){
            sDatetime = row[0];
            break;
        }
    }
    //mysql_free_result
    if (nullptr != myquery){
        mysql_free_result(myquery);
        myquery = nullptr;
    }

    if (!sDatetime.empty()){
        if ('0' != sDatetime[0]){
            dt = boost::posix_time::time_from_string(sDatetime);
        }
    }

    return dt;
}

RT_TPS_DATABASE_END_NAMESPACE