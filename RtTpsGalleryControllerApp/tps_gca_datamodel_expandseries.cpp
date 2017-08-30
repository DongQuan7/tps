//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_datamodel_editcourse.cpp
///  \brief   
///
///  \version 1.0
///  \date    Apr. 03, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tps_gca_datamodel_expandseries.h"   
#include <boost/serialization/list.hpp>

//TPS
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsGcaDmExpandSeries::TpsGcaDmExpandSeries(std::string seriesUID, int sliceNumber, bool isExpandBottom, bool isSaveAsNewSeries) :
    m_sSeriesUID(seriesUID), m_iSliceNumber(sliceNumber), 
    m_bIsExpandBottom(isExpandBottom), m_bIsSaveAsNewSeries(isSaveAsNewSeries){
}

TpsGcaDmExpandSeries::~TpsGcaDmExpandSeries(void) {
}

bool TpsGcaDmExpandSeries::SerializeToJson(web::json::value& jsonValue) {
    bool bRet = true;
    try
    {
        utility::string_t strSeriesUID(m_sSeriesUID.begin(), m_sSeriesUID.end());
        jsonValue[L"SeriesUID"] = web::json::value::string(strSeriesUID);

        jsonValue[L"SliceNumber"] = web::json::value::number(m_iSliceNumber);

        jsonValue[L"IsExpandBottom"] = web::json::value::boolean(m_bIsExpandBottom);

        jsonValue[L"IsSaveAsNewSeries"] = web::json::value::boolean(m_bIsSaveAsNewSeries);

    }
    catch(web::json::json_exception ex)
    {
        TPS_LOG_DEV_ERROR<< "TpsGcaDmExpandSeries::SerializeToJson throw json_exception " << std::string(ex.what());
        bRet = false;
    }
    catch(std::bad_cast ex)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmExpandSeries::SerializeToJson throw std::bad_cast" << std::string(ex.what());
        bRet = false;
    }
    catch(std::exception ex)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmExpandSeries::SerializeToJson throw std::exception "  << std::string(ex.what());
        bRet = false;
    }
    catch(...)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmExpandSeries::SerializeToJson throw exception ";
        bRet = false;
    }
    return bRet;
}

/////////////////////////////////////////////////////////////////
/// Save To DB by appserver
/////////////////////////////////////////////////////////////////
bool TpsGcaDmExpandSeries::SaveEx() {  
    bool bRet = true;
    try
    {
        bRet = SaveByRest("GalleryService","ExpandSeries");
    }
    catch(...)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmExpandSeries::SaveEx throw exception ";
        bRet = false;
    }
    return bRet;
}


TPS_END_NAMESPACE
