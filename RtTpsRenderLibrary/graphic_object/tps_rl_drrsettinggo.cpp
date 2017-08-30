#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_drrsettinggo.h"

TPS_BEGIN_NAMESPACE

DRRSettingGraphicObject::DRRSettingGraphicObject()
{

}

DRRSettingGraphicObject::~DRRSettingGraphicObject()
{

}

void DRRSettingGraphicObject::SetCT2DensityTableUid(const std::string& uid)
{
    mCT2DensityTableUid = uid;
}
std::string DRRSettingGraphicObject::GetCT2DensityTableUid()
{
    return mCT2DensityTableUid;
}

void DRRSettingGraphicObject::SetNormalgroupUid(const std::string& uid)
{
    mNormalGroupUid = uid;
}
std::string DRRSettingGraphicObject::GetNormalgroupUid()
{
    return mNormalGroupUid;
}

void DRRSettingGraphicObject::SetElectronEnergy(float electronEnergy)
{
    mElectronEnergy = electronEnergy;
}
void DRRSettingGraphicObject::GetElectronEnergy(float& electronEnergy)
{
    electronEnergy = mElectronEnergy;
}

void DRRSettingGraphicObject::SetCTThresholdMin(float CTThresholdMin)
{
    mCTThresholdMin = CTThresholdMin;
}
void DRRSettingGraphicObject::GetCTThresholdMin(float& CTThresholdMin)
{
    CTThresholdMin = mCTThresholdMin;
}

void DRRSettingGraphicObject::SetCTThresholdMax(float CTThresholdMax)
{
    mCTThresholdMax = CTThresholdMax;
}
void DRRSettingGraphicObject::GetCTThresholdMax(float& CTThresholdMax)
{
    CTThresholdMax = mCTThresholdMax;
}

TPS_END_NAMESPACE