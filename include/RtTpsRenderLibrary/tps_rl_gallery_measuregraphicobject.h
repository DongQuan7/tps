////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  yuxuan.duan  yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_measuregraphicobject.h
/// 
///  \brief class MeasureGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/03/09
////////////////////////////////////////////////////////////////


#ifndef TPS_RL_GALLERYMEASUREGRAPHICOBJECT_H_
#define TPS_RL_GALLERYMEASUREGRAPHICOBJECT_H_

#include "tps_rl_defs.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measurevaluegraphicobject.h"

// Mcsf
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "McsfMedViewer3DArithmetic/point2d.h"
#include "McsfMedViewer3DArithmetic/point3d.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC GalleryMeasureGraphicObject : public TpsGraphicObjectBase  {
public:
    GalleryMeasureGraphicObject(void);
    virtual ~GalleryMeasureGraphicObject(void);
    virtual bool Initialize();
    virtual bool Finalize();
    //virtual bool IsToUpdate() const ;

    // for measure distance operation
    //void SetMeasureDistanceStartPoint(float startX, float startY, int iWidth, int iHeight);
    //void GetMeasureDistanceStartPoint(float *startX, float *startY, int iWidth, int iHeight);
    //void SetMeasureDistanceStopPoint(float stopX, float stopY, int iWidth, int iHeight);
    //void GetMeasureDistanceStopPoint(float *stopX, float *stopY, int iWidth, int iHeight);
    void SetMeasureDistancePoints(float startX, float startY, float stopX, float stopY, int iWidth, int iHeight);
    void GetMeasureDistancePoints(std::list<Mcsf::MedViewer3D::Point2D>& distancePoints, int iWidth, int iHeight);

    void GetMeasureDistancePointsList(std::vector<std::list<Mcsf::MedViewer3D::Point2D>>& distancePointsList, int iWidth, int iHeight);
    void AddMeasureDistancePointsToList();

    void AddMeasureDistancePoints3DToList(std::list<Mcsf::MedViewer3D::Point3D> distancePoints3D);
    void GetMeasureDistancePoints3DList(std::vector<std::list<Mcsf::MedViewer3D::Point3D>>& distancePoints3DList);

    void SetMeasureDistance(float distance);
    void AddMeasureDistanceToList();

    void AddMeasureDistanceIsOnPlaneResultToList(bool isOnPlane);
    void UpdateMeasureDistanceIsOnPlaneResultList(int i, bool isOnPlane);
    void GetMeasureDistanceIsOnPlaneResultList(std::vector<bool>& isOnPlaneList);

    void SetMeasureDistanceActionStatus(bool isActionStarted, bool isActionDone);
    void GetMeasureDistanceActionStatus(bool *isActionStarted, bool *isActionDone);

    void SetMeasureDistancePan(double translationX, double translationY);
    void SetMeasureDistanceScale(double oldfactor, double factor);

    void MeasureDistanceHitTest(float x, float y, int iWidth, int iHeight);
    void GetMeasureDistanceHitIndex(int *hitIndex);

    void DeleteHitDistance();

    // for measure angle operation
    void SetMeasureAnglePoints(std::list<Mcsf::MedViewer3D::Point2D> anglePoints, int iWidth, int iHeight);
    void GetMeasureAnglePoints(std::list<Mcsf::MedViewer3D::Point2D>& anglePoints, int iWidth, int iHeight);

    void GetMeasureAnglePointsList(std::vector<std::list<Mcsf::MedViewer3D::Point2D>>& anglePointsList, int iWidth, int iHeight);
    void AddMeasureAnglePointsToList();

    void AddMeasureAnglePoints3DToList(std::list<Mcsf::MedViewer3D::Point3D> anglePoints3D);
    void GetMeasureAnglePoints3DList(std::vector<std::list<Mcsf::MedViewer3D::Point3D>>& anglePoints3DList);

    void SetMeasureAngleCursorPoint(float cursorX, float cursorY, int iWidth, int iHeight);
    void GetMeasureAngleCursorPoint(float *cursorX, float *cursorY, int iWidth, int iHeight);

    void SetMeasureAngleActionStatus(bool isActionStarted, bool isFirstSideDone, bool isSecondSideDone);
    void GetMeasureAngleActionStatus(bool *isActionStarted, bool *isFirstSideDone, bool *isSecondSideDone);

    void SetMeasureAnglePan(double translationX, double translationY);
    void SetMeasureAngleScale(double oldfactor, double factor);

    void SetMeasureAngle(float angle);
    void AddMeasureAngleToList();

    void AddMeasureAngleIsOnPlaneResultToList(bool isOnPlane);
    void UpdateMeasureAngleIsOnPlaneResultList(int i, bool isOnPlane);
    void GetMeasureAngleIsOnPlaneResultList(std::vector<bool>& isOnPlaneList);

    //for measure value operation
    void SetMeasureValGO(std::shared_ptr<GalleryMeasureValGraphicObject> measureValGOPtr);
    void UpdateMeasureValGO();

private:
    Mcsf::MedViewer3D::Point2D ScreenCoordinateToImageCoordinate(Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight);
    Mcsf::MedViewer3D::Point2D ImageCoordinateToScreenCoordinate(Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight);
    double DistanceSquare(const Mcsf::MedViewer3D::Point2D srcPoint, const Mcsf::MedViewer3D::Point2D dstPoint);
    bool IntersectPoint(const Mcsf::MedViewer3D::Point2D hitpoint, const Mcsf::MedViewer3D::Point2D dstPoint, double tolerance);
    bool IntersectLine(const Mcsf::MedViewer3D::Point2D vPoint1, const Mcsf::MedViewer3D::Point2D vPoint2,
        const Mcsf::MedViewer3D::Point2D vPoint3, double dDistance, double* disPointtoLeneSquare);


private:
    // for measure distance(all the points are in image CS)
    //float                                   mRulerStartX;
    //float                                   mRulerStartY;
    //float                                   mRulerStopX;
    //float                                   mRulerStopY;
    std::list<Mcsf::MedViewer3D::Point2D>   mRulerPoints;
    std::vector<std::list<Mcsf::MedViewer3D::Point2D>>   mRulerPointsList;
    std::vector<std::list<Mcsf::MedViewer3D::Point3D>>   mRulerPoints3DList;
    float                                                mRulerDistance;
    std::vector<float>                                   mRulerDistanceList;
    std::vector<bool>                                    mRulerIsOnPlaneList;
    bool                                    mRulerIsActionStarted;
    bool                                    mRulerIsActionDone;
    int                                     mRulerHitIndex;
    int                                     mRulerHitType;// To do, distinguish the point or the line which is hit.

    // for measure angle (all the points are in image CS)
    float                                   mProtractorCursorX;
    float                                   mProtractorCursorY;
    std::list<Mcsf::MedViewer3D::Point2D>   mProtractorAnglePoints;
    std::vector<std::list<Mcsf::MedViewer3D::Point2D>>   mProtractorAnglePointsList;
    std::vector<std::list<Mcsf::MedViewer3D::Point3D>>   mProtractorAnglePoints3DList;
    float                                                mProtractorAngle;
    std::vector<float>                                   mProtractorAngleList;
    std::vector<bool>                                    mProtractorIsOnPlaneList;
    bool                                    mProtractorActionStarted;
    bool                                    mProtractorFirstSideDone;
    bool                                    mProtractorSecondSideDone;

    //for measure value GO
    std::shared_ptr<GalleryMeasureValGraphicObject> mMeasureValGOPtr;
};
TPS_END_NAMESPACE
#endif