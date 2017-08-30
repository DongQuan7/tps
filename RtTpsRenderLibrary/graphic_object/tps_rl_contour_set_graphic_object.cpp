#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_contour_set_graphic_object.h"
#include "RtTpsFramework/tps_fw_contour_set.h"
#include "RtTpsFramework/tps_fw_signed_distance_field.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

ContourSetGraphicObject::ContourSetGraphicObject() : mContourSet(new ContourSet())
{
    
}

ContourSetGraphicObject::~ContourSetGraphicObject()
{
    TPS_DEL_PTR(mContourSet);
}

ContourSet* ContourSetGraphicObject::GetContourSet() const
{
    return mContourSet;
}

CSGOCollection::CSGOCollection()
{

}

CSGOCollection::~CSGOCollection()
{
    Clear();
}

void CSGOCollection::Clear()
{
    mCSGOMap.clear();
}

void CSGOCollection::AddContourSet(const std::string& uid, const CSGOPtr& csGO)
{
    mCSGOMap[uid] = csGO;
}

CSGOPtr CSGOCollection::GetContourSet(const std::string& uid) const
{
    auto it = mCSGOMap.find(uid);
    if (it == mCSGOMap.end())
    {
        return nullptr;
    }
    return it->second;
}

std::map<std::string, CSGOPtr> CSGOCollection::GetCSGOMap() const
{
    return mCSGOMap;
}

Mcsf::MedViewer3D::Vector3D CSGOCollection::GetNormal() const
{
    return mNormal;
}

void CSGOCollection::SetNormal(const Mcsf::MedViewer3D::Vector3D& normal)
{
    mNormal = normal;
}
TPS_END_NAMESPACE  // end namespace tps