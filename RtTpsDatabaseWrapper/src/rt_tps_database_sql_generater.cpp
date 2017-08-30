//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2015
///  All rights reserved.
///
///  \author  Xi Chen  mailto:xi.chen@united-imaging.com
///
///  \file      rt_tps_database_sql_generater.cpp
///  \brief     rt sql generater
///
///  \version 1.0
///  \date    Oct 20,2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "rt_tps_database_sql_generater.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

RtSqlGenerater::RtSqlGenerater() : msTableInfo(""), meSqlOperator(SQL_NONE_OPERATOR)
{
    mvFieldInfo.clear();
    mvFilterInfo.clear();
}

RtSqlGenerater::~RtSqlGenerater()
{
    msTableInfo = "";
    meSqlOperator = SQL_NONE_OPERATOR;
    mvFieldInfo.clear();
    mvFilterInfo.clear();
}

void RtSqlGenerater::SetSqlOperator(SQL_OPERATOR eSqlOperator)
{
    meSqlOperator = eSqlOperator;
}

void RtSqlGenerater::SetTable(std::string sTableName)
{
    msTableInfo = sTableName;
}

void RtSqlGenerater::SetField(std::string sFieldName)
{
    mvFieldInfo.push_back(sFieldName);
}

void RtSqlGenerater::SetFilter(std::string sFieldName, std::string sValue, LOGICAL_OPERATOR logicalOperator/* = EMPTY*/)
{
    mvFilterInfo.push_back(std::make_tuple<std::string, std::string, LOGICAL_OPERATOR>(sFieldName, sValue, logicalOperator));
}

bool RtSqlGenerater::GeneraterSql(std::string& sSqlResult)
{
    sSqlResult = "";
    if (msTableInfo.empty() || meSqlOperator == SQL_NONE_OPERATOR)
    {
        return false;
    }

    if (meSqlOperator == SQL_SELECT)
    {
        sSqlResult += STR_SELECT;
    }
    else
    {
        sSqlResult += STR_DELETE;
    }

    if (meSqlOperator == SQL_SELECT)
    {
        sSqlResult += STR_SPACE;

        if (mvFieldInfo.empty())
        {
            sSqlResult += STR_ASTERISK;
        }
        else
        {
            for (int index = 0; index < mvFieldInfo.size(); index++)
            {
                if (index != 0)
                {
                    sSqlResult += STR_COMMA;
                }

                sSqlResult += mvFieldInfo[index];
            }
        }
    }

    sSqlResult += STR_SPACE;
    sSqlResult += STR_FROM;

    sSqlResult += STR_SPACE;
    sSqlResult += msTableInfo;

    if (!mvFilterInfo.empty())
    {
        sSqlResult += STR_SPACE;
        sSqlResult += STR_WHERE;

        for (int index = 0; index < mvFilterInfo.size(); index++)
        {
            sSqlResult += STR_SPACE;
            auto tupleFilterInfo = mvFilterInfo[index];
            auto sFieldName = get<0>(tupleFilterInfo);
            auto sValue = get<1>(tupleFilterInfo);
            auto sOperator = get<2>(tupleFilterInfo);

            if (index == 0)
            {
                sOperator = EMPTY;
            }

            switch (sOperator)
            {
            case EMPTY:
                break;
            case AND:
                sSqlResult += STR_AND;
                sSqlResult += STR_SPACE;
                break;
            case OR:
                sSqlResult += STR_OR;
                sSqlResult += STR_SPACE;
                break;
            }

            sSqlResult += sFieldName;
            sSqlResult += STR_EQUAL;
            sSqlResult += STR_SINGLEQUOTE;
            sSqlResult += sValue;
            sSqlResult += STR_SINGLEQUOTE;
        }
    }

    return true;
}

void RtSqlGenerater::Clear()
{
    msTableInfo = "";
    meSqlOperator = SQL_NONE_OPERATOR;
    mvFieldInfo.clear();
    mvFilterInfo.clear();
}
RT_TPS_DATABASE_END_NAMESPACE;