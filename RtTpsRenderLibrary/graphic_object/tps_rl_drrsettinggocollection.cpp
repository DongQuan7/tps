#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_drrsettinggocollection.h"

TPS_BEGIN_NAMESPACE

DRRSettingGraphicObjectCollection::DRRSettingGraphicObjectCollection()
{

}
DRRSettingGraphicObjectCollection::~DRRSettingGraphicObjectCollection()
{

}

void DRRSettingGraphicObjectCollection::AddDrrSettingGo(DRRSettingGraphicObjectPtr go)
{
    mDRRSettingGoCollection.push_back(go);
}
void DRRSettingGraphicObjectCollection::RemoveDrrSettingGo(const std::string& normalGroupUid)
{
    for (auto iter = mDRRSettingGoCollection.begin(); iter != mDRRSettingGoCollection.end(); ++iter)
    {
        if((*iter)->GetNormalgroupUid() == normalGroupUid)
        {
            mDRRSettingGoCollection.erase(iter);
            break;
        }
    }
}
DRRSettingGraphicObjectPtr DRRSettingGraphicObjectCollection::GetDrrSettingGo(const std::string& normalGroupUid)
{
    for (auto iter = mDRRSettingGoCollection.begin(); iter != mDRRSettingGoCollection.end(); ++iter)
    {
        if((*iter)->GetNormalgroupUid() == normalGroupUid)
        {
            return (*iter);
        }
    }
    return nullptr;
}

TPS_END_NAMESPACE