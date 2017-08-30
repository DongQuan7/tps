////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wang Jie  jie.wang@united-imaging.com
/// 
///  \file tps_rc_planQA_mpr_render_controller_axial.cpp
/// 
///  \brief class TpsPlanQAMprRenderControllerAxial implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/11/14
////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "RtTpsRenderController/tps_rc_planQA_mpr_render_controller_axial.h"

#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"

#include "RtTpsRenderLibrary/tps_rl_setupptcrosshairstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_editsetuppoigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

    TpsPlanQAMprRenderControllerAxial::TpsPlanQAMprRenderControllerAxial(WINDOW_TYPE type) 
    : TpsPlanQAMPRRenderController(type)
{

}

TpsPlanQAMprRenderControllerAxial::~TpsPlanQAMprRenderControllerAxial() 
{

}

bool TpsPlanQAMprRenderControllerAxial::SetupScene_i() 
{
    if(!TpsPlanQAMPRRenderController::SetupScene_i()) 
    {
        TPS_LOG_DEV_ERROR<<"mpr render controller set up scene failed!";
        return false;
    }

    mSetupCrosshairStrategy.reset(new TpsSetupPtCrossHairStrategy());
    RegisterRenderStrategy_i(Mcsf::MedViewer3D::ClsView::GRAPHIC, mSetupCrosshairStrategy);

    return true;
}

bool TpsPlanQAMprRenderControllerAxial::ResetImage_i() 
{
    if(!TpsPlanQAMPRRenderController::ResetImage_i()) 
    {
        TPS_LOG_DEV_ERROR<<"mpr render controller reset image failed!";
        return false;
    }

    std::string imageUID = mDataSourceMap[IMAGE];
    IGraphicObjectPtr goSetupPoi = nullptr;
    if(!CheckGraphicObject_i(imageUID,GO_TYPE_SETUP_POI,goSetupPoi)) 
    {
        TPS_LOG_DEV_ERROR<<"failed to get go setup!";
        return false;
    }

    auto setUp = std::dynamic_pointer_cast<EditSetupPtGraphicObject>(goSetupPoi);

    if(nullptr == goSetupPoi || nullptr == setUp ) {
        TPS_LOG_DEV_ERROR<<"Failed to get go!";
        return false;
    }

    mSetupCrosshairStrategy->SetGraphicObject(setUp);

    return true;
}

void TpsPlanQAMprRenderControllerAxial::ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid)
{
    TpsMPRRenderController::ClearGraphicObjects_i(id, uid);
    if (id == IMAGE)
    {
        RemoveGraphicObject_i(uid, GO_TYPE_SETUP_POI);
    }
}

void TpsPlanQAMprRenderControllerAxial::RemoveDataSource_i(DATA_SOURCE_ID id)
{
    TpsPlanQAMPRRenderController::RemoveDataSource_i(id);
    switch (id) {
    case IMAGE:
        if (mSetupCrosshairStrategy  != nullptr)
        {
            mSetupCrosshairStrategy->SetGraphicObject(nullptr);
        }
        break;
    default:
        break;
    }
}
TPS_END_NAMESPACE
