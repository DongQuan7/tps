////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang mailto:qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_rl_render_helper.cpp
/// 
///  \brief common api for render
/// 
///  \version 1.0
/// 
///  \date    Apr 18, 2016
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "gl/glut.h"
#include "RtTpsRenderLibrary/tps_rl_render_helper.h"

TPS_BEGIN_NAMESPACE;

// true:supported FBO false: unsupported FBO
bool TpsRenderHelper::CheckFBO(void)
{
    auto handle = glutCreateWindow("Frame buffer object");
    glewExperimental=TRUE;
    GLenum err=glewInit();

    bool res = true;
    if(GLEW_OK != err){
        //Problem: glewInit failed, something is seriously wrong.
        TPS_LOG_DEV_ERROR<<"glewInit failed, aborting.\n";
#ifdef _DEBUG
        printf("glewInit failed, aborting.\n");	
#endif
        res = false;
    }

    if (nullptr == glGenFramebuffersEXT){
        TPS_LOG_DEV_ERROR<<"glGenFramebuffersEXT is null.\n";
#ifdef _DEBUG
        printf("glGenFramebuffersEXT is null.\n");	
#endif
        res = false;
    }
    TPS_LOG_DEV_INFO<<"TpsRenderHelper::CheckFBO OK.\n";

    glutDestroyWindow(handle);
    return res;
}

TPS_END_NAMESPACE
