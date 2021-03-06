﻿//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_object_helper_equipment.cpp
///  \brief     This file was generated by CodeGenerater.exe 
///             From database version: 1.1.11
///
///  \version 1.0
///  \date    8/8/2017
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "rt_tps_database_object_helper.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_equipment.h"
#include "boost/date_time/posix_time/time_parsers.hpp"
typedef boost::posix_time::ptime DATETIME_BOOST;
typedef boost::posix_time::time_duration TIME_BOOST;
typedef boost::gregorian::date DATE_BOOST;

RT_TPS_DATABASE_BEGIN_NAMESPACE;

bool RtDatabaseHelper::InsertUpdateDBFromObject(const RtEquipment& src, bool bInsert)
{
    MYSQL_STMT* stmt = mysql_stmt_init(m_pMySql);
    if (!stmt)
    {
        TPS_LOG_DEV_ERROR<<"mysql_stmt_init(), out of memory";
        return false;
    }
    //14 parameters
    unsigned int field_num = 0;
    std::string sSql("");
    std::string sName("");
    if (!bInsert)
    {
        for (unsigned int i(0); i < EQUIPMENT_FIELD_MAX; ++i)
        {
            if (!src.is_dirty(i)) continue;
            const char* file_name = ENUM2STRING_EQUIPMENT_FIELD[i];
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
        sSql = "UPDATE " TABLE_EQUIPMENT "SET " + sName + " WHERE uid='" + src.get_uid() + "'";
    }
    else
    {
        std::string sValues("");
        for (unsigned int i(0); i < EQUIPMENT_FIELD_MAX; ++i)
        {
            if (!src.is_dirty(i)) continue;
            const char* file_name = ENUM2STRING_EQUIPMENT_FIELD[i];
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
        sSql = "INSERT INTO " TABLE_EQUIPMENT " (" + sName + ") VALUES (" + sValues + ")";
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
    std::string	data_manufacturer = "";
    std::string	data_institutionname = "";
    std::string	data_Institutionaddress = "";
    std::string	data_stationname = "";
    std::string	data_institutionaleepartmentname = "";
    std::string	data_manufacturermodelname = "";
    std::string	data_deviceserialnumber = "";
    std::string	data_gantryid = "";
    double		data_spatialresolution = 0.;
    MYSQL_TIME	data_dateoflastcalibration;
    MYSQL_TIME	data_timeoflastcalibration;
    int			data_pixelpaddingvalue = 0;
    std::string	data_softwareversion = "";

    unsigned int iCount = 0;
    for (unsigned int i(0); i < EQUIPMENT_FIELD_MAX; ++i)
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
        case 1: 	//manufacturer varchar
            {
                data_manufacturer = src.get_manufacturer();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_manufacturer.c_str();
                bind[iCount].buffer_length = (unsigned long)data_manufacturer.size();
            }
            break;
        case 2: 	//institutionname varchar
            {
                data_institutionname = src.get_institutionname();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_institutionname.c_str();
                bind[iCount].buffer_length = (unsigned long)data_institutionname.size();
            }
            break;
        case 3: 	//Institutionaddress varchar
            {
                data_Institutionaddress = src.get_Institutionaddress();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_Institutionaddress.c_str();
                bind[iCount].buffer_length = (unsigned long)data_Institutionaddress.size();
            }
            break;
        case 4: 	//stationname varchar
            {
                data_stationname = src.get_stationname();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_stationname.c_str();
                bind[iCount].buffer_length = (unsigned long)data_stationname.size();
            }
            break;
        case 5: 	//institutionaleepartmentname varchar
            {
                data_institutionaleepartmentname = src.get_institutionaleepartmentname();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_institutionaleepartmentname.c_str();
                bind[iCount].buffer_length = (unsigned long)data_institutionaleepartmentname.size();
            }
            break;
        case 6: 	//manufacturermodelname varchar
            {
                data_manufacturermodelname = src.get_manufacturermodelname();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_manufacturermodelname.c_str();
                bind[iCount].buffer_length = (unsigned long)data_manufacturermodelname.size();
            }
            break;
        case 7: 	//deviceserialnumber varchar
            {
                data_deviceserialnumber = src.get_deviceserialnumber();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_deviceserialnumber.c_str();
                bind[iCount].buffer_length = (unsigned long)data_deviceserialnumber.size();
            }
            break;
        case 8: 	//gantryid varchar
            {
                data_gantryid = src.get_gantryid();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_gantryid.c_str();
                bind[iCount].buffer_length = (unsigned long)data_gantryid.size();
            }
            break;
        case 9: 	//spatialresolution double
            {
                data_spatialresolution = src.get_spatialresolution();
                bind[iCount].buffer_type = MYSQL_TYPE_DOUBLE;
                bind[iCount].buffer = (void*)&data_spatialresolution;
                bind[iCount].buffer_length = sizeof(double);
            }
            break;
        case 10: 	//dateoflastcalibration date
            {
                ConvertDateTime2DB(src.get_dateoflastcalibration(), &data_dateoflastcalibration);
                bind[iCount].buffer_type = MYSQL_TYPE_DATE;
                bind[iCount].buffer = (void*)&data_dateoflastcalibration;
                if (src.get_dateoflastcalibration().is_not_a_date()) bind[iCount].is_null = &isnull;
            }
            break;
        case 11: 	//timeoflastcalibration time
            {
                ConvertDateTime2DB(src.get_timeoflastcalibration(), &data_timeoflastcalibration);
                bind[iCount].buffer_type = MYSQL_TYPE_TIME;
                bind[iCount].buffer = (void*)&data_timeoflastcalibration;
                if (src.get_timeoflastcalibration().is_not_a_date_time()) bind[iCount].is_null = &isnull;
            }
            break;
        case 12: 	//pixelpaddingvalue int
            {
                data_pixelpaddingvalue = src.get_pixelpaddingvalue();
                bind[iCount].buffer_type = MYSQL_TYPE_LONG;
                bind[iCount].buffer = (void*)&data_pixelpaddingvalue;
                bind[iCount].buffer_length = sizeof(int);
            }
            break;
        case 13: 	//softwareversion varchar
            {
                data_softwareversion = src.get_softwareversion();
                bind[iCount].buffer_type = MYSQL_TYPE_VAR_STRING;
                bind[iCount].buffer = (void*)data_softwareversion.c_str();
                bind[iCount].buffer_length = (unsigned long)data_softwareversion.size();
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
	RtDatabaseObject* rtDatabaseObject = const_cast<RtEquipment*>(&src);
	rtDatabaseObject->setdirty_all_fields(false);
    return true;
}

bool RtDatabaseHelper::FetchDataFromDB(const std::string& sSql, std::vector<RtEquipment*>& vList)
{
    MYSQL_STMT* stmt = mysql_stmt_init(m_pMySql);
    if (!stmt)
    {
        TPS_LOG_DEV_ERROR<<"mysql_stmt_init(), out of memory";
        return false;
    }

    MYSQL_RES  *prepare_meta_result = nullptr;
    //tmsequipment    14 parameters
    const int field_num = EQUIPMENT_FIELD_MAX;
    unsigned long MAX_LENGTH[field_num] = {0};
    MAX_LENGTH[0] = 64;
    MAX_LENGTH[1] = 256;
    MAX_LENGTH[2] = 256;
    MAX_LENGTH[3] = 256;
    MAX_LENGTH[4] = 256;
    MAX_LENGTH[5] = 256;
    MAX_LENGTH[6] = 256;
    MAX_LENGTH[7] = 256;
    MAX_LENGTH[8] = 256;
    MAX_LENGTH[9] = 0;
    MAX_LENGTH[10] = 0;
    MAX_LENGTH[11] = 0;
    MAX_LENGTH[12] = 0;
    MAX_LENGTH[13] = 64;

    //std::string	data_uid
    //std::string	data_manufacturer
    //std::string	data_institutionname
    //std::string	data_Institutionaddress
    //std::string	data_stationname
    //std::string	data_institutionaleepartmentname
    //std::string	data_manufacturermodelname
    //std::string	data_deviceserialnumber
    //std::string	data_gantryid
    double	data_spatialresolution = 0.;
    MYSQL_TIME	data_dateoflastcalibration;
    MYSQL_TIME	data_timeoflastcalibration;
    int	data_pixelpaddingvalue = 0;
    //std::string	data_softwareversion

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
        case 9: 	//spatialresolution double
            bind[i].buffer_type= MYSQL_TYPE_DOUBLE;
            bind[i].buffer= (char *)&data_spatialresolution;
            break;
        case 10: 	//dateoflastcalibration date
            bind[i].buffer_type= MYSQL_TYPE_DATE;
            bind[i].buffer= (char *)&data_dateoflastcalibration;
            break;
        case 11: 	//timeoflastcalibration time
            bind[i].buffer_type= MYSQL_TYPE_TIME;
            bind[i].buffer= (char *)&data_timeoflastcalibration;
            break;
        case 12: 	//pixelpaddingvalue int
            bind[i].buffer_type= MYSQL_TYPE_LONG;
            bind[i].buffer= (char *)&data_pixelpaddingvalue;
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
    RtEquipment* pDis = nullptr;
    while(true)
    {
        ret = mysql_stmt_fetch(stmt);
        if (ret!=0 && ret!=MYSQL_DATA_TRUNCATED) break;
        pDis = new RtEquipment();
        for (unsigned int i(0); i<column_count; ++i)
        {
            if (is_null[i]) continue;
            switch (i)
            {
            case 0: 	//uid varchar
                pDis->set_uid((char *)bind[i].buffer);
                break;
            case 1: 	//manufacturer varchar
                pDis->set_manufacturer((char *)bind[i].buffer);
                break;
            case 2: 	//institutionname varchar
                pDis->set_institutionname((char *)bind[i].buffer);
                break;
            case 3: 	//Institutionaddress varchar
                pDis->set_Institutionaddress((char *)bind[i].buffer);
                break;
            case 4: 	//stationname varchar
                pDis->set_stationname((char *)bind[i].buffer);
                break;
            case 5: 	//institutionaleepartmentname varchar
                pDis->set_institutionaleepartmentname((char *)bind[i].buffer);
                break;
            case 6: 	//manufacturermodelname varchar
                pDis->set_manufacturermodelname((char *)bind[i].buffer);
                break;
            case 7: 	//deviceserialnumber varchar
                pDis->set_deviceserialnumber((char *)bind[i].buffer);
                break;
            case 8: 	//gantryid varchar
                pDis->set_gantryid((char *)bind[i].buffer);
                break;
            case 9: 	//spatialresolution double
                pDis->set_spatialresolution(data_spatialresolution);
                break;
            case 10: 	//dateoflastcalibration date
                {
                    DATE_BOOST dateoflastcalibration;
                    ConvertDateTime2Boost(data_dateoflastcalibration, &dateoflastcalibration);
                    pDis->set_dateoflastcalibration(dateoflastcalibration);
                }
                break;
            case 11: 	//timeoflastcalibration time
                {
                    TIME_BOOST timeoflastcalibration;
                    ConvertDateTime2Boost(data_timeoflastcalibration, &timeoflastcalibration);
                    pDis->set_timeoflastcalibration(timeoflastcalibration);
                }
                break;
            case 12: 	//pixelpaddingvalue int
                pDis->set_pixelpaddingvalue(data_pixelpaddingvalue);
                break;
            case 13: 	//softwareversion varchar
                pDis->set_softwareversion((char *)bind[i].buffer);
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