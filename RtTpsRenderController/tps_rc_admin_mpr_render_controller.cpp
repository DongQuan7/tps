////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wang Jie  jie.wang@united-imaging.com
/// 
///  \file tps_rc_admin_mpr_render_controller.cpp
/// 
///  \brief class TpsAdminMPRRenderController implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/11/14
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderController/tps_rc_admin_mpr_render_controller.h"

//ZHENGHE
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

// Render Strategies
#include "RtTpsRenderLibrary/tps_rl_volume4d_mpr_renderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_roi_render_strategy.h"
#include "RtTpsRenderLibrary/tps_rl_poi_renderstrategy.h"               // for TpsPoiRenderStrategy
#include "RtTpsRenderLibrary/tps_rl_series_table_render_strategy.h"

// Graphic Objects
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_volume4d_graphicobject.h"           // for Volume4dGraphicObject
#include "RtTpsRenderLibrary/tps_rl_series_table_cache_graphic_object.h"

// go collections
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"

// frameworks
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_series_group4d.h"
#include "RtTpsFramework/tps_fw_go_converter_parameter.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"


using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsAdminMPRRenderController::TpsAdminMPRRenderController(WINDOW_TYPE type)
    : TpsMPRRenderController(type), mVOIStrategy(nullptr)
{
    SetSectionType_i();
}

TpsAdminMPRRenderController::~TpsAdminMPRRenderController()
{
}

void TpsAdminMPRRenderController::SetSectionType_i()
{
    switch(mWindowType) {
    case WINDOW_ADMIN_MPR_AXIAL:
        mSectionType = AXIAL;
        break;
    case WINDOW_ADMIN_MPR_SAGITTAL:
        mSectionType = SAGITTAL;
        break;
    case WINDOW_ADMIN_MPR_CORONAL:
        mSectionType = CORONAL;
        break;
    default:
        mSectionType = EMPTY_COMPONENT;
        break;
    }
    mLocation = EMPTY_COMPONENT;
    mWindowGroupType = (DATA_SOURCE_GROUP_ID)(mWindowType >> 8);
}

bool TpsAdminMPRRenderController::SetupScene_i()
{
    if (!TpsMPRRenderController::SetupScene_i())
    {
        TPS_LOG_DEV_ERROR<<"Failed to setup scene of MPRRenderController";
        return false;
    }
    
    try
    {
        mVOIStrategy.reset(new MprRoiRenderStrategy());
        this->RegisterRenderStrategy_i(ClsView::GRAPHIC, mVOIStrategy);

        //poi
        m_spPoiStrategy.reset(new TpsPoiRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, m_spPoiStrategy);

        //seriestable
        mSeriesTableRenserStrategy.reset(new SeriesTableRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mSeriesTableRenserStrategy);

        return true;
    }
    catch (std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Failed to setup scene: "<<ex.what();
        return false;
    }
}



bool TpsAdminMPRRenderController::ResetImage_i()
{
    TpsMPRRenderController::ResetImage_i();

    std::string imageUID = mDataSourceMap[IMAGE];
    auto spSeriesGroup4d = mDataRepository->GetImageDataManager()->GetSeriesGroup4d(imageUID);
    bool is4DSeries = spSeriesGroup4d != nullptr;
    IGraphicObjectPtr goVolume = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_VOLUME, goVolume))
    {
        TPS_LOG_DEV_ERROR<<"Failed to check volume graphic object of image: "<<imageUID;
        return false;
    }

    // Volume4d GO
    std::shared_ptr<Volume4dGraphicObject> spVolume4dGO = nullptr;
    if (is4DSeries)
    {
        IGraphicObjectPtr goVolume4d= nullptr;
        if (!CheckGraphicObject_i(imageUID, GO_TYPE_VOLUME_D4, goVolume4d))
        {
            TPS_LOG_DEV_ERROR << "Failed to check volume4d graphic object of image: " << imageUID;
            return false;
        }
        spVolume4dGO =std::dynamic_pointer_cast<Volume4dGraphicObject>(goVolume4d);
        if (spVolume4dGO == nullptr)
        {
            TPS_LOG_DEV_ERROR << "Failed to check volume4d graphic object of image: " << imageUID;
            return false;
        }
        spVolume4dGO->ClearAllVolumes();

        IGraphicObjectPtr goVol = nullptr;
        std::shared_ptr<GraphicObjVolume> spVolume = nullptr;
        GOConverterParameter para;
        for (int i = 0; i < spSeriesGroup4d->m_vecSeriesUid.size(); ++i)
        {
            para.data_source_map[IMAGE] = spSeriesGroup4d->m_vecSeriesUid[i];
            if (spSeriesGroup4d->m_vecSeriesUid[i] == imageUID)
            {
                goVol = goVolume;
            }
            else if (!CheckGraphicObject_i(spSeriesGroup4d->m_vecSeriesUid[i], GO_TYPE_VOLUME, goVol, false, &para))
            {
                TPS_LOG_DEV_ERROR<<"Failed to check volume graphic object of image: "<<spSeriesGroup4d->m_vecSeriesUid[i];
            }

            spVolume =std::dynamic_pointer_cast<GraphicObjVolume>(goVol);
            if (spVolume != nullptr)
            {
                spVolume4dGO->AddVolume(spVolume);
            }
        }
        spVolume =std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
        spVolume4dGO->SetCurrentVolume(spVolume);
        spVolume4dGO->SetMainVolume(spVolume);
    }

    //check mpr graphic object
    if (mSectionType == EMPTY_COMPONENT)
    {
        TPS_LOG_DEV_ERROR<<"The section type is unknown.";
        return false;
    }
    IGraphicObjectPtr goMpr = nullptr;
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType, mLocation);
    if (!CheckGraphicObject_i(imageUID, goType, goMpr))
    {
        TPS_LOG_DEV_ERROR<<"Failed to check MPR graphic object of image: "<<imageUID;
        return false;
    }

    //check voi graphic object collection & VOI Rendering Cache graphic object
    IGraphicObjectPtr goVOI = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_VOI_COLLECTION, goVOI)){
        TPS_LOG_DEV_ERROR<<"Failed to check VOI graphic object collection of image: "<<imageUID;
        return false;
    }
    auto voi = std::dynamic_pointer_cast<TpsVOIGOCollection>(goVOI);
    if( voi == nullptr){
        return false;
    }

    //POI
    IGraphicObjectPtr goPoi = nullptr;
    if(!CheckGraphicObject_i(imageUID, GO_TYPE_POI_COLLECTION, goPoi)) {
        TPS_LOG_DEV_ERROR<<"Failed to check Poi graphic object collection of image:"<<imageUID;
        return false;
    }
    //poi
    auto poi = std::dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(goPoi);

    //series table GO
    goType = GOTypeHelper::ComposeAsGOType(SERIES_TABLE_CACHE, mSectionType, mLocation);
    IGraphicObjectPtr goSeriesTable = nullptr;
    if(!CheckGraphicObject_i(imageUID, goType, goSeriesTable)) 
    {
        TPS_LOG_DEV_ERROR<<"Failed to check series table cache graphic object of image:"<<imageUID;
        return false;
    }
    auto seriesTableGO = std::dynamic_pointer_cast<SeriesTableCacheGraphicObject>(goSeriesTable);

    //RTTI
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(goMpr);

    if (volume == nullptr || mpr == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects.";
    }

    if (mVOIStrategy != nullptr)
    {
        mVOIStrategy->SetVolumeGO(volume);
        mVOIStrategy->SetMPRGO(mpr);
        mVOIStrategy->SetVOIGOCollection(voi);
    }

    if (m_spPoiStrategy != nullptr)
    {
        m_spPoiStrategy->SetMprGraphicObject(mpr);
        m_spPoiStrategy->SetVolumeGraphicObject(volume);
        m_spPoiStrategy->SetPoiGraphicObjectCollection(poi);
    }

    if (is4DSeries && mMprStrategy != nullptr)
    {
        mMprStrategy->SetMPRGO(mpr);
        mMprStrategy->SetVolume4dGraphicObject(spVolume4dGO);
        mMprStrategy->Enable4DMode(true);
    }

    if (mSeriesTableRenserStrategy != nullptr)
    {
        mSeriesTableRenserStrategy->SetVolumeGO(volume);
        mSeriesTableRenserStrategy->SetMPRGO(mpr);
        mSeriesTableRenserStrategy->SetSeriesTableCacheGO(seriesTableGO);
    }
    return true;
}


void TpsAdminMPRRenderController::RemoveDataSource_i(DATA_SOURCE_ID id)
{
    TpsMPRRenderController::RemoveDataSource_i(id);

    switch (id) 
    {
    case IMAGE:
        if (mVOIStrategy != nullptr)
        {
            mVOIStrategy->SetVolumeGO(nullptr);
            mVOIStrategy->SetMPRGO(nullptr);
            mVOIStrategy->SetVOIGOCollection(nullptr);
        }
        if (m_spPoiStrategy != nullptr)
        {
            m_spPoiStrategy->SetMprGraphicObject(nullptr);
            m_spPoiStrategy->SetVolumeGraphicObject(nullptr);
            m_spPoiStrategy->SetPoiGraphicObjectCollection(nullptr);
        }
        if (mSeriesTableRenserStrategy != nullptr)
        {
            mSeriesTableRenserStrategy->SetVolumeGO(nullptr);
            mSeriesTableRenserStrategy->SetMPRGO(nullptr);
            mSeriesTableRenserStrategy->SetSeriesTableCacheGO(nullptr);
        }
        break;
    default:
        break;
    }
}

void TpsAdminMPRRenderController::ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid)
{
    TpsMPRRenderController::ClearGraphicObjects_i(id, uid);

    if (id == IMAGE) {
        std::string imageUID = uid;
        GO_TYPE goType = GOTypeHelper::ComposeAsGOType(CROSSHAIR, mSectionType, mLocation);
        RemoveGraphicObject_i(imageUID, goType);
        RemoveGraphicObject_i(imageUID, GO_TYPE_VOI_COLLECTION);
        goType = GOTypeHelper::ComposeAsGOType(SERIES_TABLE_CACHE, mSectionType, mLocation);
        RemoveGraphicObject_i(imageUID, goType);
    }
}

TPS_END_NAMESPACE  // end namespace tps
