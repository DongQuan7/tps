//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Wang Zhendong  zhendong.wang@united-imaging.com
/// 
///  \file tps_dp_alg_converter_common.h
/// 
///  \version 1.0
/// 
///  \date    2016/10/27
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DP_ALGCONVERTERCOMMON_H_
#define TPS_DP_ALGCONVERTERCOMMON_H_

#include "stdafx.h"
#include "tps_dp_defs.h"

void RemoveSpace(std::string& s);



// 提供AlgConverter的公共模板方法
template <typename k, typename v>
std::map<k,v> GetMapFromString(std::string sSource)
{
    RemoveSpace(sSource);
    std::vector<std::string> vecString;
    (void)boost::split(vecString, sSource, boost::is_any_of("|"));
    std::map<k, v> OutMap;
    std::for_each(vecString.begin(), vecString.end(),
        [&](std::string oneStringPair)
    {
        std::vector<std::string> vString;
        (void)boost::split(vString, oneStringPair, boost::is_any_of(","));
        if (vString.size() == 2)
        {
            k kk = boost::lexical_cast<k>(vString[0]);
            v vv = boost::lexical_cast<v>(vString[1]);
            OutMap.insert(std::make_pair<k, v>(kk, vv));
        }
    });
    return OutMap;
}


template <typename T>
std::vector<T> GetVecFromString(std::string s)
{
    // 去除空格
    RemoveSpace(s);

    std::vector<T> vObj;
    string::size_type loc = 0;
    loc = s.find('|');
    if (loc != string::npos)
    {
        return vObj;
    }

    std::vector<std::string> vecValue;
    (void)boost::split(vecValue, s, boost::is_any_of(","));
    size_t iLen = vecValue.size();
    vObj.resize(iLen);
    for (int i = 0; i < iLen; i++)
    {
        if (!vecValue[i].empty())
        {
            vObj[i] = boost::lexical_cast<T>(vecValue[i]);
        }
    }
    vecValue.clear();
    return vObj;
}
#endif
