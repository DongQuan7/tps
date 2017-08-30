////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_rotate_arc_beam_on_mpr.h
/// 
///  \brief class RotateImrtBeamOnMprCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/12/10
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_ROTATE_ARC_BEAM_ON_MPR_H_
#define TPS_BL_CMD_ROTATE_ARC_BEAM_ON_MPR_H_

#include "tps_bl_defs.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_create_beam.h"

namespace Mcsf
{
    class Vector3f;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps
class RtBeam;
class RtBeamsegment;
class RtMachine;
class BeamActiveResponser;

/// \class RotateArcBeamOnMprCmd
/// 
/// \brief *****
class TPS_BL_DECLSPEC RotateArcBeamOnMprCmd : public CreateBeamCmd{
public:
    //the constructor and destructor
    RotateArcBeamOnMprCmd(WINDOW_TYPE wndType, const std::string& wndUid, 
        double preX, double preY, double currX, double currY, int mouseActionType, 
        const std::string& beamUid, bool isToRotateStart);
    ~RotateArcBeamOnMprCmd();

    void SetResponser(const std::shared_ptr<BeamActiveResponser>& responser);
protected:
    virtual int Execute();
    void CopyMlcShapeEditParameters_i(RtMlcshape* const source, RtMlcshape* target);
    std::string GenerateDefaultBeamDescription_i(double startGantryAngle, double arcLength,
        double endGantryAngle, double collimatorAngle, double tableAngle);
private:
    WINDOW_TYPE     mWndType;
    std::string     mWndUid;
    double          mPreX;
    double          mPreY;
    double          mCurrX;
    double          mCurrY;
    int             mMouseActionType;
    std::string     mBeamUid;
    bool            mIsToRotateStart;
    std::shared_ptr<BeamActiveResponser> mResponser;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(RotateArcBeamOnMprCmd);
};

TPS_END_NAMESPACE  // end namespace tps

#endif