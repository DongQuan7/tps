////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_shaders.h
/// 
///  \brief class Shaders declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/12/01
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SHADERS_H_
#define TPS_RL_SHADERS_H_

#include "resource.h"
#include <string>
#include <map>

namespace tps
{
    class Shaders 
    {
        public:
            static const std::string& Get(int id);

        private:
            static std::map<int, std::string> mShaderDictionary;
    };
}

#endif