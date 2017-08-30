//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
///
///  \file    tps_rl_gallery_mprrotate_operation.cpp
///  \brief   
///
///  \version 1.0
///  \date    Feb. 05, 2015
///  \
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_mprrotate_operation.h"

//ZHENGHE
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"

//TPS
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsGalleryMPRRotateOperation::TpsGalleryMPRRotateOperation(int width, int height, float startx, float starty, float stopx, float stopy)
    :mWidth(width), mHeight(height), mStartX(startx),mStartY(starty),
    mStopX(stopx),mStopY(stopy),
    mVolumeKey(""),mMPRKey(""){
}

TpsGalleryMPRRotateOperation::~TpsGalleryMPRRotateOperation(){

}

void TpsGalleryMPRRotateOperation::SetGraphicObjectKeys(const std::string& sVolumeKey, 
    const std::string& sMPRKey) {
        mVolumeKey = sVolumeKey;
        mMPRKey = sMPRKey;
}

bool TpsGalleryMPRRotateOperation::ModifyGraphicObject() const{

    try{
        using namespace Mcsf::MedViewer3D;

        std::vector<std::string> vSplit;
        boost::split(vSplit, mMPRKey, boost::is_any_of("|"));
        if (vSplit.size() != 2){
            TPS_LOG_DEV_ERROR<<"The graphic object type name is not right.";
            return nullptr;
        }

        std::string imageUID = vSplit[0];
        GO_TYPE mprGoType = GOTypeHelper::ToGoType(vSplit[1].c_str());
        auto section = GOTypeHelper::GetFirstPostfixComponent(mprGoType);

        auto goVolume = mModelWarehouse->GetGraphicObject(mVolumeKey);
        auto goMpr = mModelWarehouse->GetGraphicObject(mMPRKey);

        //down cast
        auto mpr = std::dynamic_pointer_cast<ClsGraphicObjectMPR>(goMpr);
        if (mpr == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects to goMpr.";
            return false;
        }

        // calculation for operation
        // 1. convert translation points from screen cs to MPR cs
        //int iWidth = 0, iHeight = 0;
        //mprGO->GetDisplaySize(&iWidth, &iHeight);
        if (mWidth <= 0 || mHeight <=0 ){
            return false;
        }
        double dScaleRatio = (mWidth >= mHeight) ? double(mWidth) / mHeight : double(mHeight) / mWidth;

        // Case 1: pan|xStart|yStart|xEnd|yEnd
        Point2D pt2dStart = (mWidth > mHeight) ? 
            Point2D((mStartX - 0.5) * dScaleRatio + 0.5, mStartY) :
        Point2D(mStartX, (mStartY - 0.5) * dScaleRatio + 0.5);

        Point2D pt2dEnd = (mWidth > mHeight) ? 
            Point2D((mStopX - 0.5 ) * dScaleRatio + 0.5, mStopY) :
        Point2D(mStopX, (mStopY - 0.5) * dScaleRatio + 0.5);

        //Point2D pt2dCenter = (iWidth > iHeight) ? 
        //    Point2D((-0.5) * dScaleRatio + 0.5, 0.0) :
        //Point2D(0.0, (-0.5) * dScaleRatio + 0.5);

        Point2D pt2dCenter(0.5, 0.5);

        // 2. recalculate the MPR GO cs
        Point3D pt3dStart = mpr->PlaneCoordinateToWorldCoordiate(pt2dStart.x, pt2dStart.y);
        Point3D pt3dEnd   = mpr->PlaneCoordinateToWorldCoordiate(pt2dEnd.x, pt2dEnd.y);
        //Point3D pt3dCenter = mpr->GetCenter();
        Point3D pt3dCenter = mpr->PlaneCoordinateToWorldCoordiate(pt2dCenter.x, pt2dCenter.y);
        Vector3D vecStart = pt3dStart - pt3dCenter;
        Vector3D vecEnd = pt3dEnd - pt3dCenter;
        double dAngle = vecStart.AngleBetween(vecEnd);
        dAngle *= -1;
        Vector3D normal = vecStart.CrossProduct(vecEnd);
        //if (normal.z > 0) dAngle *= -1;
        if (abs(dAngle)<0.001) return true;

        Mcsf::MedViewer3D::Quaternion qRotation(dAngle, normal);
        mpr->SetRotate(qRotation);
        mpr->SetDirty(true);

        //CrosshairGo===========================================================================
        auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, section);
        std::string strKey = imageUID + "|" + GOTypeHelper::ToString(goType);
        auto go = mModelWarehouse->GetModelObject(strKey);
        auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
        if (spCrosshairGO != nullptr){
            // Update crosshair in THIS mpr
            Mcsf::MedViewer3D::Point3D pt3dCrosshair;
            spCrosshairGO->GetCrosshairPoint3d(&pt3dCrosshair);
            Mcsf::MedViewer3D::Point2D pt2dCrosshair = mpr->GetPlaneCoordinate(pt3dCrosshair, false);
            spCrosshairGO->SetCrosshairPoint2d(pt2dCrosshair);
            float fAngleX, fAngleY;
            spCrosshairGO->GetAngles(&fAngleX, &fAngleY);
            if (normal.z < 0) dAngle *= -1;
            fAngleX += dAngle * 180 / PI;
            fAngleY += dAngle * 180 / PI;
            spCrosshairGO->SetAngles(fAngleX, fAngleY);
            spCrosshairGO->SetDirty(true);
        }
        return true;
    }

    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"TpsGalleryMPRPanOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }
    //return true;
}

TPS_END_NAMESPACE