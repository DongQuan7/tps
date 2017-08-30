//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rl_gallery_crosshair_mode_operation.cpp
///  \brief   
///
///  \version 1.0
///  \date    Dec. 12, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_gallery_mprcornerinfo_visible_operation.h"

// TPS
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h" 

// ZHENGHE

#include "McsfViewer3DModel/mcsf_viewer3d_model.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsGalleryMPRCornerInfoVisibleOperation::TpsGalleryMPRCornerInfoVisibleOperation(std::string& strImageUID, bool bVisible)
    :m_strImageUID(strImageUID), 
     m_bVisible(bVisible) {
}

TpsGalleryMPRCornerInfoVisibleOperation::~TpsGalleryMPRCornerInfoVisibleOperation(){

}

bool TpsGalleryMPRCornerInfoVisibleOperation::ModifyGraphicObject() const {

    try{
        
        FIRST_POSTFIX_COMPONENT strMprSection[3] = {AXIAL, CORONAL, SAGITTAL};
        std::shared_ptr<TpsTextOverlayGraphicObject> spCornerTextGO[3] = {nullptr, nullptr, nullptr};
        std::shared_ptr<MPRGraphicObject> spMprGO[3] = {nullptr, nullptr, nullptr};

        std::string strKey;
        std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> go;
        int i;
        GO_TYPE goType;
        for (i = 0; i < sizeof(strMprSection) / sizeof(strMprSection[0]); i++) {
            goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR_CORNER_INFO, strMprSection[i]);
            strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
            go = mModelWarehouse->GetModelObject(strKey);
            spCornerTextGO[i] = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(go);
            if (spCornerTextGO[i] == nullptr){
                TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryMPRCornerInfoVisibleOperation][ModifyGraphicObject]: "
                    <<"Failed to dynamic cast IGraphicObjects.";
                return TPS_ER_FAILURE;
            }

            goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, strMprSection[i]);
            strKey = m_strImageUID + "|" + GOTypeHelper::ToString(goType);
            go = mModelWarehouse->GetModelObject(strKey);
            spMprGO[i] = std::dynamic_pointer_cast<MPRGraphicObject>(go);
            if (spMprGO[i] == nullptr){
                TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryMPRCornerInfoVisibleOperation][ModifyGraphicObject]: "
                    <<"Failed to dynamic cast IGraphicObjects.";
                return TPS_ER_FAILURE;
            }
        }
        for(i = 0; i < sizeof(strMprSection) / sizeof(strMprSection[0]); i++){
            if (spCornerTextGO[i]->GetVisible() != m_bVisible)
            {
                spCornerTextGO[i]->SetVisible(m_bVisible);
                spCornerTextGO[i]->SetDirty(true);
                spMprGO[i]->SetDirty(true);
            }
        }
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"TpsGalleryMPRCornerInfoVisibleOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }
    return true;
}


TPS_END_NAMESPACE  // end namespace tps
