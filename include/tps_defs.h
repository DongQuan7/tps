//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  guohui.wu     guohui.wu@united-imaging.com
///           jiandong.zhou jiandong.zhou@united-imaging.com
///
///  \file    tps_common.h
///  \brief   define the common macros
///
///  \version 1.0
///  \date    Aug. 6, 2013
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_DEFS_H_
#define TPS_DEFS_H_


#include <string>
#include "tps_error.h"


// #include <gl/glew.h>
// #include <gl/glut.h>

//#include <boost/scoped_array.hpp>

#if defined (_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251 4275 4244 4267 4512 4127 4018)
#endif 

#ifdef TPS_BEGIN_NAMESPACE
#undef TPS_BEGIN_NAMESPACE
#endif
#define TPS_BEGIN_NAMESPACE \
    namespace tps           {

#ifdef TPS_END_NAMESPACE
#undef TPS_END_NAMESPACE
#endif
#define TPS_END_NAMESPACE   \
                            }

#define PLUNC_NAMESPACE       plunc
#define PLUNC_BEGIN_NAMESPACE      \
    namespace PLUNC_NAMESPACE       {
#define PLUC_END_NAMESPACE         }


#define TPS_PLUNC_BEGIN_NAMESPACE   \
    TPS_BEGIN_NAMESPACE             \
    PLUNC_BEGIN_NAMESPACE

#define TPS_PLUNC_END_NAMESPACE    \
    TPS_END_NAMESPACE              \
    PLUC_END_NAMESPACE

#ifdef TPS_CHECK_NULL_PTR
#undef TPS_CHECK_NULL_PTR
#endif
#define TPS_CHECK_NULL_PTR(pointer)                    \
    if (nullptr == pointer) {                              \
        throw std::invalid_argument( "Pointer is null.");  \
    }

#ifdef TPS_CHECK_NULL_PTR_WITH_DESCRIPTION
#undef TPS_CHECK_NULL_PTR_WITH_DESCRIPTION
#endif
#define TPS_CHECK_NULL_PTR_WITH_DESCRIPTION(pointer, description) \
    if (nullptr == pointer) { \
        throw std::invalid_argument(#description); \
    }

#define TPS_DISALLOW_COPY(classname) \
    classname(const classname&)
#define TPS_DISALLOW_ASSIGN(classname) \
    void operator = (const classname &)
#define  TPS_DISALLOW_COPY_AND_ASSIGN(classname) \
    TPS_DISALLOW_COPY(classname); \
    TPS_DISALLOW_ASSIGN(classname)

#define TPS_EXPORT_API   extern "C" __declspec(dllexport)
#define TPS_IMPORT_API   extern "C" __declspec(dllimport)
#define TPS_EXPORT_CLS   __declspec(dllexport)
#define TPS_IMPORT_CLS   __declspec(dllimport)


//#define TPS_BACKEND_LOGGER_UID 0x00000000

//netbase call error define
enum TPS_HANDLE_ERROR_CODE
{
    TPS_HANDLE_OK     = 0,
    TPS_HANDLE_FAILED = -1,
};

#ifndef NULL
#define NULL    0
#endif //NULL


#define TPSPRINTF printf

#ifndef TPS_MAX
#define TPS_MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef TPS_MIN
#define TPS_MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef TPS_CLAMP
#define TPS_CLAMP(value, min, max) TPS_MIN(max, TPS_MAX(min, value))
#endif

#ifndef FLOAT_EPSILON
#define FLOAT_EPSILON (std::numeric_limits<float>::epsilon())
#endif

#ifndef DOUBLE_EPSILON
#define DOUBLE_EPSILON (std::numeric_limits<double>::epsilon())
#endif

#ifndef TPS_FLOAT_SIGN
#define TPS_FLOAT_SIGN(a) ((abs(a) <= FLOAT_EPSILON) ? 0 : ((a) <-FLOAT_EPSILON ? -1 : 1))
#endif

#ifndef TPS_FLOAT_COMPARE
#define TPS_FLOAT_COMPARE(a, b) (abs((a) - (b)) <= FLOAT_EPSILON)
#endif

#ifndef TPS_DOUBLE_COMPARE
#define TPS_DOUBLE_COMPARE(a, b) (abs((a) - (b)) <= DOUBLE_EPSILON)
#endif

#ifndef TPS_ROUND
#define TPS_ROUND(a) ((a) > 0.0 ? floor((a) + 0.5) : ceil((a) - 0.5))
#endif

#ifndef TPS_DEL_PTR
#define TPS_DEL_PTR(ptr) delete ptr; ptr = nullptr;
#endif

#ifndef TPS_DEL_ARRAY
#define TPS_DEL_ARRAY(ptr) delete[] ptr; ptr = nullptr;
#endif

#define INVALID_DOSE_VALUE -1.F
#define INVALID_SPECIFIED_DOSE_VALUE 0.F
#define TPS_NEGATIVE_INF -1e20
#define TPS_POSITIVE_INF 1e20

//to be deleted
struct LAYOUT_UNIT{
    int viewerControlID;
    int cellID;
    LAYOUT_UNIT(int id1 = -1, int id2 = -1) : viewerControlID(id1), cellID(id2){}
};
#endif //TPS_DEFS_H_
