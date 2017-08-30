////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_core_delegate_translator.h
/// 
///  \brief class DelegateTranslator declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/12
////////////////////////////////////////////////////////////////

#ifndef TPS_CORE_DELEGATE_TRANSLATOR_H_
#define TPS_CORE_DELEGATE_TRANSLATOR_H_

using namespace System;
using namespace System::Runtime::InteropServices;

namespace tps  // begin namespace tps
{
    public delegate void CommonDelegate(array<System::Object^>^ args);
    public delegate void SingleByteArrayDelegate(array<System::Byte>^ byteArray);
    public delegate void WindowUpdatedDelegate(Int32 width, Int32 height, array<System::Byte>^ pixelData);
    /// \class DelegateCarrier
    /// 
    /// \brief use carrier instead of Delegate directly, so we can put several 
    ///  different delegates into one GCHandle
    generic<typename T>
    public ref class DelegateCarrier
    {
    public:
        property T Delegate 
        {
            T get() { return mDelegator; }
            void set(T value) { mDelegator = value; }
        }

        property Int32 WindowType {
            Int32 get() { return mWindowType; }
            void set(Int32 value) { mWindowType = value; }
        }

        property String^ ConnectionID {
            String^ get() { return mConnectionID; }
            void set(String^ value) { mConnectionID = value; }
        }

        void* ToGCHandlePtr()
        {
            GCHandle gch = GCHandle::Alloc(this);
            return GCHandle::ToIntPtr(gch).ToPointer();
        }
    private:
        T       mDelegator;
        Int32   mWindowType;
        String^ mConnectionID;
    };
}  // end namespace tps

#endif