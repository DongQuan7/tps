////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_datamodel_deleteslices.h
///  \brief   
///
///  \version 1.0
///
///  \date    Apr. 22, 2015
/////////////////////////////////////////////////////////////////
#ifndef TPS_GCA_DM_DELETESLICES_H_
#define TPS_GCA_DM_DELETESLICES_H_

#include "RtTpsGalleryControllerApp/tps_gca_defs.h"
#include "tms/rt_fwk_dl_object_base.h"
#include <cpprest/json.h>

namespace RTFWK{
    class RTFWKDLDOStorageObjectBase;
}

TPS_BEGIN_NAMESPACE


class TpsGcaDmDeleteSlices: public RTFWK::RTFWKDLDOStorageObjectBase
{       
public:  
    TpsGcaDmDeleteSlices(std::string seriesUID, std::vector<std::string> deletedImageUIDs, bool isSaveAsNewSeries);
    ~TpsGcaDmDeleteSlices();

protected:
    bool SerializeToJson(web::json::value& jsonValue);

    virtual bool SaveEx();

private:
    std::string              m_sSeriesUID;
    std::vector<std::string> m_vDeletedImageUIDs;
    bool                     m_bIsSaveAsNewSeries;
};

TPS_END_NAMESPACE

#endif  //TPS_COMMON_IDATALOADER_H_
