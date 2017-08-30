////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_ap_fusionalgorithmproxy.cpp
/// 
///  \brief class FusionAlgorithmProxy implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/05/12
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsAlgorithmProxy/tps_ap_fusion_algorithmproxy.h"

#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfAlgorithms/McsfAppLib/McsfAlgoRegistrationApplication.h"
#include "McsfAlgorithms/McsfAppLib/McsfAlgoRegistrationApplicationRT.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

bool FusionAlgorithmProxy::Register(
    const TpsImage3DEntity& fixedImage, 
    const TpsImage3DEntity& floatImage, 
    Mcsf::Matrix4f& registrationMatrix)
{
    Mcsf::Matrix4f matV2P_fixed, matV2P_float;
    fixedImage.GetVolumeToPatientMatrix(matV2P_fixed);
    floatImage.GetVolumeToPatientMatrix(matV2P_float);
    registrationMatrix = matV2P_float.Inverse();
    registrationMatrix.Append(matV2P_fixed);

    return true;
}

bool FusionAlgorithmProxy::Register_InitSetup(
    const TpsImage3DEntity& fixedImage, 
    const TpsImage3DEntity& floatImage, 
    Mcsf::Matrix4f& registrationMatrix)
{

    Mcsf::Matrix4f matV2P_fixed;
    fixedImage.GetVolumeToPatientMatrix(matV2P_fixed);
    const RtImage3DHeader* header = fixedImage.GetRtSeries()->get_header();
    Mcsf::Point3f centerFixed(float(header->m_iXDim - 1) / 2, 
                        float(header->m_iYDim - 1) / 2, 
                        float(header->m_iSliceCount - 1) / 2);
    centerFixed = matV2P_fixed.Transform(centerFixed);

    Mcsf::Matrix4f matV2P_float;
    floatImage.GetVolumeToPatientMatrix(matV2P_float);
    header = floatImage.GetRtSeries()->get_header();
    Mcsf::Point3f centerFloat(float(header->m_iXDim - 1) / 2, 
                        float(header->m_iYDim - 1) / 2, 
                        float(header->m_iSliceCount - 1) / 2);
    centerFloat = matV2P_float.Transform(centerFloat);
    Mcsf::Vector4f translate(centerFloat - centerFixed, 1);
    registrationMatrix.SetIdentity();
    registrationMatrix.SetCol3(translate);
    return true;
}

bool FusionAlgorithmProxy::Register_Landmark(
    const Mcsf::Point3f* fixedPoints, 
    const Mcsf::Point3f* floatPoints, 
    unsigned int pairCount, 
    Mcsf::Matrix4f& matrix)
{
    TPS_LOG_TRACE_INFO<<"FusionAlgorithmProxy::Register_Landmark Begin!";
    double* fixedPointsBuffer = new double[pairCount * 3];
    double* floatPointsBuffer = new double[pairCount * 3];
    for (unsigned int i=0; i<pairCount; ++i){
        fixedPointsBuffer[3*i] = fixedPoints[i].GetX();
        fixedPointsBuffer[3*i + 1] = fixedPoints[i].GetY();
        fixedPointsBuffer[3*i + 2] = fixedPoints[i].GetZ();
        floatPointsBuffer[3*i] = floatPoints[i].GetX();
        floatPointsBuffer[3*i + 1] = floatPoints[i].GetY();
        floatPointsBuffer[3*i + 2] = floatPoints[i].GetZ();
    }

    double rotate[9];
    double translate[3];
    if (!Mcsf::RT_rigidpointalign(fixedPointsBuffer, floatPointsBuffer, pairCount, 3, rotate, translate)){
        TPS_LOG_DEV_ERROR<<"[ERROR][FusionAlgorithmProxy][Register_Landmark]: "
            <<"Invoke Mcsf::RT_rigidpointalign failed!";
        delete[] fixedPointsBuffer; delete[] floatPointsBuffer;
        return false;
    }
    delete[] fixedPointsBuffer; delete[] floatPointsBuffer;

    matrix.SetCol(0, Mcsf::Vector4f(rotate[0], rotate[3], rotate[6], 0));
    matrix.SetCol(1, Mcsf::Vector4f(rotate[1], rotate[4], rotate[7], 0));
    matrix.SetCol(2, Mcsf::Vector4f(rotate[2], rotate[5], rotate[8], 0));
    matrix.SetCol(3, Mcsf::Vector4f(translate[0], translate[1], translate[2], 1));
    TPS_LOG_TRACE_INFO<<"FusionAlgorithmProxy::Register_Landmark Leave successfully!";
    return true;
}

Mcsf::CVolumeDataInfo* _PrepareVolumeData(const TpsImage3DEntity& image)
{
    RtSeries* series = image.GetRtSeries();
    RtImage3DHeader* header = series->get_header();
    unsigned int size[3] = 
    {
        header->m_iXDim, header->m_iYDim, header->m_iSliceCount
    };
    double spacing[3] = 
    {
        header->m_dXSize, header->m_dYSize, header->m_fSliceSpacing
    };
    double intercept = header->m_dScaleIntercept;
    double slope= header->m_dScaleSlope;
    auto point = header->m_vScanInfoList[0].m_ImagePosition;
    double imagePosition[3] = 
    {
        point.x, point.y, point.z
    };
    double imageOrientation[6] =
    {
        header->m_ImageXOrientation.x,
        header->m_ImageXOrientation.y,
        header->m_ImageXOrientation.z,
        header->m_ImageYOrientation.x,
        header->m_ImageYOrientation.y,
        header->m_ImageYOrientation.z
    };
    unsigned long ulSize(0);
    short* buffer = (short*)series->get_imagedata(&ulSize);
    ulSize /= sizeof(short);
    short* imageData = new short[ulSize];
    //当前算法只支持short，如果为unsigned，映射至short
    if (header->m_iDataType == 0)
    {
        for (int i = 0; i < ulSize; ++i){
            imageData[i] = buffer[i] ^ 0x8000;
        }
        intercept += 32768.0 * slope;
    }
    else
    {
        memcpy(imageData, buffer, ulSize * sizeof(short));
    }

    auto volumeInfo = new Mcsf::CVolumeDataInfo();
    volumeInfo->pImage = imageData;
    volumeInfo->SetSize(size);
    volumeInfo->SetSpacing(spacing);
    volumeInfo->SetPosition(imagePosition);
    volumeInfo->SetOrientation(imageOrientation);
    volumeInfo->SetRescaleParameter(slope, intercept);
    return volumeInfo;
}

void _PreProcess(
    const TpsImage3DEntity& fixedImage, 
    const TpsImage3DEntity& floatImage,
    Mcsf::CVolumeDataInfo* floatVolumeInfo, 
    unsigned char* floatRoiMask,
    Mcsf::Matrix4f& floatNewImgToOrgImg)
{
    floatNewImgToOrgImg.SetIdentity();

    RtSeries* series = fixedImage.GetRtSeries();
    RtImage3DHeader* header = series->get_header();
    auto imgPos1st = header->m_vScanInfoList.front().m_ImagePosition;
    auto imgPos2st = header->m_vScanInfoList.back().m_ImagePosition;
    Mcsf::Vector3f fixedVecZ(imgPos2st.x - imgPos1st.x , 
        imgPos2st.y - imgPos1st.y, imgPos2st.z - imgPos1st.z);

    series = floatImage.GetRtSeries();
    header = series->get_header();
    imgPos1st = header->m_vScanInfoList.front().m_ImagePosition;
    imgPos2st = header->m_vScanInfoList.back().m_ImagePosition;
    Mcsf::Vector3f floatVecZ(imgPos2st.x - imgPos1st.x , 
        imgPos2st.y - imgPos1st.y, imgPos2st.z - imgPos1st.z);

    if(fabs(fixedVecZ.AngleBetween(floatVecZ)) > PI / 2)
    {
        unsigned int* size = floatVolumeInfo->uiSize;
        unsigned int plane_size = size[0] * size[1];
        short*imageBuffer(floatVolumeInfo->pImage);
        short* tmp_buffer = new short[plane_size];
        void * source1, * source2;
        unsigned int head(0), tail(size[2] - 1);
        while(head++ < tail--)
        {
            source1 = imageBuffer + head * plane_size;
            source2 = imageBuffer + tail * plane_size;
            memcpy(tmp_buffer, source1, plane_size * sizeof(short));
            memcpy(source1, source2, plane_size * sizeof(short));
            memcpy(source2, tmp_buffer, plane_size * sizeof(short));

            source1 = floatRoiMask + head * plane_size;
            source2 = floatRoiMask + tail * plane_size;
            memcpy(tmp_buffer, source1, plane_size * sizeof(unsigned char));
            memcpy(source1, source2, plane_size * sizeof(unsigned char));
            memcpy(source2, tmp_buffer, plane_size * sizeof(unsigned char));
        }
        delete[] tmp_buffer;

        floatNewImgToOrgImg.SetElem(2, 2, -1.f);
        floatNewImgToOrgImg.SetElem(3, 2, (size[2] - 1) * floatVolumeInfo->dSpacing[2]);
    }
}

bool FusionAlgorithmProxy::Register_Alignment(
    /*const */TpsImage3DEntity& fixedImage, 
    /*const */TpsImage3DEntity& floatImage, 
    unsigned char* fixedRoiMask,
    unsigned char* floatRoiMask,
    REG_CARED_TISSUE caredTissue,
    Mcsf::IProgress* progress,
    Mcsf::Matrix4f& matrix, 
    bool iterativeReg/* = false*/)
{
    TPS_LOG_TRACE_INFO<<"FusionAlgorithmProxy::Register_Alignment Begin!";

    if (fixedImage.GetRtSeries() == nullptr || floatImage.GetRtSeries() == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][FusionAlgorithmProxy][Register_Alignment]: "
            <<"Invalid parameters!";
        return false;
    }
    //Currently, only 16-bit data is supported
    if (fixedImage.GetRtSeries()->get_header()->m_iBitsAllocated != 16 || 
        floatImage.GetRtSeries()->get_header()->m_iBitsAllocated != 16)
    {
        TPS_LOG_DEV_ERROR<<"[ERROR][FusionAlgorithmProxy][Register_Alignment]: "
            <<"Can not do rigid registration: only support 16-bit data currently.";
        return false;
    }

    auto fixedVolumeInfo = _PrepareVolumeData(fixedImage);
    auto floatVolumeInfo = _PrepareVolumeData(floatImage);
    std::string fixedModality = fixedImage.GetRtSeries()->get_modality();
    std::string floatModality = floatImage.GetRtSeries()->get_modality();
    auto ulSize = fixedVolumeInfo->uiSize[0] * 
        fixedVolumeInfo->uiSize[1] * fixedVolumeInfo->uiSize[2];
    short* floatNewBuffer = new short[ulSize];
    memset(floatNewBuffer, 0, ulSize * sizeof(short));

    Mcsf::Matrix4f floatNewImgToOrgImg;
    _PreProcess(fixedImage, floatImage, floatVolumeInfo, floatRoiMask, 
        floatNewImgToOrgImg);

    Mcsf::Matrix4f matFixedVolume2Patient;
    fixedImage.GetVolumeToPatientMatrix(matFixedVolume2Patient);
    Mcsf::Matrix4f matFixedVolume2Image = Mcsf::MakeScale(
        Mcsf::Vector3f(fixedVolumeInfo->dSpacing[0], 
        fixedVolumeInfo->dSpacing[1], fixedVolumeInfo->dSpacing[2]));
    Mcsf::Matrix4f matFloatImage2Volume = Mcsf::MakeScale(
        Mcsf::Vector3f(1.0f / floatVolumeInfo->dSpacing[0], 
        1.0f / floatVolumeInfo->dSpacing[1], 1.0f / floatVolumeInfo->dSpacing[2]));
    matFloatImage2Volume.Append(floatNewImgToOrgImg);
    Mcsf::Matrix4f matFloatVolume2Patient;
    floatImage.GetVolumeToPatientMatrix(matFloatVolume2Patient);

    //now we do not support iterative registration
    double* preTM = nullptr;
    Mcsf::Matrix4f matPreRes;
    matPreRes.SetIdentity();
    iterativeReg;
    //if (iterativeReg){
    //    //if iterative registration, get pre-relationship buffer
    //    matPreRes = matrix;
    //    matPreRes.Append(matFixedVolume2Patient);
    //    matPreRes.Append(matFixedVolume2Image.Inverse());
    //    matPreRes.Prepend(matFloatVolume2Patient.Inverse());
    //    matPreRes.Prepend(matFloatImage2Volume.Inverse());
    //    preTM = new double[16];
    //    for (int c = 0; c < 4; ++c){
    //        for (int r = 0; r < 4; ++r){
    //            preTM[4 * r + c] = matPreRes.GetElem(c, r);
    //        }
    //    }
    //}

    double tm[9]={0.0}, offset[3]={0.0}, dResTM[16] = {0.0};
    bool bRes = false;
    if (fixedModality == "CT" && floatModality == "MR")
    {
        bRes = Mcsf::RT_TPS_RigidRegistrationCTMR(
            *fixedVolumeInfo, *floatVolumeInfo, 
            fixedRoiMask, floatRoiMask, caredTissue, 
            dResTM, tm, offset, floatNewBuffer, 0.0, 1.0, progress);
    }
    else if (fixedModality == "CT" && floatModality == "PT")
    {
        bRes = Mcsf::RT_TPS_RigidRegistrationCTPET(
            *fixedVolumeInfo, *floatVolumeInfo, 
            fixedRoiMask, floatRoiMask, caredTissue, 
            dResTM, tm, offset, floatNewBuffer, 0.0, 1.0, progress);
    }
    else
    {
        bRes = Mcsf::RT_TPS_RigidRegistrationCTCT(
            *fixedVolumeInfo, *floatVolumeInfo, 
            fixedRoiMask, floatRoiMask, caredTissue, 
            dResTM, tm, offset, floatNewBuffer, 0.0, 1.0, progress);
    }
    delete[] fixedVolumeInfo->pImage;
    delete[] floatVolumeInfo->pImage;
    delete fixedVolumeInfo;
    delete floatVolumeInfo;
    delete[] floatNewBuffer;
    if (preTM) delete[] preTM;
    if (!bRes){
        TPS_LOG_DEV_ERROR<<"[ERROR][FusionAlgorithmProxy][Register_Alignment]: "
            <<"Invoke AdaptiveRTRigidRegistration failed!";
        return false;
    }
    
    //the output matrix is in image c.s., which means the first point of volume is(0, 0, 0),
    //the second one is (spacing[0], 0, 0), we need transform to patient c.s.
    matrix.SetCol(0, Mcsf::Vector4f(dResTM[0], dResTM[4], dResTM[8], dResTM[12]));
    matrix.SetCol(1, Mcsf::Vector4f(dResTM[1], dResTM[5], dResTM[9], dResTM[13]));
    matrix.SetCol(2, Mcsf::Vector4f(dResTM[2], dResTM[6], dResTM[10], dResTM[14]));
    matrix.SetCol(3, Mcsf::Vector4f(dResTM[3], dResTM[7], dResTM[11], dResTM[15]));
    matrix.Append(matPreRes);
    matrix.Append(matFixedVolume2Image);
    matrix.Append(matFixedVolume2Patient.Inverse());
    matrix.Prepend(matFloatImage2Volume);
    matrix.Prepend(matFloatVolume2Patient);
    TPS_LOG_TRACE_INFO<<"FusionAlgorithmProxy::Register_Alignment Leave successfully!";
    return true;
}

bool FusionAlgorithmProxy::Register_NonRigid(
    /*const */TpsImage3DEntity& fixedImage, 
    /*const */TpsImage3DEntity& floatImage, 
    unsigned int* fixedRoiBoundingBox,
    unsigned int* floatRoiBoundingBox,
    Mcsf::IProgress* progress,
    void* resultMovBuffer)
{
    TPS_LOG_TRACE_INFO<<"FusionAlgorithmProxy::Register_NonRigid Begin!";
    fixedRoiBoundingBox; floatRoiBoundingBox;//todo
    RtSeries* pFixedSeries = fixedImage.GetRtSeries();
    RtSeries* pFloatSeries = floatImage.GetRtSeries();
    if (pFixedSeries == nullptr || pFloatSeries == nullptr || resultMovBuffer == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][FusionAlgorithmProxy][Register_NonRigid]: "
            <<"Invalid parameters!";
        return false;
    }

    unsigned int sizeFixed[3], sizeFloat[3];
    double spacingFixed[3], spacingFloat[3];

    RtImage3DHeader* header = pFixedSeries->get_header();
    sizeFixed[0] = header->m_iXDim; 
    sizeFixed[1] = header->m_iYDim; 
    sizeFixed[2] = header->m_iSliceCount;
    spacingFixed[0] = header->m_dXSize; 
    spacingFixed[1] = header->m_dYSize; 
    spacingFixed[2] = header->m_fSliceSpacing;
    double intercept = header->m_dScaleIntercept;
    double slope= header->m_dScaleSlope;
    unsigned long ulSize;
    short* fixedBuffer = (short*)pFixedSeries->get_imagedata(&ulSize);
    int countFixed = sizeFixed[0] * sizeFixed[1] * sizeFixed[2];
    float* aShifting = new float[countFixed * 3];
    memset(aShifting, 0, sizeof(float) * countFixed * 3);
    memset(resultMovBuffer, 0, countFixed * sizeof(short));

    header = pFloatSeries->get_header();
    sizeFloat[0] = header->m_iXDim; 
    sizeFloat[1] = header->m_iYDim; 
    sizeFloat[2] = header->m_iSliceCount;
    spacingFloat[0] = header->m_dXSize; 
    spacingFloat[1] = header->m_dYSize; 
    spacingFloat[2] = header->m_fSliceSpacing;
    short* floatBuffer = (short*)pFloatSeries->get_imagedata(&ulSize);

    double tm[9]={0.0}, offset[3]={0.0};
    bool bRes = Mcsf::RT_NonRigidRegistration(
        fixedBuffer, floatBuffer, 
        sizeFixed, sizeFloat, 
        spacingFixed, spacingFloat, 
        (short*)resultMovBuffer,
        aShifting,
        tm, offset, 
        intercept, slope, progress);
    delete[] aShifting;
    if (!bRes){
        TPS_LOG_DEV_ERROR<<"[ERROR][FusionAlgorithmProxy][Register_NonRigid]: "
            <<"Invoke Mcsf::RT_NonRigidRegistration failed!";
        return false;
    }
    
    TPS_LOG_TRACE_INFO<<"FusionAlgorithmProxy::Register_NonRigid Leave successfully!";
    return true;
}
TPS_END_NAMESPACE  // end namespace tps
