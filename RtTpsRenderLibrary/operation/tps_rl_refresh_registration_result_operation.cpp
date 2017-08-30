////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_refresh_registration_result_operation.CPP
/// 
///  \brief class RefreshRegistrationResultOperation implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/12/05
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_refresh_registration_result_operation.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"


TPS_BEGIN_NAMESPACE  // begin namespace tps

RefreshRegistrationResultOperation::RefreshRegistrationResultOperation()
{

}

RefreshRegistrationResultOperation::~RefreshRegistrationResultOperation()
{

}

bool RefreshRegistrationResultOperation::ModifyGraphicObject() const{
    std::string key = COMMON_BASE + "|" + GOTypeHelper::ToString(GO_TYPE_VARYING_FUSION_GO);
    std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> go = nullptr;
    go = mModelWarehouse->GetModelObject(key);

    auto converter = mConverterFactory->GetConverter(GO_TYPE_VARYING_FUSION_GO);
    if (converter == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get graphic object converter.";
        return false;
    }

    GOConverterParameter para;
    if (!converter->Update(para, go)){
        TPS_LOG_DEV_ERROR<<"Failed to update fusion graphic object.";
        return false;
    }
    go->SetDirty(true);
    return true;
}


RefreshApprovedRegistrationResultOperation::RefreshApprovedRegistrationResultOperation(
    const std::string& fixedUid, const std::string& floatUid)
    : mFixedUid(fixedUid), mFloatUid(floatUid)
{

}

RefreshApprovedRegistrationResultOperation::~RefreshApprovedRegistrationResultOperation()
{

}

bool RefreshApprovedRegistrationResultOperation::ModifyGraphicObject() const
{
    std::string key = mFixedUid + "|" + mFloatUid + "|" + GOTypeHelper::ToString(GO_TYPE_FUSION);
    std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> go = nullptr;
    go = mModelWarehouse->GetModelObject(key);

    auto converter = mConverterFactory->GetConverter(GO_TYPE_FUSION);
    if (converter == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get graphic object converter.";
        return false;
    }

    GOConverterParameter para;
    para.data_source_map[IMAGE] = mFixedUid;
    para.data_source_map[FLOAT_IMAGE] = mFloatUid;
    if (!converter->Update(para, go)){
        TPS_LOG_DEV_ERROR<<"Failed to update fusion graphic object.";
        return false;
    }
    go->SetDirty(true);
    return true;
}
TPS_END_NAMESPACE  // end namespace tps