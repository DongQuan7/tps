//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_igallerycmdhandler.h
///  \brief   
///
///  \version 1.0
///  \date    Dec. 11, 2014
///  \{ 30830
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

// TPS
#include "RtTpsRenderLibrary/tps_rl_gallery_crosshair_movement_operation.h"
//#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"      
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"

// ZHENGHE   
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

static bool ConvertSlicePosAndThicknessToImageText(
    std::string sliceposValKey, std::string thicknessValKey,
    double slicepos, double thickness, std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo);


TpsGalleryCrosshairMovementOperation::TpsGalleryCrosshairMovementOperation(const std::string& strImageUID,
    FIRST_POSTFIX_COMPONENT ucSectionType, double xStart, double yStart, double xEnd, double yEnd, int hitMode, int crossMode)
    :m_strImageUID(strImageUID), 
     m_ucSectionType(ucSectionType),
     m_dXStart(xStart), 
     m_dYStart(yStart),
     m_dXEnd(xEnd), 
     m_dYEnd(yEnd),
     m_hitMode(hitMode),
     m_crossMode(crossMode){
}

TpsGalleryCrosshairMovementOperation::~TpsGalleryCrosshairMovementOperation(){

}

//bool TpsGalleryCrosshairMovementOperation::ModifyGraphicObject() const {
//
//    try{
//        GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, m_ucSectionType);
//        std::string strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//        auto go = mModelWarehouse->GetModelObject(strKey);
//        auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
//        if (spCrosshairGO == nullptr){
//            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairMovementOperation][ModifyGraphicObject]: "
//                <<"Failed to dynamic cast IGraphicObjects.";
//            return false;
//        }
//
//        goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, m_ucSectionType);
//        strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//        go = mModelWarehouse->GetModelObject(strKey);
//        auto spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
//        if (spMprGO == nullptr){
//            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairMovementOperation][ModifyGraphicObject]: "
//                <<"Failed to dynamic cast IGraphicObjects.";
//            return false;
//        }
//
//        strKey = m_strImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
//        go = mModelWarehouse->GetModelObject(strKey);
//        auto spVolumeGO = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(go);
//        if (spVolumeGO == nullptr){
//            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairMovementOperation][ModifyGraphicObject]: "
//                <<"Failed to dynamic cast IGraphicObjects.";
//            return false;
//        }
//        size_t dim[3];
//        spVolumeGO->GetDimension(dim);
//        Mcsf::MedViewer3D::Matrix4x4 modelToVolume(dim[0],       0,      0, 0, 
//                                                        0,  dim[1],      0, 0, 
//                                                        0,       0, dim[2], 0, 
//                                                    -0.5,    -0.5,   -0.5, 1);
//        Mcsf::MedViewer3D::Matrix4x4 matWorld2Volume = modelToVolume * spVolumeGO->GetModel2World().Inverse();
//
//        //Mcsf::MedViewer3D::Matrix4x4 matWorld2Patient = spVolumeGO->GetWorld2Patient();
//        //Mcsf::MedViewer3D::Matrix4x4 matVolume2Patient = matWorld2Patient * matWorld2Volume.Inverse();
//
//        Mcsf::MedViewer3D::Point2D point2d;
//        spCrosshairGO->GetCrosshairPoint2d(&point2d);
//
//        int nHitTest = spCrosshairGO->IsHit();
//        // 目前，暂不支持在Oblique模式下平移十字线，但可以旋转和拖动十字线中心
//        //float fAngleX, fAngleY;
//        //spCrosshairGO->GetAngles(&fAngleX, &fAngleY);
//
//        // nHitTest :
//        //     1 : X coordinate axis; 
//        //     2 : Y coordinate axis;
//        //     3 : X+Y coordinate axis;
//        if (nHitTest == 1) {
//            //float fDeltaY = m_dYEnd - point2d.y;
//            point2d.y = m_dYEnd;
//            //float fCos = cos(fAngleY / 180.0 * PI);
//            //float fSin = sin(fAngleY / 180.0 * PI);
//            //if (fabs(fSin) > 0.0001) {
//            //    point2d.x += fDeltaY * fCos / fSin;
//            //}
//        }
//        else if (nHitTest == 2) {
//            //float fDeltaX = m_dXEnd - point2d.x;
//            point2d.x = m_dXEnd;
//            //float fCos = cos(fAngleX / 180.0 * PI);
//            //float fSin = sin(fAngleY / 180.0 * PI);
//
//        }
//        else if (nHitTest == 3) {
//            point2d.x = m_dXEnd;
//            point2d.y = m_dYEnd;
//        }
//        else {
//            TPS_LOG_DEV_ERROR<<"Unknown hitted mode.";
//            return false;
//        }
//
//        Mcsf::MedViewer3D::Point3D pt3dCrosshairCenter = spMprGO->PlaneCoordinateToWorldCoordiate(point2d.x, point2d.y);
//        Mcsf::MedViewer3D::Point3D pt3dCrosshairCenterInVolume = matWorld2Volume.Transform(pt3dCrosshairCenter);
//        double dSliceX = pt3dCrosshairCenterInVolume.x;
//        double dSliceY = pt3dCrosshairCenterInVolume.y;
//        double dSliceZ = pt3dCrosshairCenterInVolume.z;
//
//        CrosshairGraphicObject::CROSSHAIR_MODE crosshairMode;
//        spCrosshairGO->GetCrosshairMode(&crosshairMode);
//        if (crosshairMode == CrosshairGraphicObject::CROSSHAIR_ORTHOGONAL) {
//            // Adjust to exact slice
//            dSliceX = (int)(dSliceX + 0.5);
//            dSliceY = (int)(dSliceY + 0.5);
//            dSliceZ = (int)(dSliceZ + 0.5);
//            pt3dCrosshairCenter = matWorld2Volume.Inverse().Transform(Mcsf::MedViewer3D::Point3D(dSliceX, dSliceY, dSliceZ));
//            point2d = spMprGO->GetPlaneCoordinate(pt3dCrosshairCenter, false);
//            //Mcsf::MedViewer3D::Point3D pt3dCrosshairCenterPrev = matWorld2Volume.Inverse().Transform(Mcsf::MedViewer3D::Point3D(dSliceX, dSliceY, dSliceZ - 1));
//            //Mcsf::MedViewer3D::Point3D pt3dCrosshairCenterPost = matWorld2Volume.Inverse().Transform(Mcsf::MedViewer3D::Point3D(dSliceX, dSliceY, dSliceZ + 1));
//        }
//
//        //printf("Move crosshair (%f, %f, %f), dim (%d, %d, %d)\n", dSliceX, dSliceY, dSliceZ, dim[0], dim[1], dim[2]);
//        // If the crosshair center is out of the image, ignore it
//        if ((dSliceX < 0 ) || (dSliceY < 0) || (dSliceZ < 0) ||
//            (dSliceX > (dim[0] - 1)) || (dSliceY > (dim[1] - 1)) || (dSliceZ > (dim[2] - 1)))
//        {
//            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairMovementOperation]: Failed due to crosshair center out of image.";
//            return false;
//        }
//
//        spCrosshairGO->SetCrosshairPoint2d(point2d);
//        Mcsf::MedViewer3D::Point3D oldCrosshairCenter;
//        spCrosshairGO->GetCrosshairPoint3d(&oldCrosshairCenter);
//        spCrosshairGO->SetCrosshairPoint3d(pt3dCrosshairCenter);
//        spCrosshairGO->SetDirty(true);
//        Mcsf::MedViewer3D::Vector3D v3dOffset = pt3dCrosshairCenter - oldCrosshairCenter;
//
//        // linked update
//        FIRST_POSTFIX_COMPONENT ucLinkedMpr[2];
//        //std::string strSlicePostionKey[2];
//        //std::string strThicknessKey[2];
//        ////double dSlicePosition[2];
//
//        if (m_ucSectionType == AXIAL) {
//            ucLinkedMpr[0] = CORONAL;
//            ucLinkedMpr[1] = SAGITTAL;
//            //strSlicePostionKey[0] = SLICEPOSITION_CORONAL_SECTION_VAL;
//            //strSlicePostionKey[1] = SLICEPOSITION_SAGITTAL_SECTION_VAL;
//            //strThicknessKey[0] = THICKNESS_CORONAL_SECTION_VAL;
//            //strThicknessKey[1] = THICKNESS_SAGITTAL_SECTION_VAL;
//            ////dSlicePosition[0] = dSliceY;
//            ////dSlicePosition[1] = dSliceX;
//        }
//        else if(m_ucSectionType == SAGITTAL) {
//            ucLinkedMpr[0] = CORONAL;
//            ucLinkedMpr[1] = AXIAL;
//            //strSlicePostionKey[0] = SLICEPOSITION_CORONAL_SECTION_VAL;
//            //strSlicePostionKey[1] = SLICEPOSITION_AXIAL_SECTION_VAL;
//            //strThicknessKey[0] = THICKNESS_CORONAL_SECTION_VAL;
//            //strThicknessKey[1] = THICKNESS_AXIAL_SECTION_VAL;
//            ////dSlicePosition[0] = dSliceY;
//            ////dSlicePosition[1] = dSliceZ;
//        }
//        else if(m_ucSectionType == CORONAL) {
//            ucLinkedMpr[0] = AXIAL;
//            ucLinkedMpr[1] = SAGITTAL;
//            //strSlicePostionKey[0] = SLICEPOSITION_AXIAL_SECTION_VAL;
//            //strSlicePostionKey[1] = SLICEPOSITION_SAGITTAL_SECTION_VAL;
//            //strThicknessKey[0] = THICKNESS_AXIAL_SECTION_VAL;
//            //strThicknessKey[1] = THICKNESS_SAGITTAL_SECTION_VAL;
//            ////dSlicePosition[0] = dSliceZ;
//            ////dSlicePosition[1] = dSliceX;
//        }
//
//        std::shared_ptr<TpsTextOverlayGraphicObject> spCornorInfoGO = nullptr;
//        double dThickness = 0.0;
//        for (int i = 0; i < 2; i++) {
//
//            goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, ucLinkedMpr[i]);
//            strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//            go = mModelWarehouse->GetModelObject(strKey);
//            spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
//            if (spCrosshairGO == nullptr){
//                TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairMovementOperation][ModifyGraphicObject]: "
//                    <<"Failed to dynamic cast IGraphicObjects.";
//                return false;
//            }
//
//            goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, ucLinkedMpr[i]);
//            strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//            go = mModelWarehouse->GetModelObject(strKey);
//            spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
//            if (spMprGO == nullptr){
//                TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairMovementOperation][ModifyGraphicObject]: "
//                    <<"Failed to dynamic cast IGraphicObjects.";
//                return false;
//            }
//            dThickness = spMprGO->GetThickness();
//
//            spCrosshairGO->SetCrosshairPoint3d(pt3dCrosshairCenter);
//            point2d = spMprGO->GetPlaneCoordinate(pt3dCrosshairCenter, false);
//            spCrosshairGO->SetCrosshairPoint2d(point2d);
//
//            Mcsf::MedViewer3D::Vector3D v3dNormal;
//            spMprGO->GetNormal(&v3dNormal);
//            double dDistance = v3dOffset.DotProduct(v3dNormal);
//            spMprGO->Translate(v3dNormal * dDistance);
//            spMprGO->SetDirty(true);
//
//            //goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR_CORNER_INFO, ucLinkedMpr[i]);
//            //strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//            //go = mModelWarehouse->GetModelObject(strKey);
//            //spCornorInfoGO = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(go);
//            //if (spCornorInfoGO == nullptr){
//            //    TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairMovementOperation][ModifyGraphicObject]: "
//            //        <<"Failed to dynamic cast IGraphicObjects.";
//            //    return false;
//            //}
//            //
//            //double thickTemp, slicePosTemp;
//            //int nSliceTotal = -1;
//            //TpsMprHelper::GetSliceInfo(spMprGO.get(), spVolumeGO.get(), &slicePosTemp, &nSliceTotal, &thickTemp);
//            //ConvertSlicePosAndThicknessToImageText(strSlicePostionKey[i],strThicknessKey[i], slicePosTemp, thickTemp, spCornorInfoGO);
//            ////ConvertSlicePosAndThicknessToImageText(strSlicePostionKey[i],strThicknessKey[i], dSlicePosition[i], dThickness, spCornorInfoGO);
//        }
//
//        //if (m_ucSectionType != AXIAL) {
//        //    // prev and post image
//        //    SECOND_POSTFIX_COMPONENT ucPrevPost[2] = {PREV, POST};
//        //    std::string strSlicePositionPrevPost[2] = {SLICEPOSITION_AXIAL_SECTION_PREV_VAL, SLICEPOSITION_AXIAL_SECTION_POST_VAL};
//        //    std::string strThicknessPrevPost[2] = {THICKNESS_AXIAL_SECTION_PREV_VAL, THICKNESS_AXIAL_SECTION_POST_VAL};
//        //    //double dSlicePostionPrevPost[2] = {dSliceZ - 1, dSliceZ + 1};
//
//        //    for (int i = 0; i < 2; i++) {
//        //        goType = GOTypeHelper::ComposeAsGOType(MPR, AXIAL, ucPrevPost[i]);
//        //        strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//        //        go = mModelWarehouse->GetModelObject(strKey);
//        //        spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
//        //        if (spMprGO == nullptr){
//        //            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairMovementOperation][ModifyGraphicObject]: "
//        //                <<"Failed to dynamic cast IGraphicObjects.";
//        //            return false;
//        //        }
//        //        dThickness = spMprGO->GetThickness();
//
//        //        Mcsf::MedViewer3D::Vector3D v3dNormal;
//        //        spMprGO->GetNormal(&v3dNormal);
//        //        double dDistance = v3dOffset.DotProduct(v3dNormal);
//        //        spMprGO->Translate(v3dNormal * dDistance);
//        //        spMprGO->SetDirty(true);
//
//        //        goType = GOTypeHelper::ComposeAsGOType(MPR_CORNER_TEXT, AXIAL, ucPrevPost[i]);
//        //        strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//        //        go = mModelWarehouse->GetModelObject(strKey);
//        //        spCornorInfoGO = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(go);
//        //        if (spCornorInfoGO == nullptr){
//        //            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairMovementOperation][ModifyGraphicObject]: "
//        //                <<"Failed to dynamic cast IGraphicObjects.";
//        //            return false;
//        //        }
//
//        //        double thickTemp, slicePosTemp;
//        //        GetSlicePositionAndThickness(spMprGO.get(), spVolumeGO.get(), &slicePosTemp, &thickTemp);
//        //        ConvertSlicePosAndThicknessToImageText(strSlicePositionPrevPost[i], strThicknessPrevPost[i], slicePosTemp, thickTemp, spCornorInfoGO);
//        //        /*if (fabs(dSlicePostionPrevPost[i] - slicePosTemp) > 0.01)
//        //            printf("Wrong slice pos: %f, in theory %f\n", dSlicePostionPrevPost[i], slicePosTemp);*/
//        //        //ConvertSlicePosAndThicknessToImageText(strSlicePositionPrevPost[i], strThicknessPrevPost[i], dSlicePostionPrevPost[i], dThickness, spCornorInfoGO);
//        //    }
//        //}
//
//        if (crosshairMode == CrosshairGraphicObject::CROSSHAIR_ORTHOGONAL) 
//            return true;
//        // update 
//    }
//    catch(std::exception& ex){
//        TPS_LOG_DEV_ERROR<<"CrosshairHitTestOperation::ModifyGraphicObject:"<<ex.what();
//        return false;
//    }
//    return true;
//}

bool TpsGalleryCrosshairMovementOperation::ModifyGraphicObject() const {
    try{

        Mcsf::MedViewer3D::Point2D pt2dPosPoint(0, 0);
        GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, m_ucSectionType);
        std::string strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
        auto go = mModelWarehouse->GetModelObject(strKey);
        auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
        if (spCrosshairGO == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairMovementOperation][ModifyGraphicObject]: "
                <<"Failed to dynamic cast IGraphicObjects.";
            return false;
        }

        goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, m_ucSectionType);
        strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
        go = mModelWarehouse->GetModelObject(strKey);
        auto spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
        if (spMprGO == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairMovementOperation][ModifyGraphicObject]: "
                <<"Failed to dynamic cast IGraphicObjects.";
            return false;
        }

        strKey = m_strImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
        go = mModelWarehouse->GetModelObject(strKey);
        auto spVolumeGO = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(go);
        if (spVolumeGO == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairMovementOperation][ModifyGraphicObject]: "
                <<"Failed to dynamic cast IGraphicObjects.";
            return false;
        }

        size_t dim[3];
        spVolumeGO->GetDimension(dim);
        Mcsf::MedViewer3D::Matrix4x4 modelToVolume(dim[0], 0, 0, 0,
            0, dim[1], 0, 0,
            0, 0, dim[2], 0,
            -0.5, -0.5, -0.5, 1);

        Mcsf::MedViewer3D::Vector3D v3dOffset;
        Mcsf::MedViewer3D::Point3D pt3dEndPoint = spMprGO->PlaneCoordinateToWorldCoordiate(m_dXEnd, m_dYEnd);
     //   Mcsf::MedViewer3D::Point3D pt3dStart = spMprGO->PlaneCoordinateToWorldCoordiate(m_dXStart, m_dYStart);

        Mcsf::MedViewer3D::Matrix4x4 matWorld2Volume = modelToVolume * spVolumeGO->GetModel2World().Inverse();
        if (m_crossMode == CrosshairGraphicObject::CROSSHAIR_ORTHOGONAL) {
            // Adjust to exact slice

            Mcsf::MedViewer3D::Point3D ortEndPoint = matWorld2Volume.Transform(pt3dEndPoint);
            pt3dEndPoint.x = (int)(ortEndPoint.x + 0.5);
            pt3dEndPoint.y = (int)(ortEndPoint.y + 0.5);
            pt3dEndPoint.z = (int)(ortEndPoint.z + 0.5);
            pt3dEndPoint = matWorld2Volume.Inverse().Transform(pt3dEndPoint);
        }

   //     v3dOffset = pt3dEndPoint - pt3dStart;//计算出位移量的空间位移


        // linked update
        FIRST_POSTFIX_COMPONENT ucLinkedMpr[2];

        if (m_ucSectionType == AXIAL) {
            ucLinkedMpr[0] = CORONAL;
            ucLinkedMpr[1] = SAGITTAL;
        }
        else if(m_ucSectionType == SAGITTAL) {
            ucLinkedMpr[0] = AXIAL;
            ucLinkedMpr[1] = CORONAL;
        }
        else if(m_ucSectionType == CORONAL) {
            ucLinkedMpr[0] = AXIAL;
            ucLinkedMpr[1] = SAGITTAL;
        }

       /* public enum HITTED_MODE
        {
            HITTED_NOTHING = 0,
            HITTED_AXIS_Y,
            HITTED_AXIS_X,
            HITTED_CENTER,
        };*/

       // int nSelectedXY = 0;


        //添加一中模式，move 3位 中心移动，2为x线移动，1为y移动，4位z移动
        std::vector<FIRST_POSTFIX_COMPONENT> selectedXY;
        if (m_hitMode == CrosshairGraphicObject::HITTED_CENTER) {
            selectedXY.push_back(ucLinkedMpr[0]);
            selectedXY.push_back(ucLinkedMpr[1]);
        }
        else if (m_hitMode == CrosshairGraphicObject::HITTED_AXIS_X) {
            selectedXY.push_back(ucLinkedMpr[0]);
        }
        else if (m_hitMode == CrosshairGraphicObject::HITTED_AXIS_Y) {
            selectedXY.push_back(ucLinkedMpr[1]);
        }
        else {
            return false;
        }

        for (int i = 0; i < selectedXY.size();i++)
        {
            goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, selectedXY[i]);
            strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
            go = mModelWarehouse->GetModelObject(strKey);
            spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
            if (spMprGO == nullptr){
                TPS_LOG_DEV_ERROR << "[ERROR][TpsGalleryCrosshairMovementOperation][ModifyGraphicObject]: "
                    << "Failed to dynamic cast IGraphicObjects.";
                return false;
            }

            Mcsf::MedViewer3D::Point3D pt3dStart = spMprGO->PlaneCoordinateToWorldCoordiate(0, 0);//取平面上随机一个点，这里取（0，0），可取随机一个点。
            v3dOffset = pt3dEndPoint - pt3dStart;//计算出位移量的空间位移
            Mcsf::MedViewer3D::Vector3D v3dNormal;
            spMprGO->GetNormal(&v3dNormal);
            spMprGO->Translate(v3dOffset);//将选中面平移

            //平移完以后调整vertice 窗口位置，将中心点移到新的图形外接矩阵的中心位置
            Mcsf::MedViewer3D::Point3D vertices[4];
            spMprGO->GetVertices(vertices);
            Mcsf::MedViewer3D::Point3D verticesF[4];
            Mcsf::MedViewer3D::Matrix4x4 matW2V = spVolumeGO->GetModel2World().Inverse();
            matW2V.Prepend(modelToVolume);
            for (int i = 0; i < 4; ++i){
                verticesF[i] = matW2V.Transform(vertices[i]);
            }
            std::vector<Mcsf::MedViewer3D::Point3D> intersections;
            std::vector<Mcsf::MedViewer3D::Point3D> newIntersects;
            TpsGalleryCrosshairMovementOperation::GetIntersectionsOfVerticeandMpr(dim, verticesF, intersections);

            for (int i = 0; i < intersections.size(); ++i){
                newIntersects.push_back(matW2V.Inverse().Transform(intersections[i]));
            }
           
            if (newIntersects.size() > 0)
            {
                double maxX, maxY, minX, minY;
                Mcsf::MedViewer3D::Point2D temp = spMprGO->GetPlaneCoordinate(newIntersects[0],false);
                maxX = minX = temp.x;
                maxY = minY = temp.y;
                for (int i = 1; i < newIntersects.size(); i++)
                {
                    Mcsf::MedViewer3D::Point2D insectPoint = spMprGO->GetPlaneCoordinate(newIntersects[i],false);
                    if (insectPoint.x > maxX)
                        maxX = insectPoint.x;
                    else if ((insectPoint.x < minX))
                        minX = insectPoint.x;
                    if (insectPoint.y > maxY)
                        maxY = insectPoint.y;
                    else if ((insectPoint.y < minY))
                        minY = insectPoint.y;
                }
                double midX = (maxX + minX) / 2;
                double midY = (maxY + minY) / 2;

                Mcsf::MedViewer3D::Point3D verticesNewCenter = spMprGO->PlaneCoordinateToWorldCoordiate(midX, midY);
                Mcsf::MedViewer3D::Point3D verticesCenter = (vertices[0] + vertices[2]) * 0.5;

                auto matM2W = spVolumeGO->GetModel2World();
                Mcsf::MedViewer3D::Vector3D offSet = verticesNewCenter - verticesCenter;
                spMprGO->Translate(offSet);
            }
            else
            {
                spMprGO->Translate(-v3dOffset);
            }

            spMprGO->SetDirty(true);
        }
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"TpsGalleryCrosshairMovementOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }
    return true;
}

bool TpsGalleryCrosshairMovementOperation::GetIntersectionsOfVerticeandMpr(
    size_t* dim, const Mcsf::MedViewer3D::Point3D* pVertices,
    std::vector<Mcsf::MedViewer3D::Point3D>& intersections) {
        double x0 = pVertices[0].x;
        double y0 = pVertices[0].y;
        double z0 = pVertices[0].z;
        double x1 = pVertices[1].x;
        double y1 = pVertices[1].y;
        double z1 = pVertices[1].z;
        double x2 = pVertices[2].x;
        double y2 = pVertices[2].y;
        double z2 = pVertices[2].z;
        double x3 = pVertices[3].x;
        double y3 = pVertices[3].y;
        double z3 = pVertices[3].z;
        double xPara = (y2 - y1)*(z3 - z1) - (z2 - z1)*(y3 - y1);
        double yPara = (x2 - x1)*(z3 - z1) - (x3 - x1)*(z2 - z1);
        double zPara = (x2 - x1)*(y3 - y1) - (x3 - x1)*(y2 - y1);
        double planeFunc = 0.0;
        planeFunc = (x0 - x1)*xPara - (y0 - y1)*yPara + (z0 - z1)*zPara;
        //planeFunc = (x1 - x0) * (y2 - y0) * (z3 - z0) + (y1 - y0) * (z2 - z0) * (x3 - x0) +
        //    (z1 - z0) * (x2 - x0) * (y3 - y0) - (z1 - z0) * (y2 - y0) * (x3 - x0) - 
        //    (x1 - x0) * (z2 - z0) * (y3 - y0) - (y1 - y0) * (x2 - x0) * (z3 - z0);

        if ((planeFunc > 1e-6) || (planeFunc < -(1e-6))) return false;

        Mcsf::MedViewer3D::Point3D intersection;
        double x = 0;
        double y = 0;
        double z = 0;
        bool flag = false;

        if ((xPara > 1e-6) || (xPara < -1e-6)) {

            flag = true;

            x = ((0 - y1) * yPara - (dim[2] - z1) * zPara) / xPara + x1;

            if (x >= 0 && x <= dim[0]) {
                intersection.x = x;
                intersection.y = 0;
                intersection.z = dim[2];
                intersections.push_back(intersection);
            }

            x = ((dim[1] - y1) * yPara - (dim[2] - z1) * zPara) / xPara + x1;

            if (x >= 0 && x <= dim[0]) {
                intersection.x = x;
                intersection.y = dim[1];
                intersection.z = dim[2];
                intersections.push_back(intersection);
            }

            x = ((0 - y1) * yPara - (0 - z1) * zPara) / xPara + x1;

            if (x >= 0 && x <= dim[0]) {
                intersection.x = x;
                intersection.y = 0;
                intersection.z = 0;
                intersections.push_back(intersection);
            }

            x = ((dim[1] - y1) * yPara - (0 - z1) * zPara) / xPara + x1;

            if (x >= 0 && x <= dim[0]) {
                intersection.x = x;
                intersection.y = dim[1];
                intersection.z = 0;
                intersections.push_back(intersection);
            }
        }
        if ((yPara > 1e-6) || (yPara < -1e-6)) {

            y = ((0 - x1) * xPara + (0 - z1) * zPara) / yPara + y1;

            if ((y > 0 && y < dim[1]) || ((y == 0 || y == dim[1]) && (!flag))) {
                intersection.x = 0;
                intersection.y = y;
                intersection.z = 0;
                intersections.push_back(intersection);
            }

            y = ((dim[0] - x1) * xPara + (dim[2] - z1) * zPara) / yPara + y1;

            if ((y > 0 && y < dim[1]) || ((y == 0 || y == dim[1]) && (!flag))) {
                intersection.x = dim[0];
                intersection.y = y;
                intersection.z = dim[2];
                intersections.push_back(intersection);
            }

            y = ((0 - x1) * xPara + (dim[2] - z1) * zPara) / yPara + y1;

            if ((y > 0 && y < dim[1]) || ((y == 0 || y == dim[1]) && (!flag))) {
                intersection.x = 0;
                intersection.y = y;
                intersection.z = dim[2];
                intersections.push_back(intersection);
            }

            y = ((dim[0] - x1) * xPara + (0 - z1) * zPara) / yPara + y1;

            if ((y > 0 && y < dim[1]) || ((y == 0 || y == dim[1]) && (!flag))) {
                intersection.x = dim[0];
                intersection.y = y;
                intersection.z = 0;
                intersections.push_back(intersection);
            }

            flag = true;
        }

        if ((zPara > 1e-6) || (zPara < -1e-6)){

            z = ((0 - y1) * yPara - (0 - x1) * xPara) / zPara + z1;

            if ((z > 0 && z < dim[2]) || ((z == 0 || z == dim[2]) && (!flag))) {
                intersection.x = 0;
                intersection.y = 0;
                intersection.z = z;
                intersections.push_back(intersection);
            }

            z = ((0 - y1) * yPara - (dim[0] - x1) * xPara) / zPara + z1;

            if ((z > 0 && z < dim[2]) || ((z == 0 || z == dim[2]) && (!flag))) {
                intersection.x = dim[0];
                intersection.y = 0;
                intersection.z = z;
                intersections.push_back(intersection);
            }

            z = ((dim[1] - y1) * yPara - (dim[0] - x1) * xPara) / zPara + z1;

            if ((z > 0 && z < dim[2]) || ((z == 0 || z == dim[2]) && (!flag))) {
                intersection.x = dim[0];
                intersection.y = dim[1];
                intersection.z = z;
                intersections.push_back(intersection);
            }

            z = ((dim[1] - y1) * yPara - (0 - x1) * xPara) / zPara + z1;

            if ((z > 0 && z < dim[2]) || ((z == 0 || z == dim[2]) && (!flag))) {
                intersection.x = 0;
                intersection.y = dim[1];
                intersection.z = z;
                intersections.push_back(intersection);
            }
        }
        if (intersections.size() < 3) {
            return false;
        }
        return true;
}

static bool ConvertSlicePosAndThicknessToImageText(
    std::string sliceposValKey, std::string thicknessValKey,
    double slicepos, double thickness, std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo){

    //sliceposValKey, thicknessValKey, slicepos, thickness, mprCornerInfo;
    //return true;

    char strtext_slice[64]={0};
    _snprintf_s(strtext_slice, sizeof(strtext_slice), "%d", (int)slicepos);
    if(strtext_slice[sizeof(strtext_slice) - 1] != '\0'){
        //printf("warning:string will be truncated");
        strtext_slice[sizeof(strtext_slice)-1]='\0';
    }

    char strtext_thickness[64]={0};
    _snprintf_s(strtext_thickness, sizeof(strtext_thickness), "%.1f", thickness);
    if(strtext_thickness[sizeof(strtext_thickness) - 1] != '\0'){
        //printf("warning:string will be truncated");
        strtext_thickness[sizeof(strtext_thickness)-1]='\0';
    }

    std::vector<std::string> slice_thickness_key_val;
    slice_thickness_key_val.push_back(sliceposValKey);
    slice_thickness_key_val.push_back(thicknessValKey);

    FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
    Pixel32* pStrBuffer = nullptr;
    int strWidth, strHeight;
    
    for(int i = 0; i < slice_thickness_key_val.size(); ++i){
        std::wstringstream wss;
        wss<<(i == 0 ? strtext_slice : strtext_thickness);

        if(!pBufGen->GenerateRasterBuffer(wss.str(), &pStrBuffer, strWidth, strHeight)){
            TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer.\n";
            return false;
        }

        mprCornerInfo->UpdateTextBlock(slice_thickness_key_val[i], (unsigned char*)pStrBuffer, strWidth, strHeight);

        if(pStrBuffer!=nullptr){
            delete[] pStrBuffer;
            pStrBuffer = nullptr;
        }
    }

    return true;
}

TPS_END_NAMESPACE  // end namespace tps
