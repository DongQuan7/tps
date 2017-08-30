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
#include "tps_bl_mprprinter.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"

#include "RtTpsRenderLibrary/tps_rl_fusionrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_beamrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mprdoserenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_roi_render_strategy.h"
#include "RtTpsRenderLibrary/tps_rl_poi_renderstrategy.h"               // for TpsPoiRenderStrategy
#include "RtTpsRenderLibrary/tps_rl_poitextrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_scalerulerstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_colorspectrarenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mascot_renderstrategy.h"            // for MascotRenderStrategy
#include "RtTpsRenderLibrary/tps_rl_cornertextinforenderstrategy.h"
//#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_mprdosegraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_doselineobject.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_axialpageturn_operation.h"
#include "RtTpsRenderLibrary/tps_rl_refreshdoseoperation.h"
#include "RtTpsRenderLibrary/tps_rl_cornerinfographicobjectconverter.h"

#include "RtTpsFramework/tps_fw_fire_operation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_go_converter_parameter.h"

//TODO: why need to add this
#undef DLL_EXPORT
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"

TPS_BEGIN_NAMESPACE

    MPRPrinter::MPRPrinter(int width, int height, ITpsDataRepository* mDataRepository)
    : PrinterBase(width, height, mDataRepository),
    mFusionStrategy(nullptr),
    mBeamStrategy(nullptr),
    mVOIStrategy(nullptr),
    mScaleRulerStrategy(nullptr),
    mDoseStrategy(nullptr),
    mColorSpectraStrategy(nullptr),
    m_spPoiStrategy(nullptr),
    m_spMascotRenderStrategy(nullptr),
    mPageTurnOperation(nullptr),
    mRefreshDoseOperation(nullptr) {

}

MPRPrinter::~MPRPrinter() {

}

bool MPRPrinter::PreparePrinting()
{
	std::string  strWindowName = "Axial";

     mFusionStrategy.reset(new FusionRenderStrategy());
	 mClsView->AddRenderStrategy(0, mFusionStrategy, Mcsf::MedViewer3D::ClsView::IMAGE);

 //   /////////////////////////GRAPHIC Overlay////////////////////////////
    m_spPoiStrategy.reset(new TpsPoiRenderStrategy());
    mClsView->AddRenderStrategy(0, m_spPoiStrategy, Mcsf::MedViewer3D::ClsView::GRAPHIC);

    mDoseStrategy.reset(new TpsMPRDoseRenderStrategy());
    //mClsView->AddRenderStrategy(0, mDoseStrategy, Mcsf::MedViewer3D::ClsView::eOverlay(OVERLAY_GRAPHIC));
    mClsView->AddRenderStrategy(0, mDoseStrategy, Mcsf::MedViewer3D::ClsView::GRAPHIC);

    mBeamStrategy.reset(new TpsBeamRenderStrategy());
    mBeamStrategy->SetSectionType(AXIAL);// for ARC beams
    mClsView->AddRenderStrategy(0, mBeamStrategy, Mcsf::MedViewer3D::ClsView::GRAPHIC);

    /////////////////////////VOI Overlay////////////////////////////
    mVOIStrategy.reset(new MprRoiRenderStrategy());
    mClsView->AddRenderStrategy(0, mVOIStrategy, Mcsf::MedViewer3D::ClsView::GRAPHIC); //ROIPOS TODO

    m_spMascotRenderStrategy.reset(new MascotRenderStrategy());
    m_spMascotRenderStrategy->SetName(strWindowName);
    mClsView->AddRenderStrategy(0, m_spMascotRenderStrategy, Mcsf::MedViewer3D::ClsView::GRAPHIC);

    mScaleRulerStrategy.reset(new TpsScaleRulerStrategy());
    mClsView->AddRenderStrategy(0, mScaleRulerStrategy, Mcsf::MedViewer3D::ClsView::GRAPHIC);

    mColorSpectraStrategy.reset(new TpsColorSpectraRenderStrategy());
    mClsView->AddRenderStrategy(0, mColorSpectraStrategy, Mcsf::MedViewer3D::ClsView::GRAPHIC);

    ////////////////////////////Text strategy////////////////////////////////////
    mCornerTextInfoStrategy = new CornerTextInfoRenderStrategy();
    if (!RegisterCPURenderStrategy("CornerTextInfo", mCornerTextInfoStrategy)) {
        TPS_LOG_DEV_WARNING<<"Failed to RegisterCPURenderStrategy CornerTextInfo";
    }

    mPoiTypeTextStrategy = new POITypeTextRenderStrategy();
    mPoiTypeTextStrategy->SetDataRepository(mDataRepository);
    if(!RegisterCPURenderStrategy("PoiTypeText", mPoiTypeTextStrategy)){
        TPS_LOG_DEV_WARNING<<"Failed to RegisterCPURenderStrategy PoiTypeText";
    }

	return true;
}

bool MPRPrinter::OnBeginPrinting()
{
    if (mPlanUID.empty() || mSeriesUID.empty()) {
        return false;
    }

    GOConverterParameter para;
    para.data_source_map[IMAGE] = mSeriesUID;
    para.data_source_map[PLAN] = mPlanUID;
    //check volume graphic object
    IGraphicObjectPtr goVolume = nullptr;
    if (!CheckGraphicObject(mSeriesUID, GO_TYPE_VOLUME, goVolume, para)) {
        TPS_LOG_DEV_ERROR<<"Failed to check volume graphic object of image: "<<mSeriesUID;
        return false;
    }

    auto volume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(goVolume);

    mFusionStrategy->SetVolumeGO(volume);
    mVOIStrategy->SetVolumeGO(volume);
    mBeamStrategy->SetVolumeGraphicObject(volume);
    m_spPoiStrategy->SetVolumeGraphicObject(volume);
    mScaleRulerStrategy->SetVolumeGraphicObject(volume);

    mCornerTextInfoStrategy->SetVolumeGraphicObject(volume);
    mPoiTypeTextStrategy->SetVolumeGraphicObject(volume);

    //check mpr graphic object
    IGraphicObjectPtr goMpr = nullptr;
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, AXIAL, EMPTY_COMPONENT);
    if (!CheckGraphicObject(mSeriesUID, goType, goMpr, para)){
        TPS_LOG_DEV_ERROR<<"Failed to check MPR graphic object of image: "<<mSeriesUID;
        return false;
    }

    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(goMpr);

    mFusionStrategy->SetMPRGO(mpr);
    mVOIStrategy->SetMPRGO(mpr);
    m_spMascotRenderStrategy->SetMPRGraphicObject(mpr);
    mBeamStrategy->SetMprGraphicObject(mpr);
    mDoseStrategy->SetMPRGraphicObject(mpr);
    m_spPoiStrategy->SetMprGraphicObject(mpr);
    mScaleRulerStrategy->SetMprGraphicObject(mpr);

    mCornerTextInfoStrategy->SetMprGraphicObject(mpr);
    mPoiTypeTextStrategy->SetMprGraphicObject(mpr);

    //check voi collection graphic object
    IGraphicObjectPtr goVOI = nullptr;
    if (!CheckGraphicObject(mSeriesUID, GO_TYPE_VOI_COLLECTION, goVOI, para)){
        TPS_LOG_DEV_ERROR<<"Failed to check VOI graphic object collection of image: "<<mSeriesUID;
        return false;
    }

    auto voi = std::dynamic_pointer_cast<TpsVOIGOCollection>(goVOI);

    mVOIStrategy->SetVOIGOCollection(voi);

    //check poi graphic object
    IGraphicObjectPtr goPoi = nullptr;
    if(!CheckGraphicObject(mPlanUID, GO_TYPE_POI_COLLECTION, goPoi, para)) {
        TPS_LOG_DEV_ERROR<<"Failed to check Poi graphic object collection of plan:"<<mPlanUID;
        return false;
    }

    auto poi = std::dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(goPoi);
    m_spPoiStrategy->SetPoiGraphicObjectCollection(poi);

    mPoiTypeTextStrategy->SetPoiGraphicObjectCollection(poi);


    //check normal group collection graphic object
    IGraphicObjectPtr goNGCollection = nullptr;
    if (!CheckGraphicObject(mPlanUID, GO_TYPE_NORMALGROUP_COLLECTION, goNGCollection, para)){
        TPS_LOG_DEV_ERROR<<"Failed to check normal group collection of try: "<<mPlanUID;
        return false;
    }

    auto ngCollection = std::dynamic_pointer_cast<NormalGroupCollection>(goNGCollection);

    mBeamStrategy->SetNormalGroupCollection(ngCollection);

    //check dose graphic object
    IGraphicObjectPtr goDose = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(DOSE, AXIAL, EMPTY_COMPONENT);
    if (!CheckGraphicObject(mPlanUID, goType, goDose, para)){
        TPS_LOG_DEV_ERROR<<"Failed to check dose graphic object of try: "<<mPlanUID;
        return false;
    }

    auto dose = std::dynamic_pointer_cast<TpsMPRDoseGraphicObject>(goDose);
    mDoseStrategy->SetDoseGraphicObject(dose);

    //check dose line graphic object
    IGraphicObjectPtr goDoseLine = nullptr;
    if (!CheckGraphicObject(mPlanUID, GO_TYPE_DOSE_LINE_SETTING, goDoseLine, para)){
        TPS_LOG_DEV_ERROR<<"Failed to check dose line object ";
        return false;
    }

    auto doseline = std::dynamic_pointer_cast<TpsDoseLineGraphicObject>(goDoseLine);

    mDoseStrategy->SetDoseLineObject(doseline);
    mColorSpectraStrategy->SetDoseGraphicObject( doseline);

    IGraphicObjectPtr goMprCornerInfo = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(MPR_CORNER_TEXT, AXIAL, EMPTY_COMPONENT);
    if (!CheckGraphicObject(mSeriesUID, goType, goMprCornerInfo, para)){
        TPS_LOG_DEV_ERROR<<"Failed to check MPR corner info graphic object of image: "<<mSeriesUID;
        return false;
    }

    auto mprCornerInfo = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(goMprCornerInfo);

    RtPlan* plan;
    mDataRepository->GetPatientManager()->GetPlan(mPlanUID, &plan);
    if (nullptr != plan)
    {
        std::string planName = plan->get_name();
        if (!planName.empty())
        {
            Pixel32* pStrBuffer = nullptr;
            int strWidth = 0;
            int strHeight = 0;
            std::wstring wPlanName;
            wPlanName.assign(planName.begin(), planName.end());

            if (!FTStringBufferGenerator::GetInstance()->GenerateRasterBuffer(wPlanName, &pStrBuffer, strWidth, strHeight))
            {
                TPS_LOG_DEV_ERROR<<"Failed to generate raster buffer for"<<planName;
            }

            mprCornerInfo->UpdateTextBlock("PlanName", (unsigned char*)pStrBuffer, strWidth, strHeight);
        }
    }

    mCornerTextInfoStrategy->SetCornerTextInfoGraphicObject(mprCornerInfo);

    IGraphicObjectPtr goMprRulerText = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(MPR_RULER_TEXT, AXIAL, EMPTY_COMPONENT);
    if (!CheckGraphicObject(mSeriesUID, goType, goMprRulerText, para)){
        TPS_LOG_DEV_ERROR<<"Failed to check MPR ruler text graphic object of image: "<<mSeriesUID;
        return false;
    }

    auto mprRulerText = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(goMprRulerText);

    mScaleRulerStrategy->SetRulerTextInfoGraphicObject(mprRulerText);

    mPageTurnOperation.reset(new AxialPageTurnOperation(mSeriesUID));
    mRefreshDoseOperation.reset(new TpsRefreshDoseOperation(mPlanUID, AXIAL));

    return true;
}


PRINT_STATE MPRPrinter::OnPrinting()
{
    int requestSliceNum = mSeriesSlices.front();

    mPageTurnOperation->SetTargetSliceNum(requestSliceNum);

    mPageTurnOperation->Initialize(mModelWarehouse,mDataRepository,mConverterFactory);
    mPageTurnOperation->Execute();

    mRefreshDoseOperation->Initialize(mModelWarehouse,mDataRepository,mConverterFactory);
    mRefreshDoseOperation->Execute();

    mSeriesSlices.pop();

    return mSeriesSlices.empty() ? FINISH : CONTINUE;
}

bool MPRPrinter::OnEndPrinting()
{
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

void MPRPrinter::ClearModelWarehouse()
{
    if (nullptr == mModelWarehouse)
    {
        return;
    }
    mModelWarehouse->ClearAll();
}

TPS_END_NAMESPACE