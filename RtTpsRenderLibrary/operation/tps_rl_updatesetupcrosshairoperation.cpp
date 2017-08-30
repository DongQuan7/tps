////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_updatesetupcrosshairoperation.cpp
/// 
///  \brief class TpsUpdateSetUpCrosshairOPeration
/// 
///  \version 1.0
/// 
///  \date    2014/4/10
////////////////////////////////////////////////////////////////


#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_updatesetupcrosshairoperation.h"
#include "RtTpsRenderLibrary/tps_rl_editsetuppoigraphicobject.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"


TPS_BEGIN_NAMESPACE
TpsUpdateSetUpCrosshairOperation::TpsUpdateSetUpCrosshairOperation(const std::string 
 &imageuid, const Mcsf::Point2f &mousePosition, bool isVisible, FIRST_POSTFIX_COMPONENT section) : 
mImageuid(imageuid), mMousePosition(mousePosition),mIsVisible(isVisible),mSectionType(section) {

}

TpsUpdateSetUpCrosshairOperation::~TpsUpdateSetUpCrosshairOperation() {

}

bool TpsUpdateSetUpCrosshairOperation::ModifyGraphicObject() const {

    if(mSectionType != AXIAL && mSectionType != EASYPLAN_IMAGE_SECTION) {
        return true;
    }
    auto setupCrossHairGo = mModelWarehouse->GetModelObject(
        mImageuid + "|" + GOTypeHelper::ToString(GO_TYPE_SETUP_POI));
    if(nullptr == setupCrossHairGo) {
        TPS_LOG_DEV_ERROR<<"Failed to get set up poi"<<mImageuid;
        return false;
    }

    auto setupGo = dynamic_pointer_cast<EditSetupPtGraphicObject>(setupCrossHairGo);
    if(nullptr == setupGo) {
        TPS_LOG_DEV_ERROR<<"Failed to convert set up go!";
        return false;
    }
    setupGo->SetVisible(mIsVisible);
    setupGo->SetPosition(TpsArithmeticConverter::ConvertToPoint2D(mMousePosition));
    setupGo->SetDirty(true);
    return true;

}



TPS_END_NAMESPACE

