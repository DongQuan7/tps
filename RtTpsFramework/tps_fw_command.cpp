//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  jiandong.zhou     jiandong.zhou@united-imaging.com
/// 
///  \file tps_command.cpp
/// 
///  \brief  define command of internal for outside

///  \version 1.0
///  \date    Aug. 6, 2013
//////////////////////////////////////////////////////////////////////////

#include"StdAfx.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsFramework/tps_fw_commandmanager.h"
#include "RtTpsFramework/tps_fw_safelock.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "tps_logger.h"
TPS_BEGIN_NAMESPACE

TpsCommand::TpsCommand() : mDataRepository(nullptr), mDatabaseWrapper(nullptr),
mLoadObjects(nullptr), mSaveObjects(nullptr), mRenderProxy(nullptr){

}

TpsCommand::~TpsCommand(void) {

}

int TpsCommand::RunCommand() {
    int iRet;
    iRet = PreExecute();
    if (iRet != TPS_ER_SUCCESS)
    {
#ifdef _DEBUG
TPSPRINTF("\nPreExecute error!");
#endif
        TPS_LOG_DEV_ERROR<<"Pre-Execute failed!";
        return iRet;
    }

    iRet = Execute();
    if (iRet != TPS_ER_SUCCESS)
    {
#ifdef _DEBUG
        TPSPRINTF("\nExecute error!");
#endif
        TPS_LOG_DEV_ERROR<<"Execute failed!";
        return iRet;
    }

    iRet = PostExecute();
    if (iRet != TPS_ER_SUCCESS)
    {
#ifdef _DEBUG
        TPSPRINTF("\nPostExecute error!");
#endif
        TPS_LOG_DEV_ERROR<<"Post-Execute failed!";
        return iRet;
    }
    return TPS_ER_SUCCESS;
}

int TpsCommand::PreExecute() 
{
    if (mDatabaseWrapper == nullptr){
#ifdef _DEBUG
        TPSPRINTF("\nDatabase wrapper is null!");
#endif
        TPS_LOG_DEV_ERROR<<"Database wrapper is null!";
        return TPS_ER_FAILURE;
    }
    
    if (mDataRepository == nullptr){
#ifdef _DEBUG
        TPSPRINTF("\nData repository is null!");
#endif
        TPS_LOG_DEV_ERROR<<"Data repository is null!";
        return TPS_ER_FAILURE;
    }

    if (mRenderProxy == nullptr){
#ifdef _DEBUG
        TPSPRINTF("\nRender proxy is null!");
#endif
        TPS_LOG_DEV_ERROR<<"Render proxy is null!";
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int TpsCommand::Execute() {
    return TPS_ER_SUCCESS;
}

int TpsCommand::PostExecute() {
    return TPS_ER_SUCCESS;
}

TpsRedoUndoCommand::TpsRedoUndoCommand(){

}

TpsRedoUndoCommand::~TpsRedoUndoCommand(void){

}

int TpsRedoUndoCommand::Undo(){
    return TPS_ER_FAILURE;
}

int TpsRedoUndoCommand::Redo(){
    return Execute();
}

bool TpsRedoUndoCommand::Ready() const{
    return true;
}

int TpsRedoUndoCommand::PostExecute(){
    auto cmdMng = mDataRepository->GetCommandManager();
    if (cmdMng == nullptr){
        TPS_LOG_DEV_ERROR<<"Command manager is empty.";
        return TPS_ER_FAILURE;
    }

    TpsSafeLock<TpsCommandManager> lock(cmdMng);
    cmdMng->RegisterCommand(this->Clone());
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
