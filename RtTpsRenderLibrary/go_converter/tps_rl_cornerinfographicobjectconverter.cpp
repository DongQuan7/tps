////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Xi.Chen xi.chen@united-imaging.com
/// 
///  \file tps_rl_cornerinfographicobjectconverter.h
/// 
///  \brief class TpsCornerInfoGOConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/06/23
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_cornerinfographicobjectconverter.h"

// Boost
#include "boost/date_time/gregorian/formatters.hpp"
#include <boost/date_time/posix_time/posix_time.hpp> 


//ZHENGHE
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_suv_calculator.h"

//TPS
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"

TPS_BEGIN_NAMESPACE

TpsCornerInfoGOConverter::TpsCornerInfoGOConverter(GO_TYPE goType) 
    : TpsGOConverterBase(goType), mVolume(nullptr), mMpr(nullptr), mHeader(nullptr){

}

TpsCornerInfoGOConverter::~TpsCornerInfoGOConverter() {

}

GraphicObjectPtr TpsCornerInfoGOConverter::CreateGraphicObject_i(){

    return std::shared_ptr<TpsTextOverlayGraphicObject>(
        new TpsTextOverlayGraphicObject());
}

bool TpsCornerInfoGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    mVolume = nullptr; mMpr = nullptr; mHeader = nullptr;

    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end())
    {
        TPS_LOG_DEV_ERROR<<"Can not find the IMAGE data source.";
        return false;
    }
    std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> spGO = nullptr;

    //TPS_GO_MPR_CORNERINFO_CORONAL
    auto sectionType = GOTypeHelper::GetFirstPostfixComponent(mGOType);

    // Volume GO
    spGO = mModelWarehouse->GetModelObject(
        it->second + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME));
    if ((mVolume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(spGO)) == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get Volume GO: "<<it->second;
        return false;
    }

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, sectionType);
    spGO = mModelWarehouse->GetModelObject(
        it->second + "|" + GOTypeHelper::ToString(goType));
    if ((mMpr = std::dynamic_pointer_cast<MPRGraphicObject>(spGO)) == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get MPR GO: "<<it->second;
        return false;
    }

    mSeries = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImageSeries(it->second, &mSeries))
    {
        TPS_LOG_DEV_WARNING<<"Failed to get series which uid: "<<it->second;
    }

    if (!mDataRepository->GetImageDataManager()->GetImage3DHeader(it->second, mHeader)){
        TPS_LOG_DEV_WARNING<<"Failed to get image 3d header of series: "<<it->second;
    }
    return true;
}

bool TpsCornerInfoGOConverter::Update_i(GraphicObjectPtr go){
    auto textGo = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(go);
    if (mMpr == nullptr || mVolume == nullptr || textGo == nullptr){
        return false;
    }

    const int ckeynum = 5;
    GO_TYPE section_page_info[ckeynum] = {
        GO_TYPE_MPR_CORNERINFO_CORONAL,
        GO_TYPE_MPR_CORNERINFO_SAGITTAL,
        GO_TYPE_MPR_CORNERINFO_AXIAL,
        GO_TYPE_MPR_CORNERINFO_AXIAL_PREV,
        GO_TYPE_MPR_CORNERINFO_AXIAL_POST,
    };
    std::string mpr_slicepos_key_val[ckeynum] = {
        SLICEPOSITION_CORONAL_SECTION_VAL, 
        SLICEPOSITION_SAGITTAL_SECTION_VAL, 
        SLICEPOSITION_AXIAL_SECTION_VAL, 
        SLICEPOSITION_AXIAL_SECTION_PREV_VAL, 
        SLICEPOSITION_AXIAL_SECTION_POST_VAL};
    std::string mpr_slicetotal_key_val[ckeynum] = {
        SLICEPOSITION_TOTAL_CORONAL_SECTION_VAL, 
        SLICEPOSITION_TOTAL_SAGITTAL_SECTION_VAL, 
        SLICEPOSITION_TOTAL_AXIAL_SECTION_VAL, 
        SLICEPOSITION_TOTAL_AXIAL_SECTION_PREV_VAL, 
        SLICEPOSITION_TOTAL_AXIAL_SECTION_POST_VAL};
    std::string mpr_thickness_key_val[ckeynum] = {
        THICKNESS_CORONAL_SECTION_VAL, 
        THICKNESS_SAGITTAL_SECTION_VAL, 
        THICKNESS_AXIAL_SECTION_VAL, 
        THICKNESS_AXIAL_SECTION_PREV_VAL, 
        THICKNESS_AXIAL_SECTION_POST_VAL};

    int k =0;
    for(; k<ckeynum; ++k){
        if(section_page_info[k] == mGOType)
            break;
    }

    if(k == ckeynum ) {
        TPS_LOG_DEV_WARNING<<"Cannot find the section!";
        return false;
    }

    std::wstring wstrSeriesDesp, wstrSeriesDate;
    std::string seriesModality, seriesDesp, seriesDate;

    seriesDesp = mSeries->get_seriesdescription();
    seriesDesp = seriesDesp.empty() ? " " : seriesDesp;
    wstrSeriesDesp.assign(seriesDesp.begin(), seriesDesp.end());

    // get seried date and time combined string 
    std::locale fmt(std::locale::classic(), new boost::gregorian::date_facet("%H:%M:%S"));
    std::stringstream ss;
    ss.imbue(fmt);
    ss<< boost::gregorian::to_iso_extended_string(mSeries->get_seriesdate());
    ss<< " ";
    ss<< boost::posix_time::to_simple_string(mSeries->get_seriestime());
    seriesDate = ss.str();
    if (seriesDate.empty())
    {
        seriesDate = "ERROR: SERIES NOT EXIST!";
    }
    wstrSeriesDate.assign(seriesDate.begin(), seriesDate.end());


    std::wstring wplanName;
    std::string planName = " ";  //Plan name will be set in mprprinter.cpp
    wplanName.assign(planName.begin(), planName.end());

    std::vector<std::vector<std::pair<std::string, std::wstring>>> cornerTextKey;
    std::vector<std::pair<std::string, std::wstring>> textLine;
    int lt_num = 0, rb_num = 0;

    // Left-Top Corner

    // Line 1: Plan Name
    textLine.push_back(std::make_pair("PlanName", wplanName));
    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++lt_num;

    // Line 2: Series Description
    textLine.push_back(std::make_pair("SerDesp", wstrSeriesDesp));
    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++lt_num;

    // Line 3: Series Date
    textLine.push_back(std::make_pair("SerDate",wstrSeriesDate));
    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++lt_num;

    // Line 4: Patient Position
    std::string patpos = mSeries->get_patientposition();
    std::wstring wpp;
    wpp.assign(patpos.begin(), patpos.end());
    textLine.push_back(std::make_pair("PatientPosition", wpp));
    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++lt_num;


    // Line 5: CT: CTVal Density: DensityVal PET SUV: PETVal
    if(seriesDesp.empty()|| seriesModality == "CT"){
        textLine.push_back(std::make_pair("CT", L"CT:"));
        textLine.push_back(std::make_pair("CTVal", L"0.0"));
        textLine.push_back(std::make_pair("CTUnit", L"HU"));

        textLine.push_back(std::make_pair("Density", L"Density:"));
        textLine.push_back(std::make_pair("DensityVal", L"0.0 "));
        textLine.push_back(std::make_pair("gcm3", L"g/cm^3"));
    }else if(seriesModality == "PT"){
        textLine.push_back(std::make_pair("SUV", L"SUV:"));
        textLine.push_back(std::make_pair("PETVal", L"0.0"));
        //textLine.push_back(std::make_pair("PETUnit", L"PET"));
    }else if(seriesModality == "MR"){
        //textLine.push_back(std::make_pair("GREY", L"GREY:"));
        //textLine.push_back(std::make_pair("MRVal", L"0.0"));
        //textLine.push_back(std::make_pair("MRUnit", L"MR"));
    }

    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++lt_num;

    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++lt_num;

    // Right-Bottom Corner
    // Line 1: WW WL
    if(seriesModality == "PT"){
        textLine.push_back(std::make_pair("WW", L"T:"));
        textLine.push_back(std::make_pair("WWVal", L"10.0 "));
        textLine.push_back(std::make_pair("WL", L"B:"));
        textLine.push_back(std::make_pair("WLVal", L"0.0 "));
    }else{
        textLine.push_back(std::make_pair("WW", L"WW:"));
        textLine.push_back(std::make_pair("WWVal", L"   0"));
        textLine.push_back(std::make_pair("WL", L"WL:"));
        textLine.push_back(std::make_pair("WLVal", L"   0"));
    }

    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++rb_num;

    // Line 2: Slice Position
    textLine.push_back(std::make_pair("Slice", L"Slice No:"));
    textLine.push_back(std::make_pair(mpr_slicepos_key_val[k], L"  0"));
    textLine.push_back(std::make_pair("Slash", L"/"));
    textLine.push_back(std::make_pair(mpr_slicetotal_key_val[k], L"  0"));

    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++rb_num;

    // Line 3: Slice Thickness
    textLine.push_back(std::make_pair("Thickness", L"Thickness:"));
    std::string sliceThickness = mSeries->get_slicethickness();
    std::wstring wstrSlickThickness;
    sliceThickness = sliceThickness.empty() ? "0.0" : sliceThickness;
    wstrSlickThickness.assign(sliceThickness.begin(), sliceThickness.end());
    textLine.push_back(std::make_pair(mpr_thickness_key_val[k], wstrSlickThickness));
    textLine.push_back(std::make_pair("CM3", L"cm"));


    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++rb_num;

    FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
    Pixel32* pStrBuffer = nullptr;
    int strHeight = 0;
    int strWidth = 0;

    HorizontalAlignment ha = ALIGN_LEFT;
    VerticalAlignment va = ALIGN_TOP;
    int top = 0, bottom = 0;
    for (int i = 0; i< lt_num; ++i){
        int left = 0;
        for (int j = 0; j < cornerTextKey[i].size(); ++j){
            std::wstring nam = cornerTextKey[i][j].second;
            if(!pBufGen->GenerateRasterBuffer(cornerTextKey[i][j].second, &pStrBuffer, strWidth, strHeight)){
                TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for"<<cornerTextKey[i][j].first;
                return false;
            }
            TextBlock textBlock(cornerTextKey[i][j].first, left, top, 0, 0, (unsigned char*)pStrBuffer, strWidth, strHeight, ha, va);
            textGo->AddTextBlock(cornerTextKey[i][j].first, &textBlock);

            if(pStrBuffer != nullptr){
                delete[] pStrBuffer;
                pStrBuffer = nullptr;
            }

            // below is UGLY code in order to fit the loop structure
            if(i == 5){                     // line 6: Mouse Position In patient Coordinate
                left += strWidth;
                if(j == 2 || j == 5){
                    left += 5;
                }
            }else if(i == 4){               // line 5: CT and Density
                left += strWidth ;

                if(j == 1){                 // CT:
                    left += 30;
                }else if(j == 2){          // Density:
                    left += 15;
                }
            }else{                          // other lines
                left += strWidth + 5;
            }
        }
        top += strHeight + 2;
    }

    // Right-Bottom Corner
    ha = ALIGH_RIGHT;
    va = ALIGN_BOTTOM;
    bottom = 0;
    for (int i = cornerTextKey.size() - 1; i > cornerTextKey.size() - rb_num - 1; --i){
        int right = 0;
        for(int j = cornerTextKey[i].size() - 1; j >= 0; --j ){
            if (!pBufGen->GenerateRasterBuffer(cornerTextKey[i][j].second, &pStrBuffer, strWidth, strHeight)){
                TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for slice.\n";
                return false;
            }
            TextBlock textBlock(cornerTextKey[i][j].first, 0, 0, right, bottom, (unsigned char*)pStrBuffer, strWidth, strHeight, ha, va);
            textGo->AddTextBlock(cornerTextKey[i][j].first, &textBlock);
            if(pStrBuffer != nullptr){
                delete[] pStrBuffer;
                pStrBuffer = nullptr;
            }
            right += strWidth + 2;
        }
        bottom += strHeight + 2;
    }

    // -----------------------------------------------------------------------------------------------
    // Update Window Width and Window Level(Center)
    // -----------------------------------------------------------------------------------------------
    if(mVolume->GetModality() != "PT"){
        std::wstringstream wss_wwT(L""), wss_wlB(L"");
        int ww = 0, wl = 0;
        if (mHeader != nullptr) {
            ww = mHeader->m_iWindowWidth;
            wl = mHeader->m_iWindowCenter;
        } else {
            TPS_LOG_DEV_WARNING<<"Failed to GetImage3DHeader. series uid["<<mIndexSeriesPair.second->get_uid()<<"].";
        }
        wss_wwT << ww;
        wss_wlB << wl;

        if(!pBufGen->GenerateRasterBuffer(wss_wwT.str(), &pStrBuffer, strWidth, strHeight)){
            TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for Window Width.\n"; return false;
        }
        textGo->UpdateTextBlock("WWVal", (unsigned char*)pStrBuffer, strWidth, strHeight);
        delete[] pStrBuffer;

        if(!pBufGen->GenerateRasterBuffer(wss_wlB.str(), &pStrBuffer, strWidth, strHeight)){
            TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for Window Level(Center).\n"; return false;
        }
        textGo->UpdateTextBlock("WLVal", (unsigned char*)pStrBuffer, strWidth, strHeight);
        delete[] pStrBuffer;
    }

    return true;
}

TPS_END_NAMESPACE
