////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Wang.Zhendong zhendong.wang@united-imaging.com
/// 
///  \file    tps_rl_change_ct2densitytable_operation.h
/// 
///  \brief 
/// 
///  \version 1.0
/// 
///  \date    March. 22, 2016
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_CHANGE_CT2DENSITYTABLE_OPERATION_H_
#define TPS_RL_CHANGE_CT2DENSITYTABLE_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps


class TPS_RL_DECLSPEC ChangeCT2DensityTableOperation
    : public OperationBase {
public:

    ChangeCT2DensityTableOperation(const std::string& imageUid, const std::string& normUid,
        const std::string& ct2DensityTableUid);
    ~ChangeCT2DensityTableOperation();

    virtual bool ModifyGraphicObject() const;

private:
    mutable std::string mImageUid;
    mutable std::string mNormalgroupUid;
    mutable std::string mCt2DensityTableUid;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(ChangeCT2DensityTableOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
