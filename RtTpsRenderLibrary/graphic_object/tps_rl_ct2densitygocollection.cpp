#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_ct2densitygocollection.h"

TPS_BEGIN_NAMESPACE

Ct2DensityGraphicObejctCollection::Ct2DensityGraphicObejctCollection()
{

}
Ct2DensityGraphicObejctCollection::~Ct2DensityGraphicObejctCollection()
{

}

void Ct2DensityGraphicObejctCollection::AddCt2DensityGo(Ct2DensityGraphicObejctPtr go)
{
    mCt2DensityGoCollection.push_back(go);
}
void Ct2DensityGraphicObejctCollection::RemoveCt2DensityGo(const std::string& ct2densityUid)
{
    for (auto iter = mCt2DensityGoCollection.begin(); iter != mCt2DensityGoCollection.end(); ++iter)
    {
        if((*iter)->GetCT2DensityTableUid() == ct2densityUid)
        {
            mCt2DensityGoCollection.erase(iter);
            break;
        }
    }
}
Ct2DensityGraphicObejctPtr Ct2DensityGraphicObejctCollection::GetCt2DensityGo(const std::string& ct2densityUid)
{
    for (auto iter = mCt2DensityGoCollection.begin(); iter != mCt2DensityGoCollection.end(); ++iter)
    {
        if((*iter)->GetCT2DensityTableUid() == ct2densityUid)
        {
            return (*iter);
        }
    }
    return nullptr;
}

TPS_END_NAMESPACE