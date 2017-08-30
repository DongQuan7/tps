////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  Zhendong Wang  zhendong.wang@united-imaging.com
/// 
///  \file tps_bl_cmd_beam_block_update.h
/// 
///  \brief class  BeamBlockUpdateCmd 
/// 
///  \version 1.0
/// 
///  \date    January.15,2016
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_BEAM_BLOCK_UPDATE_H_
#define TPS_BL_CMD_BEAM_BLOCK_UPDATE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include <vector>

TPS_BEGIN_NAMESPACE

struct TpsCertifiedBlock;

class TPS_BL_DECLSPEC BeamBlockUpdateCmd : public TpsCommand
{
public:
    BeamBlockUpdateCmd(const std::string &beamUid, const std::shared_ptr<TpsCertifiedBlock>& block, int blockType);
    virtual ~BeamBlockUpdateCmd(void);

protected:
    virtual int Execute();

private:
    std::shared_ptr<TpsCertifiedBlock>   m_block;
    int                                  m_blockType;
    std::string                          m_beamUid;
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_BEAM_SEGMENT_UPDATE_H_
