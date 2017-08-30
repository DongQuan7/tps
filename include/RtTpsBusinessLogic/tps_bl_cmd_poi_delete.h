////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Song LI  song.li@united-imaging.com
/// 
///  \file tps_cmd_poi_delete.h
/// 
///  \brief class PoiDeleteCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/12/09
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_POI_DELETE_H_
#define TPS_BL_CMD_POI_DELETE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certifieddelete.h"

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC PoiDeleteCmd : public TpsCommand {

public:
    PoiDeleteCmd(const CertifiedDelete &);

    virtual ~PoiDeleteCmd();

    CertifiedDelete GetCertified();

protected:
    virtual int Execute();

private:
    CertifiedDelete             m_stcCertifiedDelete;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(PoiDeleteCmd);
};

TPS_END_NAMESPACE
#endif