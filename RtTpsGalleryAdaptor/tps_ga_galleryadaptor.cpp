//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_ga_galleryadaptor.cpp
///  \brief   
///
///  \version 1.0
///  \date    Nov. 24, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "tps_ga_galleryadaptor.h"

//TPS
#include "RtTpsFramework/tps_fw_controllerbase.h"
#include "RtTpsFramework/tps_fw_commandids.h"

#include "tps_ga_createviewmodule_cmdhandler.h"
#include "tps_ga_loadseries_cmdhandler.h"
#include "tps_ga_mproperationcmdhandler.h"
#include "tps_ga_mouseposinfo_cmdhandler.h"
#include "tps_ga_crosshairmovement_cmdhandler.h"
#include "tps_ga_crosshairmode_cmdhandler.h"
#include "tps_ga_loadpatient_cmdhandler.h"
#include "tps_ga_measureangle_cmdhandler.h"
#include "tps_ga_measuredistance_cmdhandler.h"
#include "tps_ga_editcourse_cmdhandler.h"
#include "tps_ga_reloadpatient_cmdhandler.h"
#include "tps_ga_deleteseries_cmdhandler.h"
#include "tps_ga_imagetextvisible_cmdhandler.h"
#include "tps_ga_expandseries_cmdhandler.h"
#include "tps_ga_vrrotate_cmdhandler.h"
#include "tps_ga_getsliceinfo_cmdhandler.h"
#include "tps_ga_deletecurrentslice_cmdhandler.h"
#include "tps_ga_saveseries_cmdhandler.h"
#include "tps_ga_leavingmodule_cmdhandler.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

IMPLEMENT_APP_ADAPTOR(TpsGalleryAdaptor)

TpsGalleryAdaptor::TpsGalleryAdaptor(int id)
    : TpsAdaptorBase(id),
    mIGalleryController(NULL){

}


TpsGalleryAdaptor::~TpsGalleryAdaptor(void) {

}

int TpsGalleryAdaptor::Start(const std::vector<std::string> &args) {
    //Get IGalleryAdaptor
    mIGalleryController = static_cast<IGalleryController*> (this->GetController());

    if (mIGalleryController == nullptr) {
        TPSPRINTF("query IGalleryController failed!\n");
        return TPS_FAILURE;
    }

    if (TPS_FAILURE == TpsAdaptorBase::Start(args)){
        TPS_LOG_DEV_ERROR << "failed to start adapter";
        return TPS_FAILURE;
    }

    return TPS_SUCCESS;
}

int TpsGalleryAdaptor::End() {
    TpsAdaptorBase::End();

    return TPS_SUCCESS;
}

int TpsGalleryAdaptor::RegisterCommand() {

    Mcsf::ICommunicationProxy* mProxy = this->GetCommProxy();

    mProxy->RegisterCommandHandler(CREATE_VIEW_MODULE_GALLERY, 
        new CreateViewModuleGalleryCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(LOAD_SERIES_GALLERY, 
        new LoadSeriesCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(MPR_PAN_GALLERY, 
        new TpsGaMPRPanCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(MPR_ZOOM_GALLERY, 
        new TpsGaMPRZoomCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(MPR_WINDOWING_GALLERY, 
        new TpsGaMPRWindowingCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(MPR_PAGE_TURN_GALLERY, 
        new TpsGaMPRPageTurnCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(MOUSE_POSITION_CHANGE_GALLERY, 
        new TpsGaMousePosInfoCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(MOVE_CROSSHAIR_GALLERY, 
        new TpsGaCrosshairMovementCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(CROSSHAIR_MODE_GALLERY, 
        new TpsGaCrosshairModeCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(LOAD_PATIENT_GALLERY, 
        new TpsGaLoadPatientCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(MPR_ROTATE_GALLERY,
        new TpsGaMPRRotateCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(MEASUREANGLE_GALLERY,
        new TpsGaMeasureAngleCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(MEASUREDISTANCE_GALLERY,
        new TpsGaMeasureDistanceCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(EDIT_COURSE_GALLERY, 
        new TpsGaEditCourseCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(RELOAD_PATIENT_GALLERY, 
        new TpsGaReloadPatientCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(DELETE_SERIES_GALLERY, 
        new TpsGaDeleteSeriesCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(IMAGETEXT_VISIBLE_GALLERY, 
        new TpsGaImageTextVisibleCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(EXPAND_SERIES_GALLERY, 
        new TpsGaExpandSeriesCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(VR_ROTATE_GALLERY, 
        new TpsGaVRRotateCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(GET_SLICEINFO_GALLERY, 
        new TpsGaGetSliceInfoCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(DELETE_CURRENTSLICE_GALLERY, 
        new TpsGaDeleteCurrentSliceCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(SAVE_SERIES_GALLERY, 
        new TpsGaSaveSeriesCmdHandler(mIGalleryController));

    mProxy->RegisterCommandHandler(LEAVING_MODULE_GALLERY, 
        new TpsGaLeavingModuleCmdHandler(mIGalleryController));

    return TPS_SUCCESS;
}


TPS_END_NAMESPACE
