////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  liu linqi  linqi.liu@united-imaging.com
/// 
///  \file tps_rl_doselineobject.h
/// 
///  \brief class TpsDoseLineObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/08/04
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DOSELINECOBJECT_H
#define TPS_RL_DOSELINECOBJECT_H

#include "tps_rl_defs.h"
#include "McsfMedViewer3DArithmetic/vector3d.h"
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"
#include "Mcsf3DArithmetic/mcsf_3d_matrix4x4.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include <vector>

TPS_BEGIN_NAMESPACE

struct DoseLineAttribute{
    //float mDoseQuantity;
    float mColor[3];
    bool mVisible;
    float mDosePercent;
};

class TPS_RL_DECLSPEC TpsDoseLineGraphicObject : public TpsGraphicObjectBase {
public:
    //the constructor and destructor
    TpsDoseLineGraphicObject();
    ~TpsDoseLineGraphicObject();

    //////////////////////////////////////////////////////////////////////////
    // \brief get hide or display dose line.
    // \param[out] Group DoseLineAttribute
    void GetDisplayAttributes(std::vector<DoseLineAttribute>& vecDoseLine) const;

    //////////////////////////////////////////////////////////////////////////
    // \brief set hide or display dose line.
    // \param[in] Group doseLine attribute;
    bool SetDisplayAttributes(const std::vector<DoseLineAttribute>& vecDoseLine);

    inline DOSELINE_CALCMODE GetHundredPercentDoseMode() const { return mDoseMode; }
    inline void SetHundredPercentDoseMode(DOSELINE_CALCMODE mode) { mDoseMode = mode; }

    inline float GetSpecifiedDose() const { return mSpecifiedDose; }
    inline void SetSpecifiedDose(float dose) { mSpecifiedDose = dose; }

    inline float GetMaxDose() const{ return mMaxDose; }
    inline void SetMaxDose(float dose) { mMaxDose = dose; }
    inline float GetTotalGoal() const{ return mTotalGoal; }
    inline void SetTotalGoal(float totalGoal) { mTotalGoal = totalGoal; }

    inline float GetTransparency() const { return mTransparency; }
    inline void SetTransparency(float transparency) { mTransparency = transparency; }

    inline float GetTransparency3D() const { return mTransparency3D; }
    inline void SetTransparency3D(float transparency) { mTransparency3D = transparency; }

    inline void Clear() { mVecDoseLine.clear(); }

    //get\set vision mode, vision mode is a global setting
    inline void SetVisonMode(DOSELINE_RENDERMODE mode) { mVisionMode = mode; }
    inline DOSELINE_RENDERMODE GetVisionMode() const { return mVisionMode; }

    void SetDisplayMode(DOSELINE_DISPLAYMODE dispMode);
    DOSELINE_DISPLAYMODE GetDisplayMode();

    float GetOneHundredDoseValue() const;
private:
    std::vector<DoseLineAttribute>              mVecDoseLine;

    DOSELINE_CALCMODE                           mDoseMode;
    float                                         mSpecifiedDose;
    float   mMaxDose;
    float   mTotalGoal;
    float                                         mTransparency;
    float                                         mTransparency3D;
    DOSELINE_RENDERMODE                         mVisionMode;
    DOSELINE_DISPLAYMODE                           mDisPlayMode;
};
TPS_END_NAMESPACE
#endif