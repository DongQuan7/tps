////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rc_ut_rendercontroller.h
/// 
///  \brief class TpsUtRenderController declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/04/28
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderController/tps_rc_ut_rendercontroller.h"

#include "boost/algorithm/string/split.hpp"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

//ZHENGHE
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
//#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

// Render Strategies
#include "RtTpsRenderLibrary/tps_rl_mprrenderstrategy.h"

// Graphic Objects
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
//#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

#include "McsfMedViewer3DArithmetic/arithmetic.h"                   // for Mcsf::MedViewer3D::Point3f

#include "tps_logger.h"

using namespace Mcsf::MedViewer3D;

#define NEGLECT_CHECK_IN -1

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsUtRenderController::TpsUtRenderController(WINDOW_TYPE type)
    : TpsMPRRenderController(type) {
//    : TpsRenderController(type){
}

TpsUtRenderController::~TpsUtRenderController(){

}

//bool TpsUtRenderController::Initialize(DISPLAY_SIZE displaySize, DATA_SOURCE_GROUP_ID group/* = -1*/)
//{
    //displaySize;
    //create view
    //mView.reset(new Mcsf::MedViewer3D::ClsView(displaySize.width, displaySize.height, 
    //    mModelWarehouse->GetModelEntity()));
    //if (!mView->Initialize()){
    //    TPS_LOG_DEV_ERROR<<"Failed to initialize ClsView.";
    //    return false;
    //}
    //mModelWarehouse->AttachObserver(mView);

    //display cache
    //mDisplayCache.reset(new TpsDisplayCache());
    //mDisplayCache->SetRenderUnit(mView);
    ////mView->AttachObserver(mDisplayCache);
    //if (!SetupScene_i()){
    //    TPS_LOG_DEV_ERROR<<"Failed to setup scene.";
    //    return false;
    //}

    //mDataSourceGroupID = group;
    //if (mDataSourceGroupID != NEGLECT_CHECK_IN){
    //    TpsRenderingComponentRecorder::GetInstance()->CheckIn(group, this);
    //}
    //return true;
//}

void TpsUtRenderController::Resize(DISPLAY_SIZE displaySize){
    m_iWidth = displaySize.width;
    m_iHeight = displaySize.height;
}

bool TpsUtRenderController::GetDisplaySize(DISPLAY_SIZE& displaySize) {
    displaySize.width = m_iWidth;
    displaySize.height = m_iHeight;
    return true;
}

bool TpsUtRenderController::SetupScene_i(){
    return true;
}

bool TpsUtRenderController::TransformPoint(const Mcsf::MedViewer3D::Point2D& ptIn, 
                                           Mcsf::MedViewer3D::Point3D& ptOut)
{
    ptOut.x = ptIn.x;
    ptOut.y = ptIn.y;
    ptOut.z = 0.0;
    return true;
}

bool TpsUtRenderController::ResetDataSource_i(DATA_SOURCE_ID id) {
    bool bRes = false;
    switch (id) {
    case IMAGE:
        bRes = ResetImage_i();
        break;
    default:
        TPS_LOG_DEV_ERROR<<"Unrecognized data source id: "<<id;
        break;
    }
    return bRes;
}

FIRST_POSTFIX_COMPONENT TpsUtRenderController::GetSectionType() {
    return m_nSectionType;
}

bool TpsUtRenderController::ResetImage_i(){
    return true;
}

void TpsUtRenderController::RemoveDataSource_i(DATA_SOURCE_ID id){
    switch (id) {
    case IMAGE:
    default:
        break;
    }
}
TPS_END_NAMESPACE  // end namespace tps
