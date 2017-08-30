////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_da_image3dentity.cpp
/// 
///  \brief class TpsImage3DEntity implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/24
////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "RtTpsFramework/tps_fw_image3dentity.h"

//ZHENGHE
#include "McsfAlgorithms/McsfAppLib/McsfAlgoAutoContour.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"

#include "RtTpsFramework/tps_fw_series_group4d.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

#include <limits>
#include "tps_logger.h"

//#include "tps_performance.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

bool SaveToTrueColorBitmap(const std::string& path, unsigned char* buffer,
const int& width, const int& height);

TpsImage3DEntity::TpsImage3DEntity() 
    :mTurnStepSum(0), mAutoContour(nullptr), mSliceBuffer(nullptr),mSeries(nullptr)
{
    m_fXminInPat = -999.f; //-268.5
    m_fXmaxInPat = 999.f; // 265.5
    m_fYminInPat = -999.f; // -258.5
    m_fYmaxInPat = 999.f; // 275.5
    m_fZminInPat = 0.f; // 67.5
    m_fZmaxInPat = 999.f; // 400.5

    m_wpSeriesGroup4d.reset();
    mOriginalPoint = new Mcsf::Point3f();
    try{
        //mDICOMPatientToIECPatient.SetIdentity();
        mVolumeToDICOMPatient = new Mcsf::Matrix4f();
        mVolumeToDICOMPatient->SetIdentity();
    }
    catch (std::exception e){
        TPS_LOG_DEV_ERROR<<e.what();
    }
}

TpsImage3DEntity::TpsImage3DEntity(const TpsImage3DEntity* pSrcImage3DEntity, bool bDeepCopy)
    : mTurnStepSum(0), mAutoContour(nullptr), mSliceBuffer(nullptr) 
{
    bDeepCopy;
    memcpy((void*)this, (void*)pSrcImage3DEntity,  sizeof(TpsImage3DEntity));
}

TpsImage3DEntity::~TpsImage3DEntity()
{
    DEL_PTR(mSeries);
    DEL_ARRAY(mSliceBuffer);
    DEL_PTR(mAutoContour);
    DEL_PTR(mVolumeToDICOMPatient);
    DEL_PTR(mOriginalPoint);
}

void TpsImage3DEntity::CreateAutoContour()
{
    if (nullptr == mSeries){
        TPS_LOG_DEV_ERROR<<"mImage3D is null";
        return;
    }

    RtImage3DHeader* pHeader = mSeries->get_header();
    unsigned long ulSize(0);
    char* pImageData = mSeries->get_imagedata(&ulSize);
    if (nullptr == pImageData || pHeader->m_iSliceCount < 2){
        TPS_LOG_DEV_ERROR<<"pImage3DData ImageData is null or pImage3DData->m_Header.m_iSliceCount<2";
        return;
    }

    const int sliceCount = pHeader->m_iSliceCount;
    unsigned int uiSize[3] = {pHeader->m_iXDim, pHeader->m_iYDim, sliceCount};

    //LOAD VOLUME INFO
    Mcsf::Vector3f vecOrtX(pHeader->m_ImageXOrientation.x, 
        pHeader->m_ImageXOrientation.y, pHeader->m_ImageXOrientation.z);
    Mcsf::Vector3f vecOrtY(pHeader->m_ImageYOrientation.x, 
        pHeader->m_ImageYOrientation.y, pHeader->m_ImageYOrientation.z);

    //spacing
    Mcsf::Vector3f vecOrtZ = vecOrtX.CrossProduct(vecOrtY);
    RtImage3DPerScanInfo *pFstImage, *pScdImage;
    RtImage3DPerScanInfo *pLstImage;
    pFstImage = &pHeader->m_vScanInfoList[0];
    pScdImage = &pHeader->m_vScanInfoList[1];
    pLstImage = &pHeader->m_vScanInfoList[sliceCount - 1];

    Mcsf::Point3f fstPosition(pFstImage->m_ImagePosition.x, 
        pFstImage->m_ImagePosition.y, pFstImage->m_ImagePosition.z);
    Mcsf::Point3f lstPosition(pLstImage->m_ImagePosition.x,
        pLstImage->m_ImagePosition.y, pLstImage->m_ImagePosition.z);
    Mcsf::Vector3f vecFst2Lst = lstPosition - fstPosition;
    if (vecFst2Lst.DotProduct(vecOrtZ) < -1e-6)
    {
        vecOrtZ *= -1;
    }

    //Mcsf::MedViewer3D::Vector3D vecFst2Lst = lstPosition3D - fstPosition3D;
    double dZSpacing = fabs(vecOrtZ.DotProduct(vecFst2Lst));
    dZSpacing /= (pHeader->m_iSliceCount - 1);

    double dSpacing[3]={pHeader->m_dXSize, pHeader->m_dYSize, dZSpacing};
    //Mcsf::McsfAlgoAutoContour* pAutoContour = new Mcsf::McsfAlgoAutoContour(
    //    (short*)image3DData.m_pImageData,uiSize, dSpacing);
    float fOrientation[6] = {vecOrtX.GetX(), vecOrtX.GetY(), vecOrtX.GetZ(),
        vecOrtY.GetX(),vecOrtY.GetY(),vecOrtY.GetZ()};
    float fSliceLocation[2] = {pFstImage->m_ImagePosition.z, 
        pScdImage->m_ImagePosition.z};
    const std::string sPatientPosition(pHeader->m_cPatientPosition);
    const float fSlope = (float)pHeader->m_dScaleSlope;
    const short intercept = (short)pHeader->m_dScaleIntercept;//TODO float

    DEL_PTR(mAutoContour);
    //TEST_PERFORMANCE_INIT;
    //TEST_PERFORMANCE_BEGIN;

    //create object
    mAutoContour = new Mcsf::McsfAlgoAutoContour(
        (short*)pImageData,uiSize, dSpacing, 
        fOrientation, fSliceLocation,sPatientPosition, fSlope, intercept);
    //TEST_PERFORMANCE_END("new Mcsf::McsfAlgoAutoContour");
    return;
}

bool TpsImage3DEntity::InitializationAutoContour(McsfGeometry::SubVolume* pSkin){
    if (nullptr == mAutoContour){
        this->CreateAutoContour();
    }

    //TEST_PERFORMANCE_INIT;
    //TEST_PERFORMANCE_BEGIN;
    if (nullptr == pSkin){
        if(!mAutoContour->Initialization()){
            TPS_LOG_DEV_ERROR<<"McsfAlgoAutoContour Initialization failed!";
            return false;
        }
    }
    else{
        if(!mAutoContour->Initialization(*pSkin)){
            TPS_LOG_DEV_ERROR<<"McsfAlgoAutoContour Initialization failed!";
            return false;
        }
    }
    //TEST_PERFORMANCE_END("Mcsf::McsfAlgoAutoContour Initialization");
    return true;
}

bool TpsImage3DEntity::SetupImage3D(RtSeries* const series)
{
    mSeries = series;
    std::string sImageUID = series->get_uid();
    RtImage3DHeader* header = series->get_header();
    if (header == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d object which series uid: "<<sImageUID;
        return false;
    }
    if (header->m_vScanInfoList.empty())
    {
        TPS_LOG_DEV_ERROR<<"There is no scan info list.";
        return false;
    }

    mIsCopied = series->get_isqa();
    mPatientName = series->get_patientname();
    // Xi.Chen : For Load Series By Course.
    // TODO
    
    //CreateAutoContour(); need 3.8s Oct.9,2014

    //////////////////////////////////////////////////////////////////////////
    ///         ONLY FOR TEST       set dummy original point    
    SetDefaultSetupPoint_i();

    SetupVolumeToPatient_i();

    CorrectWindowLevel_i();
    //SetupDICOMPatientToIECPatient_i();

    mStartSliceNumber = header->m_vScanInfoList[0].m_uiInstanceNumber;
    return true;
}

//bool TpsImage3DEntity::SetupImage3D(RtImage3D* image3D){
//
//    if (mImage3D != image3D)
//        mImage3D  =  image3D;
//
//    //////////////////////////////////////////////////////////////////////////
//    ///         ONLY FOR TEST       set dummy original point    
//    //SetDefaultSetupPoint_i();
//
//    SetupVolumeToPatient_i();
//    SetupDICOMPatientToIECPatient_i();
//    return true;
//}


RtSeries* TpsImage3DEntity::GetRtSeries() const
{
    return mSeries;
}

Mcsf::McsfAlgoAutoContour* TpsImage3DEntity::GetAutoContour() const{
    return mAutoContour;
}

bool TpsImage3DEntity::GetImage3DHeader(RtImage3DHeader*& header) const
{
    if (mSeries == nullptr){
        TPS_LOG_DEV_ERROR<<"The image 3d is empty.";
        return false;
    }
    header = mSeries->get_header();
    return true;
}

bool TpsImage3DEntity::GetImage3DBuffer(char*& buffer) const{
    if (mSeries == nullptr){
        TPS_LOG_DEV_ERROR<<"The image 3d is empty.";
        return false;
    }
    unsigned long ulSize(0);
    buffer = mSeries->get_imagedata(&ulSize);
    return true;
}

std::string TpsImage3DEntity::GetUID() const
{
    return mSeries->get_uid();
}

template<typename T>
void CalcWindowLevel(T* data, unsigned int size, double& ww, double& wl)
{
    if (data == nullptr) 
    {
        ww = 0.0;
        wl = 0.0;
        return;
    }

    //当前取最大最小，效果不好，以后可以采用更好的统计方法
    T max = std::numeric_limits<T>().min();
    T min = std::numeric_limits<T>().max();
    for (int i = 0; i < size; ++i)
    {
        min = TPS_MIN(min, data[i]);
        max = TPS_MAX(max, data[i]);
    }
    ww = max - min;
    wl = min + ww * 0.5;
}

void CalcWindowLevel(void* data, int dataType, unsigned int size, double& ww, double& wl)
{
    switch(dataType)
    {
    case DATA_TYPE_UNSIGNED_CHAR:
        CalcWindowLevel((unsigned char*)data, size, ww, wl);
        break;
    case DATA_TYPE_CHAR:
        CalcWindowLevel((char*)data, size, ww, wl);
        break;
    case DATA_TYPE_UNSIGNED_SHORT:
        CalcWindowLevel((unsigned short*)data, size, ww, wl);
        break;
    case DATA_TYPE_SHORT:
        CalcWindowLevel((short*)data, size, ww, wl);
        break;
    }
}

bool IsWLValid(const std::string& modality, double ww, double wl)
{
    if(modality == "CT")
    {
        return wl >= WINDOW_CENTER_MIN_CT && 
            wl <= WINDOW_CENTER_MAX_CT && 
            ww >= WINDOW_WIDTH_MIN && 
            ww <= WINDOW_WIDTH_MAX_CT;
    }
    else if(modality == "MR")
    {
        return wl >= WINDOW_CENTER_MIN_MR && 
            wl <= WINDOW_CENTER_MAX_MR && 
            ww >= WINDOW_WIDTH_MIN && 
            ww <= WINDOW_WIDTH_MAX_MR;
    }
    else if(modality == "PT")
    {
        return wl >= WINDOW_CENTER_MIN_PT && 
            wl <= WINDOW_CENTER_MAX_PT && 
            ww >= WINDOW_WIDTH_MIN_PT && 
            ww <= WINDOW_WIDTH_MAX_PT;
    }
    else return false;
}

void TpsImage3DEntity::CorrectWindowLevel_i()
{
    RtImage3DHeader* header = mSeries->get_header();
    if (IsWLValid(mSeries->get_modality(), 
        header->m_iWindowWidth, header->m_iWindowCenter)) return;

    int iBitsAllc = header->m_iBitsAllocated;
    int iPixelRepresentation = header->m_iDataType;
    DATATYPE dataType;
    if (iBitsAllc == 8 && iPixelRepresentation == 0)
    {
        dataType = DATA_TYPE_UNSIGNED_CHAR;
    }
    else if (iBitsAllc == 8 && iPixelRepresentation == 1)
    {
        dataType = DATA_TYPE_CHAR;
    }
    else if (iBitsAllc == 16 && iPixelRepresentation == 0)
    {
        dataType = DATA_TYPE_UNSIGNED_SHORT;
    }
    else if (iBitsAllc == 16 && iPixelRepresentation == 1)
    {
        dataType = DATA_TYPE_SHORT;
    }
    else 
    {
        TPS_LOG_DEV_ERROR<<"Unsupported data type which bit allocated: "
            <<iBitsAllc<<" pixel representation: "<<iPixelRepresentation;
        return;
    }
    double ww, wl;
    unsigned long ulSize(0);
    char* buffer = mSeries->get_imagedata(&ulSize);
    ulSize /= (iBitsAllc / 8);
    CalcWindowLevel(buffer, dataType, ulSize, ww, wl);
    ww *= header->m_dScaleSlope;
    wl = wl * header->m_dScaleSlope + header->m_dScaleIntercept;
    header->m_iWindowWidth = static_cast<int>(ww);
    header->m_iWindowCenter = static_cast<int>(wl);
}

bool TpsImage3DEntity::InitSliceBuffer(int turnDirection)
{
    try
    {
        mTurnStepSum += turnDirection;
        unsigned long ulSize(0);
        char* buffers3D = mSeries->get_imagedata(&ulSize);
        RtImage3DHeader* header = mSeries->get_header();

        int xCount = header->m_iXDim;
        int yCount = header->m_iYDim;
        int zCount = header->m_iSliceCount;
        double slop =header->m_dScaleSlope;
        double intercept =header->m_dScaleIntercept;

        int planeCount = xCount * yCount;
        unsigned char* buffer = new unsigned char[planeCount];
        int cnt = zCount % 2 ? 2 : 1;
        if(mTurnStepSum < -zCount/2 -cnt){
            mTurnStepSum =  -zCount/2 -cnt;
        }else if(mTurnStepSum > zCount /2 + cnt){
            mTurnStepSum = zCount / 2 + cnt;
        }

        int sliceIndex = zCount / 2 + mTurnStepSum;
        if(sliceIndex < 0 ){
            sliceIndex = 0;
            mTurnStepSum = -zCount / 2;
        }else if(sliceIndex > zCount - 1){
            sliceIndex = zCount - 1;
            mTurnStepSum = zCount / 2 + cnt - 2; // make sure we reach the last page even if the slice count is odd
        }
        //cout<<sliceIndex<<endl;
        int windowWidth = header->m_iWindowWidth;
        int windowCenter = header->m_iWindowCenter;
        float gray = 0.f;
        short* volumeData = (short*)buffers3D;
        int ct = 0;
        bool isPT = mSeries->get_modality() == "PT";
        for(int j = 0; j < yCount; ++j){
            for(int i = 0; i < xCount; ++i){
                ct = volumeData[sliceIndex * planeCount + j * xCount + i] * slop + intercept;
                gray = float(ct - windowCenter) / windowWidth + 0.5f;
                gray = TPS_CLAMP(gray, 0.f, 1.f);
                if (isPT) gray = 1.f - gray;
                buffer[j * xCount + i] = int(gray * 255 + 0.5f);
            }
        }
        //#ifdef _DEBUG
        //        unsigned char* tempBuffer = new unsigned char[yCount * xCount * 4];
        //        for (int i=0; i<xCount*yCount; ++i){
        //            tempBuffer[4*i] = buffer[i];
        //            tempBuffer[4*i + 1] = buffer[i];
        //            tempBuffer[4*i + 2] = buffer[i];
        //            tempBuffer[4*i + 3] = 255;
        //        }
        //        char fileName[256];
        //        sprintf_s(fileName, "d:\\temp\\sliceBuffer.bmp");
        //
        //        SaveToTrueColorBitmap(fileName, tempBuffer, xCount, yCount);
        //        delete[] tempBuffer;
        //#endif
        if (mSliceBuffer){
            delete[] mSliceBuffer;
        }
        mSliceBuffer = new unsigned char[xCount * yCount];
        memcpy(mSliceBuffer, buffer, xCount * yCount);

        if(buffer != nullptr){
            delete[] buffer;
            buffer = nullptr;
        }
    }
    catch(...) {
        printf("Fatal error!");
    }
    
    return true;
}

bool TpsImage3DEntity::SetSetupPoint(const Mcsf::Point3f& point){
    if (mSeries == nullptr){
        TPS_LOG_DEV_ERROR<<"The series is empty.";
        return false;
    }

    if (point != *mOriginalPoint){
        *mOriginalPoint = point;
//         mDICOMPatientToIECPatient.SetIdentity();
//         mDICOMPatientToIECPatient.Translation(Mcsf::Vector3f(-1 * point.GetX(), 
//             -1 * point.GetY(), -1 * point.GetZ()));
//         mDICOMPatientToIECPatient.RotationX(float(-PI / 2));
    }
    return true;
}

Mcsf::Point3f* TpsImage3DEntity::GetSetupPoint() const
{
    return mOriginalPoint;
}

bool TpsImage3DEntity::SetImagetransformation(RtImagetransformation* &  vTransformation){
    if (mSeries == nullptr){
        TPS_LOG_DEV_ERROR << "The series is empty.";
        return false;
    }

    auto it = mImagetransformationMap.find(vTransformation->get_transfomationseriesuid());
    if (it != mImagetransformationMap.end()){
        if (vTransformation != it->second)
        {
            DEL_PTR(it->second);
        }
    }

    mImagetransformationMap[vTransformation->get_transfomationseriesuid()] = vTransformation;
    return true;
}

bool TpsImage3DEntity::GetRegistrationMatrix(
    const std::string& sUID, Mcsf::Matrix4f& matrix) const
{
    if (mSeries == nullptr){
        TPS_LOG_DEV_ERROR << "The series is empty.";
        return false;
    }

    auto it = mImagetransformationMap.find(sUID);
    if (it == mImagetransformationMap.end()){
        TPS_LOG_DEV_ERROR << "Registration matrix does not exist to image data: " << sUID;
        return false;
    }

    const float* buffer = nullptr;
    buffer = it->second->get_registrationmatrix();
    matrix.SetCol0(Mcsf::Vector4f(buffer[0], buffer[1], buffer[2], buffer[3]));
    matrix.SetCol1(Mcsf::Vector4f(buffer[4], buffer[5], buffer[6], buffer[7]));
    matrix.SetCol2(Mcsf::Vector4f(buffer[8], buffer[9], buffer[10], buffer[11]));
    matrix.SetCol3(Mcsf::Vector4f(buffer[12], buffer[13], buffer[14], buffer[15]));
    return true;
}

bool TpsImage3DEntity::GetImageTransformationUid(const std::string& sFloatUID, std::string& sUID) const
{
    if (mSeries == nullptr){
        TPS_LOG_DEV_ERROR << "The series is empty.";
        return false;
    }

    auto it = mImagetransformationMap.find(sFloatUID);
    if (it == mImagetransformationMap.end()){
        TPS_LOG_DEV_ERROR << "Registration Uid does not exist to image data: " << sFloatUID;
        return false;
    }
    sUID = it->second->get_uid();
    return true;
}

bool TpsImage3DEntity::RemoveImagetransformation(const std::string& sUID){
    if (mSeries == nullptr){
        TPS_LOG_DEV_ERROR<<"The series is empty.";
        return false;
    }

    auto it = mImagetransformationMap.find(sUID);
    if (it == mImagetransformationMap.end()){
        TPS_LOG_DEV_INFO << "Registration matrix does not exist to image data: " << sUID;
        return true;
    }

    DEL_PTR(it->second);
    mImagetransformationMap.erase(it);
    return true;
}

void* TpsImage3DEntity::GenerateEmptyDeformableRegistrationResultBuffer(){
    if (mSeries == nullptr){
        TPS_LOG_DEV_ERROR<<"The image 3d is empty.";
        return nullptr;
    }

    RtImage3DHeader* header = mSeries->get_header();
    if (nullptr == header){
        TPS_LOG_DEV_ERROR<<"The header is null.";
        return nullptr;
    }
    int dim[3] = { header->m_iXDim, header->m_iYDim, header->m_iSliceCount };
    int count = dim[0] * dim[1] * dim[2] * header->m_iBitsAllocated / 8/* * SamplePerPixel*/;
    char* buffer = new char[count];
    memset(buffer, 0, count * sizeof(char));
    return buffer;
}

void TpsImage3DEntity::SetDeformableRegistrationResult(const std::string& sUID, void* buffer){
    auto it = mDeformableRegistrationResultMap.find(sUID);
    if (it != mDeformableRegistrationResultMap.end() && buffer != it->second){
        delete[] it->second;
    }

    mDeformableRegistrationResultMap[sUID] = buffer;
}

bool TpsImage3DEntity::GetDeformableRegistrationResult(const std::string& sUID, void*& buffer){
    auto it = mDeformableRegistrationResultMap.find(sUID);
    if (it == mDeformableRegistrationResultMap.end()){
        TPS_LOG_DEV_ERROR<<"Deformable registration result does not exist.";
        return false;
    }

    buffer = it->second;
    return true;
}

void TpsImage3DEntity::RemoveDeformableRegistrationResult(const std::string& sUID){
    auto it = mDeformableRegistrationResultMap.find(sUID);
    if (it == mDeformableRegistrationResultMap.end()){
        TPS_LOG_DEV_INFO<<"Deformable registration result does not exist.";
        return;
    }

    delete[] it->second;
    mDeformableRegistrationResultMap.erase(it);
}

//bool TpsImage3DEntity::GetDICOMPatientToIECPatientMatrix(Mcsf::Matrix4f& matrix) const{
//    if (mSeries == nullptr){
//        TPS_LOG_DEV_ERROR<<"The series is empty.";
//        return false;
//    }
//
//    matrix = mDICOMPatientToIECPatient;
//    return true;
//}

bool TpsImage3DEntity::GetVolumeToPatientMatrix(Mcsf::Matrix4f& matrix) const{
    if (mSeries == nullptr){
        TPS_LOG_DEV_ERROR<<"The series is empty.";
        return false;
    }
    
    matrix = *mVolumeToDICOMPatient;
    return true;
}

void TpsImage3DEntity::SetDefaultSetupPoint_i()
{
    RtImage3DHeader* header = mSeries->get_header();
    if (nullptr == header){
        return;
    }

    int centerIndex = header->m_iSliceCount / 2;
    if (centerIndex == 0) 
    {
        mOriginalPoint->SetX(0.f);
        mOriginalPoint->SetY(0.f);
        mOriginalPoint->SetZ(0.f);
        return;
    }

    auto scan = header->m_vScanInfoList[centerIndex];

    Mcsf::Vector3f dbvec3f_XOri(header->m_ImageXOrientation.x,
        header->m_ImageXOrientation.y, header->m_ImageXOrientation.z);
    Mcsf::Vector3f dbvec3f_YOri(header->m_ImageYOrientation.x,
        header->m_ImageYOrientation.y, header->m_ImageYOrientation.z);

    Mcsf::Vector3f setup = 
        (header->m_iXDim - 1) * header->m_dXSize / 2 * dbvec3f_XOri + 
        (header->m_iYDim - 1) * header->m_dYSize / 2 * dbvec3f_YOri;

    mOriginalPoint->SetX(scan.m_ImagePosition.x + setup.GetX());
    mOriginalPoint->SetY(scan.m_ImagePosition.y + setup.GetY());
    mOriginalPoint->SetZ(scan.m_ImagePosition.z + setup.GetZ());
}

void TpsImage3DEntity::SetupVolumeToPatient_i()
{
    RtImage3DHeader* header = mSeries->get_header();
    if (nullptr == header){
        return;
    }

    mSpacing[0] = header->m_dXSize;
    mSpacing[1] = header->m_dYSize;
    Mcsf::Vector3f ortX(header->m_ImageXOrientation.x, 
        header->m_ImageXOrientation.y, header->m_ImageXOrientation.z);
    Mcsf::Vector3f ortY(header->m_ImageYOrientation.x, 
        header->m_ImageYOrientation.y, header->m_ImageYOrientation.z);

    int sliceCount = header->m_iSliceCount;
    if (!sliceCount) return;
    auto scanInfoList = header->m_vScanInfoList;
    Mcsf::Point3f org(scanInfoList[0].m_ImagePosition.x, 
        scanInfoList[0].m_ImagePosition.y, scanInfoList[0].m_ImagePosition.z);
    Mcsf::Vector3f vecZ(scanInfoList[sliceCount - 1].m_ImagePosition.x - org.GetX(),
        scanInfoList[sliceCount - 1].m_ImagePosition.y - org.GetY(),
        scanInfoList[sliceCount - 1].m_ImagePosition.z - org.GetZ());

    mSpacing[2] = vecZ.Magnitude() / (sliceCount - 1);
    Mcsf::Vector3f ortZ = vecZ.GetNormalize();

    Mcsf::Vector4f col0(mSpacing[0] * ortX.GetX(), mSpacing[0] * ortX.GetY(), 
        mSpacing[0] * ortX.GetZ(), 0.0f);
    Mcsf::Vector4f col1(mSpacing[1] * ortY.GetX(), mSpacing[1] * ortY.GetY(), 
        mSpacing[1] * ortY.GetZ(), 0.0f);
    Mcsf::Vector4f col2(mSpacing[2] * ortZ.GetX(), mSpacing[2] * ortZ.GetY(), 
        mSpacing[2] * ortZ.GetZ(), 0.0f);
    Mcsf::Vector4f col3(org.GetX(), org.GetY(), org.GetZ(), 1.0f);
    *mVolumeToDICOMPatient = Mcsf::Matrix4f(col0, col1, col2, col3);

    // setup the patient image bounding box
    // used by POI so far (by song.li)
    int xDim = header->m_iXDim;
    int yDim = header->m_iYDim;
    Mcsf::Point3f ptMin = org;
    Mcsf::Point3f dbpt3f_imgpos(scanInfoList[sliceCount - 1].m_ImagePosition.x,
        scanInfoList[sliceCount - 1].m_ImagePosition.y, 
        scanInfoList[sliceCount - 1].m_ImagePosition.z);
    Mcsf::Point3f ptMax = dbpt3f_imgpos + (xDim - 1.0) * mSpacing[0] * ortX + (yDim - 1.0) * mSpacing[1] * ortY;
    m_fXminInPat = min(ptMin.GetX(), ptMax.GetX());
    m_fXmaxInPat = max(ptMin.GetX(), ptMax.GetX());
    m_fYminInPat = min(ptMin.GetY(), ptMax.GetY());
    m_fYmaxInPat = max(ptMin.GetY(), ptMax.GetY());
    m_fZminInPat = min(ptMin.GetZ(), ptMax.GetZ());
    m_fZmaxInPat = max(ptMin.GetZ(), ptMax.GetZ());
/*
    std::cout<<"========================================"<<std::endl;
    std::cout<<"m_fXminInPat:"<<m_fXminInPat<<std::endl;
    std::cout<<"m_fXmaxInPat:"<<m_fXmaxInPat<<std::endl;
    std::cout<<"m_fYminInPat:"<<m_fYminInPat<<std::endl;
    std::cout<<"m_fYmaxInPat:"<<m_fYmaxInPat<<std::endl;
    std::cout<<"m_fZminInPat:"<<m_fZminInPat<<std::endl;
    std::cout<<"m_fZmaxInPat:"<<m_fZmaxInPat<<std::endl;
    std::cout<<"========================================"<<std::endl;
*/
    mMatV2P = TpsArithmeticConverter::ConvertToMatrix4x4(*mVolumeToDICOMPatient);
}

//void TpsImage3DEntity::SetupDICOMPatientToIECPatient_i()
//{
//    Mcsf::Point3f originalIEC(mOriginalPoint);
//    mDICOMPatientToIECPatient.SetIdentity();
//    mDICOMPatientToIECPatient.Translation(Mcsf::Vector3f(-1 * originalIEC.GetX(), 
//        -1 * originalIEC.GetY(), -1 * originalIEC.GetZ()));
//    mDICOMPatientToIECPatient.RotationX(float(-PI / 2));
//}

bool TpsImage3DEntity::GetIsQACopied() {
    return mIsCopied;
}

const std::map<std::string, RtImagetransformation*>& TpsImage3DEntity::GetAllImagetransformation() const {
    return mImagetransformationMap;
}

std::string TpsImage3DEntity::GetPatientName() const {
    return mPatientName;
}

std::string TpsImage3DEntity::GetImageUidByIndex(const int& iIndex) const
{
    std::string sUid("");
    const RtImage3DHeader* pHeader = mSeries->get_header();
    if (nullptr == pHeader){
        return sUid;
    }
    if (iIndex >= pHeader->m_iSliceCount) return "";
    return pHeader->m_vScanInfoList[iIndex].m_sUID;
}

std::string TpsImage3DEntity::GetGroupUid()
{
    std::shared_ptr<TpsSeriesGroup4d> spGroup = m_wpSeriesGroup4d.lock();
    if (spGroup == nullptr) return "";
    return spGroup->m_strGroupUid;
}

void TpsImage3DEntity::GetBoundaryInDosegrid(Mcsf::Matrix4f matP2G, int& lbx, int& lby, int& lbz,
    int& rtx, int& rty, int& rtz) const
{
    std::vector<Mcsf::Point3f> vectors;
    vectors.resize(8);
    float xRange[2], yRange[2], zRange[2];
    xRange[0] = m_fXminInPat; xRange[1] = m_fXmaxInPat;
    yRange[0] = m_fYminInPat; yRange[1] = m_fYmaxInPat;
    zRange[0] = m_fZminInPat; zRange[1] = m_fZmaxInPat;
    int ix = 0, iy = 0, iz = 0;
    for (int i = 0; i < 8; i++)
    {
        ix = i >> 2;
        iy = (i >> 1) - (ix << 1);
        iz = i - ((i >> 1) << 1);
        vectors[i] = matP2G.Transform(Mcsf::Point3f(xRange[ix], yRange[iy], zRange[iz]));
    }

    float lbxf = vectors[0].GetX(); float rtxf = lbxf;
    float lbyf = vectors[0].GetY(); float rtyf = lbyf;
    float lbzf = vectors[0].GetZ(); float rtzf = lbzf;

    for (int i = 1; i < 8; i++)
    {
        if(lbxf > vectors[i].GetX()) lbxf = vectors[i].GetX();
        if(rtxf < vectors[i].GetX()) rtxf = vectors[i].GetX();
        if(lbyf > vectors[i].GetY()) lbyf = vectors[i].GetY();
        if(rtyf < vectors[i].GetY()) rtyf = vectors[i].GetY();
        if(lbzf > vectors[i].GetZ()) lbzf = vectors[i].GetZ();
        if(rtzf < vectors[i].GetZ()) rtzf = vectors[i].GetZ();
    }
    lbx = floor(lbxf); lby = floor(lbyf); lbz = floor(lbzf);
    rtx = ceil(rtxf); rty = ceil(rtyf); rtz = ceil(rtzf);
}

void TpsImage3DEntity::GetDimension(size_t* dim) const
{
    if (dim == nullptr) return;
    const RtImage3DHeader* pHeader = mSeries->get_header();
    if (nullptr == pHeader){
        return;
    }
    dim[0] = pHeader->m_iXDim;
    dim[1] = pHeader->m_iYDim;
    dim[2] = pHeader->m_iSliceCount;
}

void TpsImage3DEntity::GetSpacing(double* spacing) const
{
    memcpy(spacing, mSpacing, 3 * sizeof(double));
}

bool SaveToTrueColorBitmap(const std::string& path, unsigned char* buffer,
    const int& width, const int& height){

#ifdef WIN32
        //TODO: check whether the file is existed

        // calculate the pad size of whole image
        int nStride = (width*32+7)/8;
        int paddedsize = nStride*height;

        // declare bmp structures 
        BITMAPFILEHEADER bmfh;
        BITMAPINFOHEADER info;

        // and initialize them to zero
        memset ( &bmfh, 0, sizeof (BITMAPFILEHEADER ) );
        memset ( &info, 0, sizeof (BITMAPINFOHEADER ) );

        // fill the file header with data
        bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
        bmfh.bfReserved1 = 0;
        bmfh.bfReserved2 = 0;
        bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paddedsize;
        bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits

        // fill the info header

        info.biSize = sizeof(BITMAPINFOHEADER);
        info.biWidth = width;
        info.biHeight = height;
        info.biPlanes = 1;			// we only have one bit-plane
        info.biBitCount = 32;		// RGB mode is 24 bits
        info.biCompression = BI_RGB;	
        info.biSizeImage = 0;		// can be 0 for 24 bit images
        info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
        info.biYPelsPerMeter = 0x0ec4;     
        info.biClrUsed = 0;			// we are in RGB mode and have no palette
        info.biClrImportant = 0;    // all colors are important

        // now we open the file to write to
        //#ifdef _DEBUG
        //    HANDLE file = CreateFileW(sPath.c_str() , GENERIC_WRITE, FILE_SHARE_READ,
        //        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
        //#else
        std::wstring wsPath(path.begin(), path.end());
        HANDLE file = CreateFile(wsPath.c_str() , GENERIC_WRITE, FILE_SHARE_READ,
            NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
        //#endif

        if ( file == NULL )
        {
            CloseHandle ( file );
            return false;
        }

        // write file header
        unsigned long bwritten;
        if ( WriteFile ( file, &bmfh, sizeof ( BITMAPFILEHEADER ), &bwritten, NULL ) == false )
        {	
            CloseHandle ( file );
            return false;
        }
        // write info header
        if ( WriteFile ( file, &info, sizeof ( BITMAPINFOHEADER ), &bwritten, NULL ) == false )
        {	
            CloseHandle ( file );
            return false;
        }
        // write image data
        if ( WriteFile ( file, buffer, paddedsize, &bwritten, NULL ) == false )
        {	
            CloseHandle ( file );
            return false;
        }

        // and close file
        CloseHandle ( file );
#else
        sPath;
        pPixelBuffer;
        nImageWidth;
        nImageHeight;
#endif // _WINDOWS

        return true;
}
TPS_END_NAMESPACE  // end namespace tps
