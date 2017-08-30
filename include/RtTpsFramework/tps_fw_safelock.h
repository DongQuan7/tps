////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_safelock.h
/// 
///  \brief class TpsSafeLock declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/11/30
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_SAFELOCK_H_
#define TPS_FW_SAFELOCK_H_

#include "RtTpsFramework/tps_fw_defs.h"


TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsSafeLock
/// 
/// \brief a template class to implement safe lock multi-thread objects
template <typename T> 
class TpsSafeLock
{
public:
    //the constructor and destructor
    TpsSafeLock(T* t);
    ~TpsSafeLock();

private:
    T* mT;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsSafeLock);
};

template <typename T>
TpsSafeLock<T>::TpsSafeLock(T* t) : mT(t)
{
    if(mT)
    {
        mT->Lock();
    }
}

template <typename T>
TpsSafeLock<T>::~TpsSafeLock()
{
    mT->Unlock();
}
TPS_END_NAMESPACE  // end namespace tps

#endif