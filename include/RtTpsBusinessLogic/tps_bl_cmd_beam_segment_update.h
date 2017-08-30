////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  Song LI  song.li@united-imaging.com
/// 
///  \file tps_bl_cmd_beam_segment_update.h
/// 
///  \brief class  BeamSegmentUpdateCmd 
/// 
///  \version 1.0
/// 
///  \date    Oct.15,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_BEAM_SEGMENT_UPDATE_H_
#define TPS_BL_CMD_BEAM_SEGMENT_UPDATE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include <vector>

TPS_BEGIN_NAMESPACE

struct TpsCertifiedSegment;

class TPS_BL_DECLSPEC BeamSegmentUpdateCmd : public TpsCommand
{
public:
    BeamSegmentUpdateCmd(const std::vector<std::shared_ptr<TpsCertifiedSegment>>& vecSegment);
    virtual ~BeamSegmentUpdateCmd(void);

protected:
    virtual int Execute();

private:
    std::vector<std::shared_ptr<TpsCertifiedSegment>>   m_vecCeritifedSegmentPtr;
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_BEAM_SEGMENT_UPDATE_H_
