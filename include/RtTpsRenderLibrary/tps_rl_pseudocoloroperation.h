////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_pseudocoloroperation.h
/// 
///  \brief class PseudoColorOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/05/29
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_PSEUDOCOLOROROPERATION_H_
#define TPS_RL_PSEUDOCOLOROROPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class RefreshFusionFactorOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC PseudoColorOperation
 : public OperationBase{
public:
    //the constructor and destructor
    PseudoColorOperation(const std::string& strPseudoColorName);
    virtual ~PseudoColorOperation();

    virtual bool ModifyGraphicObject() const;

    inline void SetGraphicObjectKeyList(
        const std::vector<std::string>& vecKeys) { mVecGoKeys = vecKeys; }
private:
    std::string                 m_strPseudoColorName;
    std::vector<std::string>    mVecGoKeys;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(PseudoColorOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif