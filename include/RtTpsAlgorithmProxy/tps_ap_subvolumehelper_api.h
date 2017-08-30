////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_ap_subvolumehelper_api.h
/// 
///  \brief class subvolume helper include auto-segmentation
/// 
///  \version 1.0
/// 
///  \date    Dec.10,2013
////////////////////////////////////////////////////////////////

#ifndef TPS_AP_VOIHELPER_API_H_
#define TPS_AP_VOIHELPER_API_H_

#include "tps_ap_defs.h"

#include "RtTpsAlgorithmProxy/tps_pl_datastructure.h"
#include "RtTpsFramework/tps_fw_common_datastructure.h"
#include "RtTpsFramework/tps_fw_arithmetic.h"

namespace McsfGeometry
{
    class SubVolume;
    class Plane;
}

namespace Mcsf
{
    namespace MedViewer3D
    {
        class Vector3D;
        class MedViewer3DImageData;
        class Point2D;
    }
    class Vector2f;
    class Point2f;
    class Point3f;
    class Matrix4f;
    class McsfAlgoAutoContour;
    class IProgress;
}

TPS_BEGIN_NAMESPACE;  // begin namespace TPS

// \def EPSINON the excepted error
#ifndef EPSINON
#define EPSINON 0.0000001
#endif
#ifndef FI_LIMIT
#define FI_LIMIT (40)
#endif

class TPSMask;
class ROIEntity;
class TpsImage3DEntity;
class TpsImageDataManager;
class RtDosegrid;
class RtCt2density;

struct PlaneContour
{
    PlaneContour() 
    {
        count = 0;
        x = nullptr;
        y = nullptr;
    }
    ~PlaneContour() 
    {
        count = 0;
        if(x != nullptr) 
        {
            delete x;
            x = nullptr;
        }
        if(y != nullptr) 
        {
            delete y;
            y = nullptr;
        }
    }

    void SetCount(int count1) 
    {
        if(x != nullptr) 
        {
            delete x;
            x = nullptr;
        }
        if(y != nullptr) 
        {
            delete y;
            y = nullptr;
        }
        count = count1;
        x = new float[count];
        y = new float[count];
    }

    float* x;
    float* y;
    int count; //todo : private
};

typedef McsfGeometry::SubVolume SubVolume;
typedef McsfGeometry::Plane Plane;

class RtContour;
/// \class VoiHelperAPI
/// 
/// \brief *****
class TPS_AP_DECLSPEC SubVolumeHelperAPI
{

public:

    /// \brief Skin Segmentation called Algorithm of MCSF
    ///
    /// \param[in]  image3DData    image3d entity
    /// \param[out] pSubVolume     subVolume to be built
    /// \return int TPS_ER_SUCCESS or TPS_ER_FAILURE
    static int SkinSegmentation(
        const TpsImage3DEntity& image3DData, 
        SubVolume* pSubVolume,
        Mcsf::IProgress* pProgress = 0);

    ///\brief  Lungs Segmentation.
    ///
    /// The function is used to do lung segmentation and get the lung's mask, the source data should be foot priority(F(foot)->H(head)).
    ///\param[in]  image3DData              image3d entity
    ///\param[in]  skinMask                 subMask of Skin.
    ///\param[in]  lungType                 0:both, 1:left, 2:right
    ///\param[out] pLungs                   subMask of Lung.
    static int LungsSegmentation(
        const TpsImage3DEntity& image3DData,
        int lungType, 
        SubVolume* pLungs,
        Mcsf::IProgress* pProgress = 0);

    ///\brief  SpinalCord Segmentation.
    ///
    /// The function is used to do segmentation and get the spinal cord's mask.
    ///\param[in]  image3DData              Image data in TMS
    ///\param[in]  skinMask                 subMask of Skin.
    ///\param[out] pOutputMask              Pointer array to the cord's mask data. The size of the pointer array equals uiWidth*uiHeight*uiImagecount.
    static int SpinalCordSegmentation(
        const TpsImage3DEntity& image3DData,
        SubVolume* pOutputMask,
        Mcsf::IProgress* pProgress = 0);

    ///\brief  Heart Segmentation.
    ///
    /// The function is used to do heart segmentation and get the heart's mask
    /// this method do skin and lung segmentation interiorly.
    ///\param[in]  image3DData              Image data in TMS
    ///\param[out] pOutputMask              output mask
    static int HeartSegmentation(
        const TpsImage3DEntity& image3DData, 
        SubVolume* pOutputMask,
        Mcsf::IProgress* pProgress = 0);

    ///\brief  Liver Segmentation.
    ///
    /// The function is used to do Liver segmentation and get the Liver's mask
    /// this method do skin and lung segmentation interiorly.
    ///\param[in]  image3DData              Image data 
    ///\param[out] pOutputMask              output mask
    static int LiverSegmentation(
        const TpsImage3DEntity& image3DData, 
        SubVolume* pOutputMask,
        Mcsf::IProgress* pProgress = 0);

    ///\brief  Spleen Segmentation.
    ///
    /// The function is used to do Spleen segmentation and get the Spleen's mask
    ///\param[in]  image3DData              Image data 
    ///\param[out] pOutputMask              output mask
    static int SpleenSegmentation(
        const TpsImage3DEntity& image3DData,
        SubVolume* pOutputMask,
        Mcsf::IProgress* pProgress = 0);

    ///\brief  Kidney Segmentation.
    ///
    /// The function is used to do Kidney segmentation and get the Kidney's mask
    ///\param[in]  image3DData              Image data 
    ///\param[in]  kidneyType               0:both, 1:left, 2:right
    ///\param[out] pKindney                 output mask
    static int KidneySegmentation(
        const TpsImage3DEntity& image3DData,
        int kidneyType, 
        SubVolume* pKindney,
        Mcsf::IProgress* pProgress = 0);

    ///\brief  Parotid Segmentation.
    ///
    /// The function is used to do Parotid segmentation and get the Kidney's mask
    ///\param[in]  image3DData              Image data 
    ///\param[out] pOutputMask              output mask
    static int ParotidSegmentation(
        const TpsImage3DEntity& image3DData, 
        int nType,
        SubVolume* pOutputMask,
        Mcsf::IProgress* pProgress = 0);

    ///\brief  Eyeball Segmentation.
    ///
    /// The function is used to do Eyeball segmentation and get the mask
    ///\param[in]  image3DData              Image data 
    ///\param[in]  eyeType               0:both, 1:left, 2:right
    ///\param[out] pOutputMask              output mask
    static int EyeballSegmentation(
        const TpsImage3DEntity& image3DData, 
        int eyeType,
        SubVolume* pOutputMask,
        Mcsf::IProgress* pProgress = 0);
    
    ///\brief  Brainstem Segmentation.
    ///
    /// The function is used to do Brainstem segmentation and get the mask
    ///\param[in]  image3DData              Image data 
    ///\param[out] pOutputMask              output mask
    static int BrainstemSegmentation(
        const TpsImage3DEntity& image3DData, 
        SubVolume* pOutputMask,
        Mcsf::IProgress* pProgress = 0);

    ///\brief  Function Segmentation.
    ///
    /// The function is used to do mandible segmentation and get the mandible's mask
    ///\param[in]  image3DData              Image data 
    ///\param[out] pSubMandibleMask         subMask of Mandible.
    static int MandibleSegmentation(
        const TpsImage3DEntity& image3DData, 
        SubVolume* pOutputMask,
        Mcsf::IProgress* pProgress = 0);

    ///\brief  Femoralhead Segmentation.
    ///
    /// The function is used to do Femoralhead segmentation and get the mask
    ///\param[in]  image3DData              Image data 
    ///\param[out] pOutputMask              output mask
    static int FemoralheadSegmentation(
        const TpsImage3DEntity& image3DData, 
        SubVolume* pOutputMask,
        Mcsf::IProgress* pProgress = 0);

    ///\brief  Brain Segmentation.
    ///
    /// The function is used to do Brain segmentation and get the mask
    ///\param[in]  image3DData              Image data 
    ///\param[out] pOutputMask              output mask
    static int BrainSegmentation(
        const TpsImage3DEntity& image3DData, 
        SubVolume* pOutputMask,
        Mcsf::IProgress* pProgress = 0);
    
    ///\brief  Rectum Segmentation.
    ///
    /// The function is used to do Rectum segmentation and get the mask
    ///\param[in]  image3DData              Image data 
    ///\param[out] pOutputMask              output mask
    static int RectumSegmentation(
        const TpsImage3DEntity& image3DData, 
        SubVolume* pOutputMask,
        Mcsf::IProgress* pProgress = 0);


    ///\brief  Prostate Segmentation.
    ///
    /// The function is used to do Rectum segmentation and get the mask
    ///\param[in]  image3DData              Image data 
    ///\param[out] pOutputMask              output mask
    static int ProstateSegmentation(
        const TpsImage3DEntity& image3DData, 
        bool bInputRectumMask, 
        SubVolume* pSubRectumMask,
        SubVolume* pOutputMask,
        Mcsf::IProgress* pProgress = 0);

    ///\brief  JawJoint Segmentation.
    ///
    /// The function is used to do JawJoint segmentation and get the mask
    ///\param[in]  image3DData              Image data 
    ///\param[out] pOutputMask              output mask
    static int JawJointSegmentation(
        const TpsImage3DEntity& image3DData, 
        SubVolume* pOutputMask,
        Mcsf::IProgress* pProgress = 0);

    ///\brief  Function Segmentation.
    ///
    /// The function is used to do segmentation and get the brain Pancreas's mask.
    ///\param[in]  image3DData              Image data 
    ///\param[out] pSubPancreasMask                      subMask of Pancreas.
    static int PancreasSegmentation(
        const TpsImage3DEntity& image3DData, 
        SubVolume* pOutputMask,
        Mcsf::IProgress* pProgress = 0);

    //////////////////////////////////////////////////////////////////////////
    ///\brief  merge to VOIs
    ///
    ///\param[in]  volume1
    ///\param[in]  volume2
    ///\param[out] pVolumeMerged
    static void MergeVOIs(SubVolume& volume1, SubVolume& volume2, SubVolume* pVolumeMerged);

    ///\brief  SubVolume--> Mask of one slice
    ///
    /// \param[in] subVolume  subvolume of a VOI
    /// \param[in] pVertices  the vertices(4 points) of the plane in the volume coordinate system
    /// \param[in] uiWidth    Width of the plane in x-axis
    /// \param[in] uiHeight   Height of the plane in y-axis
    ///\param[out] pTpsMask   TPSMask in one slice
    /// \return int TPS_ER_SUCCESS or TPS_ER_FAILURE
    static int SliceSubVolume(SubVolume& subVolume, 
        const Mcsf::Point3f* pVertices, 
        unsigned int uiWidth,
        unsigned int uiHeight,
        TPSMask* pTpsMask);

    ///\brief  SubVolume--> Mask of one slice
    ///
    /// \param[in] subVolume  subvolume of a VOI
    /// \param[in] pVertices  the vertices(4 points) of the plane in the volume coordinate system
    /// \param[in] uiWidth    Width of the plane in x-axis
    /// \param[in] uiHeight   Height of the plane in y-axis
    ///\param[out] buffer   TPSMask in one slice
    /// \return int TPS_ER_SUCCESS or TPS_ER_FAILURE
    static int SliceSubVolume(SubVolume& subVolume, 
        const Mcsf::Point3f* pVertices, 
        unsigned int uiWidth,
        unsigned int uiHeight,
        unsigned char* buffer);

    static int SliceSubVolume(unsigned char* subVolume, 
        unsigned int* aDim, 
        const Mcsf::Point3f* pVertices, 
        unsigned int uiWidth,
        unsigned int uiHeight,
        unsigned char* buffer);

    static int SliceSubVolume(short* subVolume, 
        unsigned int* aDim, 
        const Mcsf::Point3f* pVertices, 
        unsigned int uiWidth,
        unsigned int uiHeight,
        short* buffer);

    static int SliceSubVolume(int* subVolume, 
        unsigned int* aDim, 
        const Mcsf::Point3f* pVertices, 
        unsigned int uiWidth,
        unsigned int uiHeight,
        int* buffer);

    //////////////////////////////////////////////////////////////////////////
    // \brief  get section of volume data by a specified plane
    // \param[in]  pVolume  the volume data, type: float
    // \param[in]  aDim  the dimension of volume data
    // \param[in]  pVertices  the four vertice of the plane
    // \param[in]  uiWidth  the width of output section data
    // \param[in]  uiHeight  the height of output section data
    // \param[out]  pSection  the output section data
    // \return int  TPS_ER_SUCCESS or TPS_ER_FAILURE
    static int GetSectionOfVolume(float* pVolume, unsigned int* aDim, 
        const Mcsf::Point3f* pVertices, unsigned int uiWidth, 
        unsigned int uiHeight, float* pSection);

    /// \brief Get a mask from contours
    /// Contour ---> Mask conversion methods
    /// \param[in] vvContour            Several contours and every contour has several 2D points    
    /// \param[in] uiWidth              Width of the plane in x-axis
    /// \param[in] uiHeight             Height of the plane in y-axis
    /// \param[out]pTpsMask             Pointer to the result mask
    /// \return int TPS_ER_SUCCESS or TPS_ER_FAILURE
    static int GetMaskFromContours(
        const std::vector<std::vector<::Mcsf::Point2f>>& vvContour, 
        unsigned int uiWidth,
        unsigned int uiHeight,
        TPSMask* pTpsMask);

    //////////////////////////////////////////////////////////////////////////
    /// \brief  get mask from contours
    /// \param[in] vvContour  the contour to be converted
    /// \param[in] uiWidth  the width of the contour plane & the output buffer
    /// \param[in] uiHeight  the height of the contour plane & the output buffer
    /// \param[out] buffer  the output buffer, need allocate memory outside
    static int GetMaskFromContours(
        const std::vector<std::vector<::Mcsf::Point2f>>& vvContour, 
        unsigned int uiWidth,
        unsigned int uiHeight,
        unsigned char* buffer, bool isSpline = false);

    //////////////////////////////////////////////////////////////////////////
    ///将Contours转为Mask，不考虑编辑逻辑，contours只存在环或者不相交两种位置关系
    static int GetMaskFromCompletedContours(
        const std::vector<std::vector<::Mcsf::Point2f>>& vvContour, 
        unsigned int uiWidth,
        unsigned int uiHeight,
        TPSMask* pTpsMask);
    static int GetMaskFromCompletedContours(
        const std::vector<std::vector<::Mcsf::Point2f>>& vvContour, 
        unsigned int uiWidth,
        unsigned int uiHeight,
        unsigned char* buffer, bool isSpline = false);

    //static int ConvertMaskFromContour(std::vector<std::vector<::Mcsf::Point2f>>& vvContour, 
    //    unsigned int uiWidth, unsigned int uiHeight, unsigned char* buffer);

    /// \brief GetContoursFromMask.
    /// Mask ---> Contour conversion methods
    /// \param[in]  maskData                mask data in one plane
    /// \param[out] vvContour               a contour of one plane
    /// \return int TPS_ER_SUCCESS or TPS_ER_FAILURE
    static int GetContoursFromMask(const TPSMask& maskData, 
        std::vector<std::vector<::Mcsf::Point2f>>& vvContour);

    //////////////////////////////////////////////////////////////////////////
    /// \brief get contours from mask
    /// \param[in] buffer  the mask buffer
    /// \param[in] width & height  the size of mask buffer
    /// \param[out] vvContour  the output contours
    static int GetContoursFromMask(unsigned char* buffer, 
        unsigned int width, unsigned int height, 
        std::vector<std::vector<::Mcsf::Point2f>>& vvContour);

    /// \brief GetOutlineFromMask.
    /// Mask ---> Contour conversion methods
    /// \param[in]  pImageInput             input image
    /// \param[in] uiWidth                  Width of the plane in x-axis
    /// \param[in] uiHeight                 Height of the plane in y-axis
    /// \param[out] pImageOutput            output image
    /// \return int TPS_ER_SUCCESS or TPS_ER_FAILURE
    static int GetOutlineFromMask(const unsigned char* pImageInput, 
        unsigned int uiWidth, unsigned int uiHeight,
        unsigned char* pImageOutput);

    /// \brief UpdateSubVolume.
    /// Mask ---> SubVolume conversion methods
    /// \param[in] pVertices                the vertices(4 points) of the plane in the volume coordinate system 
    /// \param[in] maskData                 mask data in one plane
    /// \param[out] pSubVolume              VOI subvolume to be updated
    /// \return int TPS_ER_SUCCESS or TPS_ER_FAILURE
    static int UpdateSubVolume(const Mcsf::Point3f *pVertices, 
        const TPSMask& maskData,
        SubVolume* pSubVolume, bool isAdd = false);

    /// \brief UpdateSubVolume.
    /// Mask ---> SubVolume conversion methods
    /// \param[in] pVertices                the vertices(4 points) of the plane in the volume coordinate system 
    /// \param[in] buffer                   the buffer of the mask data to be set
    /// \param[in] width & height           the buffer size of the mask data to be set
    /// \param[out] pSubVolume              VOI subvolume to be updated
    /// \return int TPS_ER_SUCCESS or TPS_ER_FAILURE
    static int UpdateSubVolume(const Mcsf::Point3f *pVertices, 
        unsigned char* buffer, unsigned int width, unsigned int height, 
        SubVolume* pSubVolume, bool isAdd = false);

    /// \brief UpdateSubVolume.from contour
    /// Mask ---> SubVolume conversion methods
    /// \param[in] vvContour            Several contours and every contour has several 2D points 
    /// \param[in] uiWidth              Width of the plane in x-axis
    /// \param[in] uiHeight             Height of the plane in y-axis
    /// \param[in] pVertices            the vertices(4 points) of the plane in the volume coordinate system 
    /// \param[out] pSubVolume          VOI subvolume to be updated
    /// \return int TPS_ER_SUCCESS or TPS_ER_FAILURE
    static int UpdateSubVolumeFromContour(
        std::vector<std::vector<::Mcsf::Point2f>>& vvContour,
        unsigned int uiWidth,
        unsigned int uiHeight,
        const Mcsf::Point3f *pVertices,
        SubVolume* pSubVolume);

    /// \brief convert VOI from contour
    /// \param[in] vvContour            Several contours and every contour has several 2D points 
    /// \param[in] uiWidth              Width of the plane in x-axis
    /// \param[in] uiHeight             Height of the plane in y-axis
    /// \param[out] pSubVolume          VOI subvolume to be updated
    /// \return int TPS_ER_SUCCESS or TPS_ER_FAILURE
    //static int BuildSubVolumeFromContour(const TpsImage3DEntity& pImage3D, VOIEntity* pVoiEntity);

    /// \brief reduce VOI into contours with less points but keep shapes
    /// \param[in] voiEntity            VOI 
    /// \param[in] image3D              Image 3D
    /// \param[out] vtContour           list of Contours
    /// \return void
    /*static void ReduceVOI(const VOIEntity& voiEntity, const TpsImage3DEntity& image3D, 
        std::vector<RTFWK::RTFWKDLDOContour*>& vtContour);*/

    /// \brief reduce VOI into contours with less points but keep shapes
    /// \param[in] voiEntity            VOI 
    /// \param[in] sImage3DUID          Image 3D UID
    /// \param[out] TPS_ANASTRUCT       in voiEntity
    /// \return void
    /*static void ReduceVOI2Contours(const VOIEntity& voiEntity, 
        const std::string& sImage3DUID, Mcsf::Matrix4f& matPat2World);*/

    /// \brief  Get bounding box of a voi
    /// \param[in] voiEntity            VOI entity
    /// \param[in] image3D              Image 3d data which the VOI belong to
    /// \param[out] boundaryBox         boundary box, xMin, xMax, yMin, yMax, zMin, zMax
    /// \return int TPS_ER_SUCCESS or TPS_ER_FAILURE
//     static int GetBoundingBox(const VOIEntity& voiEntity,
//         const TpsImage3DEntity& image3D, float* boundaryBox);

    /// \brief GetContoursFromMask.
    /// Subvolume ---> Contour conversion methods
    /// \param[in] subVolume            subvolume of a VOI
    /// \param[in] pVertices            the vertices(4 points) of the plane in the volume coordinate system
    /// \param[in] uiWidth              Width of the plane in x-axis
    /// \param[in] uiHeight             Height of the plane in y-axis
    /// \param[out] vvContour           a contour of one plane
    /// \return int TPS_ER_SUCCESS or TPS_ER_FAILURE
    static int GetContoursFromSubvolume(SubVolume& subVolume, 
        const Mcsf::Point3f* pVertices, 
        unsigned int uiWidth,
        unsigned int uiHeight, 
        std::vector<std::vector<::Mcsf::Vector2f>>* vvContour);

    /// \brief GetMinimumBoundingRectangle.
    /// \param[in] dim                  dim of volume(or other object)
    /// \param[in] pVertices            the vertices(4 points, form a rectangle and start from left buttom to left top) 
    /// of the plane in the volume coordinate system
    /// \param[out] pMiniVertices       the minimum bounding rectangle
    /// \return int TPS_ER_SUCCESS or TPS_ER_FAILURE
    static int GetMinimumBoundingRectangle(unsigned int* dim, 
                        const Mcsf::Point3f* pVertices,
                        Mcsf::Point3f* pMiniVertices);

    //get the voi center
    static int GetSubVolumeCenter(SubVolume& subVolume, Mcsf::Point3f& center);

    //////////////////////////////////////////////////////////////////////////
    //update the statistical information of voi including: 
    //[1]the volume of the voi [mm3]
    //[2]center point of the voi (int the original volume coordinate), 
    //[3]max/min/average density of the voi
    static bool UpdateVOIStatisticalInformation(const RtCt2density* pCt2Density, const TpsImage3DEntity& image3D, ROIEntity* roi);

    //////////////////////////////////////////////////////////////////////////
    /// \brief get the bounding rectangle according to specified rectangle
    /// \param[in] dim  the dimension of the volume
    /// \param[in] vertices  the vertices of input limited plane
    /// \param[out] resultVertices  the result vertices in volume coordinate system
    /// \param[out] volumeToNormalPlaneTM  the transform matrix from volume to normalized plane c.s.
    static void GetBoundingRectangle(size_t* dim, const Mcsf::Point3f* vertices, 
        Mcsf::Point3f* resultVertices, Mcsf::Matrix4f& volumeToNormalPlaneTM);

    static bool SaveToTrueColorBitmap(const std::string& path, unsigned char* buffer,
        const int& height, const int& width);

    static bool SaveToTrueColorBitmapFloat(const std::string& path, const float* buffer,
        const int& height, const int& width);

    static void SaveToGrayBitmap(const std::string& path, unsigned char* buffer,
    const int& height, const int& width, int color = 255);

    static float AutoRemoveTable(const TpsImage3DEntity& image3DData);

    /// \brief SubVolume.resampling
    /// \param[in] bufferSrc            input image
    /// \param[in] xSrc                 xSrc
    /// \param[in] ySrc                 ySrc
    /// \param[in] zSrc                 zSrc
    /// \param[in] xScale               xScale
    /// \param[in] yScale               yScale
    /// \param[in] zScale               zScale
    /// \param[in] bufferDes            output image
    static void SubVolumeResample(
        const unsigned char *bufferSrc,
        int xSrc,int ySrc,int zSrc,
        float xScale, float yScale, float zScale,
        unsigned char* bufferDes);

    //////////////////////////////////////////////////////////////////////////
    /// \brief setup ana-struct from rt contours
    /// \param[in] vecContours  the contour list
    /// \param[out] pVoiEntity  the output voi entity
    /// \return bool  true if succeeded, false otherwise
//     static bool SetupAnastruct(const TpsImage3DEntity* pImage3D, const std::vector<RtContour*>& vecContours, 
//         VOIEntity* pVoiEntity);
    static bool SetupContourSet(const TpsImage3DEntity* pImage3D,
        const std::vector<std::vector<PT3>>& vecContours, ROIEntity* pRoiEntity);

    //////////////////////////////////////////////////////////////////////////
    // \brief convert ana-struct to rt contours
    // \param[in] voi  the voi entity
    // \param[out] vtContour  the rt contour list
    // \return bool  true if succeeded, false otherwise
    static bool UpdateRtContours(TpsImage3DEntity* pImage3D, ROIEntity& roi);

    static bool IsInContour(PlaneContour* contour, ::Mcsf::MedViewer3D::Point2D pt);

    //////////////////////////////////////////////////////////////////////////
    // \brief expand the sub-volume to original size
    //static void ExpandSubVolumeToOriginSize(size_t* size, VOIEntity *voi);

    //////////////////////////////////////////////////////////////////////////
    // \brief refine the sub-volume, get the minimum bounding box current voi
    //static void RefineSubvolume(size_t* dim, VOIEntity *voi);

    //////////////////////////////////////////////////////////////////////////
    // \brief refine the sub-volume according to the edited plane buffer
    // \param[in] dim  the original size of the volume
    // \param[in] vertices  the four vertices of the plane in volume c.s. 
    // \param[in] buffer  the plane buffer
    // \param[in] width|height  the size of the plane buffer
    // \param[in&out] subVolume  the sub-volume to be refined
    // \return bool  true if succeeded, false otherwise
    // \note this method can only expand the existing sub-volume if the plane
    //      has valid points outside the current bounding box. So it can not shrink,
    //      and the worst case is the sub-volume will be the origin size of volume after
    //      continuous editing, but it does not affect, the performance is always 
    //      more important than the space in current stage.
//     static bool RefineSubvolumeViaPlane(
//         const size_t* dim, const Mcsf::Point3f* vertices, 
//         const unsigned char* buffer, int width, int height, VOIEntity *voi);

    static bool IsMaskIntersect(unsigned char* oldMask, unsigned char* newMask, int wh);

    //////////////////////////////////////////////////////////////////////////
    // \brief smooth voi
    // \param[in] voi  the voi entity
    // \param[in] pImage3D  the image 3d entity
    // \return bool  true if succeeded, false otherwise
    static bool SmoothVOI(ROIEntity *voi,const TpsImage3DEntity& pImage3D);

    static bool InterpolateVOI(ROIEntity *voiEntity);

    //////////////////////////////////////////////////////////////////////////
    // \brief Calculate percentage of a certain voi in a certain dosegrid
    // \param[in] image3D  image 3d which contains matrix interested
    // \param[in] voi  the voi entity
    // \param[in] dosegrid the "certain" dosegrid mentioned above
    // \return float  value of (volume in grid) / (total volume)
    static float CalculateInGridPercentage(const TpsImage3DEntity& image3D, ROIEntity *voi, RtDosegrid* dosegrid);
    static Mcsf::Matrix4f ConvertToMatrix4f(const float arrayMatrix[16]);
    //static bool ReadContourPointFromFile(const std::string& filePath, int accuracy,
    //    std::vector<Mcsf::Point3f>& contourPts);
    
    static void VOIToPluncAnastruct(const ROIEntity* voi, plunc::ANASTRUCT* ana);
    static void PluncAnastructToVOI(plunc::ANASTRUCT* ana, const TpsImage3DEntity& pImage3D, ROIEntity* voi);
};

TPS_END_NAMESPACE  // end namespace TPS

#endif  //TPS_AP_VOIHELPER_API_H_
