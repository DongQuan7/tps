////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong Zhao  wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_setzoom_operation.cpp 
/// 
///  \brief class TpsEasyPlanSetZoomOperation
/// 
///  \version 1.0
/// 
///  \date    2014/11/18
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h "
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setzoom_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgocollection.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

//ZHENGHE
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
TPS_BEGIN_NAMESPACE
TpsEasyPlanSetZoomOperation::TpsEasyPlanSetZoomOperation(const std::string &drrImageUID, const std::string &planUID, const float zoomFactor)
: mDrrImageUID(drrImageUID), mPlanUID(planUID), mZoomFactor(zoomFactor){

}

TpsEasyPlanSetZoomOperation::~TpsEasyPlanSetZoomOperation()
{

}

bool TpsEasyPlanSetZoomOperation::ModifyGraphicObject() const
{
    try{
        using namespace Mcsf::MedViewer3D;

        // find GO
        auto go = mModelWarehouse->GetModelObject(mPlanUID + "|" + 
            GOTypeHelper::ToString(GO_TYPE_RT_IMAGE_COLLECTION));
        if(nullptr == go){
            TPS_LOG_DEV_ERROR<<"failed to get easyplan drr collection go";
            return false;
        }
        std::shared_ptr<EasyPlanDrrGraphicObjectCollection> drrCollectionGO = std::dynamic_pointer_cast<EasyPlanDrrGraphicObjectCollection> (go);

        auto easyPlanDrrGO = drrCollectionGO->GetDrr(mDrrImageUID);
        if(nullptr == easyPlanDrrGO){
            TPS_LOG_DEV_ERROR<<"failed to dynamic cast to easyplan drr go in TpsEasyPlanSetZoomOperation::ModifyGraphicObject()";
            return false;
        }

        //  get old zoom factor
        float factor = 1.0f;
        factor = easyPlanDrrGO->GetZoomFactor();
        factor *= mZoomFactor;
        // should be equal with image render zoom factor
        if(factor <0.2f) {
            factor = 0.2f;
            mZoomFactor = 1.0f;
        }
        else if(factor >8.0f) {
            factor = 8.0f;
            mZoomFactor = 1.0f;
        }
        // set new zoom factor
        easyPlanDrrGO->SetZoomFactor(factor);

        // set new matrix
        Mcsf::Vector3f col0((1/mZoomFactor),0.0f,0.0f);
        Mcsf::Vector3f col1(0.0f,(1/mZoomFactor),0.0f);
        Mcsf::Vector3f col2(0.0f ,0.0f ,1.0f);
        // fixed       
        Mcsf::Matrix3f  newMatrix3f;
        newMatrix3f.SetCol0(col0);
        newMatrix3f.SetCol1(col1);
        newMatrix3f.SetCol2(col2);
        Mcsf::Matrix3f Drr2ImageMat(Mcsf::Vector3f(1.0f,0.0f,0.0f),Mcsf::Vector3f (0.0f,1.0f,0.0f),
            Mcsf::Vector3f (0.0f,0.0f,1.0f));
        Mcsf::Matrix3f MlcMat(Mcsf::Vector3f(1.0f,0.0f,0.0f),Mcsf::Vector3f (0.0f,1.0f,0.0f),
            Mcsf::Vector3f (0.0f,0.0f,1.0f));

        if (!easyPlanDrrGO->GetDrrModel2ImageModel(&Drr2ImageMat)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetZoomOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->GetDrrModel2ImageModel";
            return false;
        }
        Drr2ImageMat.Append(newMatrix3f);

        if (!easyPlanDrrGO->GetMlcTransformMatrix(&MlcMat)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetZoomOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->GetDrrModel2ImageModel";
            return false;
        }
        MlcMat.Append(newMatrix3f);

        if(!easyPlanDrrGO->SetDrrModel2ImageModel(Drr2ImageMat)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetZoomOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->SetDrrModel2ImageModel";
            return false;
        }
        if(!easyPlanDrrGO->SetMlcTransformMatrix(MlcMat)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetZoomOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->SetMlcTransformMatrix";
            return false;
        }

        // 3: set dirty flag
        easyPlanDrrGO->SetDirty(true);
        return true;
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Exception happened in TpsEasyPlanSetZoomOperation::ModifyGraphicObject():"<<ex.what();
        return false;
    }
}

TPS_END_NAMESPACE
