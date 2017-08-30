////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_block_graphic_object.h
/// 
///  \brief class BlockGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/03/17
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_BLOCK_GRAPHIC_OBJECT_H_
#define TPS_RL_BLOCK_GRAPHIC_OBJECT_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "McsfMedViewer3DArithmetic/point2d.h"

#include <vector>
namespace Mcsf{
    namespace MedViewer3D{
        class Point2D;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class BlockGraphicObject
/// 
/// \brief *****
class TPS_RL_DECLSPEC BlockGraphicObject : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    BlockGraphicObject();
    virtual ~BlockGraphicObject();

    RtDbDef::BLOCK_TYPE GetBlockType() const;
    void SetBlockType(RtDbDef::BLOCK_TYPE type);

    void SetBlockData(const std::vector<Mcsf::MedViewer3D::Point2D>& blockData);
    void GetBlockData(std::vector<Mcsf::MedViewer3D::Point2D>*& blockData);

private:
    RtDbDef::BLOCK_TYPE                       mBlockType;
    std::vector<Mcsf::MedViewer3D::Point2D> mBlockData;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(BlockGraphicObject);
};

TPS_END_NAMESPACE  // end namespace tps

#endif