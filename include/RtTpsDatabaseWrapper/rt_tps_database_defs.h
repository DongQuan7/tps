//////////////////////////////////////////////////////////////////////////
/// \defgroup McsfDatabase of Common Software Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2011
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  qiangqiang.zhou@united-imaging.com
///
///  \file        mcsf_database_common.h
///  \brief   common definitions for Database.
///
///  \version 1.0
///  \date    10/10/2011
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef RT_TPS_DATABASE_DEFS_H_
#define RT_TPS_DATABASE_DEFS_H_

/// \def define NULL
#ifndef NULL
    #ifdef __cplusplus
        #define NULL    0
    #else
        #define NULL    ((void *)0)
    #endif
#endif

// A macro to delete pointer and assign it to NULL
#ifndef DEL_PTR
#define DEL_PTR(ptr) \
    do \
    { \
    if (nullptr != ptr) \
        { \
        delete ptr; \
        ptr = nullptr; \
        } \
        } while (false)
#endif

// A macro to delete vector item and assign it to NULL
#ifndef DEL_PTRVECTOR
#define DEL_PTRVECTOR(ptr) \
    for (auto v = ptr.begin(); v != ptr.end(); v++) \
    { \
    DEL_PTR(*v); \
    } \
    ptr.clear();
#endif

// A macro to delete Map item and assign it to NULL
#ifndef DEL_PTRMAP
#define DEL_PTRMAP(ptr) \
    for (auto v = ptr.begin(); v != ptr.end(); v++) \
    { \
    DEL_PTR(v->second); \
    } \
    ptr.clear();
#endif


// A macro to delete pointer array and assign it to NULL
#ifndef DEL_ARRAY
#define DEL_ARRAY(ptr) \
    do \
    { \
    if (nullptr != ptr) \
        { \
        delete[] ptr; \
        ptr = nullptr; \
        } \
    } while (false)
#endif

/// \def RT_TPS_DATABASE_MAJOR_VERSION
#define RT_TPS_DATABASE_MAJOR_VERSION 1
/// \def RT_TPS_DATABASE_MINOR_VERSION
#define RT_TPS_DATABASE_MINOR_VERSION 1

#if !defined(ULIB)
#define ULIB 
#endif

// \def export
#if defined(ULIB)
//#if ULIB==RtTpsDatabaseWrapper
#   if defined(__lint)
#       define RT_DB_EXPORT      __export(RtTpsDatabaseWrapper)
#   elif defined(_WIN32)
#       define RT_DB_EXPORT      __declspec(dllexport)
#   elif __GNUC__ >= 4
#       define RT_DB_EXPORT      __attribute__ ((visibility("default")))
#   else
#       define RT_DB_EXPORT
#   endif
#else
// \def import
#   if defined(__lint)
#       define RT_DB_EXPORT       __export(RtTpsDatabaseWrapper)
#   elif defined(_WIN32) && !defined(WNT_STATIC_LINK)
#       define RT_DB_EXPORT       __declspec(dllimport)
#   else
#       define RT_DB_EXPORT
#   endif
#endif

/// \def RT_TPS_DATABASE_NAMESPACE
#ifndef RT_TPS_DATABASE_VERSIONED_NAMESPACE
    #define RT_TPS_DATABASE_NAMESPACE       tps
    #define RT_TPS_DATABASE_BEGIN_NAMESPACE \
        namespace RT_TPS_DATABASE_NAMESPACE {    /* begin namespace tps */
    #define RT_TPS_DATABASE_END_NAMESPACE   }    /* end namespace tps   */
#else
    #define RT_TPS_DATABASE_MAKE_VERSIONED_NAMESPACE_IMPL(Major, Minor) \
        tps_##Major##_##Minor
    #define RT_TPS_DATABASE_MAKE_VERSIONED_NAMESPACE(Major, Minor) \
        RT_TPS_DATABASE_MAKE_VERSIONED_NAMESPACE_IMPL(Major, Minor)
    #define RT_TPS_DATABASE_VERSIONED_NAMESPACE \
        RT_TPS_DATABASE_MAKE_VERSIONED_NAMESPACE(RT_TPS_DATABASE_MAJOR_VERSION, RT_TPS_DATABASE_MINOR_VERSION)
    #define RT_TPS_DATABASE_BEGIN_NAMESPACE \
        namespace RT_TPS_DATABASE_VERSIONED_NAMESPACE {    /* begin namespace tps */
    #define RT_TPS_DATABASE_END_NAMESPACE   }              /* end namespace tps   */
#endif

/// \def define disallowing copy and assign operation
#ifndef RT_TPS_DATABASE_DISALLOW_COPY_AND_ASSIGN_DEFINED
    #define RT_TPS_DATABASE_DISALLOW_COPY_AND_ASSIGN_DEFINED
    #define RT_TPS_DATABASE_DISALLOW_COPY(classname) \
        classname(const classname & );
    #define RT_TPS_DATABASE_DISALLOW_ASSIGN(classname) \
        void operator = (const classname & );
    #define RT_TPS_DATABASE_DISALLOW_COPY_AND_ASSIGN(classname) \
        RT_TPS_DATABASE_DISALLOW_COPY  (classname)\
        RT_TPS_DATABASE_DISALLOW_ASSIGN(classname)
#endif

#endif
