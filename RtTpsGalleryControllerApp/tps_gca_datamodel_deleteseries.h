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
#ifndef TPS_GCA_DM_DELETESERIES_H_
#define TPS_GCA_DM_DELETESERIES_H_

#include "RtTpsGalleryControllerApp/tps_gca_defs.h"
#include "tms/rt_fwk_dl_object_base.h"
#include <cpprest/json.h>

namespace RTFWK{
    class RTFWKDLDOStorageObjectBase;
}

TPS_BEGIN_NAMESPACE


class TpsGcaDmDeleteSeries: public RTFWK::RTFWKDLDOStorageObjectBase
{       
public:  
    TpsGcaDmDeleteSeries(std::string seriesUID):m_sSeriesUID(seriesUID){};
    ~TpsGcaDmDeleteSeries(){};

    virtual std::string GetDBUID() const {return m_sSeriesUID;};

    /////////////////////////////////////////////////////////////////
    // Save To DB by appserver
    /////////////////////////////////////////////////////////////////
    virtual bool DeleteEx()
    {
        bool bRet = true;
        try
        {
            bRet = DeleteByRest("GalleryService","DeleteSeries");
        }
        catch(...)
        {
            TPS_LOG_DEV_ERROR<<"TpsGcaDmDeleteSeries::SaveEx throw exception ";
            bRet = false;
        }
        return bRet;
    };         

private:
    std::string m_sSeriesUID;
};

TPS_END_NAMESPACE

#endif  //TPS_COMMON_IDATALOADER_H_
