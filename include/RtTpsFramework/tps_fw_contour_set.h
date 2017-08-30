#ifndef TPS_FW_CONTOUR_SET_H_
#define TPS_FW_CONTOUR_SET_H_

#include "tps_fw_defs.h"

#include "RtTpsFramework/tps_fw_arithmetic.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_fw_contour.h"

#include <vector>
#include <map>

TPS_BEGIN_NAMESPACE  // begin namespace tps

class Contour;

class TPS_FW_DECLSPEC PageContours
{
public:
    PageContours();
    ~PageContours();
    PageContours(const PageContours& pc);
    PageContours& operator =(const PageContours& pc);

    int slice_number;
    bool is_dirty;
    std::vector<Contour> contour_list;
};

class TPS_FW_DECLSPEC ContourSet
{
public:
    ContourSet();
    ~ContourSet();
    ContourSet(const ContourSet& cs);
    ContourSet& operator=(const ContourSet& cs);

    void Clear();
    bool IsAnyDirty() const;
    int GetTotalContoursCount() const;

    int TotalVertexCount() const;
    inline bool IsEmpty() const { return page_contours.empty(); }
    //return slice count, if empty, return 0
    int GetSliceRange(int& min, int& max) const;

    void ClearDirty() const;

    //private:
    VEC3        normal;
    int         axis;//0: X, 1: Y, 2: Z in volume c.s.
    //cs的volume坐标系，按照主轴方向旋转体数据的volume坐标系而来，原点相同
    MAT4        pat_to_vol;
    double      spacing[3];

    std::map<int, PageContours*> page_contours;
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_FW_CONTOUR_SET_H_