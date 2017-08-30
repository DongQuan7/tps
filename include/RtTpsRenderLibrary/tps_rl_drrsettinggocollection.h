////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  Zuo Kai  kai.zuo@united-imaging.com
/// 
///  \file tps_rl_drrsettinggocollection.h
/// 
///  \brief class drrsetting go collection declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/03/08
////////////////////////////////////////////////////////////////
#ifndef TPS_RL_DRRSETTING_GRAPHIC_OBJECT_COLLECTION_H_
#define TPS_RL_DRRSETTING_GRAPHIC_OBJECT_COLLECTION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsRenderLibrary/tps_rl_drrsettinggo.h"
#include <vector>

TPS_BEGIN_NAMESPACE  // begin namespace tps

    /// \class BlockGraphicObject
typedef std::shared_ptr<DRRSettingGraphicObject> DRRSettingGraphicObjectPtr;;
/// 
/// \brief *****
class TPS_RL_DECLSPEC DRRSettingGraphicObjectCollection : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    DRRSettingGraphicObjectCollection();
    virtual ~DRRSettingGraphicObjectCollection();

    void AddDrrSettingGo(DRRSettingGraphicObjectPtr go);
    void RemoveDrrSettingGo(const std::string& normalGroupUid);
    DRRSettingGraphicObjectPtr GetDrrSettingGo(const std::string& normalGroupUid);
private:
    std::vector<DRRSettingGraphicObjectPtr>    mDRRSettingGoCollection;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(DRRSettingGraphicObjectCollection);
};

TPS_END_NAMESPACE  // end namespace tps

#endif