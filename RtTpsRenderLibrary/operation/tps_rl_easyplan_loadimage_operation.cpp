////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong Zhao  wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_resetimage_operation.cpp 
/// 
///  \brief class TpsEasyPlanLoadImageOperation
/// 
///  \version 1.0
/// 
///  \date    2014/12/20
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

#include "RtTpsRenderLibrary/tps_rl_easyplan_loadimage_operation.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h "
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"

//ZHENGHE
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"

TPS_BEGIN_NAMESPACE

    TpsEasyPlanLoadImageOperation::TpsEasyPlanLoadImageOperation( const std::string &planUID, 
    const int& viewerWidth, const int& viewerHeight )
    : mPlanUID(planUID), mViewerWidth(viewerWidth), mViewerHeight(viewerHeight){

}

TpsEasyPlanLoadImageOperation::~TpsEasyPlanLoadImageOperation() {

}

bool TpsEasyPlanLoadImageOperation::ModifyGraphicObject() const {
    try{
        using namespace Mcsf::MedViewer3D;

        auto go = mModelWarehouse->GetModelObject(mPlanUID + "|" 
            + GOTypeHelper::ToString(GO_TYPE_RT_IMAGE_COLLECTION));
        if(nullptr == go) {
            TPS_LOG_DEV_ERROR<<"failed to get easyplan drr go collection ! plan uid:"
                <<mPlanUID;
            return nullptr;
        }
        auto drrCollection = std::dynamic_pointer_cast<EasyPlanDrrGraphicObjectCollection>(go);

        std::vector<RtBeam*> beamList = mDataRepository->GetBeamManager()->GetBeamsByPlan(mPlanUID);
        std::vector<RtBeam*>::iterator itBeam = beamList.begin();
        for(; itBeam != beamList.end(); itBeam++)
        {
            RtBeam* beam = *itBeam;

            TpsImage2DEntity* image2DEntity = nullptr;
            bool iRet = mDataRepository->GetImageDataManager()->Get2DImageDataByBeamUID(beam->get_uid(), image2DEntity);
            //load iamge operation中取不到drr是不正常的情况
            if (!iRet)
            {
                TPS_LOG_DEV_ERROR<<"Failed get image2D from ImageDataManager in load image operation.";
                return false;
            }

            std::string imageUID = "";
            if (!image2DEntity->GetUID(imageUID))
            {
                TPS_LOG_DEV_ERROR<<"Failed get image2D uid.";
                return false;
            }

            auto converter = mConverterFactory->GetConverter(GO_TYPE_EASYPLAN_DRR);
            if (converter == nullptr)
            {
                TPS_LOG_DEV_ERROR<<"Failed get easyPlanDrr graphic object converter.";
                return false;
            }
            GOConverterParameter para;
            para.data_source_map[IMAGE] = imageUID;
            std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> easyPlanDrrGO = converter->Convert(para);
            if (easyPlanDrrGO == nullptr){
                TPS_LOG_DEV_ERROR<<"Failed convert easyPlanDrr graphic object.";
                return false;
            }
            std::shared_ptr<EasyPlanDrrGraphicObejct> drrGO = dynamic_pointer_cast<EasyPlanDrrGraphicObejct>(easyPlanDrrGO);

            if(nullptr == drrGO) {
                TPS_LOG_DEV_ERROR<<"failed to dynamic cast to easyplan drr go in TpsEasyPlanLoadImageOperation::ModifyGraphicObject()";
                return nullptr;
            }

            // find graphic go and reset them
            // 2: set initialMatrix to GO
            if(!drrGO->ResetDrrModel2ImageModel()) {
                TPS_LOG_DEV_ERROR<<"TpsEasyPlanLoadImageOperation::ModifyGraphicObject() : "
                    <<"Failed to drrGO->SetDrrModel2ImageModel";
                return nullptr;
            }
            drrGO->SetZoomFactor(1.0f);

            Mcsf::Matrix3f matImageModel2DrrModel(Mcsf::Vector3f(1.0f,0.0f,0.0f),Mcsf::Vector3f (0.0f,1.0f,0.0f),
                Mcsf::Vector3f (0.0f,0.0f,1.0f));
            if (!drrGO->GetDrrModel2ImageModel(&matImageModel2DrrModel)) {
                TPS_LOG_DEV_ERROR<<"Failed to  TpsEasyPlanLoadImageOperation::ModifyGraphicObject() :currentImageDrrGO->GetDrrModel2ImageModel ";
                return nullptr;
            }
            Mcsf::Point3f isoInImgModel = drrGO->GetIsocenterPosInImageModel();
            matImageModel2DrrModel = matImageModel2DrrModel.Inverse();
            Mcsf::Vector3f isoInImgCoor(0.0f,0.0f,0.0f);
            Mcsf::Vector3f isoInDrrCoor(0.0f,0.0f,0.0f);
            isoInImgCoor = Mcsf::Vector3f(isoInImgModel.GetX(),isoInImgModel.GetY(),1.0f);
            isoInDrrCoor = matImageModel2DrrModel * isoInImgCoor;

            drrGO->ResetMlcTransformMatrix();

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
            if (!drrGO->GetDrrModel2ImageModel(&Drr2ImageMat)) {
                TPS_LOG_DEV_ERROR<<"TpsEasyPlanLoadImageOperation::ModifyGraphicObject() : "
                    <<"Failed to drrGO->GetDrrModel2ImageModel";
                return nullptr;
            }
            Drr2ImageMat.Append(newMatrix3f);
            if(!drrGO->SetDrrModel2ImageModel(Drr2ImageMat)) {
                TPS_LOG_DEV_ERROR<<"TpsEasyPlanLoadImageOperation::ModifyGraphicObject() : "
                    <<"Failed to drrGO->SetDrrModel2ImageModel";
                return nullptr;
            }

            drrGO->SetBeamUID(beam->get_uid());
            drrCollection->AddDrr(imageUID, drrGO);
        }

        // 3: set dirty flag
        drrCollection->SetDirty(true);
        return true;
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Exception happened in TpsEasyPlanLoadImageOperation::ModifyGraphicObject():"<<ex.what();
        return false;
    }
}

TPS_END_NAMESPACE
