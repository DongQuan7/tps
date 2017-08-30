////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rc_mprrendercontroller.cpp
/// 
///  \brief class TpsMPRRenderController implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/13
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderController/tps_rc_mpr_render_controller.h"

//ZHENGHE
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

// Render Strategies
#include "RtTpsRenderLibrary/tps_rl_volume4d_mpr_renderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_scalerulerstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_renderstrategy.h"         // for CrosshairRenderStrategy

// Graphic Objects
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"          // for CrosshairGraphicObject
#include "RtTpsRenderLibrary/tps_rl_fusiongraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_fusionchessgraphicobject.h"

// frameworks
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_fusionsecondarydatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_graphic_object_converter_factory_base.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

TpsMPRRenderController::TpsMPRRenderController(WINDOW_TYPE type)
    : TpsRenderController(type),
      mMprStrategy(nullptr),
      mScaleRulerStrategy(nullptr),
      m_spCrosshairRenderStrategy(nullptr)
{
    SetSectionType_i();
}

TpsMPRRenderController::~TpsMPRRenderController()
{

}

bool TpsMPRRenderController::SetupScene_i(){
    try
    {
        ///////////////////////////IMAGE Overlay////////////////////////////
        mMprStrategy.reset(new Volume4dMprRenderStrategy());
        RegisterRenderStrategy_i(ClsView::IMAGE, mMprStrategy);

        //scale ruler
        mScaleRulerStrategy.reset(new TpsScaleRulerStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, mScaleRulerStrategy);

        return true;
    }
    catch (std::exception& ex){
        TPS_LOG_DEV_ERROR<<"Failed to setup scene: "<<ex.what();
        return false;
    }
}

bool TpsMPRRenderController::ResetDataSource_i(DATA_SOURCE_ID id)
{
    bool bRes = false;
    switch (id) {
    case IMAGE:
        bRes = ResetImage_i();
        break;
    case FLOAT_IMAGE:
        bRes = ResetFloatImage_i();
        break;
    case PLAN:
        bRes = ResetPlan_i();
        break;
    default:
        TPS_LOG_DEV_ERROR<<"Unrecognized data source id: "<<id;
        break;
    }
    return bRes;
}

bool TpsMPRRenderController::ResetImage_i()
{
    std::string imageUID = mDataSourceMap[IMAGE];
    //check volume graphic object
    IGraphicObjectPtr goVolume = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_VOLUME, goVolume))
    {
        TPS_LOG_DEV_ERROR<<"Failed to check volume graphic object of image: "<<imageUID;
        return false;
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

    IGraphicObjectPtr goMprRulerText = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(MPR_RULER_TEXT, mSectionType, mLocation);
    if (!CheckGraphicObject_i(imageUID, goType, goMprRulerText))
    {
        TPS_LOG_DEV_ERROR<<"Failed to check MPR ruler text graphic object of image: "<<imageUID;
        return false;
    }

    // crosshair GO
    //CrossHair GO还在使用中，RenderStrategy没有在使用了
    IGraphicObjectPtr goCrosshair = nullptr;
    if (this->mLocation == PREV || this->mLocation == POST) {
    }
    else {
        GO_TYPE goType = GOTypeHelper::ComposeAsGOType(CROSSHAIR, mSectionType, mLocation);
        if (!CheckGraphicObject_i(imageUID, goType, goCrosshair)){
            TPS_LOG_DEV_ERROR<<"Failed to check crosshair graphic object of image: "<<imageUID;
            return false;
        }
    }

    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(goMpr);
    auto mprRulerText = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(goMprRulerText);
    auto crosshair = std::dynamic_pointer_cast<CrosshairGraphicObject>(goCrosshair);
    if (volume == nullptr || mpr == nullptr || 
        mprRulerText == nullptr || crosshair == nullptr )
    {
            TPS_LOG_DEV_ERROR<<"Failed to dynamic cast IGraphicObjects.";
            //return false;
    }

    if (mMprStrategy != nullptr)
    {
        mMprStrategy->SetVolumeGO(volume);
        mMprStrategy->SetMPRGO(mpr);
        mMprStrategy->Enable4DMode(false);
    }
    if (mScaleRulerStrategy  != nullptr)
    {
        mScaleRulerStrategy->SetMprGraphicObject(mpr);
        mScaleRulerStrategy->SetVolumeGraphicObject(volume);
        mScaleRulerStrategy->SetRulerTextInfoGraphicObject(mprRulerText);
    }
    //crosshair strategy data
    if (m_spCrosshairRenderStrategy != nullptr) {
        m_spCrosshairRenderStrategy->SetCrosshairGraphicObject(crosshair);
    }

    return true;
}

bool TpsMPRRenderController::ResetFloatImage_i()
{
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end()){
        TPS_LOG_DEV_ERROR<<"The fixed image data source does not exist.";
        mDataSourceMap.erase(FLOAT_IMAGE);
        return false;
    }

    std::string fixedImageUID = mDataSourceMap[IMAGE];
    std::string floatImageUID = mDataSourceMap[FLOAT_IMAGE];
    GOConverterParameter para;
    para.data_source_map[IMAGE] = floatImageUID;
    // Secondary Volume GO
    IGraphicObjectPtr go;
    std::shared_ptr<GraphicObjVolume> spSecondaryVolumeGO = nullptr;
    if (!CheckGraphicObject_i(floatImageUID, GO_TYPE_VOLUME, go, false, &para) ||
        (spSecondaryVolumeGO = std::dynamic_pointer_cast<GraphicObjVolume>(go)) == nullptr){
            TPS_LOG_DEV_ERROR<<"TpsFusionRenderController::ResetFloatImage_i(): "
                <<"Failed to get secondary volume : "<<floatImageUID;
            return false;
    }

    // Secondary MPR GO
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType);
    std::shared_ptr<MPRGraphicObject> spSecondaryMPRGO = nullptr;
    if (!CheckGraphicObject_i(floatImageUID, goType, go, false, &para) ||
        (spSecondaryMPRGO = std::dynamic_pointer_cast<MPRGraphicObject>(go)) == nullptr){
            TPS_LOG_DEV_ERROR<<"TpsFusionRenderController::ResetImage(): "
                <<"Failed to get secondary MPR : "<<floatImageUID;
            return false;
    }

    //check fusion chess graphic object
    std::shared_ptr<FusionChessGraphicObject> fusionChessGo = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(FUSION_CHESS, mSectionType);
    if (!CheckGraphicObject_i(fixedImageUID + "|" + floatImageUID, goType, go) ||
        (fusionChessGo = std::dynamic_pointer_cast<FusionChessGraphicObject>(go)) == nullptr){
            TPS_LOG_DEV_ERROR<<"TpsFusionRenderController::ResetImage(): "
                <<"Failed to check fusion chess graphic object which fixed image: "
                <<fixedImageUID<<", float image: "<<floatImageUID;
            return false;
    }

    //check fusion graphic object
    tps::IGraphicObjectPtr goFusion = nullptr;
    if (!CheckGraphicObject_i(fixedImageUID + "|" + floatImageUID, 
        GO_TYPE_FUSION, goFusion) || goFusion == nullptr){
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

bool TpsMPRRenderController::ResetPlan_i()
{
    return true;
}

void TpsMPRRenderController::SetSectionType_i()
{
    mSectionType = EMPTY_COMPONENT;
    mLocation = EMPTY_COMPONENT;
    mWindowGroupType = (DATA_SOURCE_GROUP_ID)(mWindowType >> 8);
}

void TpsMPRRenderController::RemoveDataSource_i(DATA_SOURCE_ID id)
{
    switch (id) {
    case IMAGE:
        if (mMprStrategy  != nullptr)
        {
            mMprStrategy->SetVolumeGO(nullptr);
            mMprStrategy->SetMPRGO(nullptr);
            mMprStrategy->Enable4DMode(false);
        }
        if (mScaleRulerStrategy != nullptr)
        {
            mScaleRulerStrategy->SetMprGraphicObject(nullptr);
            mScaleRulerStrategy->SetVolumeGraphicObject(nullptr);
            mScaleRulerStrategy->SetRulerTextInfoGraphicObject(nullptr);
        }
        if (m_spCrosshairRenderStrategy != nullptr) {
            m_spCrosshairRenderStrategy->SetCrosshairGraphicObject(nullptr);
        }
        break;
    case FLOAT_IMAGE:
        if (mMprStrategy != nullptr)
        {
            mMprStrategy->SetFloatVolumeGO(nullptr);
            mMprStrategy->SetFloatMprGO(nullptr);
            mMprStrategy->SetChessGraphicObject(nullptr);
            mMprStrategy->SetFusionGraphicObject(nullptr);
        }
        break;
    default:
        break;
    }
}

void TpsMPRRenderController::ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid)
{
    if (id == IMAGE) {
        std::string imageUID = uid;
        RemoveGraphicObject_i(imageUID, GO_TYPE_VOLUME);
        GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType, mLocation);
        RemoveGraphicObject_i(imageUID, goType);
        goType = GOTypeHelper::ComposeAsGOType(MPR_RULER_TEXT, mSectionType, mLocation);
        RemoveGraphicObject_i(imageUID, goType);
    }
    else if (id == FLOAT_IMAGE){
        RemoveGraphicObject_i(uid, GO_TYPE_VOLUME);
        GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType);
        RemoveGraphicObject_i(uid, goType);
    }
}

HitTestResult TpsMPRRenderController::HitTest(double /*screenX*/, double /*screenY*/)
{
    HitTestResult hitRes;
    hitRes._hitNum = HIT_NUM_NONE;
    return hitRes;
}




bool TpsMPRRenderController::TransformPoint(const Point3D& ptIn, Point2D& ptOut)
{
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end()){
        TPS_LOG_DEV_ERROR << "The image data source is not ready.";
        return false;
    }

    // mpr go
    std::string imageUID = mDataSourceMap[IMAGE];
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType);
    std::string key = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(key);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr){
        TPS_LOG_DEV_ERROR << "The mpr graphic object is empty.";
        return false;
    }

    // volume go
    std::string volumeKey = imageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    if (volume == nullptr) {
        TPS_LOG_DEV_ERROR << "Failed to get volume go!" << imageUID;
        return false;
    }

    size_t dim[3];
    volume->GetDimension(dim);
    Mcsf::MedViewer3D::Matrix4x4 modelToVolume(dim[0], 0, 0, 0,
        0, dim[1], 0, 0,
        0, 0, dim[2], 0,
        -0.5, -0.5, -0.5, 1);
    Mcsf::MedViewer3D::Matrix4x4 matWorld2Volume = modelToVolume * volume->GetModel2World().Inverse();
    Mcsf::MedViewer3D::Matrix4x4 matPat2World = volume->GetWorld2Patient().Inverse();

    // 坐标映射
    Mcsf::MedViewer3D::Point3D center3D = matPat2World.Transform(ptIn);
    Mcsf::MedViewer3D::Point3D pt3dCrosshairCenterInVolume = matWorld2Volume.Transform(center3D);
    double dSliceX = pt3dCrosshairCenterInVolume.x;
    double dSliceY = pt3dCrosshairCenterInVolume.y;
    double dSliceZ = pt3dCrosshairCenterInVolume.z;
    dSliceX = (int)(dSliceX + 0.5);
    dSliceY = (int)(dSliceY + 0.5);
    dSliceZ = (int)(dSliceZ + 0.5);
    center3D = matWorld2Volume.Inverse().Transform(Mcsf::MedViewer3D::Point3D(dSliceX, dSliceY, dSliceZ));
    Point2D ptRes;
    ptRes = mpr->GetPlaneCoordinate(center3D, false);
    //printf("res: %.2f, %.2f\n", ptRes.x, ptRes.y);
    // modify again
    int width, height;
    mView->GetDisplaySize(width, height);
    double dRatioX = 1.0, dRatioY = 1.0;
    if (width >= height){
        dRatioX = static_cast<double>(height) / width;
    }
    else{
        dRatioY = static_cast<double>(width) / height;
    }

    ptOut.x = ptRes.x*dRatioX + (1 - dRatioX) / 2.0;
    ptOut.y = ptRes.y*dRatioY + (1 - dRatioY) / 2.0;
    //printf("out: %.2f, %.2f\n", ptOut.x, ptOut.y);
    return true;
}

bool TpsMPRRenderController::TransformPoint(const Point2D& ptIn, Point3D& ptOut)
{
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end()){
        TPS_LOG_DEV_ERROR<<"The image data source is not ready.";
        return false;
    }

    std::string imageUID = mDataSourceMap[IMAGE];
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType);
    std::string key = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(key);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr){
        TPS_LOG_DEV_ERROR<<"The mpr graphic object is empty.";
        return false;
    }

    //ptOut = mpr->PlaneCoordinateToWorldCoordiate(ptIn.x, ptIn.y);
    
    int width, height;
    mView->GetDisplaySize(width, height);
    double dRatioX = 1.0, dRatioY = 1.0;
    if (width >= height){
        dRatioX = static_cast<double>(height) / width;
    }
    else{
        dRatioY = static_cast<double>(width) / height;
    }

    Point2D ptRes;
    ptRes.x = (ptIn.x - (1 - dRatioX) / 2.0) / dRatioX;
    ptRes.y = (ptIn.y - (1 - dRatioY) / 2.0) / dRatioY;

    Point3D ptInWorld;
    ptInWorld = mpr->GetWorldCoordinate(ptRes);

    std::string volumeKey = imageUID + "|"+ GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    if(volume == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get volume go!"<<imageUID;
        return false;
    }
    Matrix4x4 matW2P = volume->GetWorld2Patient();
    ptOut = matW2P.Transform(ptInWorld);
    //printf("2->3 out:%.2f, %.2f, %.2f\n", ptOut.x, ptOut.y, ptOut.z);
    return true;
}

bool TpsMPRRenderController::TransformPoints(
    const std::vector<Mcsf::MedViewer3D::Point2D>& ptListIn, 
    std::vector<Mcsf::MedViewer3D::Point3D>& ptListOut,
    bool isSquaredPoints/* = true*/)
{
    ptListOut.clear();
    if (ptListIn.empty()) return true;

    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end()){
        TPS_LOG_DEV_ERROR<<"The image data source is not ready.";
        return false;
    }

    std::string imageUID = mDataSourceMap[IMAGE];
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType);
    std::string key = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(key);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr){
        TPS_LOG_DEV_ERROR<<"The mpr graphic object is empty.";
        return false;
    }

    std::string volumeKey = imageUID + "|"+ GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    if(volume == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get volume go!"<<imageUID;
        return false;
    }
    Matrix4x4 matW2P = volume->GetWorld2Patient();

    double dRatioX = 1.0, dRatioY = 1.0;
    if (!isSquaredPoints)
    {
        int width, height;
        mView->GetDisplaySize(width, height);
        if (width >= height){
            dRatioX = static_cast<double>(height) / width;
        }
        else{
            dRatioY = static_cast<double>(width) / height;
        }
    }

    Point2D pt2d;
    Point3D pt3d;
    for (int i = 0; i < ptListIn.size(); ++i)
    {
        pt2d.x = (ptListIn[i].x - (1 - dRatioX) / 2.0) / dRatioX;
        pt2d.y = (ptListIn[i].y - (1 - dRatioY) / 2.0) / dRatioY;
        ptListOut.push_back(matW2P.Transform(mpr->GetWorldCoordinate(pt2d)));
    }
    return true;
}

bool TpsMPRRenderController::GetCrosshairProjectionPosition(float& x2D, float& y2D)
{
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end()){
        TPS_LOG_DEV_ERROR << "The image data source is not ready.";
        return false;
    }

    int width, height;
    mView->GetDisplaySize(width, height);
    if (width <= 0 || height <= 0){
        TPS_LOG_DEV_ERROR<<"The view size is not correct.";
        return false;
    }

    // mpr go
    std::string imageUID = mDataSourceMap[IMAGE];
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType);
    std::string key = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(key);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr){
        TPS_LOG_DEV_ERROR << "The mpr graphic object is empty.";
        return false;
    }

    // volume go
    std::string volumeKey = imageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    if (volume == nullptr) {
        TPS_LOG_DEV_ERROR << "Failed to get volume go!" << imageUID;
        return false;
    }

    // crosshair go
    goType = GOTypeHelper::ComposeAsGOType(CROSSHAIR, mSectionType);
    std::string strKey = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto goCrosshair = mModelWarehouse->GetModelObject(strKey);
    auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(goCrosshair);
    if (spCrosshairGO == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][CrosshairMovementOperation][ModifyGraphicObject]: "
            <<"Failed to dynamic cast IGraphicObjects.";
        return false;
    }
    // world2volume
    size_t dim[3];
    volume->GetDimension(dim);
    Mcsf::MedViewer3D::Matrix4x4 modelToVolume(dim[0], 0, 0, 0,
        0, dim[1], 0, 0,
        0, 0, dim[2], 0,
        -0.5, -0.5, -0.5, 1);
    Mcsf::MedViewer3D::Matrix4x4 matWorld2Volume = modelToVolume * volume->GetModel2World().Inverse();

    Mcsf::MedViewer3D::Point3D center3D;
    spCrosshairGO->GetCrosshairPoint3d(&center3D); // in world
    Mcsf::MedViewer3D::Point3D pt3dCrosshairCenterInVolume = matWorld2Volume.Transform(center3D);
    double dSliceX = pt3dCrosshairCenterInVolume.x;
    double dSliceY = pt3dCrosshairCenterInVolume.y;
    double dSliceZ = pt3dCrosshairCenterInVolume.z;
    dSliceX = (int)(dSliceX + 0.5);
    dSliceY = (int)(dSliceY + 0.5);
    dSliceZ = (int)(dSliceZ + 0.5);
    center3D = matWorld2Volume.Inverse().Transform(Mcsf::MedViewer3D::Point3D(dSliceX, dSliceY, dSliceZ));
    Point2D ptRes;
    ptRes = mpr->GetPlaneCoordinate(center3D, false);
    //printf("res: %.2f, %.2f\n", ptRes.x, ptRes.y);
    // modify again
    double dRatioX = 1.0, dRatioY = 1.0;
    if (width >= height){
        dRatioX = static_cast<double>(height) / width;
    }
    else{
        dRatioY = static_cast<double>(width) / height;
    }

    x2D = ptRes.x*dRatioX + (1 - dRatioX) / 2.0;
    y2D = ptRes.y*dRatioY + (1 - dRatioY) / 2.0;
    return true;
}

//获取是直线上下左右的四个点，不一定是终点。
//x1,y1,x2,y2 是y上的两个点，x3,y3,x4,y4是x上的两点。
bool TpsMPRRenderController::GetCrossXYPoints(double& x1, double& y1, double& x2, double& y2, double& x3, double& y3, double& x4, double& y4)
{
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end()){
        TPS_LOG_DEV_ERROR << "The image data source is not ready.";
        return false;
    }

    int width, height;
    mView->GetDisplaySize(width, height);
    if (width <= 0 || height <= 0){
        TPS_LOG_DEV_ERROR << "The view size is not correct.";
        return false;
    }

    // mpr go
    std::string imageUID = mDataSourceMap[IMAGE];
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType);
    std::string key = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(key);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr){
        TPS_LOG_DEV_ERROR << "The mpr graphic object is empty.";
        return false;
    }

    // volume go
    std::string volumeKey = imageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    if (volume == nullptr) {
        TPS_LOG_DEV_ERROR << "Failed to get volume go!" << imageUID;
        return false;
    }

    // crosshair go
    goType = GOTypeHelper::ComposeAsGOType(CROSSHAIR, mSectionType);
    std::string strKey = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto goCrosshair = mModelWarehouse->GetModelObject(strKey);
    auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(goCrosshair);
    if (spCrosshairGO == nullptr){
        TPS_LOG_DEV_ERROR << "[ERROR][CrosshairMovementOperation][ModifyGraphicObject]: "
            << "Failed to dynamic cast IGraphicObjects.";
        return false;
    }

    Mcsf::MedViewer3D::Point3D point1, point2, point3, point4;
    Mcsf::MedViewer3D::Point2D pt1, pt2, pt3, pt4;
    spCrosshairGO->GetCrossFourPoints(&point1, &point2, &point3, &point4);
    pt1 = mpr->GetPlaneCoordinate(point1, false);
    pt2 = mpr->GetPlaneCoordinate(point2, false);
    pt3 = mpr->GetPlaneCoordinate(point3, false);
    pt4 = mpr->GetPlaneCoordinate(point4, false);
   
    //printf("res: %.2f, %.2f\n", ptRes.x, ptRes.y);
    // modify again
    double dRatioX = 1.0, dRatioY = 1.0;
    if (width >= height){
        dRatioX = static_cast<double>(height) / width;
    }
    else{
        dRatioY = static_cast<double>(width) / height;
    }

    x1 = pt1.x*dRatioX + (1 - dRatioX) / 2.0;
    y1 = pt1.y*dRatioY + (1 - dRatioY) / 2.0;
    x2 = pt2.x*dRatioX + (1 - dRatioX) / 2.0;
    y2 = pt2.y*dRatioY + (1 - dRatioY) / 2.0;
    x3 = pt3.x*dRatioX + (1 - dRatioX) / 2.0;
    y3 = pt3.y*dRatioY + (1 - dRatioY) / 2.0;
    x4 = pt4.x*dRatioX + (1 - dRatioX) / 2.0;
    y4 = pt4.y*dRatioY + (1 - dRatioY) / 2.0;
    return true;
}

//根据windowType 和windowuid 来重置MPR底图
bool TpsMPRRenderController::ResetMPRObject()
{
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end()){
        TPS_LOG_DEV_ERROR << "The image data source is not ready.";
        return false;
    }
    std::string strSeriesUid = mDataSourceMap[IMAGE];
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType, mLocation);
    std::string goTypeStr = GOTypeHelper::ToString(goType);
    std::string key = strSeriesUid + "|" + goTypeStr;
    auto go = mModelWarehouse->GetModelObject(key);
    auto spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    auto converter = mGOConverterFactory->GetConverter(goType);

    if (converter == nullptr){
        TPS_LOG_DEV_ERROR << "[ERROR][TpsMPRRenderController][ResetMPRObject]: "
            << "Failed to get graphic object converter which type is: " << goType;
        return false;
    }
    if (!converter->Update(go)){
        TPS_LOG_DEV_ERROR << "[ERROR][TpsMPRRenderController][ResetMPRObject]: "
            << "Failed to update RT data to graphic object which key is: " << strSeriesUid;
        return false;
    }

    spMprGO->SetDirty(true);
    return true;
}


double TpsMPRRenderController::GetDistanceToPlane(std::string &imageUID, float x3D, float y3D, float z3D)
{
    IGraphicObjectPtr goMpr = nullptr;
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType, mLocation);
    if (!CheckGraphicObject_i(imageUID, goType, goMpr)){
        TPS_LOG_DEV_ERROR<<"Failed to check MPR graphic object of image: "<<imageUID;
        return -1;
    }
     auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(goMpr);

    //check volume graphic object
    IGraphicObjectPtr goVolume = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_VOLUME, goVolume)){
        TPS_LOG_DEV_ERROR<<"Failed to check volume graphic object of image: "<<imageUID;
        return -1;
    }
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);


    double dSpacingInWorld, dDistanceToPlaneInWorld, dDistanceToPlaneInPatient;
    Mcsf::MedViewer3D::Point3D vertices[4];
    mpr->GetVertices(vertices);
    Mcsf::MedViewer3D::Plane plane(vertices[0],vertices[1],vertices[3]);
    Mcsf::Point3f ptIn(x3D, y3D, z3D);
    Mcsf::MedViewer3D::Point3D ptInPat = TpsArithmeticConverter::ConvertToPoint3D(ptIn);
    Mcsf::MedViewer3D::Point3D ptInWorld;
    Mcsf::MedViewer3D::Matrix4x4 matWorld2Patient = volume->GetWorld2Patient();
    Mcsf::MedViewer3D::Matrix4x4 matPatient2World = matWorld2Patient.Inverse();
    ptInWorld = matPatient2World.Transform(ptInPat);
    dSpacingInWorld = mpr->GetThickness(); //0.5 * GetSpacing_i();
    dDistanceToPlaneInWorld = plane.DistancePointToPlane(ptInWorld);
    Mcsf::MedViewer3D::Vector3D distance(0.0, 0.0, dDistanceToPlaneInWorld); 
    dDistanceToPlaneInPatient = matWorld2Patient.Transform(distance).z;
    return dDistanceToPlaneInPatient;
}

/*!\fn TpsBEVRenderController::GetBevViewDirecrion
* <key> \n
* PRA: Yes \n
* Name:bool GetBevViewDirecrion \n
* Traced from: 27421 DS_PRA_TPS_UBoyIndicator \n 
* Description: calculate the placement of U boy  \n
* Short Description: UBoy placement Indicator \n
* Component: TPS \n
* </key> \n
***********************************************************/
bool TpsMPRRenderController::GetMprViewDirecrion(Mcsf::MedViewer3D::Vector3D &vectorView, Mcsf::MedViewer3D::Vector3D &vectotUp) 
{
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end()){
        TPS_LOG_DEV_ERROR<<"The image data source is not ready.";
        return false;
    }

    std::string imageUID = mDataSourceMap[IMAGE];
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType);
    std::string key = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(key);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr) {
        TPS_LOG_DEV_ERROR<<"The mpr graphic object is empty.";
        return false;
    }

    mpr->GetViewDirection(&vectotUp, &vectorView);
    return true;
}

bool TpsMPRRenderController::MapMilimeter2Pixel(double lengthInmm, 
    double* pixelinXNP, double* pixelinYNP)
{
    // get mpr and volume go
    // mpr go
    std::string imageUID = mDataSourceMap[IMAGE];
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType);
    std::string key = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(key);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr){
        TPS_LOG_DEV_ERROR << "The mpr graphic object is empty.";
        return false;
    }

    // volume go
    std::string volumeKey = imageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    if (volume == nullptr) {
        TPS_LOG_DEV_ERROR << "Failed to get volume go!" << imageUID;
        return false;
    }

    DISPLAY_SIZE size;
    this->GetDisplaySize(size);
    double smallSize = size.width > size.height ? size.height : size.width;

    Matrix4x4 matW2P = volume->GetWorld2Patient();
    Point3D aVertices[4];
    mpr->GetVertices(aVertices);
    Vector3D vecX = matW2P.Transform(aVertices[1] - aVertices[0]);
    double spacing = vecX.Magnitude() / (smallSize - 1);
    *pixelinXNP = lengthInmm / spacing;
    *pixelinYNP = lengthInmm / spacing;

    return true;
}

FIRST_POSTFIX_COMPONENT TpsMPRRenderController::GetSectionType()
{
    return mSectionType;
}

bool TpsMPRRenderController::SetCrosshairHittedMode(int mode)
{
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end()){
        TPS_LOG_DEV_ERROR << "The image data source is not ready.";
        return false;
    }
    std::string strSeriesUid = mDataSourceMap[IMAGE];
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(CROSSHAIR, mSectionType);
    std::string strKey = strSeriesUid + "|" + GOTypeHelper::ToString(goType);
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    auto go = mModelWarehouse->GetModelObject(strKey);
    auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
    if (spCrosshairGO == nullptr){
        TPS_LOG_DEV_ERROR << "pCrosshairGO is null.";
        printf("\nspCrosshairGO is null.");
        return false;
    }
    spCrosshairGO->SetHittedMode((CrosshairGraphicObject::HITTED_MODE)mode);
    return true;
}

void TpsMPRRenderController::HideNonImageOverlay(bool isToHide)
{
    std::vector<TpsRSPtr> nonImageRSList;
    for (auto it = mStrategyList.begin(); it != mStrategyList.end(); ++it){
        if (it->first == ClsView::IMAGE) continue;
        for (int i = 0; i < it->second.size(); ++i){
            if (it->second[i] == nullptr) continue;
            nonImageRSList.push_back(it->second[i]);
        }
    }

    if (!nonImageRSList.empty()) this->SynchronizeRenderThread();
    for (int i = 0; i < nonImageRSList.size(); ++i){
        nonImageRSList[i]->SetIsVisible(!isToHide);
    }
}

bool TpsMPRRenderController::GetNormal(Mcsf::MedViewer3D::Vector3D& normal)
{
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end()){
        TPS_LOG_DEV_ERROR<<"The image data source is not ready.";
        return false;
    }

    std::string imageUID = mDataSourceMap[IMAGE];
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType);
    std::string key = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(key);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr){
        TPS_LOG_DEV_ERROR<<"The mpr graphic object is empty.";
        return false;
    }

    std::string volumeKey = imageUID + "|"+ GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    if(volume == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get volume go!"<<imageUID;
        return false;
    }
    Matrix4x4 matW2P = volume->GetWorld2Patient();
    Vector3D vecNormalWorld;
    if (!mpr->GetNormal(&vecNormalWorld)){
        TPS_LOG_DEV_ERROR<<"Failed to get normal vector of mpr GO.";
        return false;
    }
    normal = matW2P.Transform(vecNormalWorld);
    normal.Normalize();
    return true;
}

bool TpsMPRRenderController::IsInVolume(float x, float y) const
{
    auto it = mDataSourceMap.find(IMAGE);
    if (it == mDataSourceMap.end()){
        return false;
    }

    std::string imageUID = it->second;
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType);
    std::string key = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(key);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr){
        return false;
    }

    std::string volumeKey = imageUID + "|"+ GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    if(volume == nullptr) {
        return false;
    }

    int width, height;
    mView->GetDisplaySize(width, height);
    double dRatioX = 1.0, dRatioY = 1.0;
    if (width >= height){
        dRatioX = static_cast<double>(height) / width;
    }
    else{
        dRatioY = static_cast<double>(width) / height;
    }

    Point2D ptRes;
    ptRes.x = (x - (1 - dRatioX) / 2.0) / dRatioX;
    ptRes.y = (y - (1 - dRatioY) / 2.0) / dRatioY;
    Point3D point = mpr->GetWorldCoordinate(ptRes);
    auto matW2M = volume->GetModel2World().Inverse();
    point = matW2M.Transform(point);
    return point.x >= 0.0 && point.x <= 1.0 && point.y >= 0.0 && 
        point.y <= 1.0 && point.z >= 0.0 && point.z <= 1.0;
}
TPS_END_NAMESPACE  // end namespace tps
