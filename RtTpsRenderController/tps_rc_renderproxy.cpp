////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rc_rendermanager.cpp
/// 
///  \brief class TpsRenderManager implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/12
////////////////////////////////////////////////////////////////
#include"StdAfx.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include <numeric>      // std::iota
#include <algorithm>    // std::sort
#include <functional>   // std::bind
#include "boost/lexical_cast.hpp"

#include "McsfMedViewer3DArithmetic/point3d.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "Mcsf3DArithmetic/mcsf_3d_vector3f.h"


#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsAlgorithmProxy/tps_pl_datastructure.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"
#include "RtTpsFramework/tps_fw_series_group4d.h"

#include "RtTpsFramework/tps_fw_rendercontrollerbase.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_datasourcegroup.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_doseline.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_data.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_refreshdoseoperation.h"
#include "RtTpsRenderLibrary/tps_rl_refreshdoselineoperation.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_addpoioperation.h"
#include "RtTpsRenderLibrary/tps_rl_addvoioperation.h"
#include "RtTpsRenderLibrary/tps_rl_preview_voi_operation.h"
#include "RtTpsRenderLibrary/tps_rl_addbeamoperation.h"
#include "RtTpsRenderLibrary/tps_rl_deleteoperation.h"
#include "RtTpsRenderLibrary/tps_rl_updatebeamoperation.h"
#include "RtTpsRenderLibrary/tps_rl_editdosegridboundaryoperation.h"
#include "RtTpsRenderLibrary/tps_rl_showdosegridboundaryoperation.h"
#include "RtTpsRenderLibrary/tps_rl_updatepoioperation.h"
#include "RtTpsRenderLibrary/tps_rl_placecamerafrombeamoperation.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_updatesetupcrosshairoperation.h"
#include "RtTpsRenderLibrary/tps_rl_addnormalgroupoperation.h"
#include "RtTpsRenderLibrary/tps_rl_update_normal_group_operation.h"
#include "RtTpsRenderLibrary/tps_rl_update_corner_info_operation.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_movement_operation.h"
#include "RtTpsRenderLibrary/tps_rl_dose_update_mesh_operation.h"
#include "RtTpsRenderLibrary/tps_rl_hot_spots_operation.h"
#include "RtTpsRenderLibrary/tps_rl_highlightbeamoperation.h"
#include "RtTpsRenderLibrary/tps_rl_updatesegmentoperation.h"
#include "RtTpsRenderLibrary/tps_rl_change_ct2densitytable_operation.h"
#include "RtTpsRenderLibrary/tps_rl_update_graphic_object_operation.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"


// easy plan
#include "RtTpsCertifiedLib/rt_tps_certifiedmlceditinfo.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_measuredistance_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setwindowing_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setpan_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setrotation_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setzoom_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setisocenter_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setnormpoint_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_deleteisocenter_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_deletenormpoint_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplanchangebeamoperation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_resetimage_operation.h"
//#include "RtTpsRenderLibrary/tps_rl_easyplan_changeapperturetype_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_measureangle_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_modifybeam_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setinverse_operation.h"
#include "RtTpsRenderLibrary/tps_rl_update_block_operation.h"
//#include "RtTpsRenderLibrary/tps_rl_move_voi_table_operation.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_loadimage_operation.h"

// shaded surface
#include "RtTpsRenderLibrary/tps_rl_rotateshadedsurfaceoperation.h"
#include "RtTpsRenderLibrary/tps_rl_shadedsurface_zoom_operation.h"
#include "RtTpsRenderLibrary/tps_rl_updatelightsettingoperation.h"
#include "RtTpsRenderLibrary/tps_rl_3d_window_pan_zoom_rotate_operation.h"
#include "RtTpsRenderController/tps_rc_shadedsurfacerendercontroller.h"

// fusion
#include "RtTpsRenderLibrary/tps_rl_refresh_registration_result_operation.h"
#include "RtTpsRenderLibrary/tps_rl_refreshfusionfactoroperation.h"
#include "RtTpsRenderLibrary/tps_rl_refreshfusiontypeoperation.h"
#include "RtTpsRenderLibrary/tps_rl_editchessboardooperation.h"
#include "RtTpsRenderLibrary/tps_rl_updatefusionregionoperation.h"
#include "RtTpsRenderLibrary/tps_rl_pseudocoloroperation.h"
#include "RtTpsRenderLibrary/tps_rl_remove_registration_relationship_operation.h"

// mpr
#include "RtTpsRenderLibrary/tps_rl_mprpageturn_operation.h"
#include "RtTpsRenderLibrary/tps_rl_mprreset_operation.h"
#include "RtTpsRenderLibrary/tps_rl_mprzoompanwindowing_operation.h"

//bev
#include "RtTpsRenderLibrary/tps_rl_bevdisplayoperation.h"
#include "RtTpsRenderLibrary/tps_rl_updatedrrdisplayoperation.h"
#include "RtTpsRenderLibrary/tps_rl_bevreset_operation.h"
#include "RtTpsRenderLibrary/tps_rl_switch_bev_dispay_mode_operation.h"
#include "RtTpsRenderLibrary/tps_rl_switchsegmentoperation.h"

//edit voi operation 
#include "RtTpsRenderLibrary/tps_rl_updatevoioperation.h"
#include "RtTpsRenderLibrary/tps_rl_updatevoidisplayoperation.h"

#include "RtTpsRenderLibrary/tps_rl_voieditingredoundooperation.h"

//#include "RtTpsRenderLibrary/tps_rl_edit_voi_table_operation.h"

#include "RtTpsRenderLibrary/tps_rl_setrulervisibility_operation.h"
#include "RtTpsRenderLibrary/tps_rl_setdrrvisibility_operation.h"
#include "RtTpsRenderLibrary/tps_rl_setmlctransparency_operation.h"

#include "RtTpsRenderLibrary/tps_rl_updatedrrsettingoperation.h"
#include "RtTpsRenderLibrary/tps_rl_updatebeamtemplatepreviewoperation.h"
#include "RtTpsRenderLibrary/tps_rl_edit_arc_beam_cache_operation.h"
#include "RtTpsRenderLibrary/tps_rl_set_drr_down_sample_rate_operation.h"
#include "RtTpsRenderLibrary/tps_rl_change_all_poi_visibility_operation.h"

//gallery operation
#include "RtTpsRenderLibrary/tps_rl_gallery_mprzoompanwindowing_operation.h"
#include "RttpsRenderLibrary/tps_rl_gallery_mprpageturn_operation.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_crosshair_movement_operation.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_crosshair_rotate_operation.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_crosshair_update_operation.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_mprupdatecornerinfo_operation.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_vrrotate_operation.h"

#include "RtTpsRenderController/tps_rc_rendercontrollerfactory.h"
#include "RtTpsRenderController/tps_rc_mpr_render_controller.h"
#include "RtTpsRenderController/tps_rc_bevrendercontroller.h"
#include "RtTpsRenderController/tps_rc_easyplan_image_rendercontroller.h"
#include "RtTpsRenderController/tps_rc_voi_preview_rendercontroller.h"
#include "RtTpsRenderController/tps_rc_beam_preview_rendercontroller.h"
#include "RtTpsRenderController/tps_rc_gallery_mprrendercontroller.h"
#include "RtTpsRenderController/tps_rc_gallery_shadedsurfacerendercontroller.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_displaycache.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprdosegraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_block_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"

#include "tps_logger.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"
#include "RtTpsRenderController/tps_rc_mlcrendercontroller.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_rotate_operation.h"

#include "RtTpsRenderController/tps_rc_rendercontroller.h"
#include "RtTpsRenderLibrary/tps_rl_updateshadedsurfaceGoOperation.h"

#include "tps_performance.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_update_operation.h"
#include "RtTpsRenderLibrary/tps_rl_render_helper.h"
#include "RtTpsRenderLibrary/tps_rl_calc_jump_to_step_operation.h"
#include "RtTpsRenderLibrary/tps_rl_query_current_mpr_slice_operation.h"
#include "RtTpsRenderLibrary/tps_rl_deletedrrimage_operation.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsRenderProxy::TpsRenderProxy() {

}

TpsRenderProxy::~TpsRenderProxy() {

}

bool TpsRenderProxy::Initialize(ITpsDataRepository* dataRepository)
{
    return TpsRenderProxyBase::Initialize(dataRepository);
}

TpsRenderControllerBase* TpsRenderProxy::CreateRenderController_i(WINDOW_TYPE type) 
{
    return TpsRenderControllerFactory::GetInstance()->CreateRenderController(type);
}

GOConverterFactoryBase* TpsRenderProxy::CreateGOConverterFactory_i(){
    return new TpsGOConverterFactory();
}

// true:supported FBO false: unsupported FBO
bool TpsRenderProxy::CheckFBO()
{
	return TpsRenderHelper::CheckFBO();
}

bool TpsRenderProxy::CheckDataSourceGroup(WINDOW_TYPE window, DATA_SOURCE_GROUP_ID& id)
{
    //maybe need use switch-case to exclude invalid window type
    id = (DATA_SOURCE_GROUP_ID)((window & 0xff00) >> 8);
    return true;
}

bool TpsRenderProxy::RefreshDose(const std::string& planUID ){

    if (planUID.empty()){
        TPS_LOG_DEV_ERROR<<"The plan UID is empty.";
        return false;
    }

    std::shared_ptr<TpsRefreshDoseOperation> refreshDose(
        new TpsRefreshDoseOperation(planUID));
    return ProcessOperation(refreshDose);
}


bool TpsRenderProxy::RefreshDoseLine(std::string planUid){

    std::shared_ptr<TpsRefreshDoseLineOperation> refreshDoseLineOperation(new TpsRefreshDoseLineOperation(planUid));
    ProcessOperation(refreshDoseLineOperation);

    std::shared_ptr<DoseUpdateMeshOperation> spDoseUpdateMeshOp(new DoseUpdateMeshOperation(planUid));
    ProcessOperation(spDoseUpdateMeshOp);

    return true;
}

bool TpsRenderProxy::GetCurrentMprSlice(WINDOW_TYPE window, const std::string& wndUid,
    int& slice, int& startSliceNumber, int& totalSlice)
{
    auto renderController = this->GetRenderController(window, wndUid);
    auto mprRenderController = dynamic_cast<TpsMPRRenderController*>(renderController);
    if (mprRenderController == nullptr){
        TPS_LOG_DEV_ERROR<<"The render controller is not a TpsMPRRenderController.";
        return false;
    }

    auto section = mprRenderController->GetSectionType();
    std::string seriesUid;
    mprRenderController->GetDataSource(IMAGE, seriesUid);

    std::shared_ptr<QueryMprSliceInfoOperation> op(
        new QueryMprSliceInfoOperation(seriesUid, section));
    ProcessOperation(op);

    op->GetSliceInfo(slice, totalSlice, startSliceNumber);
    return true;
}

bool TpsRenderProxy::SetCrosshairHittedMode(int wndType, const std::string& wndUid, int mode)
{
    auto renderController = this->GetRenderController((WINDOW_TYPE)wndType, wndUid);
    auto mprRenderController = dynamic_cast<TpsMPRRenderController*>(renderController);
    if (mprRenderController == nullptr){
        TPS_LOG_DEV_ERROR<<"The render controller is not a TpsMPRRenderController.";
        return false;
    }

    //if crosshair moving begin, hide non image overlays
    //if end, resume displaying.
    //bool isToHide = mode != CrosshairGraphicObject::HITTED_NOTHING;
    //如何十字线移动结束，更新VOI， Dose line
    //if (!isToHide){
        //auto section = mprRenderController->GetSectionType();
        //std::string seriesUID, planUID;
        //if (!this->GetDataSource((WINDOW_TYPE)wndType, IMAGE, seriesUID)){
        //    TPS_LOG_DEV_ERROR<<"Failed to get image data source.";
        //    return false;
        //}
        //this->GetDataSource((WINDOW_TYPE)wndType, PLAN, planUID);

        //unsigned char ucSectionTypes[] = {AXIAL, SAGITTAL, CORONAL};
        //std::string operateSeriesUid = mDataRepository->GetImageDataManager(
        //    )->GetOperationImageUid(seriesUID);
        //for (int i = 0; i < sizeof(ucSectionTypes) / sizeof(ucSectionTypes[0]); i++)
        //{
        //    if (ucSectionTypes[i] == section) 
        //        continue;
        //    //3. update voi cache collection
        //    std::shared_ptr<TpsUpdateVOICacheCollectionOperation>
        //        updateVOICacheCollectionOp(new TpsUpdateVOICacheCollectionOperation(
        //        operateSeriesUid, ucSectionTypes[i]));
        //    updateVOICacheCollectionOp->SetDataRepository(mDataRepository);
        //    this->ProcessOperation(updateVOICacheCollectionOp);

        //    //4. update dose line buffer
        //    if (!planUID.empty()){
        //        std::shared_ptr<TpsRefreshDoseOperation> refreshDose(
        //            new TpsRefreshDoseOperation(planUID, ucSectionTypes[i]));
        //        refreshDose->SetDataRepository(mDataRepository);
        //        this->ProcessOperation(refreshDose);
        //    }
        //}
    //}
    //HideMprNonImage((WINDOW_TYPE)wndType, wndUid, isToHide);
    mprRenderController->SetCrosshairHittedMode(mode);
    return true;
}

bool TpsRenderProxy::AddBeam(const std::string& planUID,
    const std::string& beamUID/*,const std::string &seriesUID*/)
{
    if (planUID.empty() || beamUID.empty()/* || seriesUID.empty()*/){
        TPS_LOG_DEV_ERROR<<"The plan UID or beam UID is empty.";
        return false;
    }

    std::shared_ptr<TpsAddBeamOperation> addBeamOp(
        new TpsAddBeamOperation(planUID, beamUID));
    ProcessOperation(addBeamOp);
    //std::shared_ptr<PlaceCameraFromBeamOperation> placeCamera(
    //    new PlaceCameraFromBeamOperation(planUID,seriesUID));
    //ProcessOperation(placeCamera);
    return true;
}

bool TpsRenderProxy::DeleteBeam(const std::string& planUid, const std::string &beamUid) 
{
    if(planUid.empty() || beamUid.empty()) {
        TPS_LOG_DEV_ERROR<<"the plan uid or beam uid is empty!";
        return false;
    }
    std::shared_ptr<TpsDeleteBeamOperation> deleteBeamOp(
        new TpsDeleteBeamOperation(planUid,beamUid));
    ProcessOperation(deleteBeamOp);
    return true;
}

bool TpsRenderProxy::UpdateBeam(const std::string& ngUID, const std::string& beamUID)
{
    if (ngUID.empty() || beamUID.empty()){
        TPS_LOG_DEV_ERROR<<"The NG UID or beam UID is empty.";
        return false;
    }

    std::shared_ptr<TpsUpdateBeamOperation> updateBeamOp(
        new TpsUpdateBeamOperation(ngUID, beamUID));
    ProcessOperation(updateBeamOp);
    return true;
}

bool TpsRenderProxy::EditDoseGridBoundary(const std::string& planUID, 
    Mcsf::Point3f startPoint, 
    Mcsf::Point3f endPoint, Mcsf::Point3f spacing) 
{
    std::shared_ptr<TpsEditDoseGridBoundaryOperation> editGridOp(
        new TpsEditDoseGridBoundaryOperation(planUID, startPoint, endPoint, spacing));
    ProcessOperation(editGridOp);
    return true;
}

bool TpsRenderProxy::ShowDoseGridBoundary(const std::string& planUID, bool isShowBoundary) 
{
    std::shared_ptr<TpsShowDoseGridBoundaryOperation> showGridOp(
        new TpsShowDoseGridBoundaryOperation(planUID, isShowBoundary));
    ProcessOperation(showGridOp);
    return true;
}

bool TpsRenderProxy::AddVOI(const std::string& voiUID, const std::string& imageUID)
{
    std::shared_ptr<TpsAddVOIOperation> addVOIOp(
        new TpsAddVOIOperation(imageUID, voiUID));
    return ProcessOperation(addVOIOp);
}

void TpsRenderProxy::JumpTo(const std::string& seriesUid, float x, float y, float z)
{
    FIRST_POSTFIX_COMPONENT sectionType[3] = { CORONAL, SAGITTAL, AXIAL };
    SECOND_POSTFIX_COMPONENT extension = EMPTY_COMPONENT;
    std::shared_ptr<CalcJumpToStepOperation> op;
    for (int i = 0; i < 3; ++i)
    {
        op.reset(new CalcJumpToStepOperation(
            seriesUid, x, y, z, sectionType[i], extension));
        if (!ProcessOperation(op)) continue;

        MprPageTurn_i(seriesUid, sectionType[i], extension, op->GetStep(), 
            Refresh_Voi | Refresh_Table | Refresh_Dose);
    }
}

void TpsRenderProxy::VoiPreviewJumpTo(const std::string& seriesUid, float x, float y, float z)
{
    FIRST_POSTFIX_COMPONENT sectionType[3] = { CORONAL, SAGITTAL, AXIAL };
    SECOND_POSTFIX_COMPONENT extension = VOI_PREVIEW;
    std::shared_ptr<CalcJumpToStepOperation> op;
    for (int i = 0; i < 3; ++i)
    {
        op.reset(new CalcJumpToStepOperation(
            seriesUid, x, y, z, sectionType[i], extension));
        if (!ProcessOperation(op)) continue;

        MprPageTurn_i(seriesUid, sectionType[i], extension, op->GetStep(), Refresh_Voi);
    }
}

bool TpsRenderProxy::AddPoi(const std::string &strSeriesUid, const std::string& strPoiUid)
{
    std::shared_ptr<TpsAddPoiOperation> addPoiOp(new TpsAddPoiOperation(strSeriesUid, strPoiUid));
    ProcessOperation(addPoiOp);
    return true;
}

bool TpsRenderProxy::DeletePoi(const std::string &strSeriesUid, const std::string& strPoiUid )
{
    std::shared_ptr<TpsDeletePOIOperation> deletePoiOp(new TpsDeletePOIOperation(strSeriesUid, strPoiUid));
    ProcessOperation(deletePoiOp);
    return true;
}

bool TpsRenderProxy::DeleteMaxPoint(const std::string &strPlanUid, const std::string& strPoiUid)
{
    std::shared_ptr<TpsDeleteMaxPointOperation> deleteMaxPointOp(new TpsDeleteMaxPointOperation(strPlanUid, strPoiUid));
    ProcessOperation(deleteMaxPointOp);
    return true;
}

void TpsRenderProxy::UpdatePOI(const std::string &strSeriesUid, const std::string &poiUid ) 
{
    std::shared_ptr<TpsUpdatePoiOperation> updatePoiOp(new TpsUpdatePoiOperation(strSeriesUid, poiUid));
    ProcessOperation(updatePoiOp);
    return;
}

void TpsRenderProxy::DeleteVoi(const std::string& imageUid, const std::string& voiUid)
{
    std::shared_ptr<TpsDeleteVOIOperation> deleteVoiOperation(new
        TpsDeleteVOIOperation(imageUid, voiUid));
    ProcessOperation(deleteVoiOperation);
}

void TpsRenderProxy::UpdateVOI(const std::string &imageUid, const std::string &voiUid)
{
    std::shared_ptr<TpsUpdateVoiOperation> updateVOI(
        new TpsUpdateVoiOperation(imageUid,voiUid));
    ProcessOperation(updateVOI);

//     FIRST_POSTFIX_COMPONENT sectionType[3] = {
//            CORONAL, SAGITTAL, AXIAL
//     };
// 
//     for(int i =0; i <3; ++i) {
//         std::shared_ptr<UpdateVoiCacheOperation> updateCache (
//             new UpdateVoiCacheOperation(imageUid, sectionType[i], voiUid));
//         ProcessOperation(updateCache);
//     }
    return;
}

void TpsRenderProxy::UpdateVOIAttribute(const std::string &imageUid, const std::string &voiUid)
{
    std::shared_ptr<TpsUpdateVoiDisplayOperation> updateVOI(
        new TpsUpdateVoiDisplayOperation(imageUid, voiUid));
    ProcessOperation(updateVOI);
}

void TpsRenderProxy::ResetBeamViewCameras(const std::string& planUID)
{
    std::shared_ptr<PlaceCameraFromBeamOperation> placeCamera(
        new PlaceCameraFromBeamOperation(planUID));
    ProcessOperation(placeCamera);
}

//bool TpsRenderProxy::ResetMlcShape(const std::string& beamUID) {
//    std::shared_ptr<TpsEasyPlanResetMlcOperation> resetMlcShapeOp(
//        new TpsEasyPlanResetMlcOperation(beamUID));
//    ProcessOperation(resetMlcShapeOp);
//    return true;
//}


bool TpsRenderProxy::UpdateNormalGroup(const std::string& planUid, 
    const std::string& normGroupUid)
{
    if (planUid.empty() || normGroupUid.empty()){
        TPS_LOG_DEV_ERROR<<"The plan UID or normal group UID is empty.";
        return false;
    }

    std::shared_ptr<UpdateNormalGroupOperation> cuNormalGroupOp(
        new UpdateNormalGroupOperation(planUid, normGroupUid));
    return ProcessOperation(cuNormalGroupOp);
}

bool TpsRenderProxy::DeleteNormalGroup(const std::string& planUid, const std::string& normGroupUid)
{
    if (planUid.empty() || normGroupUid.empty()){
        TPS_LOG_DEV_ERROR<<"The plan UID or normal group UID is empty.";
        return false;
    }

    std::shared_ptr<TpsDeleteNormGroupOperation> deleteNormalGroupOp(
        new TpsDeleteNormGroupOperation(planUid, normGroupUid));
    return ProcessOperation(deleteNormalGroupOp);
}

bool TpsRenderProxy::AddNormalGroup(const std::string& planUid, 
    const std::string& normGroupUid)
{
    if (planUid.empty() || normGroupUid.empty()){
        TPS_LOG_DEV_ERROR<<"The plan UID or normal group UID is empty.";
        return false;
    }

    std::shared_ptr<AddNormalGroupOperation> op(
        new AddNormalGroupOperation(planUid, normGroupUid));
    return ProcessOperation(op);
}

void TpsRenderProxy::UpdateSetupCrossHair(
    const std::string& wndUid, WINDOW_TYPE wndType, 
    const std::string &imageuid, 
    const Mcsf::Point2f &mousePosition, bool isVisible)
{
    auto renderControllerBase = this->GetRenderController(wndType, wndUid);
    auto renderController = dynamic_cast<TpsMPRRenderController*>(renderControllerBase);
    if(nullptr == renderController){
        TPS_LOG_DEV_ERROR<<"Failed to get render via window type: "<<wndType;
        return;
    }
    FIRST_POSTFIX_COMPONENT section = renderController->GetSectionType();

    std::shared_ptr<TpsUpdateSetUpCrosshairOperation> setUpOp(
        new TpsUpdateSetUpCrosshairOperation(imageuid, mousePosition, isVisible ,section));
    ProcessOperation(setUpOp);
    return;
}

void TpsRenderProxy::EasyPlanUpdateSetupCrossHair( const std::string& wndUid, WINDOW_TYPE wndType,
	const Mcsf::Point2f &mousePosition, bool isVisible )
{
	wndUid;
	std::string imageUID;
	if (!this->GetDataSource(wndType, IMAGE, imageUID)) {
		TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window type: "
			<<wndType;
		return;
	}

	std::shared_ptr<TpsUpdateSetUpCrosshairOperation> setUpOp(
		new TpsUpdateSetUpCrosshairOperation(imageUID, mousePosition, isVisible ,EASYPLAN_IMAGE_SECTION));
	ProcessOperation(setUpOp);
	return;
}

void TpsRenderProxy::UpdateMPRCornerInformation(WINDOW_TYPE windowType, 
    const std::string& windowUid, double scrX, double scrY)
{
    std::string seriesUID;
    if (!this->GetDataSource(windowType, IMAGE, seriesUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window type: "
            <<windowType;
        return;
    }
    std::string planUID = "";
    this->GetDataSource(windowType, PLAN, planUID);

    //2. get section type
    auto renderController = this->GetRenderController(windowType, windowUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(renderController);
    FIRST_POSTFIX_COMPONENT section;
    if (mprRc != nullptr) {
        section = mprRc->GetSectionType();
    }
    else {
        TPS_LOG_DEV_ERROR<<"The window is not a mpr window.";
        return;
    }

    std::shared_ptr<UpdateCornerInfoOperation> spUpdateCornerInfoOp(
        new UpdateCornerInfoOperation(scrX, scrY, planUID));
    std::string strVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, section);
    std::string strMprKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    spUpdateCornerInfoOp->SetSeriesUid(seriesUID);
    spUpdateCornerInfoOp->SetSectionType(section);
    this->ProcessOperation(spUpdateCornerInfoOp);
}


//重置MPR 底图 并且 重置 十字线的四个点
bool TpsRenderProxy::ResetMPRObject(WINDOW_TYPE windowType, const std::string& windowUid)
{
    auto renderController = this->GetRenderController(windowType, windowUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(renderController);
    FIRST_POSTFIX_COMPONENT section;
    if (mprRc != nullptr) {
        section = mprRc->GetSectionType();
    }
    else {
        TPS_LOG_DEV_ERROR << "The window is not a mpr window.";
        return false;
    }

    std::string seriesUID;
    if (!this->GetDataSource(windowType, IMAGE, seriesUID)) {
        TPS_LOG_DEV_ERROR << "Can not get the image data source of current window type: "
            << windowType;
        return false;
    }

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, section);
    std::string goTypeStr = GOTypeHelper::ToString(goType);
    std::string key = seriesUID + "|" + goTypeStr;
    auto go = mModelWarehouse->GetModelObject(key);
    auto spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    double ww, wc;
    double factor = 1.0f;
    {
        TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
        spMprGO->GetWindowing(ww, wc);
        spMprGO->GetScale(&factor);
        spMprGO->SetScale(1.0f);
    }
    //重置前先保存窗宽窗位
    mprRc->ResetMPRObject();
    {
        TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
        spMprGO->SetWindowing(ww, wc);
    }
    this->MprZoom(windowType, windowUid, factor);

    std::string planUID = "";
    this->GetDataSource(windowType, PLAN, planUID);
    unsigned char ucSectionTypes[] = { AXIAL, SAGITTAL, CORONAL };
    std::string operateSeriesUid = mDataRepository->GetImageDataManager(
        )->GetOperationImageUid(seriesUID);
    for (int i = 0; i < sizeof(ucSectionTypes) / sizeof(ucSectionTypes[0]); i++)
    {
        if (ucSectionTypes[i] == section)
            continue;

        //4. update dose line buffer
        if (!planUID.empty()){
            std::shared_ptr<TpsRefreshDoseOperation> refreshDose(
                new TpsRefreshDoseOperation(planUID, ucSectionTypes[i]));
            refreshDose->SetDataRepository(mDataRepository);
            this->ProcessOperation(refreshDose);
        }
    }
    return true;
}

void TpsRenderProxy::UpdateCrosshair(WINDOW_TYPE windowType, 
    const std::string& windowUid, double xStart, double yStart, double xEnd, double yEnd, int crossMode, int hitMode, double rotateX, double rotateY)
{
    //printf("x:%.5f, y:%.5f\r\n", xEnd, yEnd);
    std::string seriesUID;
    if (!this->GetDataSource(windowType, IMAGE, seriesUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window type: "
            <<windowType;
        return;
    }
    std::string planUID = "";
    this->GetDataSource(windowType, PLAN, planUID);

    //2. get section type
    auto renderController = this->GetRenderController(windowType, windowUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(renderController);
    FIRST_POSTFIX_COMPONENT section;
    if (mprRc != nullptr) {
        section = mprRc->GetSectionType();
    }
    else {
        TPS_LOG_DEV_ERROR<<"The window is not a mpr window.";
        return;
    }

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(CROSSHAIR, section);
    std::string strKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    auto go = this->GetModelWarehouse()->GetModelObject(strKey);
    auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
    if (spCrosshairGO == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][CommonOperatorImpl::UpdateCrosshair]: "
            <<"Failed to dynamic cast IGraphicObjects.";
        return;
    }


    if (hitMode == CrosshairGraphicObject::HITTED_AXIS_Y_Top || hitMode == CrosshairGraphicObject::HITTED_AXIS_Y_Bottom || hitMode == CrosshairGraphicObject::HITTED_AXIS_X_Left || hitMode == CrosshairGraphicObject::HITTED_AXIS_X_Right) // Oblique mode
    {
        std::shared_ptr<CrosshairRotateOperation> crosshairOperation(
            new CrosshairRotateOperation(seriesUID, section, xStart, yStart, xEnd, yEnd, rotateX, rotateY, hitMode, crossMode));
        this->ProcessOperation(crosshairOperation);
    }
    else
    {
        std::shared_ptr<CrosshairMovementOperation> crosshairOperation(
            new CrosshairMovementOperation(seriesUID, section, xStart, yStart, xEnd, yEnd, hitMode, crossMode));
        this->ProcessOperation(crosshairOperation);
    }

    //std::shared_ptr<CrosshairUpdateOperation>
    //    crosshairUpdateOp(new CrosshairUpdateOperation(
    //    mModelWarehouse, seriesUID));
    //this->ProcessOperation(crosshairUpdateOp);

    unsigned char ucSectionTypes[] = {AXIAL, SAGITTAL, CORONAL};
    std::string operateSeriesUid = mDataRepository->GetImageDataManager(
        )->GetOperationImageUid(seriesUID);
    for (int i = 0; i < sizeof(ucSectionTypes) / sizeof(ucSectionTypes[0]); i++)
    {
        if (ucSectionTypes[i] == section) 
            continue;
        //update dose line buffer
        if (!planUID.empty()){
            std::shared_ptr<TpsRefreshDoseOperation> refreshDose(
                new TpsRefreshDoseOperation(planUID, ucSectionTypes[i]));
            refreshDose->SetDataRepository(mDataRepository);
            this->ProcessOperation(refreshDose);
        }
    }
}

void TpsRenderProxy::RotateShadedSurface(const std::string seriesUID, 
    const Mcsf::Point2f& prePt, const Mcsf::Point2f& curPt)
{
    std::shared_ptr<RotateShadedSurfaceOperation> rotateShadedSurface(
        new RotateShadedSurfaceOperation(seriesUID, prePt, curPt));
    this->ProcessOperation(rotateShadedSurface);
}

void TpsRenderProxy::ShadedSurfaceZoom(const std::string seriesUID, float fZoomFactor)
{
    std::shared_ptr<ShadedSurfaceZoomOperation> shadedSurfaceZoom(
        new ShadedSurfaceZoomOperation(seriesUID, fZoomFactor));
    this->ProcessOperation(shadedSurfaceZoom);
}

void TpsRenderProxy::Pan3DWindow(WINDOW_TYPE wndType, const std::string& wndUid, 
    float startX, float startY, float stopX, float stopY)
{
    auto renderController = this->GetRenderController(wndType, wndUid);
    if (renderController == nullptr) {
        TPS_LOG_DEV_ERROR << "Render controller of window: " << wndUid
            << "does not exists.";
        return;
    }
    DISPLAY_SIZE displaySize;
    renderController->GetDisplaySize(displaySize);

    std::string seriesUID;
    if (!this->GetDataSource(wndType, IMAGE, seriesUID))
    {
        TPS_LOG_DEV_ERROR << "Can not get the image data source of current window.";
        return;
    }

    std::shared_ptr<_3dWindowPanOperation> shadedSurfaceZoom(
        new _3dWindowPanOperation(seriesUID, displaySize, startX, startY, stopX, stopY));
    this->ProcessOperation(shadedSurfaceZoom);
}

bool TpsRenderProxy::GetShadedSurfaceViewDirecrion(WINDOW_TYPE windowType, const std::string& windowUid, 
    double& x1, double& y1, double& z1, double& x2, double& y2, double& z2) {
        auto renderController = this->GetRenderController(windowType, windowUid);
        if (renderController == nullptr) {
            TPS_LOG_DEV_ERROR << "Render controller of window: " << windowUid
                << "does not exists.";
            return false;
        }

        auto shadedSurfaceRenderController = dynamic_cast<ShadedSurfaceRenderController*>(renderController);
        if (shadedSurfaceRenderController == nullptr) {
            TPS_LOG_DEV_ERROR << "Render controller is not bev";
            return false;
        }

        Mcsf::MedViewer3D::Vector3D vecView, vecUp;
        if (!shadedSurfaceRenderController->GetShadedSurfaceViewDirecrion(vecView, vecUp)) {
            TPS_LOG_DEV_ERROR<<"GetShadedSurfaceViewDirecrion failed!";
            return false;
        } 
        else {
            x1 = vecView.x;
            y1 = vecView.y;
            z1 = vecView.z;
            x2 = vecUp.x;
            y2 = vecUp.y;
            z2 = vecUp.z;
        }

        return true;
}

void TpsRenderProxy::UpdateLightSetting(const std::string& strImageUid, int mprSection, float mprTransparency3D, float rx, float ry, float rz, 
    float scale, float specf, float ambf, float difff, float shininess, 
    float opacity, float lightx, float lighty, float lightz,int smoothness, 
    bool useCameraLight, int displayMode, bool displayVertexCube,
    int startStripIndex, int endStripIndex)
{
    // change light settings in shadedsurface GO
    std::shared_ptr<TpsUpdateLightSettingOperation> updateLightSetting(new TpsUpdateLightSettingOperation(strImageUid, mprSection, mprTransparency3D, rx, 
        ry, rz, scale, specf, ambf, difff, shininess, opacity, lightx, lighty, lightz,
        smoothness, useCameraLight, displayMode, displayVertexCube, startStripIndex, endStripIndex));
    this->ProcessOperation(updateLightSetting);
}

#pragma region [*****************Fusion Part*************************]
bool TpsRenderProxy::StartRegister()
{
    std::string fixedImage, floatImage;
    if (!this->GetDataSource(SOURCE_GROUP_FUSION_FIXED, IMAGE, fixedImage) || 
        !this->GetDataSource(SOURCE_GROUP_FUSION_FLOAT, IMAGE, floatImage)){
        TPS_LOG_DEV_ERROR<<"The registration data source are not ready.";
        return false;
    }

    this->SetDataSource(SOURCE_GROUP_FUSION_MIXED, IMAGE, fixedImage);
    this->SetDataSource(SOURCE_GROUP_FUSION_MIXED, FLOAT_IMAGE, floatImage);
    return true;
}

void TpsRenderProxy::RefreshRegistrationResult()
{
    std::string fixedImage, floatImage;
    if (!this->GetDataSource(SOURCE_GROUP_FUSION_FIXED, IMAGE, fixedImage) || 
        !this->GetDataSource(SOURCE_GROUP_FUSION_FLOAT, IMAGE, floatImage)){
        TPS_LOG_DEV_ERROR<<"The registration data source are not ready.";
        return;
    }

    std::shared_ptr<RefreshRegistrationResultOperation> op(
        new RefreshRegistrationResultOperation());
    this->ProcessOperation(op);
}

void TpsRenderProxy::RefreshApprovedRegistrationResult(
    const std::string& fixedUid, const std::string& floatUid)
{
    std::shared_ptr<RefreshApprovedRegistrationResultOperation> op(
        new RefreshApprovedRegistrationResultOperation(fixedUid, floatUid));
    this->ProcessOperation(op);
}

void TpsRenderProxy::RefreshFusionFactor(const std::string& fixedUid, 
    const std::string& floatUid, double factor)
{
    std::shared_ptr<RefreshFusionFactorOperation> op(
        new RefreshFusionFactorOperation(fixedUid, floatUid, factor));
    this->ProcessOperation(op);
}

void TpsRenderProxy::RefreshFusionType(const std::string& fixedUid, 
    const std::string& floatUid, int type)
{
    std::shared_ptr<RefreshFusionTypeOperation> op(
        new RefreshFusionTypeOperation(fixedUid, floatUid, type));
    this->ProcessOperation(op);
}

void TpsRenderProxy::RefreshFusionFactor(double factor)
{
    std::shared_ptr<RefreshFusionFactorOperation> op(
        new RefreshFusionFactorOperation(factor));
    this->ProcessOperation(op);
}

void TpsRenderProxy::RefreshFusionType(int type)
{
    std::shared_ptr<RefreshFusionTypeOperation> op(
        new RefreshFusionTypeOperation(type));
    this->ProcessOperation(op);
}

void TpsRenderProxy::EditChessBoard(WINDOW_TYPE wndType, const std::string& wndUid, 
    double x, double y, int actionType)
{
    std::string fixedImage, floatImage;
    if (!this->GetDataSource(wndType, IMAGE, fixedImage) ||
        !this->GetDataSource(wndType, FLOAT_IMAGE, floatImage)){
        TPS_LOG_DEV_ERROR<<"The registration data source are not ready.";
        return;
    }

    auto renderControllerBase = this->GetRenderController(wndType, wndUid);
    auto renderController = dynamic_cast<TpsMPRRenderController*>(renderControllerBase);
    if(nullptr == renderController){
        TPS_LOG_DEV_ERROR<<"Failed to get render via window type: "<<wndType;
        return;
    }

    DATA_SOURCE_GROUP_ID group;
    if (!CheckDataSourceGroup(wndType, group)){
        TPS_LOG_DEV_ERROR<<"Failed to get data source group id.";
        return;
    }
    bool isInFusionWnd = group == SOURCE_GROUP_FUSION_MIXED;

    FIRST_POSTFIX_COMPONENT section = renderController->GetSectionType();
    DISPLAY_SIZE displaySize;
    renderController->GetDisplaySize(displaySize);
    std::shared_ptr<EditChessBoardOperation> op(new EditChessBoardOperation(
        x, y, actionType, displaySize));
    auto goType = GOTypeHelper::ComposeAsGOType(FUSION_CHESS, section);
    std::string prefix = isInFusionWnd ? COMMON_BASE : 
        fixedImage + "|" + floatImage;
    std::string key = prefix + "|" + GOTypeHelper::ToString(goType);
    op->SetChessGOKey(key);
    key = prefix + "|" + GOTypeHelper::ToString(isInFusionWnd ? 
        GO_TYPE_VARYING_FUSION_GO : GO_TYPE_FUSION);
    op->SetFusionGOKey(key);
    goType = GOTypeHelper::ComposeAsGOType(MPR, section);
    key = fixedImage + "|" + GOTypeHelper::ToString(goType);
    op->SetMprGOKey(key);
    this->ProcessOperation(op);
}


void TpsRenderProxy::UpdateFusionRegion(FusionRegionUpdateType refreshType)
{
    std::shared_ptr<UpdateFusionRegionOperation> op(
        new UpdateFusionRegionOperation(refreshType));
    this->ProcessOperation(op);
}

void TpsRenderProxy::SetPseudoColor(const std::string& imageUID, 
    const std::string& pseudoColorName){

    GO_TYPE mprGoTypes[3] = { GO_TYPE_MPR_CORONAL, GO_TYPE_MPR_SAGITTAL, 
        GO_TYPE_MPR_AXIAL};
    std::vector<std::string> vecKeys;
    for (int i = 0; i < 3; ++i){
        vecKeys.push_back(imageUID + "|" + GOTypeHelper::ToString(mprGoTypes[i]));
    }
    std::shared_ptr<PseudoColorOperation> op(
        new PseudoColorOperation(pseudoColorName));
    op->SetGraphicObjectKeyList(vecKeys);
    this->ProcessOperation(op);
}
#pragma endregion

bool TpsRenderProxy::GetCrosshairProjectionPosition(WINDOW_TYPE windowType, 
    const std::string& windowUid, float& x2D, float& y2D)
{
    auto renderController = this->GetRenderController(windowType, windowUid);
    if (renderController == nullptr){
        TPS_LOG_DEV_ERROR << "Render controller of window: " << windowUid
            << "does not exists.";
        return false;
    }

    auto mprRenderController = dynamic_cast<TpsMPRRenderController*>(renderController);
    if (mprRenderController==nullptr) {
        TPS_LOG_DEV_ERROR << "Render controller is not mpr";
        return false;
    }
    if (!mprRenderController->GetCrosshairProjectionPosition(x2D, y2D)) {
        TPS_LOG_DEV_ERROR << "GetCrosshairProjectionPosition failed!";
        return false;
    }

    return true;
}


    bool TpsRenderProxy::GetCrossXYPoints(WINDOW_TYPE windowType,
    const std::string& windowUid, double& x1, double& y1, double& x2, double& y2, double& x3, double& y3, double& x4, double& y4)
{
    std::string seriesUID;
    if (!this->GetDataSource(windowType, IMAGE, seriesUID)) {
        TPS_LOG_DEV_ERROR << "Can not get the image data source of current window type: "
            << windowType;
        return false;
    }

    auto renderController = this->GetRenderController(windowType, windowUid);
    if (renderController == nullptr){
        TPS_LOG_DEV_ERROR << "Render controller of window: " << windowUid
            << "does not exists.";
        return false;
    }

    auto mprRenderController = dynamic_cast<TpsMPRRenderController*>(renderController);
    if (mprRenderController == nullptr) {
        TPS_LOG_DEV_ERROR << "Render controller is not mpr";
        return false;
    }

    std::shared_ptr<CrosshairUpdateOperation>
        crosshairUpdateOp(new CrosshairUpdateOperation(
        mModelWarehouse, seriesUID));
    this->ProcessOperation(crosshairUpdateOp);

    if (!mprRenderController->GetCrossXYPoints(x1, y1, x2, y2, x3, y3, x4, y4)) {
        TPS_LOG_DEV_ERROR << "GetCrosshairProjectionPosition failed!";
        return false;
    }

    return true;
}

bool TpsRenderProxy::GetMprViewDirecrion(WINDOW_TYPE windowType, const std::string& windowUid, 
    double& x1, double& y1, double& z1, double& x2, double& y2, double& z2) {
    auto renderController = this->GetRenderController(windowType, windowUid);
    if (renderController == nullptr) {
        TPS_LOG_DEV_ERROR << "Render controller of window: " << windowUid
            << "does not exists.";
        return false;
    }

    auto mprRenderController = dynamic_cast<TpsMPRRenderController*>(renderController);
    if (mprRenderController == nullptr) {
        TPS_LOG_DEV_ERROR << "Render controller is not mpr";
        return false;
    }

    Mcsf::MedViewer3D::Vector3D vecView, vecUp;
    if (!mprRenderController->GetMprViewDirecrion(vecView, vecUp)) {
        TPS_LOG_DEV_ERROR<<"GetMprViewDirecrion failed!";
        return false;
    } 
    else {
        x1 = vecView.x;
        y1 = vecView.y;
        z1 = vecView.z;
        x2 = vecUp.x;
        y2 = vecUp.y;
        z2 = vecUp.z;
    }

    return true;
}

double TpsRenderProxy::DistancePointToPlane(WINDOW_TYPE windowType, const std::string& windowUid,
    float x3D, float y3D, float z3D) {

        std::string seriesUID;
        if (!this->GetDataSource(windowType, IMAGE, seriesUID)) {
            TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
            return -1;
        }

        auto renderController = this->GetRenderController(windowType, windowUid);
        if (renderController == nullptr){
            TPS_LOG_DEV_ERROR << "Render controller of window: " << windowUid
                << "does not exists.";
            return -1;
        }

        auto mprRenderController = dynamic_cast<TpsMPRRenderController*>(renderController);
        if (mprRenderController == nullptr) {
            TPS_LOG_DEV_ERROR << "Render controller is not mpr";
            return -1;
        }
        return mprRenderController->GetDistanceToPlane(seriesUID, x3D, y3D, z3D);
}

#pragma region [************* Mpr PageTurn, Reset, Zoom, Pan, Windowing, ********************]
bool TpsRenderProxy::MprPageTurn(WINDOW_TYPE wndType, const std::string& wndUid, int iPageturn)
{
    //TEST_PERFORMANCE_INIT;
    std::string seriesUID;
    if (!this->GetDataSource(wndType, IMAGE, seriesUID))
    {
        TPS_LOG_DEV_ERROR << "Can not get the image data source of current window.";
        return false;
    }

    auto renderController = this->GetRenderController(wndType, wndUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(renderController);
    FIRST_POSTFIX_COMPONENT section;
    if (mprRc != nullptr)
    {
        section = mprRc->GetSectionType();
    }
    else
    {
        TPS_LOG_DEV_ERROR << "The window is not a mpr window.";
        return false;
    }

    this->MprPageTurn_i(seriesUID, section, EMPTY_COMPONENT,
        iPageturn, Refresh_Dose | Refresh_Table | Refresh_Voi);
    return true;
}


TPS_ERROR_CODES TpsRenderProxy::GetPatient2ViewMat(const WINDOW_TYPE wndType, const std::string& wndUid, Mcsf::MedViewer3D::Matrix4x4& mPatient2View)
{
    //TEST_PERFORMANCE_INIT;
    std::string seriesUID;
    if (!this->GetDataSource(wndType, IMAGE, seriesUID))
    {
        TPS_LOG_DEV_ERROR << "Can not get the image data source of current window.";
        return TPS_ER_NODATASOURCE;
    }

    FIRST_POSTFIX_COMPONENT section;
    auto renderController = this->GetRenderController(wndType, wndUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(renderController);
    if (mprRc != nullptr)
    {
        section = mprRc->GetSectionType();
    }
    else
    {
        TPS_LOG_DEV_ERROR << "The window is not a mpr window.";
        return TPS_ER_NOT_MPRWINDOW;
    }

    auto goType = GOTypeHelper::ComposeAsGOType(MPR, section, EMPTY_COMPONENT);
    std::string strKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(strKey);
    auto spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (spMprGO == nullptr)
    {
        TPS_LOG_DEV_ERROR << "[ERROR][MPRPageTurnOperation][ModifyGraphicObject]: "
            << "Failed to dynamic cast IGraphicObjects.";
        return TPS_ER_NULL_MPRGO;
    }


    // volume go
    std::string volumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    if (volume == nullptr) {
        TPS_LOG_DEV_ERROR << "Failed to get volume go!" << seriesUID;
        return TPS_ER_NULL_VOLUME;
    }

    Point3D aVertices[4];
    spMprGO->GetVertices(aVertices);
    Vector3D vecY = aVertices[0] - aVertices[3];
    Vector3D vecX = aVertices[1] - aVertices[0];
    vecY.Normalize();
    vecX.Normalize();
    Vector3D vecZ = vecX.CrossProduct(vecY);

    double scale, ratio;
    double planeWidth = (aVertices[1] - aVertices[0]).Magnitude();
    double planeHeight = (aVertices[3] - aVertices[0]).Magnitude();

    DISPLAY_SIZE size;
    mprRc->GetDisplaySize(size);
    double mWidth = size.width;
    double mHeight = size.height;

    Point3D ptOrigin = aVertices[3];
    double ratioPlane = planeWidth / planeHeight;
    double ratioView = (double)mWidth / mHeight;
    if (ratioPlane <= ratioView){
        scale = mHeight / planeHeight;
        ptOrigin += -1 * vecX*((mWidth / scale - planeWidth) / 2);
    }
    else{
        scale = mWidth / planeWidth;
        ratio = ratioPlane / ratioView;
        ptOrigin += -1 * vecY*((mHeight / scale - planeHeight) / 2);
    }

    Matrix4x4 matW2P = volume->GetWorld2Patient();
    Matrix4x4 patient2ViewMat = matW2P.Inverse();

    // 将space的大小作为Z轴方向缩放的单位距离
    double spacingInWorld;
    TpsMprHelper::CalcSpacingInWorld(volume, spMprGO, spacingInWorld);
    double scaleZ = 1.0 / spacingInWorld;

    Matrix4x4 transfromMat = Matrix4x4(1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -ptOrigin.x, -ptOrigin.y, -ptOrigin.z, 1);

    Matrix4x4 rotateMat = Matrix4x4(vecX.x, vecY.x, vecZ.x, 0,
        vecX.y, vecY.y, vecZ.y, 0,
        vecX.z, vecY.z, vecZ.z, 0,
        0, 0, 0, 1);

    Matrix4x4 scaleMat = Matrix4x4(scale, 0, 0, 0,
        0, scale, 0, 0,
        0, 0, scaleZ, 0,
        0, 0, 0, 1);

    patient2ViewMat.Prepend(transfromMat);
    patient2ViewMat.Prepend(rotateMat);
    patient2ViewMat.Prepend(scaleMat);

    mPatient2View = patient2ViewMat;

    return TPS_ER_SUCCESS;
}

bool TpsRenderProxy::MprReset(WINDOW_TYPE wndType, const std::string& wndUid, int acType)
{
    // 1. get the series uid
    std::string seriesUID;
    if (!this->GetDataSource(wndType, IMAGE, seriesUID))
    {
        TPS_LOG_DEV_ERROR << "Can not get the image data source of current window.";
        return false;
    }

    auto renderController = this->GetRenderController(wndType, wndUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(renderController);
    if (mprRc == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The window is not a mpr window.";
        return false;
    }
    FIRST_POSTFIX_COMPONENT section = mprRc->GetSectionType();

    // 3. execute the MPR reset command
    std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goType = GOTypeHelper::ComposeAsGOType(MPR, section);
    std::string sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    std::string sMPRRulerTextKey = sMPRKey;

    goType = GOTypeHelper::ComposeAsGOType(CROSSHAIR, section);
    std::string mprCrossHairKey = seriesUID + "|" + GOTypeHelper::ToString(goType);

    std::shared_ptr<MPRResetOperation> resetOperation(new MPRResetOperation(seriesUID, acType));
    resetOperation->SetGraphicObjectKeys(sVolumeKey, sMPRKey, mprCrossHairKey, sMPRRulerTextKey);

    FIRST_POSTFIX_COMPONENT sections[3] = {AXIAL, CORONAL, SAGITTAL};
    if (acType & 4)
    {
        std::vector<std::string> twoMprKeys;
        for (int i = 0; i < 3; ++i)
        {
            if (section == sections[i]) continue;
            goType = GOTypeHelper::ComposeAsGOType(MPR, sections[i]);
            std::string mprkey = seriesUID + "|" + GOTypeHelper::ToString(goType);
            twoMprKeys.push_back(mprkey);
        }
        resetOperation->SetTwoOtherKeys(twoMprKeys);
    }
    this->ProcessOperation(resetOperation);

    return true;
}

bool TpsRenderProxy::MprZoom(WINDOW_TYPE wndType, const std::string& wndUid, float scaleFactor)
{
    std::string seriesUID;
    if (!this->GetDataSource(wndType, IMAGE, seriesUID))
    {
        TPS_LOG_DEV_ERROR << "Can not get the image data source of current window.";
        return false;
    }

    auto renderController = this->GetRenderController(wndType, wndUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(renderController);
    if (mprRc == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The window is not a mpr window.";
        return false;
    }
    FIRST_POSTFIX_COMPONENT section = mprRc->GetSectionType();


    std::shared_ptr<MPRZoomOperation> zoomOperation(new MPRZoomOperation(scaleFactor));

    std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goType = GOTypeHelper::ComposeAsGOType(MPR, section);
    std::string sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    goType = GOTypeHelper::ComposeAsGOType(MPR_RULER_TEXT, section);
    std::string sMPRRulerTextKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    zoomOperation->SetGraphicObjectKeys(sVolumeKey, sMPRKey, sMPRRulerTextKey);
    this->ProcessOperation(zoomOperation);

    return true;
}

bool TpsRenderProxy::MprZoom(WINDOW_TYPE wndType, const std::string& wndUid, 
    float startX, float startY, float stopX, float stopY)
{
    std::string seriesUID;
    if (!this->GetDataSource(wndType, IMAGE, seriesUID))
    {
        TPS_LOG_DEV_ERROR << "Can not get the image data source of current window.";
        return false;
    }

    auto renderController = this->GetRenderController(wndType, wndUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(renderController);
    if (mprRc == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The window is not a mpr window.";
        return false;
    }
    FIRST_POSTFIX_COMPONENT section = mprRc->GetSectionType();
    DISPLAY_SIZE dispsize;
    renderController->GetDisplaySize(dispsize);

    std::shared_ptr<MPRZoomOperation> zoomOperation(new MPRZoomOperation(dispsize.width, dispsize.height,
        startX, startY, stopX, stopY));

    std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goType = GOTypeHelper::ComposeAsGOType(MPR, section);
    std::string sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    goType = GOTypeHelper::ComposeAsGOType(MPR_RULER_TEXT, section);
    std::string sMPRRulerTextKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    zoomOperation->SetGraphicObjectKeys(sVolumeKey, sMPRKey, sMPRRulerTextKey);
    this->ProcessOperation(zoomOperation);

    return true;
}

bool TpsRenderProxy::MprPan(WINDOW_TYPE wndType, const std::string& wndUid, 
    float startX, float startY, float stopX, float stopY)
{
    std::string seriesUID;
    if (!this->GetDataSource(wndType, IMAGE, seriesUID))
    {
        TPS_LOG_DEV_ERROR << "Can not get the image data source of current window.";
        return false;
    }

    auto renderController = this->GetRenderController(wndType, wndUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(renderController);
    if (mprRc == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The window is not a mpr window.";
        return false;
    }
    FIRST_POSTFIX_COMPONENT section = mprRc->GetSectionType();
    DISPLAY_SIZE dispsize;
    renderController->GetDisplaySize(dispsize);

    std::shared_ptr<MPRPanOperation> 
        panOperation(new MPRPanOperation(dispsize.width, dispsize.height, 
        startX, startY, stopX, stopY));

    std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goType = GOTypeHelper::ComposeAsGOType(MPR, section);
    std::string sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    panOperation->SetGraphicObjectKeys(sVolumeKey,sMPRKey);
    this->ProcessOperation(panOperation);

    return true;
}

bool TpsRenderProxy::MprWindowing(WINDOW_TYPE wndType, const std::string& wndUid, 
    float deltaWidth, float deltaCenter, bool updateAll)
{
    std::string seriesUID;
    if (!this->GetDataSource(wndType, IMAGE, seriesUID))
    {
        TPS_LOG_DEV_ERROR << "Can not get the image data source of current window.";
        return false;
    }

    auto renderController = this->GetRenderController(wndType, wndUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(renderController);
    if (mprRc == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The window is not a mpr window.";
        return false;
    }
    FIRST_POSTFIX_COMPONENT section = mprRc->GetSectionType();

    RtImage3DHeader* header;
    mDataRepository->GetImageDataManager()->GetImage3DHeader(seriesUID, header);
    std::string modality(header->m_cModality);
    if (modality.compare("PT") == 0)
    {
        deltaWidth *= 10;
        deltaCenter *= 10;
    }

    std::shared_ptr<MPRWindowingOperation> windowingOperation(
        new MPRWindowingOperation(deltaWidth, deltaCenter));
    std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    windowingOperation.reset(new MPRWindowingOperation(deltaWidth, deltaCenter));
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, section);
    std::string sCurrMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);

    std::string sMPRKey;
    std::vector<std::string> vecLinkedMprKey;
    if (updateAll)
    {
        FIRST_POSTFIX_COMPONENT sections[3] = { AXIAL, CORONAL, SAGITTAL};
        for (int i = 0; i < 3; ++i)
        {
            if (sections[i] == AXIAL)
            {
                vecLinkedMprKey.push_back(seriesUID + "|" + 
                    GOTypeHelper::ToString(GO_TYPE_MPR_AXIAL_PREV));
                vecLinkedMprKey.push_back(seriesUID + "|" + 
                    GOTypeHelper::ToString(GO_TYPE_MPR_AXIAL_POST));
            }
            if (sections[i] == section) continue;
            goType = GOTypeHelper::ComposeAsGOType(MPR, sections[i]);
            sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
            vecLinkedMprKey.push_back(sMPRKey);
        }
    }
    windowingOperation->SetGraphicObjectKeys(
        sVolumeKey, sCurrMPRKey, vecLinkedMprKey);
    this->ProcessOperation(windowingOperation);

    return true;
}

#pragma endregion


/************* Bev Pan, Zoom, Windowing, Bev2Cev, ResetBev********************/

bool TpsRenderProxy::BEVPan(WINDOW_TYPE wndType, const std::string& wndUid,
    float startX, float startY, float stopX, float stopY)
{
    std::string planUID ;
    if (!this->GetDataSource(wndType, PLAN, planUID))
    {
        TPS_LOG_DEV_ERROR << "Can not get the plan data source of current window.";
        return false;
    }

    auto renderController = this->GetRenderController(wndType, wndUid);
    auto bevRc = dynamic_cast<TpsBEVRenderController*>(renderController);
    if (bevRc == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The window is not a bev window.";
        return false;
    }
    DISPLAY_SIZE dispsize;
    renderController->GetDisplaySize(dispsize);

    Mcsf::Point2f preCoord, curCoord;
    preCoord.SetX(startX);
    preCoord.SetY(startY);
    curCoord.SetX(stopX);
    curCoord.SetY(stopY);

    std::shared_ptr<BEVPanOperation> 
        bevPan(new BEVPanOperation(preCoord, curCoord, planUID, dispsize));
    this->ProcessOperation(bevPan);
    return true;
}

bool TpsRenderProxy::BEVZoom(WINDOW_TYPE wndType, float zoomFactor)
{
    std::string imageUID;
    if (!this->GetDataSource(wndType, IMAGE, imageUID))
    {
        TPS_LOG_DEV_ERROR << "The image UID UID is empty.";
        return false;
    }

    std::string planUID;
    if (!this->GetDataSource(wndType, PLAN, planUID))
    {
        TPS_LOG_DEV_ERROR << "Can not get the plan data source of current window.";
        return false;
    }

    std::shared_ptr<BEVZoomOperation> 
        bevZoom(new BEVZoomOperation(zoomFactor, imageUID, planUID));
    this->ProcessOperation(bevZoom);
    return true;
}

bool TpsRenderProxy::BEVWindowing(WINDOW_TYPE wndType, const int &windowWidth, const int &windowCenter)
{
    std::string imageUID;
    if (!this->GetDataSource(wndType, IMAGE, imageUID))
    {
        TPS_LOG_DEV_ERROR << "The image UID UID is empty.";
        return false;
    }

    std::shared_ptr<UpdateDrrDisplayOperation> updateDrr(
        new UpdateDrrDisplayOperation(imageUID, windowWidth, windowCenter, 1.0f, true));
    this->ProcessOperation(updateDrr);
    return true;
}

bool TpsRenderProxy::BEVCEVSwitch(WINDOW_TYPE wndType)
{
    std::string planUID;
    if (!this->GetDataSource(wndType, PLAN, planUID))
    {
        TPS_LOG_DEV_ERROR << "Can not get the plan data source of current window.";
        return false;
    }

    std::shared_ptr<SwitchBEVCEVOperation> swithBevCev(
        new SwitchBEVCEVOperation(planUID));
    this->ProcessOperation(swithBevCev);
    return true;
}

bool TpsRenderProxy::BEVReset(WINDOW_TYPE wndType, int acType)
{
    std::string imageUID;
    if (!this->GetDataSource(wndType, IMAGE, imageUID))
    {
        TPS_LOG_DEV_ERROR << "The image UID UID is empty.";
        return false;
    }

    std::string planUID;
    if (!this->GetDataSource(wndType, PLAN, planUID))
    {
        TPS_LOG_DEV_ERROR << "Can not get the plan data source of current window.";
        return false;
    }

    std::shared_ptr<BEVResetOperation> 
        bevReset(new BEVResetOperation(imageUID, planUID, acType));
    this->ProcessOperation(bevReset);
    return true;
}

bool TpsRenderProxy::BevSwitchDisplayMode(int displayMode)
{
    std::shared_ptr<SwitchBevDisplayModeOperation> switchBevDisplayModeOp(
        new SwitchBevDisplayModeOperation(displayMode));
    return this->ProcessOperation(switchBevDisplayModeOp);
}

void TpsRenderProxy::SwitchSegment(const std::string& beamUid, int page) {
    std::shared_ptr<TpsSwithSegmentOperation> switchSegmentOp(
        new TpsSwithSegmentOperation(beamUid, page));
    this->ProcessOperation(switchSegmentOp);
}
/************* Bev Pan, Zoom, Windowing, Bev2Cev, ResetBev********************/

bool TpsRenderProxy::ChangeCT2DensityTable(const std::string& seriesUid, 
    const std::string& normUid, 
    const std::string& ct2densityUid) {
    if (seriesUid.empty() || normUid.empty() || ct2densityUid.empty()) {
        return false;
    }
    std::shared_ptr<ChangeCT2DensityTableOperation> changeCT2DensityTableOp(
        new ChangeCT2DensityTableOperation(seriesUid, normUid, ct2densityUid));
    return this->ProcessOperation(changeCT2DensityTableOp);
}

bool TpsRenderProxy::PreviewVOI(ROIEntity* pVOI, std::string strImageUid){
    std::shared_ptr<TpsPreviewVOIOperation> spPreviewVOIOp(
        new TpsPreviewVOIOperation(strImageUid, pVOI));
    this->ProcessOperation(spPreviewVOIOp);
    return true;
}

bool TpsRenderProxy::SetRulerVisibility(const std::string& strImageUid, bool isVisible)
{
    if(strImageUid.empty()){
        return false;
    }
    std::shared_ptr<SetRulerVisibilityOperation> setRulerVisibility(new SetRulerVisibilityOperation(strImageUid, isVisible));
    this->ProcessOperation(setRulerVisibility);
    return true;
}

bool TpsRenderProxy::SetDrrVisibility(bool isVisible)
{
    std::shared_ptr<SetDrrVisibilityOperation> setVisibility(new SetDrrVisibilityOperation(isVisible));
    bool bRet = this->ProcessOperation(setVisibility);
    return bRet;
}

bool TpsRenderProxy::SetMlcTransparency(float fTransparency)
{
    std::shared_ptr<SetMlcTransparencyOperation> setMlc(new SetMlcTransparencyOperation(fTransparency));
    bool bRet = this->ProcessOperation(setMlc);
    return bRet;
}

bool TpsRenderProxy::ChangePlanGoalSelection(DATA_SOURCE_GROUP_ID groupType, 
    int level, const std::string& uid) {
    bool bRes = false;
    switch (level) {
    case SELECTION_LEVEL_PLAN:
        bRes = this->ActivePlan(groupType, uid);
        break;
    case SELECTION_LEVEL_NORMAL_GROUP:
        bRes = this->ActiveNormGroup(groupType, uid);
        break;
    case SELECTION_LEVEL_BEAM:
        bRes = this->ActiveBeam(groupType, uid);
        break;
    case SELECTION_LEVEL_UNKNOWN:
        bRes = this->DeactiveCurrentPlan_i(groupType);
        break;
    }
    return bRes;
}

bool TpsRenderProxy::ActivePlan(DATA_SOURCE_GROUP_ID groupType, const std::string& planUID) {
    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUID, &plan) || plan == nullptr) {
        TPS_LOG_DEV_ERROR<<"The try does not exist, plan: "<<planUID;
        return false;
    }

    auto seriesUid = plan->get_imageseriesuid();
    bool planChanged = false;
    if (!SetPlanAsDataSource_i(groupType, planUID, seriesUid, planChanged)){
        TPS_LOG_DEV_ERROR<<"Failed to set plan as rendering data source.";
        return false;
    }

    // 这里将Plan下第一个Normgroup的第一个beam作为active beam，如果上述都存在的话
    std::vector<std::string> ngUids = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(planUID);
    bool hasNG = !ngUids.empty();
    std::vector<std::string> beamUids;
    if (hasNG){
        beamUids = mDataRepository->GetBeamManager()->GetBeamUIDListByNormgroupUID(ngUids[0]);
    }

    std::shared_ptr<HighlightBeamOperation> highlightBeamOp(
        new HighlightBeamOperation(beamUids.empty() ? "" : beamUids[0], 
        hasNG ? ngUids[0] : "", planUID, seriesUid)); 
    this->ProcessOperation(highlightBeamOp);

    std::shared_ptr<PlaceCameraFromBeamOperation> placeCamera(
        new PlaceCameraFromBeamOperation(planUID));
    this->ProcessOperation(placeCamera);

    if (planChanged)
    {
        std::shared_ptr<TpsRefreshDoseOperation> refreshDose(
            new TpsRefreshDoseOperation(planUID));
        this->ProcessOperation(refreshDose);

        std::shared_ptr<TpsShowDoseGridBoundaryOperation> showGridOp(
            new TpsShowDoseGridBoundaryOperation(planUID, false));
        this->ProcessOperation(showGridOp);
    }
    return true;
}

bool TpsRenderProxy::ActiveNormGroup(DATA_SOURCE_GROUP_ID groupType, const std::string& normalUID){
    //mDataRepository->GetNormgroupManager()->SetCurrentNormgroupUid(normalUID);
    RtNormgroup *normGroup = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(normalUID, &normGroup) && normGroup == nullptr) {
        TPS_LOG_DEV_ERROR << "The normal group[" << normalUID << "] dose not existed.";
        return false;
    }

    std::string planUid = normGroup->get_planuid();
    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUid, &plan) || plan == nullptr){
        TPS_LOG_DEV_ERROR<<"The try does not exist, plan: "<<planUid;
        return false;
    }
    auto seriesUid = plan->get_imageseriesuid();

    bool isPlanChanged = false;
    if (!SetPlanAsDataSource_i(groupType, planUid, seriesUid, isPlanChanged)){
        TPS_LOG_DEV_ERROR<<"Failed to set plan as rendering data source.";
        return false;
    }

    std::vector<std::string> beamUids = 
        mDataRepository->GetBeamManager()->GetBeamUIDListByNormgroupUID(normalUID);
    std::shared_ptr<HighlightBeamOperation> highlightBeamOp(
        new HighlightBeamOperation(beamUids.empty() ? "" : beamUids[0], 
        normalUID, planUid, seriesUid));
    this->ProcessOperation(highlightBeamOp);

    std::shared_ptr<PlaceCameraFromBeamOperation> placeCamera(
        new PlaceCameraFromBeamOperation(planUid));
    this->ProcessOperation(placeCamera);

    if (isPlanChanged)
    {
        std::shared_ptr<TpsRefreshDoseOperation> refreshDose(
            new TpsRefreshDoseOperation(normGroup->get_planuid()));
        this->ProcessOperation(refreshDose);

        std::shared_ptr<TpsShowDoseGridBoundaryOperation> showGridOp(
            new TpsShowDoseGridBoundaryOperation(planUid, false));
        this->ProcessOperation(showGridOp);
    }
    return true;
}

bool TpsRenderProxy::ActiveBeam(DATA_SOURCE_GROUP_ID groupType, const std::string& beamUID){
    RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(beamUID);
    if (beam == nullptr){
        TPS_LOG_DEV_ERROR<<"The beam does not exist, uid: "<<beamUID;
        return false;
    }
    std::string normalUID = beam->get_normgroupuid();

    RtNormgroup *normGroup = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(normalUID, &normGroup) && normGroup == nullptr) {
        TPS_LOG_DEV_ERROR << "The normal group[" << normalUID << "] dose not existed.";
        return false;
    }

    std::string planUid = normGroup->get_planuid();
    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUid, &plan) || plan == nullptr){
        TPS_LOG_DEV_ERROR<<"The try does not exist, plan: "<<planUid;
        return false;
    }
    
    auto seriesUid = plan->get_imageseriesuid();

    bool isPlanChanged = false;
    if (!SetPlanAsDataSource_i(groupType, planUid, seriesUid, isPlanChanged)){
        TPS_LOG_DEV_ERROR<<"Failed to set plan as rendering data source.";
        return false;
    }

    //mDataRepository->GetBeamManager()->SetCurrentBeamUid(beamUID);
    std::shared_ptr<HighlightBeamOperation> highlightBeamOp(
        new HighlightBeamOperation(beamUID, normalUID, planUid, seriesUid));
    this->ProcessOperation(highlightBeamOp);

    std::shared_ptr<PlaceCameraFromBeamOperation> placeCamera(
        new PlaceCameraFromBeamOperation(planUid));
    this->ProcessOperation(placeCamera);

    if (isPlanChanged)
    {
        std::shared_ptr<TpsRefreshDoseOperation> refreshDose(
            new TpsRefreshDoseOperation(normGroup->get_planuid()));
        this->ProcessOperation(refreshDose);

        std::shared_ptr<TpsShowDoseGridBoundaryOperation> showGridOp(
            new TpsShowDoseGridBoundaryOperation(planUid, false));
        this->ProcessOperation(showGridOp);
    }
    return true;
}

bool TpsRenderProxy::DeactiveCurrentPlan_i(int groupType) {
    auto group = RequestDataSourceGroup((DATA_SOURCE_GROUP_ID)groupType);
    if (group == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get common source group.";
        return false;
    }

    group->RemoveDataSource(PLAN);
    return true;
}

bool TpsRenderProxy::SetPlanAsDataSource_i(int groupType, const std::string& planUID, 
    const std::string& imageUID, bool& isPlanChanged)
{
    auto group = RequestDataSourceGroup((DATA_SOURCE_GROUP_ID)groupType);
    if (group == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get common source group.";
        return false;
    }

    std::string currSeriesUid;
    if (!group->GetDataSource(IMAGE, currSeriesUid)){
        TPS_LOG_DEV_ERROR<<"Failed to get image data source.";
        return false;
    }
    if (currSeriesUid != imageUID)
    {
        TPS_LOG_DEV_ERROR<<"The plan to be set doesn't belong to current image.";
        return false;
    }

    std::string prePlanUID("");
    group->GetDataSource(PLAN, prePlanUID);
    isPlanChanged = prePlanUID != planUID;
    if (isPlanChanged){
        group->SetDataSource(PLAN, planUID);
    }
    return true;
}

void TpsRenderProxy::SetEvaluationResult( const std::string &resultPlanUid, const std::string &resultImageUid )
{
	if (resultImageUid.empty() || resultPlanUid.empty()) {
		TPS_LOG_DEV_INFO<<"[EvaluationRenderProxy:]result plan UID:" << resultPlanUid 
			<< " and image UID:" <<resultImageUid;
		return;
	}

	this->SetDataSource(SOURCE_GROUP_EV_RESULT, IMAGE, resultImageUid);
	this->SetDataSource(SOURCE_GROUP_EV_RESULT, PLAN, resultPlanUid);
}


bool TpsRenderProxy::DoseHotSpots(const std::string& strPlanUID, const std::string& strVoiUid, float fDoseLimit, int iHotFlag) 
{
    std::shared_ptr<TpsHotSpotsOperation> hotSpotsOp(new TpsHotSpotsOperation(strPlanUID, strVoiUid, fDoseLimit, iHotFlag));
    this->ProcessOperation(hotSpotsOp);
    return true;
}

bool TpsRenderProxy::SetEasyPlanPixelSpace(float spacingX, float spacingY)
{
    auto renderController = this->GetRenderController(WINDOW_EZPLAN_IMAGE, "EZPLAN_IMAGE");

    auto easyPlanRenderController = dynamic_cast<TpsEasyPlanImageRenderController*>(renderController);
    easyPlanRenderController->SetPixelSpaceData(spacingX, spacingY);

    return true;
}

bool TpsRenderProxy::SetEasyPlanPixelSpace()
{
    auto renderController = this->GetRenderController(WINDOW_EZPLAN_IMAGE, "EZPLAN_IMAGE");
    auto mprRenderController = dynamic_cast<TpsEasyPlanImageRenderController*>(renderController);
    std::string currentPlan("");
    GetDataSource(SOURCE_GROUP_EZPLAN,PLAN, currentPlan);

    std::vector<RtBeam*> beamList = mDataRepository->GetBeamManager()->GetBeamsByPlan(currentPlan);
    auto itBeam = beamList.begin();
    for (; itBeam != beamList.end(); itBeam++)
    {
        RtBeam* beam = *itBeam;
        TpsImage2DEntity* image2DEntity = nullptr;
        bool iRet = mDataRepository->GetImageDataManager()->Get2DImageDataByBeamUID(beam->get_uid(), image2DEntity);
        //beam没有关联的drr图像是正常情况
        if (!iRet)
        {
            DISPLAY_SIZE displaySize;
            mprRenderController->GetDisplaySize(displaySize);
            mprRenderController->SetPixelSpaceData( EASY_PLAN_PIXEL_SPACING_X, EASY_PLAN_PIXEL_SPACING_Y);
        }
        else
        {
            mprRenderController->SetPixelSpaceData(image2DEntity->get()->get_firstimageplanepixelspacing(), image2DEntity->get()->get_secondimageplanepixelspacing());
        }
    }
    return true;
}

void TpsRenderProxy::DeleteISOCenter( const std::string &planUID, const std::string &poiUID )
{
	std::shared_ptr<TpsEasyPlanDeleteIsocenterOperation> deletePOIOp(new 
		TpsEasyPlanDeleteIsocenterOperation("",planUID, poiUID));
	this->ProcessOperation(deletePOIOp);
}

bool TpsRenderProxy::UpdateDrrSetting(std::string seriesUid, std::string normUid, float ctThresholdMin, float ctThresholdMax, std::string ct2densityUid, float electronEnergy)
{
    std::shared_ptr<UpdateDrrSettingOperation> ct2densityTableOperation(
        new UpdateDrrSettingOperation(seriesUid, normUid, ctThresholdMin, ctThresholdMax, ct2densityUid, electronEnergy));
    this->ProcessOperation(ct2densityTableOperation);
    return true;
}

bool TpsRenderProxy::OperateContourSlices(WINDOW_TYPE windowType, std::string windowUid, std::string vOIUID, int /*firstSlice*/, 
    int /*secondSlice*/, int /*editMode*/, int /*subEditMode*/) {
        std::string imageUID;
        if (!this->GetDataSource(windowType, IMAGE, imageUID)){
            TPS_LOG_DEV_ERROR<<"The image UID is empty.";
            return false;
        }

        auto renderControllerBase = this->GetRenderController(windowType, windowUid);
        auto renderController = dynamic_cast<TpsMPRRenderController*>(renderControllerBase);
        if(nullptr == renderController){
            TPS_LOG_DEV_ERROR<<"TpsContouringRenderProxy::UpdateVOIBySmartContour failed! \
                               As it's not TpsMPRRenderController!";
            return false;
        }
        //FIRST_POSTFIX_COMPONENT sectionType = renderController->GetSectionType();
        tps::DISPLAY_SIZE displaySize;
        renderController->GetDisplaySize(displaySize);
//         std::shared_ptr<TpsOperateContourSlicesOperation> cutContourOp(
//             new TpsOperateContourSlicesOperation(vOIUID, imageUID, 
//             sectionType, displaySize, firstSlice, secondSlice, editMode, subEditMode));
//         this->ProcessOperation(cutContourOp);
        return true;
}

bool TpsRenderProxy::SelectContourSlices(WINDOW_TYPE windowType, std::string windowUid, Mcsf::MedViewer3D::Point2D /*currentPoint*/,
    int /*actionType*/, std::string /*vOIUID*/,int /*firstSlice*/, int /*secondSlice*/)
{
    std::string imageUID;
    if (!this->GetDataSource(windowType, IMAGE, imageUID)){
        TPS_LOG_DEV_ERROR<<"The image UID is empty.";
        return false;
    }

    auto renderControllerBase = this->GetRenderController(windowType, windowUid);
    auto renderController = dynamic_cast<TpsMPRRenderController*>(renderControllerBase);
    if(nullptr == renderController){
        TPS_LOG_DEV_ERROR<<"TpsContouringRenderProxy::UpdateVOIBySmartContour failed! \
                           As it's not TpsMPRRenderController!";
        return false;
    }
    //FIRST_POSTFIX_COMPONENT sectionType = renderController->GetSectionType();
    tps::DISPLAY_SIZE displaySize;
    renderController->GetDisplaySize(displaySize);

//     std::shared_ptr<TpsSelectContourSlicesOperation> selectContourSlicesOp(
//         new TpsSelectContourSlicesOperation(currentPoint, actionType, 
//         vOIUID, imageUID, sectionType, displaySize, firstSlice, secondSlice));
//     this->ProcessOperation(selectContourSlicesOp);
    return true;
}

bool TpsRenderProxy::EasyPlanChangeSelectedBeam(const std::string &planUid, const std::string &normGroupUid,const std::string &beamUid )
{
	std::shared_ptr<TpsEasyPlanChangeBeamOperation> changeBeamOp(
		new TpsEasyPlanChangeBeamOperation(planUid,normGroupUid,beamUid));
	this->ProcessOperation(changeBeamOp);
	return true;
}

bool TpsRenderProxy::GetSliceDoseGird(RtDosegrid *&doseGrid, TPS_FIRST_POSTFIX_COMPONENT section, std::string planUid, std::string imageUid)
{
    //get dose graphic object
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(DOSE, section);
    std::string key = planUid + "|" + GOTypeHelper::ToString(goType);
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    auto go = mModelWarehouse->GetModelObject(key);
    auto doseGo = std::dynamic_pointer_cast<TpsMPRDoseGraphicObject>(go);
    if (doseGo == nullptr){
        TPS_LOG_DEV_ERROR<<"The mpr dose graphic object is empty.";
        return false;
    }
    float* buffer = nullptr;
    int width(0), height(0);
    Matrix4x4 matWorld2Plane;
    doseGo->GetDoseGrid(buffer, height, width, matWorld2Plane);
    if (width < 1){
        TPS_LOG_DEV_ERROR<<"the width of dose grid from go is :" << width;
        return false;
    }
    if (height < 1){
        TPS_LOG_DEV_ERROR<<"the height of dose grid from go is :" << height;
        return false;
    }
    //float* newbuffer = (float*)malloc(height * height * sizeof(float));
    //memcpy(newbuffer, buffer, height * height * sizeof(float));

    //get volume graphic object
    std::string volumeKey = imageUid + "|"+ GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    if(volume == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get volume go!"<<imageUid;
        return false;
    }
    Matrix4x4 matW2P = volume->GetWorld2Patient();
    Matrix4x4 matGrid2Patient = matW2P;
    matGrid2Patient.Append(matWorld2Plane.Inverse());
    matGrid2Patient.Append(Matrix4x4(
        1.0 / width, 0.0, 0.0, 0.0, 
        0.0, 1.0 / height, 0.0, 0.0, 
        0.0, 0.0, 1.0, 0.0, 
        0.5 / width, 0.5 / height, 0.0, 1.0));
    Mcsf::Matrix4f g2p = TpsArithmeticConverter::ConvertToMatrix4f(matGrid2Patient);

    Point3D start = matGrid2Patient.Transform(Point3D(0.0, 0.0, 0.0));
    Mcsf::Point3f startPoint = Mcsf::Point3f(start.x, start.y, start.z);

    double xIncre = matGrid2Patient.Transform(Vector3D(1.0, 0.0, 0.0)).Magnitude();
    double yIncre = matGrid2Patient.Transform(Vector3D(0.0, 1.0, 0.0)).Magnitude();
    double zIncre = matGrid2Patient.Transform(Vector3D(0.0, 0.0, 1.0)).Magnitude();
    Mcsf::Point3f incement = Mcsf::Point3f(xIncre, yIncre, zIncre); 

    RtDosegrid* plandoseGrid = mDataRepository->GetDoseManager()->GetDoseGrid(planUid, DOSE_GRID_PLAN);
    doseGrid = new RtDosegrid(*plandoseGrid);
    doseGrid->set_xcoordinate3dstart(startPoint.GetX());
    doseGrid->set_ycoordinate3dstart(startPoint.GetY());
    doseGrid->set_zcoordinate3dstart(startPoint.GetZ());
    doseGrid->set_xcoordinate3dinc(incement.GetX());
    doseGrid->set_ycoordinate3dinc(incement.GetY());
    doseGrid->set_zcoordinate3dinc(incement.GetZ());
    float grid2Patient[16];
    TpsArithmeticConverter::ConvertToFloat16(g2p, grid2Patient);
    doseGrid->set_grid_to_pat_t(grid2Patient);
    doseGrid->set_xcount(width);
    doseGrid->set_ycount(height);
    doseGrid->set_zcount(1);

    doseGrid->set_bitsstored(32);
    doseGrid->set_bitsallocated(32);
    doseGrid->set_highbits(31);
    float *newBuffer = doseGrid->create_dosegrid_buffer();
    memcpy(newBuffer, buffer, width * height * sizeof(float));
    return true;
}

bool TpsRenderProxy::GetMprCornerInfo(WINDOW_TYPE windowType, 
    const std::string& windowUid, 
    double normPosX, 
    double normPosY,
    int suvUnitType, 
    double& density, 
    double& value, 
    double& doseValue,
    double& windowcenter, //Top if PT
    double& windowwidth,  //Bottom if PT
    int& sliceIndex,
	int& startSliceNumber, 
    double& sliceThickness,
    double& positionx, 
    double& positiony, 
    double& positionz, 
    bool& isValueValid, 
    std::string& modality)
{
    std::string seriesUID;
    if (!this->GetDataSource(windowType, IMAGE, seriesUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window type: "
            <<windowType;
        return false;
    }

    //如果是4D CT四角信息应当取当前显示的series信息
    auto group4D = mDataRepository->GetImageDataManager()->GetSeriesGroup4d(seriesUID);
    if (group4D != nullptr)
    {
        seriesUID = group4D->m_strCurrentSeriesUid;
    }

    std::string planUID = "";
    this->GetDataSource(windowType, PLAN, planUID);

    //2. get section type
    auto renderController = this->GetRenderController(windowType, windowUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(renderController);
    FIRST_POSTFIX_COMPONENT section;
    if (mprRc != nullptr) {
        section = mprRc->GetSectionType();
    }
    else {
        TPS_LOG_DEV_ERROR<<"The window is not a mpr window.";
        return false;
    }

    std::shared_ptr<UpdateCornerInfoOperation> spUpdateCornerInfoOp(
        new UpdateCornerInfoOperation(normPosX, normPosY, planUID));
    std::string strVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, section);
    std::string strMprKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    spUpdateCornerInfoOp->SetSuvType(suvUnitType);
    spUpdateCornerInfoOp->SetSeriesUid(seriesUID);
    spUpdateCornerInfoOp->SetSectionType(section);
    this->ProcessOperation(spUpdateCornerInfoOp);

    // 读取获得的相关信息
    density = spUpdateCornerInfoOp->mDensity;
    value = spUpdateCornerInfoOp->mValue;
    doseValue = spUpdateCornerInfoOp->mDose;
    windowcenter = spUpdateCornerInfoOp->mWindowCenter;
    windowwidth = spUpdateCornerInfoOp->mWindowWidth;
    sliceIndex = spUpdateCornerInfoOp->mSliceIndex;
	startSliceNumber = spUpdateCornerInfoOp->mStartSliceNumber;
    positionx = spUpdateCornerInfoOp->mPositionX;
    positiony = spUpdateCornerInfoOp->mPositionY;
    positionz = spUpdateCornerInfoOp->mPositionZ;
    sliceThickness = spUpdateCornerInfoOp->mSliceThickness;
    isValueValid = spUpdateCornerInfoOp->mIsValueValid;
    modality = spUpdateCornerInfoOp->mModality;

	//z方向的sliceIndex可能不是从1开始，进行修正

    return true;
}

bool TpsRenderProxy::GetMprDynamicCornerInfo(WINDOW_TYPE windowType, 
    const std::string& windowUid, 
    double normPosX, 
    double normPosY,
    int suvUnitType, 
    double& density, 
    double& value, 
    double& doseValue,
    double& positionx, 
    double& positiony, 
    double& positionz, 
    bool& isValueValid)
{
    std::string seriesUID;
    if (!this->GetDataSource(windowType, IMAGE, seriesUID)) 
    {
        TPS_LOG_DEV_ERROR << "Can not get the image data source of current window type: "
            << windowType;
        return false;
    }

    std::string planUID = "";
    this->GetDataSource(windowType, PLAN, planUID);

    //2. get section type
    auto renderController = this->GetRenderController(windowType, windowUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(renderController);
    FIRST_POSTFIX_COMPONENT section;
    if (mprRc != nullptr) 
    {
        section = mprRc->GetSectionType();
    }
    else 
    {
        TPS_LOG_DEV_ERROR << "The window is not a mpr window.";
        return false;
    }

    std::shared_ptr<UpdateCornerInfoOperation> spUpdateCornerInfoOp(
        new UpdateCornerInfoOperation(
        normPosX, 
        normPosY, 
        planUID, 
        MPR_CORNER_TYPE(CORNER_TYPE_MOUSEPOSITION 
        | CORNER_TYPE_VALUE_DENSITY_DOSE | CORNER_TYPE_VALUEVALID))
        );

    std::string strVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, section);
    std::string strMprKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    spUpdateCornerInfoOp->SetSuvType(suvUnitType);
    spUpdateCornerInfoOp->SetSeriesUid(seriesUID);
    spUpdateCornerInfoOp->SetSectionType(section);
    this->ProcessOperation(spUpdateCornerInfoOp);

    // 读取获得的相关信息
    density = spUpdateCornerInfoOp->mDensity;
    value = spUpdateCornerInfoOp->mValue;
    doseValue = spUpdateCornerInfoOp->mDose;
    positionx = spUpdateCornerInfoOp->mPositionX;
    positiony = spUpdateCornerInfoOp->mPositionY;
    positionz = spUpdateCornerInfoOp->mPositionZ;
    isValueValid = spUpdateCornerInfoOp->mIsValueValid;
    return true;
}

bool TpsRenderProxy::GetMprSliceInfo(WINDOW_TYPE windowType, 
    const std::string& windowUid,
    int& sliceIndex,
	int& startSliceNumber, 
    int& totalSlices,
    double& sliceThickness,
    float& sliceLocation)
{
    std::string seriesUID;
    if (!this->GetDataSource(windowType, IMAGE, seriesUID)) 
    {
        TPS_LOG_DEV_ERROR << "Can not get the image data source of current window type: "
            << windowType;
        return false;
    }

    RtImage3DHeader* header;
    if (!mDataRepository->GetImageDataManager()->GetImage3DHeader(seriesUID, header))
    {
        TPS_LOG_DEV_ERROR << "Can not get the image Header of current series: "
            << seriesUID;
        return false;
    }

    std::string planUID = "";
    this->GetDataSource(windowType, PLAN, planUID);

    auto renderController = this->GetRenderController(windowType, windowUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(renderController);
    FIRST_POSTFIX_COMPONENT section;
    if (mprRc != nullptr) 
    {
        section = mprRc->GetSectionType();
    }
    else 
    {
        TPS_LOG_DEV_ERROR << "The window is not a mpr window.";
        return false;
    }

    std::shared_ptr<UpdateCornerInfoOperation> spUpdateCornerInfoOp(
        new UpdateCornerInfoOperation(0, 0, planUID, CORNER_TYPE_SLICEINDEX_THICKNESS));

    std::string strVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, section);
    std::string strMprKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    spUpdateCornerInfoOp->SetSeriesUid(seriesUID);
    spUpdateCornerInfoOp->SetSectionType(section);
    this->ProcessOperation(spUpdateCornerInfoOp);

    // 读取获得的相关信息
    sliceIndex = spUpdateCornerInfoOp->mSliceIndex;
	startSliceNumber = spUpdateCornerInfoOp->mStartSliceNumber;
    totalSlices = spUpdateCornerInfoOp->mTotalSlice;
    sliceThickness = spUpdateCornerInfoOp->mSliceThickness;
    if (header != nullptr && header->m_vScanInfoList.size() >= sliceIndex)
    {
        sliceLocation = header->m_vScanInfoList[sliceIndex - 1].m_SliceLocation;
    }
    return true;
}

bool TpsRenderProxy::GetMprWindowCenterWidth(
    WINDOW_TYPE windowType, 
    const std::string& windowUid,
    int suvUnitType,
    double& windowCenter, //Top if PT
    double& windowWidth,  //Bottom if PT
    bool& isValueValid)
{
    std::string seriesUID;
    if (!this->GetDataSource(windowType, IMAGE, seriesUID))
    {
        TPS_LOG_DEV_ERROR << "Can not get the image data source of current window type: "
            << windowType;
        return false;
    }

    std::string planUID = "";
    this->GetDataSource(windowType, PLAN, planUID);

    auto renderController = this->GetRenderController(windowType, windowUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(renderController);
    FIRST_POSTFIX_COMPONENT section;
    if (mprRc != nullptr) 
    {
        section = mprRc->GetSectionType();
    }
    else 
    {
        TPS_LOG_DEV_ERROR << "The window is not a mpr window.";
        return false;
    }

    // 窗宽窗位 不需要关心鼠标点的信息 直接传（0，0）
    std::shared_ptr<UpdateCornerInfoOperation> spUpdateCornerInfoOp(
        new UpdateCornerInfoOperation(0, 0, planUID, 
        MPR_CORNER_TYPE(CORNER_TYPE_WINDOWCENTERWIDTH | CORNER_TYPE_VALUEVALID))
        );

    std::string strVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, section);
    std::string strMprKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    spUpdateCornerInfoOp->SetSuvType(suvUnitType);
    spUpdateCornerInfoOp->SetSeriesUid(seriesUID);
    spUpdateCornerInfoOp->SetSectionType(section);
    this->ProcessOperation(spUpdateCornerInfoOp);

    windowCenter = spUpdateCornerInfoOp->mWindowCenter;
    windowWidth = spUpdateCornerInfoOp->mWindowWidth;
    isValueValid = spUpdateCornerInfoOp->mIsValueValid;

    return true;

}

//bool TpsRenderProxy::EasyPlanDrawMlcSmartAperture(const CertifiedMlcEditInfo& certMlc)
//{
//    std::shared_ptr<TpsEasyPlanMlcApertureOperation> drawMlcShapeOp(
//        new TpsEasyPlanMlcApertureOperation(certMlc));
//    this->ProcessOperation(drawMlcShapeOp);
//    this->Render();
//    return true;
//}

bool TpsRenderProxy::EasyPlanUpdateBlock(const CertifiedMlcEditInfo &certNudgeInfo)
{
    std::shared_ptr<UpdateBlockOperation> op(new UpdateBlockOperation(certNudgeInfo));
    this->ProcessOperation(op);
    this->Render();
    return true;
}

//bool TpsRenderProxy::EasyPlanShowNudgeCircle(double x, double y, double radius, const std::string &beamUID)
//{
//    std::shared_ptr<ShowNudgeCircleOperation> op(new ShowNudgeCircleOperation(x, y, radius, beamUID));
//    this->ProcessOperation(op);
//    this->Render();
//    return true;
//}

//bool TpsRenderProxy::EasyPlanMlcNudge(const CertifiedMlcEditInfo &certMlc)
//{
//    std::shared_ptr<TpsEasyPlanMlcNudgeOperation> mlcNudgeOp(
//        new TpsEasyPlanMlcNudgeOperation(certMlc));
//    this->ProcessOperation(mlcNudgeOp);
//    this->Render();
//    return true;
//}

//bool TpsRenderProxy::EasyPlanDragMlc(const CertifiedMlcEditInfo &certMlc)
//{
//    std::shared_ptr<TpsEasyPlanMlcDragOperation> mlcDragOp(new TpsEasyPlanMlcDragOperation(certMlc));
//    this->ProcessOperation(mlcDragOp);
//    this->Render();
//    return true;
//}

bool TpsRenderProxy::EasyPlanGetCurrentMlcData(const std::string& planUID, plunc::LEAF_POSITION_STC*& leafData, int& leafNum) 
{
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    auto go = mModelWarehouse->GetModelObject(
        planUID + "|" + GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    std::shared_ptr<NormalGroupCollection> ngCollection = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    if (ngCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get normal group collection of try: "<<planUID;
        return false;
    }

    auto beamGoCollection = ngCollection->GetActiveNormalGroup();
    if (beamGoCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get active beam collection of plan: "<<planUID;
        return false;
    }

    auto beamGO = beamGoCollection->GetActiveBeamGo();
    if (beamGO == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get active beam of plan: "<<planUID;
        return false;
    }

    auto segmentGo = beamGO->GetActiveSegmentGo();
    if (segmentGo == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get active segment graphic object";
        return false;
    }
    tps::StcMLC* mlc = segmentGo->GetMLC();
    if (mlc == nullptr){
        TPS_LOG_DEV_ERROR<<"There is no mlc shape in segment graphic object.";
        return false;
    }

    leafData = mlc->m_pLeaf;
    leafNum = mlc->m_nLeafNum;
    return true;
}

bool TpsRenderProxy::EasyPlanGetCurrentBlockData(const std::string& planUID, std::vector<Mcsf::MedViewer3D::Point2D>& vecData) 
{
    vecData.clear();

    //std::string planUID = TpsImageDataManager::GetInstance()->GetCurrentEasyPlanUID();

    auto ngCollection = mModelWarehouse->GetModelObject(
        planUID + "|" + GOTypeHelper::ToString(GO_TYPE_NORMALGROUP_COLLECTION));
    auto goNormGroupCollection = std::dynamic_pointer_cast<NormalGroupCollection>(ngCollection);
    if (goNormGroupCollection == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed get normgroup collection.";
        return false;
    }

    auto beamGoCollection = goNormGroupCollection->GetActiveNormalGroup();
    if (beamGoCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get active beam collection of plan: "<<planUID;
        return false;
    }

    auto beamGO = beamGoCollection->GetActiveBeamGo();
    if (beamGO == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed get active beam of plan: "<<planUID;
        return false;
    }

    auto blockGo = beamGO->GetApertureBlock();
    if (blockGo == nullptr){
        TPS_LOG_DEV_ERROR<<"There is no block in beam graphic object.";
        return false;
    }

    std::vector<Mcsf::MedViewer3D::Point2D>* pData = nullptr;
    blockGo->GetBlockData(pData);
    if (pData == nullptr){
        TPS_LOG_DEV_ERROR<<"There is no block data in block graphic object.";
        return false;
    }
    vecData = *pData;
    //beamUID = beamGO->GetUID();
    return true;
}

void TpsRenderProxy::EasyPlanSetISOCenter( const std::string &planUID, const std::string &poiUID, 
    const std::string& currentImageUID, const float& currentIsoInImgX, const float& currentIsoInImgY, 
    const float& previousIsoInPlanImgCoorX, const float& previousIsoInPlanImgCoorY,
    const float& viewerWidth, const float& viewerHeight, const int& imageWidth, const int& imageHeigh,
    const float& firstImagePlanePixelSpacing, const float& secondImagePlanePixelSpacing)
{
    std::shared_ptr<TpsEasyPlanSetIsocenterOperation> addPoiOp(
        new TpsEasyPlanSetIsocenterOperation(planUID,poiUID,currentImageUID, currentIsoInImgX, currentIsoInImgY,
            previousIsoInPlanImgCoorX, previousIsoInPlanImgCoorY,
            viewerWidth, viewerHeight, imageWidth, imageHeigh, firstImagePlanePixelSpacing, secondImagePlanePixelSpacing));
    this->ProcessOperation(addPoiOp);
    this->Render();
}

void TpsRenderProxy::EasyPlanSetNormPoint( const std::string &planUID, const std::string &poiUID, const std::string &imageUid)
{
    DISPLAY_SIZE disPlaySize;
    this->GetDisplaySize((WINDOW_TYPE)0x0a00,"EZPLAN_IMAGE",disPlaySize);
    std::shared_ptr<TpsEasyPlanSetNormPointOperation> addPoiOp(
        new TpsEasyPlanSetNormPointOperation(planUID,poiUID, imageUid, disPlaySize.width, disPlaySize.height));
    this->ProcessOperation(addPoiOp);
}

bool TpsRenderProxy::EasyPlanZoomImage( WINDOW_TYPE wndType, const std::string& wndUid, const std::string& beamUid, float scaleFactor )
{
    wndUid;

    std::string planUID;
    if (!this->GetDataSource((WINDOW_TYPE)wndType, PLAN, planUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
        return false;
    }

    TpsImage2DEntity *image2DData = nullptr;
    bool iRet = mDataRepository->GetImageDataManager()->Get2DImageDataByBeamUID(beamUid, image2DData);
    //beam没有关联的drr
    if (!iRet) return true;
    std::string imageUid;
    image2DData->GetUID(imageUid);

    std::shared_ptr<TpsEasyPlanSetZoomOperation> setEasyplanZoomOp(
        new TpsEasyPlanSetZoomOperation(imageUid, planUID, scaleFactor));

    this->ProcessOperation(setEasyplanZoomOp);
    this->Render();
    return true;
}

bool TpsRenderProxy::EasyPlanRotateImage( WINDOW_TYPE wndType, const std::string& wndUid, const std::string& beamUid, float startX, float startY, float stopX, float stopY )
{
    wndUid;

    std::string planUID;
    if (!this->GetDataSource((WINDOW_TYPE)wndType, PLAN, planUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
        return false;
    }

    TpsImage2DEntity *image2DData = nullptr;
    int iRet = mDataRepository->GetImageDataManager()->Get2DImageDataByBeamUID(beamUid, image2DData);
    //beam没有关联的drr
    if (!iRet) return true;
    std::string imageUid;
    image2DData->GetUID(imageUid);

    std::shared_ptr<TpsEasyPlanSetRotationOperation> setEasyplanRotationOp(
        new TpsEasyPlanSetRotationOperation(imageUid, planUID, startX - 0.5f, startY - 0.5f, stopX - 0.5f, stopY - 0.5f));

    this->ProcessOperation(setEasyplanRotationOp);
    this->Render();
    return true;
}

bool TpsRenderProxy::EasyPlanSetPanImage( int windowType, std::string& windowUid, std::string &planUid, std::string &beamUid,
    float startX, float startY, float stopX, float stopY, double viewerWidth, double viewerHeight )
{
    windowUid;
    windowType;
    TpsImage2DEntity *image2DData = nullptr;
    int iRet = mDataRepository->GetImageDataManager()->Get2DImageDataByBeamUID(beamUid, image2DData);
    //beam没有关联的drr
    if (!iRet) return true;
    std::string imageUid;
    image2DData->GetUID(imageUid);

    std::shared_ptr<TpsEasyPlanSetPanOperation> setEasyplanPanOp(
        new TpsEasyPlanSetPanOperation( planUid, imageUid, startX, startY, stopX, stopY, viewerWidth, viewerHeight));

    this->ProcessOperation(setEasyplanPanOp);
    this->Render();
    return true;
}

bool TpsRenderProxy::EasyPlanWindowingImage( int windowType, std::string &windowUid, std::string &beamUid, int windowWidth, int windowCenter, int& currentWinWidth, int& currentWinCenter)
{
    windowUid;

    std::string planUID;
    if (!this->GetDataSource((WINDOW_TYPE)windowType, PLAN, planUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
        return false;
    }

    TpsImage2DEntity *image2DData = nullptr;
    int iRet = mDataRepository->GetImageDataManager()->Get2DImageDataByBeamUID(beamUid, image2DData);
    //beam没有关联的drr
    if (!iRet) return true;
    std::string imageUid;
    image2DData->GetUID(imageUid);

	std::shared_ptr<TpsEasyPlanSetWindowingOperation> setEasyplanWindowingOp(
		new TpsEasyPlanSetWindowingOperation(planUID, imageUid,windowWidth,windowCenter));
	this->ProcessOperation(setEasyplanWindowingOp);
    setEasyplanWindowingOp->GetCurrentWinWidthAndWinCenter(currentWinWidth, currentWinCenter);
	this->Render();
	return true;
}

bool TpsRenderProxy::EasyPlanMesureDistance(WINDOW_TYPE windowType, std::string &windowUid, std::string &beamUid, float startX, float startY,
    float stopX, float stopY, bool actionStarted, bool actionDone )
{
    windowUid;

    std::string planUID;
    if (!this->GetDataSource((WINDOW_TYPE)windowType, PLAN, planUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
        return false;
    }

    TpsImage2DEntity *image2DData = nullptr;
    int iRet = mDataRepository->GetImageDataManager()->Get2DImageDataByBeamUID(beamUid, image2DData);
    //beam没有关联的drr
    if (!iRet) return true;
    std::string imageUid;
    image2DData->GetUID(imageUid);

    std::shared_ptr<TpsEasyPlanMeasureDistanceOperation> setEasyplanMeasureDistOp(
        new TpsEasyPlanMeasureDistanceOperation(planUID, imageUid, startX, startY, stopX, stopY, actionStarted, actionDone));
    this->ProcessOperation(setEasyplanMeasureDistOp);

    this->Render();
    return true;
}

bool TpsRenderProxy::EasyPlanResetImage( int windowType, std::string & windowUid, std::string & beamUid )
{
	windowUid;
	std::string planUid;
	if (!this->GetDataSource((WINDOW_TYPE)windowType, PLAN, planUid)) {
		TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
		return false;
	}

    TpsImage2DEntity *image2DData = nullptr;
    int iRet = mDataRepository->GetImageDataManager()->Get2DImageDataByBeamUID(beamUid, image2DData);
    //beam没有关联的drr
    if (!iRet) return true;
    std::string imageUid;
    image2DData->GetUID(imageUid);

    DISPLAY_SIZE displaySize;
    if(!this->GetDisplaySize((WINDOW_TYPE)windowType, windowUid, displaySize)) {
        TPS_LOG_DEV_ERROR<<"Failed to get easy plan display size!";
        return false;
    }

	std::shared_ptr<TpsEasyPlanResetImageOperation> setEasyplanImageResetOp(
		new TpsEasyPlanResetImageOperation(imageUid, planUid, displaySize.width, displaySize.height));

	this->ProcessOperation(setEasyplanImageResetOp);
	this->Render();
	return true;
}

bool TpsRenderProxy::EasyPlanSetInverse(int wndType, const std::string& wndUid, const std::string& beamUid)
{
    wndUid;

    std::string planUID;
    if (!this->GetDataSource((WINDOW_TYPE)wndType, PLAN, planUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
        return false;
    }

    TpsImage2DEntity *image2DData = nullptr;
    int iRet = mDataRepository->GetImageDataManager()->Get2DImageDataByBeamUID(beamUid, image2DData);
    //beam没有关联的drr
    if (!iRet) return true;
    std::string imageUid;
    image2DData->GetUID(imageUid);

    std::shared_ptr<TpsEasyPlanSetInverseOperation> setEasyplanInverseOp(
        new TpsEasyPlanSetInverseOperation(imageUid, planUID));
    this->ProcessOperation(setEasyplanInverseOp);
    this->Render();
    return true;
}

bool TpsRenderProxy::EasyPlanMesureAngle(int windowType, const std::string& windowUid, const std::string& beamUid, 
    const std::list<Mcsf::MedViewer3D::Point2D>& points, float cursorX, float cursorY,
    bool actionStarted, bool firstSideDone, bool secondSideDone )
{
    windowUid;

    std::string planUID;
    if (!this->GetDataSource((WINDOW_TYPE)windowType, PLAN, planUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
        return false;
    }

    TpsImage2DEntity *image2DData = nullptr;
    int iRet = mDataRepository->GetImageDataManager()->Get2DImageDataByBeamUID(beamUid, image2DData);
    //beam没有关联的drr
    if (!iRet) return true;
    std::string imageUid;
    image2DData->GetUID(imageUid);

    std::shared_ptr<TpsEasyPlanMeasureAngleOperation> setEasyplanMeasureAngleOp(
        new TpsEasyPlanMeasureAngleOperation(planUID, imageUid, points, cursorX, cursorY,
        actionStarted, firstSideDone, secondSideDone));

    this->ProcessOperation(setEasyplanMeasureAngleOp);
    this->Render();
    return true;
}

bool TpsRenderProxy::DeleteDrrImage(const std::string& beamUid, const std::string imageUID) 
{
    if(beamUid.empty()) {
        TPS_LOG_DEV_ERROR<<"the beam uid is empty!";
        return false;
    }
    std::shared_ptr<TpsDeleteDrrImageOperation> deleteImageOp(
        new TpsDeleteDrrImageOperation(beamUid, imageUID));
    this->ProcessOperation(deleteImageOp);
    return true;
}


bool TpsRenderProxy::EasyPlanLoadImage( int windowType, const std::string& windowUid )
{
    std::string planUid;
    if (!this->GetDataSource((WINDOW_TYPE)windowType, PLAN, planUid)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
        return false;
    }

    DISPLAY_SIZE displaySize;
    if(!this->GetDisplaySize((WINDOW_TYPE)windowType, windowUid, displaySize)) {
        TPS_LOG_DEV_ERROR<<"Failed to get easy plan display size!";
        return false;
    }

    std::shared_ptr<TpsEasyPlanLoadImageOperation> easyplanLoadImageOp(
        new TpsEasyPlanLoadImageOperation(planUid, displaySize.width, displaySize.height));

    this->ProcessOperation(easyplanLoadImageOp);
    this->Render();
    return true;
}


bool TpsRenderProxy::GetPixelSpacingIn10MMOnBEV(WINDOW_TYPE windowType, const std::string& windowUid,
    double& spacingx, double& spacingy)
{
    auto renderController = this->GetRenderController(windowType, windowUid);
    return renderController->GetCurrentPixelSpacingIn10MM(spacingx, spacingy);
}

bool TpsRenderProxy::GetBevViewDirecrion(WINDOW_TYPE windowType, const std::string& windowUid, 
    double& x1, double& y1, double& z1, double& x2, double& y2, double& z2) {
        auto renderController = this->GetRenderController(windowType, windowUid);
        if (renderController == nullptr) {
            TPS_LOG_DEV_ERROR << "Render controller of window: " << windowUid
                << "does not exists.";
            return false;
        }

        auto bevRenderController = dynamic_cast<TpsBEVRenderController*>(renderController);
        if (bevRenderController == nullptr) {
            TPS_LOG_DEV_ERROR << "Render controller is not bev";
            return false;
        }

        Mcsf::MedViewer3D::Vector3D vecView, vecUp;
        if (!bevRenderController->GetBevViewDirecrion(vecView, vecUp)) {
            TPS_LOG_DEV_ERROR<<"GetbevViewDirecrion failed!";
            return false;
        } 
        else {
            x1 = vecView.x;
            y1 = vecView.y;
            z1 = vecView.z;
            x2 = vecUp.x;
            y2 = vecUp.y;
            z2 = vecUp.z;
        }

        return true;
}

void TpsRenderProxy::EasyPlanModifyBeamInfo(const std::string &beamUID)
{
	std::shared_ptr<TpsEasyPlanModifyBeamOperation> updateBeamOp(new TpsEasyPlanModifyBeamOperation(beamUID));
    std::string imageUID;
    if (this->GetDataSource(SOURCE_GROUP_EZPLAN, IMAGE, imageUID)){
        updateBeamOp->SetDrrImageUID(imageUID); 
    }
	this->ProcessOperation(updateBeamOp);
	this->Render();
}

bool TpsRenderProxy::VoiRedoUndo(std::shared_ptr<TpsVOIEditingCache> cache){

//     std::shared_ptr<TpsVOIEditingRedoUndoOperation> op(
//         new TpsVOIEditingRedoUndoOperation(cache));
//     return this->ProcessOperation(op);
    return true;
}

void TpsRenderProxy::PreviewBeamTemplate(std::string beamTemplateUid, std::string normGroupUid, std::string isocenterUid) {

    std::shared_ptr<UpdateBeamTemplatePreviewOperation> updateBeamTemplatePreviewOperation(
        new UpdateBeamTemplatePreviewOperation(beamTemplateUid, normGroupUid, isocenterUid));

    this->ProcessOperation(updateBeamTemplatePreviewOperation);
    this->Render();
}

void TpsRenderProxy::UpdateSegment(const std::string& normGroupUID, const std::string& beamUID)
{
    std::shared_ptr<TpsUpdateSegmentOperation> op(
        new TpsUpdateSegmentOperation(normGroupUID, beamUID));
    this->ProcessOperation(op);
}

void TpsRenderProxy::HideMprNonImage(WINDOW_TYPE windowType, const std::string& windowUid, bool isToHide)
{
    DATA_SOURCE_UID sourceUid;
    if (!this->GetDataSource(windowType, IMAGE, sourceUid)){
        TPS_LOG_DEV_ERROR<<"Failed to get data source uid using window type: "<<windowType;
        return;
    }

    auto groups = this->SelectGroups(
        [sourceUid](DATA_SOURCE_GROUP_ID /*id*/, TpsDataSourceGroup* group) -> bool
    {
        std::string uid;
        return (group->GetDataSource(IMAGE, uid) && uid == sourceUid);
    });
    for (int i = 0; i < groups.size(); ++i){
        if (groups[i] == nullptr) continue;
        groups[i]->ForEach([isToHide, windowUid](const std::string& uid, TpsRenderControllerBase* rc)
        {
            if (windowUid == uid) return;
            auto mprRc = dynamic_cast<TpsMPRRenderController*>(rc);
            if (mprRc == nullptr) return;
            mprRc->HideNonImageOverlay(isToHide);
        });
    }
}

void TpsRenderProxy::CircleSimple_i(int xCenter, int yCenter, int radius,
    std::vector<Mcsf::MedViewer3D::Point2D>& vecCirclePts)
{
    int x, y;
    float r2 = radius * radius;
    int nSize = (2*radius+1)*2;
    vecCirclePts.reserve(nSize);
    for (x = -radius; x <= radius; x++) {
        y = (int)(std::sqrt(float(r2 - x*x)) + 0.5);
        vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(xCenter + x, yCenter + y));
    }
    for (x=radius; x>=-radius; x--) {
        y = (int)(std::sqrt(float(r2 - x*x)) + 0.5);
        vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(xCenter + x, yCenter - y));
    }
}

// 这里修改一下算法：
// 1. 首先，只画圆的上半部分，根据原算法原理，画完之后，点集中的点并不是按沿弧顺时针方向
//    排列的
// 2. 基于点的x坐标排序，将点集理顺为沿弧顺时针排列
// 3. 以x轴做垂直镜像，反算出另一半的点集，合并出整圆的所有点集
void TpsRenderProxy::CirclePoints_i(int cx, int cy, int x, int y,
    std::vector<Mcsf::MedViewer3D::Point2D>& vecCirclePts)
{
    if (x == 0) {
        vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx, cy+y));
        //vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx, cy-y));
        vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx+y, cy));
        vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx-y, cy));
    } else if (x == y) {
        vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx + x, cy + y));
        vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx - x, cy + y));
        //vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx + x, cy - y));// 下半圆的点都不画了
        //vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx - x, cy - y));
    } else if (x < y) {
        vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx + x, cy + y));
        vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx - x, cy + y));
        //vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx + x, cy - y));
        //vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx - x, cy - y));
        vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx + y, cy + x));
        vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx - y, cy + x));
        vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx + y, cy - x));
        vecCirclePts.push_back(Mcsf::MedViewer3D::Point2D(cx - y, cy - x));
    }
}


void TpsRenderProxy::CircleMidpoint_i(int xCenter, int yCenter, int radius,
    std::vector<Mcsf::MedViewer3D::Point2D>& vecCirclePts)
{
    int x = 0;
    int y = radius;
    int p = (5 - radius*4)/4;

    CirclePoints_i(xCenter, yCenter, x, y, vecCirclePts);
    while (x < y) {
        x++;
        if (p < 0) {
            p += 2*x+1;
        } else {
            y--;
            p += 2*(x-y)+1;
        }
        CirclePoints_i(xCenter, yCenter, x, y, vecCirclePts);
    }
}

bool compare(const Mcsf::MedViewer3D::Point2D& ptA, 
    const Mcsf::MedViewer3D::Point2D& ptB)
{
    return ptA.x<ptB.x;
}

void TpsRenderProxy::RearrangeCirclePoints_i(std::vector<Mcsf::MedViewer3D::Point2D>& vecCirclePts)
{
    if (vecCirclePts.size()==0) {
        return;
    }
    int nPtSize = vecCirclePts.size();
    std::vector<int> vecIndex;
    vecIndex.reserve(nPtSize);
 
    //// 拆分出点集的x分量
    double* pointsX = new double[nPtSize];
    for (int n=0; n<nPtSize; n++) {
        pointsX[n] = vecCirclePts[n].x;
    }

    //// 按从小到大排序
    std::iota(std::begin(vecIndex), std::end(vecIndex), 0); // fill index with {0,1,2,...} This only needs to happen once
    std::sort(std::begin(vecCirclePts), std::end(vecCirclePts), 
        std::bind(compare, std::placeholders::_1, std::placeholders::_2));

    // 释放内存
    delete[] pointsX;
    return;
}

void TpsRenderProxy::EnableArcEditModeOnMprPlane(
    const std::string& planUID, const std::string& ngUID, 
    const std::string& beamUID, bool isEnabled)
{
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    std::string key = planUID + "|" + GOTypeHelper::ToString(
        GO_TYPE_NORMALGROUP_COLLECTION);
    auto go = mModelWarehouse->GetGraphicObject(key);
    auto planGo = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    if (planGo == nullptr) return;

    auto ngGo = planGo->GetNormalGroup(ngUID);
    if (ngGo == nullptr) return;
    auto beamGo = ngGo->SelectBeam(beamUID);
    if (beamGo == nullptr) return;
    beamGo->SetArcEditModeEnabled(isEnabled);
}

void TpsRenderProxy::SetArcEditCache(
    const std::string& planUID, 
    const std::string& ngUID, 
    const std::string& beamUID, 
    bool isClockwise, bool isSimpleArc, bool isToRotateStart,
    double minAngle, double maxAngle,
    double cwRange, double ccwRange,
    double startAngle, double arcLength, 
    const Mcsf::MedViewer3D::Matrix4x4& matStartGantry2Patient, 
    const Mcsf::MedViewer3D::Matrix4x4& matEndGantry2Patient, 
    const Mcsf::MedViewer3D::Matrix4x4& matPatient2OrgGantry)
{
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    std::string key = planUID + "|" + GOTypeHelper::ToString(
        GO_TYPE_NORMALGROUP_COLLECTION);
    auto go = mModelWarehouse->GetGraphicObject(key);
    auto planGo = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    if (planGo == nullptr) return;

    auto ngGo = planGo->GetNormalGroup(ngUID);
    if (ngGo == nullptr) return;
    auto beamGo = ngGo->SelectBeam(beamUID);
    if (beamGo == nullptr) return;
    ArcBeamEditCache& arcCache = beamGo->GetArcEditCache();
    arcCache._startGantry = startAngle;
    arcCache._arcLength = arcLength;
    arcCache._isClockwise = isClockwise;
    arcCache._isSimpleArc = isSimpleArc;
    arcCache._isToRotateStart = isToRotateStart;
    arcCache._minAngle = minAngle;
    arcCache._maxAngle = maxAngle;
    arcCache._cwRange = cwRange;
    arcCache._ccwRange = ccwRange;
    arcCache._matStartGantry2Patient = matStartGantry2Patient;
    arcCache._matEndGantry2Patient = matEndGantry2Patient;
    arcCache._matPatient2OrgGantry = matPatient2OrgGantry;
    arcCache._isEditingInfoInit = false;
    arcCache._isOnEdge = false;
    beamGo->SetDirty(true);
}

void TpsRenderProxy::EditArcBeamCache(
    WINDOW_TYPE windowType, 
    const std::string& windowUid,
    double preX, double preY, 
    double currX, double currY, 
    const std::string& planUid,
    const std::string& ngUid, 
    const std::string& beamUid)
{
    auto renderControllerBase = this->GetRenderController(windowType, windowUid);
    auto renderController = dynamic_cast<TpsMPRRenderController*>(renderControllerBase);
    if(nullptr == renderController){
        TPS_LOG_DEV_ERROR<<"TpsRenderProxy::EditArcBeamCache failed! \
                           As it's not TpsMPRRenderController!";
        return;
    }
    FIRST_POSTFIX_COMPONENT sectionType = renderController->GetSectionType();
    DISPLAY_SIZE displaySize;
    renderController->GetDisplaySize(displaySize);

    std::string imageUID, planUID;
    if (!this->GetDataSource(windowType, IMAGE, imageUID)){
        TPS_LOG_DEV_ERROR<<"Failed to get IMAGE data source.";
        return;
    }

    std::shared_ptr<EditArcBeamCacheOperation> op(
        new EditArcBeamCacheOperation(imageUID, planUid, ngUid, beamUid, 
        sectionType, preX, preY, currX, currY, displaySize));
    this->ProcessOperation(op);
}

bool TpsRenderProxy::GetArcGantryAnglesFromCache(
    const std::string& planUID, 
    const std::string& ngUID, 
    const std::string& beamUID, 
    double& start, double& arcLength)
{
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    std::string key = planUID + "|" + GOTypeHelper::ToString(
        GO_TYPE_NORMALGROUP_COLLECTION);
    auto go = mModelWarehouse->GetGraphicObject(key);
    auto planGo = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    if (planGo == nullptr) return false;

    auto ngGo = planGo->GetNormalGroup(ngUID);
    if (ngGo == nullptr) return false;
    auto beamGo = ngGo->SelectBeam(beamUID);
    if (beamGo == nullptr) return false;
    ArcBeamEditCache& arcCache = beamGo->GetArcEditCache();
    start = arcCache._startGantry;
    arcLength = arcCache._arcLength;
    return true;
}

bool TpsRenderProxy::GetNormalOfMPR(
    WINDOW_TYPE windowType, const std::string& windowUid, Mcsf::Vector3f& normal)
{
    auto rc = this->GetRenderController(windowType, windowUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(rc);
    if (mprRc == nullptr){
        TPS_LOG_DEV_ERROR<<"The window is not mpr window.";
        return false;
    }

    Mcsf::MedViewer3D::Vector3D vecNormal;
    if (!mprRc->GetNormal(vecNormal)){
        TPS_LOG_DEV_ERROR<<"Failed to execute GetNormal.";
        return false;
    }
    normal = TpsArithmeticConverter::ConvertToVector3f(vecNormal);
    return true;
}

//Gallery Part
int TpsRenderProxy::GetCrosshairMode(std::string seriesUID, int& iCrosshairMode)
{
    int ucSectionType = AXIAL;

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, ucSectionType);
    std::string strKey = seriesUID + "|" + GOTypeHelper::ToString(goType);

    auto go = mModelWarehouse->GetModelObject(strKey);

    auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);

    //down cast
    if (spCrosshairGO == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsContouringRenderProxy::UpdateCrosshair]: "
            <<"Failed to dynamic cast IGraphicObjects.";
        return -1;
    }
    CrosshairGraphicObject::CROSSHAIR_MODE crosshairMode;
    spCrosshairGO->GetCrosshairMode(&crosshairMode);
    iCrosshairMode = crosshairMode;

    return 1;
}

bool TpsRenderProxy::RemoveGraphicObjects()
{
    this->RemoveDataSource(SOURCE_GROUP_GALLERY, IMAGE);  // Set Strategy's GO nullptr

    WINDOW_TYPE windowTypes[3] = {WINDOW_GALLERY_MPR_AXIAL, WINDOW_GALLERY_MPR_SAGITTAL, WINDOW_GALLERY_MPR_CORONAL};
    std::string windowUids[3] = {"GALLERY_MPR_AXIAL", "GALLERY_MPR_SAGITTAL", "GALLERY_MPR_CORONAL"};
    std::vector<std::string> vSeriesUIDs;
    mDataRepository->GetGalleryImageManager()->GetSeriesUIDList(vSeriesUIDs);
    for(int i=0; i<3; ++i){
        auto renderController = this->GetRenderController(windowTypes[i], windowUids[i]);
        auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
        if (mprRc != nullptr){     
            for (size_t j = 0; j< vSeriesUIDs.size(); ++j){
                mprRc->RemoveGraphicObjects(vSeriesUIDs[j]);
            }
        }
    }

    auto renderController = this->GetRenderController(WINDOW_GALLERY_3D, "GALLERY_VR");
    auto vrRc = dynamic_cast<TpsGalleryShadedSurfaceRenderController*>(renderController);
    if (vrRc != nullptr){    
        for (size_t j = 0; j< vSeriesUIDs.size(); ++j){
            vrRc->RemoveGraphicObjects(vSeriesUIDs[j]);
        }
    }

    return true;
}

bool TpsRenderProxy::RemoveGraphicObjects(std::string seriesUID)
{
    // TODO Check Is CurrentSeriesUID ?
    this->RemoveDataSource(SOURCE_GROUP_GALLERY, IMAGE);  
    this->Render();

    WINDOW_TYPE windowTypes[3] = {WINDOW_GALLERY_MPR_AXIAL, WINDOW_GALLERY_MPR_SAGITTAL, WINDOW_GALLERY_MPR_CORONAL};
    std::string windowUids[3] = {"GALLERY_MPR_AXIAL", "GALLERY_MPR_SAGITTAL", "GALLERY_MPR_CORONAL"};

    for(int i=0; i<3; ++i){
        auto renderController = this->GetRenderController(windowTypes[i], windowUids[i]);
        auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
        if (mprRc != nullptr){     
            mprRc->RemoveGraphicObjects(seriesUID);
        }
    }

    auto renderController = this->GetRenderController(WINDOW_GALLERY_3D, "GALLERY_VR");
    auto vrRc = dynamic_cast<TpsGalleryShadedSurfaceRenderController*>(renderController);
    if (vrRc != nullptr){     
        vrRc->RemoveGraphicObjects(seriesUID);
    }

    return true;
}

bool TpsRenderProxy::GetCurrentMPRSlice(WINDOW_TYPE strWidowType, const std::string& wndUid, int& slice, int& startSliceNumber, int& totalSlice)
{
    auto renderController = this->GetRenderController(strWidowType, wndUid);
    if (nullptr == renderController){
        return false;
    }
    auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
    if (mprRc == nullptr){     
        return false;
    }
    return mprRc->GetCurrentMprSlice(slice, startSliceNumber, totalSlice);
}

bool TpsRenderProxy::GalleryMprPageTurn(WINDOW_TYPE wndType, const std::string& wndUid, int iPageturn)
{
    std::string seriesUID;
    if (!this->GetDataSource(wndType, IMAGE, seriesUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
        return false;
    }

    auto renderController = this->GetRenderController(wndType, wndUid);
    auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
    if (mprRc == nullptr){
        TPS_LOG_DEV_ERROR<<"The window is not a mpr window.";
        return false;
    }
    FIRST_POSTFIX_COMPONENT section = mprRc->GetSectionType();

    this->GalleryMprPageTurn_i(seriesUID, section, iPageturn);

    //// 4. execute the MPR scrolling command
    //std::shared_ptr<TpsGalleryMPRPageTurnOperation> 
    //    pageTurnoperation(new TpsGalleryMPRPageTurnOperation(iPageturn));

    //std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);

    //auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section);
    //std::string sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);

    //goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR_CORNER_INFO, section);
    //std::string sMPRCornerInfoKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    //goType = GOTypeHelper::ComposeAsGOType(GALLERY_MEASURE, section);
    //std::string sMeasureKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    //goType = GOTypeHelper::ComposeAsGOType(GALLERY_TEXT, section);
    //std::string sTextKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    //pageTurnoperation->SetGraphicObjectKeys(sVolumeKey, sMPRKey, sMPRCornerInfoKey, sMeasureKey, sTextKey);
    //this->ProcessOperation(pageTurnoperation);

    return true;
}

void TpsRenderProxy::GalleryMprPageTurn_i(const std::string& imageUid, 
    FIRST_POSTFIX_COMPONENT section, int steps)
{
    std::shared_ptr<TpsGalleryMPRPageTurnOperation> pageTurnoperation(
        new TpsGalleryMPRPageTurnOperation(steps, imageUid, section));
    this->ProcessOperation(pageTurnoperation);
}

//bool TpsRenderProxy::GalleryMprPageTurn(WINDOW_TYPE wndType, const std::string& wndUid, int iPageturn)
//{
//    std::string seriesUID;
//    if (!this->GetDataSource(wndType, IMAGE, seriesUID)) {
//        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
//        return false;
//    }
//
//    auto renderController = this->GetRenderController(wndType, wndUid);
//    auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
//    if (mprRc == nullptr){
//        TPS_LOG_DEV_ERROR<<"The window is not a mpr window.";
//        return false;
//    }
//    FIRST_POSTFIX_COMPONENT section = mprRc->GetSectionType();
//
//    // 3. clear the MPR rendering cache
//    /*std::shared_ptr<ClearMPRRenderCacheOperation> 
//        clearoperation(new ClearMPRRenderCacheOperation(seriesUID, section));
//    this->ProcessOperation(clearoperation);*/
//
//    // 4. execute the MPR scrolling command
//    std::shared_ptr<TpsGalleryMPRPageTurnOperation> 
//        pageTurnoperation(new TpsGalleryMPRPageTurnOperation(iPageturn));
//    std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
//    auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section);
//    std::string sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
//    goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR_CORNER_INFO, section);
//    std::string sMPRCornerInfoKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
//    goType = GOTypeHelper::ComposeAsGOType(GALLERY_MEASURE, section);
//    std::string sMeasureKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
//    goType = GOTypeHelper::ComposeAsGOType(GALLERY_TEXT, section);
//    std::string sTextKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
//    pageTurnoperation->SetGraphicObjectKeys(sVolumeKey, sMPRKey, sMPRCornerInfoKey, sMeasureKey, sTextKey);
//    this->ProcessOperation(pageTurnoperation);
//
//    //7. fire to render
//    //this->Render();
//
//    return true;
//}

bool TpsRenderProxy::GalleryMprPan(WINDOW_TYPE wndType, const std::string& wndUid,
    float startX, float startY, float stopX, float stopY)
{
    std::string seriesUID;
    if (!this->GetDataSource(wndType, IMAGE, seriesUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
        return false;
    }

    auto renderController = this->GetRenderController(wndType, wndUid);
    auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
    if (mprRc == nullptr){
        TPS_LOG_DEV_ERROR<<"The window is not a gallery mpr window.";
        return false;
    }
    FIRST_POSTFIX_COMPONENT section = mprRc->GetSectionType();
    DISPLAY_SIZE dispsize;
    renderController->GetDisplaySize(dispsize);

    std::shared_ptr<TpsGalleryMPRPanOperation> 
        panOperation(new TpsGalleryMPRPanOperation(dispsize.width, dispsize.height, 
        startX, startY, stopX, stopY));

    std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
    auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section);
    std::string sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    panOperation->SetGraphicObjectKeys(sVolumeKey,sMPRKey);
    this->ProcessOperation(panOperation);

    //this->Render();
    return true;
}

bool TpsRenderProxy::GalleryMprZoom(WINDOW_TYPE wndType, const std::string& wndUid, float scaleFactor)
{
    std::string seriesUID;
    if (!this->GetDataSource(wndType, IMAGE, seriesUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
        return false;
    }

    auto renderController = this->GetRenderController(wndType, wndUid);
    auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
    if (mprRc == nullptr){
        TPS_LOG_DEV_ERROR<<"The window is not a mpr window.";
        return false;
    }
    FIRST_POSTFIX_COMPONENT section = mprRc->GetSectionType();
   /* DISPLAY_SIZE dispsize;
    renderController->GetDisplaySize(dispsize);*/

    std::shared_ptr<TpsGalleryMPRZoomOperation> zoomOperation(new TpsGalleryMPRZoomOperation(scaleFactor));

    std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
    auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section);
    std::string sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    goType = GOTypeHelper::ComposeAsGOType(MPR_RULER_TEXT, section);
    std::string sMPRRulerTextKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    zoomOperation->SetGraphicObjectKeys(sVolumeKey, sMPRKey, sMPRRulerTextKey);
    this->ProcessOperation(zoomOperation);

    //this->Render();
    return true;
}

bool TpsRenderProxy::GalleryMprZoom(WINDOW_TYPE wndType, const std::string& wndUid,
    float startX, float startY, float stopX, float stopY)
{
    std::string seriesUID;
    if (!this->GetDataSource(wndType, IMAGE, seriesUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
        return false;
    }

    auto renderController = this->GetRenderController(wndType, wndUid);
    auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
    if (mprRc == nullptr){
        TPS_LOG_DEV_ERROR<<"The window is not a gallery mpr window.";
        return false;
    }
    FIRST_POSTFIX_COMPONENT section = mprRc->GetSectionType();
    DISPLAY_SIZE dispsize;
    renderController->GetDisplaySize(dispsize);

    std::shared_ptr<TpsGalleryMPRZoomOperation>
        zoomOperation(new TpsGalleryMPRZoomOperation(dispsize.width, dispsize.height, 
        startX, startY, stopX, stopY));

    std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
    auto goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section);
    std::string sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    goType = GOTypeHelper::ComposeAsGOType(MPR_RULER_TEXT, section);
    std::string sMPRRulerTextKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    zoomOperation->SetGraphicObjectKeys(sVolumeKey, sMPRKey, sMPRRulerTextKey);
    this->ProcessOperation(zoomOperation);

    //this->Render();
    return true;
}

bool TpsRenderProxy::GalleryMprWindowing(WINDOW_TYPE wndType, const std::string& wndUid, 
    float deltaWidth, float deltaCenter, bool updateAll)
{
    std::string seriesUID;
    if (!this->GetDataSource(wndType, IMAGE, seriesUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window.";
        return false;
    }

    auto renderController = this->GetRenderController(wndType, wndUid);
    auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
    if (mprRc == nullptr){
        TPS_LOG_DEV_ERROR<<"The window is not a mpr window.";
        return false;
    }
    FIRST_POSTFIX_COMPONENT section = mprRc->GetSectionType();

    std::shared_ptr<TpsGalleryMPRWindowingOperation> windowingOperation(
        new TpsGalleryMPRWindowingOperation(deltaWidth, deltaCenter));

    std::string sVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
    //windowingOperation.reset(new TpsGalleryMPRWindowingOperation(deltaWidth, deltaCenter));
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section);
    std::string sCurrMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR_CORNER_INFO, section);
    std::string sCurrMPRCornerInfoKey = seriesUID + "|" + GOTypeHelper::ToString(goType);

    std::string sMPRKey, sMPRCornerInfoKey;
    std::vector<std::string> vecLinkedMprKey, vecLinkedMprCornerInfoKey;
    if (updateAll){
        FIRST_POSTFIX_COMPONENT sections[3] = { AXIAL, CORONAL, SAGITTAL};
        for (int i = 0; i < 3; ++i){
            if (sections[i] == section) continue;
            goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, sections[i]);
            sMPRKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
            goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR_CORNER_INFO, sections[i]);
            sMPRCornerInfoKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
            vecLinkedMprKey.push_back(sMPRKey);
            vecLinkedMprCornerInfoKey.push_back(sMPRCornerInfoKey);
        }
    }
    windowingOperation->SetGraphicObjectKeys(sVolumeKey, sCurrMPRKey, 
        sCurrMPRCornerInfoKey, vecLinkedMprKey, vecLinkedMprCornerInfoKey);
    this->ProcessOperation(windowingOperation);

    //this->Render();
    return true;
}

bool TpsRenderProxy::GalleryShadedSurfaceRotate(const std::string seriesUID, 
    const Mcsf::Point2f& prePt, const Mcsf::Point2f& curPt)
{
    // TODO Check?
    std::shared_ptr<TpsGalleryVRRotateOperation> rotateShadedSurface(
        new TpsGalleryVRRotateOperation(seriesUID, prePt, curPt));
    this->ProcessOperation(rotateShadedSurface);

    //this->Render();
    return true;
}

bool TpsRenderProxy::GetCrosshairProjectionPositionForGallery(WINDOW_TYPE windowType, 
    const std::string& windowUid, float& x2D, float& y2D)
{
    auto renderController = this->GetRenderController(windowType, windowUid);
    if (renderController == nullptr){
        TPS_LOG_DEV_ERROR << "Render controller of window: " << windowUid
            << "does not exists.";
        return false;
    }

    auto mprRenderController = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
    if (mprRenderController==nullptr) {
        TPS_LOG_DEV_ERROR << "Render controller is not mpr";
        return false;
    }
    if (!mprRenderController->GetCrosshairProjectionPosition(x2D, y2D)) {
        TPS_LOG_DEV_ERROR << "GetCrosshairProjectionPosition failed!";
        return false;
    }

    return true;
}

bool TpsRenderProxy::GetCrossXYPointsForGallery(WINDOW_TYPE windowType,
    const std::string& windowUid, double& x1, double& y1, double& x2, double& y2, double& x3, double& y3, double& x4, double& y4)
{
    std::string seriesUID;
    if (!this->GetDataSource(windowType, IMAGE, seriesUID)) {
        TPS_LOG_DEV_ERROR << "Can not get the image data source of current window type: "
            << windowType;
        return false;
    }

    auto renderController = this->GetRenderController(windowType, windowUid);
    if (renderController == nullptr){
        TPS_LOG_DEV_ERROR << "Render controller of window: " << windowUid
            << "does not exists.";
        return false;
    }

    auto mprRenderController = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
    if (mprRenderController == nullptr) {
        TPS_LOG_DEV_ERROR << "Render controller is not mpr";
        return false;
    }

    std::shared_ptr<TpsGalleryCrosshairUpdateOperation>
        crosshairUpdateOp(new TpsGalleryCrosshairUpdateOperation(
        mModelWarehouse, seriesUID));
    this->ProcessOperation(crosshairUpdateOp);

    if (!mprRenderController->GetCrossXYPoints(x1, y1, x2, y2, x3, y3, x4, y4)) {
        TPS_LOG_DEV_ERROR << "GetCrosshairProjectionPosition failed!";
        return false;
    }

    return true;
}

bool TpsRenderProxy::SetCrosshairHittedModeForGallery(int wndType, const std::string& wndUid, int mode)
{
    auto renderController = this->GetRenderController((WINDOW_TYPE)wndType, wndUid);
    auto mprRenderController = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
    if (mprRenderController == nullptr){
        TPS_LOG_DEV_ERROR<<"The render controller is not a TpsMPRRenderController.";
        return false;
    }

    //if crosshair moving begin, hide non image overlays
    //if end, resume displaying.
    //bool isToHide = mode != CrosshairGraphicObject::HITTED_NOTHING;
    //如何十字线移动结束，更新VOI， Dose line
    //if (!isToHide){
        //auto section = mprRenderController->GetSectionType();
        //std::string seriesUID, planUID;
        //if (!this->GetDataSource((WINDOW_TYPE)wndType, IMAGE, seriesUID)){
        //    TPS_LOG_DEV_ERROR<<"Failed to get image data source.";
        //    return false;
        //}
        //this->GetDataSource((WINDOW_TYPE)wndType, PLAN, planUID);

        //unsigned char ucSectionTypes[] = {AXIAL, SAGITTAL, CORONAL};
        //std::string operateSeriesUid = mDataRepository->GetImageDataManager(
        //    )->GetOperationImageUid(seriesUID);
        //for (int i = 0; i < sizeof(ucSectionTypes) / sizeof(ucSectionTypes[0]); i++)
        //{
        //    if (ucSectionTypes[i] == section) 
        //        continue;
        //    //3. update voi cache collection
        //    std::shared_ptr<TpsUpdateVOICacheCollectionOperation>
        //        updateVOICacheCollectionOp(new TpsUpdateVOICacheCollectionOperation(
        //        operateSeriesUid, ucSectionTypes[i]));
        //    updateVOICacheCollectionOp->SetDataRepository(mDataRepository);
        //    this->ProcessOperation(updateVOICacheCollectionOp);

        //    //4. update dose line buffer
        //    if (!planUID.empty()){
        //        std::shared_ptr<TpsRefreshDoseOperation> refreshDose(
        //            new TpsRefreshDoseOperation(planUID, ucSectionTypes[i]));
        //        refreshDose->SetDataRepository(mDataRepository);
        //        this->ProcessOperation(refreshDose);
        //    }
        //}
    //}
    //HideMprNonImageForGallery((WINDOW_TYPE)wndType, isToHide);
    mprRenderController->SetCrosshairHittedModeForGallery(mode);
    return true;
}

//void TpsRenderProxy::TranslateCrosshairForGallery(WINDOW_TYPE windowType, 
//    const std::string& windowUid, double xStart, double yStart, double xEnd, double yEnd)
//{
//    //printf("x:%.5f, y:%.5f\r\n", xEnd, yEnd);
//    std::string seriesUID;
//    if (!this->GetDataSource(windowType, IMAGE, seriesUID)) {
//        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window type: "
//            <<windowType;
//        return;
//    }
//    //std::string planUID = "";
//    //this->GetDataSource(windowType, PLAN, planUID);
//
//    //2. get section type
//    auto renderController = this->GetRenderController(windowType, windowUid);
//    auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
//    FIRST_POSTFIX_COMPONENT section;
//    if (mprRc != nullptr) {
//        section = mprRc->GetSectionType();
//    }
//    else {
//        TPS_LOG_DEV_ERROR<<"The window is not a mpr window.";
//        return;
//    }
//
//    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, section);
//    std::string strKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
//    auto go = this->GetModelWarehouse()->GetModelObject(strKey);
//    auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
//    if (spCrosshairGO == nullptr){
//        TPS_LOG_DEV_ERROR<<"[ERROR][CommonOperatorImpl::UpdateCrosshair]: "
//            <<"Failed to dynamic cast IGraphicObjects.";
//        return;
//    }
//
//    std::shared_ptr<TpsGalleryCrosshairMovementOperation> crosshairOperation(
//        new TpsGalleryCrosshairMovementOperation(seriesUID, section, xStart, yStart, xEnd, yEnd));
//    this->ProcessOperation(crosshairOperation);
//}

void TpsRenderProxy::UpdateCrosshairForGallery(WINDOW_TYPE windowType, 
    const std::string& windowUid, double xStart, double yStart, double xEnd, double yEnd, int crossMode, int hitMode, double rotateX, double rotateY)
{
    //printf("x:%.5f, y:%.5f\r\n", xEnd, yEnd);
    std::string seriesUID;
    if (!this->GetDataSource(windowType, IMAGE, seriesUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window type: "
            <<windowType;
        return;
    }

    //2. get section type
    auto renderController = this->GetRenderController(windowType, windowUid);
    auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
    FIRST_POSTFIX_COMPONENT section;
    if (mprRc != nullptr) {
        section = mprRc->GetSectionType();
    }
    else {
        TPS_LOG_DEV_ERROR<<"The window is not a mpr window.";
        return;
    }

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, section);
    std::string strKey = seriesUID + "|" + GOTypeHelper::ToString(goType);
    auto go = this->GetModelWarehouse()->GetModelObject(strKey);
    auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
    if (spCrosshairGO == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][CommonOperatorImpl::UpdateCrosshair]: "
            <<"Failed to dynamic cast IGraphicObjects.";
        return;
    }

    if (hitMode == CrosshairGraphicObject::HITTED_AXIS_Y_Top || hitMode == CrosshairGraphicObject::HITTED_AXIS_Y_Bottom || 
        hitMode == CrosshairGraphicObject::HITTED_AXIS_X_Left || hitMode == CrosshairGraphicObject::HITTED_AXIS_X_Right) // Oblique mode
    {
        std::shared_ptr<TpsGalleryCrosshairRotateOperation> crosshairOperation(
            new TpsGalleryCrosshairRotateOperation(seriesUID, section, xStart, yStart, xEnd, yEnd, rotateX, rotateY, hitMode, crossMode));
        this->ProcessOperation(crosshairOperation);
    }
    else
    {
        std::shared_ptr<TpsGalleryCrosshairMovementOperation> crosshairOperation(
            new TpsGalleryCrosshairMovementOperation(seriesUID, section, xStart, yStart, xEnd, yEnd, hitMode, crossMode));
        this->ProcessOperation(crosshairOperation);
    }
}

void TpsRenderProxy::HideMprNonImageForGallery(WINDOW_TYPE windowType, bool isToHide)
{
    DATA_SOURCE_UID sourceUid;
    if (!this->GetDataSource(windowType, IMAGE, sourceUid)){
        TPS_LOG_DEV_ERROR<<"Failed to get data source uid using window type: "<<windowType;
        return;
    }

    auto groups = this->SelectGroups(
        [sourceUid](DATA_SOURCE_GROUP_ID /*id*/, TpsDataSourceGroup* group) -> bool
    {
        std::string uid;
        return (group->GetDataSource(IMAGE, uid) && uid == sourceUid);
    });
    for (int i = 0; i < groups.size(); ++i){
        if (groups[i] == nullptr) continue;
        groups[i]->ForEach([isToHide](const std::string& /*uid*/, TpsRenderControllerBase* rc)
        {
            auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(rc);
            if (mprRc == nullptr) return;
            mprRc->HideNonImageOverlay(isToHide);
        });
    }
}

bool TpsRenderProxy::GetMprCornerInfoForGallery(WINDOW_TYPE windowType, const std::string& windowUid, 
    double normPosX, double normPosY, int suvUnitType, 
    double& density, double& value, double& windowcenter, double& windowwidth, 
    int& sliceIndex, int& startSliceNumber, int& totalSliceNum, double& sliceThickness,
    double& positionx, double& positiony, double& positionz, 
    bool& isValueValid, std::string& modality, 
    double& zoomFactor, double& rotate)
{
    std::string seriesUID;
    if (!this->GetDataSource(windowType, IMAGE, seriesUID)) {
        TPS_LOG_DEV_ERROR<<"Can not get the image data source of current window type: "
            <<windowType;
        return false;
    }
    //std::string planUID = "";
    //this->GetDataSource(windowType, PLAN, planUID);

    //2. get section type
    auto renderController = this->GetRenderController(windowType, windowUid);
    auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
    FIRST_POSTFIX_COMPONENT section;
    if (mprRc != nullptr) {
        section = mprRc->GetSectionType();
    }
    else {
        TPS_LOG_DEV_ERROR<<"The window is not a mpr window.";
        return false;
    }

    //need to change by dyx
    std::shared_ptr<TpsGalleryMPRUpdateCornerTextInfoOperation> spUpdateCornerInfoOp(
        new TpsGalleryMPRUpdateCornerTextInfoOperation(normPosX, normPosY));

    std::string strVolumeKey = seriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section);
    std::string strMprKey = seriesUID + "|" + GOTypeHelper::ToString(goType);

    spUpdateCornerInfoOp->SetGraphicObjectKeys(strVolumeKey, strMprKey, "", ""); // 后两个GO在新版本下已经废弃不用
    spUpdateCornerInfoOp->SetSuvType(suvUnitType);
    spUpdateCornerInfoOp->SetSeriesUid(seriesUID);
    this->ProcessOperation(spUpdateCornerInfoOp);

    // 读取获得的相关信息
    density = spUpdateCornerInfoOp->mDensity;
    value = spUpdateCornerInfoOp->mValue;
    windowcenter = spUpdateCornerInfoOp->mWindowCenter;
    windowwidth = spUpdateCornerInfoOp->mWindowWidth;
    sliceIndex = spUpdateCornerInfoOp->mSliceIndex;
	startSliceNumber = spUpdateCornerInfoOp->mStartSliceNumber;
    totalSliceNum = spUpdateCornerInfoOp->mTotalSliceNum;
    positionx = spUpdateCornerInfoOp->mPositionX;
    positiony = spUpdateCornerInfoOp->mPositionY;
    positionz = spUpdateCornerInfoOp->mPositionZ;
    sliceThickness = spUpdateCornerInfoOp->mSliceThickness;
    isValueValid = spUpdateCornerInfoOp->mIsValueValid;
    modality = spUpdateCornerInfoOp->mModality;
    zoomFactor = spUpdateCornerInfoOp->mZoomFactor;
    rotate = spUpdateCornerInfoOp->mRotate;

    return true;
}

//重置MPR 底图 并且 重置 十字线的四个点
bool TpsRenderProxy::ResetMPRObjectForGallery(WINDOW_TYPE windowType, const std::string& windowUid)
{
    auto renderController = this->GetRenderController(windowType, windowUid);
    auto mprRc = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
    FIRST_POSTFIX_COMPONENT section;
    if (mprRc != nullptr) {
        section = mprRc->GetSectionType();
    }
    else {
        TPS_LOG_DEV_ERROR << "The window is not a mpr window.";
        return false;
    }

    std::string seriesUID;
    if (!this->GetDataSource(windowType, IMAGE, seriesUID)) {
        TPS_LOG_DEV_ERROR << "Can not get the image data source of current window type: "
            << windowType;
        return false;
    }

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, section);
    std::string goTypeStr = GOTypeHelper::ToString(goType);
    std::string key = seriesUID + "|" + goTypeStr;
    auto go = mModelWarehouse->GetModelObject(key);
    auto spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    double ww, wc;
    double factor = 1.0f;
    {
        TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
        spMprGO->GetWindowing(ww, wc);
        spMprGO->GetScale(&factor);
        spMprGO->SetScale(1.0f);
    }
    //重置前先保存窗宽窗位
    mprRc->ResetMPRObject();
    {
        TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
        spMprGO->SetWindowing(ww, wc);
    }
    this->MprZoom(windowType, windowUid, factor);

    return true;
}

bool TpsRenderProxy::GetMprViewDirecrionForGallery(WINDOW_TYPE windowType, const std::string& windowUid, 
    double& x1, double& y1, double& z1, double& x2, double& y2, double& z2) {
        auto renderController = this->GetRenderController(windowType, windowUid);
        if (renderController == nullptr) {
            TPS_LOG_DEV_ERROR << "Render controller of window: " << windowUid
                << "does not exists.";
            return false;
        }

        auto mprRenderController = dynamic_cast<TpsGalleryMPRRenderController*>(renderController);
        if (mprRenderController == nullptr) {
            TPS_LOG_DEV_ERROR << "Render controller is not mpr";
            return false;
        }

        Mcsf::MedViewer3D::Vector3D vecView, vecUp;
        if (!mprRenderController->GetMprViewDirecrion(vecView, vecUp)) {
            TPS_LOG_DEV_ERROR<<"GetMprViewDirecrion failed!";
            return false;
        } 
        else {
            x1 = vecView.x;
            y1 = vecView.y;
            z1 = vecView.z;
            x2 = vecUp.x;
            y2 = vecUp.y;
            z2 = vecUp.z;
        }

        return true;
}

bool TpsRenderProxy::GetShadedSurfaceViewDirecrionForGallery(WINDOW_TYPE windowType, const std::string& windowUid, 
    double& x1, double& y1, double& z1, double& x2, double& y2, double& z2) {
        auto renderController = this->GetRenderController(windowType, windowUid);
        if (renderController == nullptr) {
            TPS_LOG_DEV_ERROR << "Render controller of window: " << windowUid
                << "does not exists.";
            return false;
        }

        auto shadedSurfaceRenderController = dynamic_cast<TpsGalleryShadedSurfaceRenderController*>(renderController);
        if (shadedSurfaceRenderController == nullptr) {
            TPS_LOG_DEV_ERROR << "Render controller is not bev";
            return false;
        }

        Mcsf::MedViewer3D::Vector3D vecView, vecUp;
        if (!shadedSurfaceRenderController->GetShadedSurfaceViewDirecrion(vecView, vecUp)) {
            TPS_LOG_DEV_ERROR<<"GetShadedSurfaceViewDirecrion failed!";
            return false;
        } 
        else {
            x1 = vecView.x;
            y1 = vecView.y;
            z1 = vecView.z;
            x2 = vecUp.x;
            y2 = vecUp.y;
            z2 = vecUp.z;
        }

        return true;
}

void TpsRenderProxy::DownSampleDrr(WINDOW_TYPE windowType, 
    const std::string& windowUid, float downSampleRate)
{
    if (downSampleRate <= 1e-6) return;

    auto rc = this->GetRenderController(windowType, windowUid);
    auto bevRc = dynamic_cast<TpsBEVRenderController*>(rc);
    if (bevRc == nullptr) return;

    //降采样两种方式，一：增大DRR算法的步长，二：缩小FBO大小。
    //这里先同时应用两种方式，即既增大步长同时缩小FBO Size。
    //之后再看要不要只采用一种方式。
    //change the drr step size
    std::shared_ptr<SetDrrDownSampleRateOperation> op(
        new SetDrrDownSampleRateOperation(downSampleRate));
    this->ProcessOperation(op);
    //change the fbo size
    bevRc->DownSampleImage(downSampleRate);
}

//void TpsRenderProxy::MoveTable(std::string imageUid, std::string tableUid, float offSetX, float offSetY, float offsetZ, float angleX, float angleY, float angleZ)
//{
//    std::shared_ptr<MoveTableOperation> moveTableop(
//        new MoveTableOperation(imageUid, tableUid, offSetX, offSetY, offsetZ, angleX, angleY, angleZ));
//    this->ProcessOperation(moveTableop);
//    this->Render();
//}

bool TpsRenderProxy::EditContour(const std::string /*seriesUID*/, const std::string /*voiUID*/, EDIT_MODE /*editMode*/)
{
    return false;
    //return this->ProcessOperation(std::make_shared<EditContourOperation>(seriesUID, voiUID, editMode));
}

bool TpsRenderProxy::PasteContour(const std::string /*seriesUID*/, const std::string /*voiUID*/, EDIT_FUNCTION /*editFunction*/)
{
    return false;
    //return this->ProcessOperation(std::make_shared<PasteContourOperation>(seriesUID, voiUID, editFunction));
}

void TpsRenderProxy::ClearGraphicObjectsViaDataSource(DATA_SOURCE_ID sourceID, const DATA_SOURCE_UID& uid)
{
    TpsRenderControllerFactory* factory = TpsRenderControllerFactory::GetInstance();
    TpsRenderControllerBase* rc = nullptr;
    for (auto it = mCreatedWindowTypes.begin(); it != mCreatedWindowTypes.end(); ++it){
        rc = factory->CreateRenderController(it->first);
        rc->SetModelWarehouse(mModelWarehouse);
        if (rc != nullptr){
            rc->ClearGraphicObjects(sourceID, uid);
            delete rc; rc = nullptr;
        }
    }
}

void TpsRenderProxy::RemoveRestrationRelationship(
    const std::string& fixedUid, const std::string& floatUid)
{
    this->ProcessOperation(std::make_shared<RemoveRegistrationRelationshipOperation>(fixedUid, floatUid));
}

bool TpsRenderProxy::IsInVolume(WINDOW_TYPE windowType, const std::string& windowUid, float x, float y)
{
    auto renderController = this->GetRenderController(windowType, windowUid);
    auto mprRc = dynamic_cast<TpsMPRRenderController*>(renderController);
    if (mprRc == nullptr){
        TPS_LOG_DEV_ERROR<<"The window is not a mpr window.";
        return false;
    }

    return mprRc->IsInVolume(x, y);
}

void TpsRenderProxy::ChangePoiVisibilityOfSeries(const std::string& seriesUid, bool isVisible)
{
    this->ProcessOperation(std::make_shared<ChangeAllPoiVisibilityOperation>(seriesUid, isVisible));
}

void TpsRenderProxy::UpdateSeriesTable(const std::string& seriesUid)
{
    GO_TYPE goTypes[3] = 
    {
        GO_TYPE_SERIES_TABLE_CACHE_CORONAL,
        GO_TYPE_SERIES_TABLE_CACHE_SAGITTAL,
        GO_TYPE_SERIES_TABLE_CACHE_AXIAL,
    };
    for (int i = 0; i < 3; ++i)
    {
        this->ProcessOperation(
            std::make_shared<UpdateGraphicObjectOperation>(seriesUid, goTypes[i], IMAGE));
    }
}

void TpsRenderProxy::MprPageTurn_i(const std::string& imageUid, 
    FIRST_POSTFIX_COMPONENT section, SECOND_POSTFIX_COMPONENT extension, 
    int steps, int refreshType)
{
    std::string mainSeriesUID = mDataRepository->GetImageDataManager(
        )->GetOperationImageUid(imageUid);

    std::shared_ptr<MPRPageTurnOperation> pageTurnoperation(
        new MPRPageTurnOperation(steps, imageUid, section, extension));
    this->ProcessOperation(pageTurnoperation);

    //update voi cache collection
//     if (refreshType & Refresh_Voi)
//     {
//         std::shared_ptr<TpsUpdateVOICacheCollectionOperation>
//             updateVOICacheCollectionOp(new TpsUpdateVOICacheCollectionOperation(
//             mainSeriesUID, section, extension));
//         this->ProcessOperation(updateVOICacheCollectionOp);
//     }

//     // update series table
//     if (refreshType & Refresh_Table)
//     {
//         auto goType = GOTypeHelper::ComposeAsGOType(SERIES_TABLE_CACHE, section, extension);
//         this->ProcessOperation(
//             std::make_shared<UpdateGraphicObjectOperation>(mainSeriesUID, goType, IMAGE));
//     }

    // update dose plane buffer
    if (refreshType & Refresh_Dose)
    {
        std::vector<std::string> planUIDList;
        std::string otherSeriesUID, planUID;
        auto dsGroups = this->GetAllDataSourceGroup();
        for (auto it = dsGroups->begin(); it != dsGroups->end(); ++it) 
        {
            if (!it->second->GetDataSource(IMAGE, otherSeriesUID) || 
                otherSeriesUID != mainSeriesUID)
            {
                continue;
            }
            if (it->second->GetDataSource(PLAN, planUID))
            {
                if (std::find(planUIDList.begin(), planUIDList.end(), planUID) == planUIDList.end()) 
                {
                    planUIDList.push_back(planUID);
                }
            }
        }
        for (auto itUID = planUIDList.begin(); itUID != planUIDList.end(); ++itUID)
        {
            std::shared_ptr<TpsRefreshDoseOperation> refreshDose(
                new TpsRefreshDoseOperation(*itUID, section));
            this->ProcessOperation(refreshDose);
        }
    }
}

bool TpsRenderProxy::ContainsGraphicObject(const std::string& baseUid, GO_TYPE goType)
{
    std::string goTypeStr = GOTypeHelper::ToString(goType);
    std::string key = baseUid + "|" + goTypeStr;

    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    return mModelWarehouse->GetModelObject(key) != nullptr;
}
TPS_END_NAMESPACE  // end namespace tps
