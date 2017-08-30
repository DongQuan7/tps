//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  kai.zuo kai.zuo@united-imaging.com
///
///  \file    tps_cca_operatecontourslices_cmd.h
///  \brief   operatecontour command
///
///  \version 1.0
///  \date    June. 10, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_OPERATECONTOURSLICES_CMD_H_
#define TPS_BL_OPERATECONTOURSLICES_CMD_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

#include "RtTpsCertifiedLib/rt_tps_certifiedcontourslices.h"

TPS_BEGIN_NAMESPACE

class TpsImage3DEntity;

class TPS_BL_DECLSPEC OperateContourCmd : public TpsCommand
{
public:
    OperateContourCmd(CertifiedContoursSlices certifiedContoursSlices);
    virtual ~OperateContourCmd(void);
    CertifiedContoursSlices GetCertifiedContoursSlices();

protected:

    virtual int Execute();
    virtual int PostExecute();
private:
    bool UpdateVOI_i(const std::string &voiUID);

private:
    CertifiedContoursSlices mCertifiedContourSlices;
};



TPS_END_NAMESPACE

#endif //TPS_BL_OPERATECONTOURSLICES_CMD_H_