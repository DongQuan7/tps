////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong Zhao  wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_resetimage_operation.cpp 
/// 
///  \brief class TpsEasyPlanResetImageOperation
/// 
///  \version 1.0
/// 
///  \date    2014/12/20
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h "
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_resetimage_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgocollection.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
//ZHENGHE
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"
TPS_BEGIN_NAMESPACE
    
TpsEasyPlanResetImageOperation::TpsEasyPlanResetImageOperation( const std::string &drrImageUID, const std::string &planUID, 
    const int& viewerWidth, const int& viewerHeight )
    : mDrrImageUID(drrImageUID), mPlanUID(planUID), mViewerWidth(viewerWidth), mViewerHeight(viewerHeight){

}

TpsEasyPlanResetImageOperation::~TpsEasyPlanResetImageOperation() {

}

bool TpsEasyPlanResetImageOperation::ModifyGraphicObject() const {
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

        auto easyPlanDrrGO = drrCollectionGO->GetDrr(mDrrImageUID);
        if(nullptr == easyPlanDrrGO) {
            TPS_LOG_DEV_ERROR<<"failed to dynamic cast to easyplan drr go in TpsEasyPlanResetImageOperation::ModifyGraphicObject()";
            return false;
        }

        // find graphic go and reset them
        // 2: set initialMatrix to GO
        if(!easyPlanDrrGO->ResetDrrModel2ImageModel()) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanResetImageOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->SetDrrModel2ImageModel";
            return false;
        }
        easyPlanDrrGO->SetZoomFactor(1.0f);

        Mcsf::Matrix3f matImageModel2DrrModel(Mcsf::Vector3f(1.0f,0.0f,0.0f),Mcsf::Vector3f (0.0f,1.0f,0.0f),
            Mcsf::Vector3f (0.0f,0.0f,1.0f));
        if (!easyPlanDrrGO->GetDrrModel2ImageModel(&matImageModel2DrrModel)) {
            TPS_LOG_DEV_ERROR<<"Failed to  TpsEasyPlanResetImageOperation::ModifyGraphicObject() :currentImageDrrGO->GetDrrModel2ImageModel ";
            return false;
        }
        Mcsf::Point3f isoInImgModel = easyPlanDrrGO->GetIsocenterPosInImageModel();
        matImageModel2DrrModel = matImageModel2DrrModel.Inverse();
        Mcsf::Vector3f isoInImgCoor(0.0f,0.0f,0.0f);
        Mcsf::Vector3f isoInDrrCoor(0.0f,0.0f,0.0f);
        isoInImgCoor = Mcsf::Vector3f(isoInImgModel.GetX(),isoInImgModel.GetY(),1.0f);
        isoInDrrCoor = matImageModel2DrrModel * isoInImgCoor;

        easyPlanDrrGO->ResetMlcTransformMatrix();

        // go to check out tda_rl_igrtdrrzoompanwindowing_operation.cpp
        int iWidth = mViewerWidth, iHeight = mViewerHeight;
        double dScaleRatio = (iWidth >= iHeight) ? double(iWidth) / iHeight : double(iHeight) / iWidth;
        // Case 1: pan|xStart|yStart|xEnd|yEnd
        Mcsf::MedViewer3D::Point2D pt2dDrrModelStart = (iWidth > iHeight) ? 
            Mcsf::MedViewer3D::Point2D(isoInDrrCoor.GetX() * dScaleRatio, isoInDrrCoor.GetY()) :
        Mcsf::MedViewer3D::Point2D(isoInDrrCoor.GetX(), isoInDrrCoor.GetY() * dScaleRatio);
        Mcsf::MedViewer3D::Point2D pt2dDrrModelEnd = (iWidth > iHeight) ? 
            Mcsf::MedViewer3D::Point2D(0, 0) :
        Mcsf::MedViewer3D::Point2D(0, 0);


        float dx = (pt2dDrrModelEnd.x - pt2dDrrModelStart.x); //(pt2dEnd.x*2-1)-(pt2dStart.x*2-1)
        float dy = (pt2dDrrModelEnd.y - pt2dDrrModelStart.y) ; //(pt2dEnd.y*2-1) - (pt2dStart.y*2-1)
        Mcsf::Vector3f col0(1.0,0.0,0.0);
        Mcsf::Vector3f col1(0.0,1.0,0.0);
        Mcsf::Vector3f col2(-dx, -dy, 1.0);
        Mcsf::Matrix3f newMatrix3f;
        newMatrix3f.SetCol0(col0);
        newMatrix3f.SetCol1(col1);
        newMatrix3f.SetCol2(col2);
        Mcsf::Matrix3f Drr2ImageMat(Mcsf::Vector3f(1.0f,0.0f,0.0f),Mcsf::Vector3f (0.0f,1.0f,0.0f),
            Mcsf::Vector3f (0.0f,0.0f,1.0f));
        if (!easyPlanDrrGO->GetDrrModel2ImageModel(&Drr2ImageMat)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanResetImageOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->GetDrrModel2ImageModel";
            return false;
        }
        Drr2ImageMat.Append(newMatrix3f);
        if(!easyPlanDrrGO->SetDrrModel2ImageModel(Drr2ImageMat)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanResetImageOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->SetDrrModel2ImageModel";
            return false;
        }

        // 3: set dirty flag
        easyPlanDrrGO->SetDirty(true);
        return true;
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Exception happened in TpsEasyPlanResetImageOperation::ModifyGraphicObject():"<<ex.what();
        return false;
    }
}

TPS_END_NAMESPACE
