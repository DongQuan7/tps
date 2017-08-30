////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_fw_common_enums.cpp
/// 
///  \brief enum 
/// 
///  \version 1.0
/// 
///  \date    Jan.17,2014
////////////////////////////////////////////////////////////////

#include"StdAfx.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

TPS_BEGIN_NAMESPACE

void GetClorFromEnum(TPS_COLOR colorType, float fColor[4]){
    fColor[3] = 1.f;
    switch (colorType){
    case RED:
        fColor[0] = 1.f;
        fColor[1] = 0.f;
        fColor[2] = 0.f;
        break;
    case GREEN:
        fColor[0] = 0.f;
        fColor[1] = 1.f;
        fColor[2] = 0.f;
        break;
    case BLUE:
        fColor[0] = 0.f;
        fColor[1] = 0.f;
        fColor[2] = 1.f;
        break;
    case YELLOW:
        fColor[0] = 1.f;
        fColor[1] = 1.f;
        fColor[2] = 0.f;
        break;
    case WHITE:
        fColor[0] = 1.f;
        fColor[1] = 1.f;
        fColor[2] = 1.f;
        break;
    case BLACK:
        fColor[0] = 0.f;
        fColor[1] = 0.f;
        fColor[2] = 0.f;
        break;
    default:
        fColor[0] = 1.f;
        fColor[1] = 1.f;
        fColor[2] = 1.f;
        break;
    }
};

TPS_END_NAMESPACE
