//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  LI Song song.li@united-imaging.com
///
///  \file    tps_rl_dose_update_mesh_operation.h
///  \brief   DoseUpdateMeshOperation declaration
///
///  \version 1.0
///  \date    Jun. 10, 2015
///  
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DOSE_UPDATE_MESH_OPERATION_H_
#define TPS_RL_DOSE_UPDATE_MESH_OPERATION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_operationbase.h"


TPS_BEGIN_NAMESPACE  // begin namespace tps

class DoseMeshGraphicObject;
class DoseMeshGraphicObjectCollection;

typedef std::shared_ptr<DoseMeshGraphicObject> TpsDoseMeshGraphicObjectPtr;

typedef std::shared_ptr<DoseMeshGraphicObjectCollection> TpsDoseMeshGraphicObjectCollectionPtr;

class TPS_RL_DECLSPEC DoseUpdateMeshOperation : public OperationBase{

public:
    //the constructor and destructor
    DoseUpdateMeshOperation(const std::string& strPlanUid, const bool& doseGridChanged = false);
    virtual ~DoseUpdateMeshOperation();

    virtual bool ModifyGraphicObject() const;

private:
    TpsDoseMeshGraphicObjectPtr GetDoseMeshGoByRatio(const float& ratio, TpsDoseMeshGraphicObjectCollectionPtr doseMeshCollectionPtr) const;

private:
    std::string     m_strPlanUid;
    bool            m_doseGridChanged;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(DoseUpdateMeshOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_RL_DOSE_UPDATE_MESH_OPERATION_H_
