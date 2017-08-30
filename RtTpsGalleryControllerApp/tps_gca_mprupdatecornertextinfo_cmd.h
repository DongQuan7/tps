//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  shaosen.huang shaosen.huang@united-imaging.com
//
/// \file    tps_gca_mprupdatecornertextinfo_cmd.h
///
/// \brief   MPRUpdateCornerTextInfoCmd
/// 
/// \version 1.0
/// 
/// \date    2015/04/10
/////////////////////////////////////////////////////////////////////////


#ifndef TPS_GCA_MPR_UPDATE_CORNER_INFO_CMD_H
#define TPS_GCA_MPR_UPDATE_CORNER_INFO_CMD_H

#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsRenderController/tps_rc_renderdefines.h"
#include "RtTpsProtoLib/rt_tps_certifiedmouseposinfo.h"

namespace Mcsf{
    namespace MedViewer3D{
        class RenderStrategyGPU;
        class IGraphicObject;
        class Point3D;
        class Point2D;
    }
}

TPS_BEGIN_NAMESPACE
class TpsGcaMPRUpdateCornerTextInfoCmd : public TpsCommand
{
public:
    TpsGcaMPRUpdateCornerTextInfoCmd(const CertifiedMousePosInfo& certifedMousePosInfo);
    virtual ~TpsGcaMPRUpdateCornerTextInfoCmd();

protected:
    virtual bool PreExecute();
    virtual bool Execute();
    virtual bool PostExecute();

    TpsGcaMPRUpdateCornerTextInfoCmd *Clone();

private:
    CertifiedMousePosInfo       mCertifiedMousePosInfo;
};

TPS_END_NAMESPACE
#endif //TPS_COMMON_UPDATE_CORNER_INFO_CMD_H