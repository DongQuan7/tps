////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong Zhao  wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_setwindowing_operation.cpp 
/// 
///  \brief class TpsEasyPlanSetWindowingOperation
/// 
///  \version 1.0
/// 
///  \date    2014/2/14
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h "
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setwindowing_operation.h"

TPS_BEGIN_NAMESPACE
TpsEasyPlanSetWindowingOperation::TpsEasyPlanSetWindowingOperation(const std::string& planUID, const std::string &drrImageUID, 
    const int winWidth, const int winCenter )
    : mPlanUID(planUID), mDrrImageUID(drrImageUID), mDeltaWinWidth(winWidth), mDeltaWinCenter(winCenter){
      mCurrentWinWidth = 0;
      mCurrentWinCenter = 0;
}

TpsEasyPlanSetWindowingOperation::~TpsEasyPlanSetWindowingOperation()
{

}

bool TpsEasyPlanSetWindowingOperation::ModifyGraphicObject() const
{
    try{
        auto go = mModelWarehouse->GetModelObject(mPlanUID + "|" + 
            GOTypeHelper::ToString(GO_TYPE_RT_IMAGE_COLLECTION));
        if(nullptr == go){
            TPS_LOG_DEV_ERROR<<"failed to get easyplan drr collection go";
            return false;
        }
        std::shared_ptr<EasyPlanDrrGraphicObjectCollection> drrCollectionGO = std::dynamic_pointer_cast<EasyPlanDrrGraphicObjectCollection> (go);

        auto easyPlanDrrGO = drrCollectionGO->GetDrr(mDrrImageUID);
        if(nullptr == easyPlanDrrGO){
            TPS_LOG_DEV_ERROR<<"failed to dynamic cast to easyplan drr go in TpsEasyPlanSetWindowingOperation::ModifyGraphicObject()";
            return false;
        }
        int currentWinWidth = 0;
        int currentWinCenter = 0;
        easyPlanDrrGO->GetWindowingWinWidthAndWinCenter(&currentWinWidth, &currentWinCenter);
        // for fixing dim 50365, WW值范围为[1，9727] WL值范围为[-1536，8191]
        int amendedWW = currentWinWidth + mDeltaWinWidth;
        int amendedWL = currentWinCenter + mDeltaWinCenter;
        if(amendedWW <= 1) {
            amendedWW = 1;
        }
        else if(amendedWW >= 9727){
            amendedWW = 9727;
        }
        if(amendedWL <= -1536){
            amendedWL = -1536;
        }
        else if(amendedWL >= 8191){
            amendedWL = 8191;
        }
        easyPlanDrrGO->SetWindowingWinWidthAndWinCenter(amendedWW, amendedWL);
        easyPlanDrrGO->GetWindowingWinWidthAndWinCenter(&mCurrentWinWidth, &mCurrentWinCenter);
        easyPlanDrrGO->SetDirty(true);
        return true;
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Exception happened in TpsEasyPlanSetWindowingOperation::ModifyGraphicObject():"<<ex.what();
        return false;
    }
}

void TpsEasyPlanSetWindowingOperation::GetCurrentWinWidthAndWinCenter(int& currentWinWidth, int& currentWinCenter)
{
    currentWinWidth = mCurrentWinWidth;
    currentWinCenter = mCurrentWinCenter;
}

TPS_END_NAMESPACE