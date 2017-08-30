                                                     //////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_datamodel_series.cpp
///  \brief   
///
///  \version 1.0
///  \date    Apr. 03, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tps_gca_datamodel_series.h"   
#include <boost/serialization/list.hpp>

//TPS
#include "tps_logger.h"

//TMS
#include "tms/rt_fwk_utility.h"

TPS_BEGIN_NAMESPACE

TpsGcaDmSeries::TpsGcaDmSeries(std::string sSeries) : m_sSeriesUID(sSeries){

}

TpsGcaDmSeries::~TpsGcaDmSeries(void) {
}


bool TpsGcaDmSeries::DeSerializeFromJson(const web::json::value& jsonValue) {
    bool bRet = true;
    try
    {
        if (jsonValue.is_null()) {
            return false;
        }
        if (L"null" != jsonValue.to_string()){ 
            if (L"null" != jsonValue[L"ImageLists"].to_string()) {
                auto jsonImages = jsonValue[L"ImageLists"];
                for (auto iter = jsonImages.cbegin(); iter != jsonImages.cend(); ++iter){
                    const web::json::value& jsonImage = iter->second;
                    if (jsonImage.is_null()){
                        return false;
                    }
                    if (L"null" == jsonImage[L"ImageUID"].to_string() ||
                        L"null" == jsonImage[L"InstanceNumber"].to_string()) {
                            return false;
                    }
                    Image dmImage;
                    dmImage.m_sImageUID = RTFWK::ws2s(jsonImage[L"ImageUID"].as_string());
                    dmImage.m_iInstanceNumber = jsonImage[L"InstanceNumber"].as_integer();  
                    m_vImages.push_back(dmImage);
                }
            }              
        }
    }
    catch(web::json::json_exception ex)
    {
        TPS_LOG_DEV_ERROR<< "TpsGcaDmSeries::SerializeToJson throw json_exception " << std::string(ex.what());
        bRet = false;
    }
    catch(std::bad_cast ex)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmSeries::SerializeToJson throw std::bad_cast" << std::string(ex.what());
        bRet = false;
    }
    catch(std::exception ex)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmSeries::SerializeToJson throw std::exception "  << std::string(ex.what());
        bRet = false;
    }
    catch(...)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmSeries::SerializeToJson throw exception ";
        bRet = false;
    }
    return bRet;
}

std::vector<TpsGcaDmSeries::Image> TpsGcaDmSeries::GetImageLists() const{
    std::vector<TpsGcaDmSeries::Image> dmImages;
    for (auto iter = m_vImages.begin(); iter != m_vImages.end(); ++iter){
        TpsGcaDmSeries::Image dmImage;
        dmImage.m_sImageUID = iter->m_sImageUID;
        dmImage.m_iInstanceNumber = iter->m_iInstanceNumber;
        dmImages.push_back(dmImage);
    }
    return dmImages;
}

/////////////////////////////////////////////////////////////////
/// Save To DB by appserver
/////////////////////////////////////////////////////////////////
bool TpsGcaDmSeries::LoadEx() {  
    bool bRet = true;
    try
    {
        bRet = LoadByRest("GalleryService","GetSeries");
    }
    catch(...)
    {
        TPS_LOG_DEV_ERROR<<"TpsGcaDmSeries::SaveEx throw exception ";
        bRet = false;
    }
    return bRet;
}

TPS_END_NAMESPACE
