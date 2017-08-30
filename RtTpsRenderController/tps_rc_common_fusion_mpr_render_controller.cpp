////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wang Jie  jie.wang@united-imaging.com
/// 
///  \file tps_rc_common_fusion_mpr_render_controller.cpp
/// 
///  \brief class TpsCommonFusionMPRRenderController implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/06/25
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderController/tps_rc_common_fusion_mpr_render_controller.h"

//ZHENGHE
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

// Render Strategies
#include "RtTpsRenderLibrary/tps_rl_dosegridboundaryrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_roi_render_strategy.h"


// Graphic Objects
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_landmarkgraphicobject.h"

// go collections
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
// frameworks
#include "RtTpsFramework/tps_fw_displaycache.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_fusionsecondarydatamanager.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsCommonFusionMPRRenderController::TpsCommonFusionMPRRenderController(WINDOW_TYPE type)
    : TpsMPRRenderController(type),
    mFusionRegionStrategy(nullptr)
{
    SetSectionType_i();
}

TpsCommonFusionMPRRenderController::~TpsCommonFusionMPRRenderController()
{
}

void TpsCommonFusionMPRRenderController::SetSectionType_i()
{
    switch(mWindowType) {
    case WINDOW_FUSION_FIXED_MPR_AXIAL:
    case WINDOW_FUSION_FLOAT_MPR_AXIAL:
        mSectionType = AXIAL;
        break;
    case WINDOW_FUSION_FIXED_MPR_SAGITTAL:
    case WINDOW_FUSION_FLOAT_MPR_SAGITTAL:
        mSectionType = SAGITTAL;
        break;
    case WINDOW_FUSION_FIXED_MPR_CORONAL:
    case WINDOW_FUSION_FLOAT_MPR_CORONAL:
        mSectionType = CORONAL;
        break;
    default:
        mSectionType = EMPTY_COMPONENT;
        break;
    }
    mLocation = EMPTY_COMPONENT;
    mWindowGroupType = (DATA_SOURCE_GROUP_ID)(mWindowType >> 8);
}

bool TpsCommonFusionMPRRenderController::SetupScene_i()
{
    if (!TpsMPRRenderController::SetupScene_i())
    {
        TPS_LOG_DEV_ERROR << "Failed to setup scene of MPRRenderController";
        return false;
    }
    try
    {
        ///////////////////////////GRAPHIC Overlay////////////////////////////
        mFusionRegionStrategy.reset(new DoseGridBoundaryRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mFusionRegionStrategy);

        //voi
        mROIStrategy.reset(new MprRoiRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mROIStrategy);

        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "Failed to setup scene: " << ex.what();
        return false;
    }
}


bool TpsCommonFusionMPRRenderController::ResetImage_i()
{
    TpsMPRRenderController::ResetImage_i();

    std::string imageUID = mDataSourceMap[IMAGE];
    //check volume graphic object
    IGraphicObjectPtr goVolume = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_VOLUME, goVolume))
    {
        TPS_LOG_DEV_ERROR << "Failed to check volume graphic object of image: " << imageUID;
        return false;
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

    //landmark
    IGraphicObjectPtr goLandmark = nullptr;
    if (!CheckGraphicObject_i(COMMON_BASE, GO_TYPE_LANDMARK, goLandmark))
    {
        TPS_LOG_DEV_ERROR << "Failed to check landmark graphic object.";
        return false;
    }

    std::shared_ptr<TpsDoseGridBoundaryGraphicObject> fusionRegion = nullptr;
    IGraphicObjectPtr goFusionRegion = nullptr;
    GO_TYPE fusionRegionGoType = mWindowGroupType == SOURCE_GROUP_FUSION_FIXED ? 
         GO_TYPE_FUSION_REGION_FIXED : GO_TYPE_FUSION_REGION_FLOAT;
    if (!CheckGraphicObject_i(COMMON_BASE, fusionRegionGoType, goFusionRegion))
    {
        TPS_LOG_DEV_ERROR << "Failed to check fusion region graphic object.";
        return false;
    }
    fusionRegion = std::dynamic_pointer_cast<TpsDoseGridBoundaryGraphicObject>(goFusionRegion);

    //RTTI
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(goMpr);
    auto landmark = std::dynamic_pointer_cast<LandmarkGraphicObject>(goLandmark);
    if (volume == nullptr || mpr == nullptr || landmark == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to dynamic cast IGraphicObjects.";
        //return false;
    }

    if ((mWindowType == WINDOW_FUSION_FIXED_MPR_AXIAL) || 
        (mWindowType == WINDOW_FUSION_FIXED_MPR_CORONAL) || 
        (mWindowType == WINDOW_FUSION_FIXED_MPR_SAGITTAL) )
    {
        //check voi graphic object collection & VOI Rendering Cache graphic object
        IGraphicObjectPtr goVOI = nullptr;
        if (!CheckGraphicObject_i(imageUID, GO_TYPE_VOI_COLLECTION, goVOI)){
            TPS_LOG_DEV_ERROR<<"Failed to check VOI graphic object collection of image: "<<imageUID;
            return false;
        }
//         IGraphicObjectPtr goVoiRenderingCacheCollection = nullptr;
//         goType = GOTypeHelper::ComposeAsGOType(VOI_RENDERINGCACHE_COLLECTION, mSectionType, mLocation);
//         if (!CheckGraphicObject_i(imageUID, goType, goVoiRenderingCacheCollection)){
//             TPS_LOG_DEV_ERROR<<"Failed to check VOI render cache of image: "
//                 <<imageUID<<" sectionPage: "<<goType;
//             return false;
//         }
        auto voi = std::dynamic_pointer_cast<TpsVOIGOCollection>(goVOI);
//         auto voiCacheCollection = std::dynamic_pointer_cast<
//             TpsVOIRenderingCacheGOCollection>(goVoiRenderingCacheCollection);
        if( voi == nullptr/* || voiCacheCollection == nullptr*/){
            return false;
        }

        if (mROIStrategy != nullptr)
        {
            mROIStrategy->SetVolumeGO(volume);
            mROIStrategy->SetMPRGO(mpr);
            mROIStrategy->SetVOIGOCollection(voi);
            //mRoiStrategy->SetComposableCSGOCollection(cs);
        }
    }
    
    //fusion region render strategy
    if (mFusionRegionStrategy != nullptr && fusionRegion != nullptr)
    {
        mFusionRegionStrategy->SetVolumeGraphicObject(volume);
        mFusionRegionStrategy->SetMprGraphicObject(mpr);
        mFusionRegionStrategy->SetDoseGridBoundaryGO(fusionRegion);
    }
    return true;
}

bool TpsCommonFusionMPRRenderController::ResetPlan_i(){
    std::string planUID = mDataSourceMap[PLAN];
    if (mSectionType == EMPTY_COMPONENT){
        TPS_LOG_DEV_ERROR<<"The section type is unknown.";
        return false;
    }

    return true;
}


void TpsCommonFusionMPRRenderController::RemoveDataSource_i(DATA_SOURCE_ID id)
{
    TpsMPRRenderController::RemoveDataSource_i(id);

    switch (id) {
    case IMAGE:
        if (mFusionRegionStrategy != nullptr)
        {
            mFusionRegionStrategy->SetVolumeGraphicObject(nullptr);
            mFusionRegionStrategy->SetMprGraphicObject(nullptr);
        }
        if (mROIStrategy != nullptr)
        {
            mROIStrategy->SetVolumeGO(nullptr);
            mROIStrategy->SetMPRGO(nullptr);
            mROIStrategy->SetVOIGOCollection(nullptr);
            //mRoiStrategy->SetComposableCSGOCollection(cs);
        }
        break;
    default:
        break;
    }
}

HitTestResult TpsCommonFusionMPRRenderController::HitTest(double screenX, double screenY)
{
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    //landmark
    HitTestResult hitRes;
    if (hitRes._hitNum != HIT_NUM_NONE) return hitRes;
    HitAlignmentBox_i(screenX, screenY, hitRes);
    if (hitRes._hitNum != HIT_NUM_NONE) return hitRes;
    return hitRes;
}


void TpsCommonFusionMPRRenderController::HitAlignmentBox_i(double x, double y, HitTestResult& hitRes)
{
    if (mFusionRegionStrategy == nullptr) return;
    mFusionRegionStrategy->HitTest(x, y, hitRes);
}

void TpsCommonFusionMPRRenderController::ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid)
{
    TpsMPRRenderController::ClearGraphicObjects_i(id, uid);
    if (id == IMAGE)
    {
        RemoveGraphicObject_i(uid, GO_TYPE_VOI_COLLECTION);
    }
}

TPS_END_NAMESPACE  // end namespace tps
