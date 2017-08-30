////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  Zuo Kai  kai.zuo@united-imaging.com
/// 
///  \file tps_rl_drrsetting.h
/// 
///  \brief class drrsetting go declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/03/08
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DRRSETTING_GRAPHIC_OBJECT_H_
#define TPS_RL_DRRSETTING_GRAPHIC_OBJECT_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

    /// \class BlockGraphicObject
    /// 
    /// \brief *****
class TPS_RL_DECLSPEC DRRSettingGraphicObject : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    DRRSettingGraphicObject();
    virtual ~DRRSettingGraphicObject();

    void SetCT2DensityTableUid(const std::string& uid);
    std::string GetCT2DensityTableUid();

    void SetNormalgroupUid(const std::string& uid);
    std::string GetNormalgroupUid();

    void SetElectronEnergy(float electronEnergy);
    void GetElectronEnergy(float& electronEnergy);

    void SetCTThresholdMin(float CTThresholdMin);
    void GetCTThresholdMin(float& CTThresholdMin);

    void SetCTThresholdMax(float CTThresholdMax);
    void GetCTThresholdMax(float& CTThresholdMax);

private:
    std::string                                                      mNormalGroupUid;
    std::string                                                      mCT2DensityTableUid;
    float                                                            mElectronEnergy; //MeV
    float                                                            mCTThresholdMin;
    float                                                            mCTThresholdMax;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(DRRSettingGraphicObject);
};

TPS_END_NAMESPACE  // end namespace tps

#endif