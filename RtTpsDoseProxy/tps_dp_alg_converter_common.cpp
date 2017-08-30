//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Wang Zhendong  zhendong.wang@united-imaging.com
/// 
///  \file tps_dp_alg_converter_common.cpp
/// 
///  \version 1.0
/// 
///  \date    2017/01/10
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RtTpsDoseProxy/tps_dp_alg_converter_common.h"

using namespace std;


void RemoveSpace(std::string& s)
{
    std::string::size_type loc = 0;
    loc = s.find(' ');
    while (loc != std::string::npos)
    {
        s.erase(loc, 1);
        loc = s.find(' ', loc);
    }
}







