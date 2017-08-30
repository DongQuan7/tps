////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_datamodel_series.h
///  \brief   
///
///  \version 1.0
///
///  \date    Apr. 22, 2015
/////////////////////////////////////////////////////////////////
#ifndef TPS_GCA_DM_SERIES_H_
#define TPS_GCA_DM_SERIES_H_

#include "RtTpsGalleryControllerApp/tps_gca_defs.h"
#include "tms/rt_fwk_dl_object_base.h"
#include <cpprest/json.h>

namespace RTFWK{
    class RTFWKDLDOStorageObjectBase;
}

TPS_BEGIN_NAMESPACE


class TpsGcaDmSeries: public RTFWK::RTFWKDLDOStorageObjectBase
{  
public:
    struct Image{
    public:
       std::string m_sImageUID;
       int         m_iInstanceNumber;
    }; 

public:  
    TpsGcaDmSeries(std::string sSeriesUID);
    ~TpsGcaDmSeries();

    std::vector<Image> GetImageLists() const;

protected:
    virtual bool DeSerializeFromJson(const web::json::value& jsonValue);

    virtual std::string GetDBUID() const {return m_sSeriesUID;}

    virtual bool LoadEx();       

private:

    std::vector<Image> m_vImages;
    std::string        m_sSeriesUID;
};

TPS_END_NAMESPACE

#endif  //TPS_COMMON_IDATALOADER_H_
