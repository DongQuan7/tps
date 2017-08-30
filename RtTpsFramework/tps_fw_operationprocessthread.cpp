//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_fw_operationprocessthread.cpp
///
/// \brief   OperationProcessThread
/// 
/// \version 1.0
/// 
/// \date    2013/12/11
/////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_operationprocessthread.h"

TPS_BEGIN_NAMESPACE

OperationProcessThread::OperationProcessThread() {

}

OperationProcessThread::~OperationProcessThread() {

}

void OperationProcessThread::Synchronize() {
    if (this->IsQueueEmpty()) {
        return;
    }

    std::shared_ptr<Viewer3D::SyncCommandBase> emptySyncCommand(new EmptySyncCommand);
    this->PutCommand(emptySyncCommand);
    emptySyncCommand->Wait();
}

TPS_END_NAMESPACE
