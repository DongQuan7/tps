//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  jiandong.zhou jiandong.zhou@united-imaging.com
///
///  \file    tps_fw_commandmanager.cpp
///  \brief   bev windows
///
///  \version 1.0
///  \date    Oct. 9, 2013
///  \{
//////////////////////////////////////////////////////////////////////////
#include"StdAfx.h"
#include "RtTpsFramework/tps_fw_commandmanager.h"

#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsFramework/tps_fw_active_responser_interface.h"
#include "tps_logger.h"
TPS_BEGIN_NAMESPACE

TpsCommandManager::TpsCommandManager() : mBackupCommand(nullptr), mResponser(nullptr)
{
    mUndoList.clear();
    mRedoList.clear();
}

TpsCommandManager::~TpsCommandManager() 
{
    //release undo/redo List
    Reset();
}

void TpsCommandManager::RegisterCommand(TpsRedoUndoCommand* command) 
{
    if (command == nullptr || !command->Ready()) return;

    //clear redo list
    for (TpsCommandListIt it = mRedoList.begin();
        it != mRedoList.end(); ++it) {
        delete *it;
    }
    mRedoList.clear();

    //limited undo list count
    if (mUndoList.size() == UNDO_LIST_MAX_NUM) {
        delete mUndoList.back();
        mUndoList.pop_back();
    }

    //insert new command to undo list
    mUndoList.push_front(command);
    OnCommandListChanged();
}

bool TpsCommandManager::Undo() 
{
    if (mUndoList.empty()) return false; 

    TpsRedoUndoCommand* cmd = mUndoList.front();
    while(cmd == nullptr && !mUndoList.empty()){
        mUndoList.pop_front();
        cmd = mUndoList.front();
        OnCommandListChanged();
    }

    if (cmd->Undo() != TPS_ER_SUCCESS) {
        TPS_LOG_DEV_ERROR<<"The undo command failed.";
        return false;
    }

    //Insert the command to RedoList
    mRedoList.push_front(cmd);
    mUndoList.pop_front();
    OnCommandListChanged();
    return true;
}

bool TpsCommandManager::Redo() 
{
    if (mRedoList.empty()) return false;

    TpsRedoUndoCommand* cmd = mRedoList.front();
    while(cmd == nullptr && !mRedoList.empty()){
        mRedoList.pop_front();
        cmd = mRedoList.front();
        OnCommandListChanged();
    }

    if (cmd == nullptr) {
        TPS_LOG_DEV_ERROR<<"The redo command list is empty.";
        return false;
    }

    if (cmd->Redo() != TPS_ER_SUCCESS) {
        TPS_LOG_DEV_ERROR<<"The redo command failed.";
        return false;
    }

    //Insert the command to UndoList
    mUndoList.push_front(cmd);
    mRedoList.pop_front();
    OnCommandListChanged();
    return true;
}

TpsRedoUndoCommand* TpsCommandManager::GetUndoCommand_i() {
    return mUndoList.empty() ? nullptr : mUndoList.front();
}

TpsRedoUndoCommand* TpsCommandManager::GetRedoCommand_i() {
    return mRedoList.empty() ? nullptr : mRedoList.front();
}

bool TpsCommandManager::Reset() 
{
    //clear undo list
    for (TpsCommandListIt it = mUndoList.begin();
        it != mUndoList.end(); ++it) {
            delete *it;
    }

    mUndoList.clear();

    //clear redo list
    for (TpsCommandListIt it = mRedoList.begin();
        it != mRedoList.end(); ++it) {
            delete *it;
    }

    mRedoList.clear();

    if (mBackupCommand) {
        delete mBackupCommand;
        mBackupCommand = nullptr;
    }

    OnCommandListChanged();
    return true;
}

void TpsCommandManager::RegisterBackupCommand(TpsRedoUndoCommand* command)
{
    if (mBackupCommand) delete mBackupCommand;
    mBackupCommand = command;
}

TpsRedoUndoCommand* TpsCommandManager::GetBackupCommand()
{
    return mBackupCommand;
}

void TpsCommandManager::ActivateBackup()
{

    if (mBackupCommand->Ready()){
        this->RegisterCommand(mBackupCommand);
        mBackupCommand = nullptr;
    }
}

void TpsCommandManager::Lock()
{
    mMutex.lock();
}

void TpsCommandManager::Unlock()
{
    mMutex.unlock();
}

void TpsCommandManager::SetActiveResponser(const RedoUndoResponserPtr& responser)
{
    mResponser = responser;
}

void TpsCommandManager::OnCommandListChanged(){
    if (mResponser) {
        mResponser->Response(mUndoList.size(), mRedoList.size());
    }
}
TPS_END_NAMESPACE
