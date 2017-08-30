//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  LI Song song.li@united-imaging.com
///
///  \file    tps_rl_volume4d_change_current_operation.h
///  \brief   Volume4d change current declaration
///
///  \version 1.0
///  \date    Jul. 20, 2015
///  
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_VOLUME4D_CHANGE_CURRENT_OPERATION_H_
#define TPS_RL_VOLUME4D_CHANGE_CURRENT_OPERATION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_operationbase.h"


TPS_BEGIN_NAMESPACE  // begin namespace tps

class TPS_RL_DECLSPEC Volume4dChangeCurrentOperation : public OperationBase{

public:
    //the constructor and destructor
    Volume4dChangeCurrentOperation(const std::string& strMainSeriesUid, const std::string& strCurrentSeriesUid);
    virtual ~Volume4dChangeCurrentOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string     m_strMainSeriesUid;
    std::string     m_strCurrentSeriesUid;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(Volume4dChangeCurrentOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_RL_VOLUME4D_CHANGE_CURRENT_OPERATION_H_
