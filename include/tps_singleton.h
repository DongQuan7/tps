//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  jiandong.zhou jiandong.zhou@united-imaging.com
///
///  \file    tps_singleton.h
///  \brief   singleton for tps
///
///  \version 1.0
///  \date    Oct. 9, 2013
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef  TPS_SINGLETON_H
#define  TPS_SINGLETON_H

#include <boost/thread/mutex.hpp>

TPS_BEGIN_NAMESPACE

//Classic Singleton Implementation
#undef  DECLARE_SINGLETON
#define DECLARE_SINGLETON(T) \
public: \
    static T* GetInstance(); \
private: \
    friend class std::auto_ptr<T>; \
    TPS_DISALLOW_COPY_AND_ASSIGN(T); \
    static boost::mutex     mSingletonGuard; \
    static std::auto_ptr<T> mInstance;

//Boost Mutex for multiple threads
#undef  IMPLEMENT_SINGLETON
#define IMPLEMENT_SINGLETON(T) \
std::auto_ptr<T> T::mInstance(NULL); \
boost::mutex     T::mSingletonGuard; \
T* T::GetInstance() { \
    if (!mInstance.get()) { \
        boost::lock_guard<boost::mutex> lock_obj(mSingletonGuard); \
        if (!mInstance.get()) { \
            mInstance.reset(new T()); \
        }\
    } \
    return mInstance.get(); \
}

TPS_END_NAMESPACE

#endif //TPS_SINGLETON_H

