//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_gca_gallery_controllerapp.cpp
///  \brief   
///
///  \version 1.0
///  \date    Nov. 24, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include"stdafx.h"
#include "tps_gca_gallery_controllerapp.h"

#include <boost/serialization/list.hpp>

//ZHENGHE
#include "McsfGeometry/mcsf_geometry_rectangle.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h" 

//TPS
#include "tps_logger.h"

#include "RtTpsRenderController/tps_rc_galleryrenderproxy.h"

#include "RtTpsDataAccess/tps_da_configuremanager.h"
#include "RtTpsDataAccess/tps_da_entity_converter.h"
#include "RtTpsDataAccess/tps_da_gallery_patientmanager.h"
#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"

#include "RtTpsFramework/tps_rc_layoutmanager.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_fw_commandmanager.h"
#include "RtTpsProtoLib/rt_tps_certifiedmouseinfo.h"
#include "RtTpsProtoLib/rt_tps_certifiedgetdensity.h"
#include "RtTpsProtoLib/rt_tps_certified4dctseriesuidlist.h"

#include "tps_gca_dataloaderimpl.h"
#include "tps_gca_dataaccessorimpl.h"
#include "tps_gca_mproperator.h"

// TMS
#include "tms/rt_fwk_dl_access.h"
#include "tms/rt_fwk_dl_object_series.h"
#include "tms/rt_fwk_dl_object_image3d.h"

TPS_BEGIN_NAMESPACE

IMPLEMENT_APP_CONTROLLER(GalleryControllerApp)

GalleryControllerApp::GalleryControllerApp(int id)
: IGalleryController(id), 
    mDataAccessor(nullptr), 
    mDataLoader(nullptr), 
    mMPROperator(nullptr), 
    mRenderProxy(nullptr){
        //Init class member
        (void)id;
}

GalleryControllerApp::~GalleryControllerApp(void) {
}

int GalleryControllerApp::Start(const std::vector<std::string> &args) {
    TpsControllerBase::Start(args);
    //Init Render controller
    mRenderProxy = new TpsGalleryRenderProxy();
    if (!mRenderProxy->Initialize()){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsGalleryControllerApp][Init]: "
            <<"Failed to initialize gallery render proxy.";
        return TPS_FAILURE;
    }
    mMPROperator = new TpsGcaMPROperator();
    // create mpr operator interface
    if (nullptr == mMPROperator) {
        
        mMPROperator->Initialize();
    }

    // create internal interface impl instances
    // TODO: thread safety not consider here, add in the future
    // create data loader interface
    if (nullptr == mDataLoader) {
        mDataLoader = new TpsGcaDataLoaderImpl();
        mDataLoader->Initialize();
    }

    // create data accessor interface
    if (nullptr == mDataAccessor) {
        mDataAccessor = new TpsGcaDataAccessorImpl();
        mDataAccessor->Initialize();
    }

    return TPS_SUCCESS;
}


int GalleryControllerApp::End(){
    //Uninitialized Render controller
    /*
    TpsControllerBase::End();*/

    return TPS_SUCCESS;
}

int GalleryControllerApp::CreateViewModule(WINDOW_TYPE window, DISPLAY_SIZE displaySize, 
    LAYOUT_UNIT unit){
    if (window == GALLERY_AXIAL ||
        window == GALLERY_CORONAL ||
        window == GALLERY_SAGITTAL ||
        window == GALLERY_3D) {
        mRenderProxy->RemoveRenderController(window);
    }
    if(!mRenderProxy->CreateViewModule(window, displaySize, unit)){
        TPS_LOG_DEV_ERROR<<"CreateViewModule failed!";
        return TPS_FAILURE;
    }

    // No Render when creating ViewModule
    //this->Render_i();

    return TPS_SUCCESS;
}

bool GalleryControllerApp::HasViewModule(WINDOW_TYPE window){
    return mRenderProxy->HasWindow(window);
}

int GalleryControllerApp::ResizeViewModule(WINDOW_TYPE window, DISPLAY_SIZE displaySize) {
    if (!mRenderProxy->ResizeViewModule(window, displaySize)) {
        return TPS_FAILURE;
    }

    return TPS_SUCCESS;
}

int GalleryControllerApp::LoadImageData(const std::string& sImageDataUID){

    if(TpsGalleryImageManager::GetInstance()->IsSeriesInSeries4D(sImageDataUID)){
        TpsGalleryImageManager::GetInstance()->SetIsPlaying(true);
    }

    if (mCurrentImageDataUid == sImageDataUID){
        return TPS_SUCCESS;
    }

    if(!TpsGalleryImageManager::GetInstance()->IsSeriesExists(sImageDataUID)){
        TPS_LOG_DEV_ERROR<<"Image3D does not exist.";
        return TPS_FAILURE;
    }

    TpsGalleryImageManager::GetInstance()->SetCurrentImage3D(sImageDataUID);
    
    this->Render_i();

    return TPS_SUCCESS;
}

int GalleryControllerApp::LoadImage4DData(const std::vector<std::string>& cerSeries4DUidList){

    if(cerSeries4DUidList.size() <= 0){
        return false;
    }
    TpsGalleryImageManager::GetInstance()->InitSeries4DUidList(cerSeries4DUidList);

    std::string sImageDataUID = cerSeries4DUidList[0];

    if (mCurrentImageDataUid == sImageDataUID){
        return TPS_SUCCESS;
    }

    if(!TpsGalleryImageManager::GetInstance()->IsSeriesExists(sImageDataUID)){
        TPS_LOG_DEV_ERROR<<"Image3D is null.";
        return TPS_FAILURE;
    }


    TpsGalleryImageManager::GetInstance()->SetDefaultImage3D(sImageDataUID);

    Render_i();

    return TPS_SUCCESS;
}   


void GalleryControllerApp::Render_i(){
    std::string sImage3DUID("");
    TpsImage3DEntity* image3Dptr = nullptr;
    
    if (!TpsGalleryImageManager::GetInstance()->GetCurrentImage3DEntity(sImage3DUID, &image3Dptr))
    {
       return;
    }

    if (sImage3DUID.empty() || nullptr == image3Dptr){
        TPS_LOG_DEV_WARNING<<"GetDefaultImage3DData failed as mCurrentImageDataUid is empty or image3D is null";
        return;
    }

    //save current image data UID
    mCurrentImageDataUid = sImage3DUID;

    // TODO Call ControllerBase->SetDataSource; Gallery_mprrendercontroller->ResetDataSource_i; 
    //          Gallery_mprrendercontroller->ResetImage_i;
    //          CheckGraphObject
    mRenderProxy->SetDataSource(IMAGE, mCurrentImageDataUid, SOURCE_GROUP_GALLERY);

    // TODO : Render Twice here !!!
    mMPROperator->MPRCornerImageTextVisible(mCurrentImageDataUid, TpsGalleryImageManager::GetInstance()->GetIsVisibleOfImageText());

    mRenderProxy->Render();
}

/// \brief GetMPROperator
///  
/// \return IMPROperator*    pointer to IMPROperator interface
IMPROperator* GalleryControllerApp::GetMPROperator()
{
    return mMPROperator;
}

int GalleryControllerApp::UpdateCrosshair(LAYOUT_UNIT unit, int nMouseAction, 
    double xStart, double yStart, double xEnd, double yEnd, 
    int nHitTestMode) {
        WINDOW_TYPE type;

        TpsLayoutManager* pLayoutManager = mRenderProxy->GetLayoutManager();
        if(pLayoutManager == nullptr)
        {
            TPS_LOG_DEV_ERROR<<"ContouringControllerApp::ManualFusion - failed to get layout manager";
            return TPS_FAILURE;
        }
        if(!pLayoutManager->GetWindowtype(unit, type)){
            TPS_LOG_DEV_ERROR<<"ContouringControllerApp::ManualFusion - GetWindowtype failed mUnit:"<<unit.viewerControlID;
            return TPS_FAILURE;
        }

        int nResult = mRenderProxy->UpdateCrosshair(this->mCurrentImageDataUid, type, 
            nMouseAction, // 0: move 1: drag
            nHitTestMode, // 0: use previous hittest result,  1: need hittest 
            xStart, yStart, 
            xEnd, yEnd);
        return nResult;
}

int GalleryControllerApp::UpdateCrosshairMode(int nCrosshairMode, int nVisible) {

    int nResult = mRenderProxy->UpdateCrosshairMode(this->mCurrentImageDataUid,
        nCrosshairMode,                 // 0: orthogonal 1: oblique, 2: single line
        nVisible);
    return nResult;
}

ITpsGcaDataAccessor* GalleryControllerApp::GetDataAccessor() {
    return mDataAccessor;
}

ITpsGcaDataLoader* GalleryControllerApp::GetDataLoader() {
    return mDataLoader;
}

int GalleryControllerApp::GetCrosshairMode(int& iCrosshairMode){
    int nResult = mRenderProxy->GetCrosshairMode(this->mCurrentImageDataUid, iCrosshairMode);
    return nResult;
}

void GalleryControllerApp::ClearImageDatas(){
    mCurrentImageDataUid = "";
    this->mRenderProxy->RemoveGraphicObjects();
    TpsGalleryImageManager::GetInstance()->ClearManager();
    TpsGalleryPatientManager::GetInstance()->ClearManager();
}

void GalleryControllerApp::ClearImageData(const std::string& sSeriesUID){
    mCurrentImageDataUid = "";
    this->mRenderProxy->RemoveGraphicObjects(sSeriesUID);
    TpsGalleryImageManager::GetInstance()->ClearImageData(sSeriesUID);
}

bool GalleryControllerApp::RotateShadedSurface(const Mcsf::Point2f& prePt, const Mcsf::Point2f &curPt){
    if (mCurrentImageDataUid.empty()){
        return true;
    }
    
    return this->mRenderProxy->RotateShadedSurface(prePt, curPt);
}

bool GalleryControllerApp::GetCurrentMPRSlice(WINDOW_TYPE strWidowType, int& currentSlice, int& totalSlice){
    if (mCurrentImageDataUid.empty()) {
        TPS_LOG_DEV_ERROR<<"[GetCurrentMPRSlice] : mCurrentImageDataUid is empty.";
        return false;
    }
    if (!this->mRenderProxy->GetCurrentMPRSlice(strWidowType, currentSlice, totalSlice)){
        TPS_LOG_DEV_ERROR<<"[GetCurrentMPRSlice] : Failed to GetCurrentMPRSlice.";
        return false;
    }
    return true;
}

bool GalleryControllerApp::DeleteSliceInLocal(const std::string& sSeriesUID, int& currentSlice) {

    if (mCurrentImageDataUid != sSeriesUID) {
        TPS_LOG_DEV_ERROR<<"[DeleteSliceInLocal] : sSeriesUID not equal to mCurrentImageDataUid .";
        return false;
    }
    if (!TpsGalleryImageManager::GetInstance()->DeleteSlice(sSeriesUID, currentSlice)) {  
        TPS_LOG_DEV_ERROR<<"[DeleteSliceInLocal] : Failed to Delete Slice .";
        return false;
    }
    this->mRenderProxy->RemoveGraphicObjects(sSeriesUID);
    mCurrentImageDataUid = "";
    this->LoadImageData(sSeriesUID);
    return true;
}

std::string GalleryControllerApp::GetCurrentSeriesUID() {
    return mCurrentImageDataUid;
}

std::string GalleryControllerApp::GetReplyString(bool bIsSucceeded) {

    return bIsSucceeded ? "0" : "-1";
}


TPS_END_NAMESPACE
