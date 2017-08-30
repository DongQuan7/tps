////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_fw_common_enums.h
/// 
///  \brief enum to string
/// 
///  \version 1.0
/// 
///  \date    Jan.17,2014
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_COMMON_ENUMS_H_
#define TPS_FW_COMMON_ENUMS_H_

#include "tps_fw_defs.h"
#include <cstdint>


TPS_BEGIN_NAMESPACE;

typedef enum 
{
    NotCompute=0,
    OldComputeResult,
    NewComputeResult,
    OtherStatus
}CurveScaleComputeStatus;

typedef enum {
    COMMON = 0,
    CONTOURING,
    PLANNING,
    EVALUATION,
    PLANQA,
    EASYPLAN,
    GALLERY,
    ADMIN,
} MODULE_TYPE;

///////////////////////////////
// The enum declaration for organ type 
///////////////////////////////
typedef enum
{
    SKIN = 0,
    BODY,   //BODY and SKIN are the same organ but different name
    LUNGS,
    LUNG_LEFT,
    LUNG_RIGHT,
    SPINAL_CORD,
    HEART,
    LIVER,
    SPLEEN,
    KIDNEYS,
    KIDNEY_LEFT,
    KIDNEY_RIGHT,
    PAROTIDS,
    PAROTID_LEFT,
    PAROTID_RIGHT,
    BRAINSTEM,
    MANDIBLE,
    EYES,
    EYE_LEFT,
    EYE_RIGHT,
    FEMORAL_HEAD,
    BRAIN,
    RECTUM,
    PROSTATE,
    TEMPOROMANDIBULAR_JOINT,
    PANCREAS,
    BLADDER,//NOT delivered
    BONES, //NOT delivered
    UNKNOWN
} TPS_ORGAN_TYPE;

typedef enum TpsModuleType{
    PLAN_MODULE,
    EZPLAN_MODULE
}TPS_MODULE_TYPE;

static const char * Enum2String_ORGAN_TYPE[] = 
{
    "SKIN"                      , 
    "BODY"                      ,
    "LUNGS"                     , 
    "LUNG_LEFT"                 ,
    "LUNG_RIGHT"                ,
    "SPINAL_CORD"               ,
    "HEART"                     ,
    "LIVER"                     ,
    "SPLEEN"                    ,
    "KIDNEYS"                   ,
    "KIDNEY_LEFT"               ,
    "KIDNEY_RIGHT"              ,
    "PAROTIDS"                  ,
    "PAROTID_LEFT"              ,
    "PAROTID_RIGHT"             ,
    "BRAINSTEM"                 ,
    "MANDIBLE"                  ,
    "EYES"                      ,
    "EYE_LEFT"                  ,
    "EYE_RIGHT"                 ,
    "FEMORAL_HEAD"              ,
    "BRAIN"                     ,
    "RECTUM"                    ,
    "PROSTATE"                  ,
    "TEMPOROMANDIBULAR_JOINT"   ,
    "PANCREAS"                  ,
    "BLADDER"                   ,//NOT delivered
    "BONES"                     , //NOT delivered
    "UNKNOWN"
};

typedef enum
{
    IDLE = 0,
    DOWN,
    MOVE,
    UP,
    DRAG,
    DOUBLE_CLICK_,
    LEAVE,
} MOUSE_ACTION_TYPE;

typedef enum
{
    Center = 0,
    Top,
    Bottom,
} MLC_EDIT_TYPE;

typedef enum
{
    X,
    Y,
    CATORCORNER,
    ZZ,
    OTHER
}COORDINATRDIRCTION;

//The Enums: SMART_CONTOUR_ACTION_TYPE & NUDGE_ACTION_TYPE are not used in voi-editing
//operations now. But because of it's re-used somewhere else, keep them.
typedef enum{
    SMART_CONTOUR_INVALID = -1,
    CREATE_NEW_CONTOUR,
    ADD_NEW_POINT,
    UPDATE_CANDIDATE_POINT,
    CLOSE_CONTOUR,
}SMART_CONTOUR_ACTION_TYPE;

//The Enums: SMART_CONTOUR_ACTION_TYPE & NUDGE_ACTION_TYPE are not used in voi-editing
//operations now. But because of it's re-used somewhere else, keep them.
typedef enum{
    NUDGE_INVALID = -1,
    NUDGE_BEGIN,
    NUDGING,
    NUDGE_OVER,
    CLEAR_NUDGE_SHAPE,
}NUDGE_ACTION_TYPE;

typedef enum{
    DRAG_INVALID = -1,
    BEGIN_DRAG,
    DRAGGING,
    END_DRAG,
}DRAG_ACTION_TYPE;

typedef enum{
    SMART_APERTURE_INVALID = -1,
    CREATE_NEW_APERTURE,
    ADD_NEW_VERTEX,
    UPDATE_CANDIDATE_VERTEX,
    CLOSE_APERTURE,
}SMART_APERTURE_ACTION_TYPE;

typedef enum{
    //VEAT: Voi Editing Action Type
    VEAT_IDLE   = 0x00,
    VEAT_BEGIN  = 0x01,
    VEAT_ING    = 0x02,
    VEAT_END    = 0x04,
    VEAT_EXT    = 0x08,
    VEAT_LEAVE  = 0x10,
}EDIT_VOI_ACTION_TYPE;

typedef enum {
    MOVIE_START = 0,
    MOVIE_STOP,
    MOVE_SLIDER,
    UPDATE_SPEED,

}MOVIE_ACTION_TYPE;

typedef enum
{
    PAGETRUNACTION = 512,
    MPRPANACTION,
    MPRPAN_AXIALACTION,
    MPRPAN_CORONALACTION,
    MPRPAN_SAGITTALACTION,
    MPRZOOMACTION,
    MPRZOOM_AXIALACTION,
    MPRZOOM_CORONALACTION,
    MPRZOOM_SAGITTALACTION,
    MPRWINDOWINGACTION,
    MPRWINDOWING_AXIALACTION,
    MPRWINDOWING_CORONALACTION,
    MPRWINDOWING_SAGITTALACTION,
    MPRPOINTERACTION,
    MPRPOINTER_AXIALACTION,
    MPRPOINTER_CORONALACTION,
    MPRPOINTER_SAGITTALACTION,
    DRAW_BEAM_OUTLINEACTION,
    EDIT_MLCACTION,
    DUMMYACTION,
    EDIT_GRIDBOUNDARYACTION,
    DRR_WINDOWINGACTION,

    /// <summary>
    /// for Shaded Surface Window
    /// </summary>
    SHADEDSURFACE_ROTATEACTION,
    SHADEDSURFACE_ZOOMACTION,


    //zuo kai : do not change following code(from 600 ~ 612)
    SMARTCONTOURACTION = 600,
    NUDGE_ACTION = 601,
    PAINT_ACTION = 602,
    SPLINEACTION = 603,
    CIRCLEACTION = 604,
    RECTANGLEACTION = 605,
    REGIONGROWINGACTION = 606,
    DRAW_REGIONACTION = 607,
    ERASERCIRCLEACTION = 608,
    ERASERRECTANGLEACTION = 609,
    ERASERPOLYGONACTION = 610,
    ERASERLINEACTION = 611,
    ERASERSELECTEDREGIONACTION = 612,


    EDIT_CHESSACTION,
    BEVPANACTION,
    BEVZOOMACTION,
    START_MANUAL_FUSIONACTION,
    END_MANUAL_FUSIONACTION,
    SELECT_CUT_CONTOUR_SLICESACTION,
    MOVE_VOIACTION,
    //         ADD_LANDMARK_POINT,
    //         ADD_LANDMARK_POINT_FIXED,
    //         ADD_LANDMARK_POINT_FLOAT,
    MANUAL_REMOVE_TABLEACTION,
    EDIT_FUSION_REGIONACTION,
    MOVE_CROSSHAIRACTION,
    IDLEACTIONACTION,
    SET_ISOCENTERACTION,
    MLC_SMARTCONTOURACTION,
    MLC_RECTANGLEACTION,
    MLC_CIRCLEACTION,
    MLC_NUDGEACTION,
    EASYPLAN_MLC_SMARTCONTOURACTION,
    EASYPLAN_MLC_RECTANGLEACTION,
    EASYPLAN_MLC_CIRCLEACTION,
    EASYPLAN_MLC_NUDGEACTION,
    MEASURE_DISTANCEACTION,
    MEASURE_ANGLEACTION,
    EASYPLAN_WINDOWOINGACTION,
    EASYPLAN_PANACTION,
    EASYPLAN_ROTATEACTION,
    EASYPLAN_ZOOMACTION,
    DEFAULT_MOUSEACTION,
    MOVE_CURRENT_SLICE_VOIACTION,
    EDIT_FIXED_FUSION_REGIONACTION,
    EDIT_FLOAT_FUSION_REGIONACTION,
    FIXED_DUMMYACTION,
    FLOAT_DUMMYACTION,
    PLANNING_MOVE_CROSSHAIRACTION,
    EDIT_LANDMARK_POINTACTION,

    ADD_POIACTION,
    EDIT_POIACTION,
    ADD_SETUP_POIACTION,  // this is a trick, it will refer to POIAction
    EDIT_SETUP_POIACTION,
    POI_CREATE_PLANNINGACTION,
    POI_EDIT_PLANNINGACTION,
    ADD_QA_ISOCENTERACTION,

    GALLERY_MPRPAN_AXIALACTION,
    GALLERY_MPRPAN_CORONALACTION,
    GALLERY_MPRPAN_SAGITTALACTION,
    GALLERY_MPRZOOMACTION,
    GALLERY_MPRZOOM_AXIALACTION,
    GALLERY_MPRZOOM_CORONALACTION,
    GALLERY_MPRZOOM_SAGITTALACTION,
    GALLERY_MPRWINDOWINGACTION,
    GALLERY_MPRWINDOWING_AXIALACTION,
    GALLERY_MPRWINDOWING_CORONALACTION,
    GALLERY_MPRWINDOWING_SAGITTALACTION,
    GALLERY_PAGETURNACTION,
    GALLERY_MOVE_CROSSHAIRACTION,
}ACTION_TYPE;

typedef enum {
    //VEM: VOI Editing Mode
    VEM_SMART_CONTOUR = 0,
    VEM_NUDGE,
    VEM_SPLINE,
    VEM_CIRCLE,
    VEM_RECTANGLE,
    VEM_ERASER_CIRCLE,
    VEM_ERASER_RECTANGLE,
    VEM_ERASER_SELECT_REGION,
    VEM_ERASER_LINE,
    VEM_ERASER_POLYGON,
    VEM_SELECT_SLICES,
    VEM_PAINT,
}VOIEditingMode;

typedef enum {
    MLC_SMARTCONTOUR_MODE = 0,
    MLC_RECTANGLE_MODE,
    MLC_CIRCLE_MODE,
    MLC_NUDGE_MODE,
    MLC_DRAG_MODE,
    MLC_CLEAR_MODE,
}MLCEditingMode;

typedef enum {
    CIRCLESHAPE = 0,
    RECTANGLESHAPE
}NUDGESHAPE;

typedef enum{
    REDO = 0,
    UNDO,
}REDO_UNDO_TYPE;

typedef enum{
    FUSION_ROLE_FIXED = 0,
    FUSION_ROLE_FLOAT,
}FUSION_ROLE;

typedef enum{
    FUSION_TYPE_BLEND = 0,
    FUSION_TYPE_CHESS,
}FUSION_TYPE;

typedef enum{
    LANDMARK = 0,
    ALIGNMENT,
    DEFORMABLE,
    INIT_SETUP,
}REIGISTRATION_ALGORITHM;

typedef enum FusionImageStatus{
    FIXED_IMAGE_READY = 1,
    FLOAT_IMAGE_READY = 2,
}FUSION_IMAGE_STATUS;

typedef enum{
    NO_FEEDBACK = 0,
    LANDMARK_POINTS_IN_SAME_PLANE,
    LANDMARK_POINTS_MARKED_FLAG_CHANGED,
    REGISTRATION_IMAGES_STATUS,
    FUSION_TYPE_CHANGED,
    HIDE_REGISTRATION_REGION,
    //TO BE ADDED
}REGISTRATION_FEEDBACK;

typedef enum {
    BEV_VIEW = 0,
    CEV_VIEW,
}BEV_VIEW_TYPE;

typedef enum {
	DRR = 0,
	FLUENCE_MAP
}BEV_DISPLAY_TYPE;

enum PlanEditType{
    PLAN_EDIT_INVALID = -1,
    PLAN_EDIT_NAME,
    PLAN_EDIT_GOAL,
    PLAN_EDIT_MACHINE,
    PLAN_EDIT_TECHNIQUE_TYPE,
    PLAN_EDIT_IS_JOINED,
};

enum NormalEditType
{
    NORMGROUP_EDIT_INVALID = -1,
    NORMGROUP_EDIT_NAME,
    NORMGROUP_EDIT_GOAL,
    NORMGROUP_EDIT_NORMALIZATIONTYPE,
    NORMGROUP_EDIT_PRESCRIPTIONVOI,
    NORMGROUP_EDIT_PRESCRIPTIONPOI,
    NORMGROUP_EDIT_MACHINE,
    NORMGROUP_EDIT_RADIATION_TYPE,
    NORMGROUP_EDIT_TECHNIQUE_TYPE,
    NORMGROUP_EDIT_IS_JOINED,
    NORMGROUP_EDIT_ALGORITHM_TYPE,
    NORMGROUP_EDIT_PRISCRIPTION,
    //TO BE ADDED

    NORMGROUP_EDIT_ALL
};

enum BeamEditType
{
    //using UpdateBeamAttributesCmd
    EDIT_BEAM_NAME                                                 = 0x00000001,
    EDIT_BEAM_DESCRIPTION                                          = 0x00000002,
    EDIT_BEAM_VISIBILITY                                           = 0x00000004,
    EDIT_BEAM_ISAUTOCREATED_DESCRIPTION                            = 0x00000008,
    EDIT_BEAM_AUTOTRACKING_SHAPE                                   = 0x00000010,
    EDIT_BEAM_SHIELDINGBLOCK_AUTOTRACKINGSHAPE                     = 0x00000020,
    EDIT_BEAM_NULL_TARGETVOI                                       = 0x00000040,
    EDIT_BEAM_NULL_AVOIDVOI                                        = 0x00000080,
    EDIT_BEAM_SHIELDING_BLOCK_NULL_TARGETVOI                       = 0x00000100,
    EDIT_BEAM_SHIELDING_BLOCK_NULL_AVOIDVOI                        = 0x00000200,
    EDIT_BEAM_DOSE_RATE                                            = 0x00000400,
    EDIT_BEAM_IS_JOINED                                            = 0x00000800,
    EDIT_BEAM_ALGORITHM_TYPE                                       = 0x00001000,
    EDIT_BEAM_APERTURE_JAWFOLLOW_INACTIVE                          = 0x00002000,
    EDIT_BEAM_COMMISSIONUNIT                                       = 0x00004000,
    EDIT_BEAM_MU                                                   = 0x00008000,
    EDIT_BEAM_BEAMWEIGHT                                           = 0x00010000,

    //using EditBeamCmd
    EDIT_BEAM_GANTRYANGLE                                          = 0x10000001,
    EDIT_BEAM_COLLIMATORANGLE                                      = 0x10000002,
    EDIT_BEAM_COUCHANGLE                                           = 0x10000004,
    EDIT_BEAM_DIRECTION                                            = 0x10000008,
    EDIT_BEAM_ISOCENTER                                            = 0x10000010,
    EDIT_BEAM_APERTURETYPE                                         = 0x10000020,
    EDIT_BEAM_MLC_EDIT_MODE                                        = 0x10000040,
    EDIT_BEAM_APERTURE_TARGET_VOI                                  = 0x10000080,
    EDIT_BEAM_APERTURE_AVOID_VOI                                   = 0x10000100,
    EDIT_BEAM_APERTURE_TARGET_MARGIN                               = 0x10000200,
    EDIT_BEAM_APERTURE_AVOID_MARGIN                                = 0x10000400,
    EDIT_BEAM_APERTURE_JAWFOLLOW_ACTIVE                            = 0x10000800,
    EDIT_BEAM_APERTURE_JAWFOLLOW_MARGIN                            = 0x10001000,
    EDIT_BEAM_JAW_ISXSYMMETRIC                                     = 0x10002000,
    EDIT_BEAM_JAW_ISYSYMMETRIC                                     = 0x10004000,
    EDIT_BEAM_JAW_SIZE                                             = 0x10008000,
    EDIT_BEAM_USE_SHIELDING_BLOCK                                  = 0x10010000,
    EDIT_BEAM_SHIELDING_BLOCK_TARGETVOI                            = 0x10020000,
    EDIT_BEAM_SHIELDING_BLOCK_AVOIDVOI                             = 0x10040000,
    EDIT_BEAM_SHIELDING_BLOCK_TARGETMARGIN                         = 0x10080000,
    EDIT_BEAM_SHIELDING_BLOCK_AVOIDMARGIN                          = 0x10100000,
    EDIT_BEAM_TRAY                                                 = 0x10200000,
    EDIT_BEAM_PHSYICIAL_WEDGE                                      = 0x10400000,
    EDIT_BEAM_VIRTURAL_WEDGE                                       = 0x10800000,
    EDIT_BEAM_BOLUS                                                = 0x11000000,
    EDIT_BEAM_SSD                                                  = 0x12000000,
    EDIT_BEAM_ALL                                                  = 0x14000000
};

typedef enum {
    DOSE_KEEP = 0x00,
    DOSE_RE_CALC = 0x01,
    DOSE_COMPOSE = 0x02,
    DOSE_CLEAR = 0x04,
}DOSE_CHANGE_TYPE;

enum EditContourMode
{
    CUT = 0,
    COPY,
    PASTE,
    DELETEMode,
    JUDGECOMMONPART,
};

enum SubEditContourMode
{
    REPLACE = 0,
    COMBINE,
    CANCEL,
};

typedef enum {
    TOTALGOALS = 0,
    MAXDOSE,
    SPECIFIEDDOSE,
} DOSELINE_CALCMODE;

typedef enum {
    RELATIVE_VALUE = 0,
    ABSOLUTE_VALUE,
} DOSELINE_DISPLAYMODE;

typedef enum {
    NONE        = -1,
    LINE        = 0,
    COLORWASH   = 1,
    BOTH        = 2,
} DOSELINE_RENDERMODE;

//修改该enum，需要同时修改前端对应的
typedef enum {
    DEFAULT_DOSELINE_TEMPLATE = 0,
    DOSELINE_TEMPLATE,
    VOI_TEMPLATE_TYPE,
    BEAM_TEMPLATE_TYPE,
    OPTIMIZER_TEMPLATE_TYPE,
    WINDOWLEVEL_WINDOWWIDTH_TEMPLATE_TYPE
} SYSTEMCONFIG_DATA_TYPE;

typedef enum{
    PROCESSINGSTART = 0,
    PROCESSINGUPDATE,
    PROCESSINGFINISH,
} PROCESSINGSTATE;

typedef enum {
    RED = 0,    //#FF0000
    GREEN,      //#008000 
    BLUE,       //#0000FF
    YELLOW,     //#FFFF00
    WHITE,      //#FFFFFF
    BLACK       //#000000
} TPS_COLOR;

typedef enum {
    Normal = 0,
    QA,
    CopiedQA,
}SeriesType;

// Tag is type of unsigned int
// DICOM Standard: (0028,0010) Rows
// DICOM Standard: (0028,0011) Columns
// DICOM Standard: (0028,1050) Window Center
// DICOM Standard: (0028,1051) Window Width
// DICOM Standard: (0028,0100) Bits Allocated 
// DICOM Standard: (0028,0101) Bits Stored
// DICOM Standard: (0028,0102) High Bit
// DICOM Standard: (0028,0103) Pixel Representation
// DICOM Standard: (7FE0,0010) Pixel Data
// DICOM Standard: (0008,0008) Image Type
// DICOM Standard: (0028,0002) Samples per Pixel
typedef enum {
    DICOMROWSTAG = 0x00280010,
    DICOMCOLUMNSTAG = 0x00280011,
    DICOMWINDOWCENTER = 0x00281050,
    DICOMWINDOWWIDTH = 0x00281051,
    DICOMBITSALLOCATED = 0x00280100,
    DICOMBITSSTORED = 0x00280101,
    DICOMHIGHBIT = 0x00280102,
    DICOMPIXELREPRESENTATION = 0x00280103,
    DICOMPIXELDATA = 0x7FE00010,
    DICOMIMAGETYPE = 0x00080008,
    DICOMSAMPLESPERPIXEL = 0x00280002,
    DICOMIMAGEPLANEPIXELSPACING = 0x30020011,
    DICOMPIXELSPACING = 0x00280030,
    DICOMPATIENTNAME = 0x00100010,
    DICOMPATIENTID = 0x00100020,
    DICOMPATIENTBIRTHDATE = 0x00100030,
    DICOMPATIENTSEX = 0x00100040,
    DICOMSSD = 0x30020024,
    DICOMPATIENTPOSITION = 0x00185100,
    DICOMIMAGEDATE = 0x00080023,
    DICOMIMAGETIME = 0x00080033,
    DICOMACQUISITIONDATE = 0x00080022,
    DICOMACQUISITIONTIME = 0x00080032,
    DICOMGANTRYANGLE = 0x300A011E,
}DicomTag;

enum DATATYPE{
    NONE_DATA_TYPE = -1,
    DATA_TYPE_UNSIGNED_CHAR  = 0,
    DATA_TYPE_UNSIGNED_SHORT = 1,
    DATA_TYPE_CHAR = 2,
    DATA_TYPE_SHORT = 3,
    DATA_TYPE_FLOAT = 4,
    DATA_TYPE_DOUBLE = 5,
};
//#pragma warning(disable: 4505) //Unreferenced local function has been removed
void TPS_FW_DECLSPEC GetClorFromEnum(TPS_COLOR /*colorType*/, float /*fColor*/[4]);

#ifndef LANDMARK_POINT_PAIR_COUNT
#define LANDMARK_POINT_PAIR_COUNT 6
#endif

#ifndef EASY_PLAN_PIXEL_SPACING_X
#define EASY_PLAN_PIXEL_SPACING_X 1
#endif

#ifndef EASY_PLAN_PIXEL_SPACING_Y
#define EASY_PLAN_PIXEL_SPACING_Y 1
#endif

enum HIT_TEST_NUM{
    HIT_NUM_NONE                        = 0x00000000,
    //crosshair part, using 3 bits
    HIT_NUM_CROSSHAIR_H                 = 0x00000001,
    HIT_NUM_CROSSHAIR_V                 = 0x00000002,
    HIT_NUM_CROSSHAIR_CENTER            = 0x00000003,
    HIT_NUM_CROSSHAIR_ROTATE            = 0x00000004,
    //chess part, using 3 bits
    HIT_NUM_CHESS_TRANSLATE             = 0x00000008,
    HIT_NUM_CHESS_ROW_ONLY              = 0x00000010,
    HIT_NUM_CHESS_COLUMN_ONLY           = 0x00000018,
    HIT_NUM_CHESS_SQUARE_SIZE           = 0x00000020,
    //landmark part, using 1 bit
    HIT_NUM_LANDMARK                    = 0x00000040,
    //manual registration
    HIT_NUM_MANUAL_REG_ROTATE           = 0x00000041,
    HIT_NUM_MANUAL_REG_TRANSLATE        = 0x00000042,
    // mlc in bev window
    HIT_NUM_MLC_LEAF_X                  = 0X00000043,
    HIT_NUM_MLC_LEAF_Y                  = 0X00000044, 
    // jaw in bev window
    HIT_NUM_JAW_LEFT                    = 0x00000045,
    HIT_NUM_JAW_RIGHT                   = 0x00000046,
    HIT_NUM_JAW_TOP                     = 0x00000047,
    HIT_NUM_JAW_BOTTOM                  = 0x00000048,
    //beam central line
    HIT_NUM_IMRT_BEAM                   = 0x00000049,
    HIT_NUM_ARC_BEAM_START              = 0x00000050,
    HIT_NUM_ARC_BEAM_END                = 0x00000051,
    //rotate point in bev
    HIT_NUM_ROTATEPOINT_TOPLEFT         = 0x00000080,
    HIT_NUM_ROTATEPOINT_TOPRIGHT        = 0x00000081,
    HIT_NUM_ROTATEPOINT_BOTTOMLEFT      = 0x00000082,
    HIT_NUM_ROTATEPOINT_BOTTOMRIGHT     = 0x00000083,
    //dose grid in mpr
    HIT_NUM_DOSEGRID_LEFT               = 0x00000090,
    HIT_NUM_DOSEGRID_RIGHT              = 0x00000091,
    HIT_NUM_DOSEGRID_TOP                = 0x00000092,
    HIT_NUM_DOSEGRID_BOTTOM             = 0x00000093,
    HIT_NUM_DOSEGRID_FULLDIRECTION      = 0x00000094,
};

//enum DOSE_ENTITY_LEVEL{
//    DOSE_ENTITY_LEVEL_PLAN = 0,
//    DOSE_ENTITY_LEVEL_NORMGROUP = 1,
//};

enum JAW_TYPE{
    JAW_TYPE_Follow = 0,
    JAW_TYPE_Max,
    JAW_TYPE_Manually,
};

enum PLAN_GOAL_SELECTION_LEVEL{
    SELECTION_LEVEL_UNKNOWN = -1,
    SELECTION_LEVEL_PLAN,
    SELECTION_LEVEL_NORMAL_GROUP,
    SELECTION_LEVEL_BEAM,
};

enum BEV_EDIT_TARGET{
    BEV_EDIT_TARGET_MLC = 0,
    BEV_EDIT_TARGET_BLOCK,
};

enum PLAY_SPEED {
    SLOW,
    NORMAL,
    FAST,
};

enum PLAY_STATE {
    STOP,
    PLAY,
    PAUSE,
    RESUME,
};

enum PLAY_MODE {
    THREED,
    FOURD,
};

enum FusionRegionUpdateType{
    AllRegion = 0,
    OnlyFixedRegion,
    OnlyFloatRegion,
};

struct HitTestResult
{
    HIT_TEST_NUM _hitNum;
    std::string  _targetUid;
    bool         _isDirty;
    HitTestResult() : _hitNum(HIT_NUM_NONE), _targetUid(""), _isDirty(false){}
};

typedef enum EDIT_MODE
{
	EDIT_COPY = 0,
	EDIT_CUT
};

typedef enum EDIT_FUNCTION
{
	EDIT_REPLACE = 0,
	EDIT_COMBINE
};

enum TMS_GENDER{
    Male = 1,
    Female = 2,
    Other = 4
};

typedef enum {
    PREVIEW_VOI = 0,
    PREVIEW_BEAM
} PREVIEW_TYPE;

enum EvaluationRole
{
    NOT_EV1_PLAN = 0,
    NOT_EV2_PLAN,
    EV1,
    EV2,
    NOT_TempResult,
};

enum PatientPosition
{
    UNKNOWN_POSITION,
    HFS,                            //Head First-Supine
    HFP,                            //Head First-Prone
    HFDR,                           //Head First-Decubitus Right
    HFDL,                           //Head First-Decubitus Left
    FFS,                            //Feet First-Supine
    FFP,                            //Feet First-Prone
    FFDR,                           //Feet First-Decubitus Right
    FFDL,                           //Feet First-Decubitus Left
};

enum DensityOverrideType
{
    NoOverride = 0,
    NormalOverride,
    ExternalOverride,
    OverlapExternal,
    Overlap
};

enum AdvancedAddRoiMode
{
    BasedOnRoi = 0,
    BasedOnDose,
};

enum BasedOnRoiOperator
{
    Mirror = 1,
    Expand,
    Shrink,
    Combine,
};

enum CombineRoiMode
{
    Union = 0, 
    Intersect, 
    Subtract,
    XOR,
};

enum MirrorDirection
{
    LR = 0,
    HF,
    AP,
};

TPS_END_NAMESPACE
#endif  //TPS_FW_COMMON_ENUMS_H_

#ifndef DB_EXCHANGE_FRONT
#define DB_EXCHANGE_FRONT (10)
#endif
