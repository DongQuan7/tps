//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      tps_error.h
///  \brief     tps error code
///
///  \version 1.0
///  \date    Oct. 21,2016
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_ERROR_H_
#define TPS_ERROR_H_

namespace tps
{
    //tps_error.h ,tps_error_name.h 和 TpsErrorCodes.cs须保持一致
    //tps define function error code
    enum TPS_ERROR_CODES
    {
        //common error range [0, 99]
        TPS_ER_SUCCESS = 0                                  ,
        TPS_ER_FAILURE                                      ,
        TPS_ER_INPUT_INVALID                                ,
        TPS_ER_ALG_OPTIMIZER                                ,
        TPS_ER_PATIENT_NOT_EXIST                            ,
        TPS_ER_NO_SERIES_IN_PATIENT                         ,
        TPS_ER_RELATED_MACHINE_ERROR                        ,
        TPS_ER_CALC_NO_SKIN                                 ,
        TPS_ER_CANCEL                                       ,
        TPS_ER_UNSUPPORTED                                  ,
        TPS_ER_NODATASOURCE,
        TPS_ER_NOT_MPRWINDOW,
        TPS_ER_NULL_MPRGO,
        TPS_ER_NULL_VOLUME,

        //contouring error range [1000, 1199]
        TPS_ER_CONTOURING = 1000                            ,

        //planning error range [1100, 1199]
        TPS_ER_PLANNING = 1100                              ,
        TPS_ER_PLANNING_ADDPOI_OUTOFIMAGE,

        //evaluation error range [1200, 1299]
        TPS_ER_EVALUATION = 1200                            ,

        //QA error range [1300, 1399]
        TPS_ER_QA = 1300                                    ,

        //admin error range [1400, 1499]
        TPS_ER_ADMIN = 1400                                 ,

        //planning error range [1500, 1599]
        TPS_ER_EASY_PLAN = 1500                             ,

        //gallery error range [1600, 1699]
        TPS_ER_GALLERY = 1600                               ,

        //machine setting error range [1700, 1799]
        TPS_ER_MACHINE = 1700                               ,
        TPS_ER_MACHINE_SSDNOTMATCH,

        //plan report error range [1800, 1899]
        TPS_ER_PLANREPORT = 1800                            ,

        //database wrapper error range [2000, 2999]
        TPS_ER_DB_ERROR = 2000                              ,
        TPS_ER_DB_CONNECTION_FAILED                         ,
        TPS_ER_DB_VERSION_NOT_MATCHED                       ,
        TPS_ER_DB_MEMORY_ALLOCATION_FAILURE                 ,
        TPS_ER_DB_UID_NOT_EXIST                             ,
        TPS_ER_DB_UID_NOT_UNIQUE                            ,
        TPS_ER_DB_FK_UID_NOT_EXIST                          ,
        TPS_ER_DB_INPUT_VIOLATION                           ,
        TPS_ER_DB_EXCEPTION                                 ,
        TPS_ER_DB_MODALITY_NOT_SUPPORTED                    ,
        TPS_ER_DB_UNKNOWN,

        //ALG error range [3000, 4999]
        //OPT ERROR
        TPS_ER_ALG_OPT_ERROR = 3000                         ,
        TPS_ER_ALG_OPT_START                                ,
        TPS_ER_ALG_OPT_ABORT                                ,
        TPS_ER_ALG_OPT_SUSPEND                              ,
        TPS_ER_ALG_OPT_RESUME                               ,
        TPS_ER_ALG_OPT_START_FAILED                         ,
        TPS_ER_ALG_OPT_INIT_FAILED                          ,
        TPS_ER_ALG_OPT_SUSPEND_FAILED                       ,
        TPS_ER_ALG_OPT_RESUME_FAILED                        ,
        TPS_ER_ALG_OPT_ABORT_FAILED                         ,
        TPS_ER_ALG_INPUT_NOT_VALID                          ,
        TPS_ER_ALG_EMPTY_GRID                               ,
        TPS_ER_ALG_OPT_IMPROPER_GRIDSIZE                    ,
        TPS_ER_ALG_OPT_WRONG_P2G_MATRIX                     ,
        TPS_ER_ALG_DBPARAM_FAILED                           ,
        TPS_ER_ALG_LOAD_MACHINE_FAILED                      ,
        TPS_ER_ALG_OPT_WRONG_MLC_SETTING                    ,
        TPS_ER_ALG_OPT_WRONG_BEAM                           ,
        TPS_ER_ALG_WRONG_BEAM_720                           ,
        TPS_ER_ALG_NO_BEAM                                  ,
        TPS_ER_ALG_OPT_NO_COMMISSIONUNIT                    ,
        TPS_ER_ALG_WRONG_BEAM_GRID                          ,
        TPS_ER_ALG_OPT_NO_SEG_IN_BEAM                       ,
        TPS_ER_ALG_OPT_WRONG_SEG_IN_BEAM                    ,
        TPS_ER_ALG_OPT_NO_SHAPE_IN_SEG                      ,
        TPS_ER_ALG_OPT_NO_LEAF_IN_SHAPE                     ,
        TPS_ER_ALG_OPT_WRONG_ANGLE_IN_BEAM                  ,
        TPS_ER_ALG_OPT_WRONG_SHAPE_IN_BEAM                  ,
        TPS_ER_ALG_NO_ISO_POINT                             ,
        TPS_ER_ALG_OPT_WRONG_SEG_T_MATRIX                   ,
        TPS_ER_ALG_OPT_END_CONDITION_WRONG                  ,
        TPS_ER_ALG_OPT_WRONG_DVH_BIN_SIZE                   ,
        TPS_ER_ALG_NO_VOI                                   ,
        TPS_ER_ALG_OPT_WRONG_VOI                            ,
        TPS_ER_ALG_OPT_WRONG_VOI_SUBVOLUME                  ,
        TPS_ER_ALG_OPT_WRONG_VOI_CONTOUR                    ,
        TPS_ER_ALG_OPT_NO_SKIN                              ,
        TPS_ER_ALG_OPT_NO_GOAL                              ,
        TPS_ER_ALG_OPT_TOO_MANY_TOTAL_GOALS                 ,
        TPS_ER_ALG_OPT_TOO_MANY_VOI_GOALS                   ,
        TPS_ER_ALG_OAR_WITH_LOWER_GOAL                      ,
        TPS_ER_ALG_OPT_UNREASONABLE_GOALS                   ,
        TPS_ER_ALG_OPT_GOALS_NOT_IN_VOILIST                 ,
        TPS_ER_ALG_OPT_NO_LOWERBOUND_GOALS                  ,
        TPS_ER_ALG_OPT_TOO_MANY_BEAMS                       ,
        TPS_ER_ALG_OPT_TOO_MANY_BG_NG                       ,
        TPS_ER_ALG_OPT_INIT_GOAL_FAILED                     ,
        TPS_ER_ALG_OPT_NO_TARGET                            ,
        TPS_ER_ALG_OPT_CONFLICTING_GOALS                    ,
        TPS_ER_ALG_OPT_INIT_VOXEL_FAILED                    ,
        TPS_ER_ALG_OPT_NO_OPT_BEAM                          ,
        TPS_ER_ALG_OPT_INIT_OUTPUT                          ,
        TPS_ER_ALG_OPT_LS_FAILED                            ,
        TPS_ER_ALG_INVALID_BEAM_TYPE                        ,
        TPS_ER_ALG_TOO_SMALL_MU_FOR_GIVEN_ARC_LENGTH        ,
        TPS_ER_ALG_TOO_LARGE_MU_FOR_GIVEN_ARC_LENGTH        ,
        TPS_ER_ALG_INVALID_CP_COUNT_AT_ONE_ANGLE            ,
        TPS_ER_ALG_OUT_OF_MLC_COVERAGE                      ,
        TPS_ER_ALG_OPT_WRONG_ITERATION                      ,
        TPS_ER_ALG_OPT_DMPO_WRONG_ITER_TIME                 ,
        TPS_ER_ALG_OPT_DMPO_NO_SEG_IN_BEAM                  ,
        TPS_ER_ALG_OPT_CONFIG_FILE_WRONG_FOR_BURST          ,
        TPS_ER_ALG_OPT_SS_WRONG_SEG_NUMBER                  ,
        TPS_ER_ALG_OPT_NO_ISO_IN_VMAT_BEAM                  ,
        TPS_ER_ALG_CALC_EUD_FAILED                          ,
        TPS_ER_ALG_INVALID_EUD_INPUT                        ,
        TPS_ER_ALG_WRONG_EUD_PTV_A_VALUE                    ,
        TPS_ER_ALG_WRONG_EUD_OAR_A_VALUE                    ,
        TPS_ER_ALG_TOO_MANY_BACK_NG                         ,
        TPS_ER_ALG_TOO_SMALL_DOSE_RATE                      ,
        TPS_ER_ALG_TOO_LARGE_DOSE_RATE                      ,
        TPS_ER_ALG_WRONG_CONFORMAL_ARC_CAN_NOT_DELIVERY     ,
        TPS_ER_ALG_TOO_SMALL_MLC_ACC                        ,
        TPS_ER_ALG_INIT_MACHINEPARAM_FAILED                 ,
        TPS_ER_ALG_DIFF_BEAMCOMM_FOR_VMAT_BURST             ,
        TPS_ER_ALG_DIFF_DOSERATE_FOR_VMAT                   ,

        //DOSE ERROR
        TPS_ER_ALG_CALCULATION_FAILED = 3500                ,
        TPS_ER_ALG_BEAMMAP_SIZE_TOOSMALL                    ,
        TPS_ER_ALG_SEGMENT_NOT_VALID                        ,
        TPS_ER_ALG_NO_DB_ACCESS                             ,
        TPS_ER_ALG_RENORM_BEAMDOSE_NOT_VALID                ,
        TPS_ER_ALG_DOSE_CLEAR_DATA_FAILED                   ,
        TPS_ER_ALG_INIT_ALG_REQUEST_FAILED                  ,
        TPS_ER_ALG_COMPUTE_DOSE_FAILED                      ,
        TPS_ER_ALG_DOSE_EXCEED_DATALIMIT                    ,
        TPS_ER_ALG_WRONG_SKIN                               ,
        TPS_ER_ALG_WRONG_VOISUBVOLUME                       ,
        TPS_ER_ALG_WRONG_CT2DENSITY                         ,
        TPS_ER_ALG_WRONG_ALGORITHMTYPE                      ,
        TPS_ER_ALG_NO_CT                                    ,
        TPS_ER_ALG_WRONG_CT                                 ,
        TPS_ER_ALG_NO_NORMGROUP                             ,
        TPS_ER_ALG_WRONG_NORMGROUP                          ,
        TPS_ER_ALG_WRONG_BEAM                               ,
        TPS_ER_ALG_WRONG_MC_DONOT_SUPP_PHYSICALWEDGE        ,
        TPS_ER_ALG_DOSE_INIT_REQ_LIST_FAILED                ,
        TPS_ER_ALG_COMMISSIONDATA_NOT_VALID                 ,
        TPS_ER_ALG_INVALID_ACCESSORY                        ,
        TPS_ER_ALG_TOTAL_MU_ZERO                            ,
        TPS_ER_ALG_NORM_VOI_ZERO_DOSE                       ,
        TPS_ER_ALG_NORM_POINT_ZERO_DOSE                     ,
        TPS_ER_ALG_CALC_NTCP_FAILED                         ,
        TPS_ER_ALG_COMPUTE_DVH_FAILED                       ,
        TPS_ER_ALG_EZDOSECALC_FAILED                        ,
        TPS_ER_ALG_COMPOSE_DOSEGRIDS_FAILED                 ,
        TPS_ER_ALG_WRONG_LIST_CNT                           ,
        TPS_ER_ALG_WRONG_HET_TRACE                          ,
        TPS_ER_ALG_WRONG_CALCULATEHETERO                    ,
        TPS_ER_ALG_WRONG_CONVERTCT2HETERO                   ,
        TPS_ER_ALG_WRONG_INITLOGTARTABLE                    ,
       


        //ALG easy plan error range [4100, 4199]
        TPS_ER_ALG_EZPLAN_OK = 4100                         ,
        TPS_ER_ALG_EZPLAN_INVALIDINPUT_BEAM                 ,
        TPS_ER_ALG_EZPLAN_INVALIDINPUT_POINTLIST            ,
        TPS_ER_ALG_EZPLAN_INVALIDINPUT_SSD                  ,

        //ALG commission error range [4200, 4299]
        TPS_ER_ALG_COMMISSION_ERROR = 4200                  ,
        TPS_ER_ALG_COMMISSION_ERROR_MEASUEDPROFILE          ,
        TPS_ER_ALG_COMMISSION_ERROR_MEASUEDDATA             ,
        TPS_ER_ALG_COMMISSION_ERROR_WEDGEFACTOR             ,
        TPS_ER_ALG_COMMISSION_ERROR_INCOMPLETE              ,
        TPS_ER_ALG_COMMISSION_ERROR_ECSWITCH                ,
        TPS_ER_ALG_COMMISSION_ERROR_NOFIELD10_10            ,
        TPS_ER_ALG_COMMISSION_ERROR_LESSPROFILE             ,
        TPS_ER_ALG_COMMISSION_ERROR_OFFSET                  ,
        TPS_ER_ALG_WRONG_SEGMENT_COUNT                      ,
        TPS_ER_ALG_CHECK_SEGMENT_COUNT_FAILED               ,
        TPS_ER_ALG_WRONG_SEGMENT_MU                         ,
        TPS_ER_ALG_WRONG_SEGMENT_WEIGHT                     ,
        TPS_ER_ALG_WRONG_GANTRY_ANGLE                       ,
        TPS_ER_ALG_WRONG_CONNECTION_GANTRY_ANGLE            ,
        TPS_ER_ALG_WRONG_BURST_GANTRY_ANGLE                 ,
        TPS_ER_ALG_WRONG_GANTRY_ROTATION_DIRECTION          ,
        TPS_ER_ALG_WRONG_GANTRY_ANGLE_DIFFERENCE            ,
        TPS_ER_ALG_WRONG_COLLIMATOR_ANGLE                   ,
        TPS_ER_ALG_WRONG_TABLE_ANGLE                        ,
        TPS_ER_ALG_WRONG_JAW_POSITION                       ,
        TPS_ER_ALG_WRONG_LEAF_SHAPE                         ,
        TPS_ER_ALG_WRONG_CONNECTION_SHAPE                   ,





        TPS_ER_UNKNOWN_ERROR = 5000                         ,
    };
}
#endif //TPS_ERROR_H_
