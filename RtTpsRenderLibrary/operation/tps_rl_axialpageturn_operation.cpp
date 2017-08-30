//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_rl_axialpageturn_operation.cpp
///
/// \brief   AxialPageTurnOperation
/// 
/// \version 1.0
/// 
/// \date    2015/05/04
/////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_axialpageturn_operation.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"

#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

AxialPageTurnOperation::AxialPageTurnOperation(const std::string& seriesUID)
    : mSeriesUID(seriesUID), mTargetSliceNum(0) {

}

AxialPageTurnOperation::~AxialPageTurnOperation(){

}

bool AxialPageTurnOperation::ModifyGraphicObject() const{

    if (mSeriesUID.empty()){
        TPS_LOG_DEV_ERROR<<"[ERROR][MPRPageTurnOperation][ModifyGraphicObject]: "
            <<"SeriesUID is empty.";
        return false;
    }

    std::string sVolumeKey = mSeriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);

    auto goType = GOTypeHelper::ComposeAsGOType(MPR, AXIAL);
    std::string sMPRKey = mSeriesUID + "|" + GOTypeHelper::ToString(goType);

    goType = GOTypeHelper::ComposeAsGOType(MPR_CORNER_TEXT, AXIAL);
    std::string sMPRCornerInfoKey = mSeriesUID + "|" + GOTypeHelper::ToString(goType);

    auto goVolume = mModelWarehouse->GetGraphicObject(sVolumeKey);
    auto goMpr = mModelWarehouse->GetGraphicObject(sMPRKey);
    auto goMprCornerInfo = mModelWarehouse->GetGraphicObject(sMPRCornerInfoKey);

    if(goVolume == nullptr || goMpr == nullptr || goMprCornerInfo == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][MPRPageTurnOperation][ModifyGraphicObject]: "
            <<"Failed to get Graphic Object.";
        return false;
    }

    auto volume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(goVolume);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(goMpr);
    auto mprCornerInfo = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(goMprCornerInfo);

    //down cast
    if (volume == nullptr || mpr == nullptr || mprCornerInfo == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][MPRPageTurnOperation][ModifyGraphicObject]: "
            <<"Failed to dynamic cast IGraphicObjects.";
        return false;
    }

    size_t dim[3];
    volume->GetDimension(dim);
    Mcsf::MedViewer3D::Matrix4x4 modelToVolume(dim[0],       0,      0, 0, 
                                                0, dim[1],      0, 0, 
                                                0,      0, dim[2], 0, 
                                                -0.5,   -0.5,   -0.5, 1);

    Mcsf::MedViewer3D::Point3D vertices[4];
    mpr->GetVertices(vertices);
    Mcsf::MedViewer3D::Vector3D vecNormalW, vecNormal;
    mpr->GetNormal(&vecNormalW);
    vecNormalW.Normalize();

    Mcsf::MedViewer3D::Matrix4x4 matWorld2Volume = modelToVolume * volume->GetModel2World().Inverse();
    Mcsf::MedViewer3D::Point3D ptInVolume = matWorld2Volume.Transform(vertices[0]);
    vecNormal = matWorld2Volume.Transform(vecNormalW);
    vecNormal.Normalize();

    Mcsf::MedViewer3D::Matrix4x4 matpw = volume->GetModel2PatientMat().Inverse();
    matpw.Prepend(volume->GetModel2World());

    auto matP2W = volume->GetWorld2Patient().Inverse();
    double stepLen;
    TpsMprHelper::CalcSpacingInWorld(volume, mpr, stepLen);

    int sliceStep(0), realTargetNum(0), sliceTotal;
    //notice that: this method does not apply to oblique MPR case
    if(abs(abs(vecNormal.x) - 1) <= 1e-6){
        sliceTotal = dim[0];
        realTargetNum = TPS_CLAMP(mTargetSliceNum, 1, dim[0]);
        sliceStep = TPS_FLOAT_SIGN(vecNormal.x) * (mTargetSliceNum - int(ptInVolume.x  + 1.5));
    }
    else if(abs(abs(vecNormal.y) - 1) <= 1e-6){
        sliceTotal = dim[1];
        realTargetNum = TPS_CLAMP(mTargetSliceNum, 1, dim[1]);
        sliceStep = TPS_FLOAT_SIGN(vecNormal.y) * (mTargetSliceNum - int(ptInVolume.y  + 1.5));
    }
    else if(abs(abs(vecNormal.z) - 1) <= 1e-6){
        sliceTotal = dim[2];
        realTargetNum = TPS_CLAMP(mTargetSliceNum, 1, dim[2]);
        sliceStep = TPS_FLOAT_SIGN(vecNormal.z) * (mTargetSliceNum - int(ptInVolume.z  + 1.5));
    }
    else{
        TPS_LOG_DEV_WARNING<<"Can not calculate slice number of oblique mpr!";
        return false;
    }

    for (int j=0; j<4; ++j){
        vertices[j] = vertices[j] + sliceStep * stepLen * vecNormalW;
    }
    Mcsf::MedViewer3D::MPRAPI::CalculateMPRMatrix(mpr, vertices);

    //////////////////////////////////////////////////////////////////////////
    Mcsf::MedViewer3D::Point3D verticesTest[4];
    for (int j=0; j<4; ++j){
        verticesTest[j] = matWorld2Volume.Transform(vertices[j]);
    }

    double thickness = mpr->GetThickness();
    Mcsf::MedViewer3D::Vector3D vectorForThickness(0.0, 0.0, thickness);
    thickness = volume->GetWorld2Patient().Transform(vectorForThickness).z / 10; //mm->cm

    ConvertSlicePosAndThicknessToImageText_i(SLICEPOSITION_AXIAL_SECTION_VAL, SLICEPOSITION_TOTAL_AXIAL_SECTION_VAL,
        THICKNESS_AXIAL_SECTION_VAL, mTargetSliceNum, sliceTotal, thickness, mprCornerInfo);
    double posInPatient = volume->GetWorld2Patient().Transform(vertices[0]).z;
    UpdatePosInPatientImageText_i("PositionZVal", posInPatient, mprCornerInfo);

    mpr->SetDirty(true);
    return true;
}

void AxialPageTurnOperation::SetTargetSliceNum(int sliceNum) {
    mTargetSliceNum = sliceNum;
}

void AxialPageTurnOperation::UpdatePosInPatientImageText_i(const std::string& posKey, double mousePosInPat, 
    std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo) const{
        std::wstringstream wss_pos;
        char strtext_pos[64] = {0};
        _snprintf_s(strtext_pos, sizeof(strtext_pos), "%.1f", 0.1 * mousePosInPat);
        if(strtext_pos[sizeof(strtext_pos) - 1] != '\0'){
            //printf("warning:string will be truncated");
            strtext_pos[sizeof(strtext_pos)-1]='\0';
        }
        wss_pos<<strtext_pos;
        FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
        Pixel32* pStrBuffer = nullptr;
        int strWidth, strHeight;
        if(!pBufGen->GenerateRasterBuffer(wss_pos.str(), &pStrBuffer, strWidth, strHeight)){
            TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer.\n";
            delete[] pStrBuffer;
            return;
        }

        mprCornerInfo->UpdateTextBlock(posKey, (unsigned char*)pStrBuffer, strWidth, strHeight);
        delete[] pStrBuffer;

}

bool AxialPageTurnOperation::ConvertSlicePosAndThicknessToImageText_i(
    std::string sliceposValKey, std::string slicetotalValKey, std::string thicknessValKey,
    double slicepos, size_t slicetotal, double thickness, std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo) const{

        std::ostringstream oss_pos;
        oss_pos << slicepos;
        std::string str_pos = oss_pos.str();
        std::wstring wstr_pos;
        wstr_pos.assign(str_pos.begin(), str_pos.end());


        char strtext_slicetotal[64]={0};
        _snprintf_s(strtext_slicetotal, sizeof(strtext_slicetotal), "%d", (int)slicetotal);
        if(strtext_slicetotal[sizeof(strtext_slicetotal) - 1] != '\0'){
            //printf("warning:string will be truncated");
            strtext_slicetotal[sizeof(strtext_slicetotal)-1]='\0';
        }

        char strtext_thickness[64]={0};
        _snprintf_s(strtext_thickness, sizeof(strtext_thickness), "%.1f", thickness);
        if(strtext_thickness[sizeof(strtext_thickness) - 1] != '\0'){
            //printf("warning:string will be truncated");
            strtext_thickness[sizeof(strtext_thickness)-1]='\0';
        }

        std::vector<std::string> sliceNo_and_thickness_key_val;
        sliceNo_and_thickness_key_val.push_back(sliceposValKey);
        sliceNo_and_thickness_key_val.push_back(slicetotalValKey);
        sliceNo_and_thickness_key_val.push_back(thicknessValKey);

        FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
        Pixel32* pStrBuffer = nullptr;
        int strWidth, strHeight;

        for(int i = 0; i < sliceNo_and_thickness_key_val.size(); ++i){
            std::wstringstream wss;
            if(i == 0){
                wss << wstr_pos;
            }else{
                wss << (i == 1 ? strtext_slicetotal : strtext_thickness);
            }

            if(!pBufGen->GenerateRasterBuffer(wss.str(), &pStrBuffer, strWidth, strHeight)){
                TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer.\n";
                return false;
            }

            mprCornerInfo->UpdateTextBlock(sliceNo_and_thickness_key_val[i], (unsigned char*)pStrBuffer, strWidth, strHeight);

            if(pStrBuffer!=nullptr){
                delete[] pStrBuffer;
                pStrBuffer = nullptr;
            }
        }

        return true;
}
TPS_END_NAMESPACE  // end namespace tps
