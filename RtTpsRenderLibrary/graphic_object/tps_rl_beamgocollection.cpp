#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"

TPS_BEGIN_NAMESPACE
TpsBeamGraphicObjectCollection::TpsBeamGraphicObjectCollection() :mActiveBeamUID(""), mIsJoined(true){
}
TpsBeamGraphicObjectCollection::~TpsBeamGraphicObjectCollection() {
}
bool TpsBeamGraphicObjectCollection::Initialize() {
    mBeamGoMap.clear();
    return true;
}
bool TpsBeamGraphicObjectCollection::Finalize() {
    return true;
}
bool TpsBeamGraphicObjectCollection::DeleteBeam(const std::string &suid) {
    std::map<std::string,std::shared_ptr<TpsBeamGraphicObject>>::iterator itr = mBeamGoMap.find(suid);
    if(itr == mBeamGoMap.end()) {
        return false;
    }
    mBeamGoMap.erase(itr);
    return true;
}
void TpsBeamGraphicObjectCollection::AddBeam(const std::string &str ,std::shared_ptr<TpsBeamGraphicObject> beamObj) {
    
    auto it = mBeamGoMap.find(str);
    if (it != mBeamGoMap.end()){
        TPS_LOG_DEV_WARNING<<"The beam already exist.";
    }

    mBeamGoMap[str] = beamObj;
}
void TpsBeamGraphicObjectCollection::RemoveAll() {
    mBeamGoMap.clear();
}
std::shared_ptr<TpsBeamGraphicObject> TpsBeamGraphicObjectCollection::SelectBeam(const std::string &str) {
    std::map<std::string,std::shared_ptr<TpsBeamGraphicObject>>::iterator itr = mBeamGoMap.find(str);
    if(itr != mBeamGoMap.end()) {
        std::shared_ptr<TpsBeamGraphicObject> beamObj = itr->second;
        return beamObj;
    }
    return NULL;
}
std::map<std::string,std::shared_ptr<TpsBeamGraphicObject>> TpsBeamGraphicObjectCollection::GetBeamMap() const{
    return mBeamGoMap;
}

std::shared_ptr<TpsBeamGraphicObject> TpsBeamGraphicObjectCollection::GetActiveBeamGo() const {
    auto itBeam = mBeamGoMap.find(mActiveBeamUID);
    if (itBeam == mBeamGoMap.end()){
        return nullptr;
    }

    return itBeam->second;
}

void TpsBeamGraphicObjectCollection::SetActiveBeam(std::string beamUid){
    mActiveBeamUID = beamUid;
    //mActiveBeam = mBeamGoMap[beamUid];
}

bool TpsBeamGraphicObjectCollection::IsDirty() const
{
    bool dirty = TpsGraphicObjectBase::IsDirty();
    for (auto it = mBeamGoMap.begin(); it != mBeamGoMap.end(); ++it){
        if (nullptr != it->second)
        {
            dirty |= it->second->IsDirty();
        }
    }
    return dirty;
}

void TpsBeamGraphicObjectCollection::SetDirty(bool bFlag)
{
    TpsGraphicObjectBase::SetDirty(bFlag);
    if (!bFlag){
        for (auto it = mBeamGoMap.begin(); it != mBeamGoMap.end(); ++it){
            if(nullptr != it->second) it->second->SetDirty(bFlag);
        }
    }
}

const double* TpsBeamGraphicObjectCollection::GetFieldLimits() const
{
    return mFieldLimits;
}

void TpsBeamGraphicObjectCollection::SetFieldLimits(const double* limits)
{
    if (limits == nullptr) return;
    memcpy(mFieldLimits, limits, 4 * sizeof(double));
}
TPS_END_NAMESPACE
