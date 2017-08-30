////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_cmd_setmlctransparency.cpp
/// 
///  \brief class SetMlcTransparencyCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    Apr.16,2015
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_setmlctransparency.h"
#include "RtTpsRenderLibrary/tps_rl_setmlctransparency_operation.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsFramework/tps_fw_fire_operation.h"

TPS_BEGIN_NAMESPACE;   // begin namespace tps

SetMlcTransparencyCmd::SetMlcTransparencyCmd(float fTransparency) : mTransparency(fTransparency)
{

}

SetMlcTransparencyCmd::~SetMlcTransparencyCmd()
{

}

int SetMlcTransparencyCmd::Execute()
{
    float fTrans = mTransparency;
    fTrans = fTrans < 0.0 ? 0.0 : fTrans;
    fTrans = fTrans > 1.0 ? 1.0 : fTrans;
    std::shared_ptr<SetMlcTransparencyOperation> setMlc(new SetMlcTransparencyOperation(fTrans));
    if(!mRenderProxy->ProcessOperation(setMlc)){
        TPS_LOG_DEV_ERROR<<"Failed to set mlc transparency.";
        return TPS_ER_FAILURE;
    }
    std::shared_ptr<TpsFireOperation> fire(new TpsFireOperation());
    mRenderProxy->ProcessOperation(fire);
    return TPS_ER_SUCCESS;
}
TPS_END_NAMESPACE  // end namespace tps