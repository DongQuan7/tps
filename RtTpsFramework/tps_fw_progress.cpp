////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_progress.cpp
/// 
///  \brief class TpsProgress implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/03/30
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_progress.h"
#include "RtTpsFramework/tps_fw_active_responser_interface.h"
//#include "RtTpsFramework/tps_fw_commproxy.h"
//#include "RtTpsFramework/tps_fw_commandids.h"

//#include "boost/lexical_cast.hpp"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsProgress::TpsProgress() : mProgess(0.0), mMessage(""), mNeedCancel(false), 
mWeight(1.0), mStartVal(0.0), mProgressResponser(nullptr)
{

}

TpsProgress::~TpsProgress()
{

}

const std::string& TpsProgress::Message(void) const
{
    return mMessage;
}

void TpsProgress::SetMessage(const std::string& msg)
{
    mMessage = msg;
    OnProgressChanged_i();
}

double TpsProgress::Current(void) const
{
    return (mStartVal + mProgess * mWeight);
}

void TpsProgress::SetCurrent(double progress)
{
    mProgess = progress;
    mProgess = TPS_CLAMP(mProgess, 0.0, 1.0);;
    OnProgressChanged_i();
}

void TpsProgress::AddCurrent(double progress)
{
    mProgess = TPS_MIN(progress + mProgess, 1.0);
    OnProgressChanged_i();
}

bool TpsProgress::GetCancelFlag(void) const
{
    return mNeedCancel;
}

void TpsProgress::SetCancelFlag(bool bCancelFlag)
{
    mNeedCancel = bCancelFlag;
    OnProgressChanged_i();
}

void TpsProgress::OnProgressChanged_i() const
{
    if (mProgressResponser){
        mProgressResponser->Response(this->Current(), mNeedCancel, mMessage);
    }
//     Mcsf::ICommunicationProxy* proxy = TpsCommProxy::GetInstance()->GetMcsfProxy();
//     if (proxy == nullptr){
//         TPS_LOG_DEV_ERROR<<"The communication proxy is null, can not send progress value.";
//         return;
//     }
// 
//     Mcsf::CommandContext context;
//     context.iCommandId = UPDATE_PROGRESS;
//     context.sReceiver = TpsCommProxy::GetInstance()->GetTpsFEName();
//     context.sSerializeObject = boost::lexical_cast<std::string>(mNeedCancel) + "|" + 
//         boost::lexical_cast<std::string>(mStartVal + mProgess * mWeight) + "|" + mMessage;
//     proxy->AsyncSendCommand(&context);
}

void TpsProgress::SetProgressWeight(double weight)
{
    mWeight = weight;
}

void TpsProgress::SetProgressStartValue(double value)
{
    mStartVal = value;
}

void TpsProgress::Reset()
{
    mWeight = 1.0;
    mStartVal = 0.0;
}

void TpsProgress::SetProgressResponser(
    const std::shared_ptr<ProgressActiveResponser>& responser)
{
    mProgressResponser = responser;
}
TPS_END_NAMESPACE  // end namespace tps