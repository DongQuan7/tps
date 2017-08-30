#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_contour_set.h"
#include "RtTpsFramework/tps_fw_contour.h"

#include <string.h>

TPS_BEGIN_NAMESPACE

PageContours::PageContours() : is_dirty(true)
{

}

PageContours::~PageContours()
{
    contour_list.clear();
}

PageContours::PageContours(const PageContours& pc) : 
    is_dirty(pc.is_dirty), slice_number(pc.slice_number), 
    contour_list(pc.contour_list)
{

}

PageContours& PageContours::operator =(const PageContours& pc)
{
    is_dirty = pc.is_dirty;
    slice_number = pc.slice_number;
    contour_list = pc.contour_list;
    return *this;
}

ContourSet::ContourSet() : axis(2), normal(0, 0, 0)
{
    pat_to_vol.SetIdintity();
}

ContourSet::~ContourSet()
{
    Clear();
}

ContourSet::ContourSet(const ContourSet& cs)
{
    normal = cs.normal;
    axis = cs.axis;
    pat_to_vol = cs.pat_to_vol;
    for (auto it = cs.page_contours.begin(); it != cs.page_contours.end(); ++it)
    {
        page_contours[it->first] = new PageContours(*it->second);
    }
}

ContourSet& ContourSet::operator =(const ContourSet& cs)
{
    Clear();

    normal = cs.normal;
    axis = cs.axis;
    pat_to_vol = cs.pat_to_vol;
    for (auto it = cs.page_contours.begin(); it != cs.page_contours.end(); ++it)
    {
        page_contours[it->first] = new PageContours(*it->second);
    }
    return *this;
}

void ContourSet::Clear()
{
    pat_to_vol.SetIdintity();
    axis = 2;
    normal = VEC3(0, 0, 0);

    for (auto it = page_contours.begin(); it != page_contours.end(); ++it)
    {
        TPS_DEL_PTR(it->second);
    }
    page_contours.clear();
    return;
}

bool ContourSet::IsAnyDirty() const
{
    bool Ret = false;
    for (auto it = page_contours.begin(); it != page_contours.end(); ++it)
    {
        if (it->second->is_dirty)
        {
            Ret = true;
            break;
        }
    }
    return Ret;
}

int ContourSet::GetTotalContoursCount() const
{
    int count = 0;
    for (auto it = page_contours.begin(); it != page_contours.end(); ++it)
    {
        count += it->second->contour_list.size();
    }
    return count;
}

int ContourSet::TotalVertexCount() const
{
    int count = 0;
    for (auto it = page_contours.begin(); it != page_contours.end(); ++it)
    {
        for (int i = 0; i < it->second->contour_list.size(); ++i)
        {
            count += it->second->contour_list[i].VertexCount();
        }
    }
    return count;
}

int ContourSet::GetSliceRange(int& min, int& max) const
{
    if (page_contours.empty()) return 0;

    auto it = page_contours.begin();
    min = it->second->slice_number;
    max = min;
    for (; it != page_contours.end(); ++it)
    {
        min = TPS_MIN(it->second->slice_number, min);
        max = TPS_MAX(it->second->slice_number, max);
    }
    return max - min + 1;
}

void ContourSet::ClearDirty() const
{
    for (auto it = page_contours.begin(); it != page_contours.end(); ++it)
    {
        it->second->is_dirty = false;
    }
}
TPS_END_NAMESPACE