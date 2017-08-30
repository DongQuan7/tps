//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  SHI CHUNHUI mailto:chunhui.shi@united-imaging.com
///
///  \file    tps_rl_setcornertextvisibility_operation.cpp
///  \brief   declaration
///
///  \version 1.0
///  \date    2015/4/10
///  
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

// TPS
#include "RtTpsRenderLibrary/tps_rl_setrulervisibility_operation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"


TPS_BEGIN_NAMESPACE

SetRulerVisibilityOperation::SetRulerVisibilityOperation(
    const std::string& strImageUID, bool isVisible)
    :mImageUID(strImageUID), mIsVisible(isVisible) {
}

SetRulerVisibilityOperation::~SetRulerVisibilityOperation(){

}

bool SetRulerVisibilityOperation::ModifyGraphicObject() const {

    try{
        FIRST_POSTFIX_COMPONENT section[3] = {AXIAL, CORONAL, SAGITTAL};
        std::shared_ptr<MPRGraphicObject> spMprGO[3] = {nullptr, nullptr, nullptr};

        for (int i = 0; i < 3; i++) {
            GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, section[i]);
            std::string strKey = mImageUID + "|" + GOTypeHelper::ToString(goType);
            std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> go = mModelWarehouse->GetModelObject(strKey);
            spMprGO[i] = std::dynamic_pointer_cast<MPRGraphicObject>(go);
            if (spMprGO[i] == nullptr){
                TPS_LOG_DEV_ERROR<<"[ERROR][CrosshairModeOperation][ModifyGraphicObject]: "
                    <<"Failed to dynamic cast IGraphicObjects.";
                continue;
            }
            spMprGO[i]->SetRulerIsVisible(mIsVisible);
            spMprGO[i]->SetDirty(true);
        }
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"SetRulerVisibilityOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }
    return true;
}

TPS_END_NAMESPACE
