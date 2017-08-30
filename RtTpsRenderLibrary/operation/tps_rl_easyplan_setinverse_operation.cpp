////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong Zhao  wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_setinverse_operation.cpp 
/// 
///  \brief class TpsEasyPlanSetInverseOperation
/// 
///  \version 1.0
/// 
///  \date    2014/12/19
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h "
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setinverse_operation.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
//ZHENGHE
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "tps_logger.h"
TPS_BEGIN_NAMESPACE
TpsEasyPlanSetInverseOperation::TpsEasyPlanSetInverseOperation(const std::string& imageUID, const std::string& planUID)
: mImageUID(imageUID), mPlanUID(planUID){

}

TpsEasyPlanSetInverseOperation::~TpsEasyPlanSetInverseOperation() {

}

bool TpsEasyPlanSetInverseOperation::ModifyGraphicObject() const {
    try{
        using namespace Mcsf::MedViewer3D;

        // 1: find GO
        auto go = mModelWarehouse->GetModelObject(mPlanUID + "|" + 
            GOTypeHelper::ToString(GO_TYPE_RT_IMAGE_COLLECTION));
        if(nullptr == go){
            TPS_LOG_DEV_ERROR<<"failed to get easyplan drr collection go";
            return false;
        }
        std::shared_ptr<EasyPlanDrrGraphicObjectCollection> drrCollectionGO = std::dynamic_pointer_cast<EasyPlanDrrGraphicObjectCollection> (go);

        auto easyPlanDrrGO = drrCollectionGO->GetDrr(mImageUID);
        if(nullptr == easyPlanDrrGO){
            TPS_LOG_DEV_ERROR<<"failed to dynamic cast to easyplan drr go in TpsEasyPlanSetInverseOperation::ModifyGraphicObject()";
            return false;
        }

        // 2: set IsInversed flag in GO
        if(easyPlanDrrGO->GetIsInversed()) {
            easyPlanDrrGO->SetIsInversed(false);
        }
        else {
            easyPlanDrrGO->SetIsInversed(true);
        }

        // 3: set dirty flag
        easyPlanDrrGO->SetDirty(true);
        return true;
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Exception happened in TpsEasyPlanSetInverseOperation::ModifyGraphicObject():"<<ex.what();
        return false;
    }
}

TPS_END_NAMESPACE