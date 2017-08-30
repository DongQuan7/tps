//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  xiaoqing.shangguan (mailto:xiaoqing.shangguan@united-imaging.com)
///
///  \file   tps_cca_approvenormgroup_cmd.h
///  \brief   approve normgroup
///
///  \version 1.0
///  \date  2014/11/10  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_APPROVENORMGROUP_CMD_H_
#define TPS_BL_CMD_APPROVENORMGROUP_CMD_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

class RtBeam;
class TpsImage3DEntity;
class RtMachine;
class TpsIECCoordinateSystem;
class RtPoi;
class ROIEntity;
class RtApproverecord;

class TPS_BL_DECLSPEC TpsApproveNormGroupCmd : public TpsCommand
{
public:
    TpsApproveNormGroupCmd(RtApproverecord* approverecord);
    ~TpsApproveNormGroupCmd();
    std::vector<RtPoi*> GetApprovedPois() {return mApprovedPois;}
    std::vector<ROIEntity*> GetApprovedVois() {return mApprovedVois;}
protected:
    virtual int Execute();
private:
    bool GenerateDrrForBeam_i(RtBeam* beam, TpsImage3DEntity* image,
        RtMachine* tpsMachine, float* ct2Density, TpsIECCoordinateSystem* iecCs);
private:
    RtApproverecord*            mApproveRecord;
    std::vector<RtPoi*>       mApprovedPois;
    std::vector<ROIEntity*>     mApprovedVois;
};

TPS_END_NAMESPACE

#endif