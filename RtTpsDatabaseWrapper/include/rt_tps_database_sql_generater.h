//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2015
///  All rights reserved.
///
///  \author  Xi Chen  mailto:xi.chen@united-imaging.com
///
///  \file      rt_tps_database_sql_generater.h
///  \brief     rt sql generater
///
///  \version 1.0
///  \date    Oct 20,2015
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_SQL_GENERATER_H_
#define RT_TPS_DATABASE_SQL_GENERATER_H_

#include <string>
#include <vector>
#include <tuple>
#include <memory>

#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

#define STR_SELECT			"SELECT"
#define STR_DELETE			"DELETE"
#define STR_SPACE			" "
#define STR_ASTERISK		"*"
#define STR_COMMA			","
#define STR_FROM			"FROM"
#define STR_WHERE			"WHERE"
#define STR_AND				"AND"
#define STR_OR				"OR"
#define STR_EQUAL			"="
#define STR_SINGLEQUOTE		"'"

typedef enum LOGICAL_OPERATOR
{
    EMPTY = 0,
    AND,
    OR,
};

// 暂不支持插入和更新。
typedef enum SQL_OPERATOR
{
    SQL_NONE_OPERATOR = 0,
    //SQL_INSERT,
    SQL_DELETE,
    //SQL_UPDATE,
    SQL_SELECT,
};


class RtSqlGenerater
{
public:
    RtSqlGenerater();

    virtual ~RtSqlGenerater();

    virtual void SetSqlOperator(SQL_OPERATOR eSqlOperator);

    virtual void SetTable(std::string sTableName);

    virtual void SetField(std::string sFieldName);

    virtual void SetFilter(std::string sFieldName, std::string sValue, LOGICAL_OPERATOR logicalOperator = EMPTY);

    bool GeneraterSql(std::string& sSqlResult);

    void Clear();
private:
    std::string msTableInfo;
    std::vector<std::string> mvFieldInfo;
    std::vector<std::tuple<std::string, std::string, LOGICAL_OPERATOR>> mvFilterInfo;
    SQL_OPERATOR meSqlOperator;
};

RT_TPS_DATABASE_END_NAMESPACE;

#endif 
