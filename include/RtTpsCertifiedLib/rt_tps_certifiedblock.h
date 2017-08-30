#ifndef RT_TPS_CERTIFIEDBLOCK_H_
#define RT_TPS_CERTIFIEDBLOCK_H_

#include "tps_defs.h"
#include <vector>
#include "Mcsf3DArithmetic/mcsf_3d_point2f.h"
#include "RtTpsCertifiedLib/rt_tps_certified_segment.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto
{
    class RtTpsProtoBlock;
}

struct TpsCertifiedBlock
{
    std::string mBlockUid;
    std::string mBlockName;
    int mBlockType;
    int mMaterial;
    double mThickness;
    std::vector<Mcsf::Point2f> mBlockData;

    void Convert(const proto::RtTpsProtoBlock& protoBlock);

    void ConvertBack(proto::RtTpsProtoBlock* protoBlock) const;
};

TPS_END_NAMESPACE
#endif