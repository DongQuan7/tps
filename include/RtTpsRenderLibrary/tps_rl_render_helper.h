////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang mailto:qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_rl_render_helper.h
/// 
///  \brief common api for render
/// 
///  \version 1.0
/// 
///  \date    Apr 18, 2016
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_RENDER_HELPER_H_
#define TPS_RL_RENDER_HELPER_H_

#include "tps_rl_defs.h"


TPS_BEGIN_NAMESPACE  // begin namespace TPS

class TPS_RL_DECLSPEC TpsRenderHelper
{
public:
	// true:supported FBO false: unsupported FBO
    static bool CheckFBO(void);

};

TPS_END_NAMESPACE  // end namespace TPS

#endif  
