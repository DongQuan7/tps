//////////////////////////////////////////////////////////////////////////
/// \defgroup McsfDatabase of Common Software Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2012
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  qiangqiang.zhou@united-imaging.com
///
///  \file        mcsf_dbwrapper_utils.cpp
///  \brief   database wrapper utils 
///
///  \version 1.0
///  \date    June 25, 2012
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "rt_tps_database_wrapper_utils.h"

//third library header
#include "boost/filesystem.hpp"//create directory
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include <windows.h>
#include <stdio.h>
#include "tps_performance.h"

//TPS
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_ct2density.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

RtDatabaseUtils::RtDatabaseUtils()
{
}

RtDatabaseUtils::~RtDatabaseUtils()
{
}

bool RtDatabaseUtils::DeleteLocalFile(const std::string& sFilePath) const
{
    namespace bf=boost::filesystem;
    if (!bf::exists(sFilePath))
    {
        TPS_LOG_DEV_WARNING << (sFilePath + "Not exists when Delete File");
        return true;
    }

#   if defined(_WIN32)
    //read only attributes
    DWORD dwAttrs = GetFileAttributes(sFilePath.c_str());
    if (dwAttrs & FILE_ATTRIBUTE_READONLY)
    {
        SetFileAttributes(sFilePath.c_str(),
            dwAttrs & ~FILE_ATTRIBUTE_READONLY);
    }
#   endif    
    try
    {
        (void)bf::remove(sFilePath);//delete file
    }
    catch (const bf::filesystem_error &e )
    {
        const std::string sError(e.what());
        TPS_LOG_DEV_WARNING << (sError);
        return false;
    }
    return true;
}

void RtDatabaseUtils::DeleteFiles(const std::vector<std::string>& FilePathArray) const
{
    for(auto itr=FilePathArray.cbegin(); itr != FilePathArray.cend(); ++itr ) 
    { 
        DeleteLocalFile(*itr);
    }
}

bool RtDatabaseUtils::CreateMultiFolder(const std::string& sFolderName) const
{
    namespace bfs = boost::filesystem;
    try{
        // create a directory, then check it for consistency
        //   take extra care to report problems, since if this fails
        //   many subsequent tests will fail
        if (bfs::is_directory(sFolderName)){
            return true;// already exists 
        }else if (bfs::exists(sFolderName)){
            return true; //is other
        }
        return bfs::create_directories(sFolderName);
    }
    catch ( const bfs::filesystem_error & x ){
        TPS_LOG_DEV_ERROR<<("***** Creating directory: " + sFolderName + " failed.          *****\n" + std::string(x.what()));
        return false;
    }
}

bool RtDatabaseUtils::WriteFile(const std::string& sFileName, const std::string& sData)
{
    FILE* pFile;
    errno_t err=fopen_s(&pFile,sFileName.c_str(),"w");
    if(0 != err){
        TPS_LOG_DEV_ERROR<<"Failed to write the file :"<<sFileName;
        return false;
    }
    fprintf(pFile,"%s",sData.c_str());
    fclose(pFile);
    return true;
}

bool RtDatabaseUtils::WriteFileInBinary(const std::string& sFilePath, const char* buffer, std::size_t size)
{
    if(nullptr == buffer || 0 == size) return false;

    std::ofstream ofs;// (sFilePath, std::ifstream::binary);
    ofs.open(sFilePath, std::ios::out | std::ios::binary);
    if (!ofs) {
        TPS_LOG_DEV_ERROR<<"Failed to open file["<<sFilePath<<"].";
        return false;
    }

    ofs.write(buffer, size);
    ofs.close();
    return true;
}

bool RtDatabaseUtils::ReadBufferFromFile(const std::string& sFilePath, char*& buffer, std::size_t* size) const
{
    buffer = nullptr;
    if(nullptr == size) return false;

    std::ifstream ifs;// (sFilePath, std::ifstream::binary);
    ifs.open(sFilePath, std::ios::in | std::ios::binary);
    *size = 0;
    if (!ifs) {
        TPS_LOG_DEV_ERROR<<"Failed to open file["<<sFilePath<<"].";
        return false;
    }

    // get pointer to associated buffer object
    std::filebuf* pbuf = ifs.rdbuf();
    // get file size using buffer's members
    *size = pbuf->pubseekoff (0,ifs.end,ifs.in);
    if (*size > 0){
        pbuf->pubseekpos (0,ifs.in);
        // allocate memory to contain file data
        buffer=new char[*size];
        // get file data
        pbuf->sgetn (buffer,*size);
    }
    ifs.close();
    // write content to stdout
    //std::cout.write (buffer,size);
    return true;
}

//bool RtDatabaseUtils::ReadBufferFromFile(const std::string& sFilePath, std::string& strBuffer) const
//{
//    char* buffer = nullptr;
//    std::size_t fileSize(0);
//    if(!this->ReadBufferFromFile(sFilePath, buffer, &fileSize)){
//        TPS_LOG_DEV_ERROR<<"Failed to open file["<<sFilePath<<"].";
//        return false;
//    }
//    if (0 == fileSize || nullptr == buffer){
//        TPS_LOG_DEV_ERROR<<"Failed to open file["<<sFilePath<<"].";
//        return false;
//    }
//
//    strBuffer = buffer;
//    DEL_ARRAY(buffer);
//    return true;
//}

//bool RtDatabaseUtils::ReadFloatBufferFromFile(const std::string& sFilePath, std::string SplitString, std::vector<float>& vFloatValue) const
//{
//    std::ifstream ifs;// (sFilePath, std::ifstream::binary);
//    ifs.open(sFilePath, std::ios::in);
//    vFloatValue.clear();
//    if (!ifs) {
//        TPS_LOG_DEV_ERROR<<"Failed to open file["<<sFilePath<<"].";
//        return false;
//    }
//
//    // get pointer to associated buffer object
//    std::filebuf* pbuf = ifs.rdbuf();
//    // get file size using buffer's members
//    std::size_t size = pbuf->pubseekoff (0,ifs.end,ifs.in);
//    if (size > 0){
//        pbuf->pubseekpos (0,ifs.in);
//        // allocate memory to contain file data
//        char* buffer=new char[size];
//        // get file data
//        pbuf->sgetn (buffer,size);
//
//        const std::string strBuffer(buffer, size);
//        DEL_ARRAY(buffer);
//        
//        std::vector<std::string> vecValue;
//        (void)boost::split(vecValue, strBuffer, boost::is_any_of(SplitString));
//        float fValue = 0.f;
//        for (auto itr=vecValue.cbegin(); itr!=vecValue.cend(); ++itr){
//            fValue = static_cast<float>(atof((*itr).c_str()));
//            vFloatValue.push_back(fValue);
//        }
//    }
//    ifs.close();
//    return true;
//}

//bool RtDatabaseUtils::ReadIntBufferFromFile(const std::string& sFilePath, std::string SplitString, std::vector<int>& vIntValue) const
//{
//    std::ifstream ifs;
//    ifs.open(sFilePath, std::ios::in);
//    vIntValue.clear();
//    if (!ifs) {
//        TPS_LOG_DEV_ERROR<<"Failed to open file["<<sFilePath<<"].";
//        return false;
//    }
//
//    // get pointer to associated buffer object
//    std::filebuf* pbuf = ifs.rdbuf();
//    // get file size using buffer's members
//    std::size_t size = pbuf->pubseekoff (0,ifs.end,ifs.in);
//    if (size > 0){
//        pbuf->pubseekpos (0,ifs.in);
//        // allocate memory to contain file data
//        char* buffer=new char[size];
//        // get file data
//        pbuf->sgetn (buffer,size);
//
//        const std::string strBuffer(buffer, size);
//        DEL_ARRAY(buffer);
//
//        std::vector<std::string> vecValue;
//        (void)boost::split(vecValue, strBuffer, boost::is_any_of(SplitString));
//        int iValue = 0;
//        for (auto itr=vecValue.cbegin(); itr!=vecValue.cend(); ++itr){
//            iValue = static_cast<int>(atoi((*itr).c_str()));
//            vIntValue.push_back(iValue);
//        }
//    }
//    ifs.close();
//    return true;
//}

bool RtDatabaseUtils::CopyFileInBinary(const std::string& sFilePathSrc, const std::string& sFilePathDst) const
{
    LPCSTR sourceFilePath = sFilePathSrc.c_str();
    LPCSTR targetFilePath = sFilePathDst.c_str();
    CopyFile(sourceFilePath, targetFilePath, FALSE);
    return true;//TODO
}

// delete by wzd 添加电子密度后该接口不能使用
//bool RtDatabaseUtils::WriteCt2densityMapToFile(const RtCt2density& ct2density, const std::string& folderPath)
//{
//    const std::string sCt2DensityFolder = folderPath + "/ct2density";
//    if (!this->CreateMultiFolder(sCt2DensityFolder)){
//        TPS_LOG_DEV_ERROR<<"failed to create folder:"<<sCt2DensityFolder;
//        return false;
//    }
//
//    std::string sData("");
//    int iCount(0);
//    const std::map<int, float>& densitymap = ct2density.get_ct2densitymap();
//    for (auto itr=densitymap.cbegin(); itr!=densitymap.cend(); ++itr, ++iCount){
//        sData += std::to_string((long long )((*itr).first)) + "," + std::to_string((long double)((*itr).second));
//        if (iCount != densitymap.size()-1){
//            sData += "|";
//        }
//    }
//    
//    const std::string sFileName = sCt2DensityFolder + "/" + ct2density.get_uid();
//    return WriteFile(sFileName, sData);
//}

//bool RtDatabaseUtils::BuildCt2densityMapFromFile(RtCt2density* ct2densityMap)
//{
//    if (ct2densityMap==nullptr) {
//        TPS_LOG_DEV_ERROR<<"ct2densityMap is null.";
//        return false;
//    }
//
//    std::string filePath = "";//ct2densityMap->get_ct2densityvaluefilepath();
//    if (filePath.empty()) {
//        TPS_LOG_DEV_ERROR<<"filepath is empty";
//        return false;
//    }
//
//    // check file existence
//    ifstream mapFile;
//    mapFile.open(filePath, ios::in);
//    if (!mapFile) {
//        TPS_LOG_DEV_ERROR<<"Failed to open file["<<filePath<<"].";
//        return false;
//    }
//
//    // split the file based on 
//    std::stringstream buffer;
//    buffer<<mapFile.rdbuf();
//    std::string bigString(buffer.str());
//    SetupCt2DensityMap_i(bigString, ct2densityMap);
//
//    mapFile.close();
//    return true;
//}

//void RtDatabaseUtils::SetupCt2DensityMap_i(const std::string& strLongString, RtCt2density* ct2densityMap)
//{
//    std::vector<std::string> vecValue;
//    (void)boost::split(vecValue, strLongString, boost::is_any_of("|"));
//
//    std::vector<int> vCT;
//    std::map<int, float> refMap;//= ct2densityMap->GetCt2densitymap();
//    for (std::vector<std::string>::iterator it = vecValue.begin();it != vecValue.end(); ++it){
//        string strValue = *it;
//        size_t iPos = strValue.find(',');
//        std::string sKey = strValue.substr(0,iPos);
//        int iCT = atoi(sKey.c_str());
//
//        vCT.push_back(iCT);
//        float fDensity = static_cast<float>(atof(strValue.substr(iPos + 1,strValue.length() - iPos -1).c_str()));
//        refMap.insert(std::make_pair(iCT,fDensity));
//    }
//
//    ct2densityMap->set_ct2densitymap(refMap);
//
//    int iTemp = 0;
//    size_t iSize = vCT.size();
//    //Sort
//    for (int i=0;i<iSize - 1;i++){
//        for (int j=0;j<iSize - i - 1;j++){
//            if (vCT[j] > vCT[j+1]){
//                iTemp = vCT[j];
//                vCT[j] = vCT[j + 1];
//                vCT[j + 1] = iTemp;
//            }
//        }
//    }
//    float* refBuffer = const_cast<float*>(ct2densityMap->get_ct2densitybuffer());
//    for (int i = -1024; i <5000; ++i){
//        refBuffer[i+1024] = GetDensitybyCTval_i(i, vCT, refMap);
//    }
//}

//float RtDatabaseUtils::GetDensitybyCTval_i(int ct, const std::vector<int>& vCT, std::map<int, float>& ct2densMap)
//{
//    float f = 0.0f;
//    size_t iSize = vCT.size();
//    for (int i = 0;i< iSize - 1; ++i){
//        if (ct <= vCT[0]){
//            f = 0.0f;
//        }
//        else if (ct >= vCT[iSize -1]){
//            f = ct2densMap[vCT[iSize-1]];
//            break;
//        }
//        else if (ct >= vCT[i] && ct < vCT[i+1]){
//            f = ct2densMap[vCT[i]] + (float)(ct - vCT[i])/(float)(vCT[i+1] - vCT[i]) * (ct2densMap[vCT[i+1]] - ct2densMap[vCT[i]]);
//            break;
//        }
//    }
//    return f;
//}


bool RtDatabaseUtils::BuildImage3DFromSeries(RtSeries* pRtSeries, std::vector<RtImage*> imageList, unsigned long ulSize)
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;

    if (nullptr == pRtSeries)
    {
        return false;
    }
    const int iImageSize = (const int)imageList.size();
    if (iImageSize < 3)
    {
        TPS_LOG_DEV_ERROR<<"The image count is less than 3, can not convert to image 3D.";
        return false;
    }

    //pImage3d->SetSeriesUID(pRtSeries->get_uid());
    //set patient position
    //pImage3d->set_patientposition(pRtSeries->get_patientposition());

    RtImage3DHeader* imageHeader = pRtSeries->get_header();
    imageHeader->m_vScanInfoList.clear();

    imageHeader->m_vScanInfoList.resize(iImageSize);

    //add by wzd patientposition赋值
    memcpy(imageHeader->m_cPatientPosition, pRtSeries->get_patientposition().c_str(), 6);
    memcpy(imageHeader->m_cModality,pRtSeries->get_modality().c_str(),6);
    //  imageHeader->m_tSeriesTime = pRtSeries->get_seriestime();
    imageHeader->m_tAcquisitionDateTime = boost::posix_time::to_simple_string(
        pRtSeries->get_acquisitiontime());
    imageHeader->m_sFrameOfReferenceUID = pRtSeries->get_frameofreferenceuid();

    RtImage* pFirstImage = *(imageList.cbegin());
    if (nullptr == pFirstImage){
        TPS_LOG_DEV_ERROR<<"pFirstImage is null.";
        return false;
    }
    imageHeader->m_dScaleSlope = (double)atof(pFirstImage->get_rescaleslope().c_str());
    imageHeader->m_dScaleIntercept = (double)atof(pFirstImage->get_rescaleintercept().c_str());
    imageHeader->m_iSliceCount = iImageSize;
    imageHeader->m_iXDim = pFirstImage->get_columns();
    imageHeader->m_iYDim = pFirstImage->get_rows();
    imageHeader->m_iBitsAllocated = pFirstImage->get_bitsallocated();

    //check size
    int iXDim = imageHeader->m_iXDim;
    int iYDim = imageHeader->m_iYDim;
    int iSliceCount = imageHeader->m_iSliceCount;
    int iBitsAllocated = imageHeader->m_iBitsAllocated;
    //// Total size of all Pixel Data 
    unsigned long lTotalSize = iXDim * iYDim * iSliceCount * (iBitsAllocated >> 3);
    if (ulSize != lTotalSize){
        TPS_LOG_DEV_ERROR<<"Failed to BuildImage3DFromSeries as total size wrong.";
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    std::vector<std::string> vecValue;
    std::string sData = pFirstImage->get_pixelspacing();
    (void)boost::split(vecValue, sData, boost::is_any_of("\\"));
    if (2 == vecValue.size())
    {
        imageHeader->m_dXSize = (double)atof(vecValue[0].c_str());
        imageHeader->m_dYSize = (double)atof(vecValue[1].c_str());
    }

    vecValue.clear();
    sData = pFirstImage->get_windowwidth();
    (void)boost::split(vecValue, sData, boost::is_any_of("\\"));
    if (vecValue.size() > 0){  
        imageHeader->m_iWindowWidth = atoi(vecValue[0].c_str());
    }

    vecValue.clear();
    sData = pFirstImage->get_windowcenter();
    (void)boost::split(vecValue, sData, boost::is_any_of("\\"));
    if (vecValue.size() > 0)
    {  
        imageHeader->m_iWindowCenter = atoi(vecValue[0].c_str());
    }

    imageHeader->m_iDataType = pFirstImage->get_pixelrepresentation();

    vecValue.clear();
    sData = pFirstImage->get_imageorientationpatient();
    (void)boost::split(vecValue, sData, boost::is_any_of("\\"));
    if (6 == vecValue.size())
    {
        imageHeader->m_ImageXOrientation =  db_Vector3f(
            (float)atof(vecValue[0].c_str()),
            (float)atof(vecValue[1].c_str()),
            (float)atof(vecValue[2].c_str()));

        imageHeader->m_ImageYOrientation =  db_Vector3f(
            (float)atof(vecValue[3].c_str()),
            (float)atof(vecValue[4].c_str()),
            (float)atof(vecValue[5].c_str()));
    }

    int i =0;

    for (auto iterScanInfo = imageList.cbegin(); iterScanInfo != imageList.cend(), i<iImageSize; ++iterScanInfo, ++i)
    {
        std::vector<std::string> vecValue;
        std::string sData = (*iterScanInfo)->get_imagepositionpatient();
        (void)boost::split(vecValue, sData, boost::is_any_of("\\"));
        if (3 == vecValue.size()){
            db_Point3f pt(
                (float)atof(vecValue[0].c_str()),
                (float)atof(vecValue[1].c_str()),
                (float)atof(vecValue[2].c_str()));
            imageHeader->m_vScanInfoList[i].m_ImagePosition = pt;
        }
        imageHeader->m_vScanInfoList[i].m_uiInstanceNumber = (*iterScanInfo)->get_instancenumber();
        imageHeader->m_vScanInfoList[i].m_sUID = (*iterScanInfo)->get_uid();
        imageHeader->m_vScanInfoList[i].m_SliceLocation = (float)atof((*iterScanInfo)->get_slicelocation().c_str());
        imageHeader->m_vScanInfoList[i].m_BitsStored = (*iterScanInfo)->get_bitsstored();
    }

    //z spacing;
    db_Vector3f ptBegin(imageHeader->m_vScanInfoList[0].m_ImagePosition);
    db_Vector3f ptEnd(imageHeader->m_vScanInfoList[iImageSize-1].m_ImagePosition);
    db_Vector3f vec = ptEnd - ptBegin;
    float distance = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    if (abs(distance) <= 1e-6)
    {
        TPS_LOG_DEV_ERROR<<"The image position of first & last image in the series is the same.";
        return false;
    }
    else{
        imageHeader->m_fSliceSpacing = static_cast<float>(distance / (imageHeader->m_iSliceCount -1));
    }

    //////////////////////////////////////////////////////////////////////////
    //for more image set information
    pRtSeries->set_slicethickness(pFirstImage->get_slicethickness());

    bool bRet = CheckImage3D(pRtSeries);
    TEST_PERFORMANCE_END("RtDatabaseUtils::BuildImage3DFromSeries");
    return bRet;
}

//only for the same slice thickness of each slice
bool RtDatabaseUtils::CheckImage3D(RtSeries* pRtSeries)
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;

    RtImage3DHeader* pHeader = pRtSeries->get_header();
    const int imageSize = (int)pHeader->m_vScanInfoList.size();
    if (imageSize < 3)
    {
        TPS_LOG_DEV_ERROR<<"The image count is less than 3, can not convert to image 3D.";
        return false;
    }

    db_Vector3f ptBegin(pHeader->m_vScanInfoList[0].m_ImagePosition);
    db_Vector3f ptEnd(pHeader->m_vScanInfoList[imageSize-1].m_ImagePosition);
    db_Vector3f vec = ptEnd - ptBegin;
    float fSliceSpacing = 100000.f;
    float distance = 0.f;
    for(int index = 0; index < imageSize; ++index)
    {
        if (index < imageSize - 1)
        {
            ptBegin = pHeader->m_vScanInfoList[index].m_ImagePosition;
            ptEnd = pHeader->m_vScanInfoList[index + 1].m_ImagePosition;
            vec = ptEnd - ptBegin;
            distance = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
            if (distance < fSliceSpacing)
            {
                fSliceSpacing = distance;
            }
        }
    }
    if (fSliceSpacing < 1e-6)
    {
        TPS_LOG_DEV_INFO<<"fSliceSpacing is zero.";
        return false;
    }
    fSliceSpacing = sqrt(fSliceSpacing);

    ptBegin = pHeader->m_vScanInfoList[0].m_ImagePosition;
    ptEnd = pHeader->m_vScanInfoList[imageSize-1].m_ImagePosition;
    vec = ptEnd - ptBegin;
    distance = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    int iSliceCountNew = (int)(distance / fSliceSpacing + 0.5f) + 1;
    if(imageSize >= iSliceCountNew)
    {
        TPS_LOG_DEV_INFO<<"Do not need to modify image 3d data.";
        return true;
    }


    const int iBitsAllc = pHeader->m_iBitsAllocated;
    const int iPixelRepresentation = pHeader->m_iDataType;
    int szTypeLength = 0;
    int iDataType = 0;
    if (iBitsAllc == 8 && iPixelRepresentation == 0)
    {//8
        szTypeLength = sizeof(unsigned char);
        iDataType = 1;
    }
    else if (iBitsAllc == 16 && iPixelRepresentation == 0)
    {//16
        szTypeLength = sizeof(unsigned short);
        iDataType = 2;
    }
    else if (iBitsAllc == 16 && iPixelRepresentation == 1)
    {
        szTypeLength = sizeof(short);
        iDataType = 3;
    }
    else
    {
        TPS_LOG_DEV_ERROR<<"Unsupported data type which bit allocated: "
            <<iBitsAllc<<" pixel representation: "<<iPixelRepresentation;
        return false;
    }

    bool* pInter = new bool[iSliceCountNew];
    memset(pInter, 0, sizeof(bool) * iSliceCountNew);

    unsigned long ulSize(0);
    char* pData = pRtSeries->get_imagedata(&ulSize);

    int iBitsAllocated = pHeader->m_iBitsAllocated;
    
    //// Total size of all Pixel Data 
    const int iDimSlice = pHeader->m_iXDim * pHeader->m_iYDim * (iBitsAllocated >> 3);
    const unsigned long lTotalSize = iDimSlice * iSliceCountNew;
    char* pDataResize = new char[lTotalSize];
    memset(pDataResize, 0, lTotalSize);
    int iTotalInter = 0;
    float fStep = 0.f;

    for(int index = 0; index < imageSize; ++index)
    {
        memcpy(pDataResize+ iDimSlice * (index + iTotalInter), pData + iDimSlice*index, iDimSlice);
        if (index < imageSize - 1)
        {
            ptBegin = pHeader->m_vScanInfoList[index].m_ImagePosition;
            ptEnd = pHeader->m_vScanInfoList[index + 1].m_ImagePosition;
            vec = ptEnd - ptBegin;
            distance = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
            int InterPolationCount = (int)(distance / fSliceSpacing + 0.5f) - 1;
            if (InterPolationCount > 1)
            {
                ////pHeader->m_vScanInfoList[index].m_iInterCount = InterPolationCount;
                fStep = 1 / ((float)InterPolationCount + 1);
                for (int i = 0; i < InterPolationCount; ++i)
                {
                    pInter[index + iTotalInter + i] = true;

                    for (int j=0; j < iDimSlice;)
                    {
                        //a kind of Sigmoid 1 - 1. / (1+1000*exp((0.5-16*0.1))) matlab tested
                        //it's better then linear f(x) = 1 -  i* fStep - fStep
                        float fWeight = (float)(1 - 1 / (1 + 1000*exp(0.5 - 16 * i * fStep)));
                        if (1 == iDataType)//unsigned char
                        {
                            unsigned char* pImageDataFrist = (unsigned char*)(pData + iDimSlice*index + j);
                            unsigned char* pImageDataSecond = (unsigned char*)(pData + iDimSlice*(index + 1) + j);
                            unsigned char* pImageData = (unsigned char*)(pDataResize+ iDimSlice * (index + iTotalInter + i + 1) + j);
                            *pImageData = (unsigned char)((float)(*pImageDataFrist) * fWeight) + 
                                (unsigned char)((float)(*pImageDataSecond) * (1 - fWeight));
                        }
                        else //unsigned short and short
                        {
                            short* pImageDataFrist = (short*)(pData + iDimSlice*index + j);
                            short* pImageDataSecond = (short*)(pData + iDimSlice*(index + 1) + j);
                            short* pImageData = (short*)(pDataResize+ iDimSlice * (index + iTotalInter + i + 1) + j);
                            *pImageData = (short)((float)(*pImageDataFrist) * fWeight) + 
                                (short)((float)(*pImageDataSecond) * (1 - fWeight));
                        }

                        j += szTypeLength;
                    }
                }

                iTotalInter += InterPolationCount;
            }
        }
    }

    pRtSeries->set_imagedata(pDataResize, lTotalSize);
    pHeader->SetInterpolation(pInter, iSliceCountNew);
    DEL_ARRAY(pDataResize);
    DEL_ARRAY(pInter);

    TEST_PERFORMANCE_END("RtDatabaseUtils::CheckImage3D");
    return true;
}

RT_TPS_DATABASE_END_NAMESPACE;
