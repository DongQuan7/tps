////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  wenlong zhao wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_setisocenter_operation.cpp 
/// 
///  \brief class TpsAddPoiOperation
/// 
///  \version 1.0
/// 
///  \date    2015/07/18
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setisocenter_operation.h"


//#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_editsetuppoigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"



TPS_BEGIN_NAMESPACE
TpsEasyPlanSetIsocenterOperation::TpsEasyPlanSetIsocenterOperation(const std::string &planUid,const std::string &poiUid,
    const std::string& currentImageUID, const float& currentIsoInImgX, const float& currentIsoInImgY, 
    const float& previousIsoInImgCoorX, const float& previousIsoInImgCoorY,
    const float& viewerWidth, const float& viewerHeight,
    const int& imageWidth, const int& imageHeight,
    const float& firstImagePlanePixelSpacing, const float& secondImagePlanePixelSpacing
    ) : mPlanUid(planUid), mPoiUid(poiUid), 
    mCurrentImageUID(currentImageUID), mCurrentIsoInPlanImgX(currentIsoInImgX), mCurrentIsoInPlanImgY(currentIsoInImgY),
    mPreviousIsoInPlanImgCoorX(previousIsoInImgCoorX), mPreviousIsoInPlanImgCoorY(previousIsoInImgCoorY),
    mViewerWidth(viewerWidth), mViewerHeight(viewerHeight),
    mImageWidth(imageWidth), mImageHeight(imageHeight), mFirstImagePlanePixelSpacing(firstImagePlanePixelSpacing),
    mSecondImagePlanePixelSpacing(secondImagePlanePixelSpacing){
}

TpsEasyPlanSetIsocenterOperation::~TpsEasyPlanSetIsocenterOperation() {

}

bool TpsEasyPlanSetIsocenterOperation::ModifyGraphicObject() const {

    auto go = mModelWarehouse->GetModelObject(
        mPlanUid + "|" + GOTypeHelper::ToString(GO_TYPE_POI_COLLECTION));
    if(nullptr == go) {
        TPS_LOG_DEV_ERROR<<"failed to get poi go collection ! plan uid:" <<mPlanUid;
        return false;
    }
    auto goCollection = dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(go);
    auto converter = mConverterFactory->GetConverter(GO_TYPE_POI);
    if(nullptr == converter) {
        TPS_LOG_DEV_ERROR<<"failed to get poi go converter!";
        return false;
    }
    GOConverterParameter para;
    para.obj_uid = mPoiUid;
    std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> poi = converter->Convert(para);
    if(nullptr == poi) {
        TPS_LOG_DEV_ERROR<<"failed to convert poi go"<<mPoiUid;
        return false;
    }
    auto poiGo = dynamic_pointer_cast<TpsPoiGraphicObject>(poi);
    
    //// first, move drr image so the new isocenter will be in the center of medviewer
    if(!mCurrentImageUID.empty())
    {
        // current image drr go
        auto drrgo = mModelWarehouse->GetModelObject(mCurrentImageUID + "|" + 
            GOTypeHelper::ToString(GO_TYPE_EASYPLAN_DRR));
        if(nullptr == drrgo){
            TPS_LOG_DEV_ERROR<<"failed to get easyplan drr go ! image uid:"
                <<mCurrentImageUID;
            return false;
        }
        auto currentImageDrrGO = dynamic_pointer_cast<EasyPlanDrrGraphicObejct>(drrgo);
        if(nullptr == currentImageDrrGO){
            TPS_LOG_DEV_ERROR<<"failed to dynamic cast to easyplan drr go in "
                <<"TpsEasyPlanSetPanOperation::ModifyGraphicObject()";
            return false;
        }

        // current image's opposed image drr go.
        std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> opposedDrrGO = nullptr; 
        std::shared_ptr<EasyPlanDrrGraphicObejct> currentImageOpposedDrrGO = nullptr; 

        // judge if this image is plan image or plan opposite image.
        std::string planImageUid = "";
        std::string planOpposedImageUid = "";
        mDataRepository->GetImageDataManager()->GetEasyPlanImageUID(mPlanUid, &planImageUid);
        mDataRepository->GetImageDataManager()->GetEasyPlanOpposedImageUID(mPlanUid, &planOpposedImageUid);
        bool currentImageIsPlanImg = true;
        // if current image is plan image.
        if(planImageUid == mCurrentImageUID) {
            currentImageIsPlanImg = true;
            opposedDrrGO = mModelWarehouse->GetModelObject(planOpposedImageUid + "|" + 
                GOTypeHelper::ToString(GO_TYPE_EASYPLAN_DRR));
        }
        // if current image is plan opposed image.
        else if(planOpposedImageUid == mCurrentImageUID) {
            currentImageIsPlanImg = false;
            opposedDrrGO = mModelWarehouse->GetModelObject(planImageUid + "|" + 
                GOTypeHelper::ToString(GO_TYPE_EASYPLAN_DRR));
        }
        else {
            TPS_LOG_DEV_ERROR<<"the image with uid: " <<mCurrentImageUID << " dose not belong to current plan!";
            return false;
        }
        if(nullptr != opposedDrrGO) {
            currentImageOpposedDrrGO = dynamic_pointer_cast<EasyPlanDrrGraphicObejct>(opposedDrrGO);
        }

        Mcsf::Matrix3f matImageModel2DrrModel(Mcsf::Vector3f(1.0f,0.0f,0.0f),Mcsf::Vector3f (0.0f,1.0f,0.0f),
        Mcsf::Vector3f (0.0f,0.0f,1.0f));
        if (!currentImageDrrGO->GetDrrModel2ImageModel(&matImageModel2DrrModel)) {
            TPS_LOG_DEV_ERROR<<"Failed to  TpsEasyPlanSetIsocenterOperation::ModifyGraphicObject() :currentImageDrrGO->GetDrrModel2ImageModel ";
            return false;
        }
        matImageModel2DrrModel = matImageModel2DrrModel.Inverse();
        Mcsf::Vector3f currentIsoInImgCoor(0.0f,0.0f,0.0f);
        Mcsf::Vector3f currentIsoInDrrCoor(0.0f,0.0f,0.0f);
        Mcsf::Vector3f previousIsoInImgCoor(0.0f,0.0f,0.0f);
        Mcsf::Vector3f previousIsoInDrrCoor(0.0f,0.0f,0.0f);
        if(currentImageIsPlanImg){
            currentIsoInImgCoor = Mcsf::Vector3f(mCurrentIsoInPlanImgX,mCurrentIsoInPlanImgY,1.0f);
            previousIsoInImgCoor = Mcsf::Vector3f(mPreviousIsoInPlanImgCoorX,mPreviousIsoInPlanImgCoorY,1.0f);
        }
        else {
            currentIsoInImgCoor = Mcsf::Vector3f(1.0f - mCurrentIsoInPlanImgX,mCurrentIsoInPlanImgY,1.0f);
            previousIsoInImgCoor = Mcsf::Vector3f(1.0f - mPreviousIsoInPlanImgCoorX,mPreviousIsoInPlanImgCoorY,1.0f);
        }
        
        currentIsoInDrrCoor = matImageModel2DrrModel * currentIsoInImgCoor;
        previousIsoInDrrCoor = matImageModel2DrrModel * previousIsoInImgCoor;
        

        // go to check out tda_rl_igrtdrrzoompanwindowing_operation.cpp
        int iWidth = mViewerWidth, iHeight = mViewerHeight;
        double dScaleRatio = (iWidth >= iHeight) ? double(iWidth) / iHeight : double(iHeight) / iWidth;
        // Case 1: pan|xStart|yStart|xEnd|yEnd
        Mcsf::MedViewer3D::Point2D pt2dDrrModelStart = (iWidth > iHeight) ? 
            Mcsf::MedViewer3D::Point2D(currentIsoInDrrCoor.GetX() * dScaleRatio, currentIsoInDrrCoor.GetY()) :
        Mcsf::MedViewer3D::Point2D(currentIsoInDrrCoor.GetX(), currentIsoInDrrCoor.GetY() * dScaleRatio);
        Mcsf::MedViewer3D::Point2D pt2dDrrModelEnd = (iWidth > iHeight) ? 
            Mcsf::MedViewer3D::Point2D(previousIsoInDrrCoor.GetX() * dScaleRatio, previousIsoInDrrCoor.GetY()) :
        Mcsf::MedViewer3D::Point2D(previousIsoInDrrCoor.GetX(), previousIsoInDrrCoor.GetY() * dScaleRatio);


        float dx = (pt2dDrrModelEnd.x - pt2dDrrModelStart.x); //(pt2dEnd.x*2-1)-(pt2dStart.x*2-1)
        float dy = (pt2dDrrModelEnd.y - pt2dDrrModelStart.y) ; //(pt2dEnd.y*2-1) - (pt2dStart.y*2-1)
        Mcsf::Vector3f col0(1.0,0.0,0.0);
        Mcsf::Vector3f col1(0.0,1.0,0.0);
        Mcsf::Vector3f col2(-dx, -dy, 1.0);
        Mcsf::Vector3f col3(dx, -dy, 1.0); // used for current opposed image drr go.
        Mcsf::Matrix3f newMatrix3f;
        newMatrix3f.SetCol0(col0);
        newMatrix3f.SetCol1(col1);
        newMatrix3f.SetCol2(col2);
        Mcsf::Matrix3f Drr2ImageMat(Mcsf::Vector3f(1.0f,0.0f,0.0f),Mcsf::Vector3f (0.0f,1.0f,0.0f),
            Mcsf::Vector3f (0.0f,0.0f,1.0f));
        if (!currentImageDrrGO->GetDrrModel2ImageModel(&Drr2ImageMat)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetIsocenterOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->GetDrrModel2ImageModel";
            return false;
        }
        Drr2ImageMat.Append(newMatrix3f);
        if(!currentImageDrrGO->SetDrrModel2ImageModel(Drr2ImageMat)) {
            TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetIsocenterOperation::ModifyGraphicObject() : "
                <<"Failed to easyPlanDrrGO->SetDrrModel2ImageModel";
            return false;
        }
        // set isocenter position in image coordinate and save to DRR GO, this position will later be 
        // used on DRR image rotational rendering after table rotation.
        Mcsf::Point3f isocenterPosInImage;
        if(currentImageIsPlanImg) {
            isocenterPosInImage.SetX(poiGo->GetPosition().x);
        }
        else {
            isocenterPosInImage.SetX(1.0f - poiGo->GetPosition().x);
        }
        isocenterPosInImage.SetY(poiGo->GetPosition().y);
        isocenterPosInImage.SetZ(poiGo->GetPosition().z);
        currentImageDrrGO->SetIsocenterPosInImageModel(isocenterPosInImage);
        currentImageDrrGO->SetDirty(true);

        // modify the opposed image drr go.
        if(nullptr != currentImageOpposedDrrGO) {
            Mcsf::Matrix3f newMatrix3fForOpposedImg;
            newMatrix3fForOpposedImg.SetCol0(col0);
            newMatrix3fForOpposedImg.SetCol1(col1);
            newMatrix3fForOpposedImg.SetCol2(col3);

            Mcsf::Matrix3f OpposedDrr2ImageMat(Mcsf::Vector3f(1.0f,0.0f,0.0f),Mcsf::Vector3f (0.0f,1.0f,0.0f),
                Mcsf::Vector3f (0.0f,0.0f,1.0f));
            if (!currentImageOpposedDrrGO->GetDrrModel2ImageModel(&OpposedDrr2ImageMat)) {
                TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetIsocenterOperation::ModifyGraphicObject() : "
                    <<"Failed to easyPlanDrrGO->GetDrrModel2ImageModel";
                return false;
            }
            OpposedDrr2ImageMat.Append(newMatrix3fForOpposedImg);
            if(!currentImageOpposedDrrGO->SetDrrModel2ImageModel(OpposedDrr2ImageMat)) {
                TPS_LOG_DEV_ERROR<<"TpsEasyPlanSetIsocenterOperation::ModifyGraphicObject() : "
                    <<"Failed to easyPlanDrrGO->SetDrrModel2ImageModel";
                return false;
            }
            // set isocenter position in image coordinate and save to DRR GO, this position will later be 
            // used on DRR image rotational rendering after table rotation.
            Mcsf::Point3f isocenterPosInOpposedImage;
            if(currentImageIsPlanImg) {
                isocenterPosInOpposedImage.SetX(1.0f - poiGo->GetPosition().x);
            }
            else {
                isocenterPosInOpposedImage.SetX(poiGo->GetPosition().x);
            }
            //isocenterPosInOpposedImage.SetX( 1.0f -(poiGo->GetPosition().x));
            isocenterPosInOpposedImage.SetY(poiGo->GetPosition().y);
            isocenterPosInOpposedImage.SetZ(poiGo->GetPosition().z);
            currentImageOpposedDrrGO->SetIsocenterPosInImageModel(isocenterPosInOpposedImage);

            currentImageOpposedDrrGO->SetDirty(true);
        }

        poiGo->SetVisible(true);
        // if the new point is an isocenter poi, then delete the previous isocenter poi.
        // I did this because the actual isocenter UID sent here is different everytime, 
        // so based on the fact that every easy plan only have one isocenter, here we just 
        // override the previous isocenter with the new one.
        goCollection->DeletePoi(mPoiUid);
        goCollection->AddPoi(mPoiUid, poiGo);
        goCollection->SetDirty(true);
    }
    return true;
}



TPS_END_NAMESPACE