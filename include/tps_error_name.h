//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      tps_error.h
///  \brief     tps error code to name
///
///  \version 1.0
///  \date    Oct. 21,2016
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_ERROR_NAME_H_
#define TPS_ERROR_NAME_H_

//////////////////////////////////////////////////////////////////////////
//common error range [0, 99]
{ "TPS_ER_SUCCESS"                              ,0      ,"Success"                                              }, 
{ "TPS_ER_FAILURE"                              ,1      ,"Failed"                                               },
{ "TPS_ER_INPUT_INVALID"                        ,2      ,"TPS_ER_INPUT_INVALID"                                 },
{ "TPS_ER_ALG_OPTIMIZER"                        ,3      ,"TPS_ER_ALG_OPTIMIZER "                                },
{ "TPS_ER_PATIENT_NOT_EXIST"                    ,4      ,"TPS_ER_PATIENT_NOT_EXIST "                            },
{ "TPS_ER_NO_SERIES_IN_PATIENT"                 ,5      ,"TPS_ER_NO_SERIES_IN_PATIENT "                         },
{ "TPS_ER_CALC_NO_SKIN"                         ,6      ,"TPS_ER_CALC_NO_SKIN "                                 },
{ "TPS_ER_CANCEL"                               ,7      ,"TPS_ER_CANCEL "                                       },
{ "TPS_ER_UNSUPPORTED"                          ,8      ,"TPS_ER_UNSUPPORTED "                                  },

//contouring error range [1000, 1199]
{ "TPS_ER_CONTOURING"                           ,1000   ,"TPS_ER_CONTOURING "                                   },

//planning error range [1100, 1199]
{ "TPS_ER_PLANNING"                             ,1100   ,"TPS_ER_PLANNING "                                     },

//evaluation error range [1200, 1299]
{ "TPS_ER_EVALUATION"                           ,1200   ,"TPS_ER_EVALUATION "                                   },

//QA error range [1300, 1399]
{ "TPS_ER_QA"                                   ,1300   ,"TPS_ER_QA "                                           },

//admin error range [1400, 1499]
{ "TPS_ER_ADMIN"                                ,1400   ,"TPS_ER_ADMIN "                                        },

//planning error range [1500, 1599]
{ "TPS_ER_EASY_PLAN"                            ,1500   ,"TPS_ER_EASY_PLAN "                                    },

//gallery error range [1600, 1699]
{ "TPS_ER_GALLERY"                              ,1600   ,"TPS_ER_GALLERY "                                      },

//machine setting error range [1700, 1799]
{ "TPS_ER_MACHINE"                              ,1700   ,"TPS_ER_MACHINE "                                      },

//plan report error range [1800, 1899]
{ "TPS_ER_PLANREPORT"                           ,1800   ,"TPS_ER_PLANREPORT "                                   },

//database wrapper error range [2000, 2999]
{ "TPS_ER_DB_ERROR"                             ,2000   ,"TPS_ER_DB_ERROR "                                     },
//{ "TPS_ER_DB_SUCCESS = 0 ,
{ "TPS_ER_DB_CONNECTION_FAILED"                 ,2001   ,"TPS_ER_DB_CONNECTION_FAILED "                         },
{ "TPS_ER_DB_VERSION_NOT_MATCHED"               ,2002   ,"TPS_ER_DB_VERSION_NOT_MATCHED "                       },
{ "TPS_ER_DB_MEMORY_ALLOCATION_FAILURE"         ,2003   ,"TPS_ER_DB_MEMORY_ALLOCATION_FAILURE"                  },
{ "TPS_ER_DB_UID_NOT_EXIST"                     ,2004   ,"TPS_ER_DB_UID_NOT_EXIST "                             },
{ "TPS_ER_DB_UID_NOT_UNIQUE"                    ,2005   ,"TPS_ER_DB_UID_NOT_UNIQUE "                            },
{ "TPS_ER_DB_FK_UID_NOT_EXIST"                  ,2006   ,"TPS_ER_DB_FK_UID_NOT_EXIST "                          },
{ "TPS_ER_DB_INPUT_VIOLATION"                   ,2007   ,"TPS_ER_DB_INPUT_VIOLATION "                           },
{ "TPS_ER_DB_EXCEPTION"                         ,2008   ,"TPS_ER_DB_EXCEPTION "                                 },
{ "TPS_ER_DB_MODALITY_NOT_SUPPORTED"            ,2009   ,"TPS_ER_DB_MODALITY_NOT_SUPPORTED "                    },
{ "TPS_ER_DB_UNKNOWN"                           ,2010   ,"TPS_ER_DB_UNKNOWN"                                    },

//ALG error range [3000, 3999]
{ "TPS_ER_ALG_OK"                               ,3000   ,"ALG OK!"                                              },
{ "TPS_ER_ALG_ERROR"                            ,3001   ,"TPS_ER_ALG_ERROR "                                    },
{ "TPS_ER_ALG_INPUT_NOT_VALID"                  ,3002   ,"TPS_ER_ALG_INPUT_NOT_VALID "                          },
{ "TPS_ER_ALG_CALCULATION_FAILED"               ,3003   ,"TPS_ER_ALG_CALCULATION_FAILED "                       },
{ "TPS_ER_ALG_COMMISSIONDATA_NOT_VALID"         ,3004   ,"TPS_ER_ALG_COMMISSIONDATA_NOT_VALID "                 },
{ "TPS_ER_ALG_BEAMMAP_SIZE_TOOSMALL"            ,3005   ,"TPS_ER_ALG_BEAMMAP_SIZE_TOOSMALL "                    },
{ "TPS_ER_ALG_SEGMENT_NOT_VALID"                ,3006   ,"TPS_ER_ALG_SEGMENT_NOT_VALID "                        },
{ "TPS_ER_ALG_INVALID_ACCESSORY"                ,3007   ,"TPS_ER_ALG_INVALID_ACCESSORY "                        },
{ "TPS_ER_ALG_NORM_POINT_ZERO_DOSE"             ,3008   ,"TPS_ER_ALG_NORM_POINT_ZERO_DOSE "                     },
{ "TPS_ER_ALG_NO_DB_ACCESS"                     ,3009   ,"TPS_ER_ALG_NO_DB_ACCESS "                             },
{ "TPS_ER_ALG_WRONG_SKIN"                       ,3010   ,"TPS_ER_ALG_WRONG_SKIN "                               },
{ "TPS_ER_ALG_WRONG_VOISUBVOLUME"               ,3011   ,"TPS_ER_ALG_WRONG_VOISUBVOLUME "                       },
{ "TPS_ER_ALG_WRONG_CT"                         ,3012   ,"TPS_ER_ALG_WRONG_CT "                                 },
{ "TPS_ER_ALG_WRONG_NORMGROUP"                  ,3013   ,"TPS_ER_ALG_WRONG_NORMGROUP "                          },
{ "TPS_ER_ALG_WRONG_CT2DENSITY"                 ,3014   ,"TPS_ER_ALG_WRONG_CT2DENSITY "                         },
{ "TPS_ER_ALG_WRONG_BEAM"                       ,3015   ,"TPS_ER_ALG_WRONG_BEAM "                               },
{ "TPS_ER_ALG_WRONG_ALGORITHMTYPE"              ,3016   ,"TPS_ER_ALG_WRONG_ALGORITHMTYPE "                      },
{ "TPS_ER_ALG_EXCEED_DATALIMIT"                 ,3017   ,"TPS_ER_ALG_EXCEED_DATALIMIT "                         },
{ "TPS_ER_ALG_RENORM_BEAMDOSE_NOT_VALID"        ,3018   ,"TPS_ER_ALG_RENORM_BEAMDOSE_NOT_VALID "                },

//ALG easy plan error range [3100, 3199]
{ "TPS_ER_ALG_EZPLAN_OK"                        ,3100   ,"TPS_ER_ALG_EZPLAN_OK "                                },
{ "TPS_ER_ALG_EZPLAN_INVALIDINPUT_BEAM"         ,3101   ,"TPS_ER_ALG_EZPLAN_INVALIDINPUT_BEAM "                 },
{ "TPS_ER_ALG_EZPLAN_INVALIDINPUT_POINTLIST"    ,3102   ,"TPS_ER_ALG_EZPLAN_INVALIDINPUT_POINTLIST"             },
{ "TPS_ER_ALG_EZPLAN_INVALIDINPUT_SSD"          ,3103   ,"TPS_ER_ALG_EZPLAN_INVALIDINPUT_SSD "                  },

//ALG commission error range [3200, 3299]
{ "TPS_ER_ALG_COMMISSION_ERROR"                 ,3200   ,"TPS_ER_ALG_COMMISSION_ERROR "                         },
{ "TPS_ER_ALG_COMMISSION_ERROR_INCOMPLETE"      ,3201   ,"TPS_ER_ALG_COMMISSION_ERROR_INCOMPLETE"               },
{ "TPS_ER_ALG_COMMISSION_ERROR_OFFSET"          ,3202   ,"TPS_ER_ALG_COMMISSION_ERROR_OFFSET "                  },

{ "TPS_ER_UNKNOWN_ERROR"                        ,4000   ,"TPS_ER_UNKNOWN_ERROR "                                },

#endif //TPS_ERROR_H_
