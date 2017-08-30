////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_bl_cmd_voi_dose_volume_value.h
/// 
///  \brief class TpsVoiDoseVolumeValueCmd 
/// 
///  \version 1.0
/// 
///  \date    2015/10/26
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_VOI_DOSE_VOLUME_VALUE_H_
#define TPS_BL_CMD_VOI_DOSE_VOLUME_VALUE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC TpsVoiDoseVolumeValueCmd : public TpsCommand {
public:
    TpsVoiDoseVolumeValueCmd(std::string& strPlanUid, std::vector<std::string> vecVoiUid, std::vector<int> vecVoiVolume);

    ~TpsVoiDoseVolumeValueCmd();

    std::vector<float> GetVoiDoseVolumeValue() { return m_vecVoiDoseVolumeValue; }

protected:

    virtual int PreExecute();

    virtual int Execute();

    virtual int PostExecute();

private:
    std::string                 m_strPlanUid;
    std::vector<std::string>    m_vecVoiUid;
    std::vector<int>            m_vecVoiVolume;         // only support integer volume. this is the product requirement
    std::vector<float>          m_vecVoiDoseVolumeValue;
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_VOI_DOSE_VOLUME_VALUE_H_