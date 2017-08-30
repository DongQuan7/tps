#ifndef TPS_FW_ROI_ENTITY_H_
#define TPS_FW_ROI_ENTITY_H_

#include "tps_fw_defs.h"

#include "RtTpsFramework/tps_fw_arithmetic.h"
#include <set>

TPS_BEGIN_NAMESPACE  // begin namespace tps

class RtVoi;
class ContourSet;
class SignedDistanceField;
class TriMesh;

enum ROI_SHAPE
{
    CONTOUR_SET = 0,
    SDF,
    MESH,
};

class TPS_FW_DECLSPEC ROIEntity
{
public:
    ROIEntity(RtVoi* voi);
    ~ROIEntity(void);

    RtVoi* GetInnerRoi() const;
    ContourSet* GetContourSet() const;
    SignedDistanceField* GetSDF() const;

    bool IsSkin() const;

    bool IsEmpty() const;

    void SetSliceDirty(int index);
    void SetSlicesDirty(int begin, int end);
    bool IsContourSetDirty() const;

    //judge if a specified point in patient c.s. belongs to the VOI.
    bool IsInROI(const PT3& point) const;

    inline void SetIsQA(bool isQA)
    {
        mIsQA = isQA;
    }
    inline bool IsQA() const
    {
        return mIsQA;
    }
    inline void SetInGrid(float inGrid)
    {
        mInGrid = inGrid;
    }
    inline float GetInGrid() const
    {
        return mInGrid;
    }
    inline void SetMinDose( const float minDose ) 
    {
        mMinDose = minDose;
    }
    inline float GetMinDose() const 
    {
        return mMinDose;
    }
    inline void SetMaxDose( const float maxDose ) 
    {
        mMaxDose = maxDose;
    }
    inline float GetMaxDose() const 
    {
        return mMaxDose;
    }
    inline void SetMeanDose( const float meanDose ) 
    {
        mMeanDose = meanDose;
    }
    inline float GetMeanDose() const 
    {
        return mMeanDose;
    }

    std::string GetUid() const;
    std::string GetSeriesUid() const;

    std::set<int> GetContourSlices() const;
private:
    RtVoi*                  mInnerRoi;
    ContourSet*             mContourSet;
    SignedDistanceField*    mSDF;
    TriMesh*                mMesh;
    ROI_SHAPE               mPrimaryShape;

    float                   mInGrid;
    float                   mMinDose;
    float                   mMaxDose;
    float                   mMeanDose;
    bool                    mIsQA;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(ROIEntity);
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_FW_ROI_ENTITY_H_