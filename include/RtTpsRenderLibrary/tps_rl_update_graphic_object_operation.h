////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_update_graphic_object_operation.h
/// 
///  \brief class UpdateGraphicObjectOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/07/18
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATE_GRAPHIC_OBJECT_OPERATION_H_
#define TPS_RL_UPDATE_GRAPHIC_OBJECT_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class UpdateGraphicObjectOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC UpdateGraphicObjectOperation : public OperationBase
{
public:
    //the constructor and destructor
    //data source type, if -1, means not data source id(IMAGE, PLAN, FLOAT_IMAGE)
    UpdateGraphicObjectOperation(const std::string& mainUID, GO_TYPE goType, int dataSourceType = -1);
    virtual ~UpdateGraphicObjectOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string mMainUID;
    GO_TYPE     mGOType;
    int         mDataSrcType;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(UpdateGraphicObjectOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif