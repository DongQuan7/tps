#ifndef TPS_BL_CMD_ROI_ADD_H_
#define TPS_BL_CMD_ROI_ADD_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE
struct CertifiedVOI;

class TPS_BL_DECLSPEC AddROICmd : public TpsCommand
{
public:
    AddROICmd(CertifiedVOI& certifiedVOI);
    virtual ~AddROICmd(void);

protected:
    virtual int Execute();

private:
    CertifiedVOI& mCertifiedVOI;
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_ROI_ADD_H_