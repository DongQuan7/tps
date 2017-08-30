////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_da_imagedatamanager.cpp
/// 
///  \brief class ImageDataManager 
/// 
///  \version 1.0
/// 
///  \date    Nov.22,2013
////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <boost/thread/mutex.hpp>
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

#include "McsfDataHeader/mcsf_data_header_element_map_interface.h"

//TPS
#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_datamanagerbase.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"

#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_ct2density.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedmesh.h"

TPS_BEGIN_NAMESPACE

TpsImageDataManager::TpsImageDataManager(void)
{
    // Defined by requirement
    mMaxCTValue = 5000;
    mMinCTValue = -1024;
    mMutex = new boost::mutex();
    mMutexTextObj = new boost::mutex();
}

TpsImageDataManager::~TpsImageDataManager(void)
{
    delete mMutex;
    mMutex = nullptr;
    delete mMutexTextObj;
    mMutexTextObj = nullptr;
    Dispose();
}

void TpsImageDataManager::Dispose()
{
    auto itCT2DensityTable = mMapCT2DensityTable.begin();
    while (itCT2DensityTable != mMapCT2DensityTable.end())
    {
        if (nullptr != itCT2DensityTable->second)
        {
            delete itCT2DensityTable->second;
            itCT2DensityTable->second = nullptr;
        }

        itCT2DensityTable++;
    }
    mMapCT2DensityTable.clear();

    auto itImage3DEntity = mMapImage3DEntity.begin();
    while (itImage3DEntity != mMapImage3DEntity.end())
    {
        if (nullptr != itImage3DEntity->second)
        {
            delete itImage3DEntity->second;
            itImage3DEntity->second = nullptr;
        }

        itImage3DEntity++;
    }
    mMapImage3DEntity.clear();

    for (auto it = mMapDataHeader.begin(); it != mMapDataHeader.end(); ++it)
    {
        if (it->second)
        {
            delete it->second;
            it->second = nullptr;
        }
    }
    mMapDataHeader.clear();

    auto itImageText = mMapImageTextData.begin();
    while (itImageText != mMapImageTextData.end())
    {
        if (nullptr != itImageText->second)
        {
            delete itImageText->second;
            itImageText->second = nullptr;
        }

        itImageText++;
    }
    mMapImageTextData.clear();

    auto itImage2D = mMapImage2DEntity.begin();
    while (itImage2D != mMapImage2DEntity.end())
    {
        if (nullptr != itImage2D->second)
        {
            delete itImage2D->second;
            itImage2D->second = nullptr;
        }

        itImage2D++;
    }
    mMapImage2DEntity.clear();

    auto itImageSkin = mMapImageSkinMesh.begin();
    while (itImageSkin != mMapImageSkinMesh.end())
    {
        if (nullptr != itImageSkin->second)
        {
            delete itImageSkin->second;
            itImageSkin->second = nullptr;
        }
        itImageSkin++;
    }
    mMapImageSkinMesh.clear();

    mMapEasyplanImageUID.clear();
    mMapEasyplanOpposedImageUID.clear();
    mMapSeriesGroup4d.clear();
}

void TpsImageDataManager::ClearPatientData()
{
    for (auto it = mMapImage3DEntity.begin(); it != mMapImage3DEntity.end(); )
    {
        if (it->second != nullptr 
            && it->second->GetRtSeries() != nullptr
            && it->second->GetRtSeries()->get_isphantom())
        {
            ++it;
            continue;
        }
        delete it->second;
        it = mMapImage3DEntity.erase(it);
    }

    //phantom是否有用？目前被删除了
    for (auto it = mMapDataHeader.begin(); it != mMapDataHeader.end(); ++it)
    {
        if (it->second)
        {
            delete it->second;
            it->second = nullptr;
        }
    }
    mMapDataHeader.clear();

    auto itImageText = mMapImageTextData.begin();
    while (itImageText != mMapImageTextData.end())
    {
        if (nullptr != itImageText->second)
        {
            delete itImageText->second;
            itImageText->second = nullptr;
        }

        itImageText++;
    }
    mMapImageTextData.clear();

    auto itImage2D = mMapImage2DEntity.begin();
    while (itImage2D != mMapImage2DEntity.end())
    {
        if (nullptr != itImage2D->second)
        {
            delete itImage2D->second;
            itImage2D->second = nullptr;
        }

        itImage2D++;
    }
    mMapImage2DEntity.clear();
    mMapSeriesGroup4d.clear();
}

bool TpsImageDataManager::IsSeriesExists(const std::string& sImage3DUid)
{
    auto it = mMapImage3DEntity.find(sImage3DUid);
    if (it == mMapImage3DEntity.end())
    {
        TPS_LOG_DEV_ERROR << "The Series with Uid:" << sImage3DUid <<" does not exist.";
        return false;
    }
    return true;
}

TpsImage3DEntity* TpsImageDataManager::CreateImage3D()
{
    return new TpsImage3DEntity();
}

bool TpsImageDataManager::AddImage3D(const std::string& seriesUid, 
    TpsImage3DEntity* image3DEntity)
{
    if (seriesUid.empty())
    {
        TPS_LOG_DEV_ERROR << "sImage3DUid is empty.";
        return false;
    }
    mMapImage3DEntity[seriesUid] = image3DEntity;
    return true;
}

bool TpsImageDataManager::AddDataHeader(const std::string& seriesUid, 
    Mcsf::IDataHeaderElementMap* dataHeader)
{
    if (dataHeader == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The data header is null.";
        return false;
    }

    auto itDataHeader = mMapDataHeader.find(seriesUid);
    if (itDataHeader != mMapDataHeader.end())
    {
        if (itDataHeader->second == dataHeader) return true;
        if (itDataHeader->second) delete itDataHeader->second;
        TPS_LOG_DEV_INFO << "The data header already exists, replace.";
    }
    mMapDataHeader[seriesUid] = dataHeader;
    return true;
}

void TpsImageDataManager::DeleteDataHeader(const std::string& seriesUid)
{
    auto itDataHeader = mMapDataHeader.find(seriesUid);
    if (itDataHeader != mMapDataHeader.end())
    {
        if (itDataHeader->second) delete itDataHeader->second;
        mMapDataHeader.erase(itDataHeader);
    }
}

Mcsf::IDataHeaderElementMap* TpsImageDataManager::GetDataHeader(const std::string& seriesUid)
{
    auto itDataHeader = mMapDataHeader.find(seriesUid);
    if (itDataHeader == mMapDataHeader.end())
    {
        return nullptr;
    }
    return itDataHeader->second;
}

bool TpsImageDataManager::AddImage2D(const std::string& sImage2DUid,TpsImage2DEntity* image2DEntity)
{
    if (sImage2DUid.empty())
    {
        TPS_LOG_DEV_ERROR << "sImage2DUid is empty.";
        return false;
    }

    //NOTE! should delete pImage3D later
    mMapImage2DEntity.insert(
        std::map<std::string, TpsImage2DEntity*>::value_type(
        sImage2DUid, image2DEntity));

    return true;
}

bool TpsImageDataManager::DeleteImage3D(const std::string& sImage3DUid)
{
    if (sImage3DUid.empty())
    {
        TPS_LOG_DEV_ERROR << "sImage3DUid is empty.";
        return false;
    }

    auto it = mMapImage3DEntity.find(sImage3DUid);
    if (it == mMapImage3DEntity.end())
    {
        TPS_LOG_DEV_WARNING << "imageUid does not exist!";
        return false;
    }

    if (it->second != nullptr)
    {
        delete it->second;
        it->second = nullptr;
        mMapImage3DEntity.erase(it);
    }
    return true;
}

bool TpsImageDataManager::DeleteImage2D(const std::string& sImage2DUid)
{
    if (sImage2DUid.empty())
    {
        TPS_LOG_DEV_ERROR << "sImage2DUid is empty.";
        return false;
    }

    auto it = mMapImage2DEntity.find(sImage2DUid);
    if (it == mMapImage2DEntity.end())
    {
        TPS_LOG_DEV_WARNING << "imageUid does not exist!";
        return false;
    }

    if (it->second != nullptr)
    {
        delete it->second;
        it->second = nullptr;
        mMapImage2DEntity.erase(it);
    }

    return true;
}

bool TpsImageDataManager::GetImage3DData(const std::string& sImage3DUid, 
    TpsImage3DEntity** image3DData) const
{
    auto it = mMapImage3DEntity.find(sImage3DUid);
    if (it == mMapImage3DEntity.end())
    {
        TPS_LOG_DEV_ERROR << "[ERROR][ImageDataManager][GetImage3DData]: "
            << "The RTFWKDLDOImage3D instance with UID: "
            << sImage3DUid << " does not exist.";
        return false;
    }

    *image3DData = it->second;
    return true;
}

bool TpsImageDataManager::GetImage2DData(const std::string& sImage2DUid, 
    TpsImage2DEntity** image2DData) const
{
    auto it = mMapImage2DEntity.find(sImage2DUid);
    if (it == mMapImage2DEntity.end())
    {
        TPS_LOG_DEV_ERROR << "[ERROR][ImageDataManager][GetImage2DData]: "
            << "The RTFWKDLDOImage2D instance with UID: "
            << sImage2DUid << " does not exist.";
        return false;
    }

    *image2DData = it->second;
    return true;
}



bool TpsImageDataManager::GetImageSeries(const std::string &sSeriesUid, RtSeries **series) const
{
    if (series == nullptr) return false;

    auto it = mMapImage3DEntity.find(sSeriesUid);
    if (it == mMapImage3DEntity.end() || it->second == nullptr)
    {
        TPS_LOG_DEV_ERROR << "[ERROR][ImageDataManager][GetImageSeries]: "
            << "The RtImage3D instance with UID: "
            << sSeriesUid << " does not exist.";
        return false;
    }
    *series = it->second->GetRtSeries();
    return true;
}

bool TpsImageDataManager::AddImageTextData(const std::string& imageTextID,
    unsigned char* pImageTextBuffer, int bufWidth, int bufHeight)
{
    // for concurrency
    boost::lock_guard<boost::mutex> lockGuard(*mMutexTextObj);

    if (pImageTextBuffer == nullptr || bufHeight <= 0 || bufWidth <= 0)
    {
        TPS_LOG_DEV_ERROR << "pImageTextBuffer==nullptr || bufHeight<=0 || bufWidth<=0";
        return false;
    }

    auto itimgtxt = mMapImageTextData.find(imageTextID);

    if (itimgtxt != mMapImageTextData.end())
    {
        TPS_LOG_TRACE_WARNING << "imageTextID[" << imageTextID
            << "] already exists in current data manager!";
        return true;
    }
    TpsImageTextData* pNewImgTxtData = 
        new TpsImageTextData(pImageTextBuffer, bufWidth, bufHeight);

    mMapImageTextData[imageTextID] = pNewImgTxtData;
    return true;
}

bool TpsImageDataManager::UpdateImageTextData(const std::string& imageTextID,
    TpsImageTextData* pTextObj)
{
    // for concurrency
    boost::lock_guard<boost::mutex> lockGuard(*mMutexTextObj);

    if (pTextObj == nullptr)
    {
        TPS_LOG_DEV_ERROR << "pTextObj==nullptr";
        return false;
    }

    auto itimgtxt = mMapImageTextData.find(imageTextID);
    if (itimgtxt == mMapImageTextData.end())
    {
        TPS_LOG_DEV_ERROR << "imageTextID NOT exists in current data manager!";
        return false;
    }

    // delete old buffer
    //delete itimgtxt->second;
    mMapImageTextData.erase(itimgtxt);
    mMapImageTextData[imageTextID] = pTextObj;

    return true;
}

bool TpsImageDataManager::GetImageTextData(const std::string& imageTextID,
    unsigned char*& pImageTextBuffer, int& bufWidth, int& bufHeight)
{
    // for concurrency
    boost::lock_guard<boost::mutex> lockGuard(*mMutexTextObj);

    auto itimgtxt = mMapImageTextData.find(imageTextID);
    if (itimgtxt == mMapImageTextData.end())
    {
        TPS_LOG_DEV_ERROR << "imageTextID[" << imageTextID
            << "] DOES NOT exists in current data manager!";
        return false;
    }

    pImageTextBuffer = itimgtxt->second->mpTextData;
    bufWidth = itimgtxt->second->mTextDataBufWidth;
    bufHeight = itimgtxt->second->mTextDataBufHeight;

    return true;
}

bool TpsImageDataManager::GetImage3DHeader(const std::string& serUID, 
    RtImage3DHeader*& pHeader)
{
    pHeader = nullptr;
    TpsImage3DEntity* pImg3D = nullptr;
    TpsImageDataManager& imgDataMgr = *mDataRepository->GetImageDataManager();
    if (!imgDataMgr.GetImage3DData(serUID, &pImg3D))
    {
        TPS_LOG_DEV_ERROR << "Failed to GetImage3DData. uid[" << serUID<<"].";
        return false;
    }

    if (pImg3D==nullptr || !pImg3D->GetImage3DHeader(pHeader))
    {
        TPS_LOG_DEV_ERROR << "Failed to GetImage3DHeader. uid[" << serUID<<"].";
        return false;
    }

    return true;
}

bool TpsImageDataManager::GetRegistrationMatrix(
    const std::string& sImage3DFixedUID,
    const std::string& sImage3DFloatUID,
    Mcsf::Matrix4f* matRegistrationMatrix) const
{
    auto it = mMapImage3DEntity.find(sImage3DFixedUID);
    if (it == mMapImage3DEntity.end() || it->second == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The fixed image does not exist which uid: " << sImage3DFixedUID;
        return false;
    }

    if (!it->second->GetRegistrationMatrix(sImage3DFloatUID, *matRegistrationMatrix))
    {
        TPS_LOG_DEV_ERROR << "Failed to get registration matrix which fixed image: "
            << sImage3DFixedUID << " float image: " << sImage3DFloatUID;
        return false;
    }
    return true;
}


float TpsImageDataManager::CT2DensityByImageUid(float fCTValue, const std::string& sImageSeriesUid)
{
    int ctValueFloor = (int)fCTValue;

    //const float *mCurrentCt2densityArray = GetCurrentCT2DensityArray();
    const float *mCurrentCt2densityArray = GetCT2DensityArrayByImageUid(sImageSeriesUid);
    if (nullptr == mCurrentCt2densityArray)
    {
        TPS_LOG_DEV_INFO << "CurrentCT2densityArray is null.";
        return -1e20f;
    }
    if (ctValueFloor <= mMinCTValue)
    {
        TPS_LOG_DEV_WARNING << "Failed to get CT2Density: "<< fCTValue;
        return mCurrentCt2densityArray[0];
    }
    if (ctValueFloor >= mMaxCTValue)
    {
        TPS_LOG_DEV_WARNING << "Failed to get CT2Density: " << fCTValue;
        return mCurrentCt2densityArray[mMaxCTValue - mMinCTValue];
    }
    int ctValueCeil = ctValueFloor + 1;

    return mCurrentCt2densityArray[ctValueFloor - mMinCTValue] * (ctValueCeil - fCTValue)
        + mCurrentCt2densityArray[ctValueCeil - mMinCTValue] * (fCTValue - ctValueFloor);
}

float TpsImageDataManager::CT2DensityByCT2DensityUid(float fCTValue, const std::string& ct2densityUid)
{
    int ctValueFloor = (int)fCTValue;
    const float *mCurrentCt2densityArray = GetCT2DensityArrayByUid(ct2densityUid);
    if (nullptr == mCurrentCt2densityArray)
    {
        TPS_LOG_DEV_INFO << "CurrentCT2densityArray is null.";
        return -1e20f;
    }
    if (ctValueFloor <= mMinCTValue)
    {
        TPS_LOG_DEV_WARNING << "Failed to get CT2Density: "<< fCTValue;
        return mCurrentCt2densityArray[0];
    }
    if (ctValueFloor >= mMaxCTValue)
    {
        TPS_LOG_DEV_WARNING << "Failed to get CT2Density: " << fCTValue;
        return mCurrentCt2densityArray[mMaxCTValue - mMinCTValue];
    }
    int ctValueCeil = ctValueFloor + 1;

    return mCurrentCt2densityArray[ctValueFloor - mMinCTValue] * (ctValueCeil - fCTValue)
        + mCurrentCt2densityArray[ctValueCeil - mMinCTValue] * (fCTValue - ctValueFloor);
}

float TpsImageDataManager::Density2CTByImageUid(float fDensity, const std::string& sImageSeriesUid)
{
    int ctValue = -2000;
    float currentDensityValue = -1;
    float nextDensityValue = -1;
    float minDensityValue = 6;
    float maxDensityValue = -1;

    // reference SSFS key 8251
    if (fDensity < 0.0f)
    {
        TPS_LOG_DEV_WARNING << "Density value: " << fDensity << " is out of scope";
        return mMinCTValue;
    }
    if (fDensity > 5.0f)
    {
        TPS_LOG_DEV_WARNING<<"Density value: " << fDensity << " is out of scope";
        return mMaxCTValue;
    }

    const float *mCurrentCt2densityArray = GetCT2DensityArrayByImageUid(sImageSeriesUid);

    for (int i = 0; i < mMaxCTValue - mMinCTValue; i++)
    {
        currentDensityValue = mCurrentCt2densityArray[i];
        nextDensityValue = mCurrentCt2densityArray[i+1];

        if ((currentDensityValue <= fDensity && fDensity < nextDensityValue)
            || (currentDensityValue >= fDensity && fDensity > nextDensityValue))
        {
            ctValue = (i + mMinCTValue) + (fDensity - currentDensityValue) / (nextDensityValue - currentDensityValue);
            break;
        }
        if (minDensityValue > currentDensityValue)
        {
            minDensityValue = currentDensityValue;
        }
        if (maxDensityValue < currentDensityValue)
        {
            maxDensityValue = currentDensityValue;
        }
    }

    if (-2000 == ctValue)
    {
        if (fDensity > maxDensityValue)
        {
            ctValue = mMaxCTValue;
        }
        else if (fDensity < minDensityValue)
        {
            ctValue = mMinCTValue;
        }
    }

    return ctValue;
}

float TpsImageDataManager::Density2CTByCT2DensityUid(float fDensity, const std::string& ct2densityUid)
{
    int ctValue = -2000;
    float currentDensityValue = -1;
    float nextDensityValue = -1;
    float minDensityValue = 6;
    float maxDensityValue = -1;

    // reference SSFS key 8251
    if (fDensity < 0.0f)
    {
        TPS_LOG_DEV_WARNING<<"Density value: "<< fDensity << " is out of scope";
        return mMinCTValue;
    }
    if (fDensity > 5.0f)
    {
        TPS_LOG_DEV_WARNING<<"Density value: "<< fDensity << " is out of scope";
        return mMaxCTValue;
    }

    const float *mCurrentCt2densityArray = GetCT2DensityArrayByUid(ct2densityUid);

    for (int i = 0; i < mMaxCTValue - mMinCTValue; i++)
    {
        currentDensityValue = mCurrentCt2densityArray[i];
        nextDensityValue = mCurrentCt2densityArray[i+1];

        if ((currentDensityValue <= fDensity && fDensity < nextDensityValue)
            || (currentDensityValue >= fDensity && fDensity > nextDensityValue))
        {
            ctValue = (i + mMinCTValue) + (fDensity - currentDensityValue) / (nextDensityValue - currentDensityValue);
            break;
        }
        if (minDensityValue > currentDensityValue)
        {
            minDensityValue = currentDensityValue;
        }
        if (maxDensityValue < currentDensityValue)
        {
            maxDensityValue = currentDensityValue;
        }
    }

    if (-2000 == ctValue)
    {
        if (fDensity > maxDensityValue)
        {
            ctValue = mMaxCTValue;
        }
        else if (fDensity < minDensityValue)
        {
            ctValue = mMinCTValue;
        }
    }

    return ctValue;
}

RtCt2density* TpsImageDataManager::GetCT2DensityTableByUid(const std::string& dbUid)
{
    if (mMapCT2DensityTable.find(dbUid) == mMapCT2DensityTable.end())
    {
        TPS_LOG_DEV_ERROR<<"Failed to find ct2density table["<<
            dbUid<<"] from image data manager.";
        return nullptr;
    }

    return mMapCT2DensityTable[dbUid];
}

RtCt2density* TpsImageDataManager::GetCT2DensityTableByImageUid(const std::string& imgUid)
{
    auto it = mMapImage3DEntity.find(imgUid);
    if (it == mMapImage3DEntity.end() || it->second == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The fixed image does not exist which uid: "<<imgUid;
        return false;
    }

    std::string uid = it->second->GetRtSeries()->get_ct2densityuid();
    return GetCT2DensityTableByUid(uid);
}

std::string TpsImageDataManager::GetCT2DensityTableUidByImageUid_i(const std::string& imgUid)
{
    std::string ct2densityUid = "";
    auto it = mMapImage3DEntity.find(imgUid);
    if (it == mMapImage3DEntity.end() || it->second == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The fixed image does not exist which uid: "<<imgUid;
        return ct2densityUid;
    }

    ct2densityUid = it->second->GetRtSeries()->get_ct2densityuid();
    return ct2densityUid;
}



const float* TpsImageDataManager::GetCT2DensityArrayByUid(const std::string& ct2DensityUid) 
{
    try
    {
        auto iter = mMapCT2DensityTable.find(ct2DensityUid);
        if (iter == mMapCT2DensityTable.end())
        {
            TPS_LOG_DEV_ERROR << "Failed to find ct2density table in imagedata manager. uid["
                << ct2DensityUid << "].";
            return nullptr;
        }

        RtCt2density* p = iter->second;
        return p->get_ct2densitybuffer();
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "GetCT2DensityMapByUid has failed for GetCT2DensityArrayByUid Uid: " << ct2DensityUid << ex.what();
        return nullptr;
    }
}

const float* TpsImageDataManager::GetCT2DensityArrayByImageUid(const std::string& imgUid)
{
    try
    {
        std::string ct2densityUid = GetCT2DensityTableUidByImageUid_i(imgUid);
        if (ct2densityUid.empty()) return nullptr;
        return GetCT2DensityArrayByUid(ct2densityUid);
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "Get CT2Density array has failed for GetCT2DensityArrayByImageUid ImageUid: " << imgUid << ex.what();
        return nullptr;
    }

}

const float* TpsImageDataManager::GetCT2ReferenceEDensityArrayViaImage(const std::string& imgUid)
{
    try
    {
        std::string ct2densityUid = GetCT2DensityTableUidByImageUid_i(imgUid);
        if (ct2densityUid.empty()) return nullptr;
        auto iter = mMapCT2DensityTable.find(ct2densityUid);
        if (iter == mMapCT2DensityTable.end())
        {
            TPS_LOG_DEV_ERROR << "Failed to find ct2density table in imagedata manager. uid["
                << ct2densityUid << "].";
            return nullptr;
        }

        RtCt2density* p = iter->second;
        return p->get_ct2referencedensitybuffer();
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "Get CT2Density array has failed for GetCT2DensityArrayByImageUid ImageUid: " << imgUid << ex.what();
        return nullptr;
    }
}

bool TpsImageDataManager::AddCT2DensityMap(const RtCt2density& ct2density)
{
    try
    {
        MapOfCT2DensityTable::iterator it = mMapCT2DensityTable.find(ct2density.get_uid());
        if (it != mMapCT2DensityTable.end())
        {
            TPS_LOG_DEV_WARNING << "CT2DensityMap[" << ct2density.get_uid() << "] already exist in image manager";
            return false;
        }
        SetDensityBuffer_i(ct2density);
        mMapCT2DensityTable[ct2density.get_uid()] = const_cast<RtCt2density*>(&ct2density);
    }
    catch (std::exception &e)
    {
        TPS_LOG_DEV_ERROR << "AddCT2Density failed: " << e.what();
        return false;
    }

    return true;
}

void TpsImageDataManager::GetAllCT2DensityMap(std::vector<RtCt2density*>& vCt2Density)
{
    vCt2Density.clear();
    for (auto itr=mMapCT2DensityTable.cbegin(); itr!=mMapCT2DensityTable.cend(); ++itr){
        vCt2Density.push_back(itr->second);
    }
}

bool TpsImageDataManager::RemoveCT2Density(const std::string& ct2densityUid)
{
    try
    {
        MapOfCT2DensityTable::iterator it = mMapCT2DensityTable.find(ct2densityUid);
        if (it == mMapCT2DensityTable.end())
        {
            TPS_LOG_DEV_ERROR<<"CT2DensityMap["<<ct2densityUid<<"] doesn't exist in image manager";
            return false;
        }
        RtCt2density* ct2density = it->second;
        delete ct2density;
        ct2density = nullptr;
        mMapCT2DensityTable.erase(it);
    }
    catch (std::exception &e)
    {
        TPS_LOG_DEV_ERROR<<"RemoveCT2Density failed: "<< e.what();
        return false;
    }

    return true;
}

bool TpsImageDataManager::UpdateCT2Density(const RtCt2density& ct2density)
{
    try
    {
        MapOfCT2DensityTable::iterator it = mMapCT2DensityTable.find(ct2density.get_uid());
        if (it == mMapCT2DensityTable.end())
        {
            TPS_LOG_DEV_WARNING << "CT2DensityMap[" << ct2density.get_uid() << "] doesn't exist in image manager";
            return false;
        }
        RtCt2density* oldCT2density = it->second;
        std::map<int, float> newMassDensityMap(ct2density.get_ct2densitymap());
        std::map<int, float> newReferenceDensityMap(ct2density.get_ct2referencedensitymap());
        oldCT2density->set_tablename(ct2density.get_tablename());
        oldCT2density->set_ct2densitymap(newMassDensityMap);
        oldCT2density->set_ct2referencedensitymap(newReferenceDensityMap);
        SetDensityBuffer_i(*oldCT2density);
        oldCT2density->set_updatetime(ct2density.get_updatetime());

    }
    catch (std::exception &e)
    {
        TPS_LOG_DEV_ERROR << "UpdateCT2Density failed: " << e.what();
        return false;
    }
    return true;
}

MapOfCT2DensityTable* TpsImageDataManager::GetSystemCT2DensityTables()
{
    return &mMapCT2DensityTable;
}

void TpsImageDataManager::SetEasyPlanImageUID( const std::string &planUID, const std::string &imageUID )
{
    auto itimguid = mMapEasyplanImageUID.find(planUID);
    if (itimguid != mMapEasyplanImageUID.end())
    {
        TPS_LOG_TRACE_WARNING<<"planUID["<<planUID
            <<"] already has an image in current data manager!";
    }
    mMapEasyplanImageUID[planUID] = imageUID;
}

void TpsImageDataManager::GetEasyPlanImageUID( const std::string &planUID, std::string *imageUID ) 
{
    //if the image uid is empty, return it to inform there is no image
    auto itimguid = mMapEasyplanImageUID.find(planUID);
    if (itimguid == mMapEasyplanImageUID.end())
    {
        TPS_LOG_DEV_WARNING << "planUID[" << planUID
            << "] DOES NOT have an image in current data manager!";
        *imageUID = "";
        return;
    }
    *imageUID = itimguid->second;
}

void TpsImageDataManager::DeleteEasyPlanImageUID(const std::string &planUID) 
{
    //if the image uid is empty, return it to inform there is no image
    auto itimguid = mMapEasyplanImageUID.find(planUID);
    if (itimguid == mMapEasyplanImageUID.end())
    {
        TPS_LOG_DEV_WARNING << "planUID[" << planUID
            << "] DOES NOT have an image in current data manager!";
        return;
    }
    mMapEasyplanImageUID.erase(itimguid);
}

void TpsImageDataManager::SetEasyPlanOpposedImageUID( const std::string &planUID, const std::string &imageUID ) 
{
    auto itimguid = mMapEasyplanOpposedImageUID.find(planUID);
    if (itimguid != mMapEasyplanOpposedImageUID.end())
    {
        TPS_LOG_TRACE_WARNING<<"planUID["<<planUID
            <<"] already has an image in current data manager!";
    }
    mMapEasyplanOpposedImageUID[planUID] = imageUID;
}

void TpsImageDataManager::GetEasyPlanOpposedImageUID( const std::string &planUID, std::string *imageUID ) 
{
    //if the image uid is empty, return it to inform there is no image
    auto itimguid = mMapEasyplanOpposedImageUID.find(planUID);
    if (itimguid == mMapEasyplanOpposedImageUID.end())
    {
        TPS_LOG_DEV_WARNING << "planUID[" << planUID
            << "] DOES NOT have an image in current data manager!";
        *imageUID = "";
        return;
    }
    *imageUID = itimguid->second;
}

void TpsImageDataManager::DeleteEasyPlanOpposedImageUID(const std::string &planUID) 
{
    //if the image uid is empty, return it to inform there is no image
    auto itimguid = mMapEasyplanOpposedImageUID.find(planUID);
    if (itimguid == mMapEasyplanOpposedImageUID.end())
    {
        TPS_LOG_DEV_WARNING << "planUID[" << planUID
            << "] DOES NOT have an image in current data manager!";
        return;
    }
    mMapEasyplanOpposedImageUID.erase(itimguid);
}

//失败代表beam没有关联的drr
bool TpsImageDataManager::Get2DImageDataByBeamUID(const std::string &beamUID, TpsImage2DEntity*& image2DData)
{
    std::map<std::string, TpsImage2DEntity*>::iterator it;
    it = std::find_if(mMapImage2DEntity.begin(), mMapImage2DEntity.end(),[&](std::pair<std::string, TpsImage2DEntity*> obj)
    {
        return obj.second->get()->get_beamuid() == beamUID;
    });
    if (it == mMapImage2DEntity.end())
    {
        return false;
    }
    image2DData = it->second;
    return true;
}

void TpsImageDataManager::GetRtRtimageListByPlanUID(const std::string &planUID, std::vector<RtRtimage*>& rtImageList)
{
    auto beamList = mDataRepository->GetBeamManager()->GetBeamsByPlan(planUID);
    auto itBeam = beamList.begin();
    for (; itBeam != beamList.end(); itBeam++)
    {
        RtBeam* beam = *itBeam;
        auto it = std::find_if(mMapImage2DEntity.begin(), mMapImage2DEntity.end(),[&](std::pair<std::string, TpsImage2DEntity*> obj)
        {
            return obj.second->get()->get_beamuid() == beam->get_uid();
        });
        if (it != mMapImage2DEntity.end())
        {
            rtImageList.push_back(it->second->get());
        }
    }
}


bool TpsImageDataManager::CheckUidExist(const std::string& uid)
{
    return mMapImage3DEntity.find(uid) != mMapImage3DEntity.end();
}

bool TpsImageDataManager::AddSkinMesh( const std::string &imageUID, const CertifiedMesh &skinMesh )
{
    if (imageUID.empty())
    {
        return false;
    }

    CertifiedMesh *mesh = new CertifiedMesh;
    for (auto itMeshInput = skinMesh.mPoints.begin(); 
        itMeshInput != skinMesh.mPoints.end(); itMeshInput++)
    {
        mesh->mPoints.push_back(*itMeshInput);
    }

    for (auto itMeshInput = skinMesh.mNormals.begin(); 
        itMeshInput != skinMesh.mNormals.end(); itMeshInput++)
    {
        mesh->mNormals.push_back(*itMeshInput);
    }

    if (mMapImageSkinMesh[imageUID] != nullptr)
    {
        delete mMapImageSkinMesh[imageUID];
        mMapImageSkinMesh[imageUID] = nullptr;
    }

    mMapImageSkinMesh[imageUID] = mesh;
    return true;
}

bool TpsImageDataManager::GetSkinMesh( const std::string &imageUID, CertifiedMesh *&skinMesh )
{
    auto itImageSkinMesh = mMapImageSkinMesh.find(imageUID);
    if (itImageSkinMesh == mMapImageSkinMesh.end())
    {
        skinMesh = nullptr;
        return false;
    }

    skinMesh = itImageSkinMesh->second;
    return true;
}

std::vector<std::string> TpsImageDataManager::GetSeriesUidListByPatientUid(const std::string& patientUid)
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

void TpsImageDataManager::SetDensityBuffer_i(const RtCt2density& ct2density)
{
    std::map<int, float> ct2MassDensityMap = ct2density.get_ct2densitymap();
    std::map<int, float> ct2ReferenceDensityMap = ct2density.get_ct2referencedensitymap();

    std::vector<int> vCT;
    for (std::map<int, float>::iterator it = ct2MassDensityMap.begin(); it != ct2MassDensityMap.end(); ++it)
    {
        vCT.push_back(it->first);
    }

    int iTemp = 0;
    size_t iSize = vCT.size();
    float* refMassDensityBuffer = const_cast<float*>(ct2density.get_ct2densitybuffer());
    float* refReferenceDensityBuffer = const_cast<float*>(ct2density.get_ct2referencedensitybuffer());
    // Sort
    if (iSize != 0)
    {
        for (int i = 0; i < iSize - 1; i++)
        {
            for (int j = 0; j < iSize - i - 1; j++)
            {
                if (vCT[j] > vCT[j+1])
                {
                    iTemp = vCT[j];
                    vCT[j] = vCT[j + 1];
                    vCT[j + 1] = iTemp;
                }
            }
        }

        //非空表才走这个逻辑，空表直接保留初始化的refBuffer，即全为0
        for (int i = -1024; i < 5000; ++i)
        {
            refMassDensityBuffer[i + 1024] = GetDensitybyCTval_i(i, vCT, ct2MassDensityMap);
            refReferenceDensityBuffer[i + 1024] = GetDensitybyCTval_i(i, vCT, ct2ReferenceDensityMap);
        }
    }

}

float TpsImageDataManager::GetDensitybyCTval_i(int ct, const std::vector<int>& vCT, std::map<int, float>& ct2densMap)
{
    //空表表示为全0 lx 2016/12/22
    if (vCT.size() == 0) return 0;
    float f = ct2densMap[vCT[0]];
    size_t iSize = vCT.size();
    if (iSize != 0)
    {
        for (int i = 0; i < iSize - 1; ++i)
        {
            if (ct < vCT[0])
            {
                break;
            }
            else if (ct >= vCT[iSize - 1])
            {
                f = ct2densMap[vCT[iSize - 1]];
                break;
            }
            else if (ct == vCT[i])
            {
                f = ct2densMap[vCT[i]];
                break;
            }
            else if (ct > vCT[i] && ct < vCT[i + 1])
            {
                f = ct2densMap[vCT[i]] + (float)(ct - vCT[i]) / (float)(vCT[i + 1] - vCT[i]) * (ct2densMap[vCT[i + 1]] - ct2densMap[vCT[i]]);
                break;
            }
        }
    }
    return f;
}



TPS_END_NAMESPACE
