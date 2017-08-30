//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  jiandong.zhou jiandong.zhou@united-imaging.com
///
///  \file    tps_command_controller.h
///  \brief   undo/redo controller
///
///  \version 1.0
///  \date    Oct. 9, 2013
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_COMMAND_MANAGER_H
#define TPS_COMMAND_MANAGER_H

#include "tps_fw_defs.h"
#include "boost/thread/mutex.hpp"
#include "tps_singleton.h"
#include <list>

TPS_BEGIN_NAMESPACE

#define UNDO_LIST_MAX_NUM    100

class TpsRedoUndoCommand;
class RedoUndoActiveResponser;
typedef std::shared_ptr<RedoUndoActiveResponser> RedoUndoResponserPtr;

class TPS_FW_DECLSPEC TpsCommandManager{
public:
    TpsCommandManager();
    ~TpsCommandManager();
    void RegisterCommand(TpsRedoUndoCommand* command);

    //When Switch App clear old Undo/Redo List
    bool Reset();

    bool Undo();
    bool Redo();

    void RegisterBackupCommand(TpsRedoUndoCommand* command);
    TpsRedoUndoCommand* GetBackupCommand();
    void ActivateBackup();

    void Lock();
    void Unlock();

    template<class T>
    void RemoveCommands(T selector){
        size_t undoSize = mUndoList.size();
        size_t redoSize = mRedoList.size();

        mUndoList.remove_if(selector);
        mRedoList.remove_if(selector);

        if (undoSize != mUndoList.size() || redoSize != mRedoList.size()){
            OnCommandListChanged();
        }
    }

    void SetActiveResponser(const RedoUndoResponserPtr& responser);
    void OnCommandListChanged();
private:
    TpsRedoUndoCommand* GetUndoCommand_i();
    TpsRedoUndoCommand* GetRedoCommand_i();

    typedef std::list<TpsRedoUndoCommand*> TpsCommandList;
    typedef std::list<TpsRedoUndoCommand*>::iterator TpsCommandListIt;

    TpsCommandList  mUndoList;
    TpsCommandList  mRedoList;
    TpsRedoUndoCommand* mBackupCommand;
    RedoUndoResponserPtr mResponser;

    boost::mutex    mMutex;
};

TPS_END_NAMESPACE

#endif //TPS_COMMAND_MANAGER_H
