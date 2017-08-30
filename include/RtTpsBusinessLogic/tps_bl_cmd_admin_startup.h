//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  kai.zuo kai.zuo@united-imaging.com
///
///  \file    tps_cmd_adminstartup.h
///  \brief   admin startup command
///
///  \version 1.0
///  \date    Jan. 25, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_ADMIN_STARTUP_H_
#define TPS_BL_CMD_ADMIN_STARTUP_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

class RtPatient;
class RtSeries;

class TPS_BL_DECLSPEC AdminStartupCmd : public TpsCommand
{
public:
    AdminStartupCmd();
    virtual ~AdminStartupCmd(void);

    virtual void GetResult(std::vector<RtPatient*>& vPatientList,
        std::vector<RtSeries*>& vSeriesList);
protected:
    virtual int Execute();
private:
    int LoadPatients();
    int LoadSeries();

private:
    std::vector<RtPatient*> mvPatientList;
    std::vector<RtSeries*> mvSeriesList;
};

TPS_END_NAMESPACE

#endif //TPS_CCA_ADDVOI_CMD_H_