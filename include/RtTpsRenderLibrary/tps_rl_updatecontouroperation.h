////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_updatecontouroperation.h
/// 
///  \brief class TpsUpdateContourOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/06
////////////////////////////////////////////////////////////////

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h" 
#include "RtTpsFramework/tps_fw_mask.h"

TPS_BEGIN_NAMESPACE
class TPSMask;
class TPS_RL_DECLSPEC TpsUpdateContourOperation
    : public OperationBase {
public:
    TpsUpdateContourOperation(const std::string &roiUID,const TPSMask &mask,const std::string &imageUid);
    virtual ~TpsUpdateContourOperation();
    virtual bool ModifyGraphicObject() const;
private:
    std::string              mRoiUID;
    std::string              mImageUid;
    TPSMask                  mMask;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsUpdateContourOperation);


};
TPS_END_NAMESPACE