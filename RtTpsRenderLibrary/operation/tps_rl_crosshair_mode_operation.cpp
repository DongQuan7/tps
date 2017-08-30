//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  LI Song song.li@united-imaging.com
///
///  \file    tps_rl_crosshair_mode_operation.cpp
///  \brief   Crosshair mode implementation
///
///  \version 1.0
///  \date    Jul. 10, 2014
///  
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

// TPS
#include "RtTpsRenderLibrary/tps_rl_crosshair_mode_operation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

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

CrosshairModeOperation::CrosshairModeOperation(std::string& strImageUID, int nCrosshairMode, bool bVisible)
    :m_strImageUID(strImageUID), 
     m_nCrosshairMode(nCrosshairMode),
     m_bVisible(bVisible) {
}

CrosshairModeOperation::~CrosshairModeOperation(){

}

bool CrosshairModeOperation::ModifyGraphicObject() const {

    try{
        bool bVisibilityChanged = false;
        bool bCrosshairAngleChanged = false;

        FIRST_POSTFIX_COMPONENT strMprSection[3] = {AXIAL, CORONAL, SAGITTAL};
        std::shared_ptr<CrosshairGraphicObject> spCrosshairGO[3] = {nullptr, nullptr, nullptr};
        std::shared_ptr<MPRGraphicObject> spMprGO[3] = {nullptr, nullptr, nullptr};

        std::string strKey;
        std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> go;
        int i;
        GO_TYPE goType;
        for (i = 0; i < sizeof(strMprSection) / sizeof(strMprSection[0]); i++) {
            goType = GOTypeHelper::ComposeAsGOType(CROSSHAIR, strMprSection[i]);
            strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
            go = mModelWarehouse->GetModelObject(strKey);
            spCrosshairGO[i] = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
            if (spCrosshairGO[i] == nullptr){
                TPS_LOG_DEV_ERROR<<"[ERROR][CrosshairModeOperation][ModifyGraphicObject]: "
                    <<"Failed to dynamic cast IGraphicObjects.";
                return false;
            }

            goType = GOTypeHelper::ComposeAsGOType(MPR, strMprSection[i]);
            strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
            go = mModelWarehouse->GetModelObject(strKey);
            spMprGO[i] = std::dynamic_pointer_cast<MPRGraphicObject>(go);
            if (spMprGO[i] == nullptr){
                TPS_LOG_DEV_ERROR<<"[ERROR][CrosshairModeOperation][ModifyGraphicObject]: "
                    <<"Failed to dynamic cast IGraphicObjects.";
                return false;
            }
        }

        if (spCrosshairGO[0]->Visible() != m_bVisible)
        {
            spCrosshairGO[0]->SetVisible(m_bVisible);
            spCrosshairGO[0]->SetDirty(true);
            spCrosshairGO[1]->SetVisible(m_bVisible);
            spCrosshairGO[1]->SetDirty(true);
            spCrosshairGO[2]->SetVisible(m_bVisible);
            spCrosshairGO[2]->SetDirty(true);
            bVisibilityChanged = true;
        }
        CrosshairGraphicObject::CROSSHAIR_MODE nOldCrosshairMode;
        spCrosshairGO[0]->GetCrosshairMode(&nOldCrosshairMode);
        if (m_nCrosshairMode == nOldCrosshairMode) {
            // crosshair mode remains unchanged
        }
        else if (nOldCrosshairMode == CrosshairGraphicObject::CROSSHAIR_ORTHOGONAL) {
            // orthogonal mode -> oblique
            // save the MPR normal in orthogonal mode
            //Mcsf::MedViewer3D::Vector3D normal;
            for (i = 0; i < sizeof(strMprSection) / sizeof(strMprSection[0]); i++) {
                //spMprGO[i]->GetNormal(&normal);
                //spCrosshairGO[i]->SetNormalInOrtho(normal);
                //Mcsf::MedViewer3D::Point3D vertices[4];
                //spMprGO[i]->GetVertices(vertices);
                //Mcsf::MedViewer3D::Vector3D v3dXInOrtho = vertices[1] - vertices[0];
                //spCrosshairGO[i]->SetXInOrtho(v3dXInOrtho);
                spCrosshairGO[i]->SetCrosshairMode((CrosshairGraphicObject::CROSSHAIR_MODE)m_nCrosshairMode);
            }
            bCrosshairAngleChanged = false;
        }
        else {
            // oblique mode -> orthogonal mode, we will reset crosshair angles to 0/90 degree
            strKey = m_strImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
            go = mModelWarehouse->GetModelObject(strKey);
            auto spVolumeGO = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(go);
            if (spVolumeGO == nullptr){
                TPS_LOG_DEV_ERROR<<"[ERROR][CrosshairModeOperation][ModifyGraphicObject]: "
                    <<"Failed to dynamic cast IGraphicObjects.";
                return false;
            }
            size_t dim[3];
            spVolumeGO->GetDimension(dim);
            double spacing[3];
            spVolumeGO->GetSpacing(spacing);
            Mcsf::MedViewer3D::Matrix4x4 modelToVolume(dim[0],       0,      0, 0, 
                                                            0,  dim[1],      0, 0, 
                                                            0,       0, dim[2], 0, 
                                                         -0.5,    -0.5,   -0.5, 1);
            Mcsf::MedViewer3D::Matrix4x4 matWorld2Volume = modelToVolume * spVolumeGO->GetModel2World().Inverse();

            Mcsf::MedViewer3D::Matrix4x4 matM2W = spVolumeGO->GetModel2World();
            Mcsf::MedViewer3D::Matrix4x4 matP2W = spVolumeGO->GetModel2PatientMat().Inverse();
            matP2W.Prepend(matM2W);

            Mcsf::MedViewer3D::Point3D pt3dCrosshairCenter;
            spCrosshairGO[0]->GetCrosshairPoint3d(&pt3dCrosshairCenter);
            Mcsf::MedViewer3D::Point3D pt3dCrosshairCenterInVolume = matWorld2Volume.Transform(pt3dCrosshairCenter);
            double dSliceX = pt3dCrosshairCenterInVolume.x;
            double dSliceY = pt3dCrosshairCenterInVolume.y;
            double dSliceZ = pt3dCrosshairCenterInVolume.z;
            double dSliceXNew = (int)(dSliceX + 0.5);           // Adjust to exact slice
            double dSliceYNew = (int)(dSliceY + 0.5);
            double dSliceZNew = (int)(dSliceZ + 0.5);
            Mcsf::MedViewer3D::Point3D pt3dCrosshairCenterNew = matWorld2Volume.Inverse().Transform(Mcsf::MedViewer3D::Point3D(dSliceXNew, dSliceYNew, dSliceZNew));
            Mcsf::MedViewer3D::Point2D pt2dCrosshairCenterNew;

            Mcsf::MedViewer3D::Vector3D normalInOrtho;
            Mcsf::MedViewer3D::Vector3D xInOrtho;
            Mcsf::MedViewer3D::Vector3D thisNormal;
            Mcsf::MedViewer3D::Vector3D thisX;
            Mcsf::MedViewer3D::Vector3D v3dRotateAxis(0.0, 1.0, 0.0);
            double dRotateAngle = 0.0;
            Mcsf::MedViewer3D::Vector3D v3dPlaneRotateAxis(0.0, 1.0, 0.0);
            double dPlaneRotateAngle = 0.0;

            // save for axial prev and post
            Mcsf::MedViewer3D::Matrix4x4 matAxial;
            Mcsf::MedViewer3D::Vector3D normalAxial;
            //Mcsf::MedViewer3D::Vector3D v3dRotateAxisAxial(0.0, 1.0, 0.0);
            //double dRotateAngleAxial = 0.0;
            //Mcsf::MedViewer3D::Vector3D v3dPlaneRotateAxisAxial(0.0, 1.0, 0.0);
            //double dPlaneRotateAngleAxial = 0.0;
            //Mcsf::MedViewer3D::Vector3D v3dAdjustAxial(0.0, 1.0, 0.0);

            Mcsf::MedViewer3D::Vector3D v3dTranslate(pt3dCrosshairCenter.x, pt3dCrosshairCenter.y, pt3dCrosshairCenter.z);
            //Mcsf::MedViewer3D::Vector3D v3dTranslateNew(pt3dCrosshairCenterNew.x, pt3dCrosshairCenterNew.y, pt3dCrosshairCenterNew.z);
            Mcsf::MedViewer3D::Vector3D v3dOffset = pt3dCrosshairCenterNew - pt3dCrosshairCenter;
            double dDotProduct;
            Mcsf::MedViewer3D::Point3D vertices[4];
            Mcsf::MedViewer3D::Quaternion quatRotation;
            Mcsf::MedViewer3D::Matrix4x4 mat;
            for (i = 0; i < sizeof(strMprSection) / sizeof(strMprSection[0]); i++) {
                spCrosshairGO[i]->GetNormalInOrtho(&normalInOrtho);
                spCrosshairGO[i]->SetCrosshairMode((CrosshairGraphicObject::CROSSHAIR_MODE)m_nCrosshairMode);
                spCrosshairGO[i]->SetAngles(0.0f, 90.0f);
                spMprGO[i]->GetNormal(&thisNormal);
                dDotProduct = thisNormal.DotProduct(normalInOrtho);
                //  If input normal and previous plane normal are in the same direction
                //  doesn't need rotate
                if (dDotProduct > 0.9999) {
                    dRotateAngle = 0;
                }
                /// if input normal is opposite to the previous plane normal
                else if (dDotProduct < -0.9999) { 
                    spMprGO[i]->GetVertices(vertices);
                    dRotateAngle = PI;
                    v3dRotateAxis = vertices[1] - vertices[0];
                }
                else {
                    dRotateAngle = AngleBetween(thisNormal, normalInOrtho);
                    v3dRotateAxis = thisNormal.CrossProduct(normalInOrtho);
                }
                //if (i == 0) {
                //    dRotateAngleAxial = dRotateAngle;
                //    v3dRotateAxisAxial = v3dRotateAxis;
                //}

                spMprGO[i]->Translate(-v3dTranslate);
                if (dRotateAngle != 0.0) {
                    quatRotation = Mcsf::MedViewer3D::Quaternion(dRotateAngle, v3dRotateAxis);
                    mat = spMprGO[i]->GetModel2WorldMatrix();
                    mat.Prepend(Mcsf::MedViewer3D::Matrix4x4(quatRotation));
                    spMprGO[i]->SetModel2WorldMatrix(mat);
                }

                spMprGO[i]->GetVertices(vertices);
                thisX = vertices[1] - vertices[0];
                thisX.Normalize();
                spCrosshairGO[i]->GetXInOrtho(&xInOrtho);
                dDotProduct = thisX.DotProduct(xInOrtho);
                //  If input normal and previous plane normal are in the same direction
                //  doesn't need rotate
                if (dDotProduct > 0.9999) {
                    dPlaneRotateAngle = 0;
                }
                /// if input normal is opposite to the previous plane normal
                else if (dDotProduct < -0.9999) { 
                    dPlaneRotateAngle = PI;
                    v3dPlaneRotateAxis = normalInOrtho;
                }
                else {
                    dPlaneRotateAngle = AngleBetween(thisX, xInOrtho);
                    v3dPlaneRotateAxis = thisX.CrossProduct(xInOrtho);
                }
/*                if (i == 0) {
                    dPlaneRotateAngleAxial = dPlaneRotateAngle;
                    v3dPlaneRotateAxisAxial = v3dPlaneRotateAxis;
                }  */              
                if (dPlaneRotateAngle != 0.0) {
                    quatRotation = Mcsf::MedViewer3D::Quaternion(dPlaneRotateAngle, v3dPlaneRotateAxis);
                    mat = spMprGO[i]->GetModel2WorldMatrix();
                    mat.Prepend(Mcsf::MedViewer3D::Matrix4x4(quatRotation));
                    spMprGO[i]->SetModel2WorldMatrix(mat);
                }
                spMprGO[i]->Translate(v3dTranslate);
                spMprGO[i]->Translate(normalInOrtho * normalInOrtho.DotProduct(v3dOffset));
                if (i == 0) {
                //    v3dAdjustAxial = normalInOrtho * normalInOrtho.DotProduct(v3dOffset);
                    matAxial = spMprGO[i]->GetModel2WorldMatrix();
                    normalAxial = normalInOrtho;
                }
                spMprGO[i]->SetDirty(true);

                spCrosshairGO[i]->SetCrosshairPoint3d(pt3dCrosshairCenterNew);
                pt2dCrosshairCenterNew = spMprGO[i]->GetPlaneCoordinate(pt3dCrosshairCenterNew, false);
                spCrosshairGO[i]->SetCrosshairPoint2d(pt2dCrosshairCenterNew);
                spCrosshairGO[i]->SetDirty(true);
            }
        }
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"CrosshairHitTestOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }
    return true;
}


TPS_END_NAMESPACE  // end namespace tps
