////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_bl_cmd_voi_hot_spots.h
/// 
///  \brief class TpsVoiHotSpotsCmd 
/// 
///  \version 1.0
/// 
///  \date    2015/10/26
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_VOI_HOT_SPOTS_H_
#define TPS_BL_CMD_VOI_HOT_SPOTS_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC TpsVoiHotSpotsCmd : public TpsCommand {
public:
    TpsVoiHotSpotsCmd(std::string& strPlanUid, std::string& strVoiUid, float fDoseLimit, int nHotFlag);

    ~TpsVoiHotSpotsCmd();

protected:

    virtual int PreExecute();

    virtual int Execute();

    virtual int PostExecute();

private:
    std::string     m_strPlanUid;
    std::string     m_strVoiUid;
    float           m_fDoseLimit;
    int             m_nHotFlag;
};

TPS_END_NAMESPACE

#endif