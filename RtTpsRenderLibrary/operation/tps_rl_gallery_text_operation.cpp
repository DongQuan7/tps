////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  yuxuan.duan  yuxuan.duan@united-imaging.com
/// 
///  \file tps_rl_gallery_text_operation.cpp 
/// 
///  \brief class TpsGalleryTextOperation
/// 
///  \version 1.01
/// 
///  \date    2015/06/12
////////////////////////////////////////////////////////////////
#include <math.h>
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_text_operation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h "
#include "RtTpsRenderLibrary/tps_rl_gallery_textgraphicobject.h"


//ZHENGHE
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
TPS_BEGIN_NAMESPACE
TpsGalleryTextOperation::TpsGalleryTextOperation(const tps::DISPLAY_SIZE &displaySize,  const LAYOUT_UNIT &unit,
    const float startX, const float startY, const float textWidth, const float textHeight, const std::string text)
    : mWidth(displaySize.width), mHeight(displaySize.height), mUnit(unit), mStartX(startX), mStartY(startY), mTextWidth(textWidth), mTextHeight(textHeight),
    mText(text), mVolumeKey(""), mMprKey(""), mTextKey(""){
}

TpsGalleryTextOperation::~TpsGalleryTextOperation()
{

}

void TpsGalleryTextOperation::SetGraphicObjectKeys( const std::string& sVolumeKey,
    const std::string& sMprKey, 
    const std::string& sTextKey) 
{
    mVolumeKey = sVolumeKey;
    mMprKey = sMprKey;
    mTextKey = sTextKey;
}

bool TpsGalleryTextOperation::ModifyGraphicObject() const
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

        ////modify measure distance GO
        //go = mModelWarehouse->GetModelObject(mMeasureKey);
        //auto measureDistanceGO = std::dynamic_pointer_cast<GalleryMeasureGraphicObject>(go);
        //if(measureDistanceGO == nullptr){
        //    TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects to GalleryMeasureGraphicObject\n";
        //    return false;
        //}

        //measureDistanceGO->SetMeasureDistancePoints(mStartX, mStartY, mStopX, mStopY, mWidth, mHeight);
        //measureDistanceGO->SetMeasureDistanceActionStatus(mActionStarted, mActionDone);

        ////measureAngleGO->SetMeasureAnglePoints(mAnglePoints, iWidth, iHeight);
        ////measureAngleGO->SetMeasureAngleCursorPoint(mCursorX, mCursorY, iWidth, iHeight);
        ////measureAngleGO->SetMeasureAngleActionStatus(mActionStarted, mFirstSideDone, mSecondSideDone);

        //Point2D pt2dStart = (mWidth > mHeight) ? 
        //    Point2D((mStartX - 0.5) * dScaleRatio + 0.5, mStartY) :
        //Point2D(mStartX, (mStartY - 0.5) * dScaleRatio + 0.5);
        //Point3D pt3dStart = mprGO->PlaneCoordinateToWorldCoordiate(pt2dStart.x, pt2dStart.y);
        //Point3D pt3dStartInPatient = volumeGO->GetWorld2Patient().Transform(pt3dStart);

        //Point2D pt2dStop = (mWidth > mHeight) ? 
        //    Point2D((mStopX - 0.5) * dScaleRatio + 0.5, mStopY) :
        //Point2D(mStopX, (mStopY - 0.5) * dScaleRatio + 0.5);
        //Point3D pt3dStop = mprGO->PlaneCoordinateToWorldCoordiate(pt2dStop.x, pt2dStop.y);
        //Point3D pt3dStopInPatient = volumeGO->GetWorld2Patient().Transform(pt3dStop);

        //float distance = CalculateDistance(pt3dStartInPatient, pt3dStopInPatient);
        //measureDistanceGO->SetMeasureDistance(distance);

        //if(mActionDone)
        //{
        //    measureDistanceGO->AddMeasureDistanceToList();
        //    measureDistanceGO->AddMeasureDistancePointsToList();

        //    std::list<Mcsf::MedViewer3D::Point3D> distancePoints3D;
        //    distancePoints3D.push_back(pt3dStart);
        //    distancePoints3D.push_back(pt3dStop);
        //    measureDistanceGO->AddMeasureDistancePoints3DToList(distancePoints3D);


        //    bool isOnPlane = true;
        //    Point3D pt3dStartProject = mprGO->ProjectToPlane(pt3dStart);
        //    isOnPlane &= (pt3dStart == pt3dStartProject);
        //    Point3D pt3dStopProject = mprGO->ProjectToPlane(pt3dStop);
        //    isOnPlane &= (pt3dStop == pt3dStopProject);
        //    measureDistanceGO->AddMeasureDistanceIsOnPlaneResultToList(isOnPlane);
        //}
        //measureDistanceGO->SetDirty(true);

        //modify measure distance value GO
        go = mModelWarehouse->GetModelObject(mTextKey);
        auto textGO = std::dynamic_pointer_cast<GalleryTextGraphicObject>(go);
        if(textGO == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects to GalleryTextGraphicObject\n";
            return false;
        }

        //update text list
        textGO->AddTextList(mText);

        //update text pos list
        Mcsf::MedViewer3D::Point2D textStartCursorPos;
        textStartCursorPos.x = mStartX;
        textStartCursorPos.y = mStartY;
        textGO->AddTextStartCursorPosList(textStartCursorPos, mWidth, mHeight);

        //update text 4 2d vertex list
        std::list<Mcsf::MedViewer3D::Point2D> text4Vertex;
        text4Vertex.push_back(textStartCursorPos);

        Mcsf::MedViewer3D::Point2D textLeftBottomPos;
        textLeftBottomPos.x = mStartX;
        textLeftBottomPos.y = mStartY - mTextHeight / mHeight;
        text4Vertex.push_back(textLeftBottomPos);

        Mcsf::MedViewer3D::Point2D textRightTopPos;
        textRightTopPos.x = mStartX + mTextWidth / mWidth;
        textRightTopPos.y = mStartY;
        text4Vertex.push_back(textRightTopPos);

        Mcsf::MedViewer3D::Point2D textRightBottomPos;
        textRightBottomPos.x = mStartX + mTextWidth / mWidth;
        textRightBottomPos.y = mStartY - mTextHeight / mHeight;
        text4Vertex.push_back(textRightBottomPos);

        textGO->AddText4VertexList(text4Vertex, mWidth, mHeight);

        //update text 4 3d vertex list
        std::list<Mcsf::MedViewer3D::Point3D> text4Vertex3D;

        Point2D pt2dLeftTop = (mWidth > mHeight) ? 
            Point2D((textStartCursorPos.x - 0.5) * dScaleRatio + 0.5, textStartCursorPos.y) :
        Point2D(textStartCursorPos.x, (textStartCursorPos.y - 0.5) * dScaleRatio + 0.5);
        Point3D pt3dLeftTop = mprGO->PlaneCoordinateToWorldCoordiate(pt2dLeftTop.x, pt2dLeftTop.y);
        text4Vertex3D.push_back(pt3dLeftTop);

        Point2D pt2dLeftBottom = (mWidth > mHeight) ? 
            Point2D((textLeftBottomPos.x - 0.5) * dScaleRatio + 0.5, textLeftBottomPos.y) :
        Point2D(textLeftBottomPos.x, (textLeftBottomPos.y - 0.5) * dScaleRatio + 0.5);
        Point3D pt3dLeftBottom = mprGO->PlaneCoordinateToWorldCoordiate(pt2dLeftBottom.x, pt2dLeftBottom.y);
        text4Vertex3D.push_back(pt3dLeftBottom);

        Point2D pt2dRightTop = (mWidth > mHeight) ? 
            Point2D((textRightTopPos.x - 0.5) * dScaleRatio + 0.5, textRightTopPos.y) :
        Point2D(textRightTopPos.x, (textRightTopPos.y - 0.5) * dScaleRatio + 0.5);
        Point3D pt3dRightTop = mprGO->PlaneCoordinateToWorldCoordiate(pt2dRightTop.x, pt2dRightTop.y);
        text4Vertex3D.push_back(pt3dRightTop);

        Point2D pt2dRightBottom = (mWidth > mHeight) ? 
            Point2D((textRightBottomPos.x - 0.5) * dScaleRatio + 0.5, textRightBottomPos.y) :
        Point2D(textRightBottomPos.x, (textRightBottomPos.y - 0.5) * dScaleRatio + 0.5);
        Point3D pt3dRightBottom = mprGO->PlaneCoordinateToWorldCoordiate(pt2dRightBottom.x, pt2dRightBottom.y);
        text4Vertex3D.push_back(pt3dRightBottom);

        textGO->AddText4Vertex3DList(text4Vertex3D);

        //update text isOnPlane list, only need to check 3 vertex
        bool isOnPlane = true;
        for(std::list<Mcsf::MedViewer3D::Point3D>::const_iterator iter = text4Vertex3D.begin(); iter != text4Vertex3D.end(); iter++)
        {
            Point3D pt3dProject = mprGO->ProjectToPlane(*iter);
            isOnPlane &= (*iter == pt3dProject);
        }
        textGO->AddTextIsOnPlaneList(isOnPlane);

        //update text value GO
        textGO->UpdateTextGO();
        //textGO->SetDirty(true);
        mprGO->SetDirty(true);
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Exception happened in TpsGalleryMeasureDistanceOperation::ModifyGraphicObject():"<<ex.what();
        return false;
    }
    return true;
}

TPS_END_NAMESPACE