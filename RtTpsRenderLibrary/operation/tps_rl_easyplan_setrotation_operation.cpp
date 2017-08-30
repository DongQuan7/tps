////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong Zhao  wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_setrotation_operation.cpp 
/// 
///  \brief class TpsEasyPlanSetRotationOperation
/// 
///  \version 1.0
/// 
///  \date    2014/11/26
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Mcsf3DArithmetic/mcsf_3d_matrix3x3.h"
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"
#include "Mcsf3DArithmetic/mcsf_3d_vector3f.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h "
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setrotation_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgocollection.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include <cmath>

//ZHENGHE
//#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
//#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
//#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
//#include "McsfViewer3DModel/mcsf_viewer3d_model.h"

TPS_BEGIN_NAMESPACE
    TpsEasyPlanSetRotationOperation::TpsEasyPlanSetRotationOperation(const std::string &drrImageUID, const std::string &planUID, 
    const float startX, const float startY, const float stopX, const float stopY)
    :mDrrImageUID(drrImageUID), mPlanUID(planUID), mStartX(startX), mStartY(startY), mStopX(stopX), mStopY(stopY){

}

TpsEasyPlanSetRotationOperation::~TpsEasyPlanSetRotationOperation()
{

}

Mcsf::Matrix3f TpsEasyPlanSetRotationOperation::RotateMatrix(Mcsf::Matrix3f matrix) const{
	Mcsf::Matrix3f newMatrix = matrix;

    Mcsf::Point3f ptStart = matrix * Mcsf::Point3f(mStartX, mStartY, 1.0f);
    Mcsf::Point3f ptEnd = matrix * Mcsf::Point3f(mStopX, mStopY, 1.0f);
    Mcsf::Point3f ptCenter = matrix * Mcsf::Point3f(0.0f, 0.0f, 1.0f);
    Mcsf::Vector3f vecStart = ptStart - ptCenter;
    Mcsf::Vector3f vecEnd = ptEnd - ptCenter;
    float dAngle = vecStart.AngleBetween(vecEnd);
    Mcsf::Vector3f normal = vecStart.CrossProduct(vecEnd);
    if (normal.GetZ() > 0) dAngle *= -1;
    if (abs(dAngle)<0.001) return matrix;
    float a = dAngle;
    Mcsf::Vector3f col0(cos(a),sin(a),0.0);
    Mcsf::Vector3f col1(-sin(a),cos(a),0.0);
    Mcsf::Vector3f col2(0.0 ,0.0 ,1.0);
    Mcsf::Matrix3f matrix3f,centerMatrix3f;
    matrix3f.SetCol0(col0);
    matrix3f.SetCol1(col1);
    matrix3f.SetCol2(col2);
    // SetManualFusionWindowSectionType must be first, then we can GetRegistrationMatrix3f;
    //matrix3f = drr->GetRegistrationMatrix3f();

    centerMatrix3f.SetCol0(Mcsf::Vector3f(1.0f, 0.0f, 0.0f));
    centerMatrix3f.SetCol1(Mcsf::Vector3f(0.0f, 1.0f, 0.0f));
    centerMatrix3f.SetCol2(Mcsf::Vector3f(-ptCenter.GetX(), -ptCenter.GetY(), 1.0f));
    newMatrix.Prepend(centerMatrix3f);
    newMatrix.Prepend(matrix3f);
    centerMatrix3f.SetCol2(Mcsf::Vector3f(ptCenter.GetX(), ptCenter.GetY(), 1.0f));
    newMatrix.Prepend(centerMatrix3f);

    return newMatrix;
}

bool TpsEasyPlanSetRotationOperation::ModifyGraphicObject() const
{
    try{
        using namespace Mcsf::MedViewer3D;        

        auto go = mModelWarehouse->GetModelObject(mPlanUID + "|" + 
            GOTypeHelper::ToString(GO_TYPE_RT_IMAGE_COLLECTION));
        if(nullptr == go){
            TPS_LOG_DEV_ERROR<<"failed to get easyplan drr collection go";
            return false;
        }
        std::shared_ptr<EasyPlanDrrGraphicObjectCollection> drrCollectionGO = std::dynamic_pointer_cast<EasyPlanDrrGraphicObjectCollection> (go);

        auto easyPlanDrrGO = drrCollectionGO->GetDrr(mDrrImageUID);
        if(nullptr == easyPlanDrrGO){
            TPS_LOG_DEV_ERROR<<"failed to dynamic cast to easyplan drr go in "
                <<"TpsEasyPlanSetRotationOperation::ModifyGraphicObject()";
            return false;
        }

        // Mcsf::Matrix3f matDrr2Image = drr->GetDrrModel2ImageModel();
        Mcsf::Matrix3f matDrr2Image(Mcsf::Vector3f(1.0f,0.0f,0.0f),Mcsf::Vector3f (0.0f,1.0f,0.0f),
            Mcsf::Vector3f (0.0f,0.0f,1.0f));
        Mcsf::Matrix3f MlcMat(Mcsf::Vector3f(1.0f,0.0f,0.0f),Mcsf::Vector3f (0.0f,1.0f,0.0f),
            Mcsf::Vector3f (0.0f,0.0f,1.0f));
        if (!easyPlanDrrGO->GetDrrModel2ImageModel(&matDrr2Image)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetRotationOperation::ModifyGraphicObject() :"
                <<"Failed to easyPlanDrrGO->GetDrrModel2ImageModel";
            return false;
        }
        if (!easyPlanDrrGO->GetMlcTransformMatrix(&MlcMat)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetRotationOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->GetDrrModel2ImageModel";
            return false;
        }
        Mcsf::Matrix3f newDrr2Image = RotateMatrix(matDrr2Image);
        Mcsf::Matrix3f newMlcMat = RotateMatrix(MlcMat);
        
        if(!easyPlanDrrGO->SetDrrModel2ImageModel(newDrr2Image)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetRotationOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->SetDrrModel2ImageModel";
            return false;
        }
        if(!easyPlanDrrGO->SetMlcTransformMatrix(newMlcMat)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetRotationOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->SetMlcTransformMatrix";
            return false;
        }

        // 3: set dirty flag
        easyPlanDrrGO->SetDirty(true);
        return true;
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Exception happened in TpsEasyPlanSetRotationOperation::ModifyGraphicObject():"<<ex.what();
        return false;
    }
}

TPS_END_NAMESPACE
