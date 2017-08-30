////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_da_gallery_imagemanager.h
///  \brief   
///
///  \version 1.0
///
///  \date    Dec. 29, 2014
/////////////////////////////////////////////////////////////////

#ifndef TPS_DA_GALLERY_IMAGEMANAGER_H_
#define TPS_DA_GALLERY_IMAGEMANAGER_H_

#include "tps_da_defs.h"

#include "boost/thread/mutex.hpp"
//Mcsf
#include "Mcsf3DArithmetic/mcsf_3d_matrix3x3.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

#include "tps_da_imagedatamanager.h" // define TpsImageTextData

namespace Mcsf{
    class Matrix4f;
}

TPS_BEGIN_NAMESPACE

class RtSeries;
class RtImage3DHeader;
class TpsImage3DEntity;
class ITpsDataRepository;

struct TpsSeriesGroup4d;

typedef std::map<std::string, Mcsf::IDataHeaderElementMap*> DataHeaderMap;

/// \class ImageDataManager
/// 
/// \brief To manage Image3D of TMS
/// NOTE:  The destructor should release the map!
class TPS_DA_DECLSPEC TpsGalleryImageManager {
public:
    TpsGalleryImageManager();
    ~TpsGalleryImageManager();

    void SetRepository(ITpsDataRepository* repos) {
        mDataRepository = repos;
    }

    /// \brief Create image entity
    /// \return TpsImage3DEntity*
    TpsImage3DEntity* CreateImage3D();

    /// \brief Add image3d to manager
    /// \param[in] sImage3DUid      image 3d UID in TMS
    /// \param[in] image3DData      pointer of an image3d object
    /// \return bool true or false
    bool AddImage3D(const std::string& sImage3DUid, TpsImage3DEntity* image3DData);
    bool AddDataHeader(RtSeries* series);

    /// \brief Delete image3d from manager
    /// \param[in] sImage3DUid      image 3d UID in TMS
    /// \return bool true or false
    bool DeleteImage3D(const std::string& sImage3DUid);

    /// \brief Get image3d data
    /// \param[in] sImage3DUid          UID of Image3D
    /// \param[out] image3DData         Image3D entity
    /// \return bool  true when succeeded, false otherwise
    bool GetImage3DData(const std::string& sImage3DUid, 
        TpsImage3DEntity** image3DData) const;

    /// \brief Get all series uid 
    /// \param[in] sPatient             UID of patient
    /// \param[out] pSerUIDList         uid list of all series
    /// \return bool  true when succeeded, false otherwise
    bool GetSeriesUIDList(const std::string& sPatient,
        std::vector<std::string>* pSerUIDList) const;

    /// \brief Get all series uid 
    /// \param[in] sPatient             UID of patient
    /// \param[out] pSerUIDList         uid list of all series
    /// \return bool  true when succeeded, false otherwise
    bool GetSeriesUIDList(std::vector<std::string>& vSerUIDList) const;

    //直接从数据库里读取RtDatabaseWrapper::GetSeriesListByCourseUid，否则在reloadpatient时只Imagemanager不清空的情况下，获取series会有问题
    //void GetSeriesUIDListByCourseUID(const std::string &sCourseUID, std::vector<std::string>& seriesUIDList);

    /// \brief Set default image3d data according to SeriesData and SeriesTime
    /// \param[int] sImage3DUid          UID of Image3D
    /// \return void
    //void SetDefaultImage3D(const std::string& sImage3DUid);
    void SetCurrentImage3D(const std::string& sImage3DUid);  

    bool GetCurrentImage3DEntity(std::string& pImage3DUid, TpsImage3DEntity** image3DData) const;

    void SetIsVisibleOfImageText(bool isVisibleOfImageText){mIsVisibleOfImageText = isVisibleOfImageText;}
    bool GetIsVisibleOfImageText() const {return mIsVisibleOfImageText;}

    bool AddImageSeries(RtSeries* series/*const std::string &sSeriesUid, const RtSeries& series*/);

    bool DeleteImageSeries(const std::string &sSeriesUid);

    bool GetImageSeries(const std::string &sSeriesUid, RtSeries **series) const;

    std::string GetCurrentImage3DUID() const;

    /// \brief add image text data into data manager
    /// \param[in]
    /// \param[in] 
    /// \param[in]
    /// \param[in]
    /// return true for succeed; otherwise, false
    bool AddImageTextData(const std::string& imageTextID,
        unsigned char* pImageTextBuffer, int bufWidth, int bufHeight);

    /// \brief get image text data from data manager
    /// \param[in]
    /// \param[in] 
    /// \param[in]
    /// \param[in]
    /// return true for succeed; otherwise, false
    bool GetImageTextData(const std::string& imageTextID,
        unsigned char*& pImageTextBuffer, int& bufWidth, int& bufHeight);

    /// \brief update image text data by external buffer. Internal buffer data
    ///        will be released and refilled
    /// \param[in]
    /// \param[in] 
    /// \param[in]
    /// \param[in]
    /// return true for succeed; otherwise, false
    bool UpdateImageTextData(const std::string& imageTextID, TpsImageTextData* pTextObj);

    /// \brief get 3d image set header from data manager
    /// \param[in]
    /// \param[in] 
    /// return true for succeed; otherwise, false
    bool GetImage3DHeader(const std::string& serUID, RtImage3DHeader*& pHeader);

    /// \brief Add 4DCT Series Uid into a container
    /// \param[in] 
    /// return void
    void InitSeries4DUidList(const std::vector<std::string>& seriesUidList);

    /// \brief Get 4DCT Series Uid 
    /// \param[out] 
    /// return 
    void GetSeries4DUidList(std::vector<std::string>& seriesUidList);

    /// \brief whether a series is in 4DCT
    /// \param[in] seriesUid
    /// return [true] if yes, [false] if no
    bool IsSeriesInSeries4D(const std::string& seriesUid);

    /// \brief whether a series exists in ImageDataManager
    /// \param[in] seriesUid
    /// return [true] if yes, [false] if no
    bool IsSeriesExists(const std::string& seriesUid);

    void SetSeries4DPageturned();
    bool GetIsSeries4DPageturned();

    void SetIsPlaying(bool);
    bool GetIsPlaying();

    /// \brief get ct to density from TMS
    /// \param[out] pCt2denstiy
    /// \param[out] ct2densitySize
    /// return true for succeed; otherwise, false
    bool GetCT2Density(float* pCt2density);
    bool GetCT2DensitySize(int* ct2densitySize);

    float* GetCurrentCT2Density(std::string* sCurrentCt2DensityUID) const;

    //To Do 哪里set byDYX
    RtCt2density* GetCT2DensityTableByUid(const std::string& dbUid);

    void SetCurrentDataHeaderElement(int sliceNumber);
    Mcsf::IDataHeaderElementMap* GetCurrentDataHeaderElement() const;

    void ClearManager(void);

    void ClearImageData(const std::string &sSeriesUID);

    bool DeleteSlice(const std::string& sSeriesUID, const int& iSlice);

    bool GetDeletedImageUIDs(const std::string &sSeriesUID, std::vector<std::string> &deletedImageUIDs);

    bool GetSectionType(const std::string &sSeriesUID, std::string& sectionType);

    // CT 4D group
    int GroupingSeries(const std::string& courseUid);
    int GroupingSeries(const std::vector<RtSeries*>& vecSeries);
    int GetAllSeriesGroup(std::vector<std::shared_ptr<TpsSeriesGroup4d>>& vecSeriesGroup);
    int SetSeriesGroup(const std::string& groupUid, const std::vector<std::string>& seriesUidList);
    std::shared_ptr<TpsSeriesGroup4d> GetSeriesGroup4d(const std::string& strSeriesUid);
    void SetMainPhase(const std::string& guid, const std::string& uid);
    std::string GetOperationImageUid(const std::string& strSeriesUid);
    std::string GetGroupUid(const std::string& strSeriesUid);

    //std::vector<std::string> GetSeriesUidListViaCourseUid(const std::string& courseUid);
    std::vector<std::string> GetSeriesUidListByPatientUid(const std::string& patientUid);

    //备份最原始的Image3dEntity
    bool BackupOriginalImage3DEntity(const std::string& sSeriesUID);
    bool RestoreOriginalImage3DEntity(const std::string& sSeriesUID);

private:
    float GetDefaultCt2Density(float ct);
    void CreateDefaultCt2Density();

    ITpsDataRepository*                             mDataRepository;

    //Image3DUID, TpsImage3DEntity*
    std::map<std::string, TpsImage3DEntity*>        mMapImage3DEntity;
    std::map<std::string, TpsImage3DEntity*>        mMapOriginalImage3DEntity;  //用来备份Original Image3DEntity，delete slices后，撤销回到最原始的数据
    std::map<std::string, RtSeries*>                mMapImageSeries;

    std::string                                     mCurrentImage3DUID;
    bool                                            mIsVisibleOfImageText;

    std::vector<std::string>                        mVecSeries4DUidList;

    std::map<std::string, TpsImageTextData*>        mMapImageTextData;
    boost::mutex                                    mMutexTextObj;
    bool                                            mIsSeries4DPageturned;
    bool                                            mIsPlaying;
    float*                                          mCt2density;
    int                                             mCt2densitySize;

    MapOfCT2DensityTable                            mMapCT2DensityTable;
    std::string                                     mCurrentCt2DensityUID;  
    
    std::map<std::string, DataHeaderMap>            mAllSeriesDataHeaderMap;    // map<series, map<slice, IDataHeaderElement>>

    DataHeaderMap                                   mCurrentSliceDataHeaderMap; // map<slice, IDataHeaderElement>
    Mcsf::IDataHeaderElementMap*                    mCurrentDataHeaderElement;

    std::map<std::string, std::vector<std::string>> mDeletedImageMap;  // map<SeriesUID, DeletedImageUIDs>

    // CT 4D group <strGroupUid, TpsSeriesGroup4d>
    std::map<std::string, std::shared_ptr<TpsSeriesGroup4d>>    m_mapSeriesGroup4d;
};


TPS_END_NAMESPACE

#endif //TPS_DA_IMAGEDATAMANAGER_H_
