////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  yuxuan.duan  yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_measuredistance_operation.cpp 
/// 
///  \brief class TpsGalleryMeasureDistanceOperation
/// 
///  \version 1.0
/// 
///  \date    2015/04/22
////////////////////////////////////////////////////////////////
#include <math.h>
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measuredistance_operation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h "
#include "RtTpsRenderLibrary/tps_rl_gallery_measuregraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measurevaluegraphicobject.h"


//ZHENGHE
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
TPS_BEGIN_NAMESPACE
    TpsGalleryMeasureDistanceOperation::TpsGalleryMeasureDistanceOperation(const tps::DISPLAY_SIZE &displaySize,  const LAYOUT_UNIT &unit,
    const float startX, const float startY, const float stopX, const float stopY,
    const bool actionStarted, const bool actionDone)
    : mWidth(displaySize.width), mHeight(displaySize.height), mUnit(unit), mStartX(startX), mStartY(startY), mStopX(stopX), mStopY(stopY),
    mActionStarted(actionStarted), mActionDone(actionDone),
    mVolumeKey(""), mMprKey(""), mMeasureKey(""), mMeasureValKey(""){
}

TpsGalleryMeasureDistanceOperation::~TpsGalleryMeasureDistanceOperation()
{

}

void TpsGalleryMeasureDistanceOperation::SetGraphicObjectKeys( const std::string& sVolumeKey,
    const std::string& sMprKey, 
    const std::string& sMeasureKey, 
    const std::string& sMeasrueValKey) 
{
    mVolumeKey = sVolumeKey;
    mMprKey = sMprKey;
    mMeasureKey = sMeasureKey;
    mMeasureValKey = sMeasrueValKey;
}

bool TpsGalleryMeasureDistanceOperation::ModifyGraphicObject() const
{
    try{
        using namespace Mcsf::MedViewer3D;

        //Volume GO
        auto go = mModelWarehouse->GetModelObject(mVolumeKey);
        auto volumeGO = std::dynamic_pointer_cast<GraphicObjVolume>(go);
        if(volumeGO == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects to VolumeGraphicObject\n";
            return false;
        }

        //mpr GO
        go = mModelWarehouse->GetModelObject(mMprKey);
        auto mprGO = std::dynamic_pointer_cast<ClsGraphicObjectMPR>(go);
        if(mprGO == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects to MprGraphicObject\n";
            return false;
        }
        //int iWidth = 0, iHeight = 0;
        //mprGO->GetDisplaySize(&iWidth, &iHeight);
        if (mWidth <= 0 || mHeight <=0 ){
            return false;
        }
        double dScaleRatio = (mWidth >= mHeight) ? double(mWidth) / mHeight : double(mHeight) / mWidth;

        //modify measure distance GO
        go = mModelWarehouse->GetModelObject(mMeasureKey);
        auto measureDistanceGO = std::dynamic_pointer_cast<GalleryMeasureGraphicObject>(go);
        if(measureDistanceGO == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects to GalleryMeasureGraphicObject\n";
            return false;
        }

        measureDistanceGO->SetMeasureDistancePoints(mStartX, mStartY, mStopX, mStopY, mWidth, mHeight);
        measureDistanceGO->SetMeasureDistanceActionStatus(mActionStarted, mActionDone);

        //measureAngleGO->SetMeasureAnglePoints(mAnglePoints, iWidth, iHeight);
        //measureAngleGO->SetMeasureAngleCursorPoint(mCursorX, mCursorY, iWidth, iHeight);
        //measureAngleGO->SetMeasureAngleActionStatus(mActionStarted, mFirstSideDone, mSecondSideDone);

        Point2D pt2dStart = (mWidth > mHeight) ? 
            Point2D((mStartX - 0.5) * dScaleRatio + 0.5, mStartY) :
        Point2D(mStartX, (mStartY - 0.5) * dScaleRatio + 0.5);
        Point3D pt3dStart = mprGO->PlaneCoordinateToWorldCoordiate(pt2dStart.x, pt2dStart.y);
        Point3D pt3dStartInPatient = volumeGO->GetWorld2Patient().Transform(pt3dStart);

        Point2D pt2dStop = (mWidth > mHeight) ? 
            Point2D((mStopX - 0.5) * dScaleRatio + 0.5, mStopY) :
        Point2D(mStopX, (mStopY - 0.5) * dScaleRatio + 0.5);
        Point3D pt3dStop = mprGO->PlaneCoordinateToWorldCoordiate(pt2dStop.x, pt2dStop.y);
        Point3D pt3dStopInPatient = volumeGO->GetWorld2Patient().Transform(pt3dStop);

        float distance = CalculateDistance(pt3dStartInPatient, pt3dStopInPatient);
        measureDistanceGO->SetMeasureDistance(distance);

        if(mActionDone)
        {
            measureDistanceGO->AddMeasureDistanceToList();
            measureDistanceGO->AddMeasureDistancePointsToList();

            std::list<Mcsf::MedViewer3D::Point3D> distancePoints3D;
            distancePoints3D.push_back(pt3dStart);
            distancePoints3D.push_back(pt3dStop);
            measureDistanceGO->AddMeasureDistancePoints3DToList(distancePoints3D);


            bool isOnPlane = true;
            Point3D pt3dStartProject = mprGO->ProjectToPlane(pt3dStart);
            isOnPlane &= (pt3dStart == pt3dStartProject);
            Point3D pt3dStopProject = mprGO->ProjectToPlane(pt3dStop);
            isOnPlane &= (pt3dStop == pt3dStopProject);
            measureDistanceGO->AddMeasureDistanceIsOnPlaneResultToList(isOnPlane);
        }
        measureDistanceGO->SetDirty(true);

        //modify measure distance value GO
        go = mModelWarehouse->GetModelObject(mMeasureValKey);
        auto measureDistanceValGO = std::dynamic_pointer_cast<GalleryMeasureValGraphicObject>(go);
        if(measureDistanceValGO == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects to GalleryMeasureValGraphicObject\n";
            return false;
        }
        measureDistanceGO->SetMeasureValGO(measureDistanceValGO);

        measureDistanceGO->UpdateMeasureValGO();

    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Exception happened in TpsGalleryMeasureDistanceOperation::ModifyGraphicObject():"<<ex.what();
        return false;
    }
    return true;
}

float TpsGalleryMeasureDistanceOperation::CalculateDistance(Mcsf::MedViewer3D::Point3D startPoint, Mcsf::MedViewer3D::Point3D stopPoint) const
{
    float x_dis_inWorld = (stopPoint.x - startPoint.x) * (stopPoint.x - startPoint.x);
    float y_dis_inWorld = (stopPoint.y - startPoint.y) * (stopPoint.y - startPoint.y);
    float z_dis_inWorld = (stopPoint.z - startPoint.z) * (stopPoint.z - startPoint.z);
    
    return sqrt(x_dis_inWorld + y_dis_inWorld + z_dis_inWorld) / 10; //unit convert to cm
}

//Mcsf::MedViewer3D::Point2D TpsGalleryMeasureAngleOperation::ScreenCoordinateToImageCoordinate
//    (Mcsf::MedViewer3D::Point2D anglePoint, 
//    int iWidth, int iHeight) const
//{
//    int iMinL = std::min(iWidth, iHeight);
//
//    Mcsf::MedViewer3D::Point2D anglePointOut(0, 0);
//    anglePointOut.x = (anglePoint.x - 0.5) * iWidth / iMinL * 2;
//    anglePointOut.y = (anglePoint.y - 0.5) * iHeight / iMinL * 2;
//
//    return anglePointOut;
//}

TPS_END_NAMESPACE