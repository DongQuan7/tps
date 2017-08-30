////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_bl_mpr_show_fusion_cmd.h
/// 
///  \brief class FusionDeleteCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/10/16
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_MPR_SHOW_FUSION_H_
#define TPS_BL_CMD_MPR_SHOW_FUSION_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"              // for TpsCommand

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TPS_BL_DECLSPEC MprShowFusionCmd : public TpsCommand {
public:
    //the constructor and destructor
    MprShowFusionCmd(const std::string& strFixedSeriesUid, const std::string& strFloatSeriesUid, bool bVisible);
    virtual ~MprShowFusionCmd();

protected:
    virtual int Execute();

private:
    std::string m_strFixedSeriesUid;
    std::string m_strFloatSeriesUid;
    bool        m_bVisible;
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_BL_MPR_SHOW_FUSION_CMD_H_