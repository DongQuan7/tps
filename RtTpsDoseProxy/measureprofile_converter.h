//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  wenqiao.jiang@united-imaging.com
/// 
///  \file measureprofile_converter.h
/// 
///  \version 1.0
/// 
///  \date    2016/09/12
//////////////////////////////////////////////////////////////////////////
#ifndef MEASUREPROFILE_CONVERTER_H_
#define MEASUREPROFILE_CONVERTER_H_

#include "RtTpsDoseProxy/tps_dp_defs.h"

namespace RTFWK
{
    class RTFWKDLDOMeasuredProfile;
}

namespace ms {
    namespace proto{
        class RT_MS_ImportFromFileInfo;
    }
}

TPS_BEGIN_NAMESPACE

class RtMeasuredprofile;

class TPS_DP_DECLSPEC MeasureProfileConverter
{
public:
    static bool ConvertToALG(const RtMeasuredprofile* source, std::shared_ptr<RTFWK::RTFWKDLDOMeasuredProfile>& target);
    static bool ConvertToALGList(std::vector<RtMeasuredprofile*> source, std::vector<std::shared_ptr<RTFWK::RTFWKDLDOMeasuredProfile>>& Target);
    static bool ConverBackFromALGList(std::vector<RTFWK::RTFWKDLDOMeasuredProfile*> source, std::vector<std::shared_ptr<RtMeasuredprofile>>& target);
    static bool ConverBackFromALG(const RTFWK::RTFWKDLDOMeasuredProfile* source, std::shared_ptr<RtMeasuredprofile>& target);
};
TPS_END_NAMESPACE
#endif