////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_refreshfusionfactoroperation.h
/// 
///  \brief class RefreshFusionFactor declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/05/16
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_REFRESHFUSIONFACTOROPERATION_H_
#define TPS_RL_REFRESHFUSIONFACTOROPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class RefreshFusionFactorOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC RefreshFusionFactorOperation
 : public OperationBase{
public:
    //the constructor and destructor
    RefreshFusionFactorOperation(const std::string& fixedImage,
        const std::string& floatImage, double fusionFactor);
    RefreshFusionFactorOperation(double fusionFactor);
    virtual ~RefreshFusionFactorOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string         mFixedImageUid;
    std::string         mFloatImageUid;
    double              mFusionFactor;
    bool                mIsFusionMode;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(RefreshFusionFactorOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif