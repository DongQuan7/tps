//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_mprpageturncmd.cpp
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tps_gca_MPRPageturnCmd.h"

//ZHENGHE
#include "McsfGeometry/mcsf_geometry_sub_volume.h"

//TPS
#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_rl_modelwarehouse.h"
#include "RtTpsFramework/tps_rl_fire_operation.h"  
#include "RtTpsFramework/tps_rl_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_rc_renderingcomponentrecorder.h"
#include "RtTpsFramework/tps_rc_renderproxybase.h"
#include "RtTpsFramework/tps_rc_layoutmanager.h"   
#include "RtTpsRenderLibrary/tps_rl_clearmprrendercacheoperation.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_mprpageturn_operation.h" 
#include "RtTpsRenderLibrary/tps_rl_updatecachecollectionoperation.h"
#include "RtTpsRenderController/tps_rc_gallery_mprrendercontroller.h"

TPS_BEGIN_NAMESPACE

bool SelectRenderProxyViaLayout(TpsRenderProxyBase* renderProxy, void* para){
    LAYOUT_UNIT* layout = (LAYOUT_UNIT*)para;
    WINDOW_TYPE windowType;
    return renderProxy->GetLayoutManager()->GetWindowtype(*layout, windowType);
}

TpsGcaMPRPageturnCmd::TpsGcaMPRPageturnCmd(LAYOUT_UNIT unit, 
    int iPageturn) 
    :   mUnit(unit), 
    mPageturn(iPageturn){
}


TpsGcaMPRPageturnCmd::~TpsGcaMPRPageturnCmd(void) {
}

TpsGcaMPRPageturnCmd *TpsGcaMPRPageturnCmd::Clone() {
    TpsGcaMPRPageturnCmd *cmd = new TpsGcaMPRPageturnCmd(mUnit, mPageturn);
    return cmd;
}

bool TpsGcaMPRPageturnCmd::PreExecute() {
    return true;
}

bool TpsGcaMPRPageturnCmd::Execute() {

    try{
        //1. get the series uid according to the layout information
        auto recorder = TpsRenderingComponentRecorder::GetInstance();
        TpsRenderProxyBase* renderProxy = recorder->SelectOne(
            (TpsRenderingComponentRecorder::Selector)SelectRenderProxyViaLayout, (void*)&mUnit);
        if(renderProxy == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to get related render proxy.";
            return false;
        }
        std::string seriesUID;
        if (!renderProxy->GetDataSource(mUnit, IMAGE, seriesUID)) {
            TPS_LOG_DEV_ERROR<<"Can not get the image data source of current layout: "
                <<"View Control ID: "<<mUnit.viewerControlID<<" Cell ID: "<<mUnit.cellID;
            return false;
        }

        //2. get section type
        WINDOW_TYPE window;
        if (!renderProxy->GetLayoutManager()->GetWindowtype(mUnit, window)){
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

        // 2. clear the MPR rendering cache
        std::shared_ptr<ClearMPRRenderCacheOperation> 
            clearoperation(new ClearMPRRenderCacheOperation(seriesUID, section));
        ModelWarehouse::GetInstance()->ProcessOperation(clearoperation);

        // 3. execute the MPR scrolling command
        std::shared_ptr<TpsGalleryMPRPageTurnOperation> 
            pageTurnoperation(new TpsGalleryMPRPageTurnOperation(mPageturn));
        std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
        auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section);
        std::string sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
        goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR_CORNER_INFO, section);
        std::string sMPRCornerInfoKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
        goType = GOTypeHelper::ComposeAsGOType(GALLERY_MEASURE, section);
        std::string sMeasureKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
        pageTurnoperation->SetGraphicObjectKeys(sVolumeKey, sMPRKey, sMPRCornerInfoKey, sMeasureKey);
        ModelWarehouse::GetInstance()->ProcessOperation(pageTurnoperation);

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

bool TpsGcaMPRPageturnCmd::PostExecute() {
    return true;
}

TPS_END_NAMESPACE
