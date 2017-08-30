////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_voieditingredoundooperation.h
/// 
///  \brief class TpsVOIEditingRedoUndoOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/04/22
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_VOIEDITINGREDOUNDOOPERATION_H_
#define TPS_RL_VOIEDITINGREDOUNDOOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

namespace Mcsf{
    namespace MedViewer3D{
        class Matrix4x4;
    }
}

namespace McsfGeometry{
    class SubVolume;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsVOIEditingCache;

/// \class TpsVOIEditingRedoUndoOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC TpsVOIEditingRedoUndoOperation
    : public OperationBase{
public:
    //the constructor and destructor
    TpsVOIEditingRedoUndoOperation(std::shared_ptr<TpsVOIEditingCache> edtingCache);
    virtual ~TpsVOIEditingRedoUndoOperation();

    virtual bool ModifyGraphicObject() const;

private:
    void UpdateVoiMaskCaches_i(
        const std::string& imageUID, 
        const std::string& voiUID, 
        McsfGeometry::SubVolume* subVolume, 
        const Mcsf::MedViewer3D::Matrix4x4& matWorld2Volume) const;

private:
    std::shared_ptr<TpsVOIEditingCache>     mCache;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsVOIEditingRedoUndoOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif