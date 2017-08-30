////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  xiaowei.li@united-imaging.com
/// 
///  \file tps_cmd_create_patient.h
/// 
///  \brief class 
/// 
///  \version 1.0
/// 
///  \date    July 27,2017
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_CREATE_PATIENT_H_
#define TPS_BL_CMD_CREATE_PATIENT_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsProtoLib/rt_tps_proto_patient.pb.h"

TPS_BEGIN_NAMESPACE

class RtPatient;

class TPS_BL_DECLSPEC TpsCreatePatientCmd : public TpsCommand{
public:
    TpsCreatePatientCmd(const proto::RtTpsProtoPatient &protoPatient);
    virtual ~TpsCreatePatientCmd(void);

    void GetPatient(const RtPatient*& pPatient);
protected:
    virtual int Execute();

private:
    proto::RtTpsProtoPatient    mProtoPatient;
    RtPatient*                  mPatient;
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_CREATE_PATIENT_H_
