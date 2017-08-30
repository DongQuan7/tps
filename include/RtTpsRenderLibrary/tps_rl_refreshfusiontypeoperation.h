////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_refreshfusiontypeoperation.h
/// 
///  \brief class RefreshFusionType declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/05/29
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_REFRESHFUSIONTYPEOPERATION_H_
#define TPS_RL_REFRESHFUSIONTYPEOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class RefreshFusionTypeOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC RefreshFusionTypeOperation
 : public OperationBase{
public:
    //the constructor and destructor
    RefreshFusionTypeOperation(const std::string& fixedImage,
        const std::string& floatImage, int fusionType);
    RefreshFusionTypeOperation(int fusionType);
    virtual ~RefreshFusionTypeOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string         mFixedImageUid;
    std::string         mFloatImageUid;
    int                 mFusionType;
    bool                mIsFusionMode;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(RefreshFusionTypeOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif