////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_refreshfusionfactoroperation.cpp
/// 
///  \brief class RefreshFusionFactor implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/05/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_refreshfusionfactoroperation.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_fusiongraphicobject.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

//const std::string aSectionType[3] = {
//    CORONAL_SECTION, SAGITTAL_SECTION, AXIAL_SECTION
//};

RefreshFusionFactorOperation::RefreshFusionFactorOperation(
const std::string& fixedImage, const std::string& floatImage, double fusionFactor)
: mFixedImageUid(fixedImage)
, mFloatImageUid(floatImage)
, mFusionFactor(fusionFactor)
, mIsFusionMode(false)
{

}

RefreshFusionFactorOperation::RefreshFusionFactorOperation(double fusionFactor)
: mFusionFactor(fusionFactor), mIsFusionMode(true)
{

}

RefreshFusionFactorOperation::~RefreshFusionFactorOperation()
{

}

bool RefreshFusionFactorOperation::ModifyGraphicObject() const
{
    GO_TYPE goType = mIsFusionMode ? GO_TYPE_VARYING_FUSION_GO : GO_TYPE_FUSION;
    std::string key = mIsFusionMode ? COMMON_BASE : mFixedImageUid + "|" + mFloatImageUid;
    key += "|" + GOTypeHelper::ToString(goType);
    std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> go = nullptr;
    std::shared_ptr<FusionGraphicObject> fusionGO = nullptr;
    go = mModelWarehouse->GetModelObject(key);
    fusionGO = std::dynamic_pointer_cast<FusionGraphicObject>(go);
    if (fusionGO == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Fusion graphic object does not exist, Fixed image uid: "
            <<mFixedImageUid<<" Float image uid: "<<mFloatImageUid;
        return false;
    }

    fusionGO->SetFixedAlpha(1 - mFusionFactor);
    fusionGO->SetFloatAlpha(mFusionFactor);
    fusionGO->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE  // end namespace tps