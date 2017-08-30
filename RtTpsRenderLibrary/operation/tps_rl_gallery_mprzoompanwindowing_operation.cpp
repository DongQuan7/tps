//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rl_gallery_mprzoompanwindowing_operation.cpp
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_mprzoompanwindowing_operation.h"

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
#include "RtTpsRenderLibrary/tps_rl_gallery_measuregraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_textgraphicobject.h"


#define WINDOW_WIDTH_MIN 1
#define WINDOW_WIDTH_MAX_CT 9727
#define WINDOW_CENTER_MIN_CT -1536
#define WINDOW_CENTER_MAX_CT 8191
#define WINDOW_WIDTH_MAX_MR 4096
#define WINDOW_CENTER_MIN_MR 0
#define WINDOW_CENTER_MAX_MR 4095
TPS_BEGIN_NAMESPACE   // begin namespace tps

    TpsGalleryMPRZoomOperation::TpsGalleryMPRZoomOperation(float fZoomFactor)
    :mZoomFactor(fZoomFactor),
    mVolumeKey(""),mMPRKey(""), mMprRulerTextKey(""),
    mZoomInputMode(BY_FACTOR ),
    mWidth(0), mHeight(0),
    mStartX(0), mStartY(0), 
    mStopX(1), mStopY(1){

}

TpsGalleryMPRZoomOperation::TpsGalleryMPRZoomOperation(int width, int height,
    float startX, float startY, float stopX, float stopY) 
    : mWidth(width), mHeight(height), mStartX(startX), mStartY(startY), mStopX(stopX), mStopY(stopY),
    mVolumeKey(""), mMPRKey(""), mMprRulerTextKey(""){
        mZoomInputMode = BY_VECTOR;
        mZoomFactor = 1;
}

TpsGalleryMPRZoomOperation::~TpsGalleryMPRZoomOperation(){

}

void TpsGalleryMPRZoomOperation::SetGraphicObjectKeys(const std::string& sVolumeKey, 
    const std::string& sMPRKey, const std::string &mprRulerTextKey) {
        mVolumeKey = sVolumeKey;
        mMPRKey = sMPRKey;
        mMprRulerTextKey = mprRulerTextKey;
}

bool TpsGalleryMPRZoomOperation::ModifyGraphicObject() const{
    try{

        std::vector<std::string> vSplit;
        boost::split(vSplit, mMPRKey, boost::is_any_of("|"));
        if (vSplit.size() != 2){
            TPS_LOG_DEV_ERROR<<"The graphic object type name is not right.";
            return nullptr;
        }

        std::string imageUID = vSplit[0];
        GO_TYPE mprGoType = GOTypeHelper::ToGoType(vSplit[1].c_str());
        auto section = GOTypeHelper::GetFirstPostfixComponent(mprGoType);
        ModifyGraphicObject_i(imageUID, section);
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"MPRPageTurnOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }
    return true;
}

bool TpsGalleryMPRZoomOperation::ModifyGraphicObject_i(
    const std::string& imageUID, 
    FIRST_POSTFIX_COMPONENT sectionPage,
    SECOND_POSTFIX_COMPONENT location) const {
        using namespace Mcsf::MedViewer3D;

        auto goVolume = mModelWarehouse->GetGraphicObject(mVolumeKey);
        GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, sectionPage, location);
        std::string mprKey = imageUID + "|" + GOTypeHelper::ToString(goType);
        auto goMpr = mModelWarehouse->GetGraphicObject(mprKey);
        //auto goMprRulerText = mModelEntity->GetGraphicObject(mMprRulerTextKey);

        //down cast
        auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
        auto mpr = std::dynamic_pointer_cast<ClsGraphicObjectMPR>(goMpr);
        //auto mprRulerText = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(goMprRulerText);
        if (volume == nullptr || mpr == nullptr/* || mprRulerText == nullptr*/){
            TPS_LOG_DEV_WARNING<<"Failed to dynamic cast IGraphicObjects to goVoluem\
                                 or goMpr.";
            return false;
        }

        //get old factor
        double factor = 0.0f;
        mpr->GetScale(&factor);
        double oldfactor = factor;

        //calc new factor
        switch (mZoomInputMode) {
        case BY_FACTOR:
            factor *= mZoomFactor;
            break;
        case BY_VECTOR:
            {
                ////get display size
                //int iWidth = 0, iHeight = 0;
                //mpr->GetDisplaySize(&iWidth, &iHeight);
                //// Error
                //if (iWidth <=0 || iHeight <=0){
                //    return false;
                //}
                float ratio = (float)mWidth / mHeight;

                // handle zoom
                float yVar = ratio > 1.0 ? (mStartY - mStopY) : (mStartY - mStopY) * ratio;
                float zoomFactorDelta = std::exp(yVar);

                // constrain inside the range
                factor *= zoomFactorDelta;
                if (factor < 0.1f)
                {
                    factor = 0.1f;
                }
                else if (factor > 15.0f)
                {
                    factor = 15.0f;
                }

                break;
            }
        default:
            return false;
        }

        mpr->SetScale(factor);

        ////===========================================================================
        //goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, sectionPage);
        //std::string strKey = imageUID + "|" + GOTypeHelper::ToString(goType);
        //auto go = mModelWarehouse->GetModelObject(strKey);
        //auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
        //if (spCrosshairGO != nullptr){
        //    // Update CrossHair in THIS mpr
        //    Mcsf::MedViewer3D::Point3D pt3dCrosshair;
        //    spCrosshairGO->GetCrosshairPoint3d(&pt3dCrosshair);
        //    Mcsf::MedViewer3D::Point2D pt2dCrosshair = mpr->GetPlaneCoordinate(pt3dCrosshair, false);
        //    spCrosshairGO->SetCrosshairPoint2d(pt2dCrosshair);
        //    spCrosshairGO->SetDirty(true);
        //}

        //===========================================================================
        goType = GOTypeHelper::ComposeAsGOType(GALLERY_MEASURE, sectionPage);
        std::string strKey = imageUID + "|" + GOTypeHelper::ToString(goType);
        auto go = mModelWarehouse->GetModelObject(strKey);
        auto measureGO = std::dynamic_pointer_cast<GalleryMeasureGraphicObject>(go);
        if (measureGO != nullptr){
            measureGO->SetMeasureAngleScale(oldfactor, factor);
            measureGO->SetMeasureDistanceScale(oldfactor, factor);
            measureGO->SetDirty(true);

            measureGO->UpdateMeasureValGO();
        }

        //===========================================================================
        goType = GOTypeHelper::ComposeAsGOType(GALLERY_TEXT, sectionPage);
        strKey = imageUID + "|" + GOTypeHelper::ToString(goType);
        go = mModelWarehouse->GetModelObject(strKey);
        auto textGO = std::dynamic_pointer_cast<GalleryTextGraphicObject>(go);
        if (textGO != nullptr){
            textGO->SetTextScale(oldfactor, factor);
            textGO->UpdateTextGO();
            textGO->SetDirty(true);
        }

        ////===========================================================================
        //goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR_CORNER_INFO, sectionPage);
        //strKey = imageUID + "|" + GOTypeHelper::ToString(goType);
        //go = mModelWarehouse->GetModelObject(strKey);
        //auto mprCornerInfo = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(go);
        //if (NULL != mprCornerInfo){
        //     
        //    std::string wwwl_key_val = "ZoomFactor";
        //    char strtext_zoomfactor[64]={0};
        //    _snprintf_s(strtext_zoomfactor, sizeof(strtext_zoomfactor), "%.2f", factor);
        //    if(strtext_zoomfactor[sizeof(strtext_zoomfactor) - 1] != '\0'){
        //        //printf("warning:string will be truncated");
        //        strtext_zoomfactor[sizeof(strtext_zoomfactor)-1]='\0';
        //    }
        //    std::wstringstream wstr_zoomfactor;
        //    wstr_zoomfactor << strtext_zoomfactor;

        //    // create new buffer
        //    FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
        //    Pixel32* pStrBuffer = nullptr;
        //    int strWidth = 0, strHeight = 0;

        //    if(!pBufGen->GenerateRasterBuffer(wstr_zoomfactor.str(), &pStrBuffer, strWidth, strHeight)){
        //        TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer.\n";
        //        return false;
        //    }
        //    else{
        //        mprCornerInfo->UpdateTextBlock(wwwl_key_val, (unsigned char*)pStrBuffer, strWidth, strHeight);
        //        if(pStrBuffer!=nullptr){
        //            delete[] pStrBuffer;
        //            pStrBuffer = nullptr;
        //        }
        //    }              
        //}
        
        //===========================================================================

        mpr->SetDirty(true);
        return true;
}

TpsGalleryMPRPanOperation::TpsGalleryMPRPanOperation(int width, int height, float startx, float starty, float stopx, float stopy)
    :mStartX(startx),mStartY(starty),
    mStopX(stopx),mStopY(stopy),
    mVolumeKey(""),mMPRKey(""), 
    mWidth(width), mHeight(height){
}

TpsGalleryMPRPanOperation::~TpsGalleryMPRPanOperation(){

}

void TpsGalleryMPRPanOperation::SetGraphicObjectKeys(const std::string& sVolumeKey, 
    const std::string& sMPRKey) {
        mVolumeKey = sVolumeKey;
        mMPRKey = sMPRKey;
}

bool TpsGalleryMPRPanOperation::ModifyGraphicObject() const{

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
        if (mWidth<=0 || mHeight <=0){
            return false;
        }

        //mpr->GetDisplaySize(&iWidth, &iHeight);
        double dScaleRatio = (mWidth >= mHeight) ? double(mWidth) / mHeight : double(mHeight) / mWidth;

        // Case 1: pan|xStart|yStart|xEnd|yEnd
        Point2D pt2dStart = dScaleRatio>0 ? 
            Point2D((mStartX - 0.5) * dScaleRatio + 0.5, mStartY) :
        Point2D(mStartX, (mStartY - 0.5) * dScaleRatio + 0.5);

        Point2D pt2dEnd = dScaleRatio>0 ? 
            Point2D((mStopX - 0.5 ) * dScaleRatio + 0.5, mStopY) :
        Point2D(mStopX, (mStopY - 0.5) * dScaleRatio + 0.5);

        // 2. recalculate the MPR GO cs
        Point3D pt3dStart = mpr->PlaneCoordinateToWorldCoordiate(pt2dStart.x, pt2dStart.y);
        Point3D pt3dEnd   = mpr->PlaneCoordinateToWorldCoordiate(pt2dEnd.x, pt2dEnd.y);
        Vector3D vTraslation = pt3dEnd - pt3dStart;
        mpr->SetPan(vTraslation.x, vTraslation.y, vTraslation.z);
        mpr->SetDirty(true);

        ////===========================================================================
        //auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, section);
        //std::string strKey = imageUID + "|" + GOTypeHelper::ToString(goType);
        //auto go = mModelWarehouse->GetModelObject(strKey);
        //auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
        //if (spCrosshairGO != nullptr){
        //    // Update crosshair in THIS mpr
        //    Mcsf::MedViewer3D::Point3D pt3dCrosshair;
        //    spCrosshairGO->GetCrosshairPoint3d(&pt3dCrosshair);
        //    Mcsf::MedViewer3D::Point2D pt2dCrosshair = mpr->GetPlaneCoordinate(pt3dCrosshair, false);
        //    spCrosshairGO->SetCrosshairPoint2d(pt2dCrosshair);
        //    spCrosshairGO->SetDirty(true);
        //}

        //===========================================================================
        auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_MEASURE, section);
        std::string strKey = imageUID + "|" + GOTypeHelper::ToString(goType);
        auto go = mModelWarehouse->GetModelObject(strKey);
        auto measureGO = std::dynamic_pointer_cast<GalleryMeasureGraphicObject>(go);
        if (measureGO != nullptr)
        {
            pt2dStart = mpr->GetPlaneCoordinate(pt3dStart, false);
            pt2dEnd = mpr->GetPlaneCoordinate(pt3dEnd, false);
            Vector2D v2dTranslation = pt2dEnd - pt2dStart;
            measureGO->SetMeasureAnglePan(v2dTranslation.x * 2, v2dTranslation.y * 2);
            measureGO->SetMeasureDistancePan(v2dTranslation.x * 2, v2dTranslation.y * 2);
            measureGO->SetDirty(true);

            measureGO->UpdateMeasureValGO();
        }

        //===========================================================================
        goType = GOTypeHelper::ComposeAsGOType(GALLERY_TEXT, section);
        strKey = imageUID + "|" + GOTypeHelper::ToString(goType);
        go = mModelWarehouse->GetModelObject(strKey);
        auto textGO = std::dynamic_pointer_cast<GalleryTextGraphicObject>(go);
        if (textGO != nullptr)
        {
            pt2dStart = mpr->GetPlaneCoordinate(pt3dStart, false);
            pt2dEnd = mpr->GetPlaneCoordinate(pt3dEnd, false);
            Vector2D v2dTranslation = pt2dEnd - pt2dStart;
            textGO->SetTextPan(v2dTranslation.x * 2, v2dTranslation.y * 2);
            textGO->UpdateTextGO();
            textGO->SetDirty(true);
        }
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"TpsGalleryMPRPanOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }
    return true;
}

TpsGalleryMPRWindowingOperation::TpsGalleryMPRWindowingOperation(float deltaWidth, float deltaCenter)
    :mDeltaWidth(deltaWidth), mDeltaCenter(deltaCenter),
    mVolumeKey(""), mMPRKey(""), mMprCornerInfoKey("") {

}

TpsGalleryMPRWindowingOperation::~TpsGalleryMPRWindowingOperation(){

}

void TpsGalleryMPRWindowingOperation::SetGraphicObjectKeys(const std::string& sVolumeKey, 
    const std::string& sMPRKey,
    const std::string &mprCornerInfoKey,
    const std::vector<std::string>& vecLinkedMprKey, 
    const std::vector<std::string>& vecLinkedMprCornerInfoKey) {
        mVolumeKey = sVolumeKey;
        mMPRKey = sMPRKey;
        mMprCornerInfoKey = mprCornerInfoKey;
        mVecLinkedMprKey = vecLinkedMprKey;
        mVecLinkedMprCornerInfoKey = vecLinkedMprCornerInfoKey;
}

bool TpsGalleryMPRWindowingOperation::ModifyGraphicObject() const{
    try {
        using namespace Mcsf::MedViewer3D;
        auto goVolume = mModelWarehouse->GetGraphicObject(mVolumeKey);
        auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
        auto goMpr = mModelWarehouse->GetGraphicObject(mMPRKey);
        auto mpr = std::dynamic_pointer_cast<ClsGraphicObjectMPR>(goMpr);

        if (mpr == nullptr || volume == nullptr){
            TPS_LOG_DEV_ERROR<<"Volume or Mpr graphic object is null.";
            return false;
        }

        mpr->GetWindowing(mWindowWidth, mWindowCenter);
        mWindowWidth += mDeltaWidth;
        mWindowCenter += mDeltaCenter;
        std::string modality = volume->GetModality();
        if (modality == "CT"){
            mWindowWidth = TPS_CLAMP(mWindowWidth, WINDOW_WIDTH_MIN, WINDOW_WIDTH_MAX_CT);
            mWindowCenter = TPS_CLAMP(mWindowCenter, WINDOW_CENTER_MIN_CT, WINDOW_CENTER_MAX_CT);
        }
        else if (modality == "MR"){
            mWindowWidth = TPS_CLAMP(mWindowWidth, WINDOW_WIDTH_MIN, WINDOW_WIDTH_MAX_MR);
            mWindowCenter = TPS_CLAMP(mWindowCenter, WINDOW_CENTER_MIN_MR, WINDOW_CENTER_MAX_MR);
        }

        Windowing_i(mMPRKey, mMprCornerInfoKey);

        if (mVecLinkedMprKey.size() != mVecLinkedMprCornerInfoKey.size()) return true;
        for (int i = 0; i < mVecLinkedMprKey.size(); ++i){
            Windowing_i(mVecLinkedMprKey[i], mVecLinkedMprCornerInfoKey[i]);
        }
        return true;
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"MPRPageTurnOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }
}

bool TpsGalleryMPRWindowingOperation::Windowing_i(
    const std::string& mprKey, 
    const std::string& mprCornerInfoKey) const {
    using namespace Mcsf::MedViewer3D;

    auto goMpr = mModelWarehouse->GetGraphicObject(mprKey);
    auto goMprCornerInfo = mModelWarehouse->GetGraphicObject(mprCornerInfoKey);
    auto mpr = std::dynamic_pointer_cast<ClsGraphicObjectMPR>(goMpr);
    auto mprCornerInfo = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(goMprCornerInfo);
    if (mpr == nullptr /*|| mprCornerInfo == nullptr*/){
        TPS_LOG_DEV_WARNING<<"Failed to get Mpr go.";
        return false;
    }

    mpr->SetWindowing(mWindowWidth, mWindowCenter);
    mpr->SetDirty(true);

    //std::wstringstream wstr_ww, wstr_wl;
    //wstr_ww << mWindowWidth; 
    //wstr_wl << mWindowCenter;
    //std::vector<std::string> wwwl_key_val;
    //wwwl_key_val.push_back("WWVal");
    //wwwl_key_val.push_back("WLVal");

    //// create new buffer
    //FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
    //Pixel32* pStrBuffer = nullptr;
    //int strWidth = 0, strHeight = 0;

    //for(int i = 0; i < 2; ++i){
    //    if(!pBufGen->GenerateRasterBuffer(i == 0 ? wstr_ww.str() : wstr_wl.str(), &pStrBuffer, strWidth, strHeight)){
    //        TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer.\n";
    //        return false;
    //    }
    //    mprCornerInfo->UpdateTextBlock(wwwl_key_val[i], (unsigned char*)pStrBuffer, strWidth, strHeight);
    //    if(pStrBuffer!=nullptr){
    //        delete[] pStrBuffer;
    //        pStrBuffer = nullptr;
    //    }
    //}

    return true;
}

TPS_END_NAMESPACE
