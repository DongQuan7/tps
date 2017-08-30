////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  WANG Jie  jie.wang@united-imaging.com
/// 
///  \file tps_fw_prepare_notifier.cpp
/// 
///  \brief class TpsMCPrepareNotifier declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/10/09
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_prepare_notifier.h"
#include "RtTpsFramework/tps_fw_active_responser_interface.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsMCPrepareNotifier::TpsMCPrepareNotifier() :
completeFilesNumber(0), mPrepareResponser(nullptr)
{

}

TpsMCPrepareNotifier::~TpsMCPrepareNotifier()
{

}

void TpsMCPrepareNotifier::SetPrepareResponser(
    const std::shared_ptr<PrepareActiveResponser>& responser)
{
    mPrepareResponser = responser;
}

int TpsMCPrepareNotifier::GetProgress(void) const
{
    return completeFilesNumber;
}


void TpsMCPrepareNotifier::NotifyProgress(int i, int errortype)
{
    completeFilesNumber = i;
    if (mPrepareResponser){
        mPrepareResponser->Response(completeFilesNumber,errortype);
    }

    errortype;
}
TPS_END_NAMESPACE  // end namespace tps