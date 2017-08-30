////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_da_gallery_imagemanager.cpp
///  \brief   
///
///  \version 1.0
///
///  \date    Dec. 29, 2014
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"

//TPS
#include "tps_logger.h"

#include "RtTpsFramework/tps_fw_image3dentity.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_ct2density.h"

TPS_BEGIN_NAMESPACE

TpsGalleryImageManager::TpsGalleryImageManager(void) : 
    mIsSeries4DPageturned(false), mIsPlaying(false),
    mCt2density(nullptr), mCt2densitySize(0), 
    mCurrentCt2DensityUID(""), mCurrentDataHeaderElement(nullptr), mIsVisibleOfImageText(true){

} 

TpsGalleryImageManager::~TpsGalleryImageManager(void){
    auto it = mMapImage3DEntity.begin();
    while(it != mMapImage3DEntity.end()){  
        if(nullptr != it->second){
            delete it->second;
            it->second = nullptr;
        }
        mMapImage3DEntity.erase(it++);
    }

    auto itOriginal = mMapOriginalImage3DEntity.begin();
    while(itOriginal != mMapOriginalImage3DEntity.end()){  
        //itOriginal->second 已经在mMapImage3DEntity中清空
        mMapOriginalImage3DEntity.erase(itOriginal++);
    }

    auto itSeries = mMapImageSeries.begin();
    while(itSeries != mMapImageSeries.end()){  
        if(nullptr != itSeries->second){
            delete itSeries->second;
            itSeries->second = nullptr;
        }
        mMapImageSeries.erase(itSeries++);
    }

    auto itimgtxt = mMapImageTextData.begin();
    for (; itimgtxt!=mMapImageTextData.end(); itimgtxt++){
        delete itimgtxt->second;
    }

    if(nullptr != mCt2density){
        delete[] mCt2density;
        mCt2density = nullptr;
    }

    mDeletedImageMap.clear();
    m_mapSeriesGroup4d.clear();
}

void TpsGalleryImageManager::ClearManager(){
    auto it = mMapImage3DEntity.begin();
    while(it != mMapImage3DEntity.end()){  
        if(nullptr != it->second){
            delete it->second;
            it->second = nullptr;
        }
        mMapImage3DEntity.erase(it++);
    }
    mMapImage3DEntity.clear();

    auto itOriginal = mMapOriginalImage3DEntity.begin();
    while(itOriginal != mMapOriginalImage3DEntity.end()){  
        //itOriginal->second 已经在mMapImage3DEntity中清空
        mMapOriginalImage3DEntity.erase(itOriginal++);
    }
    mMapOriginalImage3DEntity.clear();

    auto itSeries = mMapImageSeries.begin();
    while(itSeries != mMapImageSeries.end()){  
        if(nullptr != itSeries->second){
            delete itSeries->second;
            itSeries->second = nullptr;
        }
        mMapImageSeries.erase(itSeries++);
    }
    mMapImageSeries.clear();

    auto itimgtxt = mMapImageTextData.begin();
    for (; itimgtxt!=mMapImageTextData.end(); itimgtxt++){
        delete itimgtxt->second;
        itimgtxt->second = nullptr;
    }
    mMapImageTextData.clear();

    if(nullptr != mCt2density){
        delete[] mCt2density;
        mCt2density = nullptr;
    }

    mIsVisibleOfImageText= true;
    mCurrentImage3DUID = "";

    mDeletedImageMap.clear();
    m_mapSeriesGroup4d.clear();
}

void TpsGalleryImageManager::ClearImageData(const std::string &sSeriesUID){
    auto it = mMapImage3DEntity.find(sSeriesUID);
    if (it != mMapImage3DEntity.end()){
        delete it->second;
        it->second = nullptr;
        mMapImage3DEntity.erase(it);
    }

    auto itOriginal = mMapOriginalImage3DEntity.find(sSeriesUID);
    if (itOriginal != mMapOriginalImage3DEntity.end()){
        //itOriginal->second 已经在mMapImage3DEntity中清空
        mMapOriginalImage3DEntity.erase(itOriginal);
    }

    auto itSeries = mMapImageSeries.find(sSeriesUID);
    if (itSeries != mMapImageSeries.end()){
        delete itSeries->second;
        itSeries->second = nullptr;
        mMapImageSeries.erase(itSeries);
    }

    auto itImgtxt = mMapImageTextData.find(sSeriesUID);
    if (itImgtxt != mMapImageTextData.end()){
        delete itImgtxt->second;
        itImgtxt->second = nullptr;
        mMapImageTextData.erase(itImgtxt);
    }
    if (mCurrentImage3DUID == sSeriesUID){
        mCurrentImage3DUID = "";
    }

    auto itImageUIDs = mDeletedImageMap.find(sSeriesUID);
    if (itImageUIDs != mDeletedImageMap.end()){
        mDeletedImageMap.erase(itImageUIDs);
    }
}

void TpsGalleryImageManager::SetSeries4DPageturned(){
    mIsSeries4DPageturned = true;
}

bool TpsGalleryImageManager::GetIsSeries4DPageturned(){
    return mIsSeries4DPageturned;
}

void TpsGalleryImageManager::InitSeries4DUidList(const std::vector<std::string>& seriesUidList){

    // Clear this vector each time we load a new 4DCT Series Group so that 
    // we can keep tracking the current Series4D Uid List regardless of 
    // the possible previously loaded 4DCT
    mVecSeries4DUidList.clear();
    std::vector<std::string>().swap(mVecSeries4DUidList);

    // add 4DCT series uid into the container
    std::for_each(seriesUidList.begin(), seriesUidList.end(),
        [&](std::string item){
            auto iter = mMapImageSeries.find(item);
            if(iter != mMapImageSeries.end()){
                mVecSeries4DUidList.push_back(iter->first);
            }
    }
    );
}

void TpsGalleryImageManager::GetSeries4DUidList(std::vector<std::string>& seriesUidList){
    seriesUidList = mVecSeries4DUidList;
}

bool TpsGalleryImageManager::IsSeriesInSeries4D(const std::string& seriesUid){
    int index = 0;
    for(; index < mVecSeries4DUidList.size(); ++index){
        if(seriesUid == mVecSeries4DUidList[index]) break;
    }
    if(index != mVecSeries4DUidList.size()){ // found
        return true;
    }
    return false;
}

bool TpsGalleryImageManager::IsSeriesExists(const std::string& sImage3DUid){
    auto it = mMapImage3DEntity.find(sImage3DUid);
    if(it == mMapImage3DEntity.end()){
        TPS_LOG_DEV_ERROR<<"The Series with Uid:"<<sImage3DUid<<" does not exist.";
        return false;
    }
    return true;
}

void TpsGalleryImageManager::SetIsPlaying(bool isPlaying){
    mIsPlaying = isPlaying;
}

bool TpsGalleryImageManager::GetIsPlaying(){
    return mIsPlaying;
}  

TpsImage3DEntity* TpsGalleryImageManager::CreateImage3D(){
    return new TpsImage3DEntity();
}

bool TpsGalleryImageManager::AddImage3D(const std::string& sImage3DUid, 
    TpsImage3DEntity* image3DEntity){

        if (sImage3DUid.empty()){
            TPS_LOG_DEV_ERROR<<"sImage3DUid is empty.";
            return false;
        }

        ////NOTE! should delete pImage3D later
        //mMapImage3DEntity.insert(
        //    std::map<std::string, TpsImage3DEntity*>::value_type(
        //    sImage3DUid, image3DEntity));
        if(nullptr != mMapImage3DEntity[sImage3DUid]){
            delete mMapImage3DEntity[sImage3DUid];
        }
        mMapImage3DEntity[sImage3DUid] = image3DEntity;

        auto itDeletedImage = mDeletedImageMap.find(sImage3DUid);
        if (itDeletedImage == mDeletedImageMap.end()) {
            mDeletedImageMap.insert(
                pair<std::string, std::vector<std::string>>(
                sImage3DUid, std::vector<std::string>()));
        }
        else {
            mDeletedImageMap[sImage3DUid] = std::vector<std::string>();
        }
        

        return true;
}

bool TpsGalleryImageManager::AddDataHeader(RtSeries* series){
    if(series == nullptr) return false;
    std::string seriesUid = series->get_uid();
    std::map<string, Mcsf::IDataHeaderElementMap*> dataheader_map;

    //todo waiting for new interface
//     if(!series->LoadAllRelatedImageFileInfo(dataheader_map)){
//         return false;
//     }

    mAllSeriesDataHeaderMap[seriesUid] = dataheader_map;
    return true;
}

bool TpsGalleryImageManager::DeleteImage3D(const std::string& sImage3DUid){

    if (sImage3DUid.empty()){
        TPS_LOG_DEV_ERROR<<"sImage3DUid is empty.";
        return false;
    }

    auto it = mMapImage3DEntity.find(sImage3DUid);
    if(it == mMapImage3DEntity.end()){
        TPS_LOG_DEV_WARNING << "imageUid does not exist!";
        return false;
    }

    if(it->second != nullptr){
        delete it->second;
        it->second = nullptr;
        mMapImage3DEntity.erase(it);
    }

    return true;
}

// When Loading, Call this Method
bool TpsGalleryImageManager::AddImageSeries(RtSeries* series/*const std::string &sSeriesUid,
    const RtSeries& series*/){

        //if(sSeriesUid.empty()){
        //    TPS_LOG_DEV_ERROR<<"sSerisesUid is empty!";
        //    return false;
        //}
        //auto it = mMapImageSeries.find(sSeriesUid);
        //if(it == mMapImageSeries.end()){
        //    RtSeries *ser = new RtSeries(series);
        //    mMapImageSeries.insert(
        //        std::map<std::string,RtSeries*>::value_type(
        //        sSeriesUid,ser));

        //    //TODO to add load all ct2density maps from TMS
        //    int iSize =0;
        //    if(!this->GetCT2DensitySize(&iSize)){
        //        TPS_LOG_DEV_ERROR<<"GetCT2DensitySize failed.";
        //    }
        //    return true;
        //}

        //return false;

        std::string sSeriesUid = series->get_uid();
        if(sSeriesUid.empty()){
            TPS_LOG_DEV_ERROR<<"sSerisesUid is empty!";
            return false;
        }
        auto it = mMapImageSeries.find(sSeriesUid);
        // rcc: if series obj already exist, remove it firstly
        if(it != mMapImageSeries.end()) {
            TPS_LOG_DEV_WARNING<<"series["<<sSeriesUid
                <<"] already exist. The old one will removed firstly.";
            delete it->second;
            mMapImageSeries.erase(it);
        }

        // rcc: deep copy
        mMapImageSeries[sSeriesUid] = new RtSeries(*series);
        return true;
}

bool TpsGalleryImageManager::DeleteImageSeries(const std::string &sSeriesUid){

    if(sSeriesUid.empty()){
        TPS_LOG_DEV_ERROR<<"sSerisesUid is empty!";
        return false;
    }
    auto it = mMapImageSeries.find(sSeriesUid);
    if(it == mMapImageSeries.end()){
        TPS_LOG_DEV_WARNING << "sSeriesUid does not exist!";
        return false;
    }

    DeleteImage3D(sSeriesUid);

    if(it->second != nullptr){
        delete it->second;
        it->second = nullptr;
        mMapImageSeries.erase(it);
    }

    return true;
}

bool TpsGalleryImageManager::GetImage3DData(const std::string& sImage3DUid, 
    TpsImage3DEntity** image3DData) const{

        auto it = mMapImage3DEntity.find(sImage3DUid);
        if(it == mMapImage3DEntity.end()){
            TPS_LOG_DEV_ERROR<<"[ERROR][ImageDataManager][GetImage3DData]: "
                <<"The RTFWKDLDOImage3D instance with UID: "
                <<sImage3DUid<<" does not exist.";
            return false;
        }

        *image3DData = it->second;
        return true;
}

bool TpsGalleryImageManager::GetImageSeries(const std::string &sSeriesUid,
    RtSeries **series) const{

        auto it = mMapImageSeries.find(sSeriesUid);
        if(it == mMapImageSeries.end()){
            TPS_LOG_DEV_ERROR<<"RTFWKDLDOSeries does not exist.";
            return false;
        }

        *series = it->second;
        return true;
}        

void TpsGalleryImageManager::SetCurrentImage3D(const std::string& seriesUid){
    mCurrentImage3DUID = seriesUid;
    RtSeries* series;
    if(!GetImageSeries(seriesUid, &series)){
        TPS_LOG_DEV_INFO<<"Failed to get series with UID:"<<seriesUid;
        return;
    }
    if(series->get_modality() == "PT"){
        auto it = mAllSeriesDataHeaderMap.find(mCurrentImage3DUID);
        if(it != mAllSeriesDataHeaderMap.end()){
            mCurrentSliceDataHeaderMap = it->second;
        }else{
            TPS_LOG_DEV_INFO<<"series not found in AllSeriesDataHeaderMap";
        }
    }
}

bool TpsGalleryImageManager::GetCurrentImage3DEntity(std::string& pImage3DUid, TpsImage3DEntity** image3DData) const{
    pImage3DUid = mCurrentImage3DUID;
    return GetImage3DData(mCurrentImage3DUID, image3DData);
}

bool TpsGalleryImageManager::GetSeriesUIDList(const std::string& sPatient,
    std::vector<std::string>* pSerUIDList) const{
        if (nullptr == pSerUIDList){
            TPS_LOG_DEV_ERROR<<"pSerUIDList is null pointer.";
            return false;
        }
        sPatient;
        //TODO: current version will not use patient for filtering
        std::string sSerUID;
        auto it = mMapImage3DEntity.begin();
        while(it != mMapImage3DEntity.end()){
            if(nullptr != it->second){
                sSerUID = it->second->GetUID();
                pSerUIDList->push_back(sSerUID);
            }
            it++;
        }
        return true;
}

bool TpsGalleryImageManager::GetSeriesUIDList(std::vector<std::string>& vSerUIDList) const{
        vSerUIDList.clear();
        //TODO: current version will not use patient for filtering
        std::string sSerUID;
        auto it = mMapImage3DEntity.begin();
        while(it != mMapImage3DEntity.end()){
            if(nullptr != it->second){
                sSerUID = it->second->GetUID();
                vSerUIDList.push_back(sSerUID);
            }
            it++;
        }
        return true;
}

//void TpsGalleryImageManager::GetSeriesUIDListByCourseUID(const std::string &sCourseUID, std::vector<std::string>& seriesUIDList)
//{
//     seriesUIDList.clear();
//     for (auto it = mMapImageSeries.begin(); it != mMapImageSeries.end(); ++it) {
//         if (nullptr != it->second){
//             if (it->second->get_courseuid() == sCourseUID){
//                 seriesUIDList.push_back(it->second->get_uid());
//             }
//         }
//     }
//}

std::vector<std::string> TpsGalleryImageManager::GetSeriesUidListByPatientUid(const std::string& patientUid)
{
    std::vector<std::string> seriesUidList;
    for (auto it = mMapImage3DEntity.begin(); it != mMapImage3DEntity.end(); ++it)
    {
        if (it->second == nullptr || it->second->GetRtSeries() == nullptr) continue;
        if (it->second->GetRtSeries()->get_patientuid() == patientUid)
        {
            seriesUidList.push_back(it->first);
        }
    }
    return seriesUidList;
}

//std::vector<std::string> TpsGalleryImageManager::GetSeriesUidListViaCourseUid(const std::string& courseUid)
//{
//    std::vector<std::string> seriesUidList;
//    for (auto it = mMapImage3DEntity.begin(); it != mMapImage3DEntity.end(); ++it){
//        if (it->second == nullptr || it->second->GetRtSeries() == nullptr) continue;
//        if (it->second->GetRtSeries()->get_courseuid() == courseUid)
//        {
//            seriesUidList.push_back(it->first);
//        }
//    }
//    return seriesUidList;
//}

std::string TpsGalleryImageManager::GetCurrentImage3DUID() const{
    return mCurrentImage3DUID;
}

bool TpsGalleryImageManager::AddImageTextData(const std::string& imageTextID,
    unsigned char* pImageTextBuffer, int bufWidth, int bufHeight){
        // for concurrency
        boost::lock_guard<boost::mutex> lockGuard(mMutexTextObj);

        if (pImageTextBuffer==nullptr || bufHeight<=0 || bufWidth<=0){
            TPS_LOG_DEV_ERROR<<"pImageTextBuffer==nullptr || bufHeight<=0 || bufWidth<=0";
            return false;
        }

        auto itimgtxt = mMapImageTextData.find(imageTextID);

        if (itimgtxt != mMapImageTextData.end()){
            TPS_LOG_TRACE_WARNING<<"imageTextID["<<imageTextID
                <<"] already exists in current data manager!";
            return true;
        }
        TpsImageTextData* pNewImgTxtData = 
            new TpsImageTextData(pImageTextBuffer, bufWidth, bufHeight);

        mMapImageTextData[imageTextID] = pNewImgTxtData;
        return true;
}

bool TpsGalleryImageManager::UpdateImageTextData(const std::string& imageTextID,
    TpsImageTextData* pTextObj){
        // for concurrency
        boost::lock_guard<boost::mutex> lockGuard(mMutexTextObj);

        if (pTextObj==nullptr){
            TPS_LOG_DEV_ERROR<<"pTextObj==nullptr";
            return false;
        }

        auto itimgtxt = mMapImageTextData.find(imageTextID);
        if (itimgtxt==mMapImageTextData.end()){
            TPS_LOG_DEV_ERROR<<"imageTextID NOT exists in current data manager!";
            return false;
        }

        // delete old buffer
        //delete itimgtxt->second;
        mMapImageTextData.erase(itimgtxt);
        mMapImageTextData[imageTextID] = pTextObj;

        return true;
}

bool TpsGalleryImageManager::GetImageTextData(const std::string& imageTextID,
    unsigned char*& pImageTextBuffer, int& bufWidth, int& bufHeight){
        // for concurrency
        boost::lock_guard<boost::mutex> lockGuard(mMutexTextObj);

        auto itimgtxt = mMapImageTextData.find(imageTextID);
        if (itimgtxt == mMapImageTextData.end()){
            TPS_LOG_DEV_ERROR<<"imageTextID["<<imageTextID
                <<"] DOES NOT exists in current data manager!";
            return false;
        }

        pImageTextBuffer = itimgtxt->second->mpTextData;
        bufWidth = itimgtxt->second->mTextDataBufWidth;
        bufHeight = itimgtxt->second->mTextDataBufHeight;

        return true;
}

bool TpsGalleryImageManager::GetImage3DHeader(const std::string& serUID, 
    RtImage3DHeader*& pHeader){
        pHeader = nullptr;
        TpsImage3DEntity* pImg3D = nullptr;
        if (!GetImage3DData(serUID, &pImg3D)){
            TPS_LOG_DEV_ERROR<<"Failed to GetImage3DData. uid["<<serUID<<"].";
            return false;
        }

        if (pImg3D==nullptr || !pImg3D->GetImage3DHeader(pHeader)){
            TPS_LOG_DEV_ERROR<<"Failed to GetImage3DHeader. uid["<<serUID<<"].";
            return false;
        }

        return true;
}

bool TpsGalleryImageManager::GetCT2DensitySize(int* ct2densitySize){
    //todo resume this part
    ct2densitySize;
    return false;
//     if (nullptr != mCt2density && mCt2densitySize > 0){
//         *ct2densitySize = mCt2densitySize;
//         return true;
//     }
// 
//     std::vector<std::string> vCT2DensityUID;
//     if (!RTFWK::RTFWKDLDataAccess::GetInstance()->GetAllCT2DensityUIDList(vCT2DensityUID)) {
//         TPS_LOG_DEV_ERROR<<"RTFWKDLDataAccess::GetAllCT2DensityUIDList Failed.";
//         return false;
//     }
// 
//     if (vCT2DensityUID.empty()){
//         TPS_LOG_DEV_WARNING<<"No CT2Denstity in TMS using default Ct2Density map";
//         CreateDefaultCt2Density();
//         *ct2densitySize = mCt2densitySize;
//         return true;
//     }
// 
//     std::string defaultUID = vCT2DensityUID.front();
//     if(defaultUID.empty()){
//         TPS_LOG_DEV_ERROR<<"No CT2Denstity in TMS";
//         return false;
//     }
//     mCurrentCt2DensityUID = defaultUID;
// 
//     RTFWK::RTFWKDLDOCT2DensityMap* ctMap = new RTFWK::RTFWKDLDOCT2DensityMap();
//     ctMap->SetDBUID(defaultUID);
//     if (!RTFWK::RTFWKDLDataAccess::GetInstance()->LoadObjectFromDB(ctMap)){
//         TPS_LOG_DEV_ERROR<<"LoadObjectFromDB CT2Denstity failed!";
//         delete ctMap;
//         ctMap = nullptr;
//         return false;
//     }
// 
//     std::map<int, float> mapCT2Density = ctMap->GetCT2DensityKeyValue();
//     if (mapCT2Density.size() < 1){
//         TPS_LOG_DEV_ERROR<<"GetCT2Density failed!";
//         delete ctMap;
//         ctMap = nullptr;
//         return false;
//     }
// 
//     mCt2density = new float[mapCT2Density.size()];
//     int count = 0;
//     for (auto itr = mapCT2Density.begin(); itr != mapCT2Density.end();++itr){
//         mCt2density[count++] = (*itr).second;//  * 1000;//float ==> unsigned short
//     }
// 
//     *ct2densitySize = count;
//     mCt2densitySize = count;
//     delete ctMap;
//     ctMap = nullptr;
// 
//     return true;
}

float TpsGalleryImageManager::GetDefaultCt2Density(float ct) {
    float density = 0.0f;
    if(ct < -1000.0f){
        density = 0.0f; 
    }
    else if( ct < 0.0f){
        density = 1.0f + 0.001f *ct;
    }
    else if(ct < 1200.0){
        density = 1.0f + 0.0005f*ct;
    }
    else if(ct < 1700.0){
        density = 1.6f + 0.0004f*(ct - 1200.0f);
    }
    else if(ct < 2000.0){
        density = 1.8f + 0.003f*(ct - 1700);
    }
    else{
        density = 7.0f;
    }

    return density;
}

void TpsGalleryImageManager::CreateDefaultCt2Density(){

    const int iSize = 6024;
    mCt2density = new float[iSize];
    int count = 0;
    for (int i=0; i<iSize; ++i){
        mCt2density[count++] = GetDefaultCt2Density(i-1024);
    }
    mCt2densitySize = count;
}

bool TpsGalleryImageManager::GetCT2Density(float* pCt2density){
    if(nullptr == pCt2density) 
        return false;

    if (nullptr != mCt2density){
        memcpy(pCt2density, mCt2density,sizeof(float)*mCt2densitySize);
        return true;
    }

    int ct2densitySize = 0;
    if(mCt2densitySize < 1){
        if(!this->GetCT2DensitySize(&ct2densitySize)){
            return false;
        }
    }

    memcpy(pCt2density, mCt2density,sizeof(float)*mCt2densitySize);

    return true;
}

float* TpsGalleryImageManager::GetCurrentCT2Density(std::string* sCurrentCt2DensityUID) const{
    if (nullptr != sCurrentCt2DensityUID){
        *sCurrentCt2DensityUID = mCurrentCt2DensityUID;
    }
    return mCt2density;
}

RtCt2density* TpsGalleryImageManager::GetCT2DensityTableByUid(const std::string& dbUid)
{
    if (mMapCT2DensityTable.find(dbUid) == mMapCT2DensityTable.end()) {
        TPS_LOG_DEV_ERROR<<"Failed to find ct2density table["<<
            dbUid<<"] from image data manager.";
        return nullptr;
    }

    return mMapCT2DensityTable[dbUid];
}

void TpsGalleryImageManager::SetCurrentDataHeaderElement(int sliceNo){
    TpsImage3DEntity* imgEntity = nullptr;
    if(GetCurrentImage3DEntity(mCurrentImage3DUID, &imgEntity)){
        if (nullptr == imgEntity){
            return;
        }
        std::string img_uid = imgEntity->GetImageUidByIndex(sliceNo);
        auto it = mCurrentSliceDataHeaderMap.find(img_uid);
        if(it != mCurrentSliceDataHeaderMap.end()){
            mCurrentDataHeaderElement = it->second;
        }
    }
}

Mcsf::IDataHeaderElementMap* TpsGalleryImageManager::GetCurrentDataHeaderElement() const{
    return mCurrentDataHeaderElement;
}

// iSliceIndex [1, TotalSlice]
bool TpsGalleryImageManager::DeleteSlice(const std::string& sSeriesUID, const int& iSliceIndex){

    auto currentImage3DEntity = mMapImage3DEntity[sSeriesUID];

    TpsImage3DEntity* pNewImage3DEntity = this->CreateImage3D();
    RtSeries* pSeriesCurrent = currentImage3DEntity->GetRtSeries();
    if (nullptr == pSeriesCurrent){
        TPS_LOG_DEV_ERROR<<"pSeriesCurrent is null.";
        return false;
    }

    RtSeries* pSeries = new RtSeries(*pSeriesCurrent);// 深拷贝
    auto pHeader = pSeries->get_header();
    unsigned long ulSize(0);
    auto pBuffer = pSeries->get_imagedata(&ulSize);
    int iSliceCount = pHeader->m_iSliceCount;

    //iSliceIndex=[1, TotalSlice]需要转换为[0, TotalSlice)
    int iSliceIndexNew = iSliceIndex - 1;

    if (iSliceIndexNew > iSliceCount - 1 || iSliceIndexNew < 0){
        return false;
    }

    long lImageSize = pHeader->m_iXDim * pHeader->m_iYDim * (pHeader->m_iBitsAllocated >>3);
    long lNewTotalSize = lImageSize* (iSliceCount-1);
    char* pNewBuffer = new char[lNewTotalSize]();
    memset(pNewBuffer, 0, lNewTotalSize);

    for (int i=0, j=0; i<iSliceCount; ++i) {
        if (i == iSliceIndexNew){
            continue;
        }
        memcpy(pNewBuffer + j * lImageSize, pBuffer + i* lImageSize, lImageSize);
        ++j;
    }
    pHeader->m_vScanInfoList.erase(pHeader->m_vScanInfoList.begin() + iSliceIndexNew);

    //delete [] pBuffer;
    //pBuffer = new char[lNewTotalSize]();
    //memcpy(pBuffer, pNewBuffer, lNewTotalSize);
    //// TODO ?

    pSeries->set_imagedata(pNewBuffer, lNewTotalSize);
    delete [] pNewBuffer;
    pNewBuffer = nullptr;

    --pHeader->m_iSliceCount; 

    pNewImage3DEntity->SetRtSeries(pSeries);
    //pNewImage3DEntity->SetupImage3D(pSeries);

    // set up a default slice buffer for each Image3d Entity
    if(!pNewImage3DEntity->InitSliceBuffer()){
        TPS_LOG_DEV_ERROR<<"Failed to Initialize slice buffer.";
        return false;
    }

    mMapImage3DEntity.erase(sSeriesUID);
    mMapImage3DEntity[sSeriesUID] = pNewImage3DEntity;

    // Get Deleted ImageUID
    mDeletedImageMap[sSeriesUID].push_back(pSeriesCurrent->get_header()->m_vScanInfoList[iSliceIndexNew].m_sUID);

    delete currentImage3DEntity;
    currentImage3DEntity = nullptr;
    return true;
}

bool TpsGalleryImageManager::GetDeletedImageUIDs(const std::string &sSeriesUID, std::vector<std::string> &deletedImageUIDs){

    auto itDeletedImage = mDeletedImageMap.find(sSeriesUID);
    if (itDeletedImage != mDeletedImageMap.end()) {
        deletedImageUIDs = mDeletedImageMap[sSeriesUID];
        return true;
    }   

    return false;
}

bool TpsGalleryImageManager::GetSectionType(const std::string &sSeriesUID, std::string& sectionType){
    TpsImage3DEntity* pImage3DEntity;

    if (!this->GetImage3DData(sSeriesUID, &pImage3DEntity)) {
        TPS_LOG_DEV_ERROR<<"Failed to Get Image3DData for Series : "<<sSeriesUID;
        return false;
    }
    
    RtImage3DHeader* pHeader = nullptr;
    if (!pImage3DEntity->GetImage3DHeader(pHeader)){
        TPS_LOG_DEV_ERROR<<"Failed to Get Image3DHeader for Series : "<<sSeriesUID;
        return false;
    }
    
    Mcsf::Vector3f imageXOrient(pHeader->m_ImageXOrientation.x, 
        pHeader->m_ImageXOrientation.y, pHeader->m_ImageXOrientation.z);
    Mcsf::Vector3f imageYOrient(pHeader->m_ImageYOrientation.x, 
        pHeader->m_ImageYOrientation.y, pHeader->m_ImageYOrientation.z);
    auto imageZOrient = imageXOrient.CrossProduct(imageYOrient);

    Mcsf::Vector3f absImageZOrient(std::abs(imageZOrient.GetX()), std::abs(imageZOrient.GetY()), std::abs(imageZOrient.GetZ()));
    Mcsf::Vector3f xOrient(1, 0, 0);
    Mcsf::Vector3f yOrient(0, 1, 0);
    Mcsf::Vector3f zOrient(0, 0, 1);
    auto xDist = absImageZOrient.DotProduct(xOrient);
    auto yDist = absImageZOrient.DotProduct(yOrient);
    auto zDist = absImageZOrient.DotProduct(zOrient);
    if (xDist > yDist && xDist > zDist){
        sectionType = "SAGITTAL";
    } else if (yDist > xDist && yDist > zDist){
        sectionType = "CORONAL";
    } else if (zDist > xDist && zDist > yDist){
        sectionType = "AXIAL";
    } else {
        TPS_LOG_DEV_ERROR<<"Failed to Calculate SectionType for Series : "<<sSeriesUID;
        sectionType ="Unknown";
        return false;
    }
    return true;
}

bool TpsGalleryImageManager::BackupOriginalImage3DEntity(const std::string& sSeriesUID)
{
    auto currentImage3DEntity = mMapImage3DEntity[sSeriesUID];

    //auto it = mMapOriginalImage3DEntity.find(sSeriesUID);
    //if (it != mMapOriginalImage3DEntity.end())
    //{
    //    if(nullptr != it->second)
    //    {
    //        delete it->second;
    //        it->second = nullptr;
    //    }
    //    mMapOriginalImage3DEntity.erase(it);
    //}

    TpsImage3DEntity* pNewImage3DEntity = this->CreateImage3D();
    RtSeries* pSeriesCurrent = currentImage3DEntity->GetRtSeries();
    if (nullptr == pSeriesCurrent){
        TPS_LOG_DEV_ERROR<<"pSeriesCurrent is null.";
        return false;
    }

    RtSeries* pSeries = new RtSeries(*pSeriesCurrent);// 深拷贝
    pNewImage3DEntity->SetRtSeries(pSeries);
    //pNewImage3DEntity->SetupImage3D(pSeries);

    // set up a default slice buffer for each Image3d Entity
    if(!pNewImage3DEntity->InitSliceBuffer()){
        TPS_LOG_DEV_ERROR<<"Failed to Initialize slice buffer.";
        return false;
    }

    //TpsImage3DEntity* originalImage3DEntity = new TpsImage3DEntity(currentImage3DEntity, true); //深拷贝

    auto it = mMapOriginalImage3DEntity.find(sSeriesUID);
    if (it != mMapOriginalImage3DEntity.end())
    {
        mMapOriginalImage3DEntity.erase(sSeriesUID);
        mMapOriginalImage3DEntity[sSeriesUID] = pNewImage3DEntity;
    }
    else
    {
        mMapOriginalImage3DEntity.insert(make_pair(sSeriesUID, pNewImage3DEntity));
    }

    return true;
}

bool TpsGalleryImageManager::RestoreOriginalImage3DEntity(const std::string& sSeriesUID)
{
    auto it = mMapImage3DEntity.find(sSeriesUID);
    if(it == mMapImage3DEntity.end()){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryImageManager][RestoreOriginalImage3DEntity]: "
            <<"The TpsImage3DEntity instance with UID: "
            <<sSeriesUID<<" does not exist.";
        return false;
    }

    it = mMapOriginalImage3DEntity.find(sSeriesUID);
    if(it == mMapOriginalImage3DEntity.end()){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryImageManager][RestoreOriginalImage3DEntity]: "
            <<"The Original TpsImage3DEntity instance with UID: "
            <<sSeriesUID<<" does not exist.";
        return false;
    }

    auto currentImage3DEntity = mMapImage3DEntity[sSeriesUID];

    mMapImage3DEntity.erase(sSeriesUID);
    mMapImage3DEntity[sSeriesUID] = mMapOriginalImage3DEntity[sSeriesUID];

    delete currentImage3DEntity;
    currentImage3DEntity = nullptr;
    
    return true;
}

TPS_END_NAMESPACE
