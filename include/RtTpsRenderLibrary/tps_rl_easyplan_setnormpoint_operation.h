////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  wenlong.zhao  wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_setnormpoint_operation.h 
/// 
///  \brief class TpsEasyPlanSetNormPointOperation
/// 
///  \version 1.0
/// 
///  \date    2015/07/18
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANSETNORMPOINT_H_
#define TPS_RL_EASYPLANSETNORMPOINT_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsEasyPlanSetNormPointOperation
    : public OperationBase {
public:
    TpsEasyPlanSetNormPointOperation(const std::string &planUid,const std::string &poiUid, const std::string &imageUid, int displayWidth, int displayHeight);
    ~TpsEasyPlanSetNormPointOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string                  mPlanUid;
    std::string                  mPoiUid;
    std::string                  mImageUid;
    int                          mWidth;
    int                          mHeight;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanSetNormPointOperation);
};
TPS_END_NAMESPACE

#endif