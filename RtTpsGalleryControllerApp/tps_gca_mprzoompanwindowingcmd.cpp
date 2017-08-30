//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_mprzoompanwindowingcmd.cpp
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "tps_gca_mprzoompanwindowingcmd.h"

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
#include "RtTpsRenderLibrary/tps_rl_gallery_mprzoompanwindowing_operation.h"
#include "RtTpsRenderController/tps_rc_gallery_mprrendercontroller.h"

TPS_BEGIN_NAMESPACE

extern bool SelectRenderProxyViaLayout(TpsRenderProxyBase* renderProxy, void* para);

TpsGcaMPRPanCmd::TpsGcaMPRPanCmd(LAYOUT_UNIT unit, float startx, float starty, 
    float stopx, float stopy) 
    : mLayoutUnit(unit), mStartX(startx),mStartY(starty), 
    mStopX(stopx),mStopY(stopy){
}


TpsGcaMPRPanCmd::~TpsGcaMPRPanCmd(void) {
}

TpsGcaMPRPanCmd *TpsGcaMPRPanCmd::Clone() {
    TpsGcaMPRPanCmd *cmd = new TpsGcaMPRPanCmd(mLayoutUnit, mStartX,mStartY,mStopX,mStopY);
    return cmd;
}

bool TpsGcaMPRPanCmd::PreExecute() {
    return true;
}

bool TpsGcaMPRPanCmd::Execute() {

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

        //2. get section type
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

        // 2. execute the MPR Pan command
        std::shared_ptr<TpsGalleryMPRPanOperation> 
            panOperation(new TpsGalleryMPRPanOperation(dispsize.width, dispsize.height, mStartX,mStartY,mStopX,mStopY));

        std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
        auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section);
        std::string sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
        panOperation->SetGraphicObjectKeys(sVolumeKey,sMPRKey);
        ModelWarehouse::GetInstance()->ProcessOperation(panOperation);

        // 3. fire to render
        std::shared_ptr<TpsFireOperation> fire(new TpsFireOperation());
        ModelWarehouse::GetInstance()->ProcessOperation(fire);
    }
    catch (std::exception& ex){
        TPS_LOG_DEV_ERROR<<ex.what();
        return false;
    }

    return true;
}

bool TpsGcaMPRPanCmd::PostExecute() {
    return true;
}

TpsGcaMPRZoomCmd::TpsGcaMPRZoomCmd(LAYOUT_UNIT unit, float scaleFactor) 
    : mLayoutUnit(unit), mScaleFactor(scaleFactor){
        mStartX = mStartY = 0;
        mStopX = mStopY = 1;
        mZoomInputMode = BY_FACTOR;
}

TpsGcaMPRZoomCmd::TpsGcaMPRZoomCmd(LAYOUT_UNIT unit, float startX, float startY, 
    float stopX, float stopY) 
    : mLayoutUnit(unit), mStartX(startX), mStartY(startY), 
    mStopX(stopX), mStopY(stopY){
        mScaleFactor = 1.0;
        mZoomInputMode = BY_VECTOR;
}

TpsGcaMPRZoomCmd::~TpsGcaMPRZoomCmd(void) {
}

TpsGcaMPRZoomCmd *TpsGcaMPRZoomCmd::Clone() {
    TpsGcaMPRZoomCmd *cmd = new TpsGcaMPRZoomCmd(mLayoutUnit, mScaleFactor);
    return cmd;
}

bool TpsGcaMPRZoomCmd::PreExecute() {
    return true;
}

bool TpsGcaMPRZoomCmd::Execute() {

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

        //2. get section type
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
        // 2. execute the MPR Pan command
        std::shared_ptr<TpsGalleryMPRZoomOperation> zoomOperation;
        switch (mZoomInputMode) {
        case BY_FACTOR: {
            zoomOperation.reset(new TpsGalleryMPRZoomOperation(mScaleFactor));
                        }
                        break;
        case BY_VECTOR: {
            zoomOperation.reset(new TpsGalleryMPRZoomOperation(mStartX,mStartY,mStopX,mStopY));
                        }
                        break;
        default:
            return false;
        }

        std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
        auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section);
        std::string sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
        goType = GOTypeHelper::ComposeAsGOType(MPR_RULER_TEXT, section);
        std::string sMPRRulerTextKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
        zoomOperation->SetGraphicObjectKeys(sVolumeKey,sMPRKey, sMPRRulerTextKey);
        ModelWarehouse::GetInstance()->ProcessOperation(zoomOperation);

        // 3. fire to render
        std::shared_ptr<TpsFireOperation> fire(new TpsFireOperation());
        ModelWarehouse::GetInstance()->ProcessOperation(fire);
    }
    catch (std::exception& ex){
        TPS_LOG_DEV_ERROR<<ex.what();
        return false;
    }

    return true;
}

bool TpsGcaMPRZoomCmd::PostExecute() {
    return true;
}


TpsGcaMPRWindowingCmd::TpsGcaMPRWindowingCmd(LAYOUT_UNIT unit, float deltaWidth, float deltaCenter, bool updateAll) 
    : mLayoutUnit(unit), mDeltaWidth(deltaWidth), mDeltaCenter(deltaCenter), 
    mUpdateAll(updateAll){
}

TpsGcaMPRWindowingCmd::~TpsGcaMPRWindowingCmd(void) {
}

TpsGcaMPRWindowingCmd *TpsGcaMPRWindowingCmd::Clone() {
    TpsGcaMPRWindowingCmd *cmd = new TpsGcaMPRWindowingCmd(mLayoutUnit, mDeltaWidth, mDeltaCenter, mUpdateAll);
    return cmd;
}

bool TpsGcaMPRWindowingCmd::PreExecute() {
    return true;
}

bool TpsGcaMPRWindowingCmd::Execute() {

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

        //2. get section type
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
        // 2. execute the MPR Pan command
        std::shared_ptr<TpsGalleryMPRWindowingOperation> 
            windowingOperation(new TpsGalleryMPRWindowingOperation(mDeltaWidth, mDeltaCenter));

        std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
        auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section);
        std::string sCurrMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
        goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR_CORNER_INFO, section);
        std::string sCurrMPRCornerInfoKey = seriesUID + "|" + GOTypeHelper::ToString(goType);

        std::string sMPRKey, sMPRCornerInfoKey;
        std::vector<std::string> vecLinkedMprKey, vecLinkedMprCornerInfoKey;
        if (mUpdateAll){
            FIRST_POSTFIX_COMPONENT sections[3] = { AXIAL, CORONAL, SAGITTAL};
            for (int i = 0; i < 3; ++i){
                if (sections[i] == section) continue;
                goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, sections[i]);
                sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
                goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR_CORNER_INFO, sections[i]);
                sMPRCornerInfoKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
                vecLinkedMprKey.push_back(sMPRKey);
                vecLinkedMprCornerInfoKey.push_back(sMPRCornerInfoKey);
            }
        }
        windowingOperation->SetGraphicObjectKeys(sVolumeKey, sCurrMPRKey, 
            sCurrMPRCornerInfoKey, vecLinkedMprKey, vecLinkedMprCornerInfoKey);
        ModelWarehouse::GetInstance()->ProcessOperation(windowingOperation);

        // 3. fire to render
        std::shared_ptr<TpsFireOperation> fire(new TpsFireOperation());
        ModelWarehouse::GetInstance()->ProcessOperation(fire);
    }
    catch (std::exception& ex){
        TPS_LOG_DEV_ERROR<<ex.what();
        return false;
    }

    return true;
}

bool TpsGcaMPRWindowingCmd::PostExecute() {
    return true;
}
TPS_END_NAMESPACE
