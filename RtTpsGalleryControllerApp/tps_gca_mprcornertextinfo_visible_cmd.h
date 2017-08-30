//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_mprpageturncmd.h
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_GCA_MPRCORNERTEXTINFO_VISIBLE_CMD_H_
#define TPS_GCA_MPRCORNERTEXTINFO_VISIBLE_CMD_H_

#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsRenderController/tps_rc_renderdefines.h"

TPS_BEGIN_NAMESPACE
class TpsGcaMPRCornerTextInfoVisibleCmd : public TpsCommand
{
public:
    TpsGcaMPRCornerTextInfoVisibleCmd(const std::string& strImageUID, const bool& isVisible);
    virtual ~TpsGcaMPRCornerTextInfoVisibleCmd(void);
    //inline HIT_TEST_RESULT   GetHitTestResult() { return m_nHitTestResult;   } 

protected:
    virtual bool PreExecute();
    virtual bool Execute();
    virtual bool PostExecute();

    TpsGcaMPRCornerTextInfoVisibleCmd * Clone();


private:
    std::string             m_strImageUID;
    bool                    m_isVisible;
};



TPS_END_NAMESPACE

#endif //TPS_COMMON_MPRHITTEST_CMD_H_
