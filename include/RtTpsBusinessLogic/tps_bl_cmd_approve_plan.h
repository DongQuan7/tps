//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  xiaoqing.shangguan (mailto:xiaoqing.shangguan@united-imaging.com)
///
///  \file   tps_cca_approveplan_cmd.h
///  \brief   approve normgroup
///
///  \version 1.0
///  \date  2014/11/11  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_APPROVPLAN_CMD_H_
#define TPS_BL_CMD_APPROVPLAN_CMD_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

class RtPoi;
class ROIEntity;
class RtApproverecord;

class TPS_BL_DECLSPEC TpsApprovePlanCmd : public TpsCommand
{
public:
    TpsApprovePlanCmd(RtApproverecord* approveRecord);
    ~TpsApprovePlanCmd();
    std::vector<RtPoi*> GetApprovedPois() {return mApprovedPois;}
    std::vector<ROIEntity*> GetApprovedVois() {return mApprovedVois;}
protected:
    virtual int Execute();

private:
    RtApproverecord*               mApproveRecord;
    std::vector<RtPoi*>          mApprovedPois;
    std::vector<ROIEntity*>        mApprovedVois;
};

TPS_END_NAMESPACE

#endif