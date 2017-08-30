//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_fw_modelwarehouse.h
///
/// \brief   ModelWarehouse
/// 
/// \version 1.0
/// 
/// \date    2013/12/11
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_FW_MODELWAREHOUSE_H_
#define TPS_FW_MODELWAREHOUSE_H_

#include "RtTpsFramework/tps_fw_defs.h"
#include <memory>
#include <map>

namespace Mcsf {
    namespace MedViewer3D {
        class ClsModel;
        class IGraphicObject;
        class CommandBase;
        class IObserver;
    }
}
namespace Viewer3D = Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

class OperationProcessThread;
class OperationBase;
typedef std::shared_ptr<Viewer3D::IObserver> IObserverPtr;
/// \class  ModelWarehouse
///
/// \brief  Model and thread manager.
class TPS_FW_DECLSPEC ModelWarehouse {
    //DECLARE_SINGLETON(ModelWarehouse)

public:
    /// \brief      Constructor.
    ModelWarehouse();

    /// \brief      Destructor.
    ~ModelWarehouse();

    /// \brief      Add model object into ClsModel.
    ///
    /// \param[in]  Model name
    /// \param[in]  Model data
    virtual void AddModelObject(const std::string&, std::shared_ptr<Viewer3D::IGraphicObject>);
    /// \brief      Remove model object from ClsModel.
    ///
    /// \param[in]  Model name
    virtual void RemoveModelObject(const std::string&, int componentsID = -1);
    /// \brief      Clear dirty flag in for ClsModel.
    virtual void ClearDirtyFlag();
    virtual void SetAllGODirty();
    /// \brief      Get model object from ClsModel.
    ///
    /// \param[in]  Model name
    ///
    /// \return     Model data
    virtual std::shared_ptr<Viewer3D::IGraphicObject> GetModelObject(const std::string&);
    /// \brief      attach observer to ClsModel
    ///
    /// \param[in]  IObserverPtr  the observer to be attached
    void AttachObserver(IObserverPtr);
    /// \brief      Lock ClsModel.
    virtual void Lock();
    /// \brief      Unlock ClsModel.
    virtual void Unlock();
    /// \brief      Get ClsModel
    virtual std::shared_ptr<Viewer3D::ClsModel> GetModelEntity() const { return mModelEntity; }

    virtual void ClearAll();

    virtual void Notify(int iId=-1);

    virtual std::shared_ptr<Viewer3D::IGraphicObject> 
        GetGraphicObject(const std::string& strGraphicObjName) const;

private:
    std::shared_ptr<Viewer3D::ClsModel> mModelEntity;
    std::shared_ptr<OperationProcessThread> mOperationProcessor;
    std::map<std::string, std::shared_ptr<OperationBase>> mOperationCollection;
    bool mIsStarted;
    std::map<std::string, std::string> mGraphicObjectNameCollection;

};

TPS_END_NAMESPACE

#endif  //TPS_RL_MODELWAREHOUSE_H_

