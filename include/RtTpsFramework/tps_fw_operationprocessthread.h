//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_fw_operationprocessthread.h
///
/// \brief   OperationProcessThread
/// 
/// \version 1.0
/// 
/// \date    2013/12/11
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_FW_OPERATIONPROCESSTHREAD_H_
#define TPS_FW_OPERATIONPROCESSTHREAD_H_

#include "RtTpsFramework/tps_fw_defs.h"


#include "McsfMedViewer3DCommon/mcsf_active_object_t.h"
#include "McsfMedViewer3DCommon/mcsf_synchronous_command_base.h"

namespace Viewer3D = Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

/// \class  EmptySyncCommand
///
/// \brief  Empty sync command.
class EmptySyncCommand : public Viewer3D::SyncCommandBase {
public:
    /// \brief      Constructor.
    EmptySyncCommand() {}
    /// \brief      Destructor.
    ~EmptySyncCommand() {}
    /// \brief  Command execute.
    ///
    /// \return     true : success | false : fail
    bool Execute_i() const { return false; }

private:
    /// \brief      the default copy and assign method are not allowed.
    MCSF_MEDVIEWER_3D_DISALLOW_COPY_AND_ASSIGN(EmptySyncCommand);
};

/// \class  OperationProcessThread
///
/// \brief  Operation process thread.
class OperationProcessThread : public Viewer3D::ActiveObjectFIFO {
public:
    /// \brief      Constructor.
    OperationProcessThread();
    /// \brief      Destructor.
    virtual ~OperationProcessThread();
    /// \brief      Wait for Thread Idle.
    virtual void Synchronize();

private:
    /// \brief      the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(OperationProcessThread);
};

TPS_END_NAMESPACE

#endif  //TPS_RL_OPERATIONPROCESSTHREAD_H_

