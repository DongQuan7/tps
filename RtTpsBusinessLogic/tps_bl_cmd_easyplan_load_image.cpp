//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenqiao.jiang (mailto:wenqiao.jiang@united-imaging.com)
///
///  \file   tps_bl_cmd_load_image_easyplan.h 
///  \brief   load image
///
///  \version 1.0
///  \date  2015/10/30  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "algorithm"
#include "McsfDicomConvertor/mcsf_dicom_convertor_factory.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_easyplan_load_image.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_fw_renderproxybase.h"
#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsFramework/tps_fw_datasourcegroup.h"
#include "RtTpsFramework/tps_fw_rendercontrollerbase.h"

#include "boost/date_time.hpp"
#include <codecvt>
#include <fstream>
#include "McsfDataHeader/mcsf_data_header_element_interface.h"



TPS_BEGIN_NAMESPACE

TpsLoadEasyPlanImageDataCmd::TpsLoadEasyPlanImageDataCmd(const std::string& sBeamUid, const std::string& sImageFilePath) 
    : mBeamUid(sBeamUid), mImageFilePath(sImageFilePath)
{

}

TpsLoadEasyPlanImageDataCmd::~TpsLoadEasyPlanImageDataCmd()
{

}

int TpsLoadEasyPlanImageDataCmd::Execute()
{
    /*int bRet=true;
    RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(mBeamUid);
    if (beam == nullptr) {
        TPS_LOG_DEV_ERROR << "Failed to get beam:" << mBeamUid;
        return TPS_ER_FAILURE;
    }*/

    int bRet=true;
    try {
        // jedge if the path is a .dcm file or .jpg file
        if ((mImageFilePath[(mImageFilePath).length() - 3] == 'd' || mImageFilePath[(mImageFilePath).length() - 3] == 'D')
            && (mImageFilePath[(mImageFilePath).length() - 2] == 'c' || mImageFilePath[(mImageFilePath).length() - 2] == 'C')
            && (mImageFilePath[(mImageFilePath).length() - 1] == 'm' || mImageFilePath[(mImageFilePath).length() - 1] == 'M'))
        {
            bRet = LoadEasyPlanDICOMImageData();
        }
        else if ((mImageFilePath[(mImageFilePath).length() - 3] == 'j' || mImageFilePath[(mImageFilePath).length() - 3] == 'J')
            && (mImageFilePath[(mImageFilePath).length() - 2] == 'p' || mImageFilePath[(mImageFilePath).length() - 2] == 'P')
            && (mImageFilePath[(mImageFilePath).length() - 1] == 'g' || mImageFilePath[(mImageFilePath).length() - 1] == 'G'))
        {
            bRet = LoadEasyPlanJPGImageData();
        }
        else if ((mImageFilePath[(mImageFilePath).length() - 4] == 'j' || mImageFilePath[(mImageFilePath).length() - 4] == 'J')
            && (mImageFilePath[(mImageFilePath).length() - 3] == 'p' || mImageFilePath[(mImageFilePath).length() - 3] == 'P')
            && (mImageFilePath[(mImageFilePath).length() - 2] == 'e' || mImageFilePath[(mImageFilePath).length() - 2] == 'E')
            && (mImageFilePath[(mImageFilePath).length() - 1] == 'g' || mImageFilePath[(mImageFilePath).length() - 1] == 'G'))
        {
            bRet = LoadEasyPlanJPGImageData();
        }
        else if ((mImageFilePath[(mImageFilePath).length() - 3] == 'b' || mImageFilePath[(mImageFilePath).length() - 3] == 'B')
            && (mImageFilePath[(mImageFilePath).length() - 2] == 'm' || mImageFilePath[(mImageFilePath).length() - 2] == 'M')
            && (mImageFilePath[(mImageFilePath).length() - 1] == 'p' || mImageFilePath[(mImageFilePath).length() - 1] == 'P'))
        {
            bRet = LoadEasyPlanBMPImageData();
        }
        else
        {
            TPS_LOG_DEV_ERROR << "Trying to load unsupported image type in: TpsEasyPlanController::LoadEasyPlanImageData";
            return false;
        }

        // set relation between beam and rt image
        /*std::string rtImageUID("");
        TpsImageDataManager* imageDataManager = mDataRepository->GetImageDataManager();
        imageDataManager->GetEasyPlanImageUID(mBeamUid, &rtImageUID);*/
        // need to solve this problem, that an easyplan beam need to be associated with an image uid.
        //beam->SetRelatedRTImageUID(rtImageUID);
    }
    catch (exception ex)
    {
        ex;
    }

    return bRet;
}
int TpsLoadEasyPlanImageDataCmd::LoadEasyPlanDICOMImageData()
{
    bool bRet=true;
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
        bRet = pCon->LoadFile(buffer, size, pImageHeader);

        if (bRet == false) {
            TPS_LOG_DEV_ERROR << "Unable to load file from path " + mImageFilePath;
            return TPS_ER_FAILURE;
        }

        // get DICOM image height and width
        Mcsf::UInt16 imageHeight;
        Mcsf::UInt16 imageWidth;
        Mcsf::ConstString imageWinCenter = nullptr;
        Mcsf::ConstString dicomImageType = nullptr;
        Mcsf::ConstString imageWinWidth = nullptr;
        int sizeOfImageWinCenterStringArray = 0;
        int sizeOfImageWinWidthStringArray = 0;
        Mcsf::UInt16 imageBitsAllocated;
        Mcsf::UInt16 imageBitsStored;
        Mcsf::UInt16 imageHeightBit;
        Mcsf::UInt16 imagePixelRepresentation;
        Mcsf::UInt16 imageSamplesPerPixel;
        Mcsf::ConstString imagePlanePixelSpacing = nullptr;
        Mcsf::ConstString pixelSpacing = nullptr;
        Mcsf::ConstString patientName = nullptr;
        Mcsf::ConstString patientID = nullptr;
        Mcsf::ConstString patientBirthDate = nullptr;
        Mcsf::ConstString patientSex = nullptr;
        Mcsf::ConstString ssd = nullptr;
        Mcsf::ConstString patientPosition = nullptr;
        Mcsf::ConstString acquisitionDate = nullptr;
        Mcsf::ConstString acquisitionTime = nullptr;
        Mcsf::ConstString gantryAngle = nullptr;
        int sizeOfImagePlanePixelSpacing = 0;
        int sizeOfDicomImageTypeStringArray = 0;
        int sizeOfPixelSpacing = 0;
        int sizeOfPatientName = 0;
        int sizeOfPatientID = 0;
        int sizeOfPatientBirthDate = 0;
        int sizeOfPatientSex = 0;
        int sizeOfSSD = 0;
        int sizeOfPatientPosition = 0;
        int sizeOfAcquisitionDate = 0;
        int sizeOfAcquisitionTime = 0;
        int sizeOfGantryAngle = 0;

        pImageHeader->GetUInt16ByTag(DICOMROWSTAG, &imageHeight);
        pImageHeader->GetUInt16ByTag(DICOMCOLUMNSTAG, &imageWidth);
        pImageHeader->GetStringByTag(DICOMWINDOWCENTER, &imageWinCenter, &sizeOfImageWinCenterStringArray);
        pImageHeader->GetStringByTag(DICOMWINDOWWIDTH, &imageWinWidth, &sizeOfImageWinWidthStringArray);
        pImageHeader->GetUInt16ByTag(DICOMBITSALLOCATED, &imageBitsAllocated);
        pImageHeader->GetUInt16ByTag(DICOMBITSSTORED, &imageBitsStored);
        pImageHeader->GetUInt16ByTag(DICOMHIGHBIT, &imageHeightBit);
        pImageHeader->GetUInt16ByTag(DICOMPIXELREPRESENTATION, &imagePixelRepresentation);
        pImageHeader->GetStringByTag(DICOMIMAGETYPE, &dicomImageType, &sizeOfDicomImageTypeStringArray);
        pImageHeader->GetUInt16ByTag(DICOMSAMPLESPERPIXEL, &imageSamplesPerPixel);
        pImageHeader->GetStringByTag(DICOMIMAGEPLANEPIXELSPACING, &imagePlanePixelSpacing, &sizeOfImagePlanePixelSpacing);
        pImageHeader->GetStringByTag(DICOMPIXELSPACING, &pixelSpacing, &sizeOfPixelSpacing);
        pImageHeader->GetStringByTag(DICOMPATIENTNAME, &patientName, &sizeOfPatientName);
        pImageHeader->GetStringByTag(DICOMPATIENTID, &patientID, &sizeOfPatientID);
        pImageHeader->GetStringByTag(DICOMPATIENTBIRTHDATE, &patientBirthDate, &sizeOfPatientBirthDate);
        pImageHeader->GetStringByTag(DICOMPATIENTSEX, &patientSex, &sizeOfPatientSex);
        pImageHeader->GetStringByTag(DICOMSSD, &ssd, &sizeOfSSD);
        pImageHeader->GetStringByTag(DICOMPATIENTPOSITION, &patientPosition, &sizeOfPatientPosition);
        pImageHeader->GetStringByTag(DICOMACQUISITIONDATE, &acquisitionDate, &sizeOfAcquisitionDate);
        pImageHeader->GetStringByTag(DICOMACQUISITIONTIME, &acquisitionTime, &sizeOfAcquisitionTime);
        pImageHeader->GetStringByTag(DICOMGANTRYANGLE, &gantryAngle, &sizeOfGantryAngle);

        // judge if image wincenter and winwidth is valid
        char tempWindowCenter[] = { '1', '2', '7' };
        char tempWindowWidth[] = { '2', '5', '6' };
        if (nullptr == imageWinWidth || nullptr == imageWinCenter) {
            //make temp window center and window width
            imageWinCenter = tempWindowCenter;
            imageWinWidth = tempWindowWidth;
        }

        // judge if image type is valid
        /*if (nullptr == imageType) {
            TPS_LOG_DEV_ERROR << "Unable to get image type " + mImageFilePath;
            return TPS_ER_FAILURE;
        }*/

        //judge if image is DRR, the last 3 elements are 'DRR'
        /*if (imageType[sizeOfImageTypeStringArray - 3] != 'D'
            || imageType[sizeOfImageTypeStringArray - 2] != 'R'
            || imageType[sizeOfImageTypeStringArray - 1] != 'R') {
            TPS_LOG_DEV_ERROR << "Image type is not DRR " + mImageFilePath;
            return TPS_ER_FAILURE;
        }*/

        // judge if image plane pixel spacing is valid
        if (nullptr == imagePlanePixelSpacing) {
            TPS_LOG_DEV_ERROR << "Unable to get image plane pixel spacing" + mImageFilePath;
            return TPS_ER_FAILURE;
        }

        // judge if pixel spacing is valid
        if (nullptr == pixelSpacing) {
            TPS_LOG_DEV_ERROR << "Unable to get pixel spacing" + mImageFilePath;
            //return false;
        }

        // convert pixel representation(unsigned int) to string
        stringstream ss;
        ss << imagePixelRepresentation;
        std::string pixelRepresentation = ss.str();

        // convert pixel spacing from 0.00000/0.00000(string) to two float numbers(0.00000, 0.00000).
        float imgPlanePixelSpacing1 = 0.0f;
        float imgPlanePixelSpacing2 = 0.0f;
        std::string s1 = "";
        std::string s2 = "";
        bool bFirstFloat = true;

        // get two float string
        for (int i = 0; i<strlen(imagePlanePixelSpacing); i++) {
            if (imagePlanePixelSpacing[i] == '\\') {
                bFirstFloat = false;
            }
            if (imagePlanePixelSpacing[i] != '\\' && bFirstFloat) {
                s1 += imagePlanePixelSpacing[i];
            }
            if (imagePlanePixelSpacing[i] != '\\' && !bFirstFloat) {
                s2 += imagePlanePixelSpacing[i];
            }
        }
        // convert string to float
        imgPlanePixelSpacing1 = atof(s1.c_str());
        imgPlanePixelSpacing2 = atof(s2.c_str());

        Mcsf::IDataHeaderElementMap::ElementPointer elementPtr = nullptr;

        // get pixel data from header, 0x7FE00010 is tag of reading pixel data
        bRet = pImageHeader->GetElementByTag(DICOMPIXELDATA, &elementPtr);
        if (bRet == false || elementPtr == nullptr) {
            TPS_LOG_DEV_ERROR << "Unable to get pixel data from file " + mImageFilePath;
            return TPS_ER_FAILURE;
        }

        // judge if the pixel data type is Unit16 or Unit8 and set pixel data to array
        int iSize = 0;
        Mcsf::ConstUInt16Array puint16Array = nullptr;
        // Mcsf::ConstUInt16Array is type of unsigned short*
        // DICOM中的pixel data本身为16位存储
        bRet = elementPtr->GetUInt16Array(&puint16Array, &iSize);
        if (bRet == false) 
        {
            TPS_LOG_DEV_ERROR << "Unable to get unsigned short array from pixel data ";
            return TPS_ER_FAILURE;
        }
        char *bufferUnit = nullptr;
        bufferUnit = new char[iSize * 2];
        memcpy(bufferUnit, puint16Array, iSize * 2);

        // 1: RTFWK RTIMAGE API TO new a RTImage with unique uid with the previous image data && save to DB
        RtRtimage *rtImage = new RtRtimage(true);
        //RtDbDef::RTIMAGE_TYPE imageTypess;
        //imageTypess = RtDbDef::DRR;
        rtImage->set_rows(imageHeight);
        rtImage->set_columns(imageWidth);
        rtImage->set_bitsstored(imageBitsStored);
        rtImage->set_highbit(imageHeightBit);
        //rtImage->set_imagetype(imageTypess);
        rtImage->set_name(mImageFilePath);
        rtImage->set_pixelrepresentation(pixelRepresentation);
        rtImage->set_bitsallocated(imageBitsAllocated);
        rtImage->set_samplesperpixel(imageSamplesPerPixel);

        // need to add image plane pixel spacing
        rtImage->set_firstimageplanepixelspacing(imgPlanePixelSpacing1);
        rtImage->set_secondimageplanepixelspacing(imgPlanePixelSpacing2);
        rtImage->set_windowcenter(atoi(imageWinCenter));
        rtImage->set_windowwidth(atoi(imageWinWidth));
        rtImage->set_frame(1);

        // convert gantry angle from string to float
        if(gantryAngle != nullptr){
            std::string sGantryAngle = "";
            sGantryAngle = gantryAngle;
            rtImage->set_angle(atof(sGantryAngle.c_str()));
        }

        // convert ssd
        if(ssd != nullptr){
            std::string s3 = "";
            s3 = ssd;
            rtImage->set_ssd(atof(s3.c_str()));
        }

        // set DICOM patient name
        if(patientName != nullptr){
            rtImage->set_patientname(patientName);
        }

        // set DICOM patient ID
        if(patientID != nullptr){
            rtImage->set_patientid(patientID);
        }

        // set patient date of birth
        if(patientBirthDate != nullptr){
            tps::DATETIME_BOOST birthdate(boost::gregorian::from_undelimited_string(patientBirthDate));
            rtImage->set_patientbirthdate(birthdate);
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
            rtImage->set_patientsex(genderEnum);
        }

        // set DICOM image type
        if(dicomImageType != nullptr){
            rtImage->set_dicomimagetype(dicomImageType);
        }

        // convert patient position to enum
        if(patientPosition != nullptr){
            int patientPositionEnum = 0;
            std::string sPatientPosition(patientPosition);
            if(sPatientPosition == "HFS"){
                patientPositionEnum = HFS;
            }
            else if(sPatientPosition == "HFP"){
                patientPositionEnum = HFP;
            }
            else if(sPatientPosition == "HFDR"){
                patientPositionEnum = HFDR;
            }
            else if(sPatientPosition == "HFDL"){
                patientPositionEnum = HFDL;
            }
            else if(sPatientPosition == "FFS"){
                patientPositionEnum = FFS;
            }
            else if(sPatientPosition == "FFP"){
                patientPositionEnum = FFP;
            }
            else if(sPatientPosition == "FFDR"){
                patientPositionEnum = FFDR;
            }
            else if(sPatientPosition == "FFDL"){
                patientPositionEnum = FFDL;
            }
            rtImage->set_patientposition(patientPositionEnum);
        }

        // set image acquisition date
        if(acquisitionDate != nullptr){
            tps::DATETIME_BOOST boostAcquisitionDate(boost::gregorian::from_undelimited_string(acquisitionDate));
            rtImage->set_acquisitiondatetime(boostAcquisitionDate);
        }

        // set pixel data
        rtImage->set_pixel_data_buffer(bufferUnit, iSize*sizeof(unsigned short)); // deep copy inside

        //set beam uid
        rtImage->set_beamuid(mBeamUid);



        if(rtImage != nullptr){
            if (TPS_ER_SUCCESS != mSaveObjects->SaveRtImage(*rtImage)) {
                TPS_LOG_DEV_ERROR<<"Failed to save rt image with image UID:"
                    << rtImage->get_uid();
                return TPS_ER_FAILURE;
            }
        }

        // 2: Save this RTImage (type of Image2DEntity) to ImageDataManager
        std::string imageUID = rtImage->get_uid();

        // set current easy plan image uid to image data manager
        TpsImageDataManager* imageDataManager = mDataRepository->GetImageDataManager();
        if(imageDataManager == nullptr){
            TPS_LOG_DEV_ERROR << "Failed to get image data manager!";
            return TPS_ER_FAILURE;
        }

        TpsImage2DEntity *pTpsImage_DRR = nullptr;
        if (false == imageDataManager->GetImage2DData(imageUID, &pTpsImage_DRR)) {
            pTpsImage_DRR = new TpsImage2DEntity();
            pTpsImage_DRR->SetRTImage(rtImage); //not deep copy
            imageDataManager->AddImage2D(imageUID, pTpsImage_DRR);
        }

        // the rtimage list will be returnned back to C# front end
        mVRtImageList.push_back(rtImage);

        RtBeam* currentBeam = mDataRepository->GetBeamManager()->GetBeam(mBeamUid);
        if (currentBeam == nullptr)
        {
            TPS_LOG_DEV_ERROR<<"Failed to get beam with beam UID:"
                << mBeamUid;
            return TPS_ER_FAILURE;
        }
        std::vector<RtBeam*> beamList = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(currentBeam->get_normgroupuid());
        if (beamList.size() > 1)
        {
            if (!CreatOpposedRtImage(puint16Array, bufferUnit, iSize, rtImage))
            {
                TPS_LOG_DEV_ERROR<<"Failed to CreatOpposedRtImage";
                return TPS_ER_FAILURE;
            }
        }

        //delete buffer
        if(bufferUnit != nullptr) {
            DEL_ARRAY(bufferUnit);
        }
        if(puint16Array != nullptr) {
            DEL_ARRAY(puint16Array);
        }

        // 3: CALL mRenderProxy->SetDataSource() and it will call the render controller -> render converter-> checkGO..
        mRenderProxy->EasyPlanLoadImage(WINDOW_EZPLAN_IMAGE, "EZPLAN_IMAGE");
        mRenderProxy->SetEasyPlanPixelSpace();
        mRenderProxy->Render();
    }
    catch (std::exception ex) {
        TPS_LOG_DEV_ERROR << "Exception occurred while loading DICOM image: " + mImageFilePath + " Exception: " + ex.what();
        return TPS_ER_FAILURE;
    }

    if (bRet == true)
    {
        return TPS_ER_SUCCESS;
    }
    else return TPS_ER_FAILURE;
}

bool TpsLoadEasyPlanImageDataCmd::CreatOpposedRtImage(const Mcsf::ConstUInt16Array puint16Array, char *bufferUnit, const int iSize, RtRtimage* rtImage)
{
    // judge if the pixel data type is Unit16 or Unit8 and set pixel data to array
    Mcsf::UInt16Array puint16ArrayOpposed = nullptr;

    char *opposedBufferUnit = nullptr;
    memcpy(bufferUnit, puint16Array, iSize * 2);

    Mcsf::UInt16 imageBitsAllocated = rtImage->get_bitsallocated();
    int imageHeight = rtImage->get_rows();
    int imageWidth = rtImage->get_columns();

    //如果DICOM图像是16位,puint16Array每个元素就对应图像的每个像素,所以先翻转，再转化为char *
    if (imageBitsAllocated == 16)
    {
        puint16ArrayOpposed = new unsigned short[iSize];
        for(int i=0; i<imageHeight; i++) 
        {
            int rowStartIndex = i * imageWidth * (imageBitsAllocated / 16);
            for(int j=0; j<imageWidth; j++) 
            {
                puint16ArrayOpposed[rowStartIndex + j] = puint16Array[rowStartIndex + imageWidth -1 - j];
            }
        }
        opposedBufferUnit = new char[iSize * 2];
        memcpy(opposedBufferUnit, puint16ArrayOpposed, iSize * 2);
    }
    //如果DICOM图像是8位，bufferUnit每个元素就对应图像的每个像素，所以直接翻转char*
    else if(imageBitsAllocated == 8)
    {
        opposedBufferUnit = new char[iSize * 2];
        for(int i=0; i<imageHeight; i++) 
        {
            int rowStartIndex = i * imageWidth;
            for(int j=0; j<imageWidth; j++) 
            {
                opposedBufferUnit[rowStartIndex + j] = bufferUnit[rowStartIndex + imageWidth -1 - j];
            }
        }
    }
    else
    {
        if(bufferUnit != nullptr) {
            DEL_ARRAY(bufferUnit);
        }
        TPS_LOG_DEV_ERROR << "The EasyPlanDICOMImage BitsAllocated is neither 8 nor 16";
        return false;
    }

    RtRtimage newRtImage(true);
    RtRtimage* opposedRtImage = new RtRtimage(*rtImage);
    opposedRtImage->set_uid(newRtImage.get_uid());
    opposedRtImage->set_pixel_data_buffer(opposedBufferUnit, iSize*sizeof(unsigned short)); // deep copy inside

    RtBeam* currentBeam = mDataRepository->GetBeamManager()->GetBeam(mBeamUid);
    std::vector<RtBeam*> beamList = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(currentBeam->get_normgroupuid());
    auto it = std::find_if(beamList.begin(), beamList.end(),[&](RtBeam* obj)
    {
        return obj->get_uid() != mBeamUid;
    });
    //有两个beam才set beam uid
    if (it != beamList.end())
    {
        opposedRtImage->set_beamuid((*it)->get_uid());
    }

    if(opposedRtImage != nullptr){
        if (TPS_ER_SUCCESS != mSaveObjects->SaveRtImage(*opposedRtImage)) {
            TPS_LOG_DEV_ERROR<<"Failed to save rt image with image UID:"
                << opposedRtImage->get_uid();
            return false;
        }
    }

    if(opposedBufferUnit != nullptr){
        DEL_ARRAY(opposedBufferUnit);
    }
    if(puint16ArrayOpposed != nullptr) {
        DEL_ARRAY(puint16ArrayOpposed);
    }

    TpsImageDataManager* imageDataManager = mDataRepository->GetImageDataManager();
    std::string opposedImageUID = opposedRtImage->get_uid();
    TpsImage2DEntity *pTpsOpposedImage_DRR = nullptr;
    if (false == imageDataManager->GetImage2DData(opposedImageUID, &pTpsOpposedImage_DRR)) {
        pTpsOpposedImage_DRR = new TpsImage2DEntity();
        pTpsOpposedImage_DRR->SetRTImage(opposedRtImage); //not deep copy
        imageDataManager->AddImage2D(opposedImageUID, pTpsOpposedImage_DRR);
    }

    mVRtImageList.push_back(opposedRtImage);
    return true;
}

int TpsLoadEasyPlanImageDataCmd::LoadEasyPlanJPGImageData()
{
    bool bRet = true;
    try {
        std::string sConvertedImagePath = "";
        for (int i = 0; i<mImageFilePath.length(); i++) {
            if ('\\' == mImageFilePath[i]) {
                sConvertedImagePath += '\\';
            }
            sConvertedImagePath += mImageFilePath[i];
        }

        // get two image buffer from local jpg file
        /*CJpg *jpeg = new CJpg;
        jpeg;*/

        //const std::string sPath = "E:\\temp.bmp";
        //SaveToTrueColorBitmap(sPath,image_test,imageHeight,imageWidth);
    }
    catch (std::exception ex) {
        ex;
    }
    if (true == bRet)
    {
        return TPS_ER_SUCCESS;
    }
    else return TPS_ER_FAILURE;
}
int TpsLoadEasyPlanImageDataCmd::LoadEasyPlanBMPImageData()
{
    bool bRet = true;
    try {
        std::string sConvertedImagePath = "";
        for (int i = 0; i < mImageFilePath.length(); i++) {
            if ('\\' == mImageFilePath[i]) {
                sConvertedImagePath += '\\';
            }
            sConvertedImagePath += mImageFilePath[i];
        }
        if ("" == sConvertedImagePath) {
            TPS_LOG_DEV_ERROR << "image file path is empty in TpsEasyPlanController::LoadEasyPlanBMPImageData";
            return TPS_ER_FAILURE;
        }

        // get image buffer from local bmp file
        BITMAPFILEHEADER fileHeader;
        BITMAPINFOHEADER infoHeader;

        FILE* file = NULL;
        fopen_s(&file, sConvertedImagePath.c_str(), "rb+");
        if (file == NULL) {
            printf("FILE OPEN FAIL \r\n");
            return TPS_ER_FAILURE;
        }
        unsigned long imageWidth = 0;
        unsigned long imageHeight = 0;
        DWORD size = 1;
        fread(&fileHeader, sizeof(fileHeader), size, file);
        fread(&infoHeader, sizeof(infoHeader), size, file);
        imageWidth = infoHeader.biWidth;
        imageHeight = infoHeader.biHeight;
        long index = 0;
        unsigned char *image_buffer = (unsigned char *)malloc(imageHeight * imageWidth * 3);
        fseek(file, 54, SEEK_SET);
        int i, j, k;
        for (i = 0; i < imageHeight; i++) {
            for (j = 0; j < imageWidth; j++) {
                for (k = 2; k >= 0; k--) {
                    image_buffer[index] = fgetc(file);
                    index++;
                }
            }
        }
        fclose(file);
    }
    catch (exception ex) {
        TPS_LOG_DEV_ERROR << "Failed at: TpsEasyPlanController::LoadEasyPlanBMPImageData(const std::string& sImageFilePath )";
        return TPS_ER_FAILURE;
    }

    if (true == bRet)
    {
        return TPS_ER_SUCCESS;
    }
    else return TPS_ER_FAILURE;
}

std::vector<RtRtimage*> TpsLoadEasyPlanImageDataCmd::GetRtImageList()
{
    return mVRtImageList;
}

TPS_END_NAMESPACE