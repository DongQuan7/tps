////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_volumegraphicobjectconverter.cpp
/// 
///  \brief class TpsVolumeGOConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/18
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_volumegraphicobjectconverter.h"

#include "McsfMedViewer3DDataRepresentation/image_data.h"
#include "McsfMedViewer3DDataRepresentation/data_config.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_volume_api.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"


#include "boost/lexical_cast.hpp"

//#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

using namespace Mcsf::MedViewer3D;
TPS_BEGIN_NAMESPACE   // begin namespace tps

const Mcsf::Vector3f aOrientationVectorInVolume[6] = {
        Mcsf::Vector3f(1.0,0.0,0.0),   //row
        Mcsf::Vector3f(-1.0,0.0,0.0),  //i_row
        Mcsf::Vector3f(0.0,1.0,0.0),   //column
        Mcsf::Vector3f(0.0,-1.0,0.0),  //i_column
        Mcsf::Vector3f(0.0,0.0,1.0),   //first->last
        Mcsf::Vector3f(0.0,0.0,-1.0)   //last->first
};

const Mcsf::Vector3f aOrientationVectorInPatient[3] = {
    Mcsf::Vector3f(1.0,0.0,0.0),   //R -> L
    Mcsf::Vector3f(0.0,-1.0,0.0),  //P -> A
    Mcsf::Vector3f(0.0,0.0,1.0)    //F -> H
};

TpsVolumeGOConverter::TpsVolumeGOConverter(GO_TYPE goType) 
    : TpsGOConverterBase(goType), mImage(nullptr){

}

TpsVolumeGOConverter::~TpsVolumeGOConverter(){

}

GraphicObjectPtr TpsVolumeGOConverter::CreateGraphicObject_i(){
    return std::shared_ptr<GraphicObjVolume>(new GraphicObjVolume(MEDVIEWER3D_SHORT, 1));
}

bool TpsVolumeGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;

    mImage = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(it->second, &mImage)){
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d data with UID: "<<it->second;
        return false;
    }
    return true;
}

bool TpsVolumeGOConverter::Update_i(GraphicObjectPtr go){
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(go);
    if (mImage == nullptr || volume == nullptr) return false;

    auto volumeData = ConvertRTImageToVolumeDataIn3D_i(mImage);
    if (volumeData == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to convert RTFWKDLDOImage3D to MedViewer3DImageData";
        return false;
    }

    if (!volume->Initialize()){
        TPS_LOG_DEV_ERROR<<"Failed to initialize volume graphic object when converting\
                           image data with UID: "<<mImage->GetUID();
        return false;
    }
    volume->SetImageData(volumeData);
    volume->SetSeriesUID(mImage->GetUID());

    //calculate volume matrix
    double aPixelSpacing[3] = { 0.0 };
    size_t aDim[3] = { 0 };
    Vector3D aImageOrientation[2];
    Point3D aImagePos[2];
    volumeData->GetSpacing(aPixelSpacing);
    volumeData->GetDimension(aDim);
    volumeData->GetImageOrientationX(&aImageOrientation[0]);
    volumeData->GetImageOrientationY(&aImageOrientation[1]);
    volumeData->GetImagePositionFirst(&aImagePos[0]);
    volumeData->GetImagePositionLast(&aImagePos[1]);
    VolumeAPI::CalculateVolumeMatrix(volume, aImagePos[0], aImagePos[1], 
        aImageOrientation[0], aImageOrientation[1], (double) aDim[0], 
        (double) aDim[1], (double) aDim[2], aPixelSpacing[0], aPixelSpacing[1]);
    return true;
}

VolumeDataPtr TpsVolumeGOConverter::ConvertRTImageToVolumeDataIn3D_i(
    const TpsImage3DEntity* const imageEntity){

    if (imageEntity == nullptr){
        TPS_LOG_DEV_ERROR<<"The input TpsImage3DEntity is null";
        return nullptr;
    }
    RtImage3DHeader* header = nullptr;
    if (!imageEntity->GetImage3DHeader(header)){
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d header.";
        return nullptr;
    }
    int sliceCount = header->m_iSliceCount;
    if (header->m_iSliceCount < 1){
        TPS_LOG_DEV_ERROR<<"The RT image has no slice.";
        return nullptr;
    }

    VolumeDataPtr volumeData(new MedViewer3DImageData());
    //LOAD VOLUME INFO

    Mcsf::Vector3f vecOrtX(header->m_ImageXOrientation.x,
        header->m_ImageXOrientation.y, header->m_ImageXOrientation.z);
    Mcsf::Vector3f vecOrtY(header->m_ImageYOrientation.x,
        header->m_ImageYOrientation.y, header->m_ImageYOrientation.z);

    volumeData->SetImageOrientationX(TpsArithmeticConverter::ConvertToVector3D(vecOrtX));
    volumeData->SetImageOrientationY(TpsArithmeticConverter::ConvertToVector3D(vecOrtY));

    const RtImage3DPerScanInfo* pFstImage;
    const RtImage3DPerScanInfo* pLstImage;
    pFstImage = &(header->m_vScanInfoList[0]);
    pLstImage = &(header->m_vScanInfoList[sliceCount - 1]);
    Mcsf::Point3f fstPosition3f(pFstImage->m_ImagePosition.x, 
        pFstImage->m_ImagePosition.y, pFstImage->m_ImagePosition.z);
    Mcsf::Point3f lstPosition3f(pLstImage->m_ImagePosition.x,
        pLstImage->m_ImagePosition.y, pLstImage->m_ImagePosition.z);
    volumeData->SetImagePositionFirst(TpsArithmeticConverter::ConvertToPoint3D(fstPosition3f));
    volumeData->SetImagePositionLast(TpsArithmeticConverter::ConvertToPoint3D(lstPosition3f));

    //set origin
    size_t origin[3]={0, 0, 0};
    volumeData->SetOrigin(origin);

    //spacing
    float fSliceSpacingX = (float)header->m_dXSize;
    float fSliceSpacingY = (float)header->m_dYSize;
    Mcsf::Vector3f vecOrtZ = vecOrtX.CrossProduct(vecOrtY);
    Mcsf::Vector3f vecFst2Lst = lstPosition3f - fstPosition3f;
    if (vecFst2Lst.DotProduct(vecOrtZ) < -1e-6){
        vecOrtZ *= -1;
    }
    double dZSpacing = fabs(vecOrtZ.DotProduct(vecFst2Lst));
    dZSpacing /= (sliceCount - 1);
    volumeData->SetSpacing(fSliceSpacingX, fSliceSpacingY, dZSpacing);

    //slice thickness
    double dSliceThickness = boost::lexical_cast<double>(
        imageEntity->GetRtSeries()->get_slicethickness());
    volumeData->SetSliceThickness(dSliceThickness);

    //modality
    std::string modality = header->m_cModality;
    volumeData->SetModality(modality);

    //rescale slope & rescale intercept
    volumeData->SetSlope(header->m_dScaleSlope);
    volumeData->SetIntercept(header->m_dScaleIntercept);

    //head, anterior, left vector
    Mcsf::Vector3f aLAH[3];
    Mcsf::Vector3f aOrt[6] = {vecOrtX, -1 * vecOrtX, vecOrtY, 
        -1 * vecOrtY, vecOrtZ, -1 * vecOrtZ};
    short aFlag[6] = { 0 };
    for (int i=0; i<3; ++i){
        int iMaxIndex = 0;
        double dMaxDotProduct = -1;
        for (int j=0; j<6; ++j){
            if (aFlag[j]) continue;
            double dotProduct = aOrientationVectorInPatient[i].DotProduct(aOrt[j]);
            if (dotProduct > dMaxDotProduct){
                iMaxIndex = j;
                dMaxDotProduct = dotProduct;
            }
        }
        aLAH[i] = aOrientationVectorInVolume[iMaxIndex];
        aFlag[(iMaxIndex / 2) * 2] = 1;
        aFlag[(iMaxIndex / 2) * 2 + 1] = 1;
    }
    volumeData->SetImageOrientation(
        TpsArithmeticConverter::ConvertToVector3D(aLAH[2]), 
        TpsArithmeticConverter::ConvertToVector3D(aLAH[1]), 
        TpsArithmeticConverter::ConvertToVector3D(aLAH[0]));

    //DataType
    int iBitsAllc = header->m_iBitsAllocated;
    int iPixelRepresentation = header->m_iDataType;
    int dataType;
    if (iBitsAllc == 8 && iPixelRepresentation == 0){//8
        dataType = MEDVIEWER3D_UCHAR;
    }
    else if (iBitsAllc == 8 && iPixelRepresentation == 1){//8
        dataType = MEDVIEWER3D_CHAR;
    }
    else if (iBitsAllc == 16 && iPixelRepresentation == 0){//16
        dataType = MEDVIEWER3D_USHORT;
    }
    else if (iBitsAllc == 16 && iPixelRepresentation == 1){
        dataType = MEDVIEWER3D_SHORT;
    }
    else{
        TPS_LOG_DEV_ERROR<<"Unsupported data type which bit allocated: "
            <<iBitsAllc<<" pixel representation: "<<iPixelRepresentation;
        return nullptr;
    }
    volumeData->SetDataType(dataType);

    //dimension
    int iColumns = header->m_iXDim;
    int iRows = header->m_iYDim;
    volumeData->SetDimension(iColumns, iRows, sliceCount);

    //other
    //volumeData->EnableLOD(false);
    //volumeData->SetCurrentSeriesUID(rtImage->GetUID());

    //allocate buffer & copy memory
    if (!volumeData->AllocateMemory()){
        TPS_LOG_DEV_ERROR<<"Failed to allocate memory to volume data.";
        return nullptr;
    }
    char* buffer = nullptr;
    if (!imageEntity->GetImage3DBuffer(buffer)){
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d buffer.";
        return nullptr;
    }

    //unsigned short* b = (unsigned short*)buffer;
    //unsigned char* bb = new unsigned char[iColumns * iRows * 4];
    //double temp;
    //for (int j = 0; j<sliceCount; ++j){
    //    for (int i=0; i<iColumns * iRows; ++i){
    //        temp = TPS_CLAMP((b[i] - dWindowCenter) / dWindowWidth + 0.5, 0.0, 1.0);
    //        temp *= 255;
    //        bb[4 * i] = temp;
    //        bb[4 * i + 1] = temp;
    //        bb[4 * i + 2] = temp;
    //        bb[4 * i + 3] = 255;
    //    }
    //    char fileName[256];
    //    sprintf_s(fileName, "D:/temp/mr_%d.bmp", j);
    //    SubVolumeHelperAPI::SaveToTrueColorBitmap(fileName, bb, iRows, iColumns);
    //    b += iColumns * iRows;
    //}
    //delete[] bb;

    volumeData->SetVolumeData(buffer);
    //load 3D texture
    if (!volumeData->Load3DTexture()){
        TPS_LOG_DEV_ERROR<<"Failed to load 3D texture to volume data.";
        return nullptr;
    }
    volumeData->GetMinScalar();

    //window level
    double dWindowCenter = header->m_iWindowCenter;
    double dWindowWidth = header->m_iWindowWidth;
    volumeData->SetWindowCenterWindowWidth(dWindowCenter, dWindowWidth);
    return volumeData;
}

TpsMaskGOConverter::TpsMaskGOConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType){

}

TpsMaskGOConverter::~TpsMaskGOConverter(){

}

GraphicObjectPtr TpsMaskGOConverter::CreateGraphicObject_i(){
    std::shared_ptr<GraphicObjVolume> mask(
        new GraphicObjVolume(MEDVIEWER3D_UCHAR, 1));
    if (!mask->Initialize()){
        TPS_LOG_DEV_ERROR<<"Failed to initialize mask volume graphic object.";
        return nullptr;
    }
    return mask;
}

bool TpsMaskGOConverter::PrepareSourceData_i(const GOConverterParameter& para){
    TpsImage3DEntity* image;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(para.obj_uid, &image)){
        TPS_LOG_DEV_ERROR<<"Image 3d entity with UID: "<<para.obj_uid << " does not exist.";
        return false;
    }
    return true;
}


TPS_END_NAMESPACE  // end namespace tps
