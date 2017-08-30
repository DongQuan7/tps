////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_cca_movevoi_cmd.h
/// 
///  \brief class MoveVOICmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/6/19
////////////////////////////////////////////////////////////////

#ifndef TPS_CCA_MOVEVOI_CMD_H_
#define TPS_CCA_MOVEVOI_CMD_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedmovevoi.h"

namespace McsfGeometry { 
    class SubVolume;
}


TPS_BEGIN_NAMESPACE
class ContouringControllerApp;

class TPS_BL_DECLSPEC MoveVOICmd : public TpsCommand {

public:
    MoveVOICmd(const CertifiedMoveVOI &);

    virtual ~MoveVOICmd();

    CertifiedMoveVOI MoveVOICmd::GetCertifiedVOI();
protected:
    virtual int Execute();
    virtual int PostExecute();

private:
    bool UpdateVOI_i(const std::string &voiUID);
    bool MoveVOI_i();
    bool MoveCurrentSliceVOI_i();
    int Round_i(double x);
private:
    CertifiedMoveVOI             mCertifiedMoveVOI;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(MoveVOICmd);
};

TPS_END_NAMESPACE
#endif