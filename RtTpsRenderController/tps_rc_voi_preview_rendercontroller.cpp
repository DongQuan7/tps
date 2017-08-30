////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song song.lig@united-imaging.com
/// 
///  \file tps_rc_voi_preview_rendercontroller.cpp
/// 
///  \brief class TpsVoiPreviewRenderController implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/06/25
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderController/tps_rc_voi_preview_rendercontroller.h"
#include "boost/algorithm/string/split.hpp"

//ZHENGHE
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"


// Render Strategies
#include "RtTpsRenderLibrary/tps_rl_mprrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_roi_render_strategy.h"

// Graphic Objects
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"


#include "tps_logger.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsVoiPreviewRenderController::TpsVoiPreviewRenderController(WINDOW_TYPE type)
    : TpsRenderController(type),m_spMPRStrategy(nullptr), m_spVOIStrategy(nullptr) 
{
    switch (type)
    {
        case WINDOW_COMMON_VOI_PREVIEW_MPR_AXIAL:
            m_nSectionType = AXIAL;
            break;
        case WINDOW_COMMON_VOI_PREVIEW_MPR_CORONAL:
            m_nSectionType = CORONAL;
            break;
        case WINDOW_COMMON_VOI_PREVIEW_MPR_SAGITTAL:
            m_nSectionType = SAGITTAL;
            break;
        default:
            m_nSectionType = EMPTY_COMPONENT;
            break;
    }
    m_nSectionExtendType = VOI_PREVIEW;
}

TpsVoiPreviewRenderController::~TpsVoiPreviewRenderController()
{

}

bool TpsVoiPreviewRenderController::SetupScene_i()
{
    try
    {
        //MPR strategy
        m_spMPRStrategy.reset(new MPRRenderStrategy());
        RegisterRenderStrategy_i(ClsView::IMAGE, m_spMPRStrategy);

        //VOI strategy
        m_spVOIStrategy.reset(new MprRoiRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, m_spVOIStrategy);

        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "TpsVoiPreviewRenderController::SetupScene_i():"
            << "Failed to add strategy: " << ex.what();
        return false;
    }
}

bool TpsVoiPreviewRenderController::ResetDataSource_i(DATA_SOURCE_ID id) 
{
    bool bRes = false;
    switch (id) 
    {
        case IMAGE:
            bRes = ResetImage_i();
            break;
        default:
            TPS_LOG_DEV_ERROR << "Unrecognized data source id: " << id;
            break;
    }
    return bRes;
}

FIRST_POSTFIX_COMPONENT TpsVoiPreviewRenderController::GetSectionType() 
{
    return m_nSectionType;
}

SECOND_POSTFIX_COMPONENT TpsVoiPreviewRenderController::GetSectionExtendType() 
{
    return m_nSectionExtendType;
}

bool TpsVoiPreviewRenderController::ResetImage_i()
{
    std::string strImageUID = mDataSourceMap[IMAGE];
    // Volume GO
    std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> go;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spVolumeGO = nullptr;
    if (!CheckGraphicObject_i(strImageUID, GO_TYPE_VOLUME, go) ||
        (spVolumeGO = std::dynamic_pointer_cast<GraphicObjVolume>(go)) == nullptr) 
    {
        TPS_LOG_DEV_ERROR << "TpsVoiPreviewRenderController::ResetImage(): "
            << "Failed to get volume : " << strImageUID;
        return false;
    }

    if (m_nSectionType == EMPTY_COMPONENT)
    {
        TPS_LOG_DEV_ERROR << "TpsVoiPreviewRenderController::ResetImage(): "
            << "Failed due to unknown mpr go type.";
        return false;
    }

    // MPR graphic object
    GO_TYPE nMprGoType = EMPTY_COMPONENT;

    std::shared_ptr<MPRGraphicObject> spMprGO = nullptr;
    nMprGoType = GOTypeHelper::ComposeAsGOType(MPR, m_nSectionType, m_nSectionExtendType);
    if (!CheckGraphicObject_i(strImageUID, nMprGoType, go) ||
        (spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go)) == nullptr)
    {
        TPS_LOG_DEV_ERROR << "TpsVoiPreviewRenderController::ResetImage():"
            << "Failed to get MPR go of image:" << strImageUID;
        return false;
    }

    // VOI graphic object collection
    std::shared_ptr<TpsVOIGOCollection> spVoiCollection = nullptr;
    if (!CheckGraphicObject_i(strImageUID, GO_TYPE_VOI_COLLECTION, go) ||
        (spVoiCollection = std::dynamic_pointer_cast<TpsVOIGOCollection>(go)) == nullptr)
    {
        TPS_LOG_DEV_ERROR << "TpsVoiPreviewRenderController::ResetImage():"
            << "Failed to get VOI for image: " << strImageUID;
        return false;
    }

    // VOI_Preview go collection
    std::shared_ptr<TpsVOIGOCollection> spVoiCollectionPreview = nullptr;
    if (!CheckGraphicObject_i(strImageUID, GO_TYPE_VOI_COLLECTION_PREVIEW, go) ||
        (spVoiCollectionPreview = std::dynamic_pointer_cast<TpsVOIGOCollection>(go)) == nullptr)
    {
            TPS_LOG_DEV_ERROR << "TpsVoiPreviewRenderController::ResetImage():"
                << "Failed to get VOI for image: "<<strImageUID;
            return false;
    }

     // Copy voi to VOI_Preview
    *spVoiCollectionPreview = *spVoiCollection;

    m_spMPRStrategy->SetVolumeGO(spVolumeGO);
    m_spMPRStrategy->SetMPRGO(spMprGO);

    // reset voi strategy data
    m_spVOIStrategy->SetVolumeGO(spVolumeGO);
    m_spVOIStrategy->SetMPRGO(spMprGO);
    m_spVOIStrategy->SetVOIGOCollection(spVoiCollectionPreview);

    return true;
}

void TpsVoiPreviewRenderController::RemoveDataSource_i(DATA_SOURCE_ID id)
{
    switch (id) 
    {
        case IMAGE:
            m_spMPRStrategy->SetVolumeGO(nullptr);
            m_spMPRStrategy->SetMPRGO(nullptr);
            m_spVOIStrategy->SetVolumeGO(nullptr);
            m_spVOIStrategy->SetMPRGO(nullptr);
            m_spVOIStrategy->SetVOIGOCollection(nullptr);
        default:
            break;
    }
}

void TpsVoiPreviewRenderController::ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid)
{
    if (id == IMAGE)
    {
        RemoveGraphicObject_i(uid, GO_TYPE_VOLUME);
        RemoveGraphicObject_i(uid, GO_TYPE_MASK);
        // MPR graphic object
        GO_TYPE goType = EMPTY_COMPONENT;
        goType = GOTypeHelper::ComposeAsGOType(MPR, m_nSectionType, m_nSectionExtendType);
        RemoveGraphicObject_i(uid, goType);
        RemoveGraphicObject_i(uid, GO_TYPE_VOI_COLLECTION);
        RemoveGraphicObject_i(uid, GO_TYPE_VOI_COLLECTION_PREVIEW);
    }
}
TPS_END_NAMESPACE  // end namespace tps
