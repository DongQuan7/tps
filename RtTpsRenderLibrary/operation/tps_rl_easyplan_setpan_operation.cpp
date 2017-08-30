////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong Zhao  wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_setpan_operation.cpp 
/// 
///  \brief class TpsEasyPlanSetPanOperation
/// 
///  \version 1.0
/// 
///  \date    2014/11/06
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h "
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setpan_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgocollection.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

//ZHENGHE
//#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
//#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
//#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
TPS_BEGIN_NAMESPACE
TpsEasyPlanSetPanOperation::TpsEasyPlanSetPanOperation(const std::string& planUID, const std::string &drrImageUID, 
const float startX, const float startY, const float stopX, const float stopY, const float viewerWidth, const float viewerHeight)
: mPlanUID(planUID), mDrrImageUID(drrImageUID), mStartX(startX), mStartY(startY), 
    mStopX(stopX), mStopY(stopY), mViewerWidth(viewerWidth), mViewerHeight(viewerHeight){

}

TpsEasyPlanSetPanOperation::~TpsEasyPlanSetPanOperation()
{

}

bool TpsEasyPlanSetPanOperation::ModifyGraphicObject() const
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
                <<"TpsEasyPlanSetPanOperation::ModifyGraphicObject()";
            return false;
        }

        auto poiCollectionGO = mModelWarehouse->GetModelObject(
            mPlanUID + "|" + GOTypeHelper::ToString(GO_TYPE_POI_COLLECTION));
        if(nullptr == poiCollectionGO) {
            TPS_LOG_DEV_ERROR<<"failed to get poi go collection ! plan uid:" <<mPlanUID;
            return false;
        }
        auto poiGoCollection = dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(poiCollectionGO);
        if(nullptr == easyPlanDrrGO){
            TPS_LOG_DEV_ERROR<<"failed to dynamic cast to TpsPoiGraphicObjectCollection in "
                <<"TpsEasyPlanSetPanOperation::ModifyGraphicObject()";
            return false;
        }
        //calculation for operation
        //1. convert translation points from screen cs to DRR cs

        // go to check out tda_rl_igrtdrrzoompanwindowing_operation.cpp
        int iWidth = mViewerWidth, iHeight = mViewerHeight;

        double dScaleRatio = (iWidth >= iHeight) ? double(iWidth) / iHeight : double(iHeight) / iWidth;

        // Case 1: pan|xStart|yStart|xEnd|yEnd
        Point2D pt2dStart = (iWidth > iHeight) ? 
            Point2D((mStartX - 0.5) * dScaleRatio, mStartY - 0.5) :
        Point2D(mStartX - 0.5, (mStartY - 0.5) * dScaleRatio);

        Point2D pt2dEnd = (iWidth > iHeight) ? 
            Point2D((mStopX - 0.5 ) * dScaleRatio, mStopY - 0.5) :
        Point2D(mStopX - 0.5, (mStopY - 0.5) * dScaleRatio);

        Point3D pt3dStart =Point3D(pt2dStart.x, pt2dStart.y, 0.0); 
        Point3D pt3dEnd   =Point3D(pt2dEnd.x, pt2dEnd.y, 0.0);
        //Vector3D vTraslation = (pt3dEnd - pt3dStart)+mOldPan_displacement;

        float dx = (pt2dEnd.x - pt2dStart.x); //(pt2dEnd.x*2-1)-(pt2dStart.x*2-1)
        float dy = (pt2dEnd.y - pt2dStart.y) ; //(pt2dEnd.y*2-1) - (pt2dStart.y*2-1)
		float transformdx = dx * iWidth;
		float transformdy = dy * iHeight;
        Mcsf::Vector3f col0(1.0,0.0,0.0);
        Mcsf::Vector3f col1(0.0,1.0,0.0);
        Mcsf::Vector3f col2(-dx ,-dy ,1.0);
		Mcsf::Vector3f col3(-transformdx ,-transformdy ,1.0);

        Mcsf::Matrix3f newMatrix3f;
		Mcsf::Matrix3f transformMatrix3f;

        newMatrix3f.SetCol0(col0);
        newMatrix3f.SetCol1(col1);
        newMatrix3f.SetCol2(col2);

		transformMatrix3f.SetCol0(col0);
		transformMatrix3f.SetCol1(col1);
		transformMatrix3f.SetCol2(col3);
        Mcsf::Matrix3f Drr2ImageMat(Mcsf::Vector3f(1.0f,0.0f,0.0f),Mcsf::Vector3f (0.0f,1.0f,0.0f),
            Mcsf::Vector3f (0.0f,0.0f,1.0f));
        Mcsf::Matrix3f MlcMat(Mcsf::Vector3f(1.0f,0.0f,0.0f),Mcsf::Vector3f (0.0f,1.0f,0.0f),
            Mcsf::Vector3f (0.0f,0.0f,1.0f));
        if (!easyPlanDrrGO->GetDrrModel2ImageModel(&Drr2ImageMat)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetPanOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->GetDrrModel2ImageModel";
            return false;
        }
        Drr2ImageMat.Append(newMatrix3f);
        if(!easyPlanDrrGO->SetDrrModel2ImageModel(Drr2ImageMat)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetPanOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->SetDrrModel2ImageModel";
            return false;
        }
        if (!easyPlanDrrGO->GetMlcTransformMatrix(&MlcMat)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetPanOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->GetMlcTransformMatrix";
            return false;
        }
        MlcMat.Append(transformMatrix3f);
        if(!easyPlanDrrGO->SetMlcTransformMatrix(MlcMat)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetPanOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->SetMlcTransformMatrix";
            return false;
        }

        // 3: set dirty flag
        easyPlanDrrGO->SetDirty(true);
        return true;
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Exception happened in TpsEasyPlanSetPanOperation::ModifyGraphicObject():"<<ex.what();
        return false;
    }
}

TPS_END_NAMESPACE
