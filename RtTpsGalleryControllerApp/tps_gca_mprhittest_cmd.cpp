//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_mprpageturncmd.h
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tps_gca_mprhittest_cmd.h"

//TPS
#include "tps_logger.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_rl_modelwarehouse.h"
#include "RtTpsFramework/tps_rl_fire_operation.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_hittest_operation.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"
#include "RtTpsFramework/tps_rl_graphicobjecttypehelper.h"

#include "tps_gca_mprpageturncmd.h" // define SelectedRenderProxyViaLayout
#include "RtTpsFramework/tps_rc_renderingcomponentrecorder.h"
#include "RtTpsFramework/tps_rc_renderproxybase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_rc_layoutmanager.h"
#include "RtTpsRenderController/tps_rc_gallery_mprrendercontroller.h"

TPS_BEGIN_NAMESPACE

extern bool SelectRenderProxyViaLayout(TpsRenderProxyBase* renderProxy, void* para);

TpsGcaMPRHitTestCmd::TpsGcaMPRHitTestCmd(std::string& strImageUID, FIRST_POSTFIX_COMPONENT strSectionType, double x, double y)
:m_strImageUID(strImageUID), m_strSectionType(strSectionType),m_fX(x), m_fY(y), m_nHitTestResult(0){
}

TpsGcaMPRHitTestCmd::TpsGcaMPRHitTestCmd(const CertifiedMousePosInfo &protoMousePosInfo) : 
    //m_protoMousePosInfo(protoMousePosInfo), 
    m_nHitTestResult(0){

        LAYOUT_UNIT unit;
        unit.viewerControlID= protoMousePosInfo.mViewerControlID;
        unit.cellID = protoMousePosInfo.mCellID;
        // 1. get the series uid according to the layout information
        auto recorder = TpsRenderingComponentRecorder::GetInstance();
        TpsRenderProxyBase* renderProxy = recorder->SelectOne(
            (TpsRenderingComponentRecorder::Selector)SelectRenderProxyViaLayout, (void*)&unit);
        if(renderProxy == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to get related render proxy.";
            return;
        }
        std::string seriesUID;
        if (!renderProxy->GetDataSource(unit, IMAGE, seriesUID)) {
            TPS_LOG_DEV_ERROR<<"Can not get the image data source of current layout: "
                <<"View Control ID: "<<unit.viewerControlID<<" Cell ID: "<<unit.cellID;
            return;
        }

        //2. get section type
        WINDOW_TYPE window;
        if (!renderProxy->GetLayoutManager()->GetWindowtype(unit, window)){
            TPS_LOG_DEV_ERROR<<"Failed to get the window type.";
            return;
        }
        auto renderController = renderProxy->GetRenderController(window);
        auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
        if (mprRc == nullptr){
            TPS_LOG_DEV_ERROR<<"The window is not a mpr window.";
            return;
        }
        FIRST_POSTFIX_COMPONENT section = mprRc->GetSectionType();
        m_strImageUID = seriesUID;
        m_strSectionType = section;

        //m_strImageUID = protoMousePosInfo.mImageUID;
        //if (protoMousePosInfo.mSectionType == SAGITTAL_SECTION){
        //    m_strSectionType = SAGITTAL;
        //}
        //else if (protoMousePosInfo.mSectionType == CORONAL_SECTION){
        //    m_strSectionType = CORONAL;
        //}
        //else m_strSectionType = AXIAL;

        float fWidth = protoMousePosInfo.mWidth;
        float fHeight = protoMousePosInfo.mHeight;
        m_fX = protoMousePosInfo.mMouseX;
        m_fY = protoMousePosInfo.mMouseY;

        // convert coordinate to correct width != height
        float fScaleRatio = (fWidth >= fHeight) ? fWidth / fHeight : fHeight / fWidth;
        if (fWidth > fHeight)
        {
            m_fX = (m_fX - 0.5) * fScaleRatio + 0.5;
        }
        else
        {
            m_fY = (m_fY - 0.5) * fScaleRatio + 0.5;
        }
}


TpsGcaMPRHitTestCmd::~TpsGcaMPRHitTestCmd(void) {
}

TpsGcaMPRHitTestCmd *TpsGcaMPRHitTestCmd::Clone() {
    TpsGcaMPRHitTestCmd *cmd = new TpsGcaMPRHitTestCmd(m_strImageUID, m_strSectionType, m_fX, m_fY);
    return cmd;
}

bool TpsGcaMPRHitTestCmd::PreExecute() {
    return true;
}

bool TpsGcaMPRHitTestCmd::Execute() {

    try{
        // 1. Crosshair Hit Test
        //LAYOUT_UNIT layout;
        //layout.cellID = mCertifiedMousePosInfo.mCellID;
        //layout.viewerControlID = mCertifiedMousePosInfo.mViewerControlID;

        //WINDOW_TYPE wintype = mCertifiedMousePosInfo.mWindowType;
        //double width = mCertifiedMousePosInfo.mWidth;
        //double height = mCertifiedMousePosInfo.mHeight;
        //double x = m_protoMousePosInfo.mMouseX;
        //double y = m_protoMousePosInfo.mMouseY;

        //std::string imageUID = m_protoMousePosInfo.mImageUID;
        //std::string section = m_protoMousePosInfo.mSectionType;

        auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, m_strSectionType);
        std::string strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);

        auto go = ModelWarehouse::GetInstance()->GetModelObject(strKey);

        auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);

        //down cast
        if (spCrosshairGO == nullptr){
 /*           TPS_LOG_DEV_ERROR<<"[ERROR][TpsGcaMPRHitTestCmd][Execute]: "
                <<"Failed to dynamic cast IGraphicObjects.";*/
            return true;
        }
        int oldHitTest = spCrosshairGO->IsHit();

        std::shared_ptr<CrosshairHitTestOperation> crosshairHitTestOperation(
            new CrosshairHitTestOperation(strKey, m_fX, m_fY));

        ModelWarehouse::GetInstance()->ProcessOperation(crosshairHitTestOperation);

        ModelWarehouse::GetInstance()->Synchronize();
        int newHitTest = spCrosshairGO->IsHit();
        m_nHitTestResult = newHitTest;

        // 2. fire to render
        if (oldHitTest != newHitTest) {
            std::shared_ptr<TpsFireOperation> fire(new TpsFireOperation());
            ModelWarehouse::GetInstance()->ProcessOperation(fire);
        }  
    }
    catch (std::exception& ex){
        TPS_LOG_DEV_ERROR<<ex.what();
        return false;
    }

    return true;
}

bool TpsGcaMPRHitTestCmd::PostExecute() {
    return true;
}

TPS_END_NAMESPACE
