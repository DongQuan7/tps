//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_fw_modelwarehouse.cpp
///
/// \brief   ModelWarehouse
/// 
/// \version 1.0
/// 
/// \date    2013/12/11
/////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"

#include "RtTpsFramework/tps_fw_operationprocessthread.h"
#include "RtTpsFramework/tps_fw_operationbase.h"
#include "RtTpsFramework/tps_fw_composable_graphic_object.h"

#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "McsfMedViewer3DGraphicObject/graphic_object_interface.h"
#include "McsfMedViewer3DCommon/mcsf_command_base.h"
#include "McsfMedViewer3DCommon/mcsf_viewer3d_observer_interface.h"

#include <memory>
TPS_BEGIN_NAMESPACE

//IMPLEMENT_SINGLETON(ModelWarehouse)

ModelWarehouse::ModelWarehouse() : mIsStarted(false) {
    mModelEntity.reset(new Viewer3D::ClsModel());
}

ModelWarehouse::~ModelWarehouse() {

}

void ModelWarehouse::AddModelObject(const std::string& modelName, std::shared_ptr<Viewer3D::IGraphicObject> modelObject) {
    if (NULL != mModelEntity) {
        mModelEntity->AddGraphicObject(modelName, modelObject);

        auto it = mGraphicObjectNameCollection.find(modelName);
        if (it == mGraphicObjectNameCollection.end()) {
            mGraphicObjectNameCollection[modelName] = modelName;
            //printf("\nGO Created: %s", modelName.c_str());
        }
    }
}

void ModelWarehouse::RemoveModelObject(const std::string& modelName, int componentsID/* = -1*/) {
    if (NULL == mModelEntity) return;

    auto go = mModelEntity->GetGraphicObject(modelName);
    auto composable_go = std::dynamic_pointer_cast<ComposableGraphicObject>(go);
    if (composable_go != nullptr)
    {
        if (componentsID != -1)
        {
            composable_go->DecomposeOne(componentsID);
            if (composable_go->ComponentsCount() != 0)
            {
                return;
            }
        }
    }

    mModelEntity->RemoveGraphicObject(modelName);

    auto it = mGraphicObjectNameCollection.find(modelName);
    if (it != mGraphicObjectNameCollection.end()) {
        mGraphicObjectNameCollection.erase(it);
        //printf("\nGO Erased: %s", modelName.c_str());
    }
}

std::shared_ptr<Viewer3D::IGraphicObject> 
ModelWarehouse::GetGraphicObject(const std::string& strGraphicObjName) const
{
    if (NULL != mModelEntity) {
        return mModelEntity->GetGraphicObject(strGraphicObjName);
    }
    return nullptr;
}

void ModelWarehouse::SetAllGODirty()
{
    if (NULL != mModelEntity) {
        auto itGOName = mGraphicObjectNameCollection.begin();
        for (; itGOName != mGraphicObjectNameCollection.end(); ++itGOName) {
            std::string strGOName = itGOName->second;
            auto ptrGO = mModelEntity->GetGraphicObject(strGOName);
            if (ptrGO != nullptr) {
                ptrGO->SetDirty(true);
            }
        }
    }
}

void ModelWarehouse::ClearDirtyFlag() {
    if (NULL != mModelEntity) {
        mModelEntity->ClearDirtyFlag();
    }
}

std::shared_ptr<Viewer3D::IGraphicObject> ModelWarehouse::GetModelObject(const std::string& modelName) {
    if (NULL != mModelEntity) {
        return mModelEntity->GetGraphicObject(modelName);
    }

    return NULL;
}

void ModelWarehouse::AttachObserver(IObserverPtr observer){
    mModelEntity->AttachObserver(observer);
}

void ModelWarehouse::Lock() {
    if (NULL != mModelEntity) {
        mModelEntity->Lock();
    }
}

void ModelWarehouse::Unlock() {
    if (NULL != mModelEntity) {
        mModelEntity->Unlock();
    }
}

void ModelWarehouse::ClearAll() {

    if (NULL == mModelEntity) {
        return;
    }

    for (auto it = mGraphicObjectNameCollection.begin();
        it != mGraphicObjectNameCollection.end(); ++it) {
            mModelEntity->RemoveGraphicObject(it->second);
    }

    mGraphicObjectNameCollection.clear();

}

void ModelWarehouse::Notify(int iId)
{
    mModelEntity->Notify(iId);
}
TPS_END_NAMESPACE
