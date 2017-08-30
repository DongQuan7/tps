//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenqiao.jiang (mailto:xiang.liang@united-imaging.com)
///
///  \file   tps_bl_cmd_easyplan_loadrtimageinfo.cpp 
///  \brief   load partial image
///
///  \version 1.0
///  \date  2017/8/4  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include <codecvt>
#include <fstream>
#include "boost/date_time.hpp"

#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_easyplan_loadrtimageinfo.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"
#include "McsfDicomConvertor/mcsf_dicom_convertor_factory.h"

TPS_BEGIN_NAMESPACE

TpsEasyPlanLoadRtImageInfoCmd::TpsEasyPlanLoadRtImageInfoCmd(const std::string& sImageFilePath) 
    : mImageFilePath(sImageFilePath)
{

}

TpsEasyPlanLoadRtImageInfoCmd::~TpsEasyPlanLoadRtImageInfoCmd()
{
    if(mVRtImage != nullptr) {
        delete(mVRtImage);
        mVRtImage = nullptr;
    }
}

int TpsEasyPlanLoadRtImageInfoCmd::Execute()
{
    try {
        // use UIH API to load Dicom image and get pixel data.
        Mcsf::ISimpleDICOMConvertorPtr pCon = Mcsf::DICOMConvertorFactory::Instance()->CreateSimpleConvertor();
        if (pCon == nullptr) {
            TPS_LOG_DEV_ERROR << "Mcsf::ISimpleDICOMConvertorPtr pCon is null pointer";
            return TPS_ER_FAILURE;
        }

        Mcsf::IDataHeaderElementMap* pImageHeader = Mcsf::IDataHeaderElementMap::CreateDataHeader();
        if (pImageHeader == nullptr) {
            TPS_LOG_DEV_ERROR << "Mcsf::IDataHeaderElementMap* pImageHeader is null pointer";
            return TPS_ER_FAILURE;
        }

        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        std::wstring wideStr = conv.from_bytes(mImageFilePath);

        char * buffer;
        long size;
        std::ifstream file(wideStr, ios::in | ios::binary | ios::ate);
        size = file.tellg();
        file.seekg(0, ios::beg);
        buffer = new char[size];
        file.read(buffer, size);
        file.close();
        if (!pCon->LoadFile(buffer, size, pImageHeader))
        {
            TPS_LOG_DEV_ERROR << "Unable to load file from path " + mImageFilePath;
            return TPS_ER_FAILURE;
        }

        Mcsf::ConstString patientName = nullptr;
        Mcsf::ConstString patientID = nullptr;
        Mcsf::ConstString patientBirthDate = nullptr;
        Mcsf::ConstString patientSex = nullptr;
        Mcsf::ConstString gantryAngle = nullptr;

        int sizeOfPatientName = 0;
        int sizeOfPatientID = 0;
        int sizeOfPatientBirthDate = 0;
        int sizeOfPatientSex = 0;
        int sizeOfGantryAngle = 0;

        pImageHeader->GetStringByTag(DICOMPATIENTNAME, &patientName, &sizeOfPatientName);
        pImageHeader->GetStringByTag(DICOMPATIENTID, &patientID, &sizeOfPatientID);
        pImageHeader->GetStringByTag(DICOMPATIENTBIRTHDATE, &patientBirthDate, &sizeOfPatientBirthDate);
        pImageHeader->GetStringByTag(DICOMPATIENTSEX, &patientSex, &sizeOfPatientSex);
        pImageHeader->GetStringByTag(DICOMGANTRYANGLE, &gantryAngle, &sizeOfGantryAngle);

        // 1: RTFWK RTIMAGE API TO new a RTImage with unique uid with the previous image data && save to DB
        mVRtImage = new RtRtimage(true);

        // convert gantry angle from string to float
        if(gantryAngle != nullptr){
            std::string sGantryAngle = "";
            sGantryAngle = gantryAngle;
            mVRtImage->set_angle(atof(sGantryAngle.c_str()));
        }

        // set DICOM patient name
        if(patientName != nullptr){
            mVRtImage->set_patientname(patientName);
        }

        // set DICOM patient ID
        if(patientID != nullptr){
            mVRtImage->set_patientid(patientID);
        }

        // set patient date of birth
        if(patientBirthDate != nullptr){
            tps::DATETIME_BOOST birthdate(boost::gregorian::from_undelimited_string(patientBirthDate));
            mVRtImage->set_patientbirthdate(birthdate);
        }

        // convert patient gender
        if(patientSex != nullptr){
            tps::TMS_GENDER genderEnum;
            std::string sPatientSex(patientSex);
            if(sPatientSex == "M"){
                genderEnum = Male;
            }
            else if(sPatientSex == "F"){
                genderEnum = Female;
            }
            else{
                genderEnum = Other;
            }
            mVRtImage->set_patientsex(genderEnum);
        }

        return TPS_ER_SUCCESS;
    }
    catch (std::exception ex) {
        TPS_LOG_DEV_ERROR << "Exception occurred while loading DICOM image: " + mImageFilePath + " Exception: " + ex.what();
        return TPS_ER_FAILURE;
    }
}

RtRtimage* TpsEasyPlanLoadRtImageInfoCmd::GetRtImage()
{
    return mVRtImage;
}

TPS_END_NAMESPACE