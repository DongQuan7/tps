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
#ifndef TPS_GCA_MPRHITTEST_CMD_H_
#define TPS_GCA_MPRHITTEST_CMD_H_

#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsRenderController/tps_rc_renderdefines.h"
#include "RtTpsProtoLib/rt_tps_certifiedmouseposinfo.h"

TPS_BEGIN_NAMESPACE
class TpsGcaMPRHitTestCmd : public TpsCommand
{
public:
    TpsGcaMPRHitTestCmd(const CertifiedMousePosInfo& protoMousePosInfo);
    TpsGcaMPRHitTestCmd(std::string& strImageUID, FIRST_POSTFIX_COMPONENT strSectionType, double x, double y);
    virtual ~TpsGcaMPRHitTestCmd(void);
    inline HIT_TEST_RESULT   GetHitTestResult() { return m_nHitTestResult;   } 

protected:
    virtual bool PreExecute();
    virtual bool Execute();
    virtual bool PostExecute();

    TpsGcaMPRHitTestCmd * Clone();


private:
    //CertifiedMousePosInfo       m_protoMousePosInfo;
    std::string             m_strImageUID;
    FIRST_POSTFIX_COMPONENT             m_strSectionType;
    double                  m_fX;
    double                  m_fY;
    HIT_TEST_RESULT               m_nHitTestResult;
    //LAYOUT_UNIT                 mUnit; 
    //int                         mPageturn;
    //std::string                 mImageUid;
    //std::string                 mSectionType;
};



TPS_END_NAMESPACE

#endif //TPS_COMMON_MPRHITTEST_CMD_H_
