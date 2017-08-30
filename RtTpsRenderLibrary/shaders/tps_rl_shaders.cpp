////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_shaders.cpp
/// 
///  \brief class Shaders implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/12/01
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <Windows.h>
#include "../tps_rl_shaders.h"
#include <vector>

namespace tps
{
    std::map<int, std::string> Shaders::mShaderDictionary = std::map<int, std::string>();

    const std::string& Shaders::Get(int id)
    {
        if (mShaderDictionary.find(id) != mShaderDictionary.end())
        {
            return mShaderDictionary[id];
        }

        HMODULE handle = ::GetModuleHandle(TEXT("RtTpsRenderLibrary.dll"));

        const char* data;
        int size;
        HRSRC rc = ::FindResource(handle, MAKEINTRESOURCE(id),
            TEXT("SHADER_FILE"));
        if (rc == nullptr)
        {
            throw std::exception("Failed to load shader resource file!");
        }

        HGLOBAL rcData = ::LoadResource(handle, rc);
        size = ::SizeofResource(handle, rc);
        data = static_cast<const char*>(::LockResource(rcData));
        mShaderDictionary[id] = std::string(data, size);
        return mShaderDictionary[id];
    }
}