#include "RtTpsCertifiedLib/rt_tps_certifiedblock.h"
#include "RtTpsProtoLib/rt_tps_proto_block.pb.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

void TpsCertifiedBlock::Convert(const proto::RtTpsProtoBlock& protoBlock)
{
    mBlockUid = protoBlock.block_uid();
    mBlockName = protoBlock.block_name();
    mBlockType = protoBlock.block_type();
    mMaterial = protoBlock.material();
    mThickness = protoBlock.thickness();
    mBlockData.clear();
    proto::BlockPoint pt;
    for (int i = 0; i < protoBlock.block_data_size(); ++i){
        pt = protoBlock.block_data(i);
        mBlockData.push_back(Mcsf::Point2f(pt.x(), pt.y()));
    }
}

void TpsCertifiedBlock::ConvertBack(proto::RtTpsProtoBlock* protoBlock) const
{
    if (protoBlock == nullptr) return;

    protoBlock->set_block_uid(mBlockUid);
    protoBlock->set_block_name(mBlockName);
    protoBlock->set_block_type(mBlockType);
    protoBlock->set_material(mMaterial);
    protoBlock->set_thickness(mThickness);
    protoBlock->clear_block_data();
    proto::BlockPoint* pt;
    for (int i = 0; i < mBlockData.size(); ++i){
        pt = protoBlock->add_block_data();
        pt->set_x(mBlockData[i].GetX());
        pt->set_y(mBlockData[i].GetY());
    }
}

TPS_END_NAMESPACE