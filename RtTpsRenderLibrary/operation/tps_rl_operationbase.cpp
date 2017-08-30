//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_rl_operationbase.cpp
///
/// \brief   OperationBase
/// 
/// \version 1.0
/// 
/// \date    2013/12/11
/////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_operationbase.h"

#include "McsfMedViewer3DGraphicObject/graphic_object_interface.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"

#include "tps_logger.h"
TPS_BEGIN_NAMESPACE

OperationBase::OperationBase() : mModelReady(false) {
}

OperationBase::~OperationBase() {
}

void OperationBase::SetModel(std::shared_ptr<Viewer3D::ClsModel> model) {
    mModelEntity = model;
    if (!mModelReady) {
        mModelReady = true;
    }
}

bool OperationBase::IsModelReady() {
    return mModelReady;
}

bool OperationBase::Execute() const {
    if (!mModelReady){
        TPS_LOG_DEV_ERROR<<"[ERROR][OperationBase][Execute]: "
            <<"the model entity is not ready.";
        return false;
    }

    if (mModelEntity == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][OperationBase][Execute]: "
            <<"the model entity is empty.";
        return false;
    }

    bool result = false;

    mModelEntity->Lock();

    result = ModifyGraphicObject();

    mModelEntity->Unlock();

    return result;
}

TPS_END_NAMESPACE

