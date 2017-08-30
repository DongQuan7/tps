////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_da_imagedatamanager.h
/// 
///  \brief class ImageDataManager declaration 
/// 
///  \version 1.0
/// 
///  \date    Nov.22,2013
////////////////////////////////////////////////////////////////

#ifndef TPS_DA_IMAGEDATAMANAGER_H_
#define TPS_DA_IMAGEDATAMANAGER_H_

#include "tps_da_defs.h"
#include <vector>

#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsDataAccess/tps_da_datamanagerbase.h"

namespace Mcsf
{
    class Matrix4f;
    class IDataHeaderElementMap;
}

namespace boost
{
    class mutex;
}

TPS_BEGIN_NAMESPACE

class TpsImage3DEntity;
class TpsImage2DEntity;

struct TpsSeriesGroup4d;
struct CertifiedMesh;

class TpsImageTextData
{
public:
    TpsImageTextData()
    {
        mpTextData = nullptr;
        mTextDataBufHeight = mTextDataBufWidth = 0;
    }
    ~TpsImageTextData()
    {
        if (nullptr != mpTextData)
        {
            delete [] mpTextData;
            mpTextData = nullptr;
        }
    }

    TpsImageTextData(unsigned char* pTextData, int bufWid, int bufHei)
    {
        mpTextData = pTextData;
        mTextDataBufHeight = bufHei;
        mTextDataBufWidth = bufWid;
    }

    unsigned char*  mpTextData;
    int             mTextDataBufHeight;
    int             mTextDataBufWidth;
};

class RtSeries;
class RtImage3DHeader;
class RtImage3D;
class RtCt2density;
class RtSeriesgroup;
class RtRtimage;

typedef std::map<std::string, RtCt2density*> MapOfCT2DensityTable;

/// \class ImageDataManager
/// 
/// \brief To manage Image3D of TMS
/// NOTE:  The destructor should release the map!
class TPS_DA_DECLSPEC TpsImageDataManager  : public TpsDataManagerBase
{
public:
    TpsImageDataManager(void);
    virtual ~TpsImageDataManager(void);

    void ClearPatientData();
    /// \brief Create image entity
    /// \return TpsImage3DEntity*
    TpsImage3DEntity* CreateImage3D();
    /// \brief Add image3d to manager
    /// \param[in] sImage3DUid      image 3d UID in TMS
    /// \param[in] image3DData      pointer of an image3d object
    /// \return bool true or false
    bool AddImage3D(const std::string& sImage3DUid, TpsImage3DEntity* image3DData);
    /// \brief Add image2d to manager
    /// \param[in] sImage2DUid      image 2d UID in TMS
    /// \param[in] image2DData      pointer of an image2d object
    /// \return bool true or false
    bool AddImage2D(const std::string& sImage2DUid,
        TpsImage2DEntity* image2DEntity);

    /// \brief add image text data into data manager
    /// \param[in]
    /// \param[in] 
    /// \param[in]
    /// \param[in]
    /// return true for succeed; otherwise, false
    bool AddImageTextData(const std::string& imageTextID,
        unsigned char* pImageTextBuffer, int bufWidth, int bufHeight);

    bool AddDataHeader(const std::string& seriesUid, Mcsf::IDataHeaderElementMap* dataHeader);
    void DeleteDataHeader(const std::string& seriesUid);
    Mcsf::IDataHeaderElementMap* GetDataHeader(const std::string& seriesUid);

    /// \brief Delete image3d from manager
    /// \param[in] sImage3DUid      image 3d UID in TMS
    /// \return bool true or false
    bool DeleteImage3D(const std::string& sImage3DUid);
    /// \brief Delete image2d from manager
    /// \param[in] sImage2DUid      image 2d UID in TMS
    /// \return bool true or false
    bool DeleteImage2D(const std::string& sImage2DUid);



    bool GetImage3DData(const std::string& sImage3DUid, TpsImage3DEntity** image3DData) const;
    /// \brief Get image2d data
    /// \param[in] sImage2DUid          UID of Image2D
    /// \param[out] image2DData         Image2D entity
    /// \return bool  true when succeeded, false otherwise
    bool GetImage2DData(const std::string& sImage2DUid, TpsImage2DEntity** image2DData) const;


    bool GetImageSeries(const std::string &sSeriesUid, RtSeries **series) const;

    /// \brief Get registration matrix 
    /// \param[in] sImage3DFixedUID     volume data fixed UID
    /// \param[in] sImage3DFloatUID     volume data float UID
    /// \param[out] matRegistrationMatrix    registration matrix float image to fixed in DICOM patient c.s.
    /// \return bool true if succeeded, false otherwise
    bool GetRegistrationMatrix(
        const std::string& sImage3DFixedUID, 
        const std::string& sImage3DFloatUID,
        Mcsf::Matrix4f* matRegistrationMatrix) const;

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
    bool GetImage3DHeader(const std::string& serUID, 
        RtImage3DHeader*& pHeader);


    void GetAllCT2DensityMap(std::vector<RtCt2density*>& vCt2Density);
    float CT2DensityByImageUid(float fCTValue, const std::string& sImageSeriesUid);
    float CT2DensityByCT2DensityUid(float fCTValue, const std::string& ct2densityUid);
    float Density2CTByImageUid(float density, const std::string& sImageSeriesUid);
    float Density2CTByCT2DensityUid(float fDensity, const std::string& ct2densityUid);
    RtCt2density* GetCT2DensityTableByImageUid(const std::string& imgUid);
    RtCt2density* GetCT2DensityTableByUid(const std::string& dbUid);

    MapOfCT2DensityTable* GetSystemCT2DensityTables();

    bool AddCT2DensityMap(const RtCt2density& pCt2density);
    bool UpdateCT2Density(const RtCt2density& ct2density);
    bool RemoveCT2Density(const std::string& ct2densityUid);


    const float* GetCT2DensityArrayByUid(const std::string& ct2DensityUid);
    const float* GetCT2DensityArrayByImageUid(const std::string& imgUid);
    // 获取CT值到电子密度的转换数组 wzd-2017-04-01
    const float* GetCT2ReferenceEDensityArrayViaImage(const std::string& imgUid);

    void GetEasyPlanImageUID(const std::string &planUID, std::string *imageUID);
    void GetEasyPlanOpposedImageUID( const std::string &planUID, std::string *imageUID );
    void DeleteEasyPlanImageUID(const std::string &planUID);
    void DeleteEasyPlanOpposedImageUID(const std::string &planUID);
    /// \brief set current image uid for easy plan
    /// \param[in] sImage2DUid      image 2d UID in TMS
    /// \return bool true or false
    void SetEasyPlanImageUID(const std::string &easyplanUID, const std::string &imageUID);
    void SetEasyPlanOpposedImageUID( const std::string &planUID, const std::string &imageUID );

    //失败代表beam没有关联的drr
    bool Get2DImageDataByBeamUID(const std::string &beamUID, TpsImage2DEntity*& image2DData);
    void GetRtRtimageListByPlanUID(const std::string &planUID, std::vector<RtRtimage*>& rtImageList);

    /// \brief whether a series exists in ImageDataManager
    /// \param[in] seriesUid
    /// return [true] if yes, [false] if no
    bool IsSeriesExists(const std::string& seriesUid);


    // CT 4D group
    int GroupingSeries(const std::string& courseUid, const std::vector<RtSeriesgroup*>& vecSeriesGroup);
    int GroupingSeries(const std::vector<RtSeries*>& vecSeries, const std::vector<RtSeriesgroup*>& vecSeriesGroup);
    int GetAllSeriesGroup(std::vector<std::shared_ptr<TpsSeriesGroup4d>>& vecSeriesGroup);
    int SetSeriesGroup(const std::string& groupUid, const std::vector<std::string>& seriesUidList);
    std::shared_ptr<TpsSeriesGroup4d> GetSeriesGroup4d(const std::string& strSeriesUid);
    void SetMainPhase(const std::string& guid, const std::string& uid);
    std::string GetOperationImageUid(const std::string& strSeriesUid);
    std::string GetGroupUid(const std::string& strSeriesUid);

    bool CheckUidExist(const std::string& uid);
    /// \brief keep skin mesh for each image if used in simulator.
    /// \param[in] seriesUid
    /// \param[in] skin mesh of this image
    /// return [true] if yes, [false] if no
    bool AddSkinMesh(const std::string &imageUID, const CertifiedMesh &skinMesh);
    bool GetSkinMesh(const std::string &imageUID, CertifiedMesh *&skinMesh);

    std::vector<std::string> GetSeriesUidListByPatientUid(const std::string& patientUid);

private:
    void Dispose();
    std::string GetCT2DensityTableUidByImageUid_i(const std::string& imgUid);
    void SetDensityBuffer_i(const RtCt2density& ct2density);
    float GetDensitybyCTval_i(int ct, const std::vector<int>& vCT, std::map<int, float>& ct2densMap);

private:

    std::map<std::string, TpsImage3DEntity*>                     mMapImage3DEntity;
    std::map<std::string, TpsImage2DEntity*>                     mMapImage2DEntity;
    std::map<std::string, CertifiedMesh*>                        mMapImageSkinMesh;
    std::map<std::string, Mcsf::IDataHeaderElementMap*>          mMapDataHeader;
    std::map<std::string, TpsImageTextData*>                     mMapImageTextData;
    std::map<std::string, std::shared_ptr<TpsSeriesGroup4d>>     mMapSeriesGroup4d;
    MapOfCT2DensityTable                                         mMapCT2DensityTable;
    std::map<std::string, std::string>                           mMapEasyplanImageUID;
    std::map<std::string, std::string>                           mMapEasyplanOpposedImageUID;

    boost::mutex                                                 *mMutexTextObj;
    boost::mutex                                                 *mMutex;
    int                                                          mMaxCTValue;
    int                                                          mMinCTValue;


};


TPS_END_NAMESPACE

#endif
