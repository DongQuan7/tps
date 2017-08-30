////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_da_states.h
/// 
///  \brief class TPS states implementation
/// 
///  \version 1.0
/// 
///  \date    Sep.2,2014
////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "RtTpsFramework/tps_fw_states.h"

#include "boost/thread/mutex.hpp"

//TPS
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

TpsStates::TpsStates(): mIsFluenceMapMode(false){
    mMutex = new boost::mutex(); 
}

TpsStates::~TpsStates(void){
    delete mMutex; mMutex = nullptr;
}

bool TpsStates::IsFluenceMapMode()const {
    return mIsFluenceMapMode;
}
void TpsStates::SetFluenceMapMode(bool flag) {
    mIsFluenceMapMode = flag;
}


TPS_END_NAMESPACE
