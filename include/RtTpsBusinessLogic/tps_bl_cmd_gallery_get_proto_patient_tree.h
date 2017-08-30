//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  yuxuan.duan
//
/// \file    tps_bl_cmd_gallery_get_proto_patient_tree.h
///
/// \brief   
/// 
/// \version 1.0
/// 
/// \date    2015/11/17
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_GALLERY_GET_PROTO_PATIENT_TREE_H_
#define TPS_BL_CMD_GALLERY_GET_PROTO_PATIENT_TREE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE // begin namespace tps

namespace proto {
        //class RT_TPS_Gallery_Courses;
        //class RT_TPS_Proto;
        //class RT_TPS_Gallery_Course;
        //class RT_TPS_Gallery_Series;
        class RT_TPS_Gallery_PatientTreeData;
        class RT_TPS_Gallery_Patient;
        class RT_TPS_Gallery_Course;
        class RT_TPS_Gallery_Series;
        class RT_TPS_Gallery_SeriesGroup;
}

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

class RtPatient;
class RtCourse;
class RtPrescription;
class RtPlan;
class RtNormgroup;
class RtBeam;
class RtVoi;
class RtPoi;
class RtSeries;
class RtDoseline;
class TpsImage3DEntity;
class RtDoseoptgoalbase;
struct TpsSeriesGroup4d;

class TPS_BL_DECLSPEC GetProtoPatientTreeForGalleryCmd : public TpsCommand
{
public:
    GetProtoPatientTreeForGalleryCmd(std::string patientUid);
    virtual ~GetProtoPatientTreeForGalleryCmd();

    proto::RT_TPS_Gallery_PatientTreeData* GetProtoPatientData() {return m_patientDataProto;}

    ///// \brief  Get course
    /////  
    ///// \param[in]  courseUID          uid string of Course
    ///// \param[out] pCourse            RTFWKDLDOCourse
    //void GetCourse(const std::string& courseUID, 
    //    RtCourse ** pCourse) const;

    bool ConvertCourseToProto(const RtCourse* course, 
        proto::RT_TPS_Gallery_Course* protoCourse);
    bool ConvertImage3DEntityToProto(TpsImage3DEntity* image3DEntity, 
        proto::RT_TPS_Gallery_Series* protoSeries);
    bool ConvertSereisGroupToProto(const TpsSeriesGroup4d* pSeriesGroup, 
        proto::RT_TPS_Gallery_SeriesGroup* protoSereisGroup);

    //bool ConvertOptGoalToProto(RtDoseoptgoalbase* optGoal, proto::RT_TPS_Optgoal* protoOptGoal);
    //bool ConvertCourseToProto(const RtCourse* course, 
    //    proto::RtTpsProtoCourse* protoCourse);
    //bool ConvertImage3DEntityToProto(std::string courseUID, TpsImage3DEntity* image3DEntity, 
    //    proto::RtTpsProtoSeries* protoSeries);
    //bool ConvertPlanToProto(RtPlan* plan, proto::RtTpsProtoPlan* protoPlan);
    //bool ConvertVoiEntityToProto(VOIEntity* voiEntity, proto::RtTpsProtoVoi* protoVoi);
    //bool ConvertPoiToProto(const RtPoi* poi, proto::RtTpsProtoPoi* protoPoi);
    //bool ConvertNormalizationGroupToProto(RtNormgroup* normgroup, 
    //    proto::RtTpsProtoNormalizationGroup* protoNormgroup);
    //bool ConvertBeamToProto(RtBeam* beam, proto::RtTpsProtoBeam* protoBeam);
    //bool ConvertPrescriptionToProto(std::string courseUID, const RtPrescription* prescription, 
    //    proto::RtTpsProtoPrescription* protoPrescription);
    //bool ConvertDoseLineToProto(const RtDoseline* doseLine, proto::RtTpsProtoDoseLine* protoDoseLine);
    //bool ConvertEasyPlanToProto(const RtPlan* plan, 
    //    std::vector<RtPoi*>& poiList,
    //    RtNormgroup* easyNormGroup,
    //    std::vector<RtBeam*>& beamList,
    //    proto::RT_TPS_EasyPlan *protoPlan, 
    //    proto::RtTpsProtoPatientTreeData *protoPatientTreeData);

protected:
    virtual int Execute();
private:
    std::string mPatientUid;
    proto::RT_TPS_Gallery_PatientTreeData* m_patientDataProto;
};

TPS_END_NAMESPACE

#endif  