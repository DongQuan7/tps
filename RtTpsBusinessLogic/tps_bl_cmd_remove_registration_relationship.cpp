////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_remove_registration_relationship.cpp
/// 
///  \brief class RemoveRegistrationRelationshipCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/10/20
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_remove_registration_relationship.h"

#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"             // for ITpsDataRepository
#include "RtTpsDataAccess/tps_da_fusionsecondarydatamanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"            // for TpsImageDataManager
#include "RtTpsFramework/tps_fw_image3dentity.h"               // for TpsImage3DEntity
#include "RtTpsRenderController/tps_rc_renderproxy.h"           // for TpsRenderProxy
#include "RtTpsFramework/tps_fw_datasourcegroup.h"              // for TpsDataSourceGroup

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

RemoveRegistrationRelationshipCmd::RemoveRegistrationRelationshipCmd(
const std::string& strFixedSeriesUid, const std::string& strFloatSeriesUid)
    :mFixedSeriesUid(strFixedSeriesUid), mFloatSeriesUid(strFloatSeriesUid){

}

RemoveRegistrationRelationshipCmd::~RemoveRegistrationRelationshipCmd(){

}

int RemoveRegistrationRelationshipCmd::Execute(){
    //get fixed image entity
    TpsImage3DEntity* fixedImage = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(
        mFixedSeriesUid, &fixedImage) || fixedImage == nullptr){
        TPS_LOG_DEV_ERROR<<"The fixed image does not exist which uid: "<<mFixedSeriesUid;
        return TPS_ER_FAILURE;
    }

    //现在Fusion页面和其他页面的配准关系已经分离，不需要这段逻辑---GML 2016-0318
    //if registration relationship to be deleted is current data source of FUSION windows
    //we shall not remove the registration matrix
    /*auto group = mRenderProxy->GetDataSourceGroup(SOURCE_GROUP_FUSION_MIXED);
    std::string currentFixed, currentFloat;
    bool needKeepMatrix = (group != nullptr && 
        group->GetDataSource(IMAGE, currentFixed) && 
        group->GetDataSource(FLOAT_IMAGE, currentFloat) && 
        currentFixed == mFixedSeriesUid && currentFloat == mFloatSeriesUid);
    if (!needKeepMatrix){
        if(!fixedImage->RemoveRegistrationMatrix(mFloatSeriesUid)) {
            TPS_LOG_DEV_ERROR<<"Failed to remove registration matrix";
            return TPS_ER_FAILURE;
        }
    }*/

    std::string sImagetransformationuid;
    if (!fixedImage->GetImageTransformationUid(mFloatSeriesUid, sImagetransformationuid))
    {
        TPS_LOG_DEV_ERROR << "Failed to get ImageTransformationUID";
        return TPS_ER_FAILURE;
    }

    auto retCode = mSaveObjects->EraseObject(
        mFixedSeriesUid, mFloatSeriesUid, RtDbDef::TYPE_IMAGETRANSFORMATION);
    if (TPS_ER_SUCCESS != retCode)
    {
        TPS_LOG_DEV_ERROR<<"Failed to delete registration relationship from DB.";
        return retCode;
    }
    std::string sCondition = "objectuid = '" + sImagetransformationuid + "'";
    retCode = mDatabaseWrapper->EraseObjectUnderCondition(sCondition, RtDbDef::TYPE_APPROVERECORD);
    if (TPS_ER_SUCCESS != retCode)
    {
        TPS_LOG_DEV_ERROR << "Failed to delete registration approve record from DB.";
        return retCode;
    }

    if (!fixedImage->RemoveImagetransformation(mFloatSeriesUid)) {
        TPS_LOG_DEV_ERROR << "Failed to remove registration matrix";
        return TPS_ER_FAILURE;
    }

    //clear data source if needed
    auto group = mRenderProxy->GetDataSourceGroup(SOURCE_GROUP_COMMON);
    if (group == nullptr){
        TPS_LOG_TRACE_INFO<<"The common data source group does not exist, need not render.";
        return TPS_ER_SUCCESS;
    }

    std::string currentFixed, currentFloat;
    std::vector<TpsDataSourceGroup*> groups = mRenderProxy->SelectGroups(
        [&](DATA_SOURCE_GROUP_ID id, TpsDataSourceGroup* group) -> bool
    {
        return (
            id != SOURCE_GROUP_FUSION_MIXED &&
            group->GetDataSource(IMAGE, currentFixed) && 
            group->GetDataSource(FLOAT_IMAGE, currentFloat) && 
            currentFixed == mFixedSeriesUid && 
            currentFloat == mFloatSeriesUid);
    });
    for (int i = 0; i < groups.size(); ++i){
        groups[i]->RemoveDataSource(FLOAT_IMAGE);
    }

    //clear graphic objects
    mRenderProxy->RemoveRestrationRelationship(mFixedSeriesUid, mFloatSeriesUid);
    if (!groups.empty()) mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE  // end namespace tps