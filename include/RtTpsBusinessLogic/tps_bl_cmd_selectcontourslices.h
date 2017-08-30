//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  kai.zuo kai.zuo@united-imaging.com
///
///  \file    tps_cca_selectcontourslices_cmd.h
///  \brief   selectcontourSlcie command
///
///  \version 1.0
///  \date    06. 12, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_SELECTCONTOURINGSLICES_CMD_H_
#define TPS_BL_SELECTCONTOURINGSLICES_CMD_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

#include "RtTpsCertifiedLib/rt_tps_certifiedselectcontourslices.h"

namespace Mcsf
{
    class Point2f;
}
TPS_BEGIN_NAMESPACE

class TpsImage3DEntity;

class TPS_BL_DECLSPEC SelectContourSlicesCmd : public TpsCommand
{
public:
    SelectContourSlicesCmd(CertifiedSelectContourSlices certifiedSelectContourSlices);
    virtual ~SelectContourSlicesCmd(void);

    CertifiedSelectContourSlices GetCertifiedSelectContourSlices();

protected:
    virtual int Execute();
    virtual int PostExecute();

private:
    CertifiedSelectContourSlices mCertifiedSelectContourSlices;
    /*static*/  Mcsf::Point2f        *mFirstPoint;
    bool                         mContourDeleted;
};
TPS_END_NAMESPACE

#endif //TPS_CCA_SELECTCONTOURINGSLICES_CMD_H_