//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  kai.zuo kai.zuo@united-imaging.com
///
///  \file    tps_pca_updategridsizecmd.h
///  \brief   update gridsize command
///
///  \version 1.0
///  \date    Apr. 9, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_UPDATEGRIDSIZECMD_H
#define TPS_BL_UPDATEGRIDSIZECMD_H

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedgridsize.h"

TPS_BEGIN_NAMESPACE


class TPS_BL_DECLSPEC TpsUpdateGridSizeCMD : public TpsCommand{
public:
    TpsUpdateGridSizeCMD(CertifiedGridSize *pocoGridSize);
    virtual ~TpsUpdateGridSizeCMD(void);
protected:
    virtual int Execute();
private:
    CertifiedGridSize *mPocoGridSize;
};

TPS_END_NAMESPACE

#endif //TPS_BL_UPDATEGRIDSIZECMD_H