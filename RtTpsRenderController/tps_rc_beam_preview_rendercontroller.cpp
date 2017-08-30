////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  CHUNHUI SHI  chunhui.shi@united-imaging.com
/// 
///  \file tps_rc_beam_preview_rendercontroller.h
/// 
///  \brief class TpsBeamPreviewRenderController declaration
/// 
///  \version 1.0
/// 
///  \date    2014/11/10
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderController/tps_rc_beam_preview_rendercontroller.h"

#include "boost/algorithm/string/split.hpp"

#include "RtTpsDataAccess/tps_da_configuremanager.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"

//ZHENGHE
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

// Render Strategies
#include "RtTpsRenderLibrary/tps_rl_mprrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_beamrenderstrategy.h"

// Graphic Objects
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_simplebeamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamtemplategraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"

#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_beamstructure.h"

#include "tps_logger.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

TpsBeamPreviewRenderController::TpsBeamPreviewRenderController(WINDOW_TYPE type)
    : TpsRenderController(type), m_spMPRStrategy(nullptr), mBeamStrategy(nullptr) 
{
    if (type == WINDOW_COMMON_BEAM_PREVIEW_MPR_AXIAL)
    {
        m_nSectionType = AXIAL;
    }
    else
    {
        m_nSectionType = EMPTY_COMPONENT;
    }

    m_nSectionExtendType = BEAM_PREVIEW;
}

TpsBeamPreviewRenderController::~TpsBeamPreviewRenderController()
{

}

FIRST_POSTFIX_COMPONENT TpsBeamPreviewRenderController::GetSectionType() 
{
    return m_nSectionType;
}

SECOND_POSTFIX_COMPONENT TpsBeamPreviewRenderController::GetSectionExtendType() 
{
    return m_nSectionExtendType;
}


bool TpsBeamPreviewRenderController::SetupScene_i()
{
    try
    {
        m_spMPRStrategy.reset(new MPRRenderStrategy());
        RegisterRenderStrategy_i(ClsView::IMAGE, m_spMPRStrategy);

        mBeamStrategy.reset(new TpsBeamRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mBeamStrategy);

        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "TpsBeamPreviewRenderController::SetupScene_i(): Failed to add strategy: " << ex.what();
        return false;
    }
}

bool TpsBeamPreviewRenderController::ResetDataSource_i(DATA_SOURCE_ID id) 
{
    bool bRes = false;
    switch (id) 
    {
        case IMAGE:
            bRes = ResetImage_i();
            break;
        case PLAN:
            bRes = ResetPlan_i();
            break;
        default:
            TPS_LOG_DEV_ERROR << "Unrecognized data source id: " << id;
            break;
    }
    return bRes;
}

bool TpsBeamPreviewRenderController::ResetImage_i()
{
    std::string imageUID = mDataSourceMap[IMAGE];

    // Volume GO
    std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> go;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spVolumeGO = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_VOLUME, go) ||
        (spVolumeGO = std::dynamic_pointer_cast<GraphicObjVolume>(go)) == nullptr)
    {
        TPS_LOG_DEV_ERROR << "TpsBeamPreviewRenderController::ResetImage(): "
            << "Failed to get volume : " << imageUID;
        return false;
    }

    if (m_nSectionType == EMPTY_COMPONENT)
    {
        TPS_LOG_DEV_ERROR << "TpsBeamPreviewRenderController::ResetImage():"
            << "Failed due to unknown mpr go type.";
        return false;
    }

    // Mpr graphic object
    GO_TYPE nMprGoType = EMPTY_COMPONENT;
    std::shared_ptr<MPRGraphicObject> spMprGO = nullptr;
    nMprGoType = GOTypeHelper::ComposeAsGOType(MPR, m_nSectionType, m_nSectionExtendType);
    if (!CheckGraphicObject_i(imageUID, nMprGoType, go) ||
        (spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go)) == nullptr)
    {
        TPS_LOG_DEV_ERROR << "TpsBeamPreviewRenderController::ResetImage():"
            << "Failed to get MPR go of image: " << imageUID;
        return false;
    }

    m_spMPRStrategy->SetVolumeGO(spVolumeGO);
    m_spMPRStrategy->SetMPRGO(spMprGO);

    mBeamStrategy->SetVolumeGraphicObject(spVolumeGO);
    mBeamStrategy->SetMprGraphicObject(spMprGO);
    mBeamStrategy->SetSectionType(AXIAL);

    return true;
}

bool TpsBeamPreviewRenderController::ResetPlan_i() 
{
    const std::string templatizedNgCollectionGoKey = "TemplatizedNgCollectionGo";
    auto oldTemplatizedNgCollectionGo = mModelWarehouse->GetModelObject(templatizedNgCollectionGoKey);
    if (oldTemplatizedNgCollectionGo != nullptr)
    {
        mModelWarehouse->RemoveModelObject("TemplatizedNgCollectionGo");
    }

    std::shared_ptr<NormalGroupCollection> templatizedNgCollectionGo(new NormalGroupCollection());
    mModelWarehouse->AddModelObject(templatizedNgCollectionGoKey, templatizedNgCollectionGo);
    mBeamStrategy->SetNormalGroupCollection(templatizedNgCollectionGo);

    return true;
}

void TpsBeamPreviewRenderController::ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid)
{
    if (id == IMAGE)
    {
        RemoveGraphicObject_i(uid, GO_TYPE_VOLUME);
        // MPR graphic object
        GO_TYPE nMprGoType = EMPTY_COMPONENT;
        nMprGoType = GOTypeHelper::ComposeAsGOType(MPR, m_nSectionType, m_nSectionExtendType);
        RemoveGraphicObject_i(uid, nMprGoType);
    }
    //else if (id == PLAN)
    //{
    //    //与uid无关的数据不应删除
    //    //mModelWarehouse->RemoveModelObject("TemplatizedNgCollectionGo");
    //}
}
TPS_END_NAMESPACE