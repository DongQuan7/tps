//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_cryptohash.cpp
///  \brief     calc MD5 hash
///              
///
///  \version 1.0
///  \date    Nov.15, 2016
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "rt_tps_database_cryptohash.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

std::string string_utils::hextostr(std::vector<unsigned char> const & hexval, bool uppercase)
{
    std::stringstream sstr;

    if(!hexval.empty())
    {
        sstr.setf(std::ios_base::hex, std::ios::basefield);
        if(uppercase)
            sstr.setf(std::ios_base::uppercase);
        sstr.fill('0');
        for (size_t i = 0; i<hexval.size(); i++)
        {
            sstr << std::setw(2) << (unsigned int)(unsigned char)(hexval[i]);
        }
    }

    return sstr.str();
}

RT_TPS_DATABASE_END_NAMESPACE