////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  SHI chunhui  chunhui.shi@united-imaging.com
/// 
///  \file rt_tps_certified4dctseriesuidlist.h
/// 
///  \brief  Certified4DCTSeriesUidList
/// 
///  \version 1.0
/// 
///  \date    2014/07/24
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIED_4DCT_SERIES_UID_LISTVEINFO_H_
#define RT_TPS_CERTIFIED_4DCT_SERIES_UID_LISTVEINFO_H_

#include "tps_defs.h"
#include "RtTpsProtoLib/rt_tps_seriesinfogroup.pb.h"

TPS_BEGIN_NAMESPACE

struct Certified4DCTSeriesUidList{

    std::vector<std::string> mUidList;
    void Convert(const proto::RT_TPS_SeriesInfoGroup& protoSeiresGroup) {
        for (int i = 0; i < protoSeiresGroup.series_uid().size(); ++i) {
            mUidList.push_back(protoSeiresGroup.series_uid(i));
        }
    }
};

TPS_END_NAMESPACE

#endif // RT_TPS_CERTIFIEDVOI_H_