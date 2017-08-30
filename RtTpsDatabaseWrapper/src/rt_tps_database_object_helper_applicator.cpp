﻿//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_object_helper_applicator.cpp
///  \brief     This file was generated by CodeGenerater.exe 
///             From database version: 1.1.11
///
///  \version 1.0
///  \date    8/8/2017
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "rt_tps_database_object_helper.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_applicator.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"
#include "boost/date_time/posix_time/time_parsers.hpp"
typedef boost::posix_time::ptime DATETIME_BOOST;
typedef boost::posix_time::time_duration TIME_BOOST;
typedef boost::gregorian::date DATE_BOOST;
#include "RtTpsDatabaseWrapper/rt_tps_database_data.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

bool RtDatabaseHelper::InsertUpdateDBFromObject(const RtApplicator& src, bool bInsert)
{
    MYSQL_STMT* stmt = mysql_stmt_init(m_pMySql);
    if (!stmt)
    {
        TPS_LOG_DEV_ERROR<<"mysql_stmt_init(), out of memory";
        return false;
    }
    //24 parameters
    unsigned int field_num = 0;
    std::string sSql("");
    std::string sName("");
    if (!bInsert)
    {
        for (unsigned int i(0); i < APPLICATOR_FIELD_MAX; ++i)
        {
            if (!src.is_dirty(i)) continue;
            const char* file_name = ENUM2STRING_APPLICATOR_FIELD[i];
            if (sName.empty())
            {
                sName += std::string(file_name) + "=?";
            }
            else
            {
                sName += "," + std::string(file_name) + "=?";
            }
            ++field_num;
        }
        sSql = "UPDATE tmsapplicator SET " + sName + " WHERE uid='" + src.get_uid() + "'";
    }
    else
    {
        std::string sValues("");
        for (unsigned int i(0); i < APPLICATOR_FIELD_MAX; ++i)
        {
            if (!src.is_dirty(i)) continue;
            const char* file_name = ENUM2STRING_APPLICATOR_FIELD[i];
            if (sName.empty())
            {
                sName += std::string(file_name);
                sValues += "?";
            }
            else
            {
                sName += "," + std::string(file_name);
                sValues += ",?";
            }
            ++field_num;
        }
        sSql = "INSERT INTO tmsapplicator (" + sName + ") VALUES (" + sValues + ")";
    }
    if (mysql_stmt_prepare(stmt, sSql.c_str(), (unsigned long)sSql.size()))
    {
        TPS_LOG_DEV_ERROR<<" mysql_stmt_prepare(), failed "<< mysql_stmt_error(stmt);
        TPS_PRINTF_DEBUG("mysql_stmt_prepare(), failed %s\n", mysql_stmt_error(stmt));
        return false;
    }

    MYSQL_BIND* bind = new MYSQL_BIND[field_num];
    memset(bind, 0, sizeof(MYSQL_BIND)*field_num);
    my_bool     isnull = true;
    std::string	data_uid = "";
    std::string	data_machineuid = "";
    std::string	data_name = "";
    float		data_sourcetobottomdistance = 0.f;
    std::string	data_applicatorid = "";
    int			data_applicatortype = 0;
    int			data_applicatorapertureshape = 0;
    float		data_applicatoropening = 0.f;
    float		data_applicatoropeningx = 0.f;
    float		data_applicatoropeningy = 0.f;
    std::string	data_applicatordescription = "";
    int			data_mlcdirectiontype = 0;
    bool		data_jawxissymmetry = false;
    bool		data_jawyissymmetry = false;
    double		data_xupperjawpos = 0.;
    double		data_xlowerjawpos = 0.;
    double		data_yupperjawpos = 0.;
    double		data_ylowerjawpos = 0.;
    int			data_accuracy = 0;
    std::string	data_leafpositions = "";
    bool		data_isstartmlcshape = false;
    std::string	data_code = "";
    std::string	data_crc = "";
    MYSQL_TIME	data_updatetime;

    unsigned int iCount = 0;
    for (unsigned int i(0); i < APPLICATOR_FIELD_MAX; ++i)
    {
        if (!src.is_dirty(i)) continue;
        if(!src.has_field(i))
        {
            bind[iCount].is_null = &isnull;
            ++iCount;
            continue;
        }
        switch (i)
        {
        case 0: 	//uid varchar
            {
                data_uid = src.get_uid();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_uid.c_str();
                bind[iCount].buffer_length = (unsigned long)data_uid.size();
            }
            break;
        case 1: 	//machineuid varchar
            {
                data_machineuid = src.get_machineuid();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_machineuid.c_str();
                bind[iCount].buffer_length = (unsigned long)data_machineuid.size();
            }
            break;
        case 2: 	//name varchar
            {
                data_name = src.get_name();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_name.c_str();
                bind[iCount].buffer_length = (unsigned long)data_name.size();
            }
            break;
        case 3: 	//sourcetobottomdistance float
            {
                data_sourcetobottomdistance = src.get_sourcetobottomdistance();
                bind[iCount].buffer_type = MYSQL_TYPE_FLOAT;
                bind[iCount].buffer = (void*)&data_sourcetobottomdistance;
                bind[iCount].buffer_length = sizeof(float);
            }
            break;
        case 4: 	//applicatorid varchar
            {
                data_applicatorid = src.get_applicatorid();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_applicatorid.c_str();
                bind[iCount].buffer_length = (unsigned long)data_applicatorid.size();
            }
            break;
        case 5: 	//applicatortype int
            {
                data_applicatortype = src.get_applicatortype();
                bind[iCount].buffer_type = MYSQL_TYPE_LONG;
                bind[iCount].buffer = (void*)&data_applicatortype;
                bind[iCount].buffer_length = sizeof(int);
            }
            break;
        case 6: 	//applicatorapertureshape int
            {
                data_applicatorapertureshape = src.get_applicatorapertureshape();
                bind[iCount].buffer_type = MYSQL_TYPE_LONG;
                bind[iCount].buffer = (void*)&data_applicatorapertureshape;
                bind[iCount].buffer_length = sizeof(int);
            }
            break;
        case 7: 	//applicatoropening float
            {
                data_applicatoropening = src.get_applicatoropening();
                bind[iCount].buffer_type = MYSQL_TYPE_FLOAT;
                bind[iCount].buffer = (void*)&data_applicatoropening;
                bind[iCount].buffer_length = sizeof(float);
            }
            break;
        case 8: 	//applicatoropeningx float
            {
                data_applicatoropeningx = src.get_applicatoropeningx();
                bind[iCount].buffer_type = MYSQL_TYPE_FLOAT;
                bind[iCount].buffer = (void*)&data_applicatoropeningx;
                bind[iCount].buffer_length = sizeof(float);
            }
            break;
        case 9: 	//applicatoropeningy float
            {
                data_applicatoropeningy = src.get_applicatoropeningy();
                bind[iCount].buffer_type = MYSQL_TYPE_FLOAT;
                bind[iCount].buffer = (void*)&data_applicatoropeningy;
                bind[iCount].buffer_length = sizeof(float);
            }
            break;
        case 10: 	//applicatordescription varchar
            {
                data_applicatordescription = src.get_applicatordescription();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_applicatordescription.c_str();
                bind[iCount].buffer_length = (unsigned long)data_applicatordescription.size();
            }
            break;
        case 11: 	//mlcdirectiontype int
            {
                data_mlcdirectiontype = src.get_mlcdirectiontype();
                bind[iCount].buffer_type = MYSQL_TYPE_LONG;
                bind[iCount].buffer = (void*)&data_mlcdirectiontype;
                bind[iCount].buffer_length = sizeof(int);
            }
            break;
        case 12: 	//jawxissymmetry tinyint
            {
                data_jawxissymmetry = src.get_jawxissymmetry();
                bind[iCount].buffer_type = MYSQL_TYPE_TINY;
                bind[iCount].buffer = (void*)&data_jawxissymmetry;
                bind[iCount].buffer_length = sizeof(bool);
            }
            break;
        case 13: 	//jawyissymmetry tinyint
            {
                data_jawyissymmetry = src.get_jawyissymmetry();
                bind[iCount].buffer_type = MYSQL_TYPE_TINY;
                bind[iCount].buffer = (void*)&data_jawyissymmetry;
                bind[iCount].buffer_length = sizeof(bool);
            }
            break;
        case 14: 	//xupperjawpos double
            {
                data_xupperjawpos = src.get_xupperjawpos();
                bind[iCount].buffer_type = MYSQL_TYPE_DOUBLE;
                bind[iCount].buffer = (void*)&data_xupperjawpos;
                bind[iCount].buffer_length = sizeof(double);
            }
            break;
        case 15: 	//xlowerjawpos double
            {
                data_xlowerjawpos = src.get_xlowerjawpos();
                bind[iCount].buffer_type = MYSQL_TYPE_DOUBLE;
                bind[iCount].buffer = (void*)&data_xlowerjawpos;
                bind[iCount].buffer_length = sizeof(double);
            }
            break;
        case 16: 	//yupperjawpos double
            {
                data_yupperjawpos = src.get_yupperjawpos();
                bind[iCount].buffer_type = MYSQL_TYPE_DOUBLE;
                bind[iCount].buffer = (void*)&data_yupperjawpos;
                bind[iCount].buffer_length = sizeof(double);
            }
            break;
        case 17: 	//ylowerjawpos double
            {
                data_ylowerjawpos = src.get_ylowerjawpos();
                bind[iCount].buffer_type = MYSQL_TYPE_DOUBLE;
                bind[iCount].buffer = (void*)&data_ylowerjawpos;
                bind[iCount].buffer_length = sizeof(double);
            }
            break;
        case 18: 	//accuracy int
            {
                data_accuracy = src.get_accuracy();
                bind[iCount].buffer_type = MYSQL_TYPE_LONG;
                bind[iCount].buffer = (void*)&data_accuracy;
                bind[iCount].buffer_length = sizeof(int);
            }
            break;
        case 19: 	//leafpositions varchar
            {
                std::vector<db_Point2d> leafpositions = src.get_leafpositions();
                const size_t leafSize = leafpositions.size();
                if (leafSize > 1 && leafSize%2 == 0){
                    //const size_t total_length = leafSize*2*FLT_MAX_LENGHT + leafSize*2;//for "\\"
                    data_leafpositions += std::to_string((long double)(leafpositions[0].x));
                    for (int x(1); x<leafSize; ++x){//300A,00BC 101,102,,,1N,201,202,,,2N
                        data_leafpositions += "\\" + std::to_string((long double)(leafpositions[x].x));
                    }
                    for (int y(0); y<leafSize; ++y){
                        data_leafpositions += "\\" + std::to_string((long double)(leafpositions[y].y));
                    }
                }
                bind[iCount].buffer_type= MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer= (void *)data_leafpositions.c_str();
                bind[iCount].buffer_length= (unsigned long)data_leafpositions.size();
            }
            break;
        case 20: 	//isstartmlcshape tinyint
            {
                data_isstartmlcshape = src.get_isstartmlcshape();
                bind[iCount].buffer_type = MYSQL_TYPE_TINY;
                bind[iCount].buffer = (void*)&data_isstartmlcshape;
                bind[iCount].buffer_length = sizeof(bool);
            }
            break;
        case 21: 	//code varchar
            {
                data_code = src.get_code();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_code.c_str();
                bind[iCount].buffer_length = (unsigned long)data_code.size();
            }
            break;
        case 22: 	//crc varchar
            {
                data_crc = src.get_crc();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_crc.c_str();
                bind[iCount].buffer_length = (unsigned long)data_crc.size();
            }
            break;
        case 23: 	//updatetime timestamp
            {
                ConvertDateTime2DB(src.get_updatetime(), &data_updatetime);
                bind[iCount].buffer_type = MYSQL_TYPE_TIMESTAMP;
                bind[iCount].buffer = (void*)&data_updatetime;
                if (src.get_updatetime().is_not_a_date_time()) bind[iCount].is_null = &isnull;
            }
            break;
        default:
            break;
        }

        ++iCount;
    }
    if (mysql_stmt_bind_param(stmt, bind))
    {
        TPS_LOG_DEV_ERROR<<" mysql_stmt_bind_param(), failed "<< mysql_stmt_error(stmt);
        DEL_ARRAY(bind);
        return false;
    }
    if (mysql_stmt_execute(stmt))
    {
        TPS_LOG_DEV_ERROR<<" mysql_stmt_execute(), failed "<< mysql_stmt_error(stmt);
        TPS_PRINTF_DEBUG("mysql_stmt_execute(), failed %s\n", mysql_stmt_error(stmt));
        DEL_ARRAY(bind);
        return false;
    }
    if (mysql_stmt_close(stmt))
    {
        TPS_LOG_DEV_ERROR<<" failed while closing the statement "<< mysql_stmt_error(stmt);
        DEL_ARRAY(bind);
        return false;
    }
	DEL_ARRAY(bind);
	RtDatabaseObject* rtDatabaseObject = const_cast<RtApplicator*>(&src);
	rtDatabaseObject->setdirty_all_fields(false);
    return true;
}

bool RtDatabaseHelper::FetchDataFromDB(const std::string& sSql, std::vector<RtApplicator*>& vList)
{
    MYSQL_STMT* stmt = mysql_stmt_init(m_pMySql);
    if (!stmt)
    {
        TPS_LOG_DEV_ERROR<<"mysql_stmt_init(), out of memory";
        return false;
    }

    MYSQL_RES  *prepare_meta_result = nullptr;
    //tmsapplicator    24 parameters
    const int field_num = APPLICATOR_FIELD_MAX;
    unsigned long MAX_LENGTH[field_num] = {0};
    MAX_LENGTH[0] = 64;
    MAX_LENGTH[1] = 64;
    MAX_LENGTH[2] = 1024;
    MAX_LENGTH[3] = 0;
    MAX_LENGTH[4] = 64;
    MAX_LENGTH[5] = 0;
    MAX_LENGTH[6] = 0;
    MAX_LENGTH[7] = 0;
    MAX_LENGTH[8] = 0;
    MAX_LENGTH[9] = 0;
    MAX_LENGTH[10] = 1024;
    MAX_LENGTH[11] = 0;
    MAX_LENGTH[12] = 0;
    MAX_LENGTH[13] = 0;
    MAX_LENGTH[14] = 0;
    MAX_LENGTH[15] = 0;
    MAX_LENGTH[16] = 0;
    MAX_LENGTH[17] = 0;
    MAX_LENGTH[18] = 0;
    MAX_LENGTH[19] = 4096;
    MAX_LENGTH[20] = 0;
    MAX_LENGTH[21] = 8;
    MAX_LENGTH[22] = 64;
    MAX_LENGTH[23] = 0;

    //std::string	data_uid
    //std::string	data_machineuid
    //std::string	data_name
    float	data_sourcetobottomdistance = 0.f;
    //std::string	data_applicatorid
    int	data_applicatortype = 0;
    int	data_applicatorapertureshape = 0;
    float	data_applicatoropening = 0.f;
    float	data_applicatoropeningx = 0.f;
    float	data_applicatoropeningy = 0.f;
    //std::string	data_applicatordescription
    int	data_mlcdirectiontype = 0;
    bool	data_jawxissymmetry = false;
    bool	data_jawyissymmetry = false;
    double	data_xupperjawpos = 0.;
    double	data_xlowerjawpos = 0.;
    double	data_yupperjawpos = 0.;
    double	data_ylowerjawpos = 0.;
    int	data_accuracy = 0;
    //std::string	data_leafpositions
    bool	data_isstartmlcshape = false;
    //std::string	data_code
    //std::string	data_crc
    MYSQL_TIME	data_updatetime;

    if (mysql_stmt_prepare(stmt, sSql.c_str(), (unsigned long)strlen(sSql.c_str())))
    {
        TPS_LOG_DEV_ERROR<<" mysql_stmt_prepare(), SELECT failed."<<mysql_stmt_error(stmt);
        return false;
    }

    //Fetch result set meta information
    prepare_meta_result = mysql_stmt_result_metadata(stmt);
    if (!prepare_meta_result) 
    {
        TPS_LOG_DEV_ERROR<<" mysql_stmt_result_metadata(), returned no meta information "<<mysql_stmt_error(stmt);
        return false;
    }

    // Get total columns in the query and validate column count
    const unsigned int column_count= mysql_num_fields(prepare_meta_result);
    if (column_count != field_num)
    {
        TPS_LOG_DEV_ERROR<<"invalid column count returned by MySQL";
        return false;
    }

    MYSQL_BIND* bind = new MYSQL_BIND[column_count];
    memset(bind, 0, sizeof(MYSQL_BIND)*column_count);
    unsigned long* length = new unsigned long[column_count];
    memset(length, 0, sizeof(unsigned long) * column_count);
    my_bool* is_null = new my_bool[column_count];
    memset(is_null, 0, sizeof(my_bool) * column_count);
    //Execute the SELECT query
    char* data_string[field_num];
    for (int i=0; i<field_num; ++i)
    {
        const unsigned long ulSize = MAX_LENGTH[i] + 1;// + 1 for string's \0 
        data_string[i] = new char[ulSize];
        memset(data_string[i], 0, ulSize*sizeof(char));
    }
    if (mysql_stmt_execute(stmt))
    {
        TPS_LOG_DEV_ERROR<<" mysql_stmt_execute(), failed"<<mysql_stmt_error(stmt);
        for (int i=0; i<field_num; ++i) DEL_ARRAY(data_string[i]);
        DEL_ARRAY(bind);
        DEL_ARRAY(length);
        DEL_ARRAY(is_null);
        return false;
    }

    for (unsigned int i(0); i<column_count; ++i)
    {
        bind[i].is_null= &is_null[i];
        bind[i].length= &length[i];
        switch (i)
        {
        case 3: 	//sourcetobottomdistance float
            bind[i].buffer_type= MYSQL_TYPE_FLOAT;
            bind[i].buffer= (char *)&data_sourcetobottomdistance;
            break;
        case 5: 	//applicatortype int
            bind[i].buffer_type= MYSQL_TYPE_LONG;
            bind[i].buffer= (char *)&data_applicatortype;
            break;
        case 6: 	//applicatorapertureshape int
            bind[i].buffer_type= MYSQL_TYPE_LONG;
            bind[i].buffer= (char *)&data_applicatorapertureshape;
            break;
        case 7: 	//applicatoropening float
            bind[i].buffer_type= MYSQL_TYPE_FLOAT;
            bind[i].buffer= (char *)&data_applicatoropening;
            break;
        case 8: 	//applicatoropeningx float
            bind[i].buffer_type= MYSQL_TYPE_FLOAT;
            bind[i].buffer= (char *)&data_applicatoropeningx;
            break;
        case 9: 	//applicatoropeningy float
            bind[i].buffer_type= MYSQL_TYPE_FLOAT;
            bind[i].buffer= (char *)&data_applicatoropeningy;
            break;
        case 11: 	//mlcdirectiontype int
            bind[i].buffer_type= MYSQL_TYPE_LONG;
            bind[i].buffer= (char *)&data_mlcdirectiontype;
            break;
        case 12: 	//jawxissymmetry tinyint
            bind[i].buffer_type= MYSQL_TYPE_TINY;
            bind[i].buffer= (char *)&data_jawxissymmetry;
            break;
        case 13: 	//jawyissymmetry tinyint
            bind[i].buffer_type= MYSQL_TYPE_TINY;
            bind[i].buffer= (char *)&data_jawyissymmetry;
            break;
        case 14: 	//xupperjawpos double
            bind[i].buffer_type= MYSQL_TYPE_DOUBLE;
            bind[i].buffer= (char *)&data_xupperjawpos;
            break;
        case 15: 	//xlowerjawpos double
            bind[i].buffer_type= MYSQL_TYPE_DOUBLE;
            bind[i].buffer= (char *)&data_xlowerjawpos;
            break;
        case 16: 	//yupperjawpos double
            bind[i].buffer_type= MYSQL_TYPE_DOUBLE;
            bind[i].buffer= (char *)&data_yupperjawpos;
            break;
        case 17: 	//ylowerjawpos double
            bind[i].buffer_type= MYSQL_TYPE_DOUBLE;
            bind[i].buffer= (char *)&data_ylowerjawpos;
            break;
        case 18: 	//accuracy int
            bind[i].buffer_type= MYSQL_TYPE_LONG;
            bind[i].buffer= (char *)&data_accuracy;
            break;
        case 20: 	//isstartmlcshape tinyint
            bind[i].buffer_type= MYSQL_TYPE_TINY;
            bind[i].buffer= (char *)&data_isstartmlcshape;
            break;
        case 23: 	//updatetime timestamp
            bind[i].buffer_type= MYSQL_TYPE_TIMESTAMP;
            bind[i].buffer= (char *)&data_updatetime;
            break;
        default:
            bind[i].buffer_type= MYSQL_TYPE_STRING;
            bind[i].buffer= (char *)data_string[i];
            bind[i].buffer_length= MAX_LENGTH[i];
            break;
        }
    }

    // Bind the result buffers
    if (mysql_stmt_bind_result(stmt, bind))
    {
        TPS_LOG_DEV_ERROR<<" mysql_stmt_bind_result() failed "<<mysql_stmt_error(stmt);
        for (int i=0; i<field_num; ++i) DEL_ARRAY(data_string[i]);
        DEL_ARRAY(bind);
        DEL_ARRAY(length);
        DEL_ARRAY(is_null);
        return false;
    }

    //Now buffer all results to client
    if (mysql_stmt_store_result(stmt))
    {
        TPS_LOG_DEV_ERROR<<" mysql_stmt_store_result() failed "<<mysql_stmt_error(stmt);
        for (int i=0; i<field_num; ++i) DEL_ARRAY(data_string[i]);
        DEL_ARRAY(bind);
        DEL_ARRAY(length);
        DEL_ARRAY(is_null);
        return false;
    }

    // Fetch all rows 
    int ret(1);
    RtApplicator* pDis = nullptr;
    while(true)
    {
        ret = mysql_stmt_fetch(stmt);
        if (ret!=0 && ret!=MYSQL_DATA_TRUNCATED) break;
        pDis = new RtApplicator();
        for (unsigned int i(0); i<column_count; ++i)
        {
            if (is_null[i]) continue;
            switch (i)
            {
            case 0: 	//uid varchar
                pDis->set_uid((char *)bind[i].buffer);
                break;
            case 1: 	//machineuid varchar
                pDis->set_machineuid((char *)bind[i].buffer);
                break;
            case 2: 	//name varchar
                pDis->set_name((char *)bind[i].buffer);
                break;
            case 3: 	//sourcetobottomdistance float
                pDis->set_sourcetobottomdistance(data_sourcetobottomdistance);
                break;
            case 4: 	//applicatorid varchar
                pDis->set_applicatorid((char *)bind[i].buffer);
                break;
            case 5: 	//applicatortype int
                pDis->set_applicatortype(data_applicatortype);
                break;
            case 6: 	//applicatorapertureshape int
                pDis->set_applicatorapertureshape(data_applicatorapertureshape);
                break;
            case 7: 	//applicatoropening float
                pDis->set_applicatoropening(data_applicatoropening);
                break;
            case 8: 	//applicatoropeningx float
                pDis->set_applicatoropeningx(data_applicatoropeningx);
                break;
            case 9: 	//applicatoropeningy float
                pDis->set_applicatoropeningy(data_applicatoropeningy);
                break;
            case 10: 	//applicatordescription varchar
                pDis->set_applicatordescription((char *)bind[i].buffer);
                break;
            case 11: 	//mlcdirectiontype int
                pDis->set_mlcdirectiontype(data_mlcdirectiontype);
                break;
            case 12: 	//jawxissymmetry tinyint
                pDis->set_jawxissymmetry(data_jawxissymmetry);
                break;
            case 13: 	//jawyissymmetry tinyint
                pDis->set_jawyissymmetry(data_jawyissymmetry);
                break;
            case 14: 	//xupperjawpos double
                pDis->set_xupperjawpos(data_xupperjawpos);
                break;
            case 15: 	//xlowerjawpos double
                pDis->set_xlowerjawpos(data_xlowerjawpos);
                break;
            case 16: 	//yupperjawpos double
                pDis->set_yupperjawpos(data_yupperjawpos);
                break;
            case 17: 	//ylowerjawpos double
                pDis->set_ylowerjawpos(data_ylowerjawpos);
                break;
            case 18: 	//accuracy int
                pDis->set_accuracy(data_accuracy);
                break;
            case 19: 	//leafpositions varchar
                {
                    const int total_length = length[i];
                    if (total_length < 2){
                        break;
                    }
                    char *buf=new char[total_length + 1];
                    memset(buf,0,sizeof(char) * total_length + 1);
                    bind[i].buffer = buf;
                    bind[i].buffer_length = total_length;
                    ret = mysql_stmt_fetch_column(stmt, &bind[i], i, 0);
                    if (0 == ret){
                        std::vector<std::string> subValue;
                        (void)boost::split(subValue, buf, boost::is_any_of("\\"));
                        std::vector<db_Point2d> vLeafPos;
                        const size_t fSize = subValue.size()/2;//300A,00BC 101,102,,,1N,201,202,,,2N
                        for (size_t x=0; x<fSize; ++x){
                            double fValuex = atof(subValue[x].c_str());
                            double fValuey = atof(subValue[fSize + x].c_str());
                            db_Point2d pt(fValuex,fValuey);
                            vLeafPos.push_back(pt);
                        }
                        pDis->set_leafpositions(vLeafPos);
                    }
                    DEL_ARRAY(buf);
                }
                break;
            case 20: 	//isstartmlcshape tinyint
                pDis->set_isstartmlcshape(data_isstartmlcshape);
                break;
            case 21: 	//code varchar
                pDis->set_code((char *)bind[i].buffer);
                break;
            case 22: 	//crc varchar
                pDis->set_crc((char *)bind[i].buffer);
                break;
            case 23: 	//updatetime timestamp
                {
                    DATETIME_BOOST updatetime;
                    ConvertDateTime2Boost(data_updatetime, &updatetime);
                    pDis->set_updatetime(updatetime);
                }
                break;
            default:
                break;
            }
        }
        pDis->setdirty_all_fields(false);
        vList.push_back(pDis);
    }
    for (int i=0; i<field_num; ++i) DEL_ARRAY(data_string[i]);
    DEL_ARRAY(bind);
    DEL_ARRAY(length);
    DEL_ARRAY(is_null);

    // Free the prepared result metadata
    mysql_free_result(prepare_meta_result);

    // Close the statement
    if (mysql_stmt_close(stmt))
    {
        TPS_LOG_DEV_ERROR<<" failed while closing the statement " << mysql_stmt_error(stmt);
        return false;
    }

    return true;
}

RT_TPS_DATABASE_END_NAMESPACE