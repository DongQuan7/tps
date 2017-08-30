#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_contour_set.h"
#include "RtTpsFramework/tps_fw_contour.h"
#include "RtTpsFramework/tps_fw_signed_distance_field.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_contour.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

ROIEntity::ROIEntity(RtVoi* voi) : 
    mInnerRoi(voi),
    mContourSet(new ContourSet()),
    mSDF(new SignedDistanceField()),
    mMesh(nullptr),
    mPrimaryShape(CONTOUR_SET)
{

}

ROIEntity::~ROIEntity(void)
{
    //TODO,ÔÝÊ±±£Áô£¬ÓÉVOIEntityÉ¾³ý
    //TPS_DEL_PTR(mInnerRoi);
    TPS_DEL_PTR(mContourSet);
    TPS_DEL_PTR(mSDF);
    //TPS_DEL_PTR(mMesh);
}

RtVoi* ROIEntity::GetInnerRoi() const
{
    return mInnerRoi;
}

ContourSet* ROIEntity::GetContourSet() const
{
    return mContourSet;
}

SignedDistanceField* ROIEntity::GetSDF() const
{
    return mSDF;
}

bool ROIEntity::IsSkin() const
{
    return mInnerRoi->get_planningrole() == RtDbDef::PLANNING_ROLE_EXTERNAL;
}

bool ROIEntity::IsEmpty() const
{
    return mContourSet == nullptr || 
           mContourSet->TotalVertexCount() == 0;
}

void ROIEntity::SetSliceDirty(int index)
{
    auto it = mContourSet->page_contours.find(index);
    if (it == mContourSet->page_contours.end())
    {
        auto pc = new PageContours();
        pc->is_dirty = true;
        mContourSet->page_contours[index] = pc;
    }
    else it->second->is_dirty = true;
}

void ROIEntity::SetSlicesDirty(int begin, int end)
{
    for (int i = begin; i <= end; ++i)
    {
        this->SetSliceDirty(i);
    }
}

bool ROIEntity::IsContourSetDirty() const
{
    for (auto it = mContourSet->page_contours.begin(); 
        it != mContourSet->page_contours.end(); ++it)
    {
        if (it->second->is_dirty) return true;
    }
    return false;
}

bool ROIEntity::IsInROI(const PT3& point) const
{
    PT3 p = mSDF->GetPatToSDF().Transform(point);
    return (*mSDF)(p.x, p.y, p.z) > -1e-6;
}

std::string ROIEntity::GetUid() const
{
    return mInnerRoi->get_uid();
}

std::string ROIEntity::GetSeriesUid() const
{
    return mInnerRoi->get_seriesuid();
}

std::set<int> ROIEntity::GetContourSlices() const
{
    // ContourSlices is secondary data
    std::set<int> contourSlices;
    const std::vector<RtContour*> vecContours = mInnerRoi->get_contours();
    for (auto it = vecContours.begin(); it != vecContours.end(); ++it)
    {
        if(*it != nullptr)
        {
            contourSlices.insert((*it)->get_sliceindex() - 1);
        }
    }
    return contourSlices;
}
TPS_END_NAMESPACE