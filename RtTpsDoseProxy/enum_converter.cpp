//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2017
///  All rights reserved.
/// 
///  \author  Wang Zhendong  zhendong.wang@united-imaging.com
/// 
///  \file enum_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2017/01/03
///
///  \update  2017/04/18
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "enum_converter.h"
#include "alg/alg_common_algreturn.h"

TPS_BEGIN_NAMESPACE

TPS_ERROR_CODES EnumConverter::AlgConvertToTPS(ALG_RETURN_TYPE algType)
{
    TPS_ERROR_CODES tpsErrorCode;
    switch (algType)
    {
        case ALG_OK:
            tpsErrorCode = TPS_ER_SUCCESS;
            break;
        case ALG_ERROR:
            tpsErrorCode = TPS_ER_FAILURE;
            break;
        // Opt error
        case ALG_OPT_START:
            tpsErrorCode = TPS_ER_ALG_OPT_START;
            break;
        case ALG_OPT_ABORT:
            tpsErrorCode = TPS_ER_ALG_OPT_ABORT;
            break;
        case ALG_OPT_SUSPEND:
            tpsErrorCode = TPS_ER_ALG_OPT_SUSPEND;
            break;
        case ALG_OPT_RESUME:
            tpsErrorCode = TPS_ER_ALG_OPT_RESUME;
            break;
        case ALG_OPT_START_FAILED:
            tpsErrorCode = TPS_ER_ALG_OPT_START_FAILED;
            break;
        case ALG_OPT_INIT_FAILED:
            tpsErrorCode = TPS_ER_ALG_OPT_INIT_FAILED;
            break;
        case ALG_OPT_SUSPEND_FAILED:
            tpsErrorCode = TPS_ER_ALG_OPT_SUSPEND_FAILED;
            break;
        case ALG_OPT_RESUME_FAILED:
            tpsErrorCode = TPS_ER_ALG_OPT_RESUME_FAILED;
            break;
        case ALG_OPT_ABORT_FAILED:
            tpsErrorCode = TPS_ER_ALG_OPT_ABORT_FAILED;
            break;
        case ALG_INPUT_NOT_VALID:
            tpsErrorCode = TPS_ER_ALG_INPUT_NOT_VALID;
            break;
        case ALG_EMPTY_GRID:
            tpsErrorCode = TPS_ER_ALG_EMPTY_GRID;
            break;
        case ALG_OPT_IMPROPER_GRIDSIZE:
            tpsErrorCode = TPS_ER_ALG_OPT_IMPROPER_GRIDSIZE;
            break;
        case ALG_OPT_WRONG_P2G_MATRIX:
            tpsErrorCode = TPS_ER_ALG_OPT_WRONG_P2G_MATRIX;
            break;
        case ALG_DBPARAM_FAILED:
            tpsErrorCode = TPS_ER_ALG_DBPARAM_FAILED;
            break;
        case ALG_LOAD_MACHINE_FAILED:
            tpsErrorCode = TPS_ER_ALG_LOAD_MACHINE_FAILED;
            break;
        case ALG_OPT_WRONG_MLC_SETTING:
            tpsErrorCode = TPS_ER_ALG_OPT_WRONG_MLC_SETTING;
            break;
        case ALG_OPT_WRONG_BEAM:
            tpsErrorCode = TPS_ER_ALG_OPT_WRONG_BEAM;
            break;
        case ALG_WRONG_BEAM_720:
            tpsErrorCode = TPS_ER_ALG_WRONG_BEAM_720;
            break;
        case ALG_NO_BEAM:
            tpsErrorCode = TPS_ER_ALG_NO_BEAM;
            break;
        case ALG_OPT_NO_COMMISSIONUNIT:
            tpsErrorCode = TPS_ER_ALG_OPT_NO_COMMISSIONUNIT;
            break;
        case ALG_WRONG_BEAM_GRID:
            tpsErrorCode = TPS_ER_ALG_WRONG_BEAM_GRID;
            break;
        case ALG_OPT_NO_SEG_IN_BEAM:
            tpsErrorCode = TPS_ER_ALG_OPT_NO_SEG_IN_BEAM;
            break;
        case ALG_OPT_WRONG_SEG_IN_BEAM:
            tpsErrorCode = TPS_ER_ALG_OPT_WRONG_SEG_IN_BEAM;
            break;
        case ALG_OPT_NO_SHAPE_IN_SEG:
            tpsErrorCode = TPS_ER_ALG_OPT_NO_SHAPE_IN_SEG;
            break;
        case ALG_OPT_NO_LEAF_IN_SHAPE:
            tpsErrorCode = TPS_ER_ALG_OPT_NO_LEAF_IN_SHAPE;
            break;
        case ALG_OPT_WRONG_ANGLE_IN_BEAM:
            tpsErrorCode = TPS_ER_ALG_OPT_WRONG_ANGLE_IN_BEAM;
            break;
        case ALG_OPT_WRONG_SHAPE_IN_BEAM:
            tpsErrorCode = TPS_ER_ALG_OPT_WRONG_SHAPE_IN_BEAM;
            break;
        case ALG_NO_ISO_POINT:
            tpsErrorCode = TPS_ER_ALG_NO_ISO_POINT;
            break;
        case ALG_OPT_WRONG_SEG_T_MATRIX:
            tpsErrorCode = TPS_ER_ALG_OPT_WRONG_SEG_T_MATRIX;
            break;
        case ALG_OPT_END_CONDITION_WRONG:
            tpsErrorCode = TPS_ER_ALG_OPT_END_CONDITION_WRONG;
            break;
        case ALG_OPT_WRONG_DVH_BIN_SIZE:
            tpsErrorCode = TPS_ER_ALG_OPT_WRONG_DVH_BIN_SIZE;
            break;
        case ALG_NO_VOI:
            tpsErrorCode = TPS_ER_ALG_NO_VOI;
            break;
        case ALG_OPT_WRONG_VOI:
            tpsErrorCode = TPS_ER_ALG_OPT_WRONG_VOI;
            break;
        case ALG_OPT_WRONG_VOI_SUBVOLUME:
            tpsErrorCode = TPS_ER_ALG_OPT_WRONG_VOI_SUBVOLUME;
            break;
        case ALG_OPT_WRONG_VOI_Contour:
            tpsErrorCode = TPS_ER_ALG_OPT_WRONG_VOI_CONTOUR;
            break;
        case ALG_OPT_NO_SKIN:
            tpsErrorCode = TPS_ER_ALG_OPT_NO_SKIN;
            break;
        case ALG_OPT_NO_GOAL:
            tpsErrorCode = TPS_ER_ALG_OPT_NO_GOAL;
            break;
        case ALG_OPT_TOO_MANY_TOTAL_GOALS:
            tpsErrorCode = TPS_ER_ALG_OPT_TOO_MANY_TOTAL_GOALS;
            break;
        case ALG_OPT_TOO_MANY_VOI_GOALS:
            tpsErrorCode = TPS_ER_ALG_OPT_TOO_MANY_VOI_GOALS;
            break;
        case ALG_OAR_WITH_LOWER_GOAL:
            tpsErrorCode = TPS_ER_ALG_OAR_WITH_LOWER_GOAL;
            break;
        case ALG_OPT_UNREASONABLE_GOALS:
            tpsErrorCode = TPS_ER_ALG_OPT_UNREASONABLE_GOALS;
            break;
        case ALG_OPT_GOALS_NOT_IN_VOILIST:
            tpsErrorCode = TPS_ER_ALG_OPT_GOALS_NOT_IN_VOILIST;
            break;
        case ALG_OPT_NO_LOWERBOUND_GOALS:
            tpsErrorCode = TPS_ER_ALG_OPT_NO_LOWERBOUND_GOALS;
            break;
        case ALG_OPT_TOO_MANY_BEAMS:
            tpsErrorCode = TPS_ER_ALG_OPT_TOO_MANY_BEAMS;
            break;
        case ALG_OPT_TOO_MANY_BG_NG:
            tpsErrorCode = TPS_ER_ALG_OPT_TOO_MANY_BG_NG;
            break;
        case ALG_OPT_INIT_GOAL_FAILED:
            tpsErrorCode = TPS_ER_ALG_OPT_INIT_GOAL_FAILED;
            break;
        case ALG_OPT_NO_TARGET:
            tpsErrorCode = TPS_ER_ALG_OPT_NO_TARGET;
            break;
        case ALG_OPT_CONFLICTING_GOALS:
            tpsErrorCode = TPS_ER_ALG_OPT_CONFLICTING_GOALS;
            break;
        case ALG_OPT_INIT_VOXEL_FAILED:
            tpsErrorCode = TPS_ER_ALG_OPT_INIT_VOXEL_FAILED;
            break;
        case ALG_OPT_NO_OPT_BEAM:
            tpsErrorCode = TPS_ER_ALG_OPT_NO_OPT_BEAM;
            break;
        case ALG_OPT_INIT_OUTPUT:
            tpsErrorCode = TPS_ER_ALG_OPT_INIT_OUTPUT;
            break;
        case ALG_OPT_LS_FAILED:
            tpsErrorCode = TPS_ER_ALG_OPT_LS_FAILED;
            break;
        case ALG_INVALID_BEAM_TYPE:
            tpsErrorCode = TPS_ER_ALG_INVALID_BEAM_TYPE;
            break;
        case ALG_TOO_SMAll_MU_FOR_GIVEN_ARC_LENGTH:
            tpsErrorCode = TPS_ER_ALG_TOO_SMALL_MU_FOR_GIVEN_ARC_LENGTH;
            break;
        case ALG_TOO_LARGE_MU_FOR_GIVEN_ARC_LENGTH:
            tpsErrorCode = TPS_ER_ALG_TOO_LARGE_MU_FOR_GIVEN_ARC_LENGTH;
            break;
        case ALG_INVALID_CP_COUNT_AT_ONE_ANGLE:
            tpsErrorCode = TPS_ER_ALG_INVALID_CP_COUNT_AT_ONE_ANGLE;
            break;
        case ALG_OUT_OF_MLC_COVERAGE:
            tpsErrorCode = TPS_ER_ALG_OUT_OF_MLC_COVERAGE;
            break;
        case ALG_OPT_WRONG_ITERATION:
            tpsErrorCode = TPS_ER_ALG_OPT_WRONG_ITERATION;
            break;
        case ALG_OPT_DMPO_WRONG_ITER_TIME:
            tpsErrorCode = TPS_ER_ALG_OPT_DMPO_WRONG_ITER_TIME;
            break;
        case ALG_OPT_DMPO_NO_SEG_IN_BEAM:
            tpsErrorCode = TPS_ER_ALG_OPT_DMPO_NO_SEG_IN_BEAM;
            break;
        case ALG_OPT_CONFIG_FILE_WRONG_FOR_BURST:
            tpsErrorCode = TPS_ER_ALG_OPT_CONFIG_FILE_WRONG_FOR_BURST;
            break;
        case ALG_OPT_SS_WRONG_SEG_NUMBER:
            tpsErrorCode = TPS_ER_ALG_OPT_SS_WRONG_SEG_NUMBER;
            break;
        case ALG_OPT_NO_ISO_IN_VMAT_BEAM:
            tpsErrorCode = TPS_ER_ALG_OPT_NO_ISO_IN_VMAT_BEAM;
            break;
        case ALG_CALC_EUD_FAILED:
            tpsErrorCode = TPS_ER_ALG_CALC_EUD_FAILED;
            break;
        case ALG_INVALID_EUD_INPUT:
            tpsErrorCode = TPS_ER_ALG_INVALID_EUD_INPUT;
            break;
        case ALG_WRONG_EUD_PTV_A_VALUE:
            tpsErrorCode = TPS_ER_ALG_WRONG_EUD_PTV_A_VALUE;
            break;
        case ALG_WRONG_EUD_OAR_A_VALUE:
            tpsErrorCode = TPS_ER_ALG_WRONG_EUD_OAR_A_VALUE;
            break;
        case ALG_TOO_MANY_BACK_NG:
            tpsErrorCode = TPS_ER_ALG_TOO_MANY_BACK_NG;
            break;
        case ALG_TOO_SMALL_DOSE_RATE:
            tpsErrorCode = TPS_ER_ALG_TOO_SMALL_DOSE_RATE;
            break;
        case ALG_TOO_LARGE_DOSE_RATE:
            tpsErrorCode = TPS_ER_ALG_TOO_LARGE_DOSE_RATE;
            break;
        case ALG_WRONG_CONFORMAL_ARC_CAN_NOT_DELIVERY:
            tpsErrorCode = TPS_ER_ALG_WRONG_CONFORMAL_ARC_CAN_NOT_DELIVERY;
            break;
        case ALG_TOO_SMALL_MLC_ACC:
            tpsErrorCode = TPS_ER_ALG_TOO_SMALL_MLC_ACC;
            break;
        case ALG_INIT_MACHINEPARAM_FAILED:
            tpsErrorCode = TPS_ER_ALG_INIT_MACHINEPARAM_FAILED;
            break;
        case ALG_DIFF_BEAMCOMM_FOR_VMAT_BURST:
            tpsErrorCode = TPS_ER_ALG_DIFF_BEAMCOMM_FOR_VMAT_BURST;
            break;
        case ALG_DIFF_DOSERATE_FOR_VMAT:
            tpsErrorCode = TPS_ER_ALG_DIFF_DOSERATE_FOR_VMAT;
            break;
        // Dose error
        case ALG_CALCULATION_FAILED:
            tpsErrorCode = TPS_ER_ALG_CALCULATION_FAILED;
            break;
        case ALG_BEAMMAP_SIZE_TOOSMALL:
            tpsErrorCode = TPS_ER_ALG_BEAMMAP_SIZE_TOOSMALL;
            break;
        case ALG_SEGMENT_NOT_VALID:
            tpsErrorCode = TPS_ER_ALG_SEGMENT_NOT_VALID;
            break;
        case ALG_NO_DB_ACCESS:
            tpsErrorCode = TPS_ER_ALG_NO_DB_ACCESS;
            break;
        case ALG_RENORM_BEAMDOSE_NOT_VALID:
            tpsErrorCode = TPS_ER_ALG_RENORM_BEAMDOSE_NOT_VALID;
            break;
        case ALG_DOSE_CLEAR_DATA_FAILED:
            tpsErrorCode = TPS_ER_ALG_DOSE_CLEAR_DATA_FAILED;
            break;
        case ALG_INIT_ALG_REQUEST_FAILED:
            tpsErrorCode = TPS_ER_ALG_INIT_ALG_REQUEST_FAILED;
            break;
        case ALG_COMPUTE_DOSE_FAILED:
            tpsErrorCode = TPS_ER_ALG_COMPUTE_DOSE_FAILED;
            break;
        case ALG_DOSE_EXCEED_DATALIMIT:
            tpsErrorCode = TPS_ER_ALG_DOSE_EXCEED_DATALIMIT;
            break;
        case ALG_WRONG_SKIN:
            tpsErrorCode = TPS_ER_ALG_WRONG_SKIN;
            break;
        case ALG_WRONG_VOISUBVOLUME:
            tpsErrorCode = TPS_ER_ALG_WRONG_VOISUBVOLUME;
            break;
        case ALG_WRONG_CT2DENSITY:
            tpsErrorCode = TPS_ER_ALG_WRONG_CT2DENSITY;
            break;
        case ALG_WRONG_ALGORITHMTYPE:
            tpsErrorCode = TPS_ER_ALG_WRONG_ALGORITHMTYPE;
            break;
        case ALG_NO_CT:
            tpsErrorCode = TPS_ER_ALG_NO_CT;
            break;
        case ALG_WRONG_CT:
            tpsErrorCode = TPS_ER_ALG_WRONG_CT;
            break;
        case ALG_NO_NORMGROUP:
            tpsErrorCode = TPS_ER_ALG_NO_NORMGROUP;
            break;
        case ALG_WRONG_NORMGROUP:
            tpsErrorCode = TPS_ER_ALG_WRONG_NORMGROUP;
            break;
        case ALG_WRONG_BEAM:
            tpsErrorCode = TPS_ER_ALG_WRONG_BEAM;
            break;
        case ALG_WRONG_MC_DONOT_SUPP_PHYSICALWEDGE:
            tpsErrorCode = TPS_ER_ALG_WRONG_MC_DONOT_SUPP_PHYSICALWEDGE;
            break;
        case ALG_DOSE_INIT_REQ_LIST_FAILED:
            tpsErrorCode = TPS_ER_ALG_DOSE_INIT_REQ_LIST_FAILED;
            break;
        case ALG_COMMISSIONDATA_NOT_VALID:
            tpsErrorCode = TPS_ER_ALG_COMMISSIONDATA_NOT_VALID;
            break;
        case ALG_INVALID_ACCESSORY:
            tpsErrorCode = TPS_ER_ALG_INVALID_ACCESSORY;
            break;
        case ALG_TOTAL_MU_ZERO:
            tpsErrorCode = TPS_ER_ALG_TOTAL_MU_ZERO;
            break;
        case ALG_NORM_VOI_ZERO_DOSE:
            tpsErrorCode = TPS_ER_ALG_NORM_VOI_ZERO_DOSE;
            break;
        case ALG_NORM_POINT_ZERO_DOSE:
            tpsErrorCode = TPS_ER_ALG_NORM_POINT_ZERO_DOSE;
            break;
        case ALG_CALC_NTCP_FAILED:
            tpsErrorCode = TPS_ER_ALG_CALC_NTCP_FAILED;
            break;
        case ALG_COMPUTE_DVH_FAILED:
            tpsErrorCode = TPS_ER_ALG_COMPUTE_DVH_FAILED;
            break;
        case ALG_EZDOSECALC_FAILED:
            tpsErrorCode = TPS_ER_ALG_EZDOSECALC_FAILED;
            break;
        case ALG_COMPOSE_DOSEGRIDS_FAILED:
            tpsErrorCode = TPS_ER_ALG_COMPOSE_DOSEGRIDS_FAILED;
            break;
        case ALG_WRONG_LIST_CNT:
            tpsErrorCode = TPS_ER_ALG_WRONG_LIST_CNT;
            break;
        case ALG_WRONG_HET_TRACE:
            tpsErrorCode = TPS_ER_ALG_WRONG_HET_TRACE;
            break;
        case ALG_WRONG_CALCULATEHETERO:
            tpsErrorCode = TPS_ER_ALG_WRONG_CALCULATEHETERO;
            break;
        case ALG_WRONG_CONVERTCT2HETERO:
            tpsErrorCode = TPS_ER_ALG_WRONG_CONVERTCT2HETERO;
            break;
        case ALG_WRONG_INITLOGTARTABLE:
            tpsErrorCode = TPS_ER_ALG_WRONG_INITLOGTARTABLE;
            break;
        // COMMISSION ERROR
        case ALG_COMMISSION_ERROR:
            tpsErrorCode = TPS_ER_ALG_COMMISSION_ERROR;
            break;
        case ALG_COMMISSION_ERROR_MEASUEDPROFILE:
            tpsErrorCode = TPS_ER_ALG_COMMISSION_ERROR_MEASUEDPROFILE;
            break;
        case ALG_COMMISSION_ERROR_MEASUEDDATA:
            tpsErrorCode = TPS_ER_ALG_COMMISSION_ERROR_MEASUEDDATA;
            break;
        case ALG_COMMISSION_ERROR_WEDGEFACTOR:
            tpsErrorCode = TPS_ER_ALG_COMMISSION_ERROR_WEDGEFACTOR;
            break;
        case ALG_COMMISSION_ERROR_INCOMPLETE:
            tpsErrorCode = TPS_ER_ALG_COMMISSION_ERROR_INCOMPLETE;
            break;
        case ALG_COMMISSION_ERROR_ECSWITCH:
            tpsErrorCode = TPS_ER_ALG_COMMISSION_ERROR_ECSWITCH;
            break;
        case ALG_COMMISSION_ERROR_NOFIELD10_10:
            tpsErrorCode = TPS_ER_ALG_COMMISSION_ERROR_NOFIELD10_10;
            break;
        case ALG_COMMISSION_ERROR_LESSPROFILE:
            tpsErrorCode = TPS_ER_ALG_COMMISSION_ERROR_LESSPROFILE;
            break;
        case ALG_COMMISSION_ERROR_OFFSET:
            tpsErrorCode = TPS_ER_ALG_COMMISSION_ERROR_OFFSET;
            break;
        case ALG_WRONG_SEGMENT_COUNT:
            tpsErrorCode = TPS_ER_ALG_WRONG_SEGMENT_COUNT;
            break;
        case ALG_CHECK_SEGMENT_COUNT_FAILED:
            tpsErrorCode = TPS_ER_ALG_CHECK_SEGMENT_COUNT_FAILED;
            break;
        case ALG_WRONG_SEGMENT_MU:
            tpsErrorCode = TPS_ER_ALG_WRONG_SEGMENT_MU;
            break;
        case ALG_WRONG_SEGMENT_WEIGHT:
            tpsErrorCode = TPS_ER_ALG_WRONG_SEGMENT_WEIGHT;
            break;
        case ALG_WRONG_GANTRY_ANGLE:
            tpsErrorCode = TPS_ER_ALG_WRONG_GANTRY_ANGLE;
            break;
        case ALG_WRONG_CONNECTION_GANTRY_ANGLE:
            tpsErrorCode = TPS_ER_ALG_WRONG_CONNECTION_GANTRY_ANGLE;
            break;
        case ALG_WRONG_BURST_GANTRY_ANGLE:
            tpsErrorCode = TPS_ER_ALG_WRONG_BURST_GANTRY_ANGLE;
            break;
        case ALG_WRONG_GANTRY_ROTATION_DIRECTION:
            tpsErrorCode = TPS_ER_ALG_WRONG_GANTRY_ROTATION_DIRECTION;
            break;
        case ALG_WRONG_GANTRY_ANGLE_DIFFERENCE:
            tpsErrorCode = TPS_ER_ALG_WRONG_GANTRY_ANGLE_DIFFERENCE;
            break;
        case ALG_WRONG_COLLIMATOR_ANGLE:
            tpsErrorCode = TPS_ER_ALG_WRONG_COLLIMATOR_ANGLE;
            break;
        case ALG_WRONG_TABLE_ANGLE:
            tpsErrorCode = TPS_ER_ALG_WRONG_TABLE_ANGLE;
            break;
        case ALG_WRONG_JAW_POSITION:
            tpsErrorCode = TPS_ER_ALG_WRONG_JAW_POSITION;
            break;
        case ALG_WRONG_LEAF_SHAPE:
            tpsErrorCode = TPS_ER_ALG_WRONG_LEAF_SHAPE;
            break;
        case ALG_WRONG_CONNECTION_SHAPE:
            tpsErrorCode = TPS_ER_ALG_WRONG_CONNECTION_SHAPE;
            break;
        default:
            tpsErrorCode = TPS_ER_FAILURE;
            break;
    }

    return tpsErrorCode;
}




TPS_END_NAMESPACE