//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  
//
/// \file    
///
/// \brief   
/// 
/// \version 1.0
/// 
/// \date    2015/08/12
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_GET_PROTO_PATIENT_TREE_H_
#define TPS_BL_CMD_GET_PROTO_PATIENT_TREE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE // begin namespace tps

namespace proto {
    class RtTpsProtoCourse;
    class RtTpsProtoSeries;
    class RtTpsProtoPlan;
    class RtTpsProtoVoi;
    class RtTpsProtoPoi;
    class RtTpsProtoNormalizationGroup;
    class RtTpsProtoBeam;
    class ReferenceImage;
    class RtTpsProtoSite;
    class RtTpsProtoPrescription;
    class RtTpsProtoDoseLine;
    class RtTpsProtoDoseLineSetting;
    class RtTpsProtoPatientTreeData;
    class RT_TPS_Optgoal;
    class RtTpsProtoSeriesGroup;
    class RtTpsProtoTable;
	class RT_TPS_DVHData;
}

class RtPatient;
class RtCourse;
class RtSite;
class RtPrescription;
class RtPlan;
class RtNormgroup;
class RtBeam;
class RtRtimage;
class RtVoi;
class RtPoi;
class RtSeries;
class RtDoseline;
class TpsImage3DEntity;
class ROIEntity;
class RtDoseoptgoalbase;
struct TpsSeriesGroup4d;
class RtDvh;

class TPS_BL_DECLSPEC GetProtoPatientTreeCmd : public TpsCommand
{
public:
	GetProtoPatientTreeCmd(std::string patientUid);
	virtual ~GetProtoPatientTreeCmd();

    proto::RtTpsProtoPatientTreeData* GetProtoData() {return m_patientDataProto;}

    bool ConvertOptGoalToProto(RtDoseoptgoalbase* optGoal, proto::RT_TPS_Optgoal* protoOptGoal);
    bool ConvertCourseToProto(const RtCourse* course, 
        proto::RtTpsProtoCourse* protoCourse);
    bool ConvertImage3DEntityToProto(TpsImage3DEntity* image3DEntity, 
        proto::RtTpsProtoSeries* protoSeries);
    bool ConvertPlanToProto(RtPlan* plan, proto::RtTpsProtoPlan* protoPlan);
    bool ConvertVoiEntityToProto(ROIEntity* voiEntity, proto::RtTpsProtoVoi* protoVoi);
    bool ConvertPoiToProto(const RtPoi* poi, proto::RtTpsProtoPoi* protoPoi, const bool isEasyPlanPoi = false);
    bool ConvertNormalizationGroupToProto(RtNormgroup* normgroup, 
        proto::RtTpsProtoNormalizationGroup* protoNormgroup);
    bool ConvertBeamToProto(RtBeam* beam, proto::RtTpsProtoBeam* protoBeam);
    bool ConvertRtImageToProto(const RtRtimage* rtImage, proto::ReferenceImage* protoReferenceImage);
    bool ConvertSiteToProto(std::string courseUID, const RtSite* site, 
        proto::RtTpsProtoSite* protoSite);
    bool ConvertPrescriptionToProto(std::string courseUID, const RtPrescription* prescription, 
        proto::RtTpsProtoPrescription* protoPrescription);
    bool ConvertDoseLineToProto(const RtDoseline* doseLine, proto::RtTpsProtoDoseLine* protoDoseLine);
    bool ConvertSereisGroupToProto(const TpsSeriesGroup4d* pSeriesGroup, 
        proto::RtTpsProtoSeriesGroup* protoSereisGroup);

	bool ConvertDVHToProto(const std::string parentUID, const RtDvh* dvh, proto::RT_TPS_DVHData* protoDvh);

protected:
	virtual int Execute();
private:
    std::string mPatientUid;
	proto::RtTpsProtoPatientTreeData* m_patientDataProto;
};

TPS_END_NAMESPACE

#endif  