//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  WANG Zhendong zhendong.wang@united-imaging.com
///
///  \file    tps_rl_bevreset_operation.cpp
///  \brief  
///
///  \version 1.0
///  \date    November 6, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_bevreset_operation.h"

//TPS
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "tps_logger.h"


TPS_BEGIN_NAMESPACE

BEVResetOperation::BEVResetOperation(const std::string& imageUID, int actionType)
: mImageUID(imageUID), mActionType(actionType)
{

}

BEVResetOperation::BEVResetOperation(const std::string& imageUID, const std::string& planUID, int actionType)
: mImageUID(imageUID), mPlanUID(planUID), mActionType(actionType)
{

}


BEVResetOperation::~BEVResetOperation()
{

}


bool BEVResetOperation::ModifyGraphicObject() const
{

    try
    {
        auto go = mModelWarehouse->GetModelObject(
            mPlanUID + "|" + GOTypeHelper::ToString(GO_TYPE_BEV_CAMERA));
        auto camera = dynamic_pointer_cast<CameraGraphicObject>(go);
        if (nullptr == camera)
        {
            TPS_LOG_DEV_ERROR << "The BEV camera does not exist.";
            return false;
        }

        //reset bevzoom
        if (mActionType & 0x01)
        {
            camera->Zoom(1.0);
            camera->SetDirty(true);
        }

        //reset bevpan
        if (mActionType & 0x02)
        {
            camera->SetFrustumOffset(0.0, 0.0);
            camera->SetDirty(true);
        }

        //reset bevwindowing
        if (mActionType & 0x04)
        {

            auto goDrr = mModelWarehouse->GetModelObject(mImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_DRR));
            auto drr = std::dynamic_pointer_cast<DrrGraphicObejct>(goDrr);
            if (drr == nullptr)
            {
                TPS_LOG_DEV_ERROR << "Failed to get drr go:" << mImageUID;
                return false;
            }

            double dWindowWidth(256.0), dWindowCenter(127.0);
            drr->SetWindowing(dWindowWidth, dWindowCenter);
            drr->SetDirty(true);
        }

        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "BEVResetOperation::ModifyGraphicObject:" << ex.what();
        return false;
    }
}

TPS_END_NAMESPACE
