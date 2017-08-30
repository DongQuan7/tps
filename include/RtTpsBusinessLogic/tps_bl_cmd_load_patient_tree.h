//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xiaoqing.Shaangguan Xiaoqing.Shaangguan@united-imaging.com
//
/// \file    tps_load_patient_tree_cmd.h
///
/// \brief   
/// 
/// \version 1.0
/// 
/// \date    2015/07/08
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_LOAD_PATIENT_TREE_H_
#define TPS_BL_CMD_LOAD_PATIENT_TREE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

namespace RTFWK{
    class RTFWKDLDOCourse;
    class RTFWKDLDOSeries;
    class RTFWKDLDONormgroup;
    class RTFWKDLDOBeam;
    class RTFWKDLDOPlan;
    class RTFWKDLDOTPSSystemconfig;
    class RTFWKDLDOContour;
    class RTFWKDLDOPatient;
    class RTFWKDLDODoseGrid;
}

namespace Mcsf{
    class Point3f;
    class Matrix4f;
}

TPS_BEGIN_NAMESPACE // begin namespace tps

struct CertifiedUnit;
struct CertifiedDoseGroup;
struct Contour;
class TpsImage3DEntity;
class FTStringBufferGenerator;
class TPSVOIMesh;
class TpsIECCoordinateSystem;
union Pixel32;

class TPS_BL_DECLSPEC LoadPatientTreeCmd : public TpsCommand
{
public:
	LoadPatientTreeCmd(std::string patientUid);
	virtual ~LoadPatientTreeCmd();
protected:
	virtual int Execute();
private:
	std::string mPatientUid;

};

TPS_END_NAMESPACE

#endif  