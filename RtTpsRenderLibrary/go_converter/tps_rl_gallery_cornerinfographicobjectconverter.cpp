//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rl_gallery_crosshair_graphicobjectconverter.cpp
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_gallery_cornerinfographicobjectconverter.h"

//zhenghe
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_suv_calculator.h" 

#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"


//gallery
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"
#include "RtTpsDataAccess/tps_da_gallery_patientmanager.h"

// Boost
#include "boost/date_time/gregorian/formatters.hpp" 
#include "boost/date_time/posix_time/posix_time.hpp" 

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"


TPS_BEGIN_NAMESPACE

TpsGalleryCornerInfoGOConverter::TpsGalleryCornerInfoGOConverter(GO_TYPE goType) 
    : TpsGOConverterBase(goType), mVolume(nullptr), mMpr(nullptr), mSeries(nullptr), mHeader(nullptr){

}

TpsGalleryCornerInfoGOConverter::~TpsGalleryCornerInfoGOConverter() {

}

GraphicObjectPtr TpsGalleryCornerInfoGOConverter::CreateGraphicObject_i(){

    return std::shared_ptr<TpsTextOverlayGraphicObject>(
        new TpsTextOverlayGraphicObject());
}

bool TpsGalleryCornerInfoGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    mVolume = nullptr; 
    mMpr = nullptr; 
    mSeries = nullptr; 
    mHeader = nullptr;

    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;

    std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> spGO = nullptr;

    //TPS_GO_MPR_CORNERINFO_CORONAL
    auto sectionType = GOTypeHelper::GetFirstPostfixComponent(mGOType);

    // Volume GO
    spGO = mModelWarehouse->GetModelObject(
        it->second + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME));
    if ((mVolume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(spGO)) == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get Volume GO: "<<it->second;
        return false;
    }

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, sectionType);
    spGO = mModelWarehouse->GetModelObject(
        it->second + "|" + GOTypeHelper::ToString(goType));
    if ((mMpr = std::dynamic_pointer_cast<MPRGraphicObject>(spGO)) == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get MPR GO: "<<it->second;
        return false;
    }

    if(!mDataRepository->GetGalleryImageManager()->GetImageSeries(it->second, &mSeries)){
        TPS_LOG_DEV_WARNING<<"Failed to get series which uid: "<<it->second;
    }

    if (!mDataRepository->GetGalleryImageManager()->GetImage3DHeader(it->second, mHeader)){
        TPS_LOG_DEV_WARNING<<"Failed to get image 3d header of series: "<<it->second;
    }
    return true;
}

bool TpsGalleryCornerInfoGOConverter::Update_i(GraphicObjectPtr go){
    auto textGo = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(go);
    if (mMpr == nullptr || mVolume == nullptr || textGo == nullptr){
        return false;
    }

    //// TODO : Update IsVisible
    //bool isVisible = TpsGalleryImageManager::GetInstance()->GetIsVisibleOfImageText();
    //textGo->SetVisible(isVisible);

    //TPS_GO_MPR_CORNERINFO_CORONAL
    auto sectionType = GOTypeHelper::GetFirstPostfixComponent(mGOType);

    const int ckeynum = 3;
    GO_TYPE section_page_info[ckeynum] = {
        GO_TYPE_GALLERY_MPR_CORNERINFO_CORONAL,
        GO_TYPE_GALLERY_MPR_CORNERINFO_SAGITTAL,
        GO_TYPE_GALLERY_MPR_CORNERINFO_AXIAL,
    };
    std::string mpr_slicepos_key_val[ckeynum] = {
        SLICEPOSITION_CORONAL_SECTION_VAL, 
        SLICEPOSITION_SAGITTAL_SECTION_VAL, 
        SLICEPOSITION_AXIAL_SECTION_VAL,
    };
    std::string mpr_slicetotal_key_val[ckeynum] = {
        SLICEPOSITION_TOTAL_CORONAL_SECTION_VAL, 
        SLICEPOSITION_TOTAL_SAGITTAL_SECTION_VAL, 
        SLICEPOSITION_TOTAL_AXIAL_SECTION_VAL, 
    };
    std::string mpr_thickness_key_val[ckeynum] = {
        THICKNESS_CORONAL_SECTION_VAL, 
        THICKNESS_SAGITTAL_SECTION_VAL, 
        THICKNESS_AXIAL_SECTION_VAL,
    };

    int k =0;
    for(; k<ckeynum; ++k){
        if(section_page_info[k] == mGOType)
            break;
    }

    if(k == ckeynum ) {
        TPS_LOG_DEV_WARNING<<"Cannot find the section!";
        return false;
    }

    std::wstring wstrSeriesDesp, wstrSeriesDate, wstrPatientPosition, wstrModality;
    std::string seriesModality, seriesDesp, seriesDateTime, patientPosition;
    if(mSeries != nullptr){
        seriesDesp = mSeries->get_seriesdescription();
        seriesModality = mSeries->get_modality();
        patientPosition = mSeries->get_patientposition();
        if (mSeries->get_seriesdate().is_not_a_date() || mSeries->get_seriestime().is_not_a_date_time()) {
            seriesDateTime = "-";
        }
        else {
            // TODO Failed
            //boost::posix_time::ptime dateTime(mSeries->GetSeriesDate(), mSeries->GetSeriesTime());
            //std::locale fmt(std::locale::classic(), new boost::gregorian::date_facet("%Y%m%d %H:%M:%S"));
            //std::ostringstream os;
            //os.imbue(fmt);
            //os << dateTime;
            //seriesDateTime = os.str();  
            
            std::locale fmt(std::locale::classic(), new boost::gregorian::date_facet("%H:%M:%S"));
            std::stringstream ss;   
            ss.imbue(fmt);
            ss<< boost::gregorian::to_iso_string(mSeries->get_seriesdate());
            ss<< " ";
            ss << mSeries->get_seriestime();
            seriesDateTime = ss.str();  // ss>>seriesDateTime  : Get Unexpected Value
        }
        seriesDesp = seriesDesp.empty() ? "CT" : seriesDesp;
        wstrSeriesDesp.assign(seriesDesp.begin(), seriesDesp.end());
        wstrSeriesDate.assign(seriesDateTime.begin(), seriesDateTime.end());
        wstrPatientPosition.assign(patientPosition.begin(), patientPosition.end());
        wstrModality.assign(seriesModality.begin(), seriesModality.end());
    }else{
        TPS_LOG_DEV_ERROR<<"Failed to GetImageSeries";
        wstrSeriesDesp = L"Series Not Found!";
        wstrSeriesDate = L"Series Not Found!";
    }


    std::vector<std::vector<std::pair<std::string, std::wstring>>> cornerTextKey;
    std::vector<std::pair<std::string, std::wstring>> textLine;
    int lt_num = 0, lb_num = 0, rb_num = 0, rt_num=0;

    //// Left-Top Corner
    //// Line 1: PatientName
    //std::string patientName = mDataRepository->GetGalleryPatientManager()->GetCurrentPatientName();
    //std::wstring wstrPatientName(patientName.begin(), patientName.end());
    //textLine.push_back(std::make_pair("PatNa", wstrPatientName));
    //cornerTextKey.push_back(textLine);
    //textLine.clear();
    //std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    //++lt_num;

    // Line 2: Series Description or Series Name
    textLine.push_back(std::make_pair("SerDesp", wstrSeriesDesp));
    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++lt_num;

    // Line 3: Series DateTime
    textLine.push_back(std::make_pair("SerDateTime",wstrSeriesDate));
    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++lt_num;

    // Line 4: PatientPosition
    textLine.push_back(std::make_pair("PatientPosition",wstrPatientPosition));
    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++lt_num;      

    // Line 5: CT: CTVal Density: DensityVal PET SUV: PETVal
    if(seriesModality.empty()|| seriesModality == "CT"){
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
        textLine.push_back(std::make_pair("MREmptyLine", L"N.A."));
        //textLine.push_back(std::make_pair("GREY", L"GREY:"));
        //textLine.push_back(std::make_pair("MRVal", L"0.0"));
        //textLine.push_back(std::make_pair("MRUnit", L"MR"));
    }

    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++lt_num;

    // Line 6: Mouse Position: XVal YVal ZVal
    textLine.push_back(std::make_pair("X", L"X:"));
    textLine.push_back(std::make_pair("PositionXVal", L"  0.0 "));
    textLine.push_back(std::make_pair("CM", L"cm"));

    textLine.push_back(std::make_pair("Y", L"Y:"));
    textLine.push_back(std::make_pair("PositionYVal", L"  0.0 "));
    textLine.push_back(std::make_pair("CM1", L"cm"));

    textLine.push_back(std::make_pair("Z", L"Z:"));
    textLine.push_back(std::make_pair("PositionZVal", L"  0.0 "));
    textLine.push_back(std::make_pair("CM2", L"cm"));

    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++lt_num;

    // Left-Bottom Corner
    // Line 1: MPR name
    textLine.push_back(std::make_pair(sectionType == CORONAL ? "Coronal" : (
        sectionType == SAGITTAL ? "Sagittal" : "Axial"), 
        k == 0 ? L"Coronal":(k == 1 ? L"Sagittal" : L"Axial")));
    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++lb_num;

    // Rigth-Top Corner
    // Line 1: Modality
    textLine.push_back(std::make_pair("Modality", wstrModality));
    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++rt_num;

    // Line 2: Zoom Factor
    textLine.push_back(std::make_pair("Zoom", L"Zoom:"));
    textLine.push_back(std::make_pair("ZoomFactor", L" 1.00"));
    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++rt_num;

    // Line 3: Rotate Angle
    textLine.push_back(std::make_pair("RotateAngle", L"Rotate:"));
    textLine.push_back(std::make_pair("RotateAngleValue", L" 0.0 "));
    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++rt_num;

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
    textLine.push_back(std::make_pair(mpr_thickness_key_val[k], L"0.0"));
    textLine.push_back(std::make_pair("CM3", L"cm"));  

    cornerTextKey.push_back(textLine);
    textLine.clear();
    std::vector<std::pair<std::string, std::wstring>>().swap(textLine);
    ++rb_num;

    FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
    Pixel32* pStrBuffer = nullptr;
    int strHeight = 0;
    int strWidth = 0;

    // Left-Top Corner
    // Series Description
    // Date
    // CT: -999.0 Density: 0.00 g/cm^3
    // x: 0.0cm y: 0.0cm z:0.0cm
    // Dose: 0 cGy

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

    // TODO not use 
    //// Left-Bottom Corner
    //ha = ALIGN_LEFT;
    //va = ALIGN_BOTTOM;
    //for (int i = lt_num; i < lt_num + lb_num; ++i){
    //    int left = 0;
    //    for (int j = cornerTextKey[i].size() == 1 ? 0:1; j >= 0; --j){

    //        if(!pBufGen->GenerateRasterBuffer(cornerTextKey[i][j].second, &pStrBuffer, strWidth, strHeight)){
    //            TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for"<<cornerTextKey[i][j].first;
    //            return false;
    //        }

    //        TextBlock textBlock(cornerTextKey[i][j].first, left, 0, 0, bottom, (unsigned char*)pStrBuffer, strWidth, strHeight, ha, va);
    //        textGo->AddTextBlock(cornerTextKey[i][j].first, &textBlock);

    //        if(pStrBuffer != nullptr){
    //            delete[] pStrBuffer;
    //            pStrBuffer = nullptr;
    //        }
    //    }
    //    bottom += strHeight + 2;
    //}

    // Right-Top Corner
    ha = ALIGH_RIGHT;
    va = ALIGN_TOP;
    top = 25;
    for (int i = lt_num + lb_num; i < lt_num + lb_num + rt_num; i++){
        int right = 0; 
        for(int j = cornerTextKey[i].size() - 1; j >= 0; --j ){
            if (!pBufGen->GenerateRasterBuffer(cornerTextKey[i][j].second, &pStrBuffer, strWidth, strHeight)){
                TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for slice.\n";
                return false;
            }
            TextBlock textBlock(cornerTextKey[i][j].first, 0, top, right, 0, (unsigned char*)pStrBuffer, strWidth, strHeight, ha, va);
            textGo->AddTextBlock(cornerTextKey[i][j].first, &textBlock);
            if(pStrBuffer != nullptr){
                delete[] pStrBuffer;
                pStrBuffer = nullptr;
            }
            right += strWidth + 2;
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
    // Update cursor position in patient coordinate
    // -----------------------------------------------------------------------------------------------
    std::wstringstream wss_pos(L"");
    double posInPatient;
    std::string posKey;
    GetMousePosition(mMpr, mVolume, &posInPatient, posKey);
    char strtext_pos[64]={0};
    _snprintf_s(strtext_pos, sizeof(strtext_pos), "%.1f", 0.1 * posInPatient);
    if(strtext_pos[sizeof(strtext_pos) - 1] != '\0'){
        //printf("warning:string will be truncated");
        strtext_pos[sizeof(strtext_pos)-1]='\0';
    }

    wss_pos<<strtext_pos;
    if(!pBufGen->GenerateRasterBuffer(wss_pos.str(), &pStrBuffer, strWidth, strHeight)){
        TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for Window Width.\n"; 
        delete[] pStrBuffer;
        return false;
    }
    textGo->UpdateTextBlock(posKey, (unsigned char*)pStrBuffer, strWidth, strHeight);
    delete[] pStrBuffer;

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
            TPS_LOG_DEV_WARNING<<"Failed to GetImage3DHeader. series uid["<<mSeries->get_uid()<<"].";
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

    // -----------------------------------------------------------------------------------------------
    // Update Slice Position and Thickness Value
    // -----------------------------------------------------------------------------------------------
    double slicepos, thickness;
    size_t slicetotal;

    GetSlicePosAndThickness(mMpr, mVolume, &slicepos, &slicetotal, &thickness);

    std::vector<std::string> slice_thickness_key_val;
    slice_thickness_key_val.push_back(mpr_slicepos_key_val[k]);
    slice_thickness_key_val.push_back(mpr_slicetotal_key_val[k]);
    slice_thickness_key_val.push_back(mpr_thickness_key_val[k]);

    std::vector<std::string> slice_thickness_key;
    slice_thickness_key.push_back("Slice");
    slice_thickness_key.push_back("Thickness");

    std::ostringstream oss_pos;
    oss_pos << slicepos;
    std::string str_pos = oss_pos.str();
    std::wstring wstr_pos;
    wstr_pos.assign(str_pos.begin(), str_pos.end());

    char strtext_slicetotal[64]={0};
    _snprintf_s(strtext_slicetotal, sizeof(strtext_slicetotal), "%d", (int)slicetotal);
    if(strtext_slicetotal[sizeof(strtext_slicetotal) - 1] != '\0'){
        //printf("warning:string will be truncated");
        strtext_slicetotal[sizeof(strtext_slicetotal)-1]='\0';
    }

    char strtext_thickness[64]={0};
    _snprintf_s(strtext_thickness, sizeof(strtext_thickness), "%.1f", thickness);
    if(strtext_thickness[sizeof(strtext_thickness) - 1] != '\0'){
        //printf("warning:string will be truncated");
        strtext_thickness[sizeof(strtext_thickness)-1]='\0';
    }

    for(int i = 0; i<slice_thickness_key_val.size(); ++i){
        std::wstringstream wss;
        if(i == 0){
            wss << wstr_pos;
        }else{
            wss << (i == 1 ? strtext_slicetotal : strtext_thickness);
        }
        if(!pBufGen->GenerateRasterBuffer(wss.str(), &pStrBuffer, strWidth, strHeight)){
            TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for slice.\n";
            return false;
        }
        textGo->UpdateTextBlock(slice_thickness_key_val[i], (unsigned char*)pStrBuffer, strWidth, strHeight);

        if(pStrBuffer != nullptr){
            delete[] pStrBuffer;
            pStrBuffer = nullptr;
        }
    }

    return true;
}

void TpsGalleryCornerInfoGOConverter::GetMousePosition(std::shared_ptr<MPRGraphicObject> mpr, 
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volume, double *pos, std::string &posKey){

    Mcsf::MedViewer3D::Point3D vertices[4];
    mpr->GetVertices(vertices);
    Mcsf::MedViewer3D::Point3D ptInPatient = volume->GetWorld2Patient().Transform(vertices[0]);

    auto sectionType = GOTypeHelper::GetFirstPostfixComponent(mGOType);
    if(sectionType == CORONAL){
        *pos = ptInPatient.y;
        posKey = "PositionYVal";
    }else if(sectionType == SAGITTAL){
        *pos = ptInPatient.x;
        posKey = "PositionXVal";
    }else if(sectionType == AXIAL){
        *pos = ptInPatient.z;
        posKey = "PositionZVal";
    }
}

void TpsGalleryCornerInfoGOConverter::GetSlicePosAndThickness(
    std::shared_ptr<MPRGraphicObject> mpr,
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volume, 
    double* slicepos, size_t* slicetotal, double* thickness){

    // ----------------------------------------------------------------------------
    // 1. get slice thickness
    // ----------------------------------------------------------------------------
    *thickness = mpr->GetThickness();
    Mcsf::MedViewer3D::Matrix4x4 matW2P = volume->GetWorld2Patient();
    Mcsf::MedViewer3D::Vector3D vectorForThickness(0.0, 0.0, *thickness);
    *thickness = matW2P.Transform(vectorForThickness).z / 10;

    // ----------------------------------------------------------------------------
    // 2. get slice position: in X, Y, Z orientation respectively
    // ----------------------------------------------------------------------------
    size_t dim[3];
    volume->GetDimension(dim);
    Mcsf::MedViewer3D::Matrix4x4 modelToVolume(
        dim[0],        0,      0, 0, 
             0,   dim[1],      0, 0, 
             0,        0, dim[2], 0, 
          -0.5,     -0.5,   -0.5, 1);

    Mcsf::MedViewer3D::Point3D vertices[4];
    mpr->GetVertices(vertices);
    Mcsf::MedViewer3D::Vector3D vecNormal;
    mpr->GetNormal(&vecNormal);

    Mcsf::MedViewer3D::Matrix4x4 matWorld2Volume = modelToVolume * volume->GetModel2World().Inverse();

    Mcsf::MedViewer3D::Point3D ptInVolume = matWorld2Volume.Transform(vertices[0]);
    vecNormal = matWorld2Volume.Transform(vecNormal);
    vecNormal.Normalize();

    //notice that: this method does not apply to oblique MPR case
    if (abs(abs(vecNormal.x) - 1) <= 1e-6){
        *slicetotal = dim[0];
        *slicepos = vecNormal.x > 0 ? (ptInVolume.x  + 1) : (dim[0] - ptInVolume.x);
    }
    else if (abs(abs(vecNormal.y) - 1) <= 1e-6){
        *slicetotal = dim[1];
        *slicepos = vecNormal.y > 0 ? (ptInVolume.y  + 1) : (dim[1] - ptInVolume.y);
    }
    else if (abs(abs(vecNormal.z) - 1) <= 1e-6){ // Axial
        *slicetotal = dim[2];
        *slicepos = vecNormal.z > 0 ? (ptInVolume.z  + 1) : (dim[2] - ptInVolume.z);
    }
    else {
        TPS_LOG_DEV_WARNING<<"Can not calculate slice number of oblique mpr!";
        *slicepos = -1;
    }

    if(volume->GetModality() == "PT"){
        mDataRepository->GetGalleryImageManager()->SetCurrentDataHeaderElement(*slicepos);
        float slope = mVolume->GetSlope();
        float intercept = mVolume->GetIntercept();

        auto data_header_element = mDataRepository->GetGalleryImageManager()->GetCurrentDataHeaderElement();
        if(data_header_element == nullptr){
            return;
        }
        using namespace Mcsf::MedViewer3D;
        GraphicObjectSUVCalculator suv_calc;
        suv_calc.GetSUVParamFromImageHeader(data_header_element);

        if(suv_calc.IsSUVBodyWeightExist()){
            suv_calc.SetSUVCalculateType(BODYWEIGHT);
            std::unique_ptr<float[]> pSUV(new float[2]);
            pSUV[0] = 10.0;
            pSUV[1] = 0.0;
            auto pStoredValue = suv_calc.GetStoredValueFromSUV(pSUV, 2);
            auto dUpperLimit = pStoredValue[0] * slope + intercept;
            auto dLowerLimit = pStoredValue[1] * slope + intercept;

            auto dWW = dUpperLimit - dLowerLimit;
            auto dWC = dUpperLimit - dWW /2;
            mpr->SetWindowing(dWW, dWC);
        }
        else if(suv_calc.IsConcentrationExist()){
            suv_calc.SetSUVCalculateType(CONCENTRATION);
        }
        else{
            suv_calc.SetSUVCalculateType(NOSUVVALUE);
        }
    }
}


TPS_END_NAMESPACE
