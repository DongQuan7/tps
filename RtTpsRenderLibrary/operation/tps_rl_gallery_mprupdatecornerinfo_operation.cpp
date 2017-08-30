//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  shaosen.huang shaosen.huang@united-imaging.com
//
/// \file    tps_rl_gallery_mprupdatecornerinfo_operation.cpp
///
/// \brief   
/// 
/// \version 1.0
/// 
/// \date    2015/04/10
/////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_mprupdatecornerinfo_operation.h"
#include "RtTpsRenderLibrary/tps_rl_update_corner_info_operation.h"

//ZHENGHE
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_suv_calculator.h"

//TPS
#include "RtTpsFramework/tps_fw_modelwarehouse.h" 
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_dummygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"

TPS_BEGIN_NAMESPACE
    using namespace Mcsf::MedViewer3D;
void UpdateSUVTextInGallery(float, std::shared_ptr<TpsTextOverlayGraphicObject>);

TpsGalleryMPRUpdateCornerTextInfoOperation::TpsGalleryMPRUpdateCornerTextInfoOperation(
    double x, double y): 
    m_dX(x), m_dY(y){

}

TpsGalleryMPRUpdateCornerTextInfoOperation::~TpsGalleryMPRUpdateCornerTextInfoOperation(){

}

void TpsGalleryMPRUpdateCornerTextInfoOperation::SetGraphicObjectKeys(const std::string& sVolumeKey, 
    const std::string& sMPRKey, const std::string &mprCornerInfoKey, const std::string& sDummyGokey) {
        m_strVolumeKey = sVolumeKey;
        m_strMPRKey = sMPRKey;
        m_strCornerInfoKey = mprCornerInfoKey;
        m_strDummyGoKey = sDummyGokey;
}

void TpsGalleryMPRUpdateCornerTextInfoOperation::SetSuvType(int suvUnitType)
{
    m_iSUVUnitType = suvUnitType;
}

bool TpsGalleryMPRUpdateCornerTextInfoOperation::ModifyGraphicObject() const {


    auto go = mModelWarehouse->GetGraphicObject(m_strVolumeKey);
    auto spVolumeGO = std::dynamic_pointer_cast<GraphicObjVolume>(go);
    if (spVolumeGO == nullptr) {
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryMPRUpdateCornerTextInfoOperation][ModifyGraphicObject]: "
            <<"Failed to get volume GO.";
        return false;
    }

    go = mModelWarehouse->GetGraphicObject(m_strMPRKey);
    auto spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (spMprGO == nullptr) {
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryMPRUpdateCornerTextInfoOperation][ModifyGraphicObject]: "
            <<"Failed to get MPR GO.";
        return false;
    }

	TpsImage3DEntity* image = nullptr;
	if (!mDataRepository->GetGalleryImageManager()->GetImage3DData(spVolumeGO->GetSeriesUID(), &image) || image == nullptr)
	{
		TPS_LOG_DEV_ERROR<<"Failed to get image 3d entity.";
		return false;
	}

    Mcsf::MedViewer3D::Point2D ptInPlane(m_dX, m_dY/*fX, fY*/);
    Mcsf::MedViewer3D::Point3D ptInWorld = spMprGO->GetWorldCoordinate(ptInPlane);
    Mcsf::MedViewer3D::Point3D ptInPatient = spVolumeGO->GetWorld2Patient().Transform(ptInWorld);
    mPositionX = ptInPatient.x; mPositionY = ptInPatient.y; mPositionZ = ptInPatient.z;
    //printf("Mouse pos: x = %f y = %f z = %f\n", ptInPatient.x, ptInPatient.y, ptInPatient.z);
    Mcsf::MedViewer3D::Point3D ptInVolume;

    Mcsf::MedViewer3D::Point3D ptInVolumeModel = spVolumeGO->GetModel2World().Inverse().Transform(ptInWorld);
    size_t volumeDim[3];
    spVolumeGO->GetDimension(volumeDim);
    ptInVolume.x = ptInVolumeModel.x *(volumeDim[0]) -0.5;
    ptInVolume.y = ptInVolumeModel.y *(volumeDim[1]) -0.5;
    ptInVolume.z = ptInVolumeModel.z * (volumeDim[2]) -0.5;

    spMprGO->GetWindowing(mWindowWidth, mWindowCenter);
    spMprGO->GetScale(&mZoomFactor);

    mValue = TPS_NEGATIVE_INF;
    mIsValueValid = true;
    spVolumeGO->GetPixelValue((int)ptInVolume.x, (int)ptInVolume.y, (int)ptInVolume.z, &mValue);


    mModality = spVolumeGO->GetModality();
    if(mModality == "CT"){
        //double density = 0.f;
        int isOverride = -1;    // 0: 被人工设置density;   -1: 未设置;
        float densityOverride = 0.0;

        isOverride = GetVoiOverrideDensity_i(mSeriesUID, ptInPatient.x, ptInPatient.y, ptInPatient.z, &densityOverride);

        mDensity = TPS_NEGATIVE_INF;
        if (isOverride == (int)TPS_ER_SUCCESS) {
            // 该点被人工设置density
            mDensity = densityOverride;
        }
        else {
            //// TODO not use TpsImageDataManager ? 
            //if (nullptr != mDataRepository->GetImageDataManager()->GetCurrentCT2DensityArray()){
            //    //density = mDataRepository->GetImageDataManager()->CT2Density(rawPixel);
            //}
            // To do Dyx
            mDensity = mDataRepository->GetImageDataManager()->CT2DensityByImageUid(mValue, mSeriesUID);
        }

        //printf("Mouse in world x=%f y=%f z=%f CT = %f density = %f override = %d\n", ptInWorld.x, ptInWorld.y, ptInWorld.z, rawPixel, density, isOverride);

        //ConvertCTAndDensityValueToImageText(rawPixel, density, mprCornerInfo);
    }
    else if(mModality == "PT"){           
        float slope = spVolumeGO->GetSlope();
        float intercept = spVolumeGO->GetIntercept();
        float rawPixel = (mValue - intercept)/slope;

        auto data_header_element = mDataRepository->GetGalleryImageManager()->GetCurrentDataHeaderElement();
        if (data_header_element == nullptr){
            TPS_LOG_DEV_ERROR<<"The data header is empty.";
            return false;
        }

        Mcsf::MedViewer3D::GraphicObjectSUVCalculator suv_calc;
        suv_calc.GetSUVParamFromImageHeader(data_header_element);

        //std::unique_ptr<float[]> uptr_rawpixel(new float[1]);
        //uptr_rawpixel[0] = static_cast<float>(rawPixel);

        //float suvValue;
        //try{
        //    if(suv_calc.IsSUVBodyWeightExist()){
        //        suv_calc.SetSUVCalculateType(BODYWEIGHT);
        //    }
        //    else if(suv_calc.IsConcentrationExist()){
        //        suv_calc.SetSUVCalculateType(CONCENTRATION);
        //    }
        //    else{
        //        suv_calc.SetSUVCalculateType(NOSUVVALUE);
        //    }
        //    auto up_petsuv = suv_calc.GetPETSUV(uptr_rawpixel, 1);
        //    suvValue = up_petsuv[0];
        //    UpdateSUVTextInGallery(suvValue, mprCornerInfo);
        //}
        //catch(std::exception ex){

        //}

        std::unique_ptr<float[]> uptr_rawpixel(new float[3]);
        uptr_rawpixel[0] = static_cast<float>(rawPixel);
        uptr_rawpixel[1] = static_cast<float>(mWindowCenter - 0.5 * mWindowWidth);//T
        uptr_rawpixel[2] = static_cast<float>(mWindowCenter + 0.5 * mWindowWidth);//B
        std::unique_ptr<float[]> petSuv;
        auto suvType = (Viewer3DSUVCalculateType)m_iSUVUnitType;
        suv_calc.SetSUVCalculateType(suvType);
        switch(suvType){
        case CONCENTRATION:
            if (!suv_calc.IsConcentrationExist()) 
                mIsValueValid = false;
            else {
                petSuv = suv_calc.GetPETSUV(uptr_rawpixel, 3);
            }
            break;
        case BODYWEIGHT:
            if (!suv_calc.IsSUVBodyWeightExist()) 
                mIsValueValid = false;
            else {
                petSuv = suv_calc.GetPETSUV(uptr_rawpixel, 3);
            }
            break;
        case LEANBODYMASS:
            if (!suv_calc.IsSUVLeanBodyMassExist()) 
                mIsValueValid = false;
            else {
                petSuv = suv_calc.GetPETSUV(uptr_rawpixel, 3);
            }
            break;
        case BODYSURFACEAREA:
            if (!suv_calc.IsSUVBodySurfaceAreaExist()) 
                mIsValueValid = false;
            else {
                petSuv = suv_calc.GetPETSUV(uptr_rawpixel, 3);
            }
            break;
        default:
            mIsValueValid = false;
        }

        if (mIsValueValid){
            mValue = petSuv[0];
            mWindowCenter = (petSuv[1] + petSuv[2]) / 2;
            mWindowWidth = fabs(petSuv[2] - petSuv[1]);
        }
        
    }
    else if(mModality == "MR"){

    }

    //ConvertPtInPatientToImageText(&ptInPatient, mprCornerInfo);
        // 变换到mm单位
    /*mSliceThickness = spMprGO->GetThickness();
    Mcsf::MedViewer3D::Point3D thickness3D(mSliceThickness,0,0); 
    mSliceThickness = spVolumeGO->GetWorld2Patient().Transform(thickness3D).x;*/
    
    //modify by wzd Point3D-->Vector3D
    mSliceThickness = spMprGO->GetThickness();
    Mcsf::MedViewer3D::Vector3D thickness3D(0.0, 0.0, mSliceThickness); 
    mSliceThickness = spVolumeGO->GetWorld2Patient().Transform(thickness3D).z;

    TpsMprHelper::PlaneTypeInVol planeType;
    TpsMprHelper::GetSliceNumber(spVolumeGO, spMprGO, 
        mSliceIndex, mTotalSliceNum, planeType);
    mStartSliceNumber = planeType == TpsMprHelper::Z ? image->GetStartSliceNumber() : 1;

    mRotate = 0;
    return true;
}



int TpsGalleryMPRUpdateCornerTextInfoOperation::GetVoiOverrideDensity_i( const std::string& seriesUid,
    float fXInPat, float fYInPat, float fZInPat, float* fDensity) const
{
    if (seriesUid.empty())
    {
        TPS_LOG_DEV_ERROR << "seriesUid is empty.";
        return TPS_ER_FAILURE;
    }

    PT3 pt(fXInPat, fYInPat, fZInPat);
    vector<ROIEntity*> voiList = mDataRepository->GetVoiManager()->GetROIBySeries(seriesUid);
    int size = voiList.size();
    for (int i = 0; i  < size ; i++)
    {
        if (voiList[i] != nullptr)
        {
            ROIEntity* pVOI = voiList[i];
            if (nullptr == pVOI || nullptr == pVOI->GetInnerRoi())
            {
                return TPS_ER_FAILURE;
            }
            auto innerRoi = pVOI->GetInnerRoi();

            if (pVOI->IsInROI(pt) && innerRoi->get_isoverride())
            {
                *fDensity = innerRoi->get_overridedenstiy();
                return TPS_ER_SUCCESS;
            }
        }
    }
    return TPS_ER_FAILURE;
}

void UpdateSUVTextInGallery(float suvVal, std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo){
    char txt_suv[64]={0};
    if(suvVal < 0.01){
        _snprintf_s(txt_suv, sizeof(txt_suv), "%.1f", suvVal);
    }else{
        _snprintf_s(txt_suv, sizeof(txt_suv), "%.2f", suvVal);
    }
    if(txt_suv[sizeof(txt_suv) - 1] != '\0'){
        txt_suv[sizeof(txt_suv)-1]='\0';
    }
    Pixel32* pStringBuffer = nullptr;
    int strWidth, strHeight;
    FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
    std::wstringstream wssStream; 

    wssStream.clear();
    wssStream.str(L"");
    wssStream << txt_suv;
    if (!pBufGen->GenerateRasterBuffer(wssStream.str(), &pStringBuffer, strWidth, strHeight)) {
        TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for SUV value.\n";
        return;
    }
    mprCornerInfo->UpdateTextBlock("PETVal", (unsigned char*)pStringBuffer, strWidth, strHeight);

    if(pStringBuffer!=nullptr){
        delete[] pStringBuffer;
        pStringBuffer = nullptr;
    }
}

bool TpsGalleryMPRUpdateCornerTextInfoOperation::ConvertPtInPatientToImageText(Mcsf::MedViewer3D::Point3D *ptInPatient,
    std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo) const{

        const std::string position_key_val[3] = {
            "PositionXVal",
            "PositionYVal",
            "PositionZVal"
        };
        char strtext_x[64]={0};
        _snprintf_s(strtext_x, sizeof(strtext_x), "%.1f", 0.1 * ptInPatient->x);
        if(strtext_x[sizeof(strtext_x) - 1] != '\0'){
            //printf("warning:string will be truncated");
            strtext_x[sizeof(strtext_x)-1]='\0';
        }
        char strtext_y[64]={0};
        _snprintf_s(strtext_y, sizeof(strtext_y), "%.1f", 0.1 * ptInPatient->y);
        if(strtext_y[sizeof(strtext_y) - 1] != '\0'){
            //printf("warning:string will be truncated");
            strtext_y[sizeof(strtext_y)-1]='\0';
        }
        char strtext_z[64]={0};
        _snprintf_s(strtext_z, sizeof(strtext_z), "%.1f", 0.1 * ptInPatient->z);
        if(strtext_z[sizeof(strtext_z) - 1] != '\0'){
            //printf("warning:string will be truncated");
            strtext_z[sizeof(strtext_z)-1]='\0';
        }

        FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
        Pixel32* pStrBuffer = nullptr;
        int iStrWidth, iStrHeight;
        std::wstringstream wssStream;

        for(int i = 0; i < 3; ++i){
            wssStream.clear();
            wssStream.str(L"");
            wssStream << (i == 0 ? strtext_x : (i == 1 ? strtext_y : strtext_z));
            if (!pBufGen->GenerateRasterBuffer(wssStream.str(), &pStrBuffer, iStrWidth, iStrHeight)) {
                TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer.\n";
                return false;
            }
            mprCornerInfo->UpdateTextBlock(position_key_val[i], (unsigned char*)pStrBuffer, iStrWidth, iStrHeight);
            if(pStrBuffer!=nullptr){
                delete[] pStrBuffer;
                pStrBuffer = nullptr;
            }
        }
        return true;
}

bool TpsGalleryMPRUpdateCornerTextInfoOperation::ConvertCTAndDensityValueToImageText(double ct, double density,
    std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo) const{
        //if(fabs(density - 1.0) < FLT_EPSILON)
        //    return false;

        const std::string ct_density_key_val[2]={
            "CTVal",
            "DensityVal"
        };

        char strtextCT[64]={0};
        _snprintf_s(strtextCT, sizeof(strtextCT), "%.1f", ct);
        if(strtextCT[sizeof(strtextCT) - 1] != '\0'){
            //printf("warning:string will be truncated");
            strtextCT[sizeof(strtextCT)-1] = '\0';
        }

        char strtextDensity[64]={0};
        if(density < 0.01f){
            _snprintf_s(strtextDensity, sizeof(strtextDensity), "%.1f", density);
        }
        else{
            _snprintf_s(strtextDensity, sizeof(strtextDensity), "%.2f", density);
        }
        if(strtextDensity[sizeof(strtextDensity) - 1] != '\0'){
            //printf("warning:string will be truncated");
            strtextDensity[sizeof(strtextDensity)-1] = '\0';
        }

        Pixel32* pStringBuffer = nullptr;
        int iStrWidth, iStrHeight;
        FTStringBufferGenerator* pBufGen = FTStringBufferGenerator::GetInstance();
        std::wstringstream wssStream; 

        for(int i = 0; i<2; ++i){
            wssStream.clear();
            wssStream.str(L"");

            wssStream<<(i == 0 ? strtextCT : strtextDensity);
            if (!pBufGen->GenerateRasterBuffer(wssStream.str(), &pStringBuffer, iStrWidth, iStrHeight)) {
                TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer.\n";
                return false;
            }

            mprCornerInfo->UpdateTextBlock(ct_density_key_val[i], (unsigned char*)pStringBuffer, iStrWidth, iStrHeight);
            if(pStringBuffer!=nullptr){
                delete[] pStringBuffer;
                pStringBuffer = nullptr;
            }
        }

        return true;
}

TPS_END_NAMESPACE
