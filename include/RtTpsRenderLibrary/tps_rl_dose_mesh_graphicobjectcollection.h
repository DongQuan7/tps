////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  Zuo Kai  kai.zuo@united-imaging.com
/// 
///  \file tps_rl_dose_mesh_graphicobjectcollection.h
/// 
///  \brief class dose mesh go collection declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/01/26
////////////////////////////////////////////////////////////////
#ifndef TPS_RL_DOSE_MESH_GRAPHIC_OBJECT_COLLECTION_H_
#define TPS_RL_DOSE_MESH_GRAPHIC_OBJECT_COLLECTION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobject.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

typedef std::shared_ptr<DoseMeshGraphicObject> TpsDoseMeshGraphicObjectPtr;
/// 
/// \brief *****
class TPS_RL_DECLSPEC DoseMeshGraphicObjectCollection : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    DoseMeshGraphicObjectCollection();
    virtual ~DoseMeshGraphicObjectCollection();

    void AddDoseMeshGo(TpsDoseMeshGraphicObjectPtr go);
    void RemoveDoseMeshGo(const float& doseRatio);
    TpsDoseMeshGraphicObjectPtr GetDoseMeshGo(const float& doseRatio);
    std::vector<TpsDoseMeshGraphicObjectPtr>& GetAllDoseMeshGo();
    void SetAllDoseMeshGo(std::vector<TpsDoseMeshGraphicObjectPtr> doseMeshGoCollection);
    void SetDoseMeshGoVisible(const float& doseRatio, const bool& visible);
    void ClearAllDoseMeshGo();
private:
    std::vector<TpsDoseMeshGraphicObjectPtr>    mDoseMeshGoCollection;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(DoseMeshGraphicObjectCollection);
};

TPS_END_NAMESPACE  // end namespace tps

#endif