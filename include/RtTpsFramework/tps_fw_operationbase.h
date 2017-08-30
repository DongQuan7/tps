//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_fw_operationbase.h
///
/// \brief   OperationBase
/// 
/// \version 1.0
/// 
/// \date    2013/12/11
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_FW_OPERATIONBASE_H_
#define TPS_FW_OPERATIONBASE_H_

#include "RtTpsFramework/tps_fw_defs.h"
#include "McsfMedViewer3DCommon/mcsf_command_base.h"
#include <map>
#include <vector>

namespace Mcsf {
    namespace MedViewer3D {
        class IGraphicObject;
    }
}
namespace Viewer3D = Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE
class ModelWarehouse;
class ITpsDataRepository;
class GOConverterFactoryBase;

/// \class  OperationBase
///
/// \brief  Base class for operation.
class TPS_FW_DECLSPEC OperationBase : public Viewer3D::CommandBase {
public:
    /// \brief  Set dependence with ClsModel.
    ///
    /// \param[in]  ClsModel pointer
    void SetModelWarehouse(ModelWarehouse* model);

    /// \brief  set data repository
    ///
    /// \param[in]  ITpsDataRepository pointer
    void SetDataRepository(ITpsDataRepository* dataRepository);

    /// \brief  set converter factory
    ///
    /// \param[in]  GOConverterFactoryBase pointer
    void SetConverterFactory(GOConverterFactoryBase* converterFactory);

    virtual void Initialize(ModelWarehouse* model, 
        ITpsDataRepository* dataRepository,
        GOConverterFactoryBase* converterFactory);

    /// \brief  get flag if operation is ready to execute
    bool IsReady() const;

    /// \brief  Modify GO. It must be overridden.
    ///
    /// \return     true : success | false : fail
    virtual bool ModifyGraphicObject() const = 0;

    /// \brief  Command execute.
    ///
    /// \return     true : success | false : fail
    virtual bool Execute() const;

protected:
    /// \brief      Constructor.
    OperationBase();
    /// \brief      Destructor.
    virtual ~OperationBase();

private:

protected:
    mutable ModelWarehouse*         mModelWarehouse;
    mutable ITpsDataRepository*     mDataRepository;
    mutable GOConverterFactoryBase* mConverterFactory;

private:
    std::map<int, std::shared_ptr<Viewer3D::IGraphicObject>>  mGraphicObjectCollection;
    std::vector<std::shared_ptr<Viewer3D::IGraphicObject>>    mLinkedGraphicObjectCollection;
    bool mIsReady;
    /// \brief      the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(OperationBase);
};

TPS_END_NAMESPACE

#endif  //TPS_RL_OPERATIONBASE_H_

