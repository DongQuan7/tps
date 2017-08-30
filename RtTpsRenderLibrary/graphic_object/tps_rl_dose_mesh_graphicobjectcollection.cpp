#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobjectcollection.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

DoseMeshGraphicObjectCollection::DoseMeshGraphicObjectCollection()
{

}

DoseMeshGraphicObjectCollection::~DoseMeshGraphicObjectCollection(){
    mDoseMeshGoCollection.clear();
}

void DoseMeshGraphicObjectCollection::AddDoseMeshGo(TpsDoseMeshGraphicObjectPtr go) {
    mDoseMeshGoCollection.push_back(go);
}
void DoseMeshGraphicObjectCollection::RemoveDoseMeshGo(const float& doseRatio) {
    for (auto iter = mDoseMeshGoCollection.begin(); iter != mDoseMeshGoCollection.end(); ++iter)
    {
        //doseratio为纯小数，整数位为0，可以判1e-6
        if(abs((*iter)->GetDoseRatio() - doseRatio) < 1e-6)
        {
            mDoseMeshGoCollection.erase(iter);
            break;
        }
    }
}
void DoseMeshGraphicObjectCollection::ClearAllDoseMeshGo()
{
    mDoseMeshGoCollection.clear();
}
TpsDoseMeshGraphicObjectPtr DoseMeshGraphicObjectCollection::GetDoseMeshGo(const float& doseRatio) {
    for (auto iter = mDoseMeshGoCollection.begin(); iter != mDoseMeshGoCollection.end(); ++iter)
    {
        //doseratio为纯小数，整数位为0，可以判1e-6
        if(abs((*iter)->GetDoseRatio() - doseRatio) < 1e-6)
        {
            return (*iter);
        }
    }
    return nullptr;
}

std::vector<TpsDoseMeshGraphicObjectPtr>& DoseMeshGraphicObjectCollection::GetAllDoseMeshGo() {
    return mDoseMeshGoCollection;
}

void DoseMeshGraphicObjectCollection::SetAllDoseMeshGo(std::vector<TpsDoseMeshGraphicObjectPtr> doseMeshGoCollection) {
    mDoseMeshGoCollection = doseMeshGoCollection;
}

void DoseMeshGraphicObjectCollection::SetDoseMeshGoVisible(const float& doseRatio, const bool& visible) {
    for (auto iter = mDoseMeshGoCollection.begin(); iter != mDoseMeshGoCollection.end(); ++iter)
    {
        //doseratio为纯小数，整数位为0，可以判1e-6
        if(abs((*iter)->GetDoseRatio() - doseRatio) < 1e-6)
        {
            (*iter)->SetVisibilty(visible);
            break;
        }
    }
}

TPS_END_NAMESPACE  // end namespace tps