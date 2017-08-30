#ifndef TPS_BL_ADD_ROI_ADVANCED_PARAMETER_STRUCTRUES_H_
#define TPS_BL_ADD_ROI_ADVANCED_PARAMETER_STRUCTRUES_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

#include "boost/variant.hpp"

#include <vector>

TPS_BEGIN_NAMESPACE  // begin namespace tps

struct MirrorRoiPara
{
    MirrorDirection direction;
};

struct ExpandRoiPara
{
    bool is2D;
    bool isIsotropic;
    float isotropicMargin;
    //R, L, A, P, F, H
    float margin[6];
    bool isToCreateRing;
    float ringSize;
};

struct RoiCombinationElement
{
    std::string roiUid;
    CombineRoiMode mode;
};

struct CombineRoiPara
{
    std::vector<RoiCombinationElement> elements;
};

class operator_visitor : public boost::static_visitor<void>
{
public:
    void operator()(const MirrorRoiPara& mirror) const
    {
        pMirror = &mirror;
    }

    void operator()(const ExpandRoiPara& expand) const
    {
        pExpand = &expand;
    }

    void operator()(const CombineRoiPara& combine) const
    {
        pCombind = &combine;
    }

    mutable const MirrorRoiPara* pMirror;
    mutable const ExpandRoiPara* pExpand;
    mutable const CombineRoiPara* pCombind;
};

struct OperatorPara
{
    typedef boost::variant<MirrorRoiPara, ExpandRoiPara, CombineRoiPara> ParaType;

    BasedOnRoiOperator mode;
    ParaType paraData;
    operator_visitor visitor;

    const MirrorRoiPara& MirrorPara() const
    {
        boost::apply_visitor(visitor, paraData);
        return *visitor.pMirror;
    }

    const CombineRoiPara& CombinePara() const
    {
        boost::apply_visitor(visitor, paraData);
        return *visitor.pCombind;
    }

    const ExpandRoiPara& ExpandPara() const
    {
        boost::apply_visitor(visitor, paraData);
        return *visitor.pExpand;
    }
};

struct AddRoiBasedOnRoiPara
{
    bool isPreview;

    std::string targetRoiUid;
    std::string roiName;
    int planningRole;
    int subRole;
    float roiColor[3];
    std::string sourceRoiUid;

    std::vector<OperatorPara> vecOpParas;
};

struct AddRoiBasedOnDosePara
{
    bool isPreview;

    std::string imageUid;
    std::string planUid;

    std::string targetRoiUid;
    std::string roiName;
    int planningRole;
    int subRole;
    float roiColor[3];

    float fromDose;
    float toDose;
};

TPS_END_NAMESPACE  // end namespace tps

#endif // tps_bl_add_roi_advanced_parameter_structrues_h__
