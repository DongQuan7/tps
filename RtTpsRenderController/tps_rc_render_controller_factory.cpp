////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rc_rendercontrollerfactory.cpp
/// 
///  \brief class TpsRenderControllerFactory implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/12
////////////////////////////////////////////////////////////////
#include"StdAfx.h"

#include "RtTpsRenderController/tps_rc_rendercontrollerfactory.h"

//all render controller include
#include "RtTpsRenderController/tps_rc_mpr_render_controller.h"
#include "RtTpsRenderController/tps_rc_mpr_render_controller_axial.h"
#include "RtTpsRenderController/tps_rc_admin_mpr_render_controller.h"
#include "RtTpsRenderController/tps_rc_planQA_mpr_render_controller.h"
#include "RtTpsRenderController/tps_rc_planQA_mpr_render_controller_axial.h"
#include "RtTpsRenderController/tps_rc_common_fusion_mpr_render_controller.h"
#include "RtTpsRenderController/tps_rc_bevrendercontroller.h"
#include "RtTpsRenderController/tps_rc_shadedsurfacerendercontroller.h"
#include "RtTpsRenderController/tps_rc_fusion_render_controller.h"
#include "RtTpsRenderController/tps_rc_voi_preview_rendercontroller.h"
#include "RtTpsRenderController/tps_rc_common_mpr_render_controller.h"
#include "RtTpsRenderController/tps_rc_beam_preview_rendercontroller.h"
#include "RtTpsRenderController/tps_rc_mlcrendercontroller.h"

#include "RtTpsRenderController/tps_rc_gallery_mprrendercontroller.h"
#include "RtTpsRenderController/tps_rc_gallery_shadedsurfacerendercontroller.h"


#include "RtTpsRenderController/tps_rc_ut_rendercontroller.h"               // Only for Unit Test Use

#include "tps_logger.h"
#include "RtTpsRenderController/tps_rc_easyplan_image_rendercontroller.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

IMPLEMENT_SINGLETON(TpsRenderControllerFactory)

TpsRenderControllerFactory::TpsRenderControllerFactory(){

}

TpsRenderControllerBase* 
TpsRenderControllerFactory::CreateRenderController(WINDOW_TYPE type)
{
    switch(type){
        case WINDOW_COMMON_BEV:
            return new TpsBEVRenderController(type);
        case WINDOW_COMMON_SHADED_SURFACE:
        case WINDOW_PLANQA_SHADED_SURFACE:
            return new ShadedSurfaceRenderController(type);
        case WINDOW_COMMON_MPR_CORONAL:
        case WINDOW_COMMON_MPR_SAGITTAL:
        case WINDOW_EV_1_MPR_CORONAL:
        case WINDOW_EV_1_MPR_SAGITTAL:
        case WINDOW_EV_1_MPR_AXIAL:
        case WINDOW_EV_2_MPR_CORONAL:
        case WINDOW_EV_2_MPR_SAGITTAL:
        case WINDOW_EV_2_MPR_AXIAL:
        case WINDOW_EV_RESULT_MPR_CORONAL:
        case WINDOW_EV_RESULT_MPR_SAGITTAL:
        case WINDOW_EV_RESULT_MPR_AXIAL:
            return new TpsCommonMPRRenderController(type);
        case WINDOW_COMMON_MPR_AXIAL:
            return new TpsMprRenderControllerAxial(type);
        case WINDOW_ADMIN_MPR_AXIAL:
        case WINDOW_ADMIN_MPR_CORONAL:
        case WINDOW_ADMIN_MPR_SAGITTAL:
            return new TpsAdminMPRRenderController(type);
        case WINDOW_PLANQA_MPR_CORONAL:
        case WINDOW_PLANQA_MPR_SAGITTAL:
            return new TpsPlanQAMPRRenderController(type);
        case WINDOW_PLANQA_MPR_AXIAL:
            return new TpsPlanQAMprRenderControllerAxial(type);
        case WINDOW_FUSION_FIXED_MPR_CORONAL:
        case WINDOW_FUSION_FIXED_MPR_SAGITTAL:
        case WINDOW_FUSION_FIXED_MPR_AXIAL:
        case WINDOW_FUSION_FLOAT_MPR_CORONAL:
        case WINDOW_FUSION_FLOAT_MPR_SAGITTAL:
        case WINDOW_FUSION_FLOAT_MPR_AXIAL:
            return new TpsCommonFusionMPRRenderController(type);
        case WINDOW_FUSION_MIXED_MPR_CORONAL:
        case WINDOW_FUSION_MIXED_MPR_SAGITTAL:
        case WINDOW_FUSION_MIXED_MPR_AXIAL:
            return new TpsFusionRenderController(type);
        case WINDOW_COMMON_VOI_PREVIEW_MPR_CORONAL:
        case WINDOW_COMMON_VOI_PREVIEW_MPR_SAGITTAL:
        case WINDOW_COMMON_VOI_PREVIEW_MPR_AXIAL:
            return new TpsVoiPreviewRenderController(type);
        case WINDOW_COMMON_BEAM_PREVIEW_MPR_AXIAL:
            return new TpsBeamPreviewRenderController(type);
        //easy plan
        case WINDOW_EZPLAN_IMAGE:
            return new TpsEasyPlanImageRenderController(type);
        case WINDOW_COMMON_SIMULATOR_MLC:
            return new TpsMLCRenderController(type);
        //gallery
        case WINDOW_GALLERY_MPR_AXIAL:
        case WINDOW_GALLERY_MPR_SAGITTAL:
        case WINDOW_GALLERY_MPR_CORONAL:
            return new TpsGalleryMPRRenderController(type);
        case WINDOW_GALLERY_3D:
            return new TpsGalleryShadedSurfaceRenderController(type);
        // for Unit Test Use only
        case WINDOW_UT_MPR_AXIAL:
        case WINDOW_UT_MPR_SAGITTAL:
        case WINDOW_UT_MPR_CORONAL:
            return new TpsUtRenderController(type);
        default:
            TPS_LOG_DEV_ERROR<<"[ERROR][TpsRenderControllerFactory][CreateRenderController]: "
                <<"Unsupported window type: "<<type;
            return nullptr;
    }
}

TPS_END_NAMESPACE  // end namespace tps
