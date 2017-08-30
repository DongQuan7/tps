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
    NORMALGROUP_GOAL_TARGET_CHANGED                       = 0x0001,// ����Ŀ�귢���仯
    BEAM_WEIGHT_CHANGED                                   = 0x0002,// Beam Weight �����仯
    DOSE_PERFRACTION_PERCENTAGE_PRODUCT_CHANGED           = 0x0004,// ���μ�������Ŀ��doseline�����ĳ˻������˱仯
    DOSE_FRACTION_CHANGED                                 = 0x0008,// �������������仯
    BEAM_MU_CHANGED                                       = 0x0010,// Beam MU �������޸�

}DoseRescaleReasonType;

struct RescaleDosePara
{
    // Ŀ���������ϵ�� 
    float muRescaleFactor;

    // �ж��Ƿ���Ҫ���¼���BeamMU
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
