////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_voiredoundocommandbase.cpp
/// 
///  \brief class TpsVOIRedoUndoCmdBase implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/04/22
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_voiredoundocommandbase.h"
#include "RtTpsFramework/tps_fw_voiedtingcache.h"
#include "RtTpsFramework/tps_fw_commandmanager.h"
#include "RtTpsFramework/tps_fw_safelock.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsVOIRedoUndoCacheCmd::TpsVOIRedoUndoCacheCmd(const std::string& uid)
: TpsVOIRedoUndoCmdBase(uid), mPreState(nullptr), mPostState(nullptr){

}

TpsVOIRedoUndoCacheCmd::~TpsVOIRedoUndoCacheCmd(){

}

void TpsVOIRedoUndoCacheCmd::SetPreState(std::shared_ptr<TpsVOIEditingCache> state){
    mPreState = state;
    mPreState->SetVoiUid(mVoiUid);
}

void TpsVOIRedoUndoCacheCmd::SetPostState(std::shared_ptr<TpsVOIEditingCache> state){
    mPostState = state;
    mPostState->SetVoiUid(mVoiUid);
}

std::shared_ptr<TpsVOIEditingCache> TpsVOIRedoUndoCacheCmd::GetPreState() const{
    return mPreState;
}

std::shared_ptr<TpsVOIEditingCache> TpsVOIRedoUndoCacheCmd::GetPostState() const{
    return mPostState;
}

int TpsVOIRedoUndoCacheCmd::PreExecute() {
    if (TPS_ER_SUCCESS != TpsVOIRedoUndoCmdBase::PreExecute()){
        return TPS_ER_FAILURE;
    }

    auto cmdMgr = mDataRepository->GetCommandManager();
    if (nullptr == cmdMgr){
        TPS_LOG_DEV_ERROR<<"The command manager is null.";
        return TPS_ER_FAILURE;
    }

    {
        TpsSafeLock<TpsCommandManager> lock(cmdMgr);
        TpsCommand* cmd = cmdMgr->GetBackupCommand();
        auto backupCmd = dynamic_cast<TpsVOIRedoUndoCmdBase*>(cmd);
        //if backup command does not exist or is not a voi redo/undo command, 
        //or the existing backup command does not belong to current VOI, register.
        if (!backupCmd || backupCmd->GetVoiUid() != mVoiUid){
            cmdMgr->RegisterBackupCommand(this->Clone());
        }
    }
    return TPS_ER_SUCCESS;
}

int TpsVOIRedoUndoCacheCmd::PostExecute() {
    return TPS_ER_SUCCESS;
}

bool TpsVOIRedoUndoCacheCmd::Ready() const{

    return mPreState != nullptr && mPostState != nullptr;
}
TPS_END_NAMESPACE  // end namespace tps