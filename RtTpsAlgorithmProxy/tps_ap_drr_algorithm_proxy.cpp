////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_ap_drr_algorithm_proxy.cpp
/// 
///  \brief class DrrAlgorithmProxy implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/05/28
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsAlgorithmProxy/tps_ap_drr_algorithm_proxy.h"

#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"

#include "alg/alg_drrgenerator.h"
#include "alg/alg_common_algreturn.h"
#include "Mcsf3DArithmetic/mcsf_3d_matrix4x4.h"

#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE;   // begin namespace tps

#ifndef PI
#define PI          (3.141592654f)
#endif


bool DrrAlgorithmProxy::GenerateDrrImage(RTALG::DRRGeneratorParam& drrParam, 
    unsigned short* image, unsigned int* windowLevel){
 
    RTALG::DRRGenerator drrGenerator;
    return ALG_OK == drrGenerator.DRRGeneration(
        drrParam, image, windowLevel[0], windowLevel[1]);
}

bool DrrAlgorithmProxy::GenerateDrrRtImage(
    const TpsImage3DEntity* image, 
    const RtMachine* machine, 
    const std::string& beamUID, 
    const std::string& isoCenterUID, 
    float* ct2Density, 
    const Mcsf::Matrix4f& matBeam2Volume,
    float energy, float gantryAngle, 
    RtRtimage*& rtImage, 
    unsigned int width, unsigned int height,
    bool isForTreatmentBeam/* = false*/)
{
    if (width == 0 || height == 0)
    {
        TPS_LOG_DEV_ERROR<<"Wrong size specified.";
        return false;
    }

    RTALG::DRRGeneratorParam param;
    RtImage3DHeader* header = nullptr;
    if (!image->GetImage3DHeader(header) || header == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get image header of image: "<<image->GetUID();
        return false;
    }

    RtDbDef::PatientPosition pp;
    std::string strPP(header->m_cPatientPosition);
    boost::to_upper(strPP);
    if (strPP == "HFS") pp = RtDbDef::HFS;
    else if (strPP == "FFS") pp = RtDbDef::FFS;
    else if (strPP == "HFP") pp = RtDbDef::HFP;
    else if (strPP == "FFP") pp = RtDbDef::FFP;
    else if (strPP == "HFDL") pp = RtDbDef::HFDL;
    else if (strPP == "FFDL") pp = RtDbDef::FFDL;
    else if (strPP == "HFDR") pp = RtDbDef::HFDR;
    else if (strPP == "FFDR") pp = RtDbDef::FFDR;
    else
    {
        TPS_LOG_DEV_ERROR<<"Unrecognized patient position.";
        return false;
    }

    param.iVolumeWidth = header->m_iXDim;
    param.iVolumeHeight = header->m_iYDim;
    param.iVolumeDepth = header->m_iSliceCount;
    param.fVoxelWidth = header->m_dXSize;
    param.fVoxelHeight = header->m_dYSize;
    param.fVoxelDepth = header->m_fSliceSpacing;
    param.fRescaleSlope = header->m_dScaleSlope;
    param.fRescaleIntercept = header->m_dScaleIntercept;
    param.iBitsAllocated = header->m_iBitsAllocated;
    param.iDataType = header->m_iDataType;

    char* buffer = nullptr;
    if (!image->GetImage3DBuffer(buffer) || buffer == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d buffer.";
        return false;
    }
    param.pVolume = buffer;

    //beam to volume
    float aB2V[16] = {0.f};
    matBeam2Volume.GetElements(aB2V);
    param.pMatrix = aB2V;
    //sad
    double sad = machine->get_sad();
    param.fSourceToAxisDistance = sad;
    //image resolution
    param.iProjSizeX = width;
    param.iProjSizeY = height;
    //fov
    double volumeSize[3] = { 
        (header->m_iXDim - 1) * header->m_dXSize, 
        (header->m_iYDim - 1) * header->m_dYSize, 
        (header->m_iSliceCount - 1) * header->m_fSliceSpacing};
    double maxLen = TPS_MAX(TPS_MAX(volumeSize[0], volumeSize[1]), volumeSize[2]);
    param.fConeAngle = atan(maxLen / 2 / sad) * 180 / PI;
    //energy
    param.fEnergy = energy;
    //flag if auto generate window level
    param.iDisplayType = 1;
    //ct 2 density, [-1024, 5000]
    param.pCT2DensityMap = ct2Density;

    unsigned int wl[2] = {0};
    unsigned int count = param.iProjSizeX * param.iProjSizeY;
    unsigned short* drrBuffer = new unsigned short[count];
    memset(drrBuffer, 0, sizeof(unsigned short) * count);
    if (!GenerateDrrImage(param, drrBuffer, wl)){
        TPS_LOG_DEV_ERROR<<"Failed to invoke DrrAlgorithmProxy::GenerateDrrImage.";
        delete[] drrBuffer;
        return false;
    }
    if (wl[1] <= 0){
        TPS_LOG_DEV_ERROR<<"Wrong window level generated.";
        delete[] drrBuffer;
        return false;
    }

    rtImage = new RtRtimage(true);
    if (isForTreatmentBeam){
        rtImage->set_treatmentbeamuid(beamUID);
    }
    else{
        rtImage->set_beamuid(beamUID);
    }
    rtImage->set_samplesperpixel(1);
    rtImage->set_photometricinterpretation("MONOCHROME2");
    rtImage->set_bitsallocated(16);
    rtImage->set_bitsstored(16);
    rtImage->set_highbit(15);
    rtImage->set_pixelrepresentation("0");
    rtImage->set_rescaleintercept(0.0);
    rtImage->set_rescaleslope(1.0);
    rtImage->set_imagetype(RtDbDef::DRR);
    rtImage->set_conversiontype(RtDbDef::WSD);
    rtImage->set_rtimageplane(RtDbDef::NORMAL);
    rtImage->set_xrayimagereceptortranslationx(0.0);
    rtImage->set_xrayimagereceptortranslationy(0.0);
    rtImage->set_xrayimagereceptortranslationz(0.0);
    rtImage->set_xrimagereceptorangle(0.0);
    //rtImage->SetRTImagePosition(-maxLen / 2, maxLen / 2);
    rtImage->set_firstrtimageposition(-maxLen / 2);
    rtImage->set_secondrtimageposition(maxLen / 2);
    //rtImage->SetImageXOrientation(Mcsf::Vector3f(1.f, 0.f, 0.f));
    rtImage->set_ximagexorientation(1.f);
    rtImage->set_ximageyorientation(0.f);
    //rtImage->SetImageYOrientation(Mcsf::Vector3f(0.f, -1.f, 0.f));
    rtImage->set_yimagexorientation(0.f);
    rtImage->set_yimageyorientation(-1.f);
    rtImage->set_firstimageplanepixelspacing(maxLen / (width - 1));
    rtImage->set_secondimageplanepixelspacing(maxLen / (height - 1));
    //rtImage->SetImagePlanePixelSpacing(maxLen / (width - 1), maxLen / (height - 1));
    rtImage->set_machinename(machine->get_name());
    rtImage->set_primarydosimeterunit("MU");
    rtImage->set_sad(sad);
    rtImage->set_sid(sad);
    rtImage->set_isocenteruid(isoCenterUID);
    //rtImage->SetSSD();
    rtImage->set_acquisitiondatetime(boost::posix_time::second_clock::local_time());
    //buffer related
    rtImage->set_columns(width);
    rtImage->set_rows(height);
    rtImage->set_windowcenter(wl[0]);
    rtImage->set_windowwidth(wl[1]);
    rtImage->set_pixel_data_buffer((char*)drrBuffer, count * sizeof(unsigned short));
    //rt image label
    delete[] drrBuffer;
    rtImage->set_rtimagelabel("g" + boost::lexical_cast<std::string>(gantryAngle));
    //rtImage.SetDescription();
    //rtImage.SetName();
    rtImage->set_frame(1);
    rtImage->set_angle(gantryAngle);
    rtImage->set_patientposition(pp);

    //     float* drrImage = new float[count];
    //     memset(drrImage, 0, sizeof(float) * count);
    //     for (int i = 0; i < count; ++i){
    //         drrImage[i] = TPS_CLAMP((float(drrBuffer[i]) - wl[0]) / wl[1] + 0.5f, 0.f, 1.f);
    //     }
    //     unsigned char* bmpBuffer = new unsigned char[count * 4];
    //     for (int j=0; j < count; ++j){
    //         bmpBuffer[4 * j] = drrImage[j] * 255;
    //         bmpBuffer[4 * j + 1] = drrImage[j] * 255;
    //         bmpBuffer[4 * j + 2] = drrImage[j] * 255;
    //         bmpBuffer[4 * j + 3] = 255;
    //     }
    //     delete[] drrImage;
    //     char fileName[256];
    //     sprintf_s(fileName, "d:\\temp\\drr_from_beam_%s.bmp", beam->GetName().c_str());
    //     SubVolumeHelperAPI::SaveToTrueColorBitmap(fileName, bmpBuffer, param.iProjSizeX, param.iProjSizeY);
    //     delete []bmpBuffer;
    return true;
}
TPS_END_NAMESPACE  // end namespace tps