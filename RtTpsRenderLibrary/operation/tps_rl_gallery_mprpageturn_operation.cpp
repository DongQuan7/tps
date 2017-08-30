//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rl_gallery_mprpageturn_operation.cpp
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_gallery_mprpageturn_operation.h"

// TPS
#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"          // CrosshairGraphicObject
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measuregraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_textgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"

// ZHENGHE
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"

#include "boost/algorithm/string/split.hpp"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

TpsGalleryMPRPageTurnOperation::TpsGalleryMPRPageTurnOperation(
    int turnStep, const std::string& strImageUID, 
    FIRST_POSTFIX_COMPONENT ucSectionType, 
    //SECOND_POSTFIX_COMPONENT ucSectionExtendType, 
    bool bIsCyclic) : 
    mTurnStep(turnStep),
    mImageUID(strImageUID),
    mSectionType(ucSectionType),
    //mSectionExtendType(ucSectionExtendType),
    mIsCyclic(bIsCyclic)
{

}

TpsGalleryMPRPageTurnOperation::~TpsGalleryMPRPageTurnOperation()
{

}

bool TpsGalleryMPRPageTurnOperation::ModifyGraphicObject() const 
{
    try
    {
        auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, mSectionType);
        std::string strKey = mImageUID + "|" + GOTypeHelper::ToString(goType);
        auto go = mModelWarehouse->GetModelObject(strKey);
        auto spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
        if (spMprGO == nullptr)
        {
            TPS_LOG_DEV_ERROR << "[ERROR][TpsGalleryMPRPageTurnOperation][ModifyGraphicObject]: "
                << "Failed to dynamic cast IGraphicObjects.";
            return false;
        }

        strKey = mImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
        go = mModelWarehouse->GetModelObject(strKey);
        auto spVolumeGO = std::dynamic_pointer_cast<GraphicObjVolume>(go);
        if (spVolumeGO == nullptr)
        {
            TPS_LOG_DEV_ERROR << "[ERROR][TpsGalleryMPRPageTurnOperation][ModifyGraphicObject]: "
                << "Failed to dynamic cast IGraphicObjects.";
            return false;
        }

        auto matP2W = spVolumeGO->GetModel2PatientMat().Inverse();
        auto matM2W = spVolumeGO->GetModel2World();
        matP2W.Prepend(matM2W);

        Vector3D vStepLen;
        spMprGO->GetNormal(&vStepLen);
        vStepLen.Normalize();
        //计算出翻转1一页需要的偏移距离
        double stepLen;
        TpsMprHelper::CalcSpacingInWorld(spVolumeGO, spMprGO, stepLen);
        //翻页正方向为视角方向，即面法向的反方向
        vStepLen *= -1 * stepLen;

        int currentSlice, totalSlice;
        TpsMprHelper::GetSliceNumberInViewOrder(spVolumeGO, spMprGO, currentSlice, totalSlice);
        int targetSlice = currentSlice + mTurnStep;
        if (mIsCyclic)
        {
            targetSlice = (targetSlice - 1) % totalSlice + 1;
            if (targetSlice <= 0) targetSlice += totalSlice;
        }
        else
        {
            targetSlice = TPS_CLAMP(targetSlice, 1, totalSlice);
        }
        int arctualStep = targetSlice - currentSlice;
        spMprGO->Translate(arctualStep * vStepLen);
        spMprGO->SetDirty(true);
    }
    catch(std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "TpsGalleryMPRPageTurnOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }

    return true;
}

//    TpsGalleryMPRPageTurnOperation::TpsGalleryMPRPageTurnOperation(int turnStep)
//    : mTurnStep(turnStep){
//}
//
//TpsGalleryMPRPageTurnOperation::~TpsGalleryMPRPageTurnOperation(){
//
//}
//
//void TpsGalleryMPRPageTurnOperation::SetGraphicObjectKeys(const std::string& sVolumeKey, 
//    const std::string& sMPRKey, const std::string &mprCornerInfoKey,
//    const std::string& sMeasureKey, const std::string& sTextKey) {
//        mVolumeKey = sVolumeKey;
//        mMPRKey = sMPRKey;
//        mMprCornerInfoKey = mprCornerInfoKey;
//        mMeasureKey = sMeasureKey;
//        mTextKey = sTextKey;
//}
//
//bool TpsGalleryMPRPageTurnOperation::ModifyGraphicObject() const {
//
//    try{
//        //TEST_PERFORMANCE_INIT
//        //TEST_PERFORMANCE_BEGIN
//        //TODO: clear MPRRenderCache, will be moved to a separate operation
//        //auto goCache = mModelWarehouse->GetModelObject(mMPRCacheKey);
//        //auto cache = std::dynamic_pointer_cast<TpsVOIRenderingCacheGO>(goCache);
//        //if (goCache != nullptr || cache != nullptr){
//        //    cache->Clear();
//        //    cache->SetDirty(false);
//        //}
//        using namespace Mcsf::MedViewer3D;
//
//        std::vector<std::string> vSplit;
//        boost::split(vSplit, mMPRKey, boost::is_any_of("|"));
//        if (vSplit.size() != 2){
//            TPS_LOG_DEV_ERROR<<"The graphic object type name is not right.";
//            return nullptr;
//        }
//
//        std::string imageUID = vSplit[0];
//        GO_TYPE mprGoType = GOTypeHelper::ToGoType(vSplit[1].c_str());
//        auto section = GOTypeHelper::GetFirstPostfixComponent(mprGoType);
//
//        auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, section);
//        std::string strKey = imageUID + "|" + GOTypeHelper::ToString(goType);
//        auto go = mModelWarehouse->GetModelObject(strKey);
//        auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
//        if (spCrosshairGO == nullptr){
//            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryMPRPageTurnOperation][ModifyGraphicObject]: "
//                <<"Failed to dynamic cast IGraphicObjects.";
//            return false;
//        }
//
//        auto goVolume = mModelWarehouse->GetGraphicObject(mVolumeKey);
//        auto goMpr = mModelWarehouse->GetGraphicObject(mMPRKey);
//        //auto goMprCornerInfo = mModelWarehouse->GetGraphicObject(mMprCornerInfoKey);
//        auto goMeasure = mModelWarehouse->GetGraphicObject(mMeasureKey);
//        auto goText = mModelWarehouse->GetGraphicObject(mTextKey);
//
//        if(goVolume == nullptr || goMpr == nullptr /*|| goMprCornerInfo == nullptr*/ ||
//           goMeasure == nullptr || goText == nullptr){
//            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryMPRPageTurnOperation][ModifyGraphicObject]: "
//                <<"Failed to get Graphic Object.";
//            return false;
//        }
//
//        auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
//        auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(goMpr);
//        /*auto mprCornerInfo = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(goMprCornerInfo);*/
//        auto measure = std::dynamic_pointer_cast<GalleryMeasureGraphicObject>(goMeasure);
//        auto text = std::dynamic_pointer_cast<GalleryTextGraphicObject>(goText);
//
//        //down cast
//        if (volume == nullptr || mpr == nullptr || /*mprCornerInfo == nullptr ||*/
//            measure == nullptr || text == nullptr){
//            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryMPRPageTurnOperation][ModifyGraphicObject]: "
//                <<"Failed to dynamic cast IGraphicObjects.";
//            return false;
//        }
//
//        auto matP2W = volume->GetModel2PatientMat().Inverse();
//        auto matM2W = volume->GetModel2World();
//        matP2W.Prepend(matM2W);
//
//        Vector3D vStepLen;
//        mpr->GetNormal(&vStepLen);
//        vStepLen.Normalize();
//        double stepLen;
//        TpsMprHelper::CalcSpacingInWorld(volume, mpr, stepLen);
//        vStepLen *= -1 * stepLen;
//
//        Point3D vertices[4], vertices_prev[4], vertices_post[4];
//        Point3D verticesM[4];
//        size_t pDim[3];
//        volume->GetDimension(pDim);
//        mpr->GetVertices(vertices);
//
//        int turnStep = mTurnStep;
//
//        for (int j=0; j<4; ++j){
//            vertices[j] = vertices[j] + turnStep * vStepLen;
//        }
//
//        size_t dim[3];
//        volume->GetDimension(dim);
//        //double volumeRangeInModel[6] = { 
//        //    -0.5 / dim[0], 1 - 0.5 / dim[0],
//        //    -0.5 / dim[1], 1 - 0.5 / dim[1],
//        //    -0.5 / dim[2], 1 - 0.5 / dim[2]
//        //};
//
//        Vector3D vecX = 0.5 * (vertices[2] - vertices[3]);
//        Vector3D vecY = 0.5 * (vertices[0] - vertices[3]);
//        Point3D ptCenter = vertices[3] + vecX + vecY;
//        Point3D ptCenterInModel = matM2W.Inverse().Transform(ptCenter);
//
//        static int out_of_slice = 0;
//        static bool flag_OutOfRange = false;
//        if (ptCenterInModel.x < 0.f|| ptCenterInModel.y < 0.f || ptCenterInModel.z < 0.f || 
//            ptCenterInModel.x > 1.f || ptCenterInModel.y > 1.f || ptCenterInModel.z > 1.f){
//            TPS_LOG_DEV_WARNING<<"MPRPageTurnOperation::ModifyGraphicObject out of the range!";
//            ++out_of_slice;
//            //if(out_of_slice > 1){
//            //    out_of_slice = 2;
//            //}
//            flag_OutOfRange = true;
//        }
//
//        //==============================================================================================
//        // find the linked mpr to calc the new crosshair center
//        //==============================================================================================
//        FIRST_POSTFIX_COMPONENT strLinkedMpr[2];
//        if (section == AXIAL) {
//            strLinkedMpr[0] = CORONAL;
//            strLinkedMpr[1] = SAGITTAL;
//        }
//        else if(section == SAGITTAL) {
//            strLinkedMpr[0] = CORONAL;
//            strLinkedMpr[1] = AXIAL;
//        }
//        else if(section == CORONAL) {
//            strLinkedMpr[0] = AXIAL;
//            strLinkedMpr[1] = SAGITTAL;
//        }
//        std::shared_ptr<MPRGraphicObject> spLinkedMprGO[2];
//        std::shared_ptr<CrosshairGraphicObject> spLinkedCrosshairGO[2];
//        Mcsf::MedViewer3D::Vector3D vecLinkedNormal[2];
//        bool needDisplayCrosshair = true;
//        for (int i = 0; i < 2; i++) {
//            goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, strLinkedMpr[i]);
//            strKey = imageUID + "|" + GOTypeHelper::ToString(goType);
//            go = mModelWarehouse->GetModelObject(strKey);
//            spLinkedCrosshairGO[i] = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
//            if (spLinkedCrosshairGO[i] == nullptr){
//                TPS_LOG_DEV_ERROR<<"[ERROR][CrosshairMovementOperation][ModifyGraphicObject]: "
//                    <<"Failed to dynamic cast IGraphicObjects.";
//                needDisplayCrosshair = false;
//                break;
//                //return false;
//            }
//
//            goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, strLinkedMpr[i]);
//            strKey = imageUID + "|" + GOTypeHelper::ToString(goType);
//            go = mModelWarehouse->GetModelObject(strKey);
//            spLinkedMprGO[i] = std::dynamic_pointer_cast<MPRGraphicObject>(go);
//            if (spLinkedMprGO[i] == nullptr){
//                TPS_LOG_DEV_ERROR<<"[ERROR][CrosshairMovementOperation][ModifyGraphicObject]: "
//                    <<"Failed to dynamic cast IGraphicObjects.";
//                needDisplayCrosshair = false;
//                break;
//                //return false;
//            }
//            spLinkedMprGO[i]->GetNormal(&vecLinkedNormal[i]);
//        }
//
//        if (needDisplayCrosshair && !flag_OutOfRange){
//            // Update crosshair in THIS mpr
//            Mcsf::MedViewer3D::Point3D pt3dCrosshair;
//            spCrosshairGO->GetCrosshairPoint3d(&pt3dCrosshair);
//            Mcsf::MedViewer3D::Vector3D vecIntersectLine = vecLinkedNormal[0].CrossProduct(vecLinkedNormal[1]);
//            Mcsf::MedViewer3D::Vector3D vecPlane = turnStep * vStepLen;
//            double dLen = vecPlane.Magnitude() * vecPlane.Magnitude() / (vecPlane.DotProduct(vecIntersectLine));
//            Mcsf::MedViewer3D::Vector3D vecCrosshair = dLen * vecIntersectLine;
//            pt3dCrosshair += vecCrosshair;
//            spCrosshairGO->SetCrosshairPoint3d(pt3dCrosshair);
//            Mcsf::MedViewer3D::Point2D pt2dCrosshair = mpr->GetPlaneCoordinate(pt3dCrosshair, false);
//            spCrosshairGO->SetCrosshairPoint2d(pt2dCrosshair);
//            spCrosshairGO->SetDirty(true);
//
//            // Linked crosshair, update data and UI
//            for (int i = 0; i < 2; i++) {
//                spLinkedCrosshairGO[i]->SetCrosshairPoint3d(pt3dCrosshair);
//                pt2dCrosshair = spLinkedMprGO[i]->GetPlaneCoordinate(pt3dCrosshair, false);
//                spLinkedCrosshairGO[i]->SetCrosshairPoint2d(pt2dCrosshair);
//                spLinkedCrosshairGO[i]->SetDirty(true);
//            }
//        }
//        //====================================================================================================
//
//        //const int ckeynum=3;
//        FIRST_POSTFIX_COMPONENT section_info[3] = {
//            CORONAL, SAGITTAL, AXIAL,
//        };
//
//        //edit by dyx
//        //std::string mpr_slicepos_key_val[ckeynum] = {
//        //    SLICEPOSITION_CORONAL_SECTION_VAL, 
//        //    SLICEPOSITION_SAGITTAL_SECTION_VAL, 
//        //    SLICEPOSITION_AXIAL_SECTION_VAL};
//        //    std::string mpr_slicetotal_key_val[ckeynum] = {
//        //        SLICEPOSITION_TOTAL_CORONAL_SECTION_VAL, 
//        //        SLICEPOSITION_TOTAL_SAGITTAL_SECTION_VAL, 
//        //        SLICEPOSITION_TOTAL_AXIAL_SECTION_VAL};
//        //        std::string mpr_thickness_key_val[ckeynum] = {
//        //            THICKNESS_CORONAL_SECTION_VAL, 
//        //            THICKNESS_SAGITTAL_SECTION_VAL, 
//        //            THICKNESS_AXIAL_SECTION_VAL};
//
//        int k =0;
//        for(; k<3; ++k){
//            if(section_info[k] == section)
//                break;
//        }
//
//        if(k == 3) {
//            TPS_LOG_DEV_WARNING<<"Cannot find the section!";
//            return false;
//        }
//
//        Mcsf::MedViewer3D::Point3D pt3DInPatient = volume->GetWorld2Patient().Transform(vertices[0]);
//        double posInPatient = 0.0;
//        std::string posKey;
//        if(section == SAGITTAL){
//            posKey = "PositionXVal";
//            posInPatient = pt3DInPatient.x;
//        }else if(section == CORONAL){
//            posKey = "PositionYVal";
//            posInPatient = pt3DInPatient.y;
//        }else if(section == AXIAL){
//            posKey = "PositionZVal";
//            posInPatient = pt3DInPatient.z;
//        }
//
//        if(!flag_OutOfRange){
//            out_of_slice = 0;
//            MPRAPI::CalculateMPRMatrix(mpr, vertices);
//            //edit by dyx
//            //double slicepos, thickness;
//            //int slicetotal;
//            //TpsMprHelper::GetSliceInfo(mpr.get(), volume.get(), &slicepos, &slicetotal, &thickness);
//            //ConvertSlicePosAndThicknessToImageText_i(mpr_slicepos_key_val[k], mpr_slicetotal_key_val[k],
//            //    mpr_thickness_key_val[k], slicepos, slicetotal, thickness, mprCornerInfo);
//
//            //UpdatePosInPatientImageText_i(posKey, posInPatient, mprCornerInfo);
//
//            //Update the isOnPlaneResultList in MeasureGO
//            std::vector<std::list<Mcsf::MedViewer3D::Point3D>> anglePoints3DList;
//            measure->GetMeasureAnglePoints3DList(anglePoints3DList);
//            for(int i = 0; i < anglePoints3DList.size(); ++i)
//            {
//                bool isOnPlane = true;
//                for(std::list<Point3D>::const_iterator iter = anglePoints3DList[i].begin(); iter != anglePoints3DList[i].end(); ++iter)
//                {
//                    Point3D pt3dProject = mpr->ProjectToPlane(*iter);
//                    isOnPlane &= (*iter == pt3dProject);
//                }
//                measure->UpdateMeasureAngleIsOnPlaneResultList(i, isOnPlane);
//            }
//
//            std::vector<std::list<Mcsf::MedViewer3D::Point3D>> distancePoints3DList;
//            measure->GetMeasureDistancePoints3DList(distancePoints3DList);
//            for(int i = 0; i < distancePoints3DList.size(); ++i)
//            {
//                bool isOnPlane = true;
//                for(std::list<Point3D>::const_iterator iter = distancePoints3DList[i].begin(); iter != distancePoints3DList[i].end(); ++iter)
//                {
//                    Point3D pt3dProject = mpr->ProjectToPlane(*iter);
//                    isOnPlane &= (*iter == pt3dProject);
//                }
//                measure->UpdateMeasureDistanceIsOnPlaneResultList(i, isOnPlane);
//            }
//
//            //Update the MeasureValGO according to the isOnPlaneResultList
//            measure->UpdateMeasureValGO();
//
//            //Update the isOnPlaneResultList in TextGO
//            std::vector<std::list<Mcsf::MedViewer3D::Point3D>> text4Vertex3DList;
//            text->GetText4Vertex3DList(text4Vertex3DList);
//            for(int i = 0; i < text4Vertex3DList.size(); ++i)
//            {
//                bool isOnPlane = true;
//                for(std::list<Point3D>::const_iterator iter = text4Vertex3DList[i].begin(); iter != text4Vertex3DList[i].end(); ++iter)
//                {
//                    Point3D pt3dProject = mpr->ProjectToPlane(*iter);
//                    isOnPlane &= (*iter == pt3dProject);
//                }
//                text->UpdateTextIsOnPlaneList(i, isOnPlane);
//            }
//            text->UpdateTextGO();
//
//            mpr->SetDirty(true);
//
//            }else{
//                out_of_slice--;
//                if(out_of_slice == 0)
//                    flag_OutOfRange = false;
//            }
//                    //static int prev_onFstSlice = 0, post_onLstSlice = 0;
//                    //static bool flag_OutOfRange_prev = false, flag_OutOfRange_post = false;
//
//                    //// process when MPR is AXIAL
//                    //if(section == AXIAL){
//                    //    goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section, PREV);
//                    //    std::string goKey = imageUID + "|" + GOTypeHelper::ToString(goType);
//                    //    auto goMpr_prev = mModelEntity->GetGraphicObject(goKey);
//                    //    goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR_CORNER_INFO, section, PREV);
//                    //    goKey = imageUID + "|" + GOTypeHelper::ToString(goType);
//                    //    auto goMprCornerInfo_prev = mModelEntity->GetGraphicObject(goKey);
//                    //    auto mprCornerInfo_prev = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(goMprCornerInfo_prev);
//                    //    auto mpr_prev = std::dynamic_pointer_cast<ClsGraphicObjectMPR>(goMpr_prev);
//
//                    //    goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section, POST);
//                    //    goKey = imageUID + "|" + GOTypeHelper::ToString(goType);
//                    //    auto goMpr_post = mModelEntity->GetGraphicObject(goKey);
//                    //    goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR_CORNER_INFO, section, POST);
//                    //    goKey = imageUID + "|" + GOTypeHelper::ToString(goType);
//                    //    auto goMprCornerInfo_post = mModelEntity->GetGraphicObject(goKey);
//                    //    auto mpr_post = std::dynamic_pointer_cast<ClsGraphicObjectMPR>(goMpr_post);
//                    //    auto mprCornerInfo_post = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(goMprCornerInfo_post);
//
//                    //    if(nullptr != goMpr_prev 
//                    //        && nullptr != goMprCornerInfo_prev
//                    //        && nullptr != mprCornerInfo_prev
//                    //        && nullptr != mpr_prev){
//
//                    //            mpr_prev->GetVertices(vertices_prev);
//                    //            for (int j=0; j<4; ++j){
//                    //                vertices_prev[j] = vertices_prev[j] + turnStep * vStepLen;
//                    //            }
//
//                    //            Vector3D vecX_prev = 0.5 * (vertices_prev[2] - vertices_prev[3]);
//                    //            Vector3D vecY_prev = 0.5 * (vertices_prev[0] - vertices_prev[3]);
//                    //            Point3D ptCenter_prev = vertices_prev[3] + vecX_prev + vecY_prev;
//                    //            Point3D ptCenterM_prev = matM2W.Inverse().Transform(ptCenter_prev);
//                    //            if (ptCenterM_prev.x < volumeRangeInModel[0] || 
//                    //                ptCenterM_prev.x > volumeRangeInModel[1] || 
//                    //                ptCenterM_prev.y < volumeRangeInModel[2] || 
//                    //                ptCenterM_prev.y > volumeRangeInModel[3] || 
//                    //                ptCenterM_prev.z < volumeRangeInModel[4] || 
//                    //                ptCenterM_prev.z > volumeRangeInModel[5] ){
//                    //                    prev_onFstSlice = out_of_slice + 1;
//                    //                    TPS_LOG_DEV_WARNING<<"TpsGalleryMPRPageTurnOperation::ModifyGraphicObject out of the range!";
//
//                    //                    if(prev_onFstSlice > 1){
//                    //                        prev_onFstSlice = 2;
//                    //                        flag_OutOfRange_prev = true;
//                    //                    }
//                    //            }
//
//                    //            double slicepos_prev, thickness_prev;
//                    //            size_t slicetotal_prev;
//
//                    //            if(!flag_OutOfRange_prev){
//                    //                MPRAPI::CalculateMPRMatrix(mpr_prev, vertices_prev);
//                    //                GetSlicePositionAndThickness(mpr_prev.get(), volume.get(), &slicepos_prev, &slicetotal_prev, &thickness_prev);
//                    //                ConvertSlicePosAndThicknessToImageText(mpr_slicepos_key_val[3], mpr_slicetotal_key_val[3], mpr_thickness_key_val[3], 
//                    //                    slicepos_prev, slicetotal_prev, thickness_prev, mprCornerInfo_prev);
//                    //                mpr_prev->SetDirty(true);
//                    //            }else{
//                    //                flag_OutOfRange_prev = false;
//                    //            }
//                    //    }
//
//                    //    if(nullptr != goMpr_post 
//                    //        && nullptr != goMprCornerInfo_post
//                    //        && nullptr != mprCornerInfo_post
//                    //        && nullptr != mpr_post){
//
//                    //            mpr_post->GetVertices(vertices_post);
//                    //            for (int j=0; j<4; ++j){
//                    //                vertices_post[j] = vertices_post[j] + turnStep * vStepLen;
//                    //            }
//
//                    //            Vector3D vecX_post = 0.5 * (vertices_post[2] - vertices_post[3]);
//                    //            Vector3D vecY_post = 0.5 * (vertices_post[0] - vertices_post[3]);
//                    //            Point3D ptCenter_post = vertices_post[3] + vecX_post + vecY_post;
//                    //            Point3D ptCenterM_post = matM2W.Inverse().Transform(ptCenter_post);
//                    //            if (ptCenterM_post.x < volumeRangeInModel[0] || 
//                    //                ptCenterM_post.x > volumeRangeInModel[1] || 
//                    //                ptCenterM_post.y < volumeRangeInModel[2] || 
//                    //                ptCenterM_post.y > volumeRangeInModel[3] || 
//                    //                ptCenterM_post.z < volumeRangeInModel[4] || 
//                    //                ptCenterM_post.z > volumeRangeInModel[5] ){
//                    //                    post_onLstSlice = out_of_slice + 1;
//                    //                    TPS_LOG_DEV_WARNING<<"TpsGalleryMPRPageTurnOperation::ModifyGraphicObject out of the range!";
//
//                    //                    if(post_onLstSlice > 1){
//                    //                        post_onLstSlice = 2;
//                    //                        flag_OutOfRange_post = true;
//                    //                    }
//                    //            }
//
//                    //            double slicepos_post, thickness_post;
//                    //            size_t slicetotal_post;
//
//                    //            if(!flag_OutOfRange_post){
//                    //                MPRAPI::CalculateMPRMatrix(mpr_post, vertices_post);
//                    //                GetSlicePositionAndThickness(mpr_post.get(), volume.get(), &slicepos_post, &slicetotal_post, &thickness_post);
//                    //                ConvertSlicePosAndThicknessToImageText(mpr_slicepos_key_val[4], mpr_slicetotal_key_val[4], mpr_thickness_key_val[4], 
//                    //                    slicepos_post, slicetotal_post, thickness_post, mprCornerInfo_post);
//                    //                mpr_post->SetDirty(true);
//                    //            }else{
//                    //                flag_OutOfRange_post = false;
//                    //            }
//                    //    }
//                    //}
//
//                    //TEST_PERFORMANCE_END("TpsGalleryMPRPageTurnOperation");
//    }
//    catch(std::exception& ex){
//        TPS_LOG_DEV_ERROR<<"TpsGalleryMPRPageTurnOperation::ModifyGraphicObject:"<<ex.what();
//        return false;
//    }
//    mDataRepository->GetGalleryImageManager()->SetSeries4DPageturned();
//    return true;
//}
//
//void TpsGalleryMPRPageTurnOperation::UpdatePosInPatientImageText_i(const std::string& posKey, double mousePosInPat, 
//    std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo) const{
//    std::wstringstream wss_pos;
//    char strtext_pos[64] = {0};
//    _snprintf_s(strtext_pos, sizeof(strtext_pos), "%.1f", 0.1 * mousePosInPat);
//    if(strtext_pos[sizeof(strtext_pos) - 1] != '\0'){
//        //printf("warning:string will be truncated");
//        strtext_pos[sizeof(strtext_pos)-1]='\0';
//    }
//    wss_pos<<strtext_pos;
//    FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
//    Pixel32* pStrBuffer = nullptr;
//    int strWidth, strHeight;
//    if(!pBufGen->GenerateRasterBuffer(wss_pos.str(), &pStrBuffer, strWidth, strHeight)){
//        TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer.\n";
//        delete[] pStrBuffer;
//        return;
//    }
//
//    mprCornerInfo->UpdateTextBlock(posKey, (unsigned char*)pStrBuffer, strWidth, strHeight);
//    delete[] pStrBuffer;
//
//}
//
//bool TpsGalleryMPRPageTurnOperation::ConvertSlicePosAndThicknessToImageText_i(
//    std::string sliceposValKey, std::string slicetotalValKey, std::string thicknessValKey,
//    double slicepos, size_t slicetotal, double thickness, std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo) const{
//
//    std::ostringstream oss_pos;
//    oss_pos << slicepos;
//    std::string str_pos = oss_pos.str();
//    std::wstring wstr_pos;
//    wstr_pos.assign(str_pos.begin(), str_pos.end());
//
//
//    char strtext_slicetotal[64]={0};
//    _snprintf_s(strtext_slicetotal, sizeof(strtext_slicetotal), "%d", (int)slicetotal);
//    if(strtext_slicetotal[sizeof(strtext_slicetotal) - 1] != '\0'){
//        //printf("warning:string will be truncated");
//        strtext_slicetotal[sizeof(strtext_slicetotal)-1]='\0';
//    }
//
//    char strtext_thickness[64]={0};
//    _snprintf_s(strtext_thickness, sizeof(strtext_thickness), "%.1f", thickness);
//    if(strtext_thickness[sizeof(strtext_thickness) - 1] != '\0'){
//        //printf("warning:string will be truncated");
//        strtext_thickness[sizeof(strtext_thickness)-1]='\0';
//    }
//
//    std::vector<std::string> sliceNo_and_thickness_key_val;
//    sliceNo_and_thickness_key_val.push_back(sliceposValKey);
//    sliceNo_and_thickness_key_val.push_back(slicetotalValKey);
//    sliceNo_and_thickness_key_val.push_back(thicknessValKey);
//
//    FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
//    Pixel32* pStrBuffer = nullptr;
//    int strWidth, strHeight;
//    
//    for(int i = 0; i < sliceNo_and_thickness_key_val.size(); ++i){
//        std::wstringstream wss;
//        if(i == 0){
//            wss << wstr_pos;
//        }else{
//            wss << (i == 1 ? strtext_slicetotal : strtext_thickness);
//        }
//
//        if(!pBufGen->GenerateRasterBuffer(wss.str(), &pStrBuffer, strWidth, strHeight)){
//            TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer.\n";
//            return false;
//        }
//
//        mprCornerInfo->UpdateTextBlock(sliceNo_and_thickness_key_val[i], (unsigned char*)pStrBuffer, strWidth, strHeight);
//
//        if(pStrBuffer!=nullptr){
//            delete[] pStrBuffer;
//            pStrBuffer = nullptr;
//        }
//    }
//
//    return true;
//}

TPS_END_NAMESPACE
