////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_logger.h
/// 
///  \brief tps logger define
/// 
///  \version 1.0
/// 
///  \date    2013/11/19
////////////////////////////////////////////////////////////////

#ifndef TPS_LOGGER_H_
#define TPS_LOGGER_H_

#include "McsfLogger/mcsf_logger.h"

//#ifdef LOGGER_SOURCE_NAME
//#undef LOGGER_SOURCE_NAME
//#define LOGGER_SOURCE_NAME "RT_TPS_LOG"
//#endif

#ifndef TPS_BE_LOG_UID
#define TPS_BE_LOG_UID 0x00000000
#endif

// \brief: TPS LOG DEV INFO
#ifndef TPS_LOG_DEV_INFO 
#define TPS_LOG_DEV_INFO \
    LOG_DEV_INFO(TPS_BE_LOG_UID)
#endif

// \brief: TPS LOG DEV WARNING
#ifndef TPS_LOG_DEV_WARNING 
#define TPS_LOG_DEV_WARNING \
    LOG_DEV_WARNING(TPS_BE_LOG_UID)
#endif

// \brief: TPS LOG DEV ERROR
#ifndef TPS_LOG_DEV_ERROR 
#define TPS_LOG_DEV_ERROR \
    LOG_DEV_ERROR(TPS_BE_LOG_UID)
#endif

// \brief: TPS LOG TRACE INFO
#ifndef TPS_LOG_TRACE_INFO 
#define TPS_LOG_TRACE_INFO \
    LOG_TRACE_INFO()
#endif

// \brief: TPS LOG TRACE WARNING
#ifndef TPS_LOG_TRACE_WARNING 
#define TPS_LOG_TRACE_WARNING \
    LOG_TRACE_WARN()
#endif

// \brief: TPS LOG TRACE ERROR
#ifndef TPS_LOG_TRACE_ERROR 
#define TPS_LOG_TRACE_ERROR \
    LOG_TRACE_ERROR()
#endif

#ifndef TPS_PRINTF_DEBUG
#ifdef _DEBUG
#define TPS_PRINTF_DEBUG(format, ...) do {                                                  \
    printf("\nFile: %s, \nLine: %d, \tFunction:%s\n", __FILE__, __LINE__, __FUNCTION__);    \
    printf(format, ##__VA_ARGS__);                                                          \
} while (0)
#else  
#define TPS_PRINTF_DEBUG(format, ...);//do nothing for release version.
#endif
#endif

#ifndef TPS_PRINTF

#define TPS_PRINTF(format, ...) do {                                                  \
    printf("\nFile: %s, \nLine: %d, \tFunction:%s\n", __FILE__, __LINE__, __FUNCTION__);    \
    printf(format, ##__VA_ARGS__);                                                          \
} while (0)
#endif

#ifndef TPS_WORKFLOW_KEYPOINT_PREFIX
#define TPS_WORKFLOW_KEYPOINT_PREFIX "[KeyPoint]:"
#endif

#ifndef ADD_WORKFLOW_KEYPOINT
#define ADD_WORKFLOW_KEYPOINT TPS_LOG_DEV_INFO<<"[KeyPoint]:"
#endif


#endif//TPS_LOGGER_H_