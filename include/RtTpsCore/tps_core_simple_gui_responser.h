////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  LI Song   song.li@united-imaging.com
/// 
///  \file tps_core_simple_gui_responser.cpp
/// 
///  \brief class SimpleGuiResponser implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/11/18
////////////////////////////////////////////////////////////////

#ifndef TPS_CORE_SIMPLE_GUI_RESPONSER_H_
#define TPS_CORE_SIMPLE_GUI_RESPONSER_H_

#include "RtTpsFramework/tps_fw_active_responser_interface.h"

namespace tps
{
    class SimpleGuiResponser : public ISimpleGuiResponser
    {
    public:
        //the constructor and destructor
        SimpleGuiResponser();
        virtual ~SimpleGuiResponser();

        virtual void SetHandle(void* pHandle);
        virtual void Response(const char* pInfo, int size);

    protected:
        virtual void FreeHandle_i();
    };
}

#endif //TPS_CORE_SIMPLE_GUI_RESPONSER_H_