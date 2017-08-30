
////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_statecontrolforbevgraphicobejct.cpp
/// 
///  \brief class StateControlGraphicObject  
/// 
///  \version 1.0
/// 
///  \date    2014/07/07
////////////////////////////////////////////////////////////////


#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_statecontrol_graphicobject.h"


TPS_BEGIN_NAMESPACE

StateControlGraphicObject::StateControlGraphicObject() : mType(DRR),
mMlcTransparency(1.0),mDrrVisibility(true), mDownSampleRate(1.0f)
{
}

StateControlGraphicObject::~StateControlGraphicObject() 
{
}

TPS_END_NAMESPACE



