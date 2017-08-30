//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  shaosen.huang shaosen.huang@united-imaging.com
//
/// \file    tps_gca_mprupdatecornertextinfo_cmd.cpp
///
/// \brief   MPRUpdateCornerTextInfoCmd
/// 
/// \version 1.0
/// 
/// \date    2015/04/10
/////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tps_gca_mprupdatecornertextinfo_cmd.h"

#include "boost/lexical_cast.hpp"

//ZHENGHE
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

//TPS
#include "tps_logger.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_rl_modelwarehouse.h"
#include "RtTpsFramework/tps_rl_fire_operation.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_mprupdatecornerinfo_operation.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsFramework/tps_rl_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_rc_renderingcomponentrecorder.h"
#include "RtTpsFramework/tps_rc_renderproxybase.h"
#include "RtTpsFramework/tps_rc_layoutmanager.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsRenderController/tps_rc_gallery_mprrendercontroller.h"

//TMS
#include "tms/rt_fwk_dl_object_image3d.h"


using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

    extern bool SelectRenderProxyViaLayout(TpsRenderProxyBase*, void*);

TpsGcaMPRUpdateCornerTextInfoCmd::TpsGcaMPRUpdateCornerTextInfoCmd(const CertifiedMousePosInfo &certifedMousePosInfo) : 
mCertifiedMousePosInfo(certifedMousePosInfo){

}

TpsGcaMPRUpdateCornerTextInfoCmd::~TpsGcaMPRUpdateCornerTextInfoCmd(){

}

TpsGcaMPRUpdateCornerTextInfoCmd *TpsGcaMPRUpdateCornerTextInfoCmd::Clone() {
    TpsGcaMPRUpdateCornerTextInfoCmd *cmd = new TpsGcaMPRUpdateCornerTextInfoCmd(mCertifiedMousePosInfo);
    return cmd;
}

bool TpsGcaMPRUpdateCornerTextInfoCmd::PreExecute() {
    return true;
}

bool TpsGcaMPRUpdateCornerTextInfoCmd::PostExecute() {
    return true;
}

bool TpsGcaMPRUpdateCornerTextInfoCmd::Execute() {

    try{
        LAYOUT_UNIT layout;
        layout.cellID = mCertifiedMousePosInfo.mCellID;
        layout.viewerControlID = mCertifiedMousePosInfo.mViewerControlID;

        WINDOW_TYPE wintype = mCertifiedMousePosInfo.mWindowType;
        double width = mCertifiedMousePosInfo.mWidth;
        double height = mCertifiedMousePosInfo.mHeight;
        double mousex = mCertifiedMousePosInfo.mMouseX;
        double mousey = mCertifiedMousePosInfo.mMouseY;

        //get the series uid according to the layout information
        auto recorder = TpsRenderingComponentRecorder::GetInstance();
        TpsRenderProxyBase* renderProxy = recorder->SelectOne(
            (TpsRenderingComponentRecorder::Selector)SelectRenderProxyViaLayout, (void*)&layout);
        if(renderProxy == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to get related render proxy.";
            return false;
        }

        std::string imageUID;
        if (!renderProxy->GetDataSource(layout, IMAGE, imageUID)) {
            TPS_LOG_DEV_ERROR<<"Can not get the image data source of current layout: "
                <<"View Control ID: "<<layout.viewerControlID<<" Cell ID: "<<layout.cellID;
            return false;
        }

        //std::string currentPlanUid;
        //renderProxy->GetDataSource(layout, PLAN, currentPlanUid);

        // get a renderController just for a section type?
        auto renderController = renderProxy->GetRenderController(wintype);
        auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
        if (mprRc == nullptr){
            TPS_LOG_DEV_ERROR<<"The window is not a mpr window.";
            return false;
        }

        FIRST_POSTFIX_COMPONENT sectionType = mprRc->GetSectionType();
        std::shared_ptr<TpsGalleryMPRUpdateCornerTextInfoOperation> updateCornerTextInfoOperation(
            new TpsGalleryMPRUpdateCornerTextInfoOperation(width, height, mousex, mousey));

        std::string sVolumeKey = imageUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
        auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, sectionType);
        std::string sMPRKey = imageUID + "|" + GOTypeHelper::ToString(goType);
        goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR_CORNER_INFO, sectionType);
        std::string sMPRCornerInfoKey = imageUID + "|" + GOTypeHelper::ToString(goType);
        std::string sDummyGoKey = boost::lexical_cast<std::string>(wintype)
            + "|" + GOTypeHelper::ToString(GO_TYPE_DUMMY);
        updateCornerTextInfoOperation->SetGraphicObjectKeys(sVolumeKey, sMPRKey, sMPRCornerInfoKey, sDummyGoKey);
        ModelWarehouse::GetInstance()->ProcessOperation(updateCornerTextInfoOperation);


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

TPS_END_NAMESPACE
