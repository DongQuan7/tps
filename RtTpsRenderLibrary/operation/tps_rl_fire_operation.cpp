////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_fire_operation.cpp
/// 
///  \brief class TpsFireOperation implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/20
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_fire_operation.h"

#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsFireOperation::TpsFireOperation(){

}

TpsFireOperation::~TpsFireOperation(){

}

bool TpsFireOperation::ModifyGraphicObject() const{
    mModelEntity->Notify();
    mModelEntity->ClearDirtyFlag();
    return true;
}
TPS_END_NAMESPACE  // end namespace tps