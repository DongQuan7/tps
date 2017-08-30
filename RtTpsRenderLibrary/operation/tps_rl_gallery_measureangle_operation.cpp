////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  yuxuan.duan  yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_measureangle_operation.cpp 
/// 
///  \brief class TpsGalleryMeasureAngleOperation
/// 
///  \version 1.0
/// 
///  \date    2015/03/06
////////////////////////////////////////////////////////////////
#include <math.h>
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measureangle_operation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h "
#include "RtTpsRenderLibrary/tps_rl_gallery_measuregraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measurevaluegraphicobject.h"


//ZHENGHE
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
TPS_BEGIN_NAMESPACE
    TpsGalleryMeasureAngleOperation::TpsGalleryMeasureAngleOperation(const tps::DISPLAY_SIZE &displaySize, const LAYOUT_UNIT &unit, 
    const list<Mcsf::MedViewer3D::Point2D>& anglePoints, 
    const float cursorX, const float cursorY,
    const bool actionStarted, const bool firstSideDone, const bool secondSideDone)
    : mWidth(displaySize.width), mHeight(displaySize.height), mUnit(unit), mAnglePoints(anglePoints), mCursorX(cursorX), mCursorY(cursorY),
    mActionStarted(actionStarted), mFirstSideDone(firstSideDone), mSecondSideDone(secondSideDone),
    mMprKey(""), mMeasureKey(""), mMeasureValKey(""){
}

TpsGalleryMeasureAngleOperation::~TpsGalleryMeasureAngleOperation()
{

}

void TpsGalleryMeasureAngleOperation::SetGraphicObjectKeys(const std::string& sMprKey, 
    const std::string& sMeasureKey, 
    const std::string& sMeasrueValKey) 
{
    mMprKey = sMprKey;
    mMeasureKey = sMeasureKey;
    mMeasureValKey = sMeasrueValKey;
}

bool TpsGalleryMeasureAngleOperation::ModifyGraphicObject() const
{
    try{
        using namespace Mcsf::MedViewer3D;

        //mpr GO
        auto go = mModelWarehouse->GetModelObject(mMprKey);
        auto mprGO = std::dynamic_pointer_cast<ClsGraphicObjectMPR>(go);
        if(mprGO == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects to MprGraphicObject\n";
            return false;
        }
        //mprGO->GetDisplaySize(&iWidth, &iHeight); 
        if (mWidth <= 0 || mHeight <=0 ){
            return false;
        }
        double dScaleRatio = (mWidth >= mHeight) ? double(mWidth) / mHeight : double(mHeight) / mWidth;

        ////cell coordinate to image coordinate
        //std::list<Mcsf::MedViewer3D::Point2D> anglePointsInImageCoordinate;
        //for(std::list<Mcsf::MedViewer3D::Point2D>::const_iterator iter = mAnglePoints.begin(); iter != mAnglePoints.end(); iter++)
        //{
        //    Mcsf::MedViewer3D::Point2D anglePointNew = 
        //    ScreenCoordinateToImageCoordinate(*iter, iWidth, iHeight);
        //    anglePointsInImageCoordinate.push_back(anglePointNew);
        //}

        //Mcsf::MedViewer3D::Point2D cursor(mCursorX, mCursorY);
        //Mcsf::MedViewer3D::Point2D cursorInImageCoordinate = 
        //    ScreenCoordinateToImageCoordinate(cursor, iWidth, iHeight);

        //modify measure angle GO
        go = mModelWarehouse->GetModelObject(mMeasureKey);
        auto measureAngleGO = std::dynamic_pointer_cast<GalleryMeasureGraphicObject>(go);
        if(measureAngleGO == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects to GalleryMeasureGraphicObject\n";
            return false;
        }
        
        measureAngleGO->SetMeasureAnglePoints(mAnglePoints, mWidth, mHeight);
        measureAngleGO->SetMeasureAngleCursorPoint(mCursorX, mCursorY, mWidth, mHeight);
        measureAngleGO->SetMeasureAngleActionStatus(mActionStarted, mFirstSideDone, mSecondSideDone);

        float angle = 0.0;
        if (mAnglePoints.size() == 2)
        {
            std::list<Mcsf::MedViewer3D::Point2D>   mTempAnglePoints;
            mTempAnglePoints = mAnglePoints;
            Mcsf::MedViewer3D::Point2D cursorPoint;
            cursorPoint.x = mCursorX;
            cursorPoint.y = mCursorY;              
            mTempAnglePoints.push_back(cursorPoint);
            angle = CalculateAngle(mTempAnglePoints, mWidth, mHeight);
            measureAngleGO->SetMeasureAngle(angle);
        }
        if(mAnglePoints.size() == 3)
        {
            angle = CalculateAngle(mAnglePoints, mWidth, mHeight);
            measureAngleGO->SetMeasureAngle(angle);
            measureAngleGO->AddMeasureAngleToList();
            measureAngleGO->AddMeasureAnglePointsToList();

            //calculate the 3D points
            std::list<Point3D>  tempAnglePoints3D;
            bool isOnPlane = true;
            for(std::list<Point2D>::const_iterator i = mAnglePoints.begin(); i != mAnglePoints.end(); ++i)
            {
                Point2D pt2d = (mHeight > mHeight) ? 
                    Point2D((i->x - 0.5) * dScaleRatio + 0.5, i->y) :
                Point2D(i->x, (i->y - 0.5) * dScaleRatio + 0.5);
                Point3D pt3d = mprGO->PlaneCoordinateToWorldCoordiate(pt2d.x, pt2d.y);
                Point3D pt3dProject = mprGO->ProjectToPlane(pt3d);
                isOnPlane &= (pt3d == pt3dProject);
                tempAnglePoints3D.push_back(pt3d);
            }
            measureAngleGO->AddMeasureAnglePoints3DToList(tempAnglePoints3D);
            measureAngleGO->AddMeasureAngleIsOnPlaneResultToList(isOnPlane);
        }
        
        measureAngleGO->SetDirty(true);

        //modify measure angle value GO
        go = mModelWarehouse->GetModelObject(mMeasureValKey);
        auto measureAngleValGO = std::dynamic_pointer_cast<GalleryMeasureValGraphicObject>(go);
        if(measureAngleValGO == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects to GalleryMeasureValGraphicObject\n";
            return false;
        }
        measureAngleGO->SetMeasureValGO(measureAngleValGO);

        //if(mAnglePoints.size() == 3)
        //{
            measureAngleGO->UpdateMeasureValGO();
        //}

    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Exception happened in TpsGalleryMeasureDistanceOperation::ModifyGraphicObject():"<<ex.what();
        return false;
    }
    return true;
}

float TpsGalleryMeasureAngleOperation::CalculateAngle(std::list<Mcsf::MedViewer3D::Point2D> anglePoints, int iWidth, int iHeight) const
{
    Mcsf::MedViewer3D::Point2D origin = anglePoints.front();
    std::list<Mcsf::MedViewer3D::Point2D>::const_iterator i = anglePoints.begin();
    Mcsf::MedViewer3D::Point2D side1Vertex = *(++i);
    Mcsf::MedViewer3D::Point2D side2Vertex = anglePoints.back();
    
    double ratio = (double)iHeight / (double)iWidth;
    double originX = origin.x;
    double originY = origin.y * ratio;
    double side1VertexX = side1Vertex.x;
    double side1VertexY = side1Vertex.y * ratio;
    double side2VertexX = side2Vertex.x;
    double side2VertexY = side2Vertex.y * ratio;

    // Rational:
    // AB = (B.X-A.X, B.Y-A.Y)
    // AC = (C.X-A.X, C.Y-A.Y)
    // cosA = (AB*AC)/(|AB|*|AC|)
    double AB_X = side1VertexX - originX;
    double AB_Y = side1VertexY - originY;
    double AC_X = side2VertexX - originX;
    double AC_Y = side2VertexY - originY;
    double AB_dot_AC = (AB_X * AC_X) + (AB_Y * AC_Y);
    double AB_Magnitude = sqrt(AB_X * AB_X + AB_Y * AB_Y);
    double AC_Magnitude = sqrt(AC_X * AC_X + AC_Y * AC_Y);
    double cosA = AB_dot_AC / (AB_Magnitude * AC_Magnitude);
    double A = acos(cosA) * 180 / PI;
    return (float)A;
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