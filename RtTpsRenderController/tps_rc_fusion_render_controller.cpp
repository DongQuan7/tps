////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song song.lig@united-imaging.com
/// 
///  \file tps_rc_fusion_render_controller.cpp
/// 
///  \brief class TpsFusionRenderController implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/05/12
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderController/tps_rc_fusion_render_controller.h"
#include "RtTpsRenderLibrary/tps_rl_fusiongraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_volume4d_mpr_renderstrategy.h"

//ZHENGHE
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

// Render Strategies
#include "RtTpsRenderLibrary/tps_rl_scalerulerstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_roi_render_strategy.h"


// Graphic Objects
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_fusionchessgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"

// go collections
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsFusionRenderController::TpsFusionRenderController(WINDOW_TYPE type)
    : TpsMPRRenderController(type)
{
        SetSectionType_i();
}

TpsFusionRenderController::~TpsFusionRenderController()
{

}

void TpsFusionRenderController::SetSectionType_i()
{
    switch(mWindowType) {
    case WINDOW_FUSION_MIXED_MPR_AXIAL:
        mSectionType = AXIAL;
        break;
    case WINDOW_FUSION_MIXED_MPR_SAGITTAL:
        mSectionType = SAGITTAL;
        break;
    case WINDOW_FUSION_MIXED_MPR_CORONAL:
        mSectionType = CORONAL;
        break;
    default:
        mSectionType = EMPTY_COMPONENT;
        break;
    }
    mLocation = EMPTY_COMPONENT;
    mWindowGroupType = (DATA_SOURCE_GROUP_ID)(mWindowType >> 8);
}

HitTestResult TpsFusionRenderController::HitTest(double screenX, double screenY)
{
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    HitTestResult hitRes;
    //chess
    HitChess_i(screenX, screenY, hitRes);
    if (hitRes._hitNum != HIT_NUM_NONE) return hitRes;
    return hitRes;
}

void TpsFusionRenderController::HitChess_i(double x, double y, HitTestResult& hitRes)
{
    if (mMprStrategy == nullptr) return;
    mMprStrategy->HitTest(x, y, hitRes);
}

bool TpsFusionRenderController::SetupScene_i()
{
    try
    {
        //MPR strategy
        mMprStrategy.reset(new Volume4dMprRenderStrategy());
        RegisterRenderStrategy_i(ClsView::IMAGE, mMprStrategy);

        //voi
        mVOIStrategy.reset(new MprRoiRenderStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mVOIStrategy);

        mScaleRulerStrategy.reset(new TpsScaleRulerStrategy());
        RegisterRenderStrategy_i(ClsView::IMAGE, mScaleRulerStrategy);

        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR<<"Failed to setup scene: "<<ex.what();
        return false;
    }
}

bool TpsFusionRenderController::ResetImage_i()
{
    using namespace Mcsf::MedViewer3D;

    TpsMPRRenderController::ResetImage_i();

    std::string fixedImageUID = mDataSourceMap[IMAGE];
    // Primary Volume GO
    IGraphicObjectPtr go;
    std::shared_ptr<GraphicObjVolume> spPrimaryVolumeGO = nullptr;
    if (!CheckGraphicObject_i(fixedImageUID, GO_TYPE_VOLUME, go) ||
        (spPrimaryVolumeGO = std::dynamic_pointer_cast<GraphicObjVolume>(go)) == nullptr) 
    {
        TPS_LOG_DEV_ERROR<<"TpsFusionRenderController::ResetImage(): "
            <<"Failed to get primary volume : "<<fixedImageUID;
        return false;
    }

    //check mpr graphic object
    if (mSectionType == EMPTY_COMPONENT)
    {
        TPS_LOG_DEV_ERROR<<"TpsFusionRenderController::ResetImage(): The section type is unknown.";
        return false;
    }
    std::shared_ptr<MPRGraphicObject> spPrimaryMPRGO = nullptr;
    // Primary MPR GO
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType, mLocation);
    if (!CheckGraphicObject_i(fixedImageUID, goType, go) ||
        (spPrimaryMPRGO = std::dynamic_pointer_cast<MPRGraphicObject>(go)) == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"TpsFusionRenderController::ResetImage(): "
            <<"Failed to get primary MPR : "<<fixedImageUID;
        return false;
    }

    std::shared_ptr<TpsTextOverlayGraphicObject> mprRulerText = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(MPR_RULER_TEXT, mSectionType, mLocation);
    if (!CheckGraphicObject_i(fixedImageUID, goType, go) || 
        (mprRulerText = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(go)) == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to check MPR ruler text graphic object of image: "<<fixedImageUID;
        return false;
    }

    bool floatImageExist = mDataSourceMap.find(FLOAT_IMAGE) != mDataSourceMap.end();
    std::shared_ptr<FusionChessGraphicObject> fusionChessGo = nullptr;
    std::shared_ptr<FusionGraphicObject> fusionGO = nullptr;
    if (floatImageExist)
    {
        std::string floatImageUID = mDataSourceMap[FLOAT_IMAGE];
        //check fusion chess graphic object
        goType = GOTypeHelper::ComposeAsGOType(FUSION_CHESS, mSectionType);
        if (!CheckGraphicObject_i(COMMON_BASE, goType, go, true) ||
            (fusionChessGo = std::dynamic_pointer_cast<FusionChessGraphicObject>(go)) == nullptr)
        {
            TPS_LOG_DEV_ERROR<<"TpsFusionRenderController::ResetImage(): "
                <<"Failed to check fusion chess graphic object which fixed image: "
                <<fixedImageUID<<", float image: "<<floatImageUID;
            return false;
        }

        //check fusion graphic object
        if (!CheckGraphicObject_i(COMMON_BASE, 
            GO_TYPE_VARYING_FUSION_GO, go, true) || go == nullptr)
        {
            TPS_LOG_DEV_ERROR<<"Failed to check fusion graphic object which fixed image: "
                <<fixedImageUID<<", float: "<<floatImageUID;
            return false;
        }
        fusionGO = std::dynamic_pointer_cast<FusionGraphicObject>(go);
    }

    //check voi graphic object collection & VOI Rendering Cache graphic object
    IGraphicObjectPtr goVOI = nullptr;
    if (!CheckGraphicObject_i(fixedImageUID, GO_TYPE_VOI_COLLECTION, goVOI)){
        TPS_LOG_DEV_ERROR<<"Failed to check VOI graphic object collection of image: "<<fixedImageUID;
        return false;
    }
    auto voi = std::dynamic_pointer_cast<TpsVOIGOCollection>(goVOI);
    if( voi == nullptr){
        return false;
    }

    mMprStrategy->SetVolumeGO(spPrimaryVolumeGO);
    mMprStrategy->SetMPRGO(spPrimaryMPRGO);
    mMprStrategy->SetChessGraphicObject(fusionChessGo);
    mMprStrategy->SetFusionGraphicObject(fusionGO);
    if (mVOIStrategy != nullptr)
    {
        mVOIStrategy->SetVolumeGO(spPrimaryVolumeGO);
        mVOIStrategy->SetMPRGO(spPrimaryMPRGO);
        mVOIStrategy->SetVOIGOCollection(voi);
    }

    mScaleRulerStrategy->SetMprGraphicObject(spPrimaryMPRGO);
    mScaleRulerStrategy->SetVolumeGraphicObject(spPrimaryVolumeGO);
    mScaleRulerStrategy->SetRulerTextInfoGraphicObject(mprRulerText);
    
    return true;
}

bool TpsFusionRenderController::ResetFloatImage_i()
{
    using namespace Mcsf::MedViewer3D;

    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end()){
        TPS_LOG_DEV_ERROR<<"The fixed image data source does not exist.";
        mDataSourceMap.erase(FLOAT_IMAGE);
        return false;
    }
    std::string fixedImageUID = mDataSourceMap[IMAGE];
    std::string floatImageUID = mDataSourceMap[FLOAT_IMAGE];
    // Secondary Volume GO
    IGraphicObjectPtr go;
    std::shared_ptr<GraphicObjVolume> spSecondaryVolumeGO = nullptr;
    if (!CheckGraphicObject_i(floatImageUID, GO_TYPE_VOLUME, go) ||
        (spSecondaryVolumeGO = std::dynamic_pointer_cast<GraphicObjVolume>(go)) == nullptr){
            TPS_LOG_DEV_ERROR<<"TpsFusionRenderController::ResetFloatImage_i(): "
                <<"Failed to get secondary volume : "<<floatImageUID;
            return false;
    }

    // Secondary MPR GO
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType);
    std::shared_ptr<MPRGraphicObject> spSecondaryMPRGO = nullptr;
    if (!CheckGraphicObject_i(floatImageUID, goType, go) ||
        (spSecondaryMPRGO = std::dynamic_pointer_cast<MPRGraphicObject>(go)) == nullptr){
            TPS_LOG_DEV_ERROR<<"TpsFusionRenderController::ResetImage(): "
                <<"Failed to get secondary MPR : "<<floatImageUID;
            return false;
    }

    //check fusion chess graphic object
    std::shared_ptr<FusionChessGraphicObject> fusionChessGo = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(FUSION_CHESS, mSectionType);
    if (!CheckGraphicObject_i(COMMON_BASE, goType, go, true) ||
        (fusionChessGo = std::dynamic_pointer_cast<FusionChessGraphicObject>(go)) == nullptr){
        TPS_LOG_DEV_ERROR<<"TpsFusionRenderController::ResetImage(): "
            <<"Failed to check fusion chess graphic object which fixed image: "
            <<fixedImageUID<<", float image: "<<floatImageUID;
        return false;
    }

    //check fusion graphic object
    tps::IGraphicObjectPtr goFusion = nullptr;
    if (!CheckGraphicObject_i(COMMON_BASE, 
        GO_TYPE_VARYING_FUSION_GO, goFusion, true) || goFusion == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to check fusion graphic object which fixed image: "
            <<fixedImageUID<<", float: "<<floatImageUID;
        return false;
    }
    auto fusionGO = std::dynamic_pointer_cast<FusionGraphicObject>(goFusion);

    mMprStrategy->SetFloatVolumeGO(spSecondaryVolumeGO);
    mMprStrategy->SetFloatMprGO(spSecondaryMPRGO);
    mMprStrategy->SetChessGraphicObject(fusionChessGo);
    mMprStrategy->SetFusionGraphicObject(fusionGO);

    return true;
}

/// clear graphic objects from ModelWarehouse
void TpsFusionRenderController::ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid)
{
    TpsMPRRenderController::ClearGraphicObjects_i(id, uid);

    if (id == IMAGE){
        RemoveGraphicObject_i(uid, GO_TYPE_VOLUME);
        GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType, mLocation);
        RemoveGraphicObject_i(uid, goType);
        goType = GOTypeHelper::ComposeAsGOType(MPR_RULER_TEXT, mSectionType, mLocation);
        RemoveGraphicObject_i(uid, goType);
        RemoveGraphicObject_i(uid, GO_TYPE_VOI_COLLECTION);
    }
    else if(id == FLOAT_IMAGE){
        RemoveGraphicObject_i(uid, GO_TYPE_VOLUME);
        GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType);
        RemoveGraphicObject_i(uid, goType);
    }
}

void TpsFusionRenderController::RemoveDataSource_i(DATA_SOURCE_ID id)
{
    TpsMPRRenderController::RemoveDataSource_i(id);

    switch (id) {
    case IMAGE:
        mMprStrategy->SetVolumeGO(nullptr);
        mMprStrategy->SetMPRGO(nullptr);
        mMprStrategy->SetChessGraphicObject(nullptr);
        mMprStrategy->SetFusionGraphicObject(nullptr);

        mScaleRulerStrategy->SetMprGraphicObject(nullptr);
        mScaleRulerStrategy->SetVolumeGraphicObject(nullptr);
        mScaleRulerStrategy->SetRulerTextInfoGraphicObject(nullptr);

        if (mVOIStrategy != nullptr)
        {
            mVOIStrategy->SetVolumeGO(nullptr);
            mVOIStrategy->SetMPRGO(nullptr);
            mVOIStrategy->SetVOIGOCollection(nullptr);
        }
        break;
    case FLOAT_IMAGE:
        mMprStrategy->SetFloatVolumeGO(nullptr);
        mMprStrategy->SetFloatMprGO(nullptr);
        mMprStrategy->SetChessGraphicObject(nullptr);
        mMprStrategy->SetFusionGraphicObject(nullptr);
    default:
        break;
    }
}
TPS_END_NAMESPACE  // end namespace tps
