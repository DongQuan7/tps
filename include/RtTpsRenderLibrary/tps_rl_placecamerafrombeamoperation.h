

#ifndef TPS_RL_PLACECAMERAFROMBEAMOPERATION_H_
#define TPS_RL_PLACECAMERAFROMBEAMOPERATION_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC PlaceCameraFromBeamOperation
    : public OperationBase {
public:
    PlaceCameraFromBeamOperation(const std::string &planUID);
    ~PlaceCameraFromBeamOperation();
    virtual bool ModifyGraphicObject() const;
private:
    std::string   mPlanUID;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(PlaceCameraFromBeamOperation);
};
TPS_END_NAMESPACE

#endif