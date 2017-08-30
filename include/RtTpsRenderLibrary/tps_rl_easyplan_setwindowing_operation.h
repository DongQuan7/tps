////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong.Zhao wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_setwindowing_operation.h
/// 
///  \brief class TpsEasyPlanSetWindowingOperation
/// 
///  \version 1.0
/// 
///  \date    2014/11/06
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANSETWINDOWINGOPERATION_H_
#define TPS_RL_EASYPLANSETWINDOWINGOPERATION_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsEasyPlanSetWindowingOperation
    : public OperationBase {
public:
    TpsEasyPlanSetWindowingOperation(const std::string& planUID, const std::string &drrImageUID,
        const int winWidth, const int winCenter);
    ~TpsEasyPlanSetWindowingOperation();
    virtual bool ModifyGraphicObject() const;
    void GetCurrentWinWidthAndWinCenter(int& currentWinWidth, int& currentWinCenter);

private:
    std::string                     mPlanUID;
    std::string                     mDrrImageUID;
    int                             mDeltaWinWidth;
    int                             mDeltaWinCenter;
    mutable int                     mCurrentWinWidth;
    mutable int                     mCurrentWinCenter;



private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanSetWindowingOperation);
};
TPS_END_NAMESPACE

#endif