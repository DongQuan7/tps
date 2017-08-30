////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_activechesseditoperation.h
/// 
///  \brief class ActiveChessEdit declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/06/03
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_ACTIVECHESSEDITOPERATION_H_
#define TPS_RL_ACTIVECHESSEDITOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class ActiveChessEditOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC ActiveChessEditOperation
 : public OperationBase{
public:
    //the constructor and destructor
    ActiveChessEditOperation(bool bActive);
    virtual ~ActiveChessEditOperation();

    virtual bool ModifyGraphicObject() const;

    inline void SetFusionGoKey(const std::string& key) { mFusionGoKey = key; }

private:
    bool        mActive;
    std::string mFusionGoKey;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(ActiveChessEditOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif