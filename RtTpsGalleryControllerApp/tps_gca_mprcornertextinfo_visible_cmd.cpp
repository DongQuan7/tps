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
#include "tps_gca_mprcornertextinfo_visible_cmd.h"

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

#include "RtTpsRenderLibrary/tps_rl_gallery_mprcornerinfo_visible_operation.h"
#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"

//TMS
#include "tms/rt_fwk_dl_object_image3d.h"


using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE


    TpsGcaMPRCornerTextInfoVisibleCmd::TpsGcaMPRCornerTextInfoVisibleCmd(const std::string& strImageUID, const bool& isVisible) : 
m_strImageUID(strImageUID), m_isVisible(isVisible){

}

TpsGcaMPRCornerTextInfoVisibleCmd::~TpsGcaMPRCornerTextInfoVisibleCmd(){

}

TpsGcaMPRCornerTextInfoVisibleCmd *TpsGcaMPRCornerTextInfoVisibleCmd::Clone() {
    TpsGcaMPRCornerTextInfoVisibleCmd *cmd = new TpsGcaMPRCornerTextInfoVisibleCmd(m_strImageUID, m_isVisible);
    return cmd;
}

bool TpsGcaMPRCornerTextInfoVisibleCmd::PreExecute() {
    return true;
}

bool TpsGcaMPRCornerTextInfoVisibleCmd::PostExecute() {
    return true;
}

bool TpsGcaMPRCornerTextInfoVisibleCmd::Execute() {

    try{
        std::shared_ptr<TpsGalleryMPRCornerInfoVisibleOperation> cornerTextInfoVisibleOperation(
            new TpsGalleryMPRCornerInfoVisibleOperation(m_strImageUID, m_isVisible));

        TpsGalleryImageManager::GetInstance()->SetIsVisibleOfImageText(m_isVisible);
        ModelWarehouse::GetInstance()->ProcessOperation(cornerTextInfoVisibleOperation);


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
