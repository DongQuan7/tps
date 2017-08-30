//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xiaoqing.Shaangguan Xiaoqing.Shaangguan@united-imaging.com
//
/// \file    tps_commands_defs.cpp
///
/// \brief   
/// 
/// \version 1.0
/// 
/// \date    2015/07/07
/////////////////////////////////////////////////////////////////////////

#ifndef TPS_BL_DEFS_H_
#define TPS_BL_DEFS_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE

#ifdef TPS_BL_EXPORT
#define TPS_BL_DECLSPEC    TPS_EXPORT_CLS
#else
#define TPS_BL_DECLSPEC    TPS_IMPORT_CLS
#endif



typedef enum 
{
    DOSE_RESCALE_REASON_NONE                              = 0x0000,
    NORMALGROUP_GOAL_TARGET_CHANGED                       = 0x0001,// 处方目标发生变化
    BEAM_WEIGHT_CHANGED                                   = 0x0002,// Beam Weight 发生变化
    DOSE_PERFRACTION_PERCENTAGE_PRODUCT_CHANGED           = 0x0004,// 单次剂量乘以目标doseline比例的乘积发生了变化
    DOSE_FRACTION_CHANGED                                 = 0x0008,// 处方次数发生变化
    BEAM_MU_CHANGED                                       = 0x0010,// Beam MU 发生了修改

}DoseRescaleReasonType;

struct RescaleDosePara
{
    // 目标剂量缩放系数 
    float muRescaleFactor;

    // 判断是否需要重新计算BeamMU
    DoseRescaleReasonType doseRescaleReasonType;

};



typedef enum
{
    Unknow_Result				= -1,
    Identical					= 0,
    DIFF_Dosegrid_Size			= 1,
    DIFF_Dosegrid_Spacing		= 2,
    DIFF_Dosegrid_Both			= 3,
}CompareDoseGridResult;

TPS_END_NAMESPACE

#endif //TPS_COMMANDS_DEFS_H
