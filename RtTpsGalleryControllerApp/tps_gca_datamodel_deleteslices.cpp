//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_datamodel_deleteslices.cpp
///  \brief   
///
///  \version 1.0
///  \date    Apr. 22, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tps_gca_datamodel_deleteslices.h"   
#include <boost/serialization/list.hpp>

//TPS
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsGcaDmDeleteSlices::TpsGcaDmDeleteSlices(std::string seriesUID, std::vector<std::string> deletedImageUIDs, bool isSaveAsNewSeries) :
    m_sSeriesUID(seriesUID), m_bIsSaveAsNewSeries(isSaveAsNewSeries), 
    m_vDeletedImageUIDs(deletedImageUIDs.begin(), deletedImageUIDs.end()){
}

TpsGcaDmDeleteSlices::~TpsGcaDmDeleteSlices(void) {
}

bool TpsGcaDmDeleteSlices::SerializeToJson(web::json::value& jsonValue) {
    bool bRet = true;
    try
    {
        utility::string_t strSeriesUID(m_sSeriesUID.begin(), m_sSeriesUID.end());
        jsonValue[L"SeriesUID"] = web::json::value::string(strSeriesUID);

        jsonValue[L"IsSaveAsNewSeries"] = web::json::value::boolean(m_bIsSaveAsNewSeries);

        int iDeletedSlicesCount = static_cast<int>(m_vDeletedImageUIDs.size());
        web::json::value jsonDeletedImageUIDs = web::json::value::array(iDeletedSlicesCount);
        for (int i=0; i<iDeletedSlicesCount; i++)
        {
            utility::string_t strSeriesUID(m_vDeletedImageUIDs[i].begin(), m_vDeletedImageUIDs[i].end());
            jsonDeletedImageUIDs[i] = web::json::value::string(strSeriesUID);
        }
        jsonValue[L"ImageUIDs"] = jsonDeletedImageUIDs;

    }
    catch(web::json::json_exception ex)
    {
        TPS_LOG_DEV_ERROR<< "TpsGcaDmDeleteSlices::SerializeToJson throw json_exception " << std::string(ex.what());
        bRet = false;
    }
    catch(std::bad_cast ex)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmDeleteSlices::SerializeToJson throw std::bad_cast" << std::string(ex.what());
        bRet = false;
    }
    catch(std::exception ex)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmDeleteSlices::SerializeToJson throw std::exception "  << std::string(ex.what());
        bRet = false;
    }
    catch(...)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmDeleteSlices::SerializeToJson throw exception ";
        bRet = false;
    }
    return bRet;
}

/////////////////////////////////////////////////////////////////
/// Save To DB by appserver
/////////////////////////////////////////////////////////////////
bool TpsGcaDmDeleteSlices::SaveEx() {  
    bool bRet = true;
    try
    {
        bRet = SaveByRest("GalleryService","DeleteSlices");
    }
    catch(...)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmDeleteSlices::SaveEx throw exception ";
        bRet = false;
    }
    return bRet;
}


TPS_END_NAMESPACE
