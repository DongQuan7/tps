////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_cmd_create_beam.h
/// 
///  \brief class  Create new beam handler declaration 
/// 
///  \version 1.0
/// 
///  \date    Oct.15,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_CREATE_BEAM_H_
#define TPS_BL_CMD_CREATE_BEAM_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedbeam.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

//bolus为当前射野宽度在皮肤上的投影两边外放3cm
#define DEFAULT_BOLUS_MARGIN 30.f

namespace Mcsf
{
    class Matrix4f;
    class Point3f;
}

TPS_BEGIN_NAMESPACE;

class RtBeam;
class RtBeamsegment;
class RtBlock;
class RtPlan;
class RtNormgroup;
class RtPoi;
class RtVoi;
class RtMachine;
class RtMlcshape;
class TpsIECCoordinateSystem;

struct TpsCertifiedBeam;
struct db_Point2d;

class TPS_BL_DECLSPEC CreateBeamCmd : public TpsCommand
{
public:
    CreateBeamCmd(const TpsCertifiedBeam& certifiedBeam, int dataSourceId = SOURCE_GROUP_COMMON);
    CreateBeamCmd();

    virtual ~CreateBeamCmd(void);

    void GetBeamCreated(const RtBeam*& pBeam);

    void SetCertifiedBeam(TpsCertifiedBeam certifiedBeam);

    void SetJawBoundary(
        const RtMachine* machine,
        const std::string& sPhyicalUid,
        const std::string& sVirtualUid);

    bool CreateBeamSegments(
        const std::string& imageUID,
        RtBeam* beam,
        RtDbDef::PLAN_TYPE planType,
        RtMachine* machine,
        RtPoi* isoCenter);

protected:

    virtual int Execute();

    void CreateArcSegments_i(
        RtBeam& beam,
        const std::string& imageUID,
        bool bIsClockWise,
        float gantryStartAngle,
        float gantryArcLength,
        float tableAngle,
        float collimatorAngle,
        RtMachine* const machine,
        std::string beamUid,
        const Mcsf::Point3f& isoCenter,
        RtDbDef::PLAN_TYPE planType,
        std::vector<RtBeamsegment*>& segmentList);

    bool CreateImrtSegment_i(
        RtBeam* beam,
        const std::string& imageUID,
        RtMachine* const machine, 
        float gantryStartAngle,
        float tableAngle,
        float collimatorAngle,
        const Mcsf::Point3f& isoCenter,
        RtDbDef::PLAN_TYPE planType,
        RtDbDef::APERTURE_TYPE apertureType,
        MLC_EDIT_TYPE mlcEditType,
        RtBeamsegment*& segment);

    //create mlc shape or block
    bool CreateImrtAperture_i(
        const RtBeam& beam,
        RtMachine* const machine,
        const Mcsf::Matrix4f& matC2P,
        RtMlcshape* mlcShape, 
        RtBlock* block,
        MLC_EDIT_TYPE mlcEditType);

    bool AutoShapeBlock_i(
        const RtBeam& beam,
        RtMachine* const machine, 
        const Mcsf::Matrix4f& matC2P,
        RtMlcshape* shape,
        RtBlock* block);

    bool AutoShapeMlc_i(
        const RtBeam& beam,
        RtMachine* const machine,
        const Mcsf::Matrix4f& matC2P,
        RtMlcshape* shape,
        MLC_EDIT_TYPE mlcEditType = Center);

    bool AutoShapeJaw_i(
        const RtBeam& beam,
        RtMachine* const machine,
        const Mcsf::Matrix4f& matC2P,
        RtMlcshape* shape);

    void CreateArcSegmentShape_i(
        const RtBeam& beam,
        RtMachine* machine,
        const Mcsf::Matrix4f& matC2P,
        RtMlcshape* shape, 
        MLC_EDIT_TYPE mlcEditType = Center);

    bool CreateShieldingBlock_i(RtBeam* beam, RtMachine* machine);

    // set editing parameters for certified beam
    void SetMlcShapeEditParameters_i(
        RtMachine* machine,
        RtBeam* pBeam,
        RtMlcshape* shape,
        int segmentIndex = 0);

    void UpdateSegmentMlcShapes_i(
        RtMlcshape* const startShape,
        RtMlcshape* const endShape,
        RtBeamsegment* segment);

    RtBeamsegment* CreateBasicBeamSegmentObject_i(int iControlPointIndex,
        float segmentStartAngle,
        float segmentArcLength,
        float tableAngle,
        float collimatorAngle,
        float matB2P[16],
        float beamWeight,
        RtDbDef::ROTATION_DIRECTION directionOfRot,
        const std::string& beamUid);

    bool CombineMlcShapes_i(
        const std::vector<RtMlcshape*>& vecMlcShape,
        RtMlcshape* shape,
        float miniLeafGap);
    
    bool ValidateData(RtDbDef::PLAN_TYPE planType, RtDbDef::RADIATION_TYPE radType) const;

    void SetCommonBeamData_i(RtDbDef::PLAN_TYPE planType, RtBeam* beam);

    double CalculateMiniGapOffset_i(
        RtDbDef::PLAN_TYPE planType,
        std::vector<RtBeamsegment*>& segmentList,
        RtMachine* machine);

    void CorrectBeamLeafGap_i(
        double offset,
        RtMachine* machine,
        std::vector<RtBeamsegment*>& segmentList);

    bool CreateWedge_i(RtBeam* beam);


    /// \brief create IEC coordinate system instance, and insert it into beam manager
    ///  
    /// \param[in] imageUID  the series UID
    /// \param[in] sad  the distance between source and axis
    /// \param[in] beamUID  the beam UID
    /// \param[in] gantryAngle  the angle of gantry
    /// \param[in] tableAngle  the angle of table
    /// \param[in] collimatorAngle  the angle of collimator
    /// \param[in] isoCenter  the location of isoCenter in patient c.s.
    /// \return int TPS_ER_SUCCESS or TPS_ER_FAILURE
    TpsIECCoordinateSystem* CreateIECCS_i(
        const std::string& imageUID,
        float sad,
        const std::string& beamUID,
        float gantryAngle,
        float tableAngle,
        float collimatorAngle,
        const Mcsf::Point3f& isoCenter);
private:

    //adjust jaw according to applicator size, no mlc
    bool CreateApplicatorShape_i(RtMachine* const machine, RtMlcshape* shape);

    enum RectShapeMode
    {
        Jaw_Followed = 0x01,
        Leaf_Followed = 0x02
    };

    //mode: first bite indicates: jaw following the rect, 
    //second bit: leaf following the rect
    //an all-open shape will be created in default setting
    bool CreateRectShape_i(
        RtMachine* const machine,
        RtMlcshape* shape,
        double* rectSize = nullptr,
        int mode = 0);

    int CreateTreatmentBeam(RtBeam*& beam, RtNormgroup* normGroup, const std::string& imageUID);

    void CalculateBeamsegmentAngles_i(
        float gantryStartAngle,
        float arcLength,
        bool bIsClockWise,
        std::vector<float>& vecSegmentAngles);



    void SetBlockParameters_i(RtBlock* block, RtMachine* machine);

    bool CreateDosegridForBeam_i(RtPlan* plan, RtNormgroup* normGroup, RtBeam* beam);


protected:
    TpsCertifiedBeam                mCertifiedBeam;
    int                             mDataSourceId;
    float                           mJawBoundary[8];
};

TPS_END_NAMESPACE

#endif //TPS_PA_PLANNING_CREATEBEAM_HANDLER_H_
