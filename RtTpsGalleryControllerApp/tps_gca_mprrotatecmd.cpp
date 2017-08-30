//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
///
///  \file    tps_gca_mprrotatecmd.cpp
///  \brief   
///
///  \version 1.0
///  \date    Feb. 05, 2015
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "tps_gca_mprrotatecmd.h"

//ZHENGHE
#include "McsfGeometry/mcsf_geometry_sub_volume.h"

//TPS
#include "tps_logger.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_rl_modelwarehouse.h"
#include "RtTpsFramework/tps_rl_fire_operation.h"

#include "RtTpsFramework/tps_rl_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_rc_renderingcomponentrecorder.h"
#include "RtTpsFramework/tps_rc_renderproxybase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_rc_layoutmanager.h"
#include "tps_gca_mprpageturncmd.h" // define SelectedRenderProxyViaLayout
#include "RtTpsRenderLibrary/tps_rl_gallery_mprrotate_operation.h"
#include "RtTpsRenderController/tps_rc_gallery_mprrendercontroller.h"

TPS_BEGIN_NAMESPACE

extern bool SelectRenderProxyViaLayout(TpsRenderProxyBase* renderProxy, void* para);

TpsGcaMPRRotateCmd::TpsGcaMPRRotateCmd(const LAYOUT_UNIT &unit, const float startx, const float starty, 
    const float stopx, const float stopy) 
    : mLayoutUnit(unit), mStartX(startx),mStartY(starty), 
    mStopX(stopx),mStopY(stopy){}


TpsGcaMPRRotateCmd::~TpsGcaMPRRotateCmd(void) {}

TpsGcaMPRRotateCmd *TpsGcaMPRRotateCmd::Clone() 
{
    TpsGcaMPRRotateCmd *cmd = new TpsGcaMPRRotateCmd(mLayoutUnit, mStartX,mStartY,mStopX,mStopY);
    return cmd;
}

bool TpsGcaMPRRotateCmd::PreExecute() {
    return true;
}

bool TpsGcaMPRRotateCmd::Execute() {

    try{
        // 1. get the series uid according to the layout information
        auto recorder = TpsRenderingComponentRecorder::GetInstance();
        TpsRenderProxyBase* renderProxy = recorder->SelectOne(
            (TpsRenderingComponentRecorder::Selector)SelectRenderProxyViaLayout, (void*)&mLayoutUnit);
        if(renderProxy == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to get related render proxy.";
            return false;
        }
        std::string seriesUID;
        if (!renderProxy->GetDataSource(mLayoutUnit, IMAGE, seriesUID)) {
            TPS_LOG_DEV_ERROR<<"Can not get the image data source of current layout: "
                <<"View Control ID: "<<mLayoutUnit.viewerControlID<<" Cell ID: "<<mLayoutUnit.cellID;
            return false;
        }

        // 2. get section type
        WINDOW_TYPE window;
        if (!renderProxy->GetLayoutManager()->GetWindowtype(mLayoutUnit, window)){
            TPS_LOG_DEV_ERROR<<"Failed to get the window type.";
            return false;
        }
        auto renderController = renderProxy->GetRenderController(window);
        auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
        if (mprRc == nullptr){
            TPS_LOG_DEV_ERROR<<"The window is not a mpr window.";
            return false;
        }
        FIRST_POSTFIX_COMPONENT section = mprRc->GetSectionType();
        DISPLAY_SIZE dispsize;
        renderProxy->GetDisplaySize(mLayoutUnit, dispsize);
        if(dispsize.width <= 0 || dispsize.height <= 0){
            return false;
        }
        // 3. execute the MPR Rotate command
        std::shared_ptr<TpsGalleryMPRRotateOperation> 
            rotateOperation(new TpsGalleryMPRRotateOperation(dispsize.width, dispsize.height, mStartX, mStartY, mStopX, mStopY));

        std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
        auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section);
        std::string sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
        rotateOperation->SetGraphicObjectKeys(sVolumeKey,sMPRKey);
        ModelWarehouse::GetInstance()->ProcessOperation(rotateOperation);

        // 4. fire to render
        std::shared_ptr<TpsFireOperation> fire(new TpsFireOperation());
        ModelWarehouse::GetInstance()->ProcessOperation(fire);
    }
    catch (std::exception& ex){
        TPS_LOG_DEV_ERROR<<ex.what();
        return false;
    }

    return true;
}

bool TpsGcaMPRRotateCmd::PostExecute() {
    return true;
}

TPS_END_NAMESPACE