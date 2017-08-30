////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_bl_cmd_voi_manset_density.h
/// 
///  \brief class TpsVoiMansetDensityCmd 
/// 
///  \version 1.0
/// 
///  \date    2015/10/26
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_VOI_MANSET_DENSITY_H_
#define TPS_BL_CMD_VOI_MANSET_DENSITY_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

class ROIEntity;

class TPS_BL_DECLSPEC TpsVoiMansetDensityCmd : public TpsCommand {
public:
    TpsVoiMansetDensityCmd(std::string& strVoiUid, float fOverrideMassDensity, float fOverrideReferenceEDensity, int nOverrideSequence);

    ~TpsVoiMansetDensityCmd();

    const ROIEntity* GetVoi() { return m_pVoiEntity; }

protected:

    virtual int PreExecute();

    virtual int Execute();

    virtual int PostExecute();

private:
    std::string                 m_strVoiUid;
    float                       m_fOverrideMassDensity;
    float                       m_fOverrideReferenceEDensity;
    int                         m_nOverrideSequence;
    ROIEntity*                  m_pVoiEntity;
};

TPS_END_NAMESPACE

#endif