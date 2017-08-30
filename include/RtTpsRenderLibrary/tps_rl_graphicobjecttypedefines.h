////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_graphicobjecttypedefines.h
/// 
///  \brief graphic object type defines 
/// 
///  \version 1.0
/// 
///  \date    2013/12/28
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GRAPHICOBJECTTYPEDEFINES_H_
#define TPS_RL_GRAPHICOBJECTTYPEDEFINES_H_

TPS_BEGIN_NAMESPACE

static const std::string COMMON_BASE = "COMMON";

//Note：DO NOT modify the existing value when adding new ENUM members
//unsigned short, 1 - 65535, 0 is empty
typedef enum TpsMainComponent 
{
    DUMMY_                                                  = 0x0001, 
    VOLUME                                                  = 0x0002,
    MASK_                                                   = 0x0003,
    MPR                                                     = 0x0004,
    SHADEDSURFCE                                            = 0x0005,
    MANUL_REMOVE_TABLE                                      = 0x0006,
    SURFACE_CAMERA                                          = 0x0007,
    CROSSHAIR                                               = 0x0008,
    DOSE                                                    = 0x0009,
    DOSE_GRID_BOUNDARY                                      = 0x000a,
    LIMIT_REGION                                            = 0x000b,
    VR                                                      = 0x000c,
    //BEV part------------------------------------------------------
    BEV_                                                    = 0x000d,
    DRR_                                                    = 0x000e,
    FLUENCEMAP                                              = 0x000f,
    BEV_DISPLAY                                             = 0x0010,
    BEV_DISPLAY_CONTROL                                     = 0x0011,
    BEV_CAMERA                                              = 0x0012,
    CT2DENSITY                                              = 0x0013,
    //fusion part---------------------------------------------------
    FUSION                                                  = 0x0014,
    FUSION_MPR                                              = 0x0015,
    LANDMARK_                                               = 0x0016,
    FUSION_REGION_FIXED                                     = 0x0017,
    FUSION_REGION_FLOAT                                     = 0x0018,
    FUSION_CHESS                                            = 0x0019,
    //voi part------------------------------------------------------
    VOI                                                     = 0x001a,
    VOI_COLLECTION                                          = 0x001b,
    VOI_RENDERINGCACHE                                      = 0x001d,
    VOI_RENDERINGCACHE_COLLECTION                           = 0x001e,
    ROI_CONTOUR_SET                                         = 0x001f,
    ROI_CONTOUR_SET_COLLECTION                              = 0x0020,

    NUDGE_CIRCLE                                            = 0x0021,
    //poi part------------------------------------------------------
    POI                                                     = 0x0022,
    POI_COLLECTION                                          = 0x0023,
    SETUP_POI                                               = 0x0024,
    //plan & goal---------------------------------------------------
    BEAM                                                    = 0x0025,
    BEAM_COLLECTION                                         = 0x0026,
    NORMALGROUP_COLLECTION                                  = 0x0027,
    WEDGE                                                   = 0x0028,
    //image text part-----------------------------------------------
    //BEV_IMAGE_TEXT                                          = 0x0029,
    MPR_CORNER_TEXT                                         = 0x002a,
    //BEV_RULER_TEXT                                         =0x002b,
    MPR_RULER_TEXT                                          = 0x002c,
    DOSE_LINE_SETTING                                       = 0x002d,
    APPLICATOR                                              = 0x0031,
    BLOCK                                                   = 0x0032,
    BEV_EDIT_MLC_BLOCK_CACHE                                = 0x0033,
    SERIES_TABLE_CACHE                                      = 0x0034,
    RT_IMAGE_COLLECTION                                     = 0X0035,

    // Gallery
    GALLERY_DUMMY                                           = 0x0100,//  =256
    GALLERY_VOLUME                                          = 0x0101,
    GALLERY_MASK                                            = 0x0102,
    GALLERY_MPR                                             = 0x0103,
    GALLERY_CROSSHAIR                                       = 0x0104,
    GALLERY_VR                                              = 0x0105,
    GALLERY_MPR_CORNER_INFO                                 = 0x0106,
    GALLERY_MEASURE                                         = 0x0107,
    GALLERY_MEASURE_VAL                                     = 0x0108, 
    GALLERY_RULER_TEXT                                      = 0x0109,
    GALLERY_TEXT                                            = 0x010B,
}TPS_MAIN_COMPONENT;

//unsigned char, 1 - 255, 0 is empty
typedef enum TpsFirstPostfixComponent
{
    CORONAL                                                 = 0x01,
    SAGITTAL                                                = 0x02,
    AXIAL                                                   = 0x03,
    //tda drr section-----------------------------------------------
    DRR_LEFT                                                = 0x04,
    DRR_RIGHT                                               = 0x05,
    //for easy plan image: drr or jpg
    EASYPLAN_IMAGE_SECTION                                  = 0x06,
    D4                                                      = 0x07,
}TPS_FIRST_POSTFIX_COMPONENT;

//unsigned char, 1 - 255, 0 is empty
typedef enum TpsSecondPostfixComponent
{
    PREV                                                    = 0x01,
    POST                                                    = 0x02,
    TEMP                                                    = 0x03,
    VOI_PREVIEW                                             = 0x04,
    BEAM_PREVIEW                                            = 0x05,
}TPS_SECOND_POSTFIX_COMPONENT;

typedef enum TpsGOType
{
    GO_TYPE_DUMMY                                           = 0x00010000,
    GO_TYPE_VOLUME                                          = 0x00020000,
    GO_TYPE_VOLUME_D4                                       = 0x00020700,
    GO_TYPE_MASK                                            = 0x00030000,
    GO_TYPE_MPR_CORONAL                                     = 0x00040100,
    GO_TYPE_MPR_SAGITTAL                                    = 0x00040200,
    GO_TYPE_MPR_AXIAL                                       = 0x00040300,
    GO_TYPE_MPR_AXIAL_PREV                                  = 0x00040301,
    GO_TYPE_MPR_AXIAL_POST                                  = 0x00040302,
    GO_TYPE_MPR_CORONAL_VOI_PREVIEW                         = 0x00040104,
    GO_TYPE_MPR_SAGITTAL_VOI_PREVIEW                        = 0x00040204,
    GO_TYPE_MPR_AXIAL_VOI_PREVIEW                           = 0x00040304,
    GO_TYPE_MPR_CORONAL_BEAM_PREVIEW                        = 0x00040105,
    GO_TYPE_MPR_SAGITTAL_BEAM_PREVIEW                       = 0x00040205,
    GO_TYPE_MPR_AXIAL_BEAM_PREVIEW                          = 0x00040305,
    GO_TYPE_SHADEDSURFCE                                    = 0x00050000,
    GO_TYPE_MANUL_REMOVE_TABLE                              = 0x00060000,
    GO_TYPE_SURFACE_CAMERA                                  = 0x00070000,
    GO_TYPE_CROSSHAIR_CORONAL                               = 0x00080100,
    GO_TYPE_CROSSHAIR_SAGITTAL                              = 0x00080200,
    GO_TYPE_CROSSHAIR_AXIAL                                 = 0x00080300,
    GO_TYPE_DOSE_CORONAL                                    = 0x00090100,
    GO_TYPE_DOSE_SAGITTAL                                   = 0x00090200,
    GO_TYPE_DOSE_AXIAL                                      = 0x00090300,
    GO_TYPE_DOSE_AXIAL_PREV                                 = 0x00090301,
    GO_TYPE_DOSE_AXIAL_POST                                 = 0x00090302,
    GO_TYPE_DOSE_GRID_BOUNDARY                              = 0x000a0000,
    GO_TYPE_LIMIT_REGION                                    = 0x000b0000,
    GO_TYPE_VR                                              = 0x000c0000,
    GO_TYPE_DRR                                             = 0x000e0000,
    //GO_TYPE_FLUENCEMAP                                      = 0x000f0000,
    GO_TYPE_BEV_DISPLAY                                     = 0x00100000,
    GO_TYPE_BEV_DISPLAY_CONTROL                             = 0x00110000,
    GO_TYPE_BEV_CAMERA                                      = 0x00120000,
    GO_TYPE_CT2DENSITYCollection                            = 0x00130000,
    GO_TYPE_FUSION                                          = 0x00140000,
    GO_TYPE_VARYING_FUSION_GO                               = 0x00140003,
    GO_TYPE_LANDMARK                                        = 0x00160000,
    GO_TYPE_FUSION_REGION_FIXED                             = 0x00170000,
    GO_TYPE_FUSION_REGION_FLOAT                             = 0x00180000,
    GO_TYPE_FUSION_CHESS_CORONAL                            = 0x00190100,
    GO_TYPE_FUSION_CHESS_SAGITTAL                           = 0x00190200,
    GO_TYPE_FUSION_CHESS_AXIAL                              = 0x00190300,
    GO_TYPE_VOI                                             = 0x001a0000,
    GO_TYPE_VOI_COLLECTION                                  = 0x001b0000,
    GO_TYPE_VOI_COLLECTION_PREVIEW                          = 0x001b0004,
    GO_TYPE_VOI_RENDERINGCACHE_CORONAL                      = 0x001d0100,
    GO_TYPE_VOI_RENDERINGCACHE_SAGITTAL                     = 0x001d0200,
    GO_TYPE_VOI_RENDERINGCACHE_AXIAL                        = 0x001d0300,
    GO_TYPE_VOI_RENDERINGCACHE_AXIAL_PREV                   = 0x001d0301,
    GO_TYPE_VOI_RENDERINGCACHE_AXIAL_POST                   = 0x001d0302,
    GO_TYPE_VOI_RENDERINGCACHE_CORONAL_PREVIEW              = 0x001d0104,
    GO_TYPE_VOI_RENDERINGCACHE_SAGITTAL_PREVIEW             = 0x001d0204,
    GO_TYPE_VOI_RENDERINGCACHE_AXIAL_PREVIEW                = 0x001d0304,
    GO_TYPE_VOI_RENDERINGCACHE_COLLECTION_CORONAL           = 0x001e0100,
    GO_TYPE_VOI_RENDERINGCACHE_COLLECTION_SAGITTAL          = 0x001e0200,
    GO_TYPE_VOI_RENDERINGCACHE_COLLECTION_AXIAL             = 0x001e0300,
    GO_TYPE_VOI_RENDERINGCACHE_COLLECTION_CORONAL_PREVIEW   = 0x001e0104,
    GO_TYPE_VOI_RENDERINGCACHE_COLLECTION_SAGITTAL_PREVIEW  = 0x001e0204,
    GO_TYPE_VOI_RENDERINGCACHE_COLLECTION_AXIAL_PREVIEW     = 0x001e0304,
    GO_TYPE_VOI_RENDERINGCACHE_COLLECTION_AXIAL_PREV        = 0x001e0301,
    GO_TYPE_VOI_RENDERINGCACHE_COLLECTION_AXIAL_POST        = 0x001e0302,
    GO_TYPE_ROI_CONTOUR_SET                                 = 0x001f0000,
    GO_TYPE_ROI_CONTOUR_SET_COLLECTION                      = 0x00200000,
    GO_TYPE_NUDGE_CIRCLE                                    = 0x00210000,
    GO_TYPE_POI                                             = 0x00220000,
    GO_TYPE_POI_COLLECTION                                  = 0x00230000,
    GO_TYPE_MAX_POINT                                       = 0x00230004,
    GO_TYPE_MAX_POINT_COLLECTION                            = 0x00230005,
    //GO_TYPE_POI_COLLECTION_IMAGE                            = 0x00230001,
    //GO_TYPE_QA_TEMP_POI_COLLECTION                          = 0x00230003,
    GO_TYPE_SETUP_POI                                       = 0x00240000,
    GO_TYPE_BEAM                                            = 0x00250000,
    GO_TYPE_BEAM_COLLECTION                                 = 0x00260000,
    GO_TYPE_BEAM_TEMPLATE_PREVIEW                           = 0x00260001,
    //GO_TYPE_CONTOURS_COLLECTION                             = 0x00260002,
    GO_TYPE_DRRSETTING_COLLECTION                           = 0x00260003,

    GO_TYPE_NORMALGROUP_COLLECTION                          = 0x00270000,
    GO_TYPE_WEDGE                                           = 0x00280000,
    //GO_TYPE_BEV_IMAGE_TEXT                                  = 0x00290000,
    //------------------------------------------------------------------
    //暂保留，plan record 还是要用
    GO_TYPE_MPR_CORNERINFO_CORONAL                          = 0x002a0100,
    GO_TYPE_MPR_CORNERINFO_SAGITTAL                         = 0x002a0200,
    GO_TYPE_MPR_CORNERINFO_AXIAL                            = 0x002a0300,
    GO_TYPE_MPR_CORNERINFO_AXIAL_PREV                       = 0x002a0301,
    GO_TYPE_MPR_CORNERINFO_AXIAL_POST                       = 0x002a0302,
    //-------------------------------------------------------------------
    GO_TYPE_MPR_RULERTEXT_CORONAL                           = 0x002c0100,
    GO_TYPE_MPR_RULERTEXT_SAGITTAL                          = 0x002c0200,
    GO_TYPE_MPR_RULERTEXT_AXIAL                             = 0x002c0300,
    GO_TYPE_MPR_RULERTEXT_AXIAL_PREV                        = 0x002c0301,
    GO_TYPE_MPR_RULERTEXT_AXIAL_POST                        = 0x002c0302,
    GO_TYPE_DOSE_LINE_SETTING                               = 0x002D0000,
    GO_TYPE_DOSE_MESH_COLLECTION                            = 0x002D0001,
    //------------------------------------------------------------------
    GO_TYPE_HOT_SPOTS                                       = 0x002E0000,
    GO_TYPE_COLD_SPOTS                                      = 0x002E0001,
    GO_TYPE_EASYPLAN_DRR                                    = 0x002F0000,
    GO_TYPE_EASYPLAN_MEASURE                                = 0x002F0100,
    GO_TYPE_EASY_PLAN_BEAM_COLLECTION                       = 0x00300000,
    //------------------------------------------------------------------
    GO_TYPE_APPLICATOR                                      = 0x00310000,
    GO_TYPE_APERTURE_BLOCK                                  = 0x00320000,
    GO_TYPE_SHIELDING_BLOCK                                 = 0x00330000,
    GO_TYPE_SERIES_TABLE_CACHE_AXIAL                        = 0x00340300,
    GO_TYPE_SERIES_TABLE_CACHE_SAGITTAL                     = 0x00340200,
    GO_TYPE_SERIES_TABLE_CACHE_CORONAL                      = 0x00340100,
    GO_TYPE_RT_IMAGE_COLLECTION                             = 0x00350000,

    // Gallery
    GO_TYPE_GALLERY_DUMMY                                   = 0x01000000,
    GO_TYPE_GALLERY_VOLUME                                  = 0x01010000,
    GO_TYPE_GALLERY_VOLUME_D4                               = 0x01010100,
    GO_TYPE_GALLERY_MASK                                    = 0x01020000,
    GO_TYPE_GALLERY_MPR_CORONAL                             = 0x01030100,
    GO_TYPE_GALLERY_MPR_SAGITTAL                            = 0x01030200,
    GO_TYPE_GALLERY_MPR_AXIAL                               = 0x01030300,
    GO_TYPE_GALLERY_CROSSHAIR_CORONAL                       = 0x01040100,
    GO_TYPE_GALLERY_CROSSHAIR_SAGITTAL                      = 0x01040200,
    GO_TYPE_GALLERY_CROSSHAIR_AXIAL                         = 0x01040300,
    GO_TYPE_GALLERY_VR                                      = 0x01050000,
    GO_TYPE_GALLERY_MPR_CORNERINFO_CORONAL                  = 0x01060100,
    GO_TYPE_GALLERY_MPR_CORNERINFO_SAGITTAL                 = 0x01060200,
    GO_TYPE_GALLERY_MPR_CORNERINFO_AXIAL                    = 0x01060300,
    GO_TYPE_GALLERY_MEASURE_CORONAL                         = 0x01070100,
    GO_TYPE_GALLERY_MEASURE_SAGITTAL                        = 0x01070200,
    GO_TYPE_GALLERY_MEASURE_AXIAL                           = 0x01070300,
    GO_TYPE_GALLERY_MEASURE_VAL_CORONAL                     = 0x01080100,
    GO_TYPE_GALLERY_MEASURE_VAL_SAGITTAL                    = 0x01080200,
    GO_TYPE_GALLERY_MEASURE_VAL_AXIAL                       = 0x01080300,
    GO_TYPE_GALLERY_RULE_TEXT_CORONAL                       = 0x01090100,
    GO_TYPE_GALLERY_RULE_TEXT_SAGITTAL                      = 0x01090200,
    GO_TYPE_GALLERY_RULE_TEXT_AXIAL                         = 0x01090300,
    GO_TYPE_GALLERY_SURFACE_CAMERA                          = 0x010A0000,
    GO_TYPE_GALLERY_TEXT_CORONAL                            = 0x010B0100,
    GO_TYPE_GALLERY_TEXT_SAGITTAL                           = 0x010B0200,
    GO_TYPE_GALLERY_TEXT_AXIAL                              = 0x010B0300,
    GO_TYPE_GALLERY_COLORTABLE                              = 0x010C0000,
}TPS_GO_TYPE;

typedef enum MPR_CORNER_TYPE
{
    CORNER_TYPE_MOUSEPOSITION              = 0x01,
    CORNER_TYPE_VALUEVALID                 = 0x02,
    CORNER_TYPE_WINDOWCENTERWIDTH          = 0x04,
    CORNER_TYPE_VALUE_DENSITY_DOSE         = 0x08,
    CORNER_TYPE_SLICEINDEX_THICKNESS       = 0x10,
    CORNER_TYPE_ALL                        = 0x1f,
}MPR_CORNER_TYPE;

//section type
static const std::string CORONAL_SECTION("CORONAL");
static const std::string SAGITTAL_SECTION("SAGITTAL");
static const std::string AXIAL_SECTION("AXIAL");
static const std::string AXIAL_SECTION_PREV("AXIAL_PREV");
static const std::string AXIAL_SECTION_POST("AXIAL_POST");

// Slice Position Key
static const std::string SLICEPOSITION = "SLICEPOSITION";
static const std::string VAL = "VAL";
static const std::string SLICEPOSITION_CORONAL_SECTION_VAL(SLICEPOSITION + "_" + CORONAL_SECTION + "_" + VAL);
static const std::string SLICEPOSITION_SAGITTAL_SECTION_VAL(SLICEPOSITION + "_" + SAGITTAL_SECTION + "_" + VAL);
static const std::string SLICEPOSITION_AXIAL_SECTION_VAL(SLICEPOSITION + "_" + AXIAL_SECTION + "_" + VAL);
static const std::string SLICEPOSITION_AXIAL_SECTION_PREV_VAL(SLICEPOSITION + "_" + AXIAL_SECTION_PREV + "_" + VAL);
static const std::string SLICEPOSITION_AXIAL_SECTION_POST_VAL(SLICEPOSITION + "_" + AXIAL_SECTION_POST + "_" + VAL);

// Slice Total Number Key
static const std::string SLICEPOSITION_TOTAL = "SLICEPOSITION_TOTAL";
static const std::string SLICEPOSITION_TOTAL_CORONAL_SECTION_VAL(SLICEPOSITION_TOTAL + "_" + CORONAL_SECTION + "_" + VAL);
static const std::string SLICEPOSITION_TOTAL_SAGITTAL_SECTION_VAL(SLICEPOSITION_TOTAL + "_" + SAGITTAL_SECTION + "_" + VAL);
static const std::string SLICEPOSITION_TOTAL_AXIAL_SECTION_VAL(SLICEPOSITION_TOTAL + "_" + AXIAL_SECTION + "_" + VAL);
static const std::string SLICEPOSITION_TOTAL_AXIAL_SECTION_PREV_VAL(SLICEPOSITION_TOTAL + "_" + AXIAL_SECTION_PREV + "_" + VAL);
static const std::string SLICEPOSITION_TOTAL_AXIAL_SECTION_POST_VAL(SLICEPOSITION_TOTAL + "_" + AXIAL_SECTION_POST + "_" + VAL);

// Slice thickness Key
static const std::string THICKNESS = "THICKNESS";
static const std::string THICKNESS_CORONAL_SECTION_VAL(THICKNESS + "_" + CORONAL_SECTION + "_" + VAL);
static const std::string THICKNESS_SAGITTAL_SECTION_VAL(THICKNESS + "_" + SAGITTAL_SECTION + "_" + VAL);
static const std::string THICKNESS_AXIAL_SECTION_VAL(THICKNESS + "_" + AXIAL_SECTION + "_" + VAL);
static const std::string THICKNESS_AXIAL_SECTION_PREV_VAL(THICKNESS + "_" + AXIAL_SECTION_PREV + "_" + VAL);
static const std::string THICKNESS_AXIAL_SECTION_POST_VAL(THICKNESS + "_" + AXIAL_SECTION_POST + "_" + VAL);

// ScaleRuler Value Key
static const std::string RULERVAL0("VAL0");
static const std::string RULERVAL2("VAL2");
static const std::string RULERVAL4("VAL4");
static const std::string RULERVAL6("VAL6");
static const std::string RULERVAL8("VAL8");
static const std::string RULERVAL10("VAL10");
TPS_END_NAMESPACE

#endif
