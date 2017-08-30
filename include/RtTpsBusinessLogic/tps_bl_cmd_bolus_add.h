//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhendong.wang zhendong.wang@united-imaging.com
///
///  \file    tps_bl_bolus_add_cmd.h
///  \brief   addbolus command
///
///  \version 1.0
///  \date    February. 9, 2017
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_BOLUS_ADD_H_
#define TPS_BL_CMD_BOLUS_ADD_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsProtoLib/rt_tps_bolus.pb.h"


TPS_BEGIN_NAMESPACE

class RtTpsBolus;
class RtBeam;
class RtBeamsegment;
class RtMlcshape;
class ROIEntity;

class TPS_BL_DECLSPEC AddBolusCmd : public TpsCommand
{
public:

    AddBolusCmd(proto::RtTpsBolus& protoBolus);

    virtual ~AddBolusCmd(void);

    void GetCreatedBolus(const ROIEntity*& pBlous);

protected:
    virtual int Execute();

private:
    proto::RtTpsBolus& mProtoBolus;

    bool CreateBolus_i(const std::string& imageUID, RtBeam* beam);

    std::string MakeBolus(const std::string& imageUID,
         const std::string& normalgroupuid,
         const std::vector<RtBeamsegment*>& segments,
         float bolusThickness, float margin[4],
         float bolusMassDensity,
         float bolusRerenceEDensity);

    bool CombineMlcShapes_i(const std::vector<RtMlcshape*>& vecMlcShape, 
        RtMlcshape* shape, float miniLeafGap);
};

TPS_END_NAMESPACE

#endif //TPS_CCA_ADDVOI_CMD_H_