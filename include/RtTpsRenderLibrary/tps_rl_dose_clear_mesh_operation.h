//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  LI Song song.li@united-imaging.com
///
///  \file    tps_rl_dose_clear_mesh_operation.h
///  \brief   DoseClearMeshOperation declaration
///
///  \version 1.0
///  \date    Aug. 24, 2015
///  
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DOSE_CLEAR_MESH_OPERATION_H_
#define TPS_RL_DOSE_CLEAR_MESH_OPERATION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_operationbase.h"


TPS_BEGIN_NAMESPACE  // begin namespace tps

class TPS_RL_DECLSPEC DoseClearMeshOperation : public OperationBase{

public:
    //the constructor and destructor
    DoseClearMeshOperation(const std::string& strPlanUid);
    virtual ~DoseClearMeshOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string     m_strPlanUid;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(DoseClearMeshOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_RL_DOSE_CLEAR_MESH_OPERATION_H_
