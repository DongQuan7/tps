////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_updatedrrdisplayoperation.cpp
/// 
///  \brief UpdateDrrDisplayOperation
/// 
///  \version 1.0
/// 
///  \date    2014/5/20
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_updatedrrdisplayoperation.h"

#include "RtTpsRenderLibrary/tps_rl_drrgraphicobject.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"

TPS_BEGIN_NAMESPACE

UpdateDrrDisplayOperation::UpdateDrrDisplayOperation(std::string imageUid,int windowWidth, 
    int windowCenter, float gray, bool isVisible) :
    mIsVisible(isVisible),
    mDeltaWindowCenter(windowCenter), 
    mDeltaWindowWidth(windowWidth),mGray(gray),
    mImageUid(imageUid)
{

}

UpdateDrrDisplayOperation::~UpdateDrrDisplayOperation()
{

}

bool UpdateDrrDisplayOperation::ModifyGraphicObject() const
{
    auto drrGo = mModelWarehouse->GetModelObject(
        mImageUid + "|" + GOTypeHelper::ToString(GO_TYPE_DRR));
    auto drr = dynamic_pointer_cast<DrrGraphicObejct>(drrGo);
    if (nullptr == drrGo || nullptr == drr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get drr go:" << mImageUid;
        return false;
    }
    double preWindowWidth, preWindowCenter;
    drr->GetWindowing(&preWindowWidth, &preWindowCenter);
    double curWindowWidth = preWindowWidth + mDeltaWindowWidth;
    double curWindowCenter = preWindowCenter + mDeltaWindowCenter;
    drr->SetWindowing(curWindowWidth, curWindowCenter);
    drr->SetDirty(true);
    return true;
}


TPS_END_NAMESPACE