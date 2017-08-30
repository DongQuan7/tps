////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_cmd_edit_beam.h
/// 
///  \brief class for edit beam
/// 
///  \version 1.0
/// 
///  \date    Nov.6,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_EDIT_BEAM_H_
#define TPS_BL_CMD_EDIT_BEAM_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_create_beam.h"

TPS_BEGIN_NAMESPACE;

class RtBeam;
class RtBeamsegment;
class RtBlock;
class RtMachine;

class TPS_BL_DECLSPEC EditBeamCmd : public CreateBeamCmd{
public:
    EditBeamCmd(const TpsCertifiedBeam& pocoBeam, int editType);
    virtual ~EditBeamCmd(void);

    inline void SetOnTheFly (bool bOnTheFly ) { mOnTheFly = bOnTheFly;}

    void GetBeamCreated(const RtBeam*& pBeam);

protected:
    virtual int Execute();

    int EditBeamIsoCenter_i(RtBeam* beam, std::string sImageUid);

    int EditBeamJawFollow_i(RtDbDef::PLAN_TYPE planType, RtBeam* beam, RtMachine* machine);

    int EditBeamJawSize_i(RtBeam* beam);

    int EditBeamShieldBlock_i(RtBeam* beam, RtMachine* machine);

    void EditBolus_i(RtBeam* beam);

    void EditWedge_i(RtBeam* beam);

    void EditBeamTray_i(RtBeam* beam);

    int EditBeamAngles_i(RtBeam* beam, std::string sImageUid);

    bool GetUpdateFluenceMapFlag_i(int editType, RtDbDef::PLAN_TYPE normalgroupType);

    bool GetReCreateArcSegmentFlag_i(int editType, RtDbDef::PLAN_TYPE normalgroupType);

    bool GetAutoShapeForApertureFlag_i(int editType, RtDbDef::PLAN_TYPE normalgroupType);

    bool GetAutoShapeForShieldBlockFlag_i(int editType, RtDbDef::PLAN_TYPE normalgroupType);

    bool IsNeedAutoShapeForAperture_i();

    bool IsNeedAutoShapeForShieldBlock_i(RtDbDef::PLAN_TYPE normalgroupType);

    int ReCreateArcSegment_i(RtBeam* beam, RtMachine* machine, RtDbDef::PLAN_TYPE normalgroupType, std::string sImageUid);

    int AutoShapeForAperture_i(RtBeam* beam, RtMachine* machine, RtDbDef::PLAN_TYPE normalgroupType, std::string sImageUid);

    int AutoShapeForShieldingBlock_i(RtBeam* beam, RtMachine* machine);

private:
    bool                    mOnTheFly;
    int                     mEditType;
  
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(EditBeamCmd);
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_EDIT_PLAN_H_