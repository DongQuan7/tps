////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_refreshfusiontypeoperation.cpp
/// 
///  \brief class RefreshFusionType implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/05/29
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_refreshfusiontypeoperation.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_fusiongraphicobject.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

RefreshFusionTypeOperation::RefreshFusionTypeOperation(
    const std::string& fixedImage, const std::string& floatImage, int fusionType)
    : mFixedImageUid(fixedImage)
    , mFloatImageUid(floatImage)
    , mFusionType(fusionType)
    , mIsFusionMode(false)
{

}

RefreshFusionTypeOperation::RefreshFusionTypeOperation(int fusionType)
    : mFusionType(fusionType), mIsFusionMode(true)
{

}

RefreshFusionTypeOperation::~RefreshFusionTypeOperation()
{

}

bool RefreshFusionTypeOperation::ModifyGraphicObject() const
{
    GO_TYPE goType = mIsFusionMode ? GO_TYPE_VARYING_FUSION_GO : GO_TYPE_FUSION;
    std::string key = mIsFusionMode ? COMMON_BASE : mFixedImageUid + "|" + mFloatImageUid;
    key += "|" + GOTypeHelper::ToString(goType);
    std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> go = nullptr;
    std::shared_ptr<FusionGraphicObject> fusionGO = nullptr;
    go = mModelWarehouse->GetModelObject(key);
    fusionGO = std::dynamic_pointer_cast<FusionGraphicObject>(go);
    if (fusionGO == nullptr){
        TPS_LOG_DEV_ERROR<<"Fusion graphic object does not exist, Fixed image uid: "
            <<mFixedImageUid<<" Float image uid: "<<mFloatImageUid;
        return false;
    }

    fusionGO->SetFusionType(FUSION_TYPE(mFusionType));
    fusionGO->SetDirty(true);

    return true;
}

TPS_END_NAMESPACE  // end namespace tps