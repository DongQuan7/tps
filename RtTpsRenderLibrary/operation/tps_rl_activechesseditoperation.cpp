////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_activechesseditoperation.cpp
/// 
///  \brief class ActiveChessEdit implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/06/03
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_activechesseditoperation.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_fusiongraphicobject.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

ActiveChessEditOperation::ActiveChessEditOperation(bool bActive) : mActive(bActive){

}

ActiveChessEditOperation::~ActiveChessEditOperation(){

}

bool ActiveChessEditOperation::ModifyGraphicObject() const{
    auto go = mModelWarehouse->GetModelObject(mFusionGoKey);
    auto fusionGo = std::dynamic_pointer_cast<FusionGraphicObject>(go);
    if (fusionGo == nullptr){
        TPS_LOG_DEV_ERROR<<"The fusion graphic object does not exist.";
        return false;
    }

    fusionGo->SetCanEditFlag(mActive);
    fusionGo->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE  // end namespace tps