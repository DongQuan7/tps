////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_pca_planningdeletecmd.h
/// 
///  \brief class TpsPlanningDeleteCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/3/08
//////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_DELETE_PLANS_H_
#define TPS_BL_CMD_DELETE_PLANS_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE
class ITpsCommonController;
class TPS_BL_DECLSPEC TpsPlanningDeleteCmd : public TpsCommand 
{
public:
    //iType 0: plan 1:normgroup 3:beam
    TpsPlanningDeleteCmd(std::string sUid, int iType);

    virtual ~TpsPlanningDeleteCmd();

protected:
    virtual int Execute();

private:
    int DeleteBeam_i(const std::string &beamUid);
    int DeleteNormGroup_i(const std::string& normGroupUID);
    int DeletePlan_i(const std::string &planUid);
    int DeleteDRRImage(const std::string &beamUid);

private:
    std::string     mUid;
    int             mType;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsPlanningDeleteCmd);
};

TPS_END_NAMESPACE

#endif
