//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  RONG Chengcheng chengcheng.rong@united-imaging.com
///
///  \file    tps_rl_mprzoompanwindowing_operation.cpp
///  \brief   MPR zoom and pan operation implementation
///
///  \version 1.0
///  \date    2014-02-20
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_mprzoompanwindowing_operation.h"

//ZHENGHE
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_suv_calculator.h"

//TPS
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_rotate_operation.h"

TPS_BEGIN_NAMESPACE

MPRZoomOperation::MPRZoomOperation(float fZoomFactor)
    :mZoomFactor(fZoomFactor),
    mZoomInputMode(BY_FACTOR ),
    mWidth(0),
    mHeight(0),
    mStartX(0),
    mStartY(0),
    mStopX(1),
    mStopY(1)
{

}

MPRZoomOperation::MPRZoomOperation(int width, int height, 
    float startX, float startY, float stopX, float stopY) 
    : mWidth(width), mHeight(height), 
    mStartX(startX), mStartY(startY), mStopX(stopX), mStopY(stopY)
{
    mZoomInputMode = BY_VECTOR;
    mZoomFactor = 1;
}

MPRZoomOperation::~MPRZoomOperation()
{

}

void MPRZoomOperation::SetGraphicObjectKeys(const std::string& sVolumeKey, 
    const std::string& sMPRKey, const std::string &mprRulerTextKey)
{
    mVolumeKey = sVolumeKey;
    mMPRKey = sMPRKey;
    mMprRulerTextKey = mprRulerTextKey;
}

bool MPRZoomOperation::ModifyGraphicObject() const
{
    try
    {

        std::vector<std::string> vSplit;
        boost::split(vSplit, mMPRKey, boost::is_any_of("|"));
        if (vSplit.size() != 2)
        {
            TPS_LOG_DEV_ERROR << "The graphic object type name is not right.";
            return false;
        }

        std::string imageUID = vSplit[0];
        GO_TYPE mprGoType = GOTypeHelper::ToGoType(vSplit[1].c_str());
        auto section = GOTypeHelper::GetFirstPostfixComponent(mprGoType);
        ModifyGraphicObject_i(imageUID, section);
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "MPRPageTurnOperation::ModifyGraphicObject:" << ex.what();
        return false;
    }
    return true;
}

bool MPRZoomOperation::ModifyGraphicObject_i(
    const std::string& imageUID, 
    FIRST_POSTFIX_COMPONENT sectionPage,
    SECOND_POSTFIX_COMPONENT location) const
{
    using namespace Mcsf::MedViewer3D;

    auto goVolume = mModelWarehouse->GetGraphicObject(mVolumeKey);
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, sectionPage, location);
    std::string mprKey = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto goMpr = mModelWarehouse->GetGraphicObject(mprKey);


    //down cast
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    auto mpr = std::dynamic_pointer_cast<ClsGraphicObjectMPR>(goMpr);

    if (volume == nullptr || mpr == nullptr)
    {
        TPS_LOG_DEV_WARNING << "Failed to dynamic cast IGraphicObjects to goVoluem\
                           or goMpr.";
        return false;
    }

    //get old factor
    double factor = 0.0f;
    mpr->GetScale(&factor);

    //float zoomBase = factor;
    //calc new factor
    switch (mZoomInputMode)
    {
        case BY_FACTOR:
            factor *= mZoomFactor;
            break;
        case BY_VECTOR:
            {
                //get display size
                //int iWidth = 0, iHeight = 0;
                //mpr->GetDisplaySize(&iWidth, &iHeight);
                float ratio = (float)mWidth / mHeight;

                // handle zoom
                float yVar = ratio > 1.0 ? (mStartY - mStopY) : (mStartY - mStopY) * ratio;
                float zoomFactorDelta = std::exp(yVar);
                factor *= zoomFactorDelta;
                if (factor < 0.1f)
                {
                    factor = 0.1f;
                }
                else if (factor > 8.0f)
                {
                    factor = 8.0f;
                }
                break;
            }
            break;
        default:
            return false;
    }

    mpr->SetScale(factor);
    mpr->SetDirty(true);
    return true;
}

MPRPanOperation::MPRPanOperation(int width, int height, 
    float startx, float starty, float stopx, float stopy)
    :mWidth(width), mHeight(height),
    mStartX(startx), mStartY(starty), mStopX(stopx), mStopY(stopy)
{
}

MPRPanOperation::~MPRPanOperation()
{

}

void MPRPanOperation::SetGraphicObjectKeys(const std::string& sVolumeKey, 
    const std::string& sMPRKey)
{
    mVolumeKey = sVolumeKey;
    mMPRKey = sMPRKey;
}

bool MPRPanOperation::ModifyGraphicObject() const
{
    try
    {
        using namespace Mcsf::MedViewer3D;

        std::vector<std::string> vSplit;
        boost::split(vSplit, mMPRKey, boost::is_any_of("|"));
        if (vSplit.size() != 2)
        {
            TPS_LOG_DEV_ERROR << "The graphic object type name is not right.";
            return nullptr;
        }

        std::string imageUID = vSplit[0];



        auto goVolume = mModelWarehouse->GetGraphicObject(mVolumeKey);
        auto goMpr = mModelWarehouse->GetGraphicObject(mMPRKey);
        //down cast
        auto mpr = std::dynamic_pointer_cast<ClsGraphicObjectMPR>(goMpr);
        if (mpr == nullptr)
        {
            TPS_LOG_DEV_ERROR << "Failed to dynamic cast IGraphicObjects to goMpr.";
            return false;
        }

        // calculation for operation
        // 1. convert translation points from screen cs to MPR cs
        if (mWidth <= 0 || mHeight <= 0)
        {
            //printf("[PanOperation]:(mWidth, mHeight), (%f, %f)\n", mWidth, mHeight);
            return false;
        }
        double dScaleRatio = (mWidth >= mHeight) ? double(mWidth) / mHeight : double(mHeight) / mWidth;

        // Case 1: pan|xStart|yStart|xEnd|yEnd
        Point2D pt2dStart = mWidth >= mHeight ? Point2D((mStartX - 0.5) * dScaleRatio + 0.5, mStartY) :
            Point2D(mStartX, (mStartY - 0.5) * dScaleRatio + 0.5);

        Point2D pt2dEnd = mWidth >= mHeight ? Point2D((mStopX - 0.5) * dScaleRatio + 0.5, mStopY) :
            Point2D(mStopX, (mStopY - 0.5) * dScaleRatio + 0.5);

        // Case 2: recalculate the MPR GO cs
        Point3D pt3dStart = mpr->PlaneCoordinateToWorldCoordiate(pt2dStart.x, pt2dStart.y);
        Point3D pt3dEnd = mpr->PlaneCoordinateToWorldCoordiate(pt2dEnd.x, pt2dEnd.y);
        Vector3D vTraslation = pt3dEnd - pt3dStart;

        mpr->SetPan(vTraslation.x, vTraslation.y, vTraslation.z);
        mpr->SetDirty(true);

    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "MPRPanOperation::ModifyGraphicObject:" << ex.what();
        return false;
    }
    return true;
}

MPRWindowingOperation::MPRWindowingOperation(float deltaWidth, float deltaCenter)
    :mDeltaWidth(deltaWidth), mDeltaCenter(deltaCenter),
    mVolumeKey(""), mMPRKey(""), mCornerInfoKey("")
{

}

MPRWindowingOperation::~MPRWindowingOperation(){

}

void MPRWindowingOperation::SetGraphicObjectKeys(const std::string& sVolumeKey, 
    const std::string& sMPRKey,
    const std::vector<std::string>& vecLinkedMprKey)
{
    mVolumeKey = sVolumeKey;
    mMPRKey = sMPRKey;
    mVecLinkedMprKey = vecLinkedMprKey;
}

bool MPRWindowingOperation::ModifyGraphicObject() const
{
    try
    {
        using namespace Mcsf::MedViewer3D;
        auto goVolume = mModelWarehouse->GetGraphicObject(mVolumeKey);
        auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
        auto goMpr = mModelWarehouse->GetGraphicObject(mMPRKey);
        auto mpr = std::dynamic_pointer_cast<ClsGraphicObjectMPR>(goMpr);

        if (mpr == nullptr || volume == nullptr)
        {
            TPS_LOG_DEV_ERROR << "Volume or Mpr or Corner graphic object is null.";
            return false;
        }

        mpr->GetWindowing(mWindowWidth, mWindowCenter);
        mWindowWidth += mDeltaWidth;
        mWindowCenter += mDeltaCenter;
        std::string modality = volume->GetModality();
        if (modality == "CT")
        {
            mWindowWidth = TPS_CLAMP(mWindowWidth, WINDOW_WIDTH_MIN, WINDOW_WIDTH_MAX_CT);
            mWindowCenter = TPS_CLAMP(mWindowCenter, WINDOW_CENTER_MIN_CT, WINDOW_CENTER_MAX_CT);
        }
        else if (modality == "MR")
        {
            mWindowWidth = TPS_CLAMP(mWindowWidth, WINDOW_WIDTH_MIN, WINDOW_WIDTH_MAX_MR);
            mWindowCenter = TPS_CLAMP(mWindowCenter, WINDOW_CENTER_MIN_MR, WINDOW_CENTER_MAX_MR);
        }
        else if (modality == "PT")
        {
            mWindowWidth = TPS_CLAMP(mWindowWidth, WINDOW_WIDTH_MIN_PT, WINDOW_WIDTH_MAX_PT);
            mWindowCenter = TPS_CLAMP(mWindowCenter, WINDOW_CENTER_MIN_PT, WINDOW_CENTER_MAX_PT);
        }
        mpr->SetWindowing(mWindowWidth, mWindowCenter);
        mpr->SetDirty(true);

        for (int i = 0; i < mVecLinkedMprKey.size(); ++i)
        {
            auto go = mModelWarehouse->GetGraphicObject(mVecLinkedMprKey[i]);
            auto linked_mpr = std::dynamic_pointer_cast<ClsGraphicObjectMPR>(go);
            if (linked_mpr == nullptr) continue;
            linked_mpr->SetWindowing(mWindowWidth, mWindowCenter);
            linked_mpr->SetDirty(true);
        }
        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "MPRPageTurnOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }
}
TPS_END_NAMESPACE
