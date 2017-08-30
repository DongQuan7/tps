////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  xi.chen  xi.chen@united-imaging.com
/// 
///  \file rt_tps_doselinesetting.h
/// 
///  \brief struct DoseOptimizationParam declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/10/29
////////////////////////////////////////////////////////////////
#ifndef RT_TPS_DOSELINESETTING_H_
#define RT_TPS_DOSELINESETTING_H_

#include "tps_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"

TPS_BEGIN_NAMESPACE

struct RtTpsDoselineSetting{
    /// #[tag field]
    RtDbDef::DISPLAYMODE_TYPE m_eDisplayMode;

    /// #[tag field]
    RtDbDef::HundredPercentDefinition_TYPE m_eHundredPercentDefinition;

    /// #[tag field]
    float m_fSpecifiedDose;

    /// #[tag field]
    RtDbDef::RENDERMODE_TYPE m_eRenderMode;

    /// #[tag field]
    float m_fTransParency;

    /// #[tag field]
    float m_fTransParency3D;

    /// #[tag field]
    bool m_bEnableSkinDoseView;

    /// #[tag field]
    float m_fMaxSkinDose;
};

TPS_END_NAMESPACE

#endif // RT_TPS_DOSELINESETTING_H_