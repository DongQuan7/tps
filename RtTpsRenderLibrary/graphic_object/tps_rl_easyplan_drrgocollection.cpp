#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"

TPS_BEGIN_NAMESPACE
EasyPlanDrrGraphicObjectCollection::EasyPlanDrrGraphicObjectCollection(){
}
EasyPlanDrrGraphicObjectCollection::~EasyPlanDrrGraphicObjectCollection() {
}

bool EasyPlanDrrGraphicObjectCollection::Initialize() {
    mDrrGoMap.clear();
    return true;
}

bool EasyPlanDrrGraphicObjectCollection::Finalize() {
    return true;
}

bool EasyPlanDrrGraphicObjectCollection::DeleteDrr(const std::string &suid) {
    std::map<std::string,std::shared_ptr<EasyPlanDrrGraphicObejct>>::iterator itr = mDrrGoMap.find(suid);
    if(itr == mDrrGoMap.end()) {
        return false;
    }
    mDrrGoMap.erase(itr);
    return true;
}

void EasyPlanDrrGraphicObjectCollection::AddDrr(const std::string &str ,std::shared_ptr<EasyPlanDrrGraphicObejct> drrObj) {

    auto it = mDrrGoMap.find(str);
    if (it != mDrrGoMap.end()){
        TPS_LOG_DEV_WARNING<<"The drr already exist.";
    }

    mDrrGoMap[str] = drrObj;
}

void EasyPlanDrrGraphicObjectCollection::RemoveAll() {
    mDrrGoMap.clear();
}

std::shared_ptr<EasyPlanDrrGraphicObejct> EasyPlanDrrGraphicObjectCollection::GetDrr(const std::string &str) {
    std::map<std::string,std::shared_ptr<EasyPlanDrrGraphicObejct>>::iterator itr = mDrrGoMap.find(str);
    if(itr != mDrrGoMap.end()) {
        std::shared_ptr<EasyPlanDrrGraphicObejct> drrObj = itr->second;
        return drrObj;
    }
    return nullptr;
}

std::map<std::string,std::shared_ptr<EasyPlanDrrGraphicObejct>> EasyPlanDrrGraphicObjectCollection::GetDrrMap() const{
    return mDrrGoMap;
}

bool EasyPlanDrrGraphicObjectCollection::IsDirty() const
{
    bool dirty = TpsGraphicObjectBase::IsDirty();
    for (auto it = mDrrGoMap.begin(); it != mDrrGoMap.end(); ++it){
        if (nullptr != it->second)
        {
            dirty |= it->second->IsDirty();
        }
    }
    return dirty;
}

void EasyPlanDrrGraphicObjectCollection::SetDirty(bool bFlag)
{
    TpsGraphicObjectBase::SetDirty(bFlag);
    if (!bFlag){
        for (auto it = mDrrGoMap.begin(); it != mDrrGoMap.end(); ++it){
            if(nullptr != it->second) it->second->SetDirty(bFlag);
        }
    }
}
TPS_END_NAMESPACE