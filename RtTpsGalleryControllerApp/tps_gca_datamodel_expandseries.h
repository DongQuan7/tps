////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_idataloader.h
///  \brief   
///
///  \version 1.0
///
///  \date    Apr. 03, 2015
/////////////////////////////////////////////////////////////////
#ifndef TPS_GCA_DM_EXPANDSERIES_H_
#define TPS_GCA_DM_EXPANDSERIES_H_

#include "RtTpsGalleryControllerApp/tps_gca_defs.h"
#include "tms/rt_fwk_dl_object_base.h"
#include <cpprest/json.h>

namespace RTFWK{
    class RTFWKDLDOStorageObjectBase;
}

TPS_BEGIN_NAMESPACE


class TpsGcaDmExpandSeries: public RTFWK::RTFWKDLDOStorageObjectBase
{       
public:  
    TpsGcaDmExpandSeries(std::string seriesUID, int sliceNumber, bool isExpandBottom, bool isSaveAsNewSeries);
    ~TpsGcaDmExpandSeries();

    bool SerializeToJson(web::json::value& jsonValue);

    /////////////////////////////////////////////////////////////////
    // Save To DB by appserver
    /////////////////////////////////////////////////////////////////
    virtual bool SaveEx();

private:
    std::string m_sSeriesUID;
    bool        m_bIsExpandBottom;
    int         m_iSliceNumber;
    bool        m_bIsSaveAsNewSeries;
};

TPS_END_NAMESPACE

#endif  //TPS_COMMON_IDATALOADER_H_
