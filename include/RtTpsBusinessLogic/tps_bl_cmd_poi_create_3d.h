////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author Xiaoqing.Shangguan  Xiaoqing.Shangguan@united-imaging.com
/// 
///  \file tps_cmd_poi_create_3d.h
/// 
///  \brief
/// 
///  \version 1.0
/// 
///  \date    2015/06/01
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_POI_CREATE_3D_H_
#define TPS_BL_CMD_POI_CREATE_3D_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certified_poi.h"

TPS_BEGIN_NAMESPACE

class RtPoi;

class TPS_BL_DECLSPEC Poi3dCreateCmd : public TpsCommand {

public:
    Poi3dCreateCmd(const CertifiedPoi &);

    virtual ~Poi3dCreateCmd();

    void GetPoiCreated(const RtPoi*& pPoi);
protected:
    virtual int Execute();

private:
    CertifiedPoi           mCertifiedPoi;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(Poi3dCreateCmd);
};

TPS_END_NAMESPACE
#endif