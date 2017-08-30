//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Yuxuan.Duan yuxuan.duan@united-imaging.com
//
/// \file    tps_bl_cmd_gallery_load_patient.h
///
/// \brief   
/// 
/// \version 1.0
/// 
/// \date    2015/11/16
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_GALLERY_LOAD_PATIENT_H_
#define TPS_BL_CMD_GALLERY_LOAD_PATIENT_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

//namespace RTFWK{
//    class RTFWKDLDOCourse;
//    class RTFWKDLDOSeries;
//    class RTFWKDLDONormgroup;
//    class RTFWKDLDOBeam;
//    class RTFWKDLDOPlan;
//    class RTFWKDLDOTPSSystemconfig;
//    class RTFWKDLDOContour;
//    class RTFWKDLDOPatient;
//    class RTFWKDLDODoseGrid;
//}
//
//namespace Mcsf{
//    class Point3f;
//    class Matrix4f;
//}

TPS_BEGIN_NAMESPACE // begin namespace tps

//struct CertifiedUnit;
//struct CertifiedPoiInfo;
//struct CertifiedDoseGroup;
//struct Contour;
//class VOIEntity;
//class TpsImage3DEntity;
//class FTStringBufferGenerator;
//class TPSVOIMesh;
//class TpsIECCoordinateSystem;
//union Pixel32;

class TPS_BL_DECLSPEC LoadPatientForGalleryCmd : public TpsCommand
{
public:
    LoadPatientForGalleryCmd(std::string patientUid);
    virtual ~LoadPatientForGalleryCmd();
protected:
    virtual int Execute();
private:
    std::string mPatientUid;

};

TPS_END_NAMESPACE

#endif  