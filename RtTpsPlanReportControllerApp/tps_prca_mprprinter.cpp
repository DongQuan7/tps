//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_prca_mprprinter.cpp
///
/// \brief   MPRPrinter
/// 
/// \version 1.0
/// 
/// \date    2015/05/04
/////////////////////////////////////////////////////////////////////////
#include"stdafx.h"

#include "RtTpsDataAccess/tps_da_patientmanager.h"

#include "RtTpsRenderLibrary/tps_rl_fusionrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_beamrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mprdoserenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_voirenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_poi_renderstrategy.h"               // for TpsPoiRenderStrategy
#include "RtTpsRenderLibrary/tps_rl_poitextrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_scalerulerstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_colorspectrarenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mascot_renderstrategy.h"            // for MascotRenderStrategy
#include "RtTpsRenderLibrary/tps_rl_cornertextinforenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_voirenderingcachegocollection.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_mprdosegraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_doselineobject.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_axialpageturn_operation.h"
#include "RtTpsRenderLibrary/tps_rl_updatecachecollectionoperation.h"
#include "RtTpsRenderLibrary/tps_rl_refreshdoseoperation.h"

#include "RtTpsFramework/tps_rl_fire_operation.h"
#include "RtTpsFramework/tps_rl_modelwarehouse.h"
#include "RtTpsFramework/tps_rl_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsFramework/tps_rl_modelwarehouse.h"

#include "RtTpsRenderController/tps_rc_renderdefines.h"

#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "tms/rt_fwk_dl_object_plan.h"

#include "tps_prca_mprprinter.h"

TPS_BEGIN_NAMESPACE

MPRPrinter::MPRPrinter(int width, int height)
    : PrinterBase(width, height),
    mFusionStrategy(nullptr),
    mBeamStrategy(nullptr),
    mVOIStrategy(nullptr),
    mScaleRulerStrategy(nullptr),
    mDoseStrategy(nullptr),
    mColorSpectraStrategy(nullptr),
    m_spPoiStrategy(nullptr),
    m_spMascotRenderStrategy(nullptr),
    mPageTurnOperation(nullptr),
    mUpdateVOICacheCollectionOperation(nullptr),
    mRefreshDoseOperation(nullptr) {

}

MPRPrinter::~MPRPrinter() {

}

bool MPRPrinter::PreparePrinting() {

    std::string  strWindowName = "Axial";

    Mcsf::MedViewer3D::ClsView::BlendMode blendMode(GL_ADD, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    mClsView->RemoveGPUOverlay(Mcsf::MedViewer3D::ClsView::GRAPHIC);
    //here use our overlay type, so we can control the blending order.
    mClsView->AddGPUOverlay(Mcsf::MedViewer3D::ClsView::eOverlay(OVERLAY_VOI), blendMode);
    mClsView->AddGPUOverlay(Mcsf::MedViewer3D::ClsView::eOverlay(OVERLAY_GRAPHIC), blendMode);
    mClsView->AddGPUOverlay(Mcsf::MedViewer3D::ClsView::eOverlay(OVERLAY_COVER), blendMode);

    mFusionStrategy.reset(new FusionRenderStrategy(false));
    mClsView->AddRenderStrategy(0, mFusionStrategy, Mcsf::MedViewer3D::ClsView::IMAGE);

    /////////////////////////GRAPHIC Overlay////////////////////////////
    m_spPoiStrategy.reset(new TpsPoiRenderStrategy());
    mClsView->AddRenderStrategy(0, m_spPoiStrategy, Mcsf::MedViewer3D::ClsView::eOverlay(OVERLAY_GRAPHIC));

    mDoseStrategy.reset(new TpsMPRDoseRenderStrategy());
    mClsView->AddRenderStrategy(0, mDoseStrategy, Mcsf::MedViewer3D::ClsView::eOverlay(OVERLAY_GRAPHIC));

    mBeamStrategy.reset(new TpsBeamRenderStrategy());
    mBeamStrategy->SetViewWindowType(ANASTRUCTEDITOR_AXIAL);// for ARC beams
    mClsView->AddRenderStrategy(0, mBeamStrategy, Mcsf::MedViewer3D::ClsView::eOverlay(OVERLAY_GRAPHIC));

    /////////////////////////VOI Overlay////////////////////////////
    mVOIStrategy.reset(new TpsVoiRenderStrategy());
    mClsView->AddRenderStrategy(0, mVOIStrategy, Mcsf::MedViewer3D::ClsView::eOverlay(OVERLAY_VOI)); //ROIPOS TODO

    m_spMascotRenderStrategy.reset(new MascotRenderStrategy());
    m_spMascotRenderStrategy->SetName(strWindowName);
    mClsView->AddRenderStrategy(0, m_spMascotRenderStrategy, Mcsf::MedViewer3D::ClsView::eOverlay(OVERLAY_COVER));

    mScaleRulerStrategy.reset(new TpsScaleRulerStrategy());
    mClsView->AddRenderStrategy(0, mScaleRulerStrategy, Mcsf::MedViewer3D::ClsView::eOverlay(OVERLAY_COVER));

    mColorSpectraStrategy.reset(new TpsColorSpectraRenderStrategy());
    mClsView->AddRenderStrategy(0, mColorSpectraStrategy, Mcsf::MedViewer3D::ClsView::eOverlay(OVERLAY_GRAPHIC));

    ////////////////////////////Text strategy////////////////////////////////////
    mCornerTextInfoStrategy = new CornerTextInfoRenderStrategy();
    if (!RegisterCPURenderStrategy("CornerTextInfo", mCornerTextInfoStrategy)) {
        TPS_LOG_DEV_WARNING<<"Failed to RegisterCPURenderStrategy CornerTextInfo";
    }

    mPoiTypeTextStrategy = new POITypeTextRenderStrategy();
    if(!RegisterCPURenderStrategy("PoiTypeText", mPoiTypeTextStrategy)){
        TPS_LOG_DEV_WARNING<<"Failed to RegisterCPURenderStrategy PoiTypeText";
    }

    return true;
}

bool MPRPrinter::OnBeginPrinting() {

    if (mPlanUID.empty() || mSeriesUID.empty()) {
        return false;
    }

    //check volume graphic object
    IGraphicObjectPtr goVolume = nullptr;
    if (!CheckGraphicObject(mSeriesUID, GO_TYPE_VOLUME, goVolume)) {
        TPS_LOG_DEV_ERROR<<"Failed to check volume graphic object of image: "<<mSeriesUID;
        return false;
    }

    auto volume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(goVolume);

    mFusionStrategy->SetFixedVolumeGraphicObject(volume);
    mVOIStrategy->SetVolumeGO(volume);
    mBeamStrategy->SetVolumeGraphicObject(volume);
    m_spPoiStrategy->SetVolumeGraphicObject(volume);
    mCornerTextInfoStrategy->SetVolumeGraphicObject(volume);
    mScaleRulerStrategy->SetVolumeGraphicObject(volume);

    //check mpr graphic object
    IGraphicObjectPtr goMpr = nullptr;
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, AXIAL, EMPTY_COMPONENT);
    if (!CheckGraphicObject(mSeriesUID, goType, goMpr)){
        TPS_LOG_DEV_ERROR<<"Failed to check MPR graphic object of image: "<<mSeriesUID;
        return false;
    }

    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(goMpr);

    mFusionStrategy->SetFixedMprGraphicObject(mpr);
    mVOIStrategy->SetMPRGO(mpr);
    m_spMascotRenderStrategy->SetMPRGraphicObject(mpr);
    mBeamStrategy->SetMprGraphicObject(mpr);
    mDoseStrategy->SetMPRGraphicObject(mpr);
    m_spPoiStrategy->SetMprGraphicObject(mpr);
    mCornerTextInfoStrategy->SetMprGraphicObject(mpr);
    mScaleRulerStrategy->SetMprGraphicObject(mpr);

    //check voi collection graphic object
    IGraphicObjectPtr goVOI = nullptr;
    if (!CheckGraphicObject(mSeriesUID, GO_TYPE_VOI_COLLECTION, goVOI)){
        TPS_LOG_DEV_ERROR<<"Failed to check VOI graphic object collection of image: "<<mSeriesUID;
        return false;
    }

    auto voi = std::dynamic_pointer_cast<TpsVOIGOCollection>(goVOI);

    mVOIStrategy->SetVOICollectionGO(voi);

    //check voi rendering cache collection graphic object
    IGraphicObjectPtr goVoiRenderingCacheCollection = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(VOI_RENDERINGCACHE_COLLECTION, AXIAL, EMPTY_COMPONENT);
    if (!CheckGraphicObject(mSeriesUID, goType, goVoiRenderingCacheCollection)){
        TPS_LOG_DEV_ERROR<<"Failed to check VOI render cache of image: "
            <<mSeriesUID<<" sectionPage: "<<goType;
        return false;
    }

    auto voiCacheCollection = std::dynamic_pointer_cast<
        TpsVOIRenderingCacheGOCollection>(goVoiRenderingCacheCollection);

    mVOIStrategy->SetVOICacheGOCollection(voiCacheCollection);

    //check poi graphic object
    IGraphicObjectPtr goPoi = nullptr;
    if(!CheckGraphicObject(mPlanUID, GO_TYPE_POI_COLLECTION, goPoi)) {
        TPS_LOG_DEV_ERROR<<"Failed to check Poi graphic object collection of plan:"<<mPlanUID;
        return false;
    }

    auto poi = std::dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(goPoi);
    m_spPoiStrategy->SetPoiGraphicObjectCollection(poi);
    mPoiTypeTextStrategy->SetPoiGraphicObjectCollection(poi);

    //check normal group collection graphic object
    IGraphicObjectPtr goNGCollection = nullptr;
    if (!CheckGraphicObject(mPlanUID, GO_TYPE_NORMALGROUP_COLLECTION, goNGCollection)){
        TPS_LOG_DEV_ERROR<<"Failed to check normal group collection of try: "<<mPlanUID;
        return false;
    }

    auto ngCollection = std::dynamic_pointer_cast<NormalGroupCollection>(goNGCollection);

    mBeamStrategy->SetNormalGroupCollection(ngCollection);

    //check dose graphic object
    IGraphicObjectPtr goDose = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(DOSE, AXIAL, EMPTY_COMPONENT);
    if (!CheckGraphicObject(mPlanUID, goType, goDose)){
        TPS_LOG_DEV_ERROR<<"Failed to check dose graphic object of try: "<<mPlanUID;
        return false;
    }

    auto dose = std::dynamic_pointer_cast<TpsMPRDoseGraphicObject>(goDose);
    mDoseStrategy->SetDoseGraphicObject(dose);

    //check dose line graphic object
    IGraphicObjectPtr goDoseLine = nullptr;
    if (!CheckGraphicObject(mPlanUID, GO_TYPE_DOSE_LINE_SETTING, goDoseLine)){
        TPS_LOG_DEV_ERROR<<"Failed to check dose line object ";
        return false;
    }

    auto doseline = std::dynamic_pointer_cast<TpsDoseLineGraphicObject>(goDoseLine);

    mDoseStrategy->SetDoseLineObject(doseline);
    mColorSpectraStrategy->SetDoseGraphicObject( doseline);

    IGraphicObjectPtr goMprCornerInfo = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(MPR_CORNER_TEXT, AXIAL, EMPTY_COMPONENT);
    if (!CheckGraphicObject(mSeriesUID, goType, goMprCornerInfo)){
        TPS_LOG_DEV_ERROR<<"Failed to check MPR corner info graphic object of image: "<<mSeriesUID;
        return false;
    }

    auto mprCornerInfo = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(goMprCornerInfo);

    RTFWK::RTFWKDLDOPlan* plan = nullptr;
    if (TpsPatientManager::GetInstance()->GetPlan(mPlanUID, &plan) && plan != nullptr) {

        std::string planName = plan->GetName();
        if (!planName.empty()) {
            Pixel32* pStrBuffer = nullptr;
            int strWidth = 0;
            int strHeight = 0;
            std::wstring wPlanName;
            wPlanName.assign(planName.begin(), planName.end());

            if (!FTStringBufferGenerator::GetInstance()->GenerateRasterBuffer(wPlanName, &pStrBuffer, strWidth, strHeight)) {
                TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for"<<planName;
            }

            mprCornerInfo->UpdateTextBlock("Plan", (unsigned char*)pStrBuffer, strWidth, strHeight);
        }
    }

    mCornerTextInfoStrategy->SetCornerTextInfoGraphicObject(mprCornerInfo);


    IGraphicObjectPtr goMprRulerText = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(MPR_RULER_TEXT, AXIAL, EMPTY_COMPONENT);
    if (!CheckGraphicObject(mSeriesUID, goType, goMprRulerText)){
        TPS_LOG_DEV_ERROR<<"Failed to check MPR ruler text graphic object of image: "<<mSeriesUID;
        return false;
    }

    auto mprRulerText = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(goMprRulerText);

    mScaleRulerStrategy->SetRulerTextInfoGraphicObject(mprRulerText);

    mPageTurnOperation.reset(new AxialPageTurnOperation(mSeriesUID));
    mUpdateVOICacheCollectionOperation.reset(new TpsUpdateVOICacheCollectionOperation(mSeriesUID, AXIAL));
    mRefreshDoseOperation.reset(new TpsRefreshDoseOperation(mPlanUID, AXIAL));

    return true;
}

PRINT_STATE MPRPrinter::OnPrinting() {

    int requestSliceNum = mSeriesSlices.front();

    mPageTurnOperation->SetTargetSliceNum(requestSliceNum);
    ModelWarehouse::GetInstance()->ProcessOperation(mPageTurnOperation);

    ModelWarehouse::GetInstance()->ProcessOperation(mUpdateVOICacheCollectionOperation);

    ModelWarehouse::GetInstance()->ProcessOperation(mRefreshDoseOperation);

    mSeriesSlices.pop();

    return mSeriesSlices.empty() ? FINISH : CONTINUE;
}

bool MPRPrinter::OnEndPrinting() {
    return true;
}

void MPRPrinter::SetPlanUID(const std::string planUID) {
    mPlanUID = planUID;
}

void MPRPrinter::SetSeriesUID(const std::string seriesUID) {
    mSeriesUID = seriesUID;
}

void MPRPrinter::SetSeriesSlices(std::queue<int>& seriesSlices) {
    mSeriesSlices = seriesSlices;
}

TPS_END_NAMESPACE
