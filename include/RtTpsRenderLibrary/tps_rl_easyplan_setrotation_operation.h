////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong.Zhao wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_setrotation_operation.h
/// 
///  \brief class TpsEasyPlanSetRotationOperation
/// 
///  \version 1.0
/// 
///  \date    2014/11/26
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANSETROTATIONOPERATION_H_
#define TPS_RL_EASYPLANSETROTATIONOPERATION_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

namespace Mcsf {
	class Matrix3f;
}
TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsEasyPlanSetRotationOperation
    : public OperationBase {
public:
    TpsEasyPlanSetRotationOperation(const std::string &drrImageUID, const std::string &planUID,
        const float startX, const float startY, const float stopX, const float stopY);
    ~TpsEasyPlanSetRotationOperation();
    virtual bool ModifyGraphicObject() const;
private:
	Mcsf::Matrix3f RotateMatrix(Mcsf::Matrix3f matrix) const;
private:
    std::string                     mPlanUID;
    std::string                     mDrrImageUID;
    float                           mStartX;
    float                           mStartY;
    float                           mStopX;
    float                           mStopY;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanSetRotationOperation);
};
TPS_END_NAMESPACE

#endif
