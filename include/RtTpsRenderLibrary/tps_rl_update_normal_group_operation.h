////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_update_normal_group_operation.h
/// 
///  \brief class UpdateNormalGroup declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/08/28
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATE_NORMAL_GROUP_OPERATION_H_
#define TPS_RL_UPDATE_NORMAL_GROUP_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

namespace RTFWK{
    class RTFWKDLDONormgroup;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

    /// \class UpdateNormalGroupOperation
    /// 
    /// \brief update normal group graphic object, note that this operation 
    ///        refresh normal group properties only, not include beams.
class TPS_RL_DECLSPEC UpdateNormalGroupOperation
    : public OperationBase{
public:
    //the constructor and destructor
    UpdateNormalGroupOperation(const std::string& planUID, const std::string& ngUID);
    virtual ~UpdateNormalGroupOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string mNormGroupUID;
    std::string mPlanUID;

    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(UpdateNormalGroupOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
