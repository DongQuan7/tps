////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Xiaoqing.Shangguan Xiaoqing.Shangguan@united-imaging.com
/// 
///  \file    tps_rl_changect2densitytableoperation.h
/// 
///  \brief 
/// 
///  \version 1.0
/// 
///  \date    Mar. 25, 2015
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_CHANGECT2DENSITYTABLE_OPERATION_H_
#define TPS_RL_CHANGECT2DENSITYTABLE_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsChangeCT2DensityTableOperation
/// 
/// \brief add a VOI graphic object into ModelWareHouse
class TPS_RL_DECLSPEC UpdateDrrSettingOperation
    : public OperationBase{
public:
    //the constructor and destructor
    //param[in] the UID of image which the VOI belongs to
    //param[in] the UID of the VOI to be added in database
    UpdateDrrSettingOperation(const std::string& imageUid, const std::string& normUid, 
        const float& ctthresholdMin, const float& ctthresholdMax, const std::string& ct2DensityTableUid, const float& electronEnergy);
    ~UpdateDrrSettingOperation();

    virtual bool ModifyGraphicObject() const;

private:
    mutable std::string mImageUid;
    mutable std::string mNormalgroupUid;
    mutable float       mctThresholdMin;
    mutable float       mctThresholdMax;
    mutable std::string mCt2DensityTableUid;
    mutable float       mElectronEnergy;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(UpdateDrrSettingOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
