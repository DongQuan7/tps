////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZUO Kai  kai.zuo@united-imaging.com
/// 
///  \file tps_rl_showdosegridboundaryoperation.h
/// 
///  \brief class TpsShowDoseGridBoundaryOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/04/11
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SHOWDOSEGRIDBOUNDARYOPERATION_H_
#define TPS_RL_SHOWDOSEGRIDBOUNDARYOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

    /// \class TpsEditDoseGridBoundaryOperation
    /// 
    /// \brief edit dose grid boundary operation
class TPS_RL_DECLSPEC TpsShowDoseGridBoundaryOperation
    : public OperationBase{
public:
    //the constructor and destructor
    TpsShowDoseGridBoundaryOperation(const std::string planUID, bool isShowBoundary);

    ~TpsShowDoseGridBoundaryOperation();

    virtual bool ModifyGraphicObject() const;

private :


private:
    std::string mPlanUID;
    bool mIsShowBoundary;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsShowDoseGridBoundaryOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif