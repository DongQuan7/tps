////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_dummyrenderstrategy.cpp
/// 
///  \brief class DummyRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/07/25
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_dummyrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_dummygraphicobject.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

DummyRenderStrategy::DummyRenderStrategy() : mDummyGo(nullptr){

}

DummyRenderStrategy::~DummyRenderStrategy(){

}

void DummyRenderStrategy::Render_i(){
    return;
}

int DummyRenderStrategy::CacheChanges(){
    if (mDummyGo == nullptr){
        return -1;
    }

    if (mDummyGo->IsDirty()){
        mDirty = true;
    }
    return 0;
}

void DummyRenderStrategy::Initialize(){

}

void DummyRenderStrategy::Finalize(){

}

void DummyRenderStrategy::Resize(int /*iWidth*/, int /*iHeight*/){
    /*mWidth = iWidth;
    mHeight = iHeight;*/
}


TPS_END_NAMESPACE  // end namespace tps