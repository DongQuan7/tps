////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wang Jie  jie.wang@united-imaging.com
/// 
///  \file tps_rc_common_rendercontroller.cpp
/// 
///  \brief class TpsCommonMPRRenderController implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/11/14
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderController/tps_rc_common_mpr_render_controller.h"

//ZHENGHE
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

// Render Strategies
#include "RtTpsRenderLibrary/tps_rl_beamrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mprdoserenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_roi_render_strategy.h"
#include "RtTpsRenderLibrary/tps_rl_poi_renderstrategy.h"               // for TpsPoiRenderStrategy
#include "RtTpsRenderLibrary/tps_rl_dosegridboundaryrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_renderstrategy.h"         // for CrosshairRenderStrategy
#include "RtTpsRenderLibrary/tps_rl_mpr_hot_spots_renderstrategy.h"     // for MprHotSpotsRenderStrategy
#include "RtTpsRenderLibrary/tps_rl_volume4d_mpr_renderstrategy.h"      // for Volume4dMprRenderStrategy
#include "RtTpsRenderLibrary/tps_rl_series_table_render_strategy.h"

// Graphic Objects
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprdosegraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_doselineobject.h"
#include "RtTpsRenderLibrary/tps_rl_hot_spots_graphicobject.h"          // for HotSpotsGraphicObject
#include "RtTpsRenderLibrary/tps_rl_volume4d_graphicobject.h"           // for Volume4dGraphicObject
#include "RtTpsRenderLibrary/tps_rl_series_table_cache_graphic_object.h"

// go collections
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_contour_set_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"

// frameworks
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_series_group4d.h"
#include "RtTpsFramework/tps_fw_go_converter_parameter.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsCommonMPRRenderController::TpsCommonMPRRenderController(WINDOW_TYPE type)
    : TpsMPRRenderController(type),
    mLimitRegionStrategy(nullptr), 
    m_spCrosshairRenderStrategy(nullptr),
    m_spMprHotSpotsRenderStrategy(nullptr)
{
    SetSectionType_i();
}

TpsCommonMPRRenderController::~TpsCommonMPRRenderController()
{
}

void TpsCommonMPRRenderController::SetSectionType_i()
{
    switch(mWindowType) {
    case WINDOW_COMMON_MPR_AXIAL:
    case WINDOW_EV_1_MPR_AXIAL:
    case WINDOW_EV_2_MPR_AXIAL:
    case WINDOW_EV_RESULT_MPR_AXIAL:
        mSectionType = AXIAL;
        break;
    case WINDOW_COMMON_MPR_SAGITTAL:
    case WINDOW_EV_1_MPR_SAGITTAL:
    case WINDOW_EV_2_MPR_SAGITTAL:
    case WINDOW_EV_RESULT_MPR_SAGITTAL:
        mSectionType = SAGITTAL;
        break;
    case WINDOW_COMMON_MPR_CORONAL:
    case WINDOW_EV_1_MPR_CORONAL:
    case WINDOW_EV_2_MPR_CORONAL:
    case WINDOW_EV_RESULT_MPR_CORONAL:
        mSectionType = CORONAL;
        break;
    default:
        mSectionType = EMPTY_COMPONENT;
        break;
    }
    mLocation = EMPTY_COMPONENT;
    mWindowGroupType = (DATA_SOURCE_GROUP_ID)(mWindowType >> 8);
}

bool TpsCommonMPRRenderController::SetupScene_i()
{
    if (!TpsMPRRenderController::SetupScene_i())
    {
        TPS_LOG_DEV_ERROR<<"Failed to setup scene of MPRRenderController";
        return false;
    }

    try
    {
        std::string  strWindowName;
        if (mSectionType == AXIAL)
            strWindowName = "Axial";
        else if (mSectionType == CORONAL)
            strWindowName = "Coronal";
        else if (mSectionType == SAGITTAL)
            strWindowName = "Sagittal";

        //dose
        mDoseStrategy.reset(new TpsMPRDoseRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mDoseStrategy);

        //beam
        mBeamStrategy.reset(new TpsBeamRenderStrategy());
        mBeamStrategy->SetSectionType(mSectionType);// for ARC beams
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mBeamStrategy);

        //voi
        //mVOIStrategy.reset(new TpsVoiRenderStrategy());
        //RegisterRenderStrategy_i(ClsView::GRAPHIC, mVOIStrategy);
        mRoiStrategy.reset(new MprRoiRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mRoiStrategy);

        //seriestable
        mSeriesTableRenserStrategy.reset(new SeriesTableRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mSeriesTableRenserStrategy);

        //poi
        m_spPoiStrategy.reset(new TpsPoiRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, m_spPoiStrategy);
        m_spMaxPointStrategy.reset(new TpsPoiRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, m_spMaxPointStrategy);

        //dosegridboundary
        mLimitRegionStrategy.reset(new DoseGridBoundaryRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mLimitRegionStrategy);

        //dosegridboundary
        mDoseGridBoundaryStrategy.reset(new DoseGridBoundaryRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mDoseGridBoundaryStrategy);

        //hot/cold spots
        m_spMprHotSpotsRenderStrategy.reset(new MprHotSpotsRenderStrategy());
        m_spMprHotSpotsRenderStrategy->SetName(strWindowName);
        RegisterRenderStrategy_i(ClsView::GRAPHIC, m_spMprHotSpotsRenderStrategy);
        return true;
    }
    catch (std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Failed to setup scene: "<<ex.what();
        return false;
    }
}

void TpsCommonMPRRenderController::HitChess_i(double x, double y, HitTestResult& hitRes)
{
    if (mMprStrategy == nullptr) return;
    mMprStrategy->HitTest(x, y, hitRes);
}

void TpsCommonMPRRenderController::HitBeam_i(double x, double y, HitTestResult& hitRes)
{
    if (mBeamStrategy == nullptr) return;
    mBeamStrategy->HitTest(x, y, hitRes);
}

void TpsCommonMPRRenderController::HitDoseGrid_i(double x, double y, HitTestResult& hitRes)
{
    if (mDoseGridBoundaryStrategy == nullptr) return;
    mDoseGridBoundaryStrategy->HitTest(x, y, hitRes);
}


bool TpsCommonMPRRenderController::ResetImage_i()
{
    TpsMPRRenderController::ResetImage_i();

    std::string imageUID = mDataSourceMap[IMAGE];
    auto spSeriesGroup4d = mDataRepository->GetImageDataManager()->GetSeriesGroup4d(imageUID);
    bool is4DSeries = spSeriesGroup4d != nullptr;

    //check volume graphic object
    IGraphicObjectPtr goVolume = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_VOLUME, goVolume))
    {
        TPS_LOG_DEV_ERROR << "Failed to check volume graphic object of image: " << imageUID;
        return false;
    }

    // Volume4d GO
    std::shared_ptr<Volume4dGraphicObject> spVolume4dGO = nullptr;
    if (is4DSeries)
    {
        IGraphicObjectPtr goVolume4d= nullptr;
        if (!CheckGraphicObject_i(imageUID, GO_TYPE_VOLUME_D4, goVolume4d))
        {
            TPS_LOG_DEV_ERROR<<"Failed to check volume4d graphic object of image: "<<imageUID;
            return false;
        }
        spVolume4dGO =std::dynamic_pointer_cast<Volume4dGraphicObject>(goVolume4d);
        if (spVolume4dGO == nullptr)
        {
            TPS_LOG_DEV_ERROR<<"Failed to check volume4d graphic object of image: "<<imageUID;
            return false;
        }
        spVolume4dGO->ClearAllVolumes();

        IGraphicObjectPtr goVol = nullptr;
        std::shared_ptr<GraphicObjVolume> spVolume = nullptr;
        GOConverterParameter para;
        for(int i = 0; i < spSeriesGroup4d->m_vecSeriesUid.size(); ++i)
        {
            para.data_source_map[IMAGE] = spSeriesGroup4d->m_vecSeriesUid[i];
            if(spSeriesGroup4d->m_vecSeriesUid[i] == imageUID)
            {
                goVol = goVolume;
            }
            else if(!CheckGraphicObject_i(spSeriesGroup4d->m_vecSeriesUid[i], GO_TYPE_VOLUME, goVol, false, &para)){
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
        TPS_LOG_DEV_ERROR << "The section type is unknown.";
        return false;
    }
    IGraphicObjectPtr goMpr = nullptr;
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType, mLocation);
    if (!CheckGraphicObject_i(imageUID, goType, goMpr))
    {
        TPS_LOG_DEV_ERROR << "Failed to check MPR graphic object of image: " << imageUID;
        return false;
    }

    //hot spot GO
    IGraphicObjectPtr goHotSpots = nullptr;
    if(!CheckGraphicObject_i(COMMON_BASE, GO_TYPE_HOT_SPOTS, goHotSpots)) {
        TPS_LOG_DEV_ERROR<<"failed to get hot spots go!";
        return false;
    }
    auto spHotSpotsGO = std::dynamic_pointer_cast<HotSpotsGraphicObject>(goHotSpots);
    //cold spot GO
    if(!CheckGraphicObject_i(COMMON_BASE, GO_TYPE_COLD_SPOTS, goHotSpots)) {
        TPS_LOG_DEV_ERROR<<"failed to get cold spots go!";
        return false;
    }
    auto spColdSpotsGO = std::dynamic_pointer_cast<HotSpotsGraphicObject>(goHotSpots);

    //POI
    IGraphicObjectPtr goPoi = nullptr;
    if(!CheckGraphicObject_i(imageUID, GO_TYPE_POI_COLLECTION, goPoi)) {
        TPS_LOG_DEV_ERROR<<"Failed to check Poi graphic object collection of image:"<<imageUID;
        return false;
    }
    //poi
    auto poi = std::dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(goPoi);

    //check voi graphic object collection & VOI Rendering Cache graphic object
    IGraphicObjectPtr goVOI = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_VOI_COLLECTION, goVOI)){
        TPS_LOG_DEV_ERROR<<"Failed to check VOI graphic object collection of image: "<<imageUID;
        return false;
    }
//     IGraphicObjectPtr goVoiRenderingCacheCollection = nullptr;
//     goType = GOTypeHelper::ComposeAsGOType(VOI_RENDERINGCACHE_COLLECTION, mSectionType, mLocation);
//     if (!CheckGraphicObject_i(imageUID, goType, goVoiRenderingCacheCollection)){
//         TPS_LOG_DEV_ERROR<<"Failed to check VOI render cache of image: "
//             <<imageUID<<" sectionPage: "<<goType;
//         return false;
//     }
    auto voi = std::dynamic_pointer_cast<TpsVOIGOCollection>(goVOI);
//     auto voiCacheCollection = std::dynamic_pointer_cast<
//         TpsVOIRenderingCacheGOCollection>(goVoiRenderingCacheCollection);
    if( voi == nullptr /*|| voiCacheCollection == nullptr*/){
        return false;
    }

//     GOConverterParameter para;
//     para.data_source_map = mDataSourceMap;
//     para.section = mSectionType;
//     IGraphicObjectPtr go = nullptr;
//     if (!CheckGraphicObject_i(imageUID, GO_TYPE_ROI_CONTOUR_SET_COLLECTION, go, true, &para))
//     {
//         TPS_LOG_DEV_ERROR<<"Failed to check contour set collection go.";
//         return false;
//     }
//     auto cs = std::dynamic_pointer_cast<ComposableGraphicObject>(go);
//     if (cs == nullptr)
//     {
//         return false;
//     }

    //Limit region GO
    IGraphicObjectPtr goLimitRegion = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_LIMIT_REGION, goLimitRegion)) {
        TPS_LOG_DEV_ERROR<<"Failed to check limit region graphic object collection of plan: "<<imageUID;
        return false;
    }

    std::shared_ptr<TpsDoseGridBoundaryGraphicObject> fusionRegion = nullptr;
    if (mWindowGroupType == SOURCE_GROUP_FUSION_FIXED || mWindowGroupType == SOURCE_GROUP_FUSION_FLOAT){
        IGraphicObjectPtr goFusionRegion = nullptr;
        GO_TYPE fusionRegionGoType = mWindowGroupType == SOURCE_GROUP_FUSION_FIXED ? 
            GO_TYPE_FUSION_REGION_FIXED : GO_TYPE_FUSION_REGION_FLOAT;
        if (!CheckGraphicObject_i(COMMON_BASE, fusionRegionGoType, goFusionRegion)){
            TPS_LOG_DEV_ERROR<<"Failed to check fusion region graphic object.";
            return false;
        }
        fusionRegion = std::dynamic_pointer_cast<TpsDoseGridBoundaryGraphicObject>(goFusionRegion);
    }

    //series table GO
    std::shared_ptr<SeriesTableCacheGraphicObject> seriesTableGO;
    if (mWindowGroupType != SOURCE_GROUP_ADMIN)
    {
        goType = GOTypeHelper::ComposeAsGOType(SERIES_TABLE_CACHE, mSectionType, mLocation);
        IGraphicObjectPtr go = nullptr;
        if(!CheckGraphicObject_i(imageUID, goType, go)) 
        {
            TPS_LOG_DEV_ERROR<<"Failed to check series table cache graphic object!";
            return false;
        }
        seriesTableGO = std::dynamic_pointer_cast<SeriesTableCacheGraphicObject>(go);
    }

    //RTTI
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(goMpr);
    //dose grid boundary GO &&  ruler text GO
    auto limitRegion = std::dynamic_pointer_cast<
        TpsDoseGridBoundaryGraphicObject>(goLimitRegion);
    if (limitRegion == nullptr ||
        volume == nullptr ||
        mpr == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects.";
        //return false;
    }

    //4DCT GO
    if (is4DSeries && mMprStrategy != nullptr)
    {
        mMprStrategy->SetMPRGO(mpr);
        mMprStrategy->SetVolume4dGraphicObject(spVolume4dGO);
        mMprStrategy->Enable4DMode(true);
    }
    if (m_spMprHotSpotsRenderStrategy != nullptr)
    {
        m_spMprHotSpotsRenderStrategy->SetMPRGraphicObject(mpr);
        m_spMprHotSpotsRenderStrategy->SetVolumeGraphicObject(volume);
        m_spMprHotSpotsRenderStrategy->SetHotSpotsGraphicObject(spHotSpotsGO);
        m_spMprHotSpotsRenderStrategy->SetColdSpotsGraphicObject(spColdSpotsGO);
    }
    if (mBeamStrategy != nullptr)
    {
        mBeamStrategy->SetVolumeGraphicObject(volume);
        mBeamStrategy->SetMprGraphicObject(mpr);
        mBeamStrategy->SetNormalGroupCollection(nullptr);
    }
//     if (mVOIStrategy != nullptr)
//     {
//         mVOIStrategy->SetVOICollectionGO(voi);
//         mVOIStrategy->SetVOICacheGOCollection(voiCacheCollection);
// 
//         mVOIStrategy->SetVolumeGO(volume);
//         mVOIStrategy->SetMPRGO(mpr);
//     }
    if (mRoiStrategy != nullptr)
    {
        mRoiStrategy->SetVolumeGO(volume);
        mRoiStrategy->SetMPRGO(mpr);
        mRoiStrategy->SetVOIGOCollection(voi);
        //mRoiStrategy->SetComposableCSGOCollection(cs);
    }
    if (mDoseStrategy != nullptr)
    {
        mDoseStrategy->SetMPRGraphicObject(mpr);
    }
    if (m_spPoiStrategy != nullptr)
    {
        m_spPoiStrategy->SetMprGraphicObject(mpr);
        m_spPoiStrategy->SetVolumeGraphicObject(volume);
        m_spPoiStrategy->SetPoiGraphicObjectCollection(poi);
    }
    if (m_spMaxPointStrategy != nullptr)
    {
        m_spMaxPointStrategy->SetMprGraphicObject(mpr);
        m_spMaxPointStrategy->SetVolumeGraphicObject(volume);
    }
    if (mDoseGridBoundaryStrategy  != nullptr)
    {
        mDoseGridBoundaryStrategy->SetVolumeGraphicObject(volume);
        mDoseGridBoundaryStrategy->SetMprGraphicObject(mpr);
        mDoseGridBoundaryStrategy->SetDoseGridBoundaryGO(nullptr);
    }
    if (mSeriesTableRenserStrategy != nullptr)
    {
        mSeriesTableRenserStrategy->SetVolumeGO(volume);
        mSeriesTableRenserStrategy->SetMPRGO(mpr);
        mSeriesTableRenserStrategy->SetSeriesTableCacheGO(seriesTableGO);
    }
    return true;
}

bool TpsCommonMPRRenderController::ResetPlan_i(){
    std::string planUID = mDataSourceMap[PLAN];
    if (mSectionType == EMPTY_COMPONENT){
        TPS_LOG_DEV_ERROR<<"The section type is unknown.";
        return false;
    }

    //normal group collection
    IGraphicObjectPtr goNGCollection = nullptr;
    if (!CheckGraphicObject_i(planUID, GO_TYPE_NORMALGROUP_COLLECTION, goNGCollection)){
        TPS_LOG_DEV_ERROR<<"Failed to check normal group collection of try: "<<planUID;
        return false;
    }

    //dose
    IGraphicObjectPtr goDose = nullptr;
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(DOSE, mSectionType, mLocation);
    if (!CheckGraphicObject_i(planUID, goType, goDose)){
         TPS_LOG_DEV_ERROR<<"Failed to check dose graphic object of try: "<<planUID;
         return false;
    }

    //dose line
    IGraphicObjectPtr goDoseLine = nullptr;
    if (!CheckGraphicObject_i(planUID, GO_TYPE_DOSE_LINE_SETTING, goDoseLine)){
        TPS_LOG_DEV_ERROR<<"Failed to check dose line object ";
        return false;
    }

    //DoseGridBoundary
    IGraphicObjectPtr goDoseGridBoundary = nullptr;
    if (!CheckGraphicObject_i(planUID, GO_TYPE_DOSE_GRID_BOUNDARY, goDoseGridBoundary)) {
         TPS_LOG_DEV_ERROR<<"Failed to check dose grid boundary graphic object of try: "<<planUID;
         return false;
    }

    //POI
    IGraphicObjectPtr goPoi = nullptr;
    if(!CheckGraphicObject_i(planUID, GO_TYPE_MAX_POINT_COLLECTION, goPoi)) {
        TPS_LOG_DEV_ERROR<<"Failed to check Max Point graphic object collection of plan:"<<planUID;
        return false;
    }
    //poi
    auto poi = std::dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(goPoi);
    m_spMaxPointStrategy->SetPoiGraphicObjectCollection(poi);

    //reset strategy data
    mBeamStrategy->SetNormalGroupCollection(
        std::dynamic_pointer_cast<NormalGroupCollection>(goNGCollection));

    //reset dose strategy data & color spectra strategy 
    auto dose = std::dynamic_pointer_cast<TpsMPRDoseGraphicObject>(goDose);
    mDoseStrategy->SetDoseGraphicObject(dose);

    //reset dose line strategy 
    auto doseline = std::dynamic_pointer_cast<TpsDoseLineGraphicObject>(goDoseLine);
    mDoseStrategy->SetDoseLineObject(doseline );

    //reset dose line boundary strategy 
    mDoseGridBoundaryStrategy->SetDoseGridBoundaryGO(
        std::dynamic_pointer_cast<TpsDoseGridBoundaryGraphicObject>(goDoseGridBoundary));

    m_spMprHotSpotsRenderStrategy->SetCurrentPlanUid(planUID);

    return true;
}

HitTestResult TpsCommonMPRRenderController::HitTest(double screenX, double screenY)
{
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    HitTestResult hitRes;
    //chess
    HitChess_i(screenX, screenY, hitRes);
    if (hitRes._hitNum != HIT_NUM_NONE) return hitRes;
    //DoseGrid
    HitDoseGrid_i(screenX, screenY, hitRes);
    if (hitRes._hitNum != HIT_NUM_NONE) return hitRes;
    //beam
    HitBeam_i(screenX, screenY, hitRes);
    if (hitRes._hitNum != HIT_NUM_NONE) return hitRes;
    return hitRes;
}

void TpsCommonMPRRenderController::RemoveDataSource_i(DATA_SOURCE_ID id)
{
    TpsMPRRenderController::RemoveDataSource_i(id);

    switch (id) {
    case IMAGE:
        if (mMprStrategy != nullptr)
        {
            mMprStrategy->SetVolume4dGraphicObject(nullptr);
        }
        if (m_spMprHotSpotsRenderStrategy != nullptr) {
            m_spMprHotSpotsRenderStrategy->SetMPRGraphicObject(nullptr);
            m_spMprHotSpotsRenderStrategy->SetVolumeGraphicObject(nullptr);
        }
        if (mBeamStrategy != nullptr)
        {
            mBeamStrategy->SetVolumeGraphicObject(nullptr);
            mBeamStrategy->SetMprGraphicObject(nullptr);
            mBeamStrategy->SetNormalGroupCollection(nullptr);
        }
//         if (mVOIStrategy != nullptr)
//         {
//             mVOIStrategy->SetVolumeGO(nullptr);
//             mVOIStrategy->SetMPRGO(nullptr);
//             mVOIStrategy->SetVOICollectionGO(nullptr);
//             mVOIStrategy->SetVOICacheGOCollection(nullptr);
//         }
        if (mRoiStrategy != nullptr)
        {
            mRoiStrategy->SetVolumeGO(nullptr);
            mRoiStrategy->SetMPRGO(nullptr);
            //mRoiStrategy->SetComposableCSGOCollection(nullptr);
            mRoiStrategy->SetVOIGOCollection(nullptr);
        }
        if (mDoseStrategy != nullptr)
        {
            mDoseStrategy->SetMPRGraphicObject(nullptr);
        }
        if (mDoseGridBoundaryStrategy  != nullptr)
        {
            mDoseGridBoundaryStrategy->SetVolumeGraphicObject(nullptr);
            mDoseGridBoundaryStrategy->SetMprGraphicObject(nullptr);
            mDoseGridBoundaryStrategy->SetDoseGridBoundaryGO(nullptr);
        }
        if (mSeriesTableRenserStrategy != nullptr)
        {
            mSeriesTableRenserStrategy->SetVolumeGO(nullptr);
            mSeriesTableRenserStrategy->SetMPRGO(nullptr);
            mSeriesTableRenserStrategy->SetSeriesTableCacheGO(nullptr);
        }
        if (m_spPoiStrategy != nullptr)
        {
            m_spPoiStrategy->SetMprGraphicObject(nullptr);
            m_spPoiStrategy->SetVolumeGraphicObject(nullptr);
            m_spPoiStrategy->SetPoiGraphicObjectCollection(nullptr);
        }
        if (m_spMaxPointStrategy != nullptr)
        {
            m_spMaxPointStrategy->SetMprGraphicObject(nullptr);
            m_spMaxPointStrategy->SetVolumeGraphicObject(nullptr);
        }
        break;
    case PLAN:
        if (mBeamStrategy != nullptr)
        {
            mBeamStrategy->SetNormalGroupCollection(nullptr);
        }
        if (mDoseStrategy != nullptr)
        {
            mDoseStrategy->SetDoseGraphicObject(nullptr);
            mDoseStrategy->SetDoseLineObject(nullptr);
        }
        if (mDoseGridBoundaryStrategy != nullptr)
        {
            mDoseGridBoundaryStrategy->SetDoseGridBoundaryGO(nullptr);
        }
        if (m_spMaxPointStrategy != nullptr)
        {
            m_spMaxPointStrategy->SetPoiGraphicObjectCollection(nullptr);
        }
    default:
        break;
    }
}

void TpsCommonMPRRenderController::ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid)
{
    TpsMPRRenderController::ClearGraphicObjects_i(id, uid);

    if (id == IMAGE) {
        std::string imageUID = uid;
        bool bVolume4dSeries = false;
        auto spSeriesGroup4d = mDataRepository->GetImageDataManager()->GetSeriesGroup4d(uid);
        if (spSeriesGroup4d != nullptr)
        {
            bVolume4dSeries = true;
            imageUID = spSeriesGroup4d->m_strMainSeriesUid;
        }
        if (bVolume4dSeries)
        {
            RemoveGraphicObject_i(imageUID, GO_TYPE_VOLUME_D4);
            for(int i = 0; i < spSeriesGroup4d->m_vecSeriesUid.size(); ++i){
                RemoveGraphicObject_i(spSeriesGroup4d->m_vecSeriesUid[i], GO_TYPE_VOLUME);
            }
        }
        GO_TYPE goType = GOTypeHelper::ComposeAsGOType(CROSSHAIR, mSectionType, mLocation);
        RemoveGraphicObject_i(imageUID, goType);
        RemoveGraphicObject_i(imageUID, GO_TYPE_MANUL_REMOVE_TABLE);
        RemoveGraphicObject_i(imageUID, GO_TYPE_VOI_COLLECTION);
        RemoveGraphicObject_i(imageUID, GO_TYPE_POI_COLLECTION);
        goType = GOTypeHelper::ComposeAsGOType(VOI_RENDERINGCACHE_COLLECTION, mSectionType, mLocation);
        RemoveGraphicObject_i(imageUID, goType);
        //RemoveGraphicObject_i(imageUID, GO_TYPE_ROI_CONTOUR_SET_COLLECTION, mSectionType);
        RemoveGraphicObject_i(imageUID, GO_TYPE_LIMIT_REGION);
        goType = GOTypeHelper::ComposeAsGOType(SERIES_TABLE_CACHE, mSectionType, mLocation);
        RemoveGraphicObject_i(imageUID, goType);
    }
    else if (id == PLAN){
        RemoveGraphicObject_i(uid, GO_TYPE_MAX_POINT_COLLECTION);
        RemoveGraphicObject_i(uid, GO_TYPE_NORMALGROUP_COLLECTION);
        GO_TYPE goType = GOTypeHelper::ComposeAsGOType(DOSE, mSectionType, mLocation);
        RemoveGraphicObject_i(uid, goType);
        RemoveGraphicObject_i(uid, GO_TYPE_DOSE_LINE_SETTING);
        RemoveGraphicObject_i(uid, GO_TYPE_DOSE_GRID_BOUNDARY);
    }
}

TPS_END_NAMESPACE  // end namespace tps
