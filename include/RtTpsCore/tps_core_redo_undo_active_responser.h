////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_core_redo_undo_active_responser.h
/// 
///  \brief class RedoUndoActiveResponserImpl declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/13
////////////////////////////////////////////////////////////////

#ifndef TPS_CORE_REDO_UNDO_ACTIVE_RESPONSER_H_
#define TPS_CORE_REDO_UNDO_ACTIVE_RESPONSER_H_

#include "RtTpsFramework/tps_fw_active_responser_interface.h"

namespace tps
{
    /// \class RedoUndoActiveResponserImpl
    /// 
    /// \brief *****
    class RedoUndoActiveResponserImpl : public RedoUndoActiveResponser
    {
    public:
        //the constructor and destructor
        RedoUndoActiveResponserImpl();
        virtual ~RedoUndoActiveResponserImpl();

        virtual void SetHandle(void* pHandle);
        virtual void Response(unsigned int undoSize, unsigned int redoSize);

    protected:
        virtual void FreeHandle_i();
    };
}

#endif