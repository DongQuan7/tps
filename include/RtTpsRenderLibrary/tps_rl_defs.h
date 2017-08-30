//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  jiandong.zhou jiandong.zhou@united-imaging.com
///
///  \file    tps_object.h
///  \brief   top base class for backend
///
///  \version 1.0
///  \date    Oct. 9, 2013
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DEFS_H
#define TPS_RL_DEFS_H

#include "RtTpsFramework/tps_fw_renderdefines.h"

TPS_BEGIN_NAMESPACE

#ifdef TPS_RL_EXPORT
#define TPS_RL_DECLSPEC    TPS_EXPORT_CLS
#else
#define TPS_RL_DECLSPEC    TPS_IMPORT_CLS
#endif

#define CHECK_GL_ERRORS  \
{ \
    GLenum err = glGetError(); \
    switch (err) \
{ \
    case GL_NO_ERROR: \
    break; \
    case GL_INVALID_ENUM: \
    printf("Invalid enum.\n"); \
    break; \
    case GL_INVALID_VALUE: \
    printf("Invalid value.\n"); \
    break; \
    case GL_INVALID_OPERATION: \
    printf("Invalid operation.\n"); \
    break; \
    case GL_INVALID_FRAMEBUFFER_OPERATION: \
    printf("Invalid frame operation.\n"); \
    break; \
    case GL_OUT_OF_MEMORY: \
    printf("Out of memory.\n"); \
    break; \
    case GL_STACK_UNDERFLOW: \
    printf("Stack underflow.\n"); \
    break; \
    case GL_STACK_OVERFLOW: \
    printf("Stack overflow.\n"); \
    break; \
    default: \
    printf("Unknown error.\n"); \
    break;\
}\
}

TPS_END_NAMESPACE

#endif //TPS_RL_DEFS_H
