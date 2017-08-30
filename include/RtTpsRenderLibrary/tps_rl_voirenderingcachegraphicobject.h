////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_voirenderingingcachegraphicobject.h
/// 
///  \brief class TpsVOIRenderingCacheGO declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/16
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_VOIRENDERINGCACHEGRAPHICOBJECT_H_
#define TPS_RL_VOIRENDERINGCACHEGRAPHICOBJECT_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"

#include "McsfMedViewer3DArithmetic/point3d.h"
#include "McsfMedViewer3DArithmetic/matrix_4x4.h"
#include <vector>

namespace Mcsf{
    class Point2f;
}
TPS_BEGIN_NAMESPACE  // begin namespace tps

struct TpsContour{
    std::vector<Mcsf::MedViewer3D::Point3D> mPointList;
    bool mIsEditing;
    bool mHasCandidate;
    bool mIsSpline;
    Mcsf::MedViewer3D::Point3D mCandidatePoint;
};

struct IntersectionsLC{
    int contourIndex; // the contour index in cache
    std::vector<int> intersections; // the intersection,the order is from the first point in contour to the final point
    bool isRightOrder; // true if the region consist of the contour points and the line between intersetions[2n] and intersetions[2n+1] is the inner part of the contour.
    bool hasInnerIntersections; //true if there has intersections between intersetions[2n] and intersetions[2n+1]
}; // intersections between line and contour

/// \class TpsVOIRenderingCacheGO
/// 
/// \brief this graphic object is a cache contour or mask when rendering VOI.
///        in editing mode of VOI, one will get a current cache & modify this cache,
///        render strategy of VOI will get the contour or mask directly to render.
///        when editing over, take the cache to update sub-volume of data layer.
///        it's also used to render VOI when VOI not dirty, in this case, it's a real cache.
class TPS_RL_DECLSPEC TpsVOIRenderingCacheGO : public TpsGraphicObjectBase{
public:
    typedef enum{
        CONTOUR,
        MASK,
    }CACHE_TYPE;

    //the constructor and destructor
    TpsVOIRenderingCacheGO();
    virtual ~TpsVOIRenderingCacheGO();

    //////////////////////////////////////////////////////////////////////////
    // \brief set contours cache when editing VOI
    // \param[in] contours  the contour list
    void SetContourCache(const std::vector<TpsContour>& contours);

    //////////////////////////////////////////////////////////////////////////
    // \brief get current contour cache    
    // \param[out] std::vector<TpsContour>  the contour list
    void GetContourCache(std::vector<TpsContour>*& contourList) /*const*/;

    //////////////////////////////////////////////////////////////////////////
    // \brief set the mask cache when editing VOI
    // \param[in] mask  the mask memory pointer
    // \param[in] width  the mask width
    // \param[in] height the mask height
    // \param[in] matWorldToNormalPlane the transform matrix 
    //               form world to normalized plane coordinate
    // \note this method will copy memory from input memory address
    void SetMaskCache(unsigned char* mask, int width, int height, 
        const Mcsf::MedViewer3D::Matrix4x4& matWorldToNormalPlane);

    //////////////////////////////////////////////////////////////////////////
    // \brief get the mask cache
    // \param[out] mask  the mask memory pointer
    // \param[out] width  the mask width
    // \param[out] height the mask height
    // \param[out] matWorldToNormalPlane the transform matrix 
    //               form world to normalized plane coordinate
    // \note  this method returns the pointer of the buffer, not copy memory
    void GetMaskCache(unsigned char*& mask, int& width, int& height, 
        Mcsf::MedViewer3D::Matrix4x4& matWorldToNormalPlane) const;

	    //////////////////////////////////////////////////////////////////////////
    // \brief set the selected mask cache when eraser line
    // \param[in] mask  the mask memory pointer
    // \param[in] width  the mask width
    // \param[in] height the mask height
    // \param[in] matWorldToNormalPlane the transform matrix 
    //               form world to normalized plane coordinate
    // \note this method will copy memory from input memory address
    void SetSelectedMaskCache(unsigned char* mask, int width, int height, 
        const Mcsf::MedViewer3D::Matrix4x4& matWorldToNormalPlane);

    //////////////////////////////////////////////////////////////////////////
    // \brief get the mask cache
    // \param[out] mask  the mask memory pointer
    // \param[out] width  the mask width
    // \param[out] height the mask height
    // \param[out] matWorldToNormalPlane the transform matrix 
    //               form world to normalized plane coordinate
    // \note  this method returns the pointer of the buffer, not copy memory
    void GetSelectedMaskCache(unsigned char*& mask, int& width, int& height, 
        Mcsf::MedViewer3D::Matrix4x4& matWorldToNormalPlane) const;

    //////////////////////////////////////////////////////////////////////////
    // \brief set the editing flag of the cache
    // \param[in] isEditing  the editing flag
    void SetEditingFlag(bool isEditing);

    //////////////////////////////////////////////////////////////////////////
    // \brief get the editing flag of the cache
    // \return bool  the editing flag
    bool GetEditingFlag();

    //////////////////////////////////////////////////////////////////////////
    // \brief clear rendering cache, contour & mask
    void Clear();

    //////////////////////////////////////////////////////////////////////////
    // \brief clear contour cache
    void ClearContourCache();

    //////////////////////////////////////////////////////////////////////////
    // \brief get current active cache type
    // \return CACHE_TYPE the cache type, CONTOUR or MASK.
    CACHE_TYPE GetCurrentCacheType() const;

    void SetContourSliceFirstPoint(Mcsf::MedViewer3D::Point3D firstPoint);
    Mcsf::MedViewer3D::Point3D GetContourSliceFirstPoint();

    void SetContourSliceSecondPoint(Mcsf::MedViewer3D::Point3D secondPoint);
    Mcsf::MedViewer3D::Point3D GetContourSliceSecondPoint();

    void ClearContourSlicePoints();

    void SetHasSlicePoint(bool hasSlicePoint);

    bool GetHasSlicePoint();

    void SetMprSlice(int slice);
    int  GetMprSlice();

    //Get current action type : such as smartcontour, nudge or spline and so on.
    int GetCurrentActionType();

    void SetCurrentActionType(int currentActionType);

    //Following function is for line eraser

    //first point of the line 
    void SetHasFirstPoint(bool hasFirstPoint);

    bool GetHasFirstPoint();

    //second point of the line 
    void SetHasSecondPoint(bool secondPoint);

    bool GetHasSecondPoint();

    void SetFirstMouseDownPosition(Mcsf::MedViewer3D::Point3D position);
    Mcsf::MedViewer3D::Point3D GetFirstMouseDownPosition();

    void SetSecondMouseDownPosition(Mcsf::MedViewer3D::Point3D position);
    Mcsf::MedViewer3D::Point3D GetSecondMouseDownPosition();

    void GetContourIndex(std::vector<int>& contourIndexList);

    void AddContourIndex(int contourIndex);

    void GetFirstInterPoints(std::vector<int>& firstInterPoints);

    void AddFirstInterPointIndex(int firstInterPoint);

    void GetSecondInterPoints(std::vector<int>& secondInterPoints);

    void AddSecondInterPointIndex(int secondInterPoint);

    void ClearEraserLinePara();

    void SetSelectedContourIndex(int index);

    int GetSelectedContourIndex();

    void SetIsFromFirst2Second(bool flag);

    bool GetIsFromFirst2Second();

    void AddIntersections(IntersectionsLC inter);

    void GetIntersectionsV(std::vector<IntersectionsLC>& interV);

    bool GetSelectedPart();

    void SetSelectedPart(bool isRightPart);

    bool HasSelectedPart();

    void ClearSelectedMaskCache(bool& needRender);

    bool IsNewCache();

    void SetIsNewCache(bool isNewCache);

    void SetPreMaskCache();

    void GetPreMaskCache(unsigned char*& mask, int& width, int& height,
        Mcsf::MedViewer3D::Matrix4x4& matWorldToNormalPlane) const;

    void ClearPreMaskCache();

    //////////////////////////////////////////////////////////////////////////
    /// for avoid voi when editing
    /// the avoid buffer is temporary data with the same plane, same size with the cache buffer
    bool GenerateAvoidCacheBuffer();
    unsigned char* GetAvoidCacheBuffer();
    void CleanAvoidCacheBuffer();

private:
    std::vector<TpsContour>                 mContourCache;
    std::vector<TpsContour>                 mSplineContourCache;

    unsigned char*                          mMaskCache;
    unsigned char*                          mSelectedMaskCache;//for line eraser
    unsigned char*                          mPreMaskCache;//for nudge undo
    int                                     mWidth;
    int                                     mHeight;
    Mcsf::MedViewer3D::Matrix4x4            mWorldToNormalPlaneTM;

    std::string                             mRenderingVOIUID;
    bool                                    mIsEditing;
    CACHE_TYPE                              mActiveCacheType;
    int                                     mCurrentActionType;
    std::vector<int>                        mContourIndexList; // for line eraser
    std::vector<int>                        mFirstInterPoints; // for line eraser
    std::vector<int>                        mSecondInterPoints; // for line eraser
    int                                     mSelectedContourIndex; // for line eraser
    bool                                    mIsFromFirst2Second; // for line eraser
    std::vector<IntersectionsLC>            mIntersectionsV; //for line eraser
    bool                                    mHasFirstPoint;//for line eraser
    bool                                    mHasSecondPoint;//for line eraser
    Mcsf::MedViewer3D::Point3D              mContourSliceFirstPoint;
    Mcsf::MedViewer3D::Point3D              mContourSliceSecondPoint;
    bool                                    mHasSlicePoint;
    bool                                    mIsSelectedRightPart;//for line eraser
    bool                                    mHasSelectedPart;//for line eraser
    int                                     mMprSlice;
    Mcsf::MedViewer3D::Point3D              mFirstDownPosition; // for circle nudge and line eraser
    Mcsf::MedViewer3D::Point3D              mSecondDownPosition; //for line eraser
    bool                                    mNeedClearSelectedMask; //for line eraser
    bool                                    mIsNewCache;

    ///avoid cache buffer, will be deleted when edit over
    unsigned char*                          mAvoidCacheBuffer;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsVOIRenderingCacheGO);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
