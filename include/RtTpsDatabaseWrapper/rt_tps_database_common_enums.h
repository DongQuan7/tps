//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_common_enums.h
///  \brief     rt tps database common enums 
///
///  \version 1.0
///  \date    4/5/2016
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_TPS_DATABASE_COMMON_ENUMS_H_
#define RT_TPS_DATABASE_COMMON_ENUMS_H_

#include "rt_tps_database_defs.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE;

namespace RtDbDef
 {
    // for machine, sartable, timecalpara
#define MAX_Sc_POSITIONS (25)

    typedef enum
    { 
        TYPE_ACCESSORY = 0,
        TYPE_ACCESSORYCHUNK,
        TYPE_APPLICATOR,
        TYPE_APPROVERECORD,
        TYPE_BEAM,
        TYPE_BEAM_ACCESSORY,
        TYPE_BEAM_POI,
        TYPE_BEAM_SESSION,
        TYPE_BEAM_VOI,
        TYPE_BEAMRECORD,
        TYPE_BEAMSEGMENT,
        TYPE_BIOROLE,
        TYPE_BLOCK,
        TYPE_CBCTINFO,
        TYPE_CHECKSUM,
        TYPE_CLINICAL,
        TYPE_COMMISSIONEDUNIT,
        TYPE_COMMISSIONEDUNIT_TRAY,
        TYPE_CONSTRAINT,
        TYPE_CONSTRAINTTEMPLATE,
        TYPE_CONTOUR,
        TYPE_CONTROLPOINT,
        TYPE_CONTROLPOINTRECORD,
        TYPE_COURSE,
        TYPE_CT2DENSITY,
        TYPE_CURVERESULANANLYSE,
        TYPE_DBVERSION,
        TYPE_DELIVERYBEAMGROUP,
        TYPE_DELIVERYBEAMGROUP_POI,
        TYPE_DELIVERYBEAMGROUP_SESSION,
        TYPE_DELIVERYBEAMGROUP_VOI,
        TYPE_DOSEGRID,
        TYPE_DOSELINE,
        TYPE_DOSEOPTENDCRITERIONBYINTERATION,
        TYPE_DOSEOPTENDCRITERIONBYTIME,
        TYPE_DOSEOPTGOALBASE,
        TYPE_DRRSETTING,
        TYPE_DRYRUN,
        TYPE_EQUIPMENT,
        TYPE_EVENTRECORD,
        TYPE_FBCT,
        TYPE_FBCT_SESSION,
        TYPE_GATINGCHANGEHISTORY,
        TYPE_GLOBALCONFIG,
        TYPE_GOLDENSTT,
        TYPE_HOLIDAY,
        TYPE_ICD,
        TYPE_ICD_SITE,
        TYPE_IMAGE,
        TYPE_IMAGEBEAM,
        TYPE_IMAGEBEAM_SESSION,
        TYPE_IMAGETRANSFORMATION,
        TYPE_IMMOBILIZATION,
        TYPE_KERNELDATA,
        TYPE_KERNELDATALIST,
        TYPE_KERNELDATALIST_KERNELDATA,
        TYPE_KERNELWEIGHT,
        TYPE_LOCK,
        TYPE_MACHINE,
        TYPE_MACHINE_CLINICAL,
        TYPE_MACHINEQA,
        TYPE_MC3DDOSE,
        TYPE_MCELECTRONMONOENERGYWEIGHT,
        TYPE_MCPHOCOMMISSIONEDUNIT,
        TYPE_MCPHOMONOENERGYWEIGHT,
        TYPE_MEASUREDDATA,
        TYPE_MEASUREDPROFILE,
        TYPE_MESSAGE,
        TYPE_MESSAGE_RECEIVER,
        TYPE_MLCSHAPE,
        TYPE_MONOCOMPRESSEDPHASESPACEDATA,
        TYPE_MONOPHASESPACEDATA,
        TYPE_NORM_POI,
        TYPE_NORM_VOI,
        TYPE_NORMGROUP,
        TYPE_NORMGROUP_VOI,
        TYPE_NTCP,
        TYPE_OPERATIONRECORD,
        TYPE_PATIENT,
        TYPE_PATIENTIMAGE,
        TYPE_PATIENTPOSITIONIMAGE,
        TYPE_PHASESPACEDATA,
        TYPE_PLAN,
        TYPE_PLAN_POI,
        TYPE_PLAN_POI_COPY,
        TYPE_PLAN_VOI,
        TYPE_PLAN_VOI_COPY,
        TYPE_POI,
        TYPE_PORTIMAGEBEAM,
        TYPE_PRESCRIPTION,
        TYPE_PROTOCOLTEMPLATE,
        TYPE_REPORTDOCUMENTS,
        TYPE_ROLE,
        TYPE_ROLEPERMISSION,
        TYPE_RTIMAGE,
        TYPE_SERIES,
        TYPE_SERIESGROUP,
        TYPE_SERIESTABLE,
        TYPE_SETUP,
        TYPE_SETUPOFFSETRECORD,
        TYPE_SHAPEELEMENT,
        TYPE_SITE,
        TYPE_SITE_IMMOBILIZATION,
        TYPE_SITETEMPLATE,
        TYPE_STAGE,
        TYPE_STEP,
        TYPE_STRUCTURESET,
        TYPE_STUDY,
        TYPE_SYSTEMDEFAULTCONFIG,
        TYPE_TABLECONTOUR,
        TYPE_TCP,
        TYPE_TEMPLATE,
        TYPE_TOLERANCE,
        TYPE_TRAY,
        TYPE_TREATMENTBEAM,
        TYPE_TREATMENTBEAM_ACCESSORY,
        TYPE_TREATMENTBEAM_POI,
        TYPE_TREATMENTBEAM_VOI,
        TYPE_TREATMENTSESSION,
        TYPE_USER,
        TYPE_USER_CLINIC,
        TYPE_USER_ROLE,
        TYPE_USERCONFIG,
        TYPE_USERSESSION,
        TYPE_VOI,
        TYPE_VOISETTING,
        TYPE_WEDGEFACTOR,
        TYPE_ASOVERRIDE,
        TYPE_ACHINESESSIONVIEW,
    }OBJECT_TYPE;

    static const char * ENUM_TO_STRING_OBJECT_TYPE[] = 
    {
        "TMSACCESSORY",
        "TMSACCESSORYCHUNK",
        "TMSAPPLICATOR",
        "TMSAPPROVERECORD",
        "TMSBEAM",
        "TMSBEAM_ACCESSORY",
        "TMSBEAM_POI",
        "TMSBEAM_SESSION",
        "TMSBEAM_VOI",
        "TMSBEAMRECORD",
        "TMSBEAMSEGMENT",
        "TMSBIOROLE",
        "TMSBLOCK",
        "TMSCBCTINFO",
        "TMSCHECKSUM",
        "TMSCLINICAL",
        "TMSCOMMISSIONEDUNIT",
        "TMSCOMMISSIONEDUNIT_TRAY",
        "TMSCONSTRAINT",
        "TMSCONSTRAINTTEMPLATE",
        "TMSCONTOUR",
        "TMSCONTROLPOINT",
        "TMSCONTROLPOINTRECORD",
        "TMSCOURSE",
        "TMSCT2DENSITY",
        "TMSCURVERESULANANLYSE",
        "TMSDBVERSION",
        "TMSDELIVERYBEAMGROUP",
        "TMSDELIVERYBEAMGROUP_POI",
        "TMSDELIVERYBEAMGROUP_SESSION",
        "TMSDELIVERYBEAMGROUP_VOI",
        "TMSDOSEGRID",
        "TMSDOSELINE",
        "TMSDOSEOPTENDCRITERIONBYINTERATION",
        "TMSDOSEOPTENDCRITERIONBYTIME",
        "TMSDOSEOPTGOALBASE",
        "TMSDRRSETTING",
        "TMSDRYRUN",
        "TMSEQUIPMENT",
        "TMSEVENTRECORD",
        "TMSFBCT",
        "TMSFBCT_SESSION",
        "TMSGATINGCHANGEHISTORY",
        "TMSGLOBALCONFIG",
        "TMSGOLDENSTT",
        "TMSHOLIDAY",
        "TMSICD",
        "TMSICD_SITE",
        "TMSIMAGE",
        "TMSIMAGEBEAM",
        "TMSIMAGEBEAM_SESSION",
        "TMSIMAGETRANSFORMATION",
        "TMSIMMOBILIZATION",
        "TMSKERNELDATA",
        "TMSKERNELDATALIST",
        "TMSKERNELDATALIST_KERNELDATA",
        "TMSKERNELWEIGHT",
        "TMSLOCK",
        "TMSMACHINE",
        "TMSMACHINE_CLINICAL",
        "TMSMACHINEQA",
        "TMSMC3DDOSE",
        "TMSMCELECTRONMONOENERGYWEIGHT",
        "TMSMCPHOCOMMISSIONEDUNIT",
        "TMSMCPHOMONOENERGYWEIGHT",
        "TMSMEASUREDDATA",
        "TMSMEASUREDPROFILE",
        "TMSMESSAGE",
        "TMSMESSAGE_RECEIVER",
        "TMSMLCSHAPE",
        "TMSMONOCOMPRESSEDPHASESPACEDATA",
        "TMSMONOPHASESPACEDATA",
        "TMSNORM_POI",
        "TMSNORM_VOI",
        "TMSNORMGROUP",
        "TMSNORMGROUP_VOI",
        "TMSNTCP",
        "TMSOPERATIONRECORD",
        "TMSPATIENT",
        "TMSPATIENTIMAGE",
        "TMSPATIENTPOSITIONIMAGE",
        "TMSPHASESPACEDATA",
        "TMSPLAN",
        "TMSPLAN_POI",
        "TMSPLAN_POI_COPY",
        "TMSPLAN_VOI",
        "TMSPLAN_VOI_COPY",
        "TPSPOI",
        "TMSPORTIMAGEBEAM",
        "TMSPRESCRIPTION",
        "TMSPROTOCOLTEMPLATE",
        "TMSREPORTDOCUMENTS",
        "TMSROLE",
        "TMSROLEPERMISSION",
        "TMSRTIMAGE",
        "TMSSERIES",
        "TMSSERIESGROUP",
        "TMSSERIESTABLE",
        "TMSSETUP",
        "TMSSETUPOFFSETRECORD",
        "TMSSHAPEELEMENT",
        "TMSSITE",
        "TMSSITE_IMMOBILIZATION",
        "TMSSITETEMPLATE",
        "TMSSTAGE",
        "TMSSTEP",
        "TMSSTRUCTURESET",
        "TMSSTUDY",
        "TMSSYSTEMDEFAULTCONFIG",
        "TMSTABLECONTOUR",
        "TMSTCP",
        "TMSTEMPLATE",
        "TMSTOLERANCE",
        "TMSTRAY",
        "TMSTREATMENTBEAM",
        "TMSTREATMENTBEAM_ACCESSORY",
        "TMSTREATMENTBEAM_POI",
        "TMSTREATMENTBEAM_VOI",
        "TMSTREATMENTSESSION",
        "TMSUSER",
        "TMSUSER_CLINIC",
        "TMSUSER_ROLE",
        "TMSUSERCONFIG",
        "TMSUSERSESSION",
        "TMSVOI",
        "TMSVOISETTING",
        "TMSWEDGEFACTOR",
        "TMSASOVERRIDE",
        "TMSACHINESESSIONVIEW",
};


typedef enum
    {
        POI_UNKNOWN_TYPE = -1,
        Setup = 1,//拍CT时病人位置的点
        NormalPoint,//归一化点
        IsoCenter,//等中心点
        Calculation,//TBD
        Max//TBD
    }POI_TYPE;

    typedef enum
    {
        FIXATIONDEVICE_UNKNOWN_TYPE = -1,
        BITEBLOCK = 1,
        HEADFRAME,
        MASK,
        MOLD,
        CAST,
        HEADREST,
        BREAST_BOARD,
        BODY_FRAME,
        VACUUM_MOLD,
        WHOLE_BODY_POD,
        RECTAL_BALLOON
    }FIXATIONDEVICE_TYPE;

    typedef enum
    {
        RADIATION_UNKNOWN_TYPE = -1,
        RADIATION_ELETRON = 1,
        RADIATION_NEUTRON,
        RADIATION_PROTON,
        RADIATION_PHOTON
    }RADIATION_TYPE;

    //个位表示MLC, 十位表示Gantry
    //0表示没有movement， 1表示有movement
    typedef enum
    {
        BEAM_UNKNOWN_TYPE = -1,
        STATICIMRT = 1,
        DYNAMICIMRT,
        BURST_ARC,
        VMAT    //CONTINOUS_ARC
    }BEAM_TYPE;

    typedef enum
    {
        CBCT_BEAM =0,
        EPID_BEAM
    }CBCTEPID_TYPE;

    typedef enum
    {
        NOT_BEAM_TYPE = -1,
        TREATMENT_BEAM = 1, //Treatment Beam no image
        //DRR_IMAGE_BEAM, //Duplicated with EPID_IMAGE_BEAM
        CBCT_IMAGE_BEAM, //PV Beam
        EPID_IMAGE_BEAM,//PV Beam
        FBCT_IMAGE_BEAM,//PV Beam
        PORT_PRE_TREATMENT_BEAM, //Treatment Beam with image
        PORT_POST_TREATMENT_BEAM, //Treatment Beam with image
        PORT_DURING_TREATMENT_BEAM, //Treatment Beam with image
        QA_3D_IMAGE_BEAM,
        CROSSHAIR_QA_IMAGE_BEAM,
        QA_2D_IMAGE_BEAM
    }BEAM_DELIVERY_TYPE;

    typedef enum
    {
        PORT_UNKNOWN_BEAM = -1,
        PORT_PRE_BEAM = 0,
        PORT_POST_BEAM,
        PORT_DURING_BEAM
    }PORTIMAGE_TYPE;

    typedef enum
    {
        QA_UNKNOWN_TYPE = -1,
        QA_2D = 0,
        QA_3D,
        Panel_Alignment
    }QA_TYPE;

    typedef enum
    {
        PLAN_UNKNOWN_TYPE = -1,
        PLAN_2D = 1,
        PLAN_3D_CRT,
        PLAN_Step_Shoot,
        PLAN_Sliding_Window,
        PLAN_Simple_Arc,
        PLAN_Conformal_Arc,
        PLAN_VMAT_Arc,
        PLAN_Burst_Arc,
        //Delete by qiangqiang's suggestion on 20150402
        //PLAN_SAD,    //add for easy plan 
        //PLAN_SSD,    //add for easy plan 
    }PLAN_TYPE;

    typedef enum  {
        TreatmentPlan=0,
        QAPlan,
        EasyPlan,
        MergedPlan,
    }PLAN_SOURCE_TYPE;

    typedef enum
    {
        IMAGE_UNKNOWN_TYPE = -1,
        DRR = 1,
        PORTAL,//验证beam子野
        SIMULATOR,
        RADIOGAPH,
        BLANK,
        FLUENCE,
        EPID
        //CBCT,
        //PLANNING_CT,//Add by zhiyuan.liu's suggesstion on 20140626
        //FBCT//Add by baoqin on 20140911
    }RTIMAGE_TYPE;

    typedef enum
    {
        SERIES_IMAGE_UNKNOWN_TYPE = -1,
        CBCT,
        PLANNING_CT,//Add by zhiyuan.liu's suggesstion on 20140626
        FBCT//Add by baoqin on 20140911
    }SERIES_IMAGE_TYPE;

    // type of tissue / Organ
    //typedef enum 
    //{
    //    BIOLOGICALROLE_UNKNOWN_TYPE,
    //    LUNG,
    //    BREAST,
    //    LIVER,
    //    BLADDER,
    //    RECTUM,
    //    HEART
    //}RTDB_BIOLOGICALROLE_TYPE;

    typedef enum 
    {
        ORGAN_UNKNOWN_TYPE,
        WHOLEORGAN,
        EXTENDEDORGAN, // = ORGAN WITH MARGINS?,
        PARTOFORGAN,
        PARTOFPAIREDORGAN // (RIGHT AND LEFT)
    }ORGAN_TYPE;

    typedef enum 
    { 
        PHYSICALROLE_UNKNOWN_TYPE,
        PROTHETISCH, //fake body
        BOLUS,
        WEDGE
    }PHYSICALROLE_TYPE;

    typedef enum 
    {
        TABLEROTATION_UNKNOWN_TYPE = -1,
        TABLEROT_ECCENTRIC = 1,
        TABLEROT_ISOCENTRIC
    }TABLEROTATION_TYPE;

    typedef enum 
    {
        MLC_LEAF_DIRECTION_UNKNOWN_TYPE = -1,
        MLCX_DIRECTION = 1,
        MLCY_DIRECTION,
    }MLC_LEAF_DIRECTION_TYPE;

    typedef enum
    {
        DISPLAYMODE_UNKNOWN_TYPE = -1,
        Relative = 0,
        Absolute
    }DISPLAYMODE_TYPE;

    typedef enum
    {
        HundredPercentDefinition_UNKNOWN_TYPE = -1,
        TotalGoals=0,
        MaxDose,
        SpecifiedDose
    }HundredPercentDefinition_TYPE;

    typedef enum
    {
        RENDERMODE_UNKNOWN_TYPE = -1,
        Line = 0,
        Colorwash = 1,
        Both
    }RENDERMODE_TYPE;

    typedef enum
    {
        DOSEALGORITHM_UNKNOWN_TYPE = -1,
        PENCILBEAM = 1,
        CONVOLUTION,
        MONTECARLO,
        ELECTRON
    }DOSEALGORITHM_TYPE;

    typedef enum
    {
        PHYSICALWEDGEDATATYPE_UNKNOWN_TYPE = -1,
        PHYSICALWEDGE_WEDGE = 1,
        PHYSICALWEDGE_CARRIER
    }PHYSICALWEDGEDATATYPE_TYPE;

    typedef enum
    {
        APPROVALSTATUS_UNKNOWN_TYPE = -1,
        DISAPPROVED = 0,
        APPROVED = 1
    }APPROVALSTATUS;


    typedef enum
    {
        ROTATION_DIRECTION_UNKNOWN_TYPE = -1,
        NONE_ROTATION_DIRECTION = 0,
        CC = 1,  // counter-clockwise
        CW = 2 // clockwise
    }ROTATION_DIRECTION;

    typedef enum 
    {
        UNLOCK = 1,
        NORMALLOCK,
        P_LOCK,
    }RTDB_VOI_LOCKTYPE;

    typedef enum SessionStatus
    {
        Unknown = -1,
        Completed = 1,
        Uncompleted,
        Stop
    }SESSION_STATUS;

    //typedef enum
    //{
    //    UNKNOWN_SHAPE_TYPE = -1,
    //    LINE = 1,
    //    RECTANGLE,
    //    FREELINE
    //}SHAPE_TYPE;

    typedef enum 
    {
        UNKNOWN_VOI_ROLE = -1,
        GTV_VOI_ROLE = 1,
        CTV_VOI_ROLE,
        PTV_VOI_ROLE,
        OAR_VOI_ROLE,
        TARGET_VOI_ROLE,
        NORMALTISSUE_VOI_ROLE,
        SKIN_VOI_ROLE
    }VOIROLE;

    ///////////////////////////////
    // The enum declaration for planning roles
    ///////////////////////////////
    typedef enum  PLANNING_ROLE_TYPE{
        PLANNING_ROLE_UNKNOWN = -1,
        PLANNING_ROLE_EXTERNAL,           //external patient contour
        PLANNING_ROLE_PTV,                //Planning Target Volume (as defined in ICRU50)
        PLANNING_ROLE_CTV,                //Clinical Target Volume (as defined in ICRU50)
        PLANNING_ROLE_GTV,                //Gross Tumor Volume (as defined in ICRU50)
        PLANNING_ROLE_ITV,                //NOT in sup11 Internal Target Volume:  expansion of CTV for internal (e.g. breathing) movement
        PLANNING_ROLE_XTV,                //NOT in sup11 alias CONTROL eg. 3mm,10mm,13mm around PTV
        PLANNING_ROLE_TREATED_VOLUME,     //Treated Volume(as defined in ICRU50)
        PLANNING_ROLE_IRRAD_VOLUME,       //Irradiated Volume(as defined in ICRU50)
        PLANNING_ROLE_BOLUS,              //patient bolus to be used for external beam therapy
        PLANNING_ROLE_AVOIDANCE,          //region in which dose is to be minimized
        PLANNING_ROLE_ORGAN ,             //patient organ
        PLANNING_ROLE_MARKER,             //patient marker
        PLANNING_ROLE_REGISTRATION ,      //registration ROI
        PLANNING_ROLE_ISOCENTER,          //treatment isocenter to be used for external beam therapy
        PLANNING_ROLE_CONTRAST_AGENT,     //volume into which a contrast agent has been injected
        PLANNING_ROLE_CAVITY ,            //patient anatomical cavity
        PLANNING_ROLE_BRACHY_CHANNEL,     //brachytherapy channel
        PLANNING_ROLE_BRACHY_ACCESSORY,   //brachytherapy accessory device
        PLANNING_ROLE_BRACHY_SRC_APP ,    //brachytherapy source applicator
        PLANNING_ROLE_BRACHY_CHNL_SHLD,   //brachytherapy channel shield
        PLANNING_ROLE_NO_DEFAULT,         //NOT in sup11 Dental Filling
        PLANNING_ROLE_SUPPORT,            //NOT in sup11 UIH table
        PLANNING_ROLE_OTHER,               //NOT in sup11 Region of interest used to fuse two image series
		PLANNING_ROLE_CONTROL,
    } PLANNING_ROLE_TYPE;

    //typedef enum SUBROLE_TYPE
    //{
    //    SUBROLE_UNKNOWN = -1,
    //    SUBROLE_SKIN,// means “skin or body”
    //    SUBROLE_RECTUM,
    //    SUBROLE_VEIN,
    //    SUBROLE_MUSCLE,
    //    SUBROLE_HEART,
    //    SUBROLE_BLADDER,
    //    SUBROLE_NERVE,
    //    SUBROLE_BRAIN,
    //    SUBROLE_BRAINSTEM,
    //    SUBROLE_BREAST ,
    //    SUBROLE_CARTILAGE,
    //    SUBROLE_TONGUE,
    //    SUBROLE_BOWEL,
    //    SUBROLE_EYE,
    //    SUBROLE_ESOPHAGUS,
    //    SUBROLE_BONE,
    //    SUBROLE_GLOTTIS,
    //    SUBROLE_KIDNEY,
    //    SUBROLE_LIPS,
    //    SUBROLE_LIVER,
    //    SUBROLE_LUNG,
    //    SUBROLE_ORAL,
    //    SUBROLE_OVARY,
    //    SUBROLE_PANCREAS,
    //    SUBROLE_PAROTID,
    //    SUBROLE_PENILEBULB,
    //    SUBROLE_EXTERNALIA,
    //    SUBROLE_PITUITARY,
    //    SUBROLE_PROSTATE,
    //    SUBROLE_CORD,
    //    SUBROLE_SPLEEN,
    //    SUBROLE_STOMACH,
    //    SUBROLE_TESTES,
    //    SUBROLE_THYROID,
    //    SUBROLE_THYMUS,
    //    SUBROLE_FIDUCIALS,
    //    SUBROLE_SEEDS,
    //    SUBROLE_METAL_FILLING,
    //    SUBROLE_NO_DEFAULT,
    //    SUBROLE_LYMPH,
    //    SUBROLE_TABLE,
    //    SUBROLE_OTHER
    //}SUBROLE_TYPE;

	typedef enum BIOLOGYROLE_TYPE
	{
		BIOLOGYROLE_UNKNOWN = -1,
		BIOLOGYROLE_NERVE,// MEANS “SKIN OR BODY”
		BIOLOGYROLE_SKIN,
		BIOLOGYROLE_ESOPHAGUS,
		BIOLOGYROLE_EYE,
		BIOLOGYROLE_BONE,
		BIOLOGYROLE_BRAIN,
		BIOLOGYROLE_TBD,
		BIOLOGYROLE_VEIN = 7,
		BIOLOGYROLE_HEART,
		BIOLOGYROLE_KIDNEY,
		BIOLOGYROLE_BOWEL,
		BIOLOGYROLE_LIPS,
		BIOLOGYROLE_LIVER,
		BIOLOGYROLE_LUNG,
		BIOLOGYROLE_MUSCEL,
		BIOLOGYROLE_CARTILAGE,
		BIOLOGYROLE_ORAL,
		BIOLOGYROLE_OVARY,
		BIOLOGYROLE_PANCREAS,
		BIOLOGYROLE_PAROTID,
		BIOLOGYROLE_PENILEBULB = 20,
		BIOLOGYROLE_EXTERNALIA,
		BIOLOGYROLE_PITUITARY,
		BIOLOGYROLE_PROSTATE,
		BIOLOGYROLE_RECTUM,
		BIOLOGYROLE_CORD = 25,
		BIOLOGYROLE_SPLEEN,
		BIOLOGYROLE_STOMACH,
		BIOLOGYROLE_TESTIS,
		BIOLOGYROLE_THYROID,
		BIOLOGYROLE_THYMUS = 30,
		BIOLOGYROLE_FIDUCIALS,
		BIOLOGYROLE_SEEDS,
		BIOLOGYROLE_METALFILLING,
		BIOLOGYROLE_NODEFAULT,
		BIOLOGYROLE_LYMPH = 35,
		BIOLOGYROLE_TABLE,
		BIOLOGYROLE_BOLUS,
		BIOLOGYROLE_OTHER,
		BIOLOGYROLE_BLADDER,
		BIOLOGYROLE_BRAINSTEM = 40,
		BIOLOGYROLE_BREAST,
		BIOLOGYROLE_TONGUE,
		BIOLOGYROLE_GLOTTIS,
	}BIOLOGYROLE_TYPE;

    typedef enum
    {
        CONVERSION_UNKNOWN_TYPE = -1,
        DV = 1,
        DI,
        DF,
        WSD
    }CONVERSION_TYPE;

    typedef enum
    {
        RTIMAGEPLANE_UNKNOWN_TYPE = -1,
        NORMAL = 1,
        NON_NORMAL
    }RTIMAGEPLANE_TYPE;

    typedef enum
    {
        PLAN_UNHNOWN_TYPE = -1,
        PLAN_ELECTRON = 1,
        PLAN_ELECTORN_2D
    }PLAN_ELECTORN_TYPE;

    typedef enum
    {
        CURVETYPE_PDD = 0,
        CURVETYPE_CROSSPLANE,
        CURVETYPE_INPLANE,
        CURVETYPE_DIAGONAL
    } CURVETYPE;

    typedef enum
    {
        FIELDTYPE_OPEN = 0,
        FIELDTYPE_WEDGE
    } FIELDTYPE;

    typedef enum 
    {
        NORMALIZEMETHOD_TONORMINALFIELD = 0,
        NORMALIZEMETHOD_TOTHISFIELD,
        NORMALIZEMETHOD_NONORMALIZE
    } NORMALIZEMETHOD;

    typedef enum
    {
        SERIESGROUP_UNKNOWN_TYPE = -1,
        SeriesGroup_4DCT = 1,
        SeriesGroup_PETCT
    }SERIESGROUP_TYPE;

    // 来自TDA的record 或者来自4DCT的plan
    typedef enum
    {
        FROM_TDA_RECORD = 1,
        FROM_4DCT_PLAN
    }WAVEDATA_SOURCE_TYPE;


    typedef enum DOSE_TYPE
    {
        DOSETYPE_UNKNOWN_TYPE = -1,
        DoseType_Max = 1,
        DoseType_Mean,
        DoseType_Min
    }DOSE_TYPE;

    //来自Beam下需要添加ApertureType（int）字段
    typedef enum APERTURE_TYPE
    {
        APERTURE_UNKNOWN_TYPE = -1,
        Mlc = 0,
        JawOnly = 1,
        Applicator = 2,
        Aperture_Block = 3
    }APERTURE_TYPE;

    typedef enum EASYPLAN_APERTURE_TYPE
    {
        EasyPlan_MLC,
        EasyPlan_Aperture_Block,
        EasyPlan_Shield_Block,
    }EASYPLAN_APERTURE_TYPE;

    typedef enum
    {
        DOSEOPTGOALTYPE_UNKNOWN_TYPE= -1,
        MAX_DOSE = 1,
        MIN_DOSE = 2,
        USER_DVH = 3,
        EUD = 4,
        SINGLE_VALUE=5
    }DOSEOPTGOAL_TYPE;

    typedef enum 
    {
        WEDGE_ORIENTATION_UNKNOWN = -1,
        TOE_IN = 1,//Modified by Zhiyuan.Liu
        TOE_OUT,
        TOE_LEFT,
        TOE_RIGHT
    }WEDGE_ORIENTATION;

    typedef enum 
    {
        WEDGE_TYPE_UNKNOWN = -1,
        PHYSICAL_WEDGE = 1,
        VIRTUAL_WEDGE
    }WEDGE_TYPE;

    typedef enum
    {
        OPTENDCRITERIONTYPE_UNKNOW_TYPE=-1,
        BY_TIME = 1,
        BY_ITERATION =2
    }OPTENDCRITERIONTYPE;

    //typedef enum 
    //{
    //    UNKNOWN_BEAMSESSIONSTATUS = -1,
    //    UNCOMPLETED = 0,
    //    COMPLETED = 1, 
    //    STOP = 2, 
    //}BEAMSESSIONSTATUS;

    typedef enum
    {
        BEAM_OPERATE_NONE = 0,
        BEAM_OPERATE_DELETE,
        BEAM_OPERATE_MODIFY,
    }BEAM_OPERATE_TYPE;

    typedef enum
    {
        BEAM_DELETE_NONE = 0,
        RADIATION_CHANGE,
        MACHINE_CORE_PARAMETER_CHANGE,
        TECHNIQUE_SWITCH_ARCNONARC,
        COMMISSIONEDUNIT_NOT_MATCHED,
        BEAMANGLE_NOT_REASONABLE,
    }BEAM_DELETE_REASON_TYPE;


    typedef enum
    {
        BEAM_MODIFY_NONE = 0,
        COMMISSIONED_UNIT,
        DOSE_RATE,
        ALGORITHMTYPE,
        SEGMENT,
        ACCESSORY,
    }BEAM_MODIFY_TYPE;

    typedef enum
    {
        MC3DDOSE_UNKNOW_TYPE=-1,
        MC3DDOSE_MONOPHASESPACEDATA = 0, //when called by RTFWKDLDOMonoPhasespacedata
        MC3DDOSE_ELECTRONMONOENERGYWEIGHT, //when called by McElectronMonoEnergyWeight
        MC3DDOSE_MONOCOMPRESSEDPHASESPACEDATA, //when called by RTFWKDLDOMonoCompressedPhasespacedata，
        MC3DDOSE_MCPHOMONOENERGYWEIGHT, //when called by RTFWKDLDOMcPhoMonoEnergyWeight
    }MC3DDoseDataType;

    typedef enum 
    {
        ACCESSORY_UNKNOWN_TYPE = -1,
        ACCESSORY_PHYSICALWEDGE = 1,
        ACCESSORY_VIRTUALWEDGE,
        ACCESSORY_MOTORIZEDWEDGE,
        ACCESSORY_BLOCK,//RT-PH Add
        ACCESSORY_TRAY,//RT-PH Add
        ACCESSORY_IDEALCOMPENSATOR,//RT-PH Add
        ACCESSORY_Cone,
    }ACCESSORY_TYPE;

    typedef enum
    {
        //JAWTYPE_UNKNOW_TYPE=-1,
        JAWTYPE_Follow = 0,
        //JAWTYPE_Max,
        JAWTYPE_Manual
    }JAWTYPE;

    typedef enum
    {
        BLOCK_UNKNOW_TYPE=-1,
        SHIELDING = 0,
        APERTURE
    }BLOCK_TYPE;

    // Indicates presence or otherwise of geometrical divergence
    typedef enum
    {
        BLOCKDIVERGENCE_UNKNOW_TYPE=-1,
        PRESENT = 0,// block edges are shaped for beam divergence
        ABSENT// block edges are not shaped for beam divergence
    }BLOCK_DIVERGENCE;

    typedef enum
    {
        BLOCKMOUNTINGPOSITION_UNKNOW_TYPE = -1,
        PATIENT_SIDE = 0,
        SOURCE_SIDE
    }BLOCK_MOUNTING_POSITION;

    typedef enum
    {
        APPLICATOR_UNKNOWN_TYPE=-1,
        ELECTRON_SQUARE = 0,
        ELECTRON_RECT,
        ELECTRON_CIRC,
        ELECTRON_SHORT,
        ELECTRON_OPEN,
        PHOTON_SQUARE,
        PHOTON_RECT,
        PHOTON_CIRC,
        INTRAOPERATIVE,
        STEREOTACTIC
    }APPLICATOR_TYPE;

    typedef enum
    {
        BIOMODEL_UNKNOW_TYPE=-1,
        TCP = 0,
        NTCP
    }BIOMODEL_TYPE;

    typedef enum
    {
        APPLICATORAPERTURESHAPE_UNKNOW_TYPE = -1,
        SYM_SQUARE = 0,
        SYM_RECTANGLE,
        SYM_CIRCULAR
    }APPLICATOR_APERTURE_SHAPE;

    typedef enum
    {
        UnknowType = -1,
        Plan = 1,
        Normgroup = 2,
        VOI = 3,
        Commissionedunit = 4,
        Machine = 5,
        Imagetransformation = 6,
        TreatmentBeam = 7,
        Deliverybeamgroup = 8,
        PlanOrder = 9,
        Prescription = 10,
        SetupAndSimulationOrder = 11,
        CTtoDensityTable = 12,
    }CLASS_TYPE;

    enum PairedType
    {
        PairedType_Unknown = -1,
        PairedType_No,
        PairedType_Yes,
        PairedType_Left,
        PairedType_Right
    };

    enum BiologyRole
    {
        BiologyRole_Unknown = -1,
        BiologyRole_Nerve,
        BiologyRole_Skin,
        BiologyRole_Esophagus,
        BiologyRole_Eye,
        BiologyRole_Bone,
        BiologyRole_Brain,
        BiologyRole_TBD,
        BiologyRole_Vein = 7,
        BiologyRole_Heart,
        BiologyRole_Kidney,
        BiologyRole_Bowel,
        BiologyRole_Lips,
        BiologyRole_Liver,
        BiologyRole_Lung,
        BiologyRole_Muscel = 14,
        BiologyRole_Cartilage,
        BiologyRole_Oral,
        BiologyRole_Ovary,
        BiologyRole_Pancreas,
        BiologyRole_Parotid,
        BiologyRole_Penilebulb = 20,
        BiologyRole_Externalia,
        BiologyRole_Pituitary,
        BiologyRole_Prostate,
        BiologyRole_Rectum,
        BiologyRole_Cord = 25,
        BiologyRole_Spleen,
        BiologyRole_Stomach,
        BiologyRole_Testes,
        BiologyRole_Thyroid,
        BiologyRole_Thymus = 30,
        BiologyRole_Fiducials,
        BiologyRole_Seeds,
        BiologyRole_MetalFilling,
        BiologyRole_NoDefault,
        BiologyRole_Lymph = 35,
        BiologyRole_Table,
        BiologyRole_Bolus,
        BiologyRole_Other,
        BiologyRole_Bladder,
        BiologyRole_Brainstem = 40,
        BiologyRole_Breast,
        BiologyRole_Tongue
    };

    enum PatientPosition
    {
        HFS = 0,
        HFP = 1,
        FFS = 2,
        FFP = 3,
        HFDL = 4,
        HFDR = 5,
        FFDL = 6,
        FFDR = 7,
    };

    enum ContourGeometricType
    {
        ContourType_UnknowType = -1,
        ContourType_Point = 1,
        ContourType_Open_Planar,
        ContourType_Open_Nonplanar,
        ContourType_Closed_Planar
    };

    enum DoseCalculateMode
    {
        BASE_ON_MU = 0,
        BASE_ON_WEIGHT
    };

    enum PoiDoseCaclMethod
    {
        Calculated = 0,
        Interpolated,
        NotCalculated
    };

    struct PoiDoseInfo
    {
        PoiDoseCaclMethod doseCalcMethod;
        float dose;
    };
};

RT_TPS_DATABASE_END_NAMESPACE;
#endif 
