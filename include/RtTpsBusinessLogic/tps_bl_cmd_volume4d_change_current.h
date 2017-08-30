//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  LI Song  song.li@united-imaging.com
///
///  \file    tps_bl_cmd_volume4d_change_current.h
///  \brief   Volume4dChangeCurrentCmd command
///  \class   Volume4dChangeCurrentCmd
///
///  \version 1.0
///  \date    July 30, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_VOLUME4D_CHANGE_CURRENT_H_
#define TPS_BL_CMD_VOLUME4D_CHANGE_CURRENT_H_

#include "RtTpsFramework/tps_fw_command.h"

#include <vector>

TPS_BEGIN_NAMESPACE

class SimpleGuiResponser;

class Volume4dChangeCurrentCmd : public TpsCommand
{
public:
    Volume4dChangeCurrentCmd(const std::string& strSeriesUid, int nControl, std::shared_ptr<SimpleGuiResponser> spResponser,bool bIsCyclic);

    virtual ~Volume4dChangeCurrentCmd(void);

protected:
    virtual int PreExecute();
    virtual int Execute();
    virtual int PostExecute();

    Volume4dChangeCurrentCmd * Clone();

private:
    std::string                         m_strSeriesUid;
    int                                 m_nControl;                 // 0: play    1: stop
    std::shared_ptr<SimpleGuiResponser> m_spResponser;
    bool                                m_bIsCyclic;
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_VOLUME4D_CHANGE_CURRENT_H_
