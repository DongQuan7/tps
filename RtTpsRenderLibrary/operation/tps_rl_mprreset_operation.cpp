//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  SHI CHUNHUI chunhui.shi@united-imaging.com
///
///  \file    tps_rl_mprreset_operation.cpp
///  \brief  
///
///  \version 1.0
///  \date    May 12, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_mprreset_operation.h"

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
#include "tps_logger.h"

#define WINDOW_WIDTH_MIN 1
#define WINDOW_WIDTH_MAX_CT 9727
#define WINDOW_CENTER_MIN_CT -1536
#define WINDOW_CENTER_MAX_CT 8191
#define WINDOW_WIDTH_MAX_MR 4096
#define WINDOW_CENTER_MIN_MR 0
#define WINDOW_CENTER_MAX_MR 4095

TPS_BEGIN_NAMESPACE

MPRResetOperation::MPRResetOperation(const std::string& imageUID) : mImageUID(imageUID)
{

}

MPRResetOperation::MPRResetOperation(const std::string& imageUID, int actionType)
    : mImageUID(imageUID), mActionType(actionType)
{

}


MPRResetOperation::~MPRResetOperation()
{

}

void MPRResetOperation::SetGraphicObjectKeys(const std::string& volKey, 
    const std::string& mprKey, const std::string& crossHairKey, const std::string &rulerKey)
{
    mVolumeKey = volKey;
    mMprKey = mprKey;
    mCrossHairKey = crossHairKey;
    mMprRulerTextKey = rulerKey;
}

void MPRResetOperation::SetTwoOtherKeys(const std::vector<std::string>& twoMprKeys)
{
    mTwoOtherMprKeys = twoMprKeys;
}


bool MPRResetOperation::ModifyGraphicObject() const
{
    try
    {
        auto goVolume = mModelWarehouse->GetGraphicObject(mVolumeKey);
        auto goMpr = mModelWarehouse->GetGraphicObject(mMprKey);
        using namespace Mcsf::MedViewer3D;
        auto volume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(goVolume);
        auto mpr = std::dynamic_pointer_cast<Mcsf::MedViewer3D::ClsGraphicObjectMPR>(goMpr);
        if (volume == nullptr || mpr == nullptr)
        {
            TPS_LOG_DEV_WARNING << "Failed to dynamic cast IGraphicObjects to goVolume or goMpr.";
            return false;
        }

        // reset zoom
        if (mActionType & 0x01)
        {
            double factor = 0.0f;
            mpr->GetScale(&factor);
            mpr->SetScale(1.f);
        }

        // reset pan
        if (mActionType & 0x02)
        {
            Point3D pt3dPlaneCenter = mpr->PlaneCoordinateToWorldCoordiate(0.5, 0.5);
            Vector3D v3dTranslate = Point3D(0.0, 0.0, 0.0) - pt3dPlaneCenter;

            // get the normal vector of the plane
            Vector3D vNormal;
            if (!mpr->GetNormal(&vNormal))
            {
                TPS_LOG_TRACE_ERROR << "Cannot get normal.";
                return false;
            }
            // the distance from the center of volume  to the plane.
            double dDistancePoint2Plane = v3dTranslate.DotProduct(vNormal);
            Vector3D v3dTranslateInPlane = v3dTranslate - dDistancePoint2Plane * vNormal;
            mpr->SetPan(-v3dTranslateInPlane.x, -v3dTranslateInPlane.y, -v3dTranslateInPlane.z);
        }

        // reset windowing
        if (mActionType & 0x04)
        {
            double ww, wc;
            volume->GetDefaultWindowing(&ww, &wc);
            mpr->SetWindowing(ww, wc);

            for (int i = 0; i < mTwoOtherMprKeys.size(); ++i)
            {
                auto tmpgo = mModelWarehouse->GetGraphicObject(mTwoOtherMprKeys[i]);
                auto mpr_go = std::dynamic_pointer_cast<ClsGraphicObjectMPR>(tmpgo);
                if (mpr_go == nullptr)
                {
                    continue;
                }
                mpr_go->SetWindowing(ww, wc);
            }
        }
        else
        {
            mpr->SetDirty(true);
            return false;
        }
        mpr->SetDirty(true);

        return true;

    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "MPRPageTurnOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }
}

TPS_END_NAMESPACE
