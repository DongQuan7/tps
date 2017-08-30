//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rl_gallery_crosshair_rotate_operation.cpp
///  \brief   
///
///  \version 1.0
///  \date    Dec. 12, 2014
///  \{31739
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"     

// TPS
#include "RtTpsRenderLibrary/tps_rl_gallery_crosshair_rotate_operation.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_crosshair_movement_operation.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"      
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"

// ZHENGHE     
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"    

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsGalleryCrosshairRotateOperation::TpsGalleryCrosshairRotateOperation(std::string& strImageUID, 
    FIRST_POSTFIX_COMPONENT strSectionType, double xStart, double yStart, double xEnd, double yEnd, double rotateX, double rotateY, int hitMode, int crossMode)
    :m_strImageUID(strImageUID), 
    m_strSectionType(strSectionType),
    m_dXStart(xStart), 
    m_dYStart(yStart),
    m_dXEnd(xEnd), 
    m_dYEnd(yEnd),
    m_dXRotateCenter(rotateX),
    m_dYRotateCenter(rotateY),
    m_hitMode(hitMode),
    m_crossMode(crossMode){
}

TpsGalleryCrosshairRotateOperation::~TpsGalleryCrosshairRotateOperation(){

}

//bool TpsGalleryCrosshairRotateOperation::ModifyGraphicObject() const {
//
//    try{
//
//        auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, m_strSectionType);
//        std::string strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//        auto go = mModelWarehouse->GetModelObject(strKey);
//        auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
//        if (spCrosshairGO == nullptr){
//            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairRotateOperation][ModifyGraphicObject]: "
//                <<"Failed to dynamic cast IGraphicObjects.";
//            return false;
//        }
//
//        goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, m_strSectionType);
//        strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//        go = mModelWarehouse->GetModelObject(strKey);
//        auto spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
//        if (spMprGO == nullptr){
//            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairRotateOperation][ModifyGraphicObject]: "
//                <<"Failed to dynamic cast IGraphicObjects.";
//            return false;
//        }
//
//        FIRST_POSTFIX_COMPONENT strMprXY[2];
//        if (m_strSectionType == AXIAL) {
//            strMprXY[0] = CORONAL;
//            strMprXY[1] = SAGITTAL;
//        }
//        else if(m_strSectionType == SAGITTAL) {
//            strMprXY[0] = AXIAL;
//            strMprXY[1] = CORONAL;
//        }
//        else if(m_strSectionType == CORONAL) {
//            strMprXY[0] = AXIAL;
//            strMprXY[1] = SAGITTAL;
//        }
//
//        // nHitTest : 
//        //      5 => X coordinate axis + Rotate; 
//        //      6 => Y coordinate axis + Rotate
//        int nHitTest = spCrosshairGO->IsHit();
//        int nSelectedXY = 0;
//        if (nHitTest == 5) {
//            nSelectedXY = 0;
//        }
//        else if (nHitTest == 6) {
//            nSelectedXY = 1;
//        }
//        else {
//            return false;
//        }
//
//        Mcsf::MedViewer3D::Point3D pt3dCrosshairCenter;
//        spCrosshairGO->GetCrosshairPoint3d(&pt3dCrosshairCenter);
//        Mcsf::MedViewer3D::Point2D pt2dCrosshairCenter;
//        spCrosshairGO->GetCrosshairPoint2d(&pt2dCrosshairCenter);
//
//        //Mcsf::MedViewer3D::Point3D pt3dStart = spMprGO->PlaneCoordinateToWorldCoordiate(m_dXStart, m_dYStart);
//        //Mcsf::MedViewer3D::Point3D pt3dEnd   = spMprGO->PlaneCoordinateToWorldCoordiate(m_dXEnd, m_dYEnd);
//        //Mcsf::MedViewer3D::Vector3D preVector3D = pt3dStart - pt3dCrosshairCenter;
//        //Mcsf::MedViewer3D::Vector3D currVector3D = pt3dEnd - pt3dCrosshairCenter;
//        //double dAngle = AngleBetween(currVector3D,preVector3D);
//        //Mcsf::MedViewer3D::Vector3D vecNormal= CrossProduct(preVector3D, currVector3D);
//        //Mcsf::MedViewer3D::Vector3D rotateNormal;
//        //vecNormal.GetNormalize(rotateNormal);
//        //Mcsf::MedViewer3D::Quaternion myRotate(dAngle, rotateNormal);
//
//        //Mcsf::MedViewer3D::Vector3D mprNormal;
//        //spMprGO->GetNormal(&mprNormal);
//        //double dDeltaAngle = 0.0;
//        //if (mprNormal == rotateNormal) {
//        //    dDeltaAngle = dAngle;
//        //}
//        //else if (mprNormal == -rotateNormal) {
//        //    dDeltaAngle = -dAngle;
//        //}
//
//        // Original angle
//        float fAngleXY[2];
//        spCrosshairGO->GetAngles(&fAngleXY[0], &fAngleXY[1]);
//
//        // New angle
//        double dAngleEnd = atan2(m_dYEnd - pt2dCrosshairCenter.y, m_dXEnd - pt2dCrosshairCenter.x);
//        //double dAngleStart = atan2(m_dYStart - pt2dCrosshairCenter.y, m_dXStart - pt2dCrosshairCenter.x);
//        double dAngleStart = fAngleXY[nSelectedXY] * PI / 180.0;
//
//        // Rotate with delta angle
//        double dDeltaAngle = dAngleEnd - dAngleStart;
//        Mcsf::MedViewer3D::Vector3D rotateNormal;
//        spMprGO->GetNormal(&rotateNormal);
//        Mcsf::MedViewer3D::Quaternion myRotate(dDeltaAngle, rotateNormal);
//
//        fAngleXY[nSelectedXY] += dDeltaAngle * 180.0 / PI;
//        while(fAngleXY[nSelectedXY] >= 360.0)
//            fAngleXY[nSelectedXY] -= 360.0;
//        while(fAngleXY[nSelectedXY] < 0.0)
//            fAngleXY[nSelectedXY] += 360.0;
//        spCrosshairGO->SetAngles(fAngleXY[0], fAngleXY[1]);
//        spCrosshairGO->SetDirty(true);
//
//        // linked update
//        {
//            //strKey = m_strImageUID + "|" + CROSSHAIR_PREFIX + strMprXY[nSelectedXY];
//            //go = mModelWarehouse->GetModelObject(strKey);
//            //spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
//            //if (spCrosshairGO == nullptr){
//            //    TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairRotateOperation][ModifyGraphicObject]: "
//            //        <<"Failed to dynamic cast IGraphicObjects.";
//            //    return false;
//            //}
//            goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, strMprXY[nSelectedXY]);
//            strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//            go = mModelWarehouse->GetModelObject(strKey);
//            spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
//            if (spMprGO == nullptr){
//                TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairRotateOperation][ModifyGraphicObject]: "
//                    <<"Failed to dynamic cast IGraphicObjects.";
//                return false;
//            }
//
//            // Move the center of object to the origin of the world coordinate
//            Mcsf::MedViewer3D::Vector3D vTranslate = Mcsf::MedViewer3D::Vector3D(pt3dCrosshairCenter.x, pt3dCrosshairCenter.y, pt3dCrosshairCenter.z);
//            spMprGO->Translate(-vTranslate);
//            // Roate
//            Mcsf::MedViewer3D::Matrix4x4 mat = spMprGO->GetModel2WorldMatrix();
//            mat.Prepend(Mcsf::MedViewer3D::Matrix4x4(myRotate));
//            spMprGO->SetModel2WorldMatrix(mat);
//            // Move the center of the object back 
//            spMprGO->Translate(vTranslate);
//            //spMprGO->SetRotate(myRotate); // no crosshair center in mprgo. !!!
//            spMprGO->SetDirty(true);
//
//            //// Prev and Post mpr update
//            //if (strMprXY[nSelectedXY] == AXIAL) {
//            //    SECOND_POSTFIX_COMPONENT strPrevPost[2] = {PREV, POST};
//            //    for (int i = 0; i < sizeof(strPrevPost)/sizeof(strPrevPost[0]); i++) {
//            //        goType = GOTypeHelper::ComposeAsGOType(MPR, AXIAL, strPrevPost[i]);
//            //        strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//            //        go = mModelWarehouse->GetModelObject(strKey);
//            //        spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
//            //        if (spMprGO == nullptr){
//            //            TPS_LOG_DEV_ERROR<<"[ERROR][CrosshairMovementOperation][ModifyGraphicObject]: "
//            //                <<"Failed to dynamic cast IGraphicObjects.";
//            //            return false;
//            //        }
//            //        spMprGO->Translate(-vTranslate);
//            //        mat = spMprGO->GetModel2WorldMatrix();
//            //        mat.Prepend(Mcsf::MedViewer3D::Matrix4x4(myRotate));
//            //        spMprGO->SetModel2WorldMatrix(mat);
//            //        spMprGO->Translate(vTranslate);
//            //        spMprGO->SetDirty(true);
//            //    }
//            //}
//        }
//
//        goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, CORONAL);
//        strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//        go = mModelWarehouse->GetModelObject(strKey);
//        auto spCoronalMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
//        if (spCoronalMprGO == nullptr){
//            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairRotateOperation][ModifyGraphicObject]: "
//                <<"Failed to dynamic cast IGraphicObjects.";
//            return false;
//        }
//        Mcsf::MedViewer3D::Vector3D normalCoronal;
//        spCoronalMprGO->GetNormal(&normalCoronal);
//        normalCoronal.Normalize();
//
//        goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, SAGITTAL);
//        strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//        go = mModelWarehouse->GetModelObject(strKey);
//        auto spSagittalMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
//        if (spSagittalMprGO == nullptr){
//            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairRotateOperation][ModifyGraphicObject]: "
//                <<"Failed to dynamic cast IGraphicObjects.";
//            return false;
//        }
//        Mcsf::MedViewer3D::Vector3D normalSagittal;
//        spSagittalMprGO->GetNormal(&normalSagittal);
//        normalSagittal.Normalize();
//
//        goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, AXIAL);
//        strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//        go = mModelWarehouse->GetModelObject(strKey);
//        auto spAxialMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
//        if (spAxialMprGO == nullptr){
//            TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryCrosshairRotateOperation][ModifyGraphicObject]: "
//                <<"Failed to dynamic cast IGraphicObjects.";
//            return false;
//        }
//        Mcsf::MedViewer3D::Vector3D normalAxial;
//        spAxialMprGO->GetNormal(&normalAxial);
//        normalAxial.Normalize();
//
//        // TODO Use i= nSelectedXY to replace For Condition
//        //      Rotate X or Y Only affect One Section;
//        for (int i = 0; i < sizeof(strMprXY) / sizeof(strMprXY[0]); i++) {
//            goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, strMprXY[i]);
//            strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//            go = mModelWarehouse->GetModelObject(strKey);
//            spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
//            if (spCrosshairGO == nullptr){
//                TPS_LOG_DEV_ERROR<<"[ERROR][CrosshairMovementOperation][ModifyGraphicObject]: "
//                    <<"Failed to dynamic cast IGraphicObjects.";
//                return false;
//            }
//
//            goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, strMprXY[i]);
//            strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
//            go = mModelWarehouse->GetModelObject(strKey);
//            spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
//            if (spMprGO == nullptr){
//                TPS_LOG_DEV_ERROR<<"[ERROR][CrosshairMovementOperation][ModifyGraphicObject]: "
//                    <<"Failed to dynamic cast IGraphicObjects.";
//                return false;
//            }
//
//            //  Unit : degree
//            //                    /|\ y
//            //                     |
//            //          90->180    |   0->90
//            //        _____________|_____________\
//            //                     |             /  x
//            //         180->270    |  270->360
//            //                     |
//
//            if (strMprXY[i] == CORONAL)
//            {
//                // coronal 法线从后向前, axial 法线从头到脚
//                Mcsf::MedViewer3D::Vector3D vecX = normalCoronal.CrossProduct(normalAxial);
//
//                Mcsf::MedViewer3D::Point2D pt1OnScreen = spMprGO->GetPlaneCoordinate(pt3dCrosshairCenter, false);
//                Mcsf::MedViewer3D::Point2D pt2OnScreen = spMprGO->GetPlaneCoordinate(pt3dCrosshairCenter + vecX, false);
//                double dDeltaX = pt2OnScreen.x - pt1OnScreen.x;
//                double dDeltaY = pt2OnScreen.y - pt1OnScreen.y;
//                double dAngleX = std::atan2(dDeltaY, dDeltaX) * 180.0 / PI;
//                if (dAngleX < 0.0) dAngleX += 360.0;
//
//                Mcsf::MedViewer3D::Vector3D vecY = normalCoronal.CrossProduct(normalSagittal);
//
//                pt1OnScreen = spMprGO->GetPlaneCoordinate(pt3dCrosshairCenter, false);
//                pt2OnScreen = spMprGO->GetPlaneCoordinate(pt3dCrosshairCenter + vecY, false);
//                dDeltaX = pt2OnScreen.x - pt1OnScreen.x;
//                dDeltaY = pt2OnScreen.y - pt1OnScreen.y;
//                double dAngleY = std::atan2(dDeltaY, dDeltaX) * 180.0 / PI;
//                if (dAngleY < 0.0) dAngleY += 360.0;
//                spCrosshairGO->SetAngles(dAngleX, dAngleY);
//            }
//            else if (strMprXY[i] == SAGITTAL)
//            {
//                // coronal 法线从后向前, axial 法线从头到脚
//                Mcsf::MedViewer3D::Vector3D vecX = normalSagittal.CrossProduct(normalAxial);
//
//                Mcsf::MedViewer3D::Point2D pt1OnScreen = spMprGO->GetPlaneCoordinate(pt3dCrosshairCenter, false);
//                Mcsf::MedViewer3D::Point2D pt2OnScreen = spMprGO->GetPlaneCoordinate(pt3dCrosshairCenter + vecX, false);
//                double dDeltaX = pt2OnScreen.x - pt1OnScreen.x;
//                double dDeltaY = pt2OnScreen.y - pt1OnScreen.y;
//                double dAngleX = std::atan2(dDeltaY, dDeltaX) * 180.0 / PI;
//                if (dAngleX < 0.0) dAngleX += 360.0;
//
//                Mcsf::MedViewer3D::Vector3D vecY = -normalSagittal.CrossProduct(normalCoronal);
//
//                pt1OnScreen = spMprGO->GetPlaneCoordinate(pt3dCrosshairCenter, false);
//                pt2OnScreen = spMprGO->GetPlaneCoordinate(pt3dCrosshairCenter + vecY, false);
//                dDeltaX = pt2OnScreen.x - pt1OnScreen.x;
//                dDeltaY = pt2OnScreen.y - pt1OnScreen.y;
//                double dAngleY = std::atan2(dDeltaY, dDeltaX) * 180.0 / PI;
//                if (dAngleY < 0.0) dAngleY += 360.0;
//                spCrosshairGO->SetAngles(dAngleX, dAngleY);
//            }
//            else if (strMprXY[i] == AXIAL)
//            {
//                // coronal 法线从后向前, axial 法线从头到脚
//                Mcsf::MedViewer3D::Vector3D vecX = -normalAxial.CrossProduct(normalCoronal);
//
//                Mcsf::MedViewer3D::Point2D pt1OnScreen = spMprGO->GetPlaneCoordinate(pt3dCrosshairCenter, false);
//                Mcsf::MedViewer3D::Point2D pt2OnScreen = spMprGO->GetPlaneCoordinate(pt3dCrosshairCenter + vecX, false);
//                double dDeltaX = pt2OnScreen.x - pt1OnScreen.x;
//                double dDeltaY = pt2OnScreen.y - pt1OnScreen.y;
//                double dAngleX = std::atan2(dDeltaY, dDeltaX) * 180.0 / PI;
//                if (dAngleX < 0.0) dAngleX += 360.0;
//
//                Mcsf::MedViewer3D::Vector3D vecY = normalAxial.CrossProduct(normalSagittal);
//
//                pt1OnScreen = spMprGO->GetPlaneCoordinate(pt3dCrosshairCenter, false);
//                pt2OnScreen = spMprGO->GetPlaneCoordinate(pt3dCrosshairCenter + vecY, false);
//                dDeltaX = pt2OnScreen.x - pt1OnScreen.x;
//                dDeltaY = pt2OnScreen.y - pt1OnScreen.y;
//                double dAngleY = std::atan2(dDeltaY, dDeltaX) * 180.0 / PI;
//                if (dAngleY < 0.0) dAngleY += 360.0;
//                spCrosshairGO->SetAngles(dAngleX, dAngleY);
//            }
//            spCrosshairGO->SetDirty(true);
//        }
//    }
//    catch(std::exception& ex){
//        TPS_LOG_DEV_ERROR<<"CrosshairHitTestOperation::ModifyGraphicObject:"<<ex.what();
//        return false;
//    }
//    return true;
//}


bool TpsGalleryCrosshairRotateOperation::ModifyGraphicObject() const {

    try{
        auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, m_strSectionType);
        std::string strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
        auto go = mModelWarehouse->GetModelObject(strKey);
        auto spCrosshairGOSection = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
        if (spCrosshairGOSection == nullptr){
            TPS_LOG_DEV_ERROR << "[ERROR][TpsGalleryCrosshairRotateOperation][ModifyGraphicObject]: "
                << "Failed to dynamic cast IGraphicObjects.";
            return false;
        }

        goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, m_strSectionType);
        strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
        go = mModelWarehouse->GetModelObject(strKey);
        auto spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
        if (spMprGO == nullptr){
            TPS_LOG_DEV_ERROR << "[ERROR][TpsGalleryCrosshairRotateOperation][ModifyGraphicObject]: "
                << "Failed to dynamic cast IGraphicObjects.";
            return false;
        }

        strKey = m_strImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
        go = mModelWarehouse->GetModelObject(strKey);
        auto spVolumeGO = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(go);
        if (spVolumeGO == nullptr){
            TPS_LOG_DEV_ERROR << "[ERROR][TpsGalleryCrosshairRotateOperation][ModifyGraphicObject]: "
                << "Failed to dynamic cast IGraphicObjects.";
            return false;
        }

        Mcsf::MedViewer3D::Vector3D rotateNormal;
        spMprGO->GetNormal(&rotateNormal);
        //计算出旋转中心
        Mcsf::MedViewer3D::Point3D rotateCenter = spMprGO->PlaneCoordinateToWorldCoordiate(m_dXRotateCenter, m_dYRotateCenter);


        FIRST_POSTFIX_COMPONENT ucMprXY[2];
        if (m_strSectionType == AXIAL) {
            ucMprXY[0] = CORONAL;
            ucMprXY[1] = SAGITTAL;
        }
        else if (m_strSectionType == SAGITTAL) {
            ucMprXY[0] = AXIAL;
            ucMprXY[1] = CORONAL;
        }
        else if (m_strSectionType == CORONAL) {
            ucMprXY[0] = AXIAL;
            ucMprXY[1] = SAGITTAL;
        }


        //CrosshairGraphicObject::HITTED_MODE  hitMode = (CrosshairGraphicObject::HITTED_MODE)m_hitMode;
        int nSelectedXY = 0;
        if (m_hitMode == CrosshairGraphicObject::HITTED_AXIS_X_Left || m_hitMode == CrosshairGraphicObject::HITTED_AXIS_X_Right) {
            nSelectedXY = 0;
        }
        else if (m_hitMode == CrosshairGraphicObject::HITTED_AXIS_Y_Top || m_hitMode == CrosshairGraphicObject::HITTED_AXIS_Y_Bottom) {
            nSelectedXY = 1;
        }
        else {
            return false;
        }
        // New angle

        Mcsf::MedViewer3D::Point2D startPoint;
        Mcsf::MedViewer3D::Point3D topPoint, bottomPoint, leftPoint, rightPoint;
        spCrosshairGOSection->GetCrossFourPoints(&topPoint, &bottomPoint, &leftPoint, &rightPoint);
        if (nSelectedXY == 0)
        {
            startPoint = spMprGO->GetPlaneCoordinate((rightPoint + leftPoint)*0.5, false);
        }
        else
        {
            startPoint = spMprGO->GetPlaneCoordinate((topPoint + bottomPoint)*0.5, false);
        }
        double dAngleEnd = atan2(m_dYEnd - m_dYRotateCenter, m_dXEnd - m_dXRotateCenter);
        double dAngleStart = atan2(m_dYStart - m_dYRotateCenter, m_dXStart - m_dXRotateCenter);

        double dDeltaAngle = dAngleEnd - dAngleStart;


        Mcsf::MedViewer3D::Quaternion myRotate(dDeltaAngle, rotateNormal);

        goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, ucMprXY[nSelectedXY]);
        strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
        go = mModelWarehouse->GetModelObject(strKey);
        spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
        if (spMprGO == nullptr){
            TPS_LOG_DEV_ERROR << "[ERROR][TpsGalleryCrosshairRotateOperation][ModifyGraphicObject]: "
                << "Failed to dynamic cast IGraphicObjects.";
            return false;
        }

        // Move the center of object to the origin of the world coordinate

        Mcsf::MedViewer3D::Vector3D vTranslate = Mcsf::MedViewer3D::Vector3D(rotateCenter.x, rotateCenter.y, rotateCenter.z);
        spMprGO->Translate(-vTranslate);
        // Roate
        Mcsf::MedViewer3D::Matrix4x4 mat = spMprGO->GetModel2WorldMatrix();
        mat.Prepend(Mcsf::MedViewer3D::Matrix4x4(myRotate));
        spMprGO->SetModel2WorldMatrix(mat);
        // Move the center of the object back 
        spMprGO->Translate(vTranslate);
        //spMprGO->SetRotate(myRotate); // no crosshair center in mprgo. !!!

        //获取MPR 的vertices，调整vertices的位置是图形中心为vertices中心
        size_t dim[3];
        spVolumeGO->GetDimension(dim);
        Mcsf::MedViewer3D::Matrix4x4 modelToVolume(dim[0], 0, 0, 0,
            0, dim[1], 0, 0,
            0, 0, dim[2], 0,
            -0.5, -0.5, -0.5, 1);
        //Mcsf::MedViewer3D::Matrix4x4 matWorld2Volume = modelToVolume * spVolumeGO->GetModel2World().Inverse();

        //Mcsf::MedViewer3D::Matrix4x4 matWorld2Patient = spVolumeGO->GetWorld2Patient();
        //Mcsf::MedViewer3D::Matrix4x4 matVolume2Patient = matWorld2Patient * matWorld2Volume.Inverse();


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
            Mcsf::MedViewer3D::Point2D temp = spMprGO->GetPlaneCoordinate(newIntersects[0], false);
            maxX = minX = temp.x;
            maxY = minY = temp.y;
            for (int i = 1; i < newIntersects.size(); i++)
            {
                Mcsf::MedViewer3D::Point2D insectPoint = spMprGO->GetPlaneCoordinate(newIntersects[i], false);
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
            Mcsf::MedViewer3D::Quaternion myRotateBack(-dDeltaAngle, rotateNormal);

            spMprGO->Translate(-vTranslate);
            Mcsf::MedViewer3D::Matrix4x4 mat = spMprGO->GetModel2WorldMatrix();
            mat.Prepend(Mcsf::MedViewer3D::Matrix4x4(myRotateBack));
            spMprGO->SetModel2WorldMatrix(mat);
            // Move the center of the object back 
            spMprGO->Translate(vTranslate);
        }

        /* Mcsf::MedViewer3D::Point3D vertices[4];
        spMprGO->GetVertices(vertices);
        Mcsf::Point3f verticesF[4];
        Mcsf::MedViewer3D::Matrix4x4 matW2V = spVolumeGO->GetModel2World().Inverse();
        matW2V.Prepend(modelToVolume);
        for (int i = 0; i < 4; ++i){
        verticesF[i] = TpsArithmeticConverter::ConvertToPoint3f(
        matW2V.Transform(vertices[i]));
        }

        Mcsf::Point3f boundingRectInVolume[4];
        Mcsf::Matrix4f matVolumeToNormalPlane;
        SubVolumeHelperAPI::GetBoundingRectangle(dim, verticesF, boundingRectInVolume, matVolumeToNormalPlane);

        Mcsf::MedViewer3D::Point3D newVertices[4];
        for (int i = 0; i < 4; ++i){
        newVertices[i] = matW2V.Inverse().Transform(TpsArithmeticConverter::ConvertToPoint3D(boundingRectInVolume[i]));
        }

        Mcsf::MedViewer3D::Point3D verticesCenter = (vertices[0] + vertices[2]) * 0.5;
        Mcsf::MedViewer3D::Point3D verticesNewCenter = (newVertices[0] + newVertices[2]) * 0.5;

        Mcsf::MedViewer3D::Vector3D offSet = verticesNewCenter - verticesCenter;

        spMprGO->Translate(offSet);*/

        spMprGO->SetDirty(true);
    }
    catch (std::exception& ex){
        TPS_LOG_DEV_ERROR << "TpsGalleryCrosshairRotateOperation::ModifyGraphicObject:" << ex.what();
        return false;
    }
    return true;
}

TPS_END_NAMESPACE  // end namespace tps
