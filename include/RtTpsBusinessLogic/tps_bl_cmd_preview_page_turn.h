//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  LI Song  song.li@united-imaging.com
///
///  \file    tps_bl_cmd_voi_preview_page_turn.h
///  \brief   VoiPreviewPageTurnCmd command
///
///  \version 1.0
///  \date    Oct. 21, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_PREVIEW_PAGE_TURN_H_
#define TPS_BL_CMD_PREVIEW_PAGE_TURN_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC PreviewPageTurnCmd : public TpsCommand
{
public:
    PreviewPageTurnCmd(const std::string& strWindowUid, int nWindowType, int nPageTurn, PREVIEW_TYPE nPreViewType);

    virtual ~PreviewPageTurnCmd(void);

protected:
    virtual int PreExecute();
    virtual int Execute();
    virtual int PostExecute();

    PreviewPageTurnCmd * Clone();

private:
    std::string                      m_strWindowUid;
    int                              m_nWindowType;
    int                              m_nPageTurn;
    PREVIEW_TYPE                     m_nPreViewType;
};



TPS_END_NAMESPACE

#endif //TPS_BL_CMD_PREVIEW_PAGE_TURN_H_
