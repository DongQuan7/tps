////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_approve_registration_result.cpp
/// 
///  \brief class ApproveRegistrationResultCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/10/20
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_approve_registration_result.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "Mcsf3DArithmetic/mcsf_3d_matrix4x4.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_imagetransformation.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_approverecord.h"


#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_fusionsecondarydatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "tps_logger.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

ApproveRegistrationResultCmd::ApproveRegistrationResultCmd(
    RtApproverecord* approveRecord,
    std::string description, int registrationMethod)
{
    mApproveRecord = approveRecord;
    mApproveResult._description = description;
    mApproveResult._registrationMethod = registrationMethod;

}

ApproveRegistrationResultCmd::~ApproveRegistrationResultCmd()
{

}

/* <key> \n
* PRA: Yes \n
* Name:int ApproveRegistrationResultCmd::Execute() \n
* Traced from: 30941 DS_PRA_TPS_UserConfirmFusionResult \n
* Description: user confirm the image registration result, \n
*              a new resgitration relationship will be stored into the database, \n
*              user can re-load the relationship next time \n
* Short Description: image resgitration confirmation \n
* Component: TPS \n
* </key> \n
*/
int ApproveRegistrationResultCmd::Execute()
{
    auto mng = mDataRepository->GetFusionSecondaryDataManager();
    std::string fixedUID = mng->GetFixedImageUID();
    std::string floatUID = mng->GetFloatImageUID();
    Mcsf::Matrix4f matrix = mng->GetRigidResult();

    //get fixed image entity
    TpsImage3DEntity* fixedImage = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(
        fixedUID, &fixedImage) || fixedImage == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The fixed image does not exist which uid: " << fixedUID;
        return TPS_ER_FAILURE;
    }

    mApproveResult._fixedUid = fixedUID;
    mApproveResult._floatUid = floatUID;
    mApproveResult._isSucceeded = false;

    RtImagetransformation* transformation = nullptr;
    float matrixBuffer[16];
    matrix.GetElements(matrixBuffer);
    //if already exists, update
    if (DB_CODES_SUCCESS == mDatabaseWrapper->GetImagetransformation(fixedUID, floatUID, transformation))
    {
        transformation->set_originalseriesuid(fixedUID);
        transformation->set_transfomationseriesuid(floatUID);
        transformation->set_registrationmatrix(matrixBuffer);
        transformation->set_description(mApproveResult._description);
        transformation->set_registrationmethod(mApproveResult._registrationMethod);
        mApproveResult._uid = transformation->get_uid();

        if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateImagetransformation(*transformation))
        {
            TPS_LOG_DEV_ERROR << "Failed to update registration result.";
            return TPS_ER_DB_ERROR;
        }
    }
    else
    {
        transformation = new RtImagetransformation(true);
        transformation->set_originalseriesuid(fixedUID);
        transformation->set_transfomationseriesuid(floatUID);
        transformation->set_registrationmatrix(matrixBuffer);
        transformation->set_description(mApproveResult._description);
        transformation->set_registrationmethod(mApproveResult._registrationMethod);
        mApproveResult._uid = transformation->get_uid();

        if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertImagetransformationObject(*transformation))
        {
            TPS_LOG_DEV_ERROR << "Failed to insert registration result.";
            DEL_PTR(transformation);
            return TPS_ER_DB_ERROR;
        }
    }

    mApproveRecord->set_objectuid(mApproveResult._uid);
    int iRet = mSaveObjects->SaveApproveRecord(*mApproveRecord);
    if (iRet != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to InsertApproveRecord in ApproveRegistrationResultCmd";
        return iRet;
    }

    if (!fixedImage->SetImagetransformation(transformation))
    {
        TPS_LOG_DEV_ERROR << "Failed to set registration matrix into fix image which uid: " << fixedUID;
        return TPS_ER_FAILURE;
    }

    mRenderProxy->RefreshApprovedRegistrationResult(fixedUID, floatUID);
    mRenderProxy->Render();
    mApproveResult._isSucceeded = true;
    return TPS_ER_SUCCESS;

}
TPS_END_NAMESPACE  // end namespace tps