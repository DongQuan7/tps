////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_rotate_imrt_beam_on_mpr.h
/// 
///  \brief class RotateImrtBeamOnMprCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/25
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_ROTATE_IMRT_BEAM_ON_MPR_H_
#define TPS_BL_CMD_ROTATE_IMRT_BEAM_ON_MPR_H_

#include "tps_bl_defs.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_create_beam.h"

namespace Mcsf
{
    class Vector3f;
    class Matrix4f;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps
class RtBeam;
class RtBeamsegment;
class RtMachine;
class BeamActiveResponser;
/// \class RotateImrtBeamOnMprCmd
/// 
/// \brief *****
class TPS_BL_DECLSPEC RotateImrtBeamOnMprCmd : public CreateBeamCmd{
public:
    //the constructor and destructor
    RotateImrtBeamOnMprCmd(WINDOW_TYPE wndType, const std::string& wndUid, 
        double currX, double currY, int mouseActionType, 
        const std::string& beamUid, const std::string& segmentUid);
    ~RotateImrtBeamOnMprCmd();

    void SetResponser(const std::shared_ptr<BeamActiveResponser>& responser);
protected:
    virtual int Execute();
    int RotateBeam_i(RtBeam* beam, RtBeamsegment* segment, 
        TpsIECCoordinateSystem* iecCs, RtMachine* machine);
    //update mlc & block shape if auto-shape enabled
    void ReshapeAperture_i(RtBeam* beam, RtBeamsegment* segment, 
        RtMachine* machine, const Mcsf::Matrix4f& matB2P);
    void ReshapeShieldBlock_i(RtBeam* beam, RtBeamsegment* segment, 
        RtMachine* machine, const Mcsf::Matrix4f& matB2P);
    //void ReCreateBolus_i(const std::string& imageUid, RtBeam* beam);
    //return radian
    double AngleBetween_i(const Mcsf::Vector3f& vecL, const Mcsf::Vector3f& vecR);
    std::string GenerateDefaultBeamDescription_i(double gantryAngle, double collimatorAngle, 
        double tableAngle);
private:
    WINDOW_TYPE     mWndType;
    std::string     mWndUid;
    double          mCurrX;
    double          mCurrY;
    int             mMouseActionType;
    std::string     mBeamUid;
    std::string     mSegmentUid;
    std::shared_ptr<BeamActiveResponser> mResponser;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(RotateImrtBeamOnMprCmd);
};

TPS_END_NAMESPACE  // end namespace tps

#endif