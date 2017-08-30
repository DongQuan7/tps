//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  LI Song song.li@united-imaging.com
///
///  \file    tps_rl_crosshair_hittest_operation.h
///  \brief   Crosshair Hit Test implementation
///
///  \version 1.0
///  \date    Jul. 10, 2014
///  
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

// TPS
#include "RtTpsRenderLibrary/tps_rl_crosshair_hittest_operation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

CrosshairHitTestOperation::CrosshairHitTestOperation(std::string strKey, double x, double y)
    :m_strCrosshairGOKey(strKey), m_dX(x), m_dY(y) {
}

CrosshairHitTestOperation::~CrosshairHitTestOperation(){

}

bool CrosshairHitTestOperation::ModifyGraphicObject() const {

    try{

        auto go = mModelWarehouse->GetGraphicObject(m_strCrosshairGOKey);

        auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);

        //down cast
        if (spCrosshairGO == nullptr){
            TPS_LOG_DEV_ERROR<<"[ERROR][CrosshairHitTestOperation][ModifyGraphicObject]: "
                <<"Failed to dynamic cast IGraphicObjects.";
            return false;
        }

        int nHitTestOld = spCrosshairGO->IsHit();
        int nHitTestNew = spCrosshairGO->HitTest(m_dX, m_dY);
        if (nHitTestOld != nHitTestNew) {
            spCrosshairGO->SetDirty(true);
        }
    }
    catch(std::exception& ex){
        TPS_LOG_DEV_ERROR<<"CrosshairHitTestOperation::ModifyGraphicObject:"<<ex.what();
        return false;
    }
    return true;
}

TPS_END_NAMESPACE  // end namespace tps
