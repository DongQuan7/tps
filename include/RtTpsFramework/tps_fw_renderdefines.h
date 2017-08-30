////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_renderdefines.h
/// 
///  \brief class RenderDefines declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/02
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_RENDERDEFINES_H_
#define TPS_FW_RENDERDEFINES_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

enum DATA_SOURCE_GROUP_ID
{
    SOURCE_GROUP_COMMON                     = 0x01,
    SOURCE_GROUP_FUSION_FIXED               = 0x02,
    SOURCE_GROUP_FUSION_FLOAT               = 0x03,
    SOURCE_GROUP_FUSION_MIXED               = 0x04,
    SOURCE_GROUP_EV_1                       = 0x05,
    SOURCE_GROUP_EV_2                       = 0x06,
    SOURCE_GROUP_EV_RESULT                  = 0x07,
    SOURCE_GROUP_PLANQA                     = 0x08,
    SOURCE_GROUP_ADMIN                      = 0x09,
    SOURCE_GROUP_EZPLAN                     = 0x0a,
    SOURCE_GROUP_GALLERY                    = 0x0b,
};

enum WINDOW_TYPE
{
    //COMMON--------------------------------------------
    WINDOW_COMMON_MPR_CORONAL               = 0x0100,
    WINDOW_COMMON_MPR_SAGITTAL              = 0x0101,
    WINDOW_COMMON_MPR_AXIAL                 = 0x0102,
    WINDOW_COMMON_MPR_AXIAL_PREV            = 0x0103,
    WINDOW_COMMON_MPR_AXIAL_POST            = 0x0104,
    WINDOW_COMMON_BEV                       = 0x0105,
    WINDOW_COMMON_SHADED_SURFACE            = 0x0106,
    WINDOW_COMMON_VOI_PREVIEW_MPR_CORONAL   = 0x0107,
    WINDOW_COMMON_VOI_PREVIEW_MPR_SAGITTAL  = 0x0108,
    WINDOW_COMMON_VOI_PREVIEW_MPR_AXIAL     = 0x0109,
    WINDOW_COMMON_SIMULATOR_MLC             = 0x010a,
    WINDOW_COMMON_BEAM_PREVIEW_MPR_AXIAL    = 0x010b,
    //FUSION FIXED--------------------------------------
    WINDOW_FUSION_FIXED_MPR_CORONAL         = 0x0200,
    WINDOW_FUSION_FIXED_MPR_SAGITTAL        = 0x0201,
    WINDOW_FUSION_FIXED_MPR_AXIAL           = 0x0202,
    //FUSION FLOAT--------------------------------------
    WINDOW_FUSION_FLOAT_MPR_CORONAL         = 0x0300,
    WINDOW_FUSION_FLOAT_MPR_SAGITTAL        = 0x0301,
    WINDOW_FUSION_FLOAT_MPR_AXIAL           = 0x0302,
    //FUSION MIXED--------------------------------------
    WINDOW_FUSION_MIXED_MPR_CORONAL         = 0x0400,
    WINDOW_FUSION_MIXED_MPR_SAGITTAL        = 0x0401,
    WINDOW_FUSION_MIXED_MPR_AXIAL           = 0x0402,
    WINDOW_FUSION_MIXED_MPR_AXIAL_POST      = 0x0403,
    WINDOW_FUSION_MIXED_MPR_AXIAL_PREV      = 0x0404,
    //EV 1----------------------------------------------
    WINDOW_EV_1_MPR_CORONAL                 = 0x0500,
    WINDOW_EV_1_MPR_SAGITTAL                = 0x0501,
    WINDOW_EV_1_MPR_AXIAL                   = 0x0502,
    //EV 2----------------------------------------------
    WINDOW_EV_2_MPR_CORONAL                 = 0x0600,
    WINDOW_EV_2_MPR_SAGITTAL                = 0x0601,
    WINDOW_EV_2_MPR_AXIAL                   = 0x0602,
    //EV RESULT-----------------------------------------
    WINDOW_EV_RESULT_MPR_CORONAL            = 0x0700,
    WINDOW_EV_RESULT_MPR_SAGITTAL           = 0x0701,
    WINDOW_EV_RESULT_MPR_AXIAL              = 0x0702,
    //PLAN QA-------------------------------------------
    WINDOW_PLANQA_MPR_CORONAL               = 0x0800,
    WINDOW_PLANQA_MPR_SAGITTAL              = 0x0801,
    WINDOW_PLANQA_MPR_AXIAL                 = 0x0802,
    WINDOW_PLANQA_SHADED_SURFACE            = 0x0803,
    //ADMIN---------------------------------------------
    WINDOW_ADMIN_MPR_CORONAL                = 0x0900,
    WINDOW_ADMIN_MPR_SAGITTAL               = 0x0901,
    WINDOW_ADMIN_MPR_AXIAL                  = 0x0902,
    WINDOW_ADMIN_3D                         = 0x0903,
    //EZ PLAN-------------------------------------------
    WINDOW_EZPLAN_IMAGE                     = 0x0a00,
    //GALLERY-------------------------------------------
    WINDOW_GALLERY_MPR_CORONAL              = 0x0b00,
    WINDOW_GALLERY_MPR_SAGITTAL             = 0x0b01,
    WINDOW_GALLERY_MPR_AXIAL                = 0x0b02,
    WINDOW_GALLERY_3D                       = 0x0b03,
    //UT------------------------------------------------
    WINDOW_UT_MPR_CORONAL                   = 0x0000,
    WINDOW_UT_MPR_SAGITTAL                  = 0x0001,
    WINDOW_UT_MPR_AXIAL                     = 0x0002,
};

enum DATA_SOURCE_ID
{
    IMAGE = 0,
    PLAN,
    //FIXED_IMAGE, //use IMAGE instead
    FLOAT_IMAGE,
};

enum GPU_OVERLAY_EXT
{
    OVERLAY_VOI = 100,
    OVERLAY_GRAPHIC,
    OVERLAY_COVER,
    OVERLAY_DUMMY,
};

typedef std::string DATA_SOURCE_UID;
typedef int         DISPLAY_CACHE_UID;

struct DISPLAY_SIZE
{
    int width;
    int height;
};

//go type defines
typedef unsigned int GO_TYPE;
typedef unsigned short MAIN_COMPONENT;
typedef unsigned char FIRST_POSTFIX_COMPONENT;
typedef unsigned char SECOND_POSTFIX_COMPONENT;
#define EMPTY_COMPONENT 0
#define HIT_TOLERANCE 5 //pixel size

#define WINDOW_WIDTH_MIN 1
#define WINDOW_WIDTH_MAX_CT 9727
#define WINDOW_CENTER_MIN_CT -1536
#define WINDOW_CENTER_MAX_CT 8191
#define WINDOW_WIDTH_MAX_MR 4096
#define WINDOW_CENTER_MIN_MR 0
#define WINDOW_CENTER_MAX_MR 4095
#define WINDOW_WIDTH_MIN_PT -2147483647
#define WINDOW_WIDTH_MAX_PT 2147483647
#define WINDOW_CENTER_MIN_PT -2147483647
#define WINDOW_CENTER_MAX_PT 2147483647

TPS_END_NAMESPACE  // end namespace tps

#endif
