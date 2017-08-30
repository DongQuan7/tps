////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_manual_registration.cpp
/// 
///  \brief class ManualRegistrationCmd implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/11/02
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_manual_registration.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsDataAccess/tps_da_fusionsecondarydatamanager.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

ManualRegistrationCmd::ManualRegistrationCmd(
    WINDOW_TYPE wndType, const std::string& wndUid, 
    float preX, float preY, float curX, float curY, 
    ManualRegistrationType actionType) 
    : mWndType(wndType), mWndUid(wndUid)
    , mPreX(preX), mPreY(preY), mCurX(curX), mCurY(curY)
    , mActionType(actionType){

}

ManualRegistrationCmd::~ManualRegistrationCmd(){

}

int ManualRegistrationCmd::Execute(){
    if (mWndType != WINDOW_FUSION_MIXED_MPR_CORONAL &&
        mWndType != WINDOW_FUSION_MIXED_MPR_SAGITTAL && 
        mWndType != WINDOW_FUSION_MIXED_MPR_AXIAL)
    {
        TPS_LOG_DEV_ERROR<<"Current window is not fusion window, \
                            can not do manual registration.";
        return TPS_ER_FAILURE;
    }

    if (fabs(mPreX - mCurX) <= 1e-6 && fabs(mPreY - mCurY) <= 1e-6)
    {
        return TPS_ER_SUCCESS;
    }

    Mcsf::Point2f preSrnPt(mPreX, mPreY), curSrnPt(mCurX, mCurY);
    Mcsf::Point3f fixed_start, fixed_end, fixed_center;
    if (!mRenderProxy->TransformPoint(mWndType, mWndUid, preSrnPt, fixed_start) || 
        !mRenderProxy->TransformPoint(mWndType, mWndUid, curSrnPt, fixed_end) || 
        !mRenderProxy->TransformPoint(mWndType, mWndUid, Mcsf::Point2f(0.5f, 0.5f), fixed_center))
    {
        TPS_LOG_DEV_ERROR<<"Failed to transform points.";
        return TPS_ER_FAILURE;
    }

    std::string fixedUid, floatUid;
    if (!mRenderProxy->GetDataSource(mWndType, IMAGE, fixedUid) ||
        !mRenderProxy->GetDataSource(mWndType, FLOAT_IMAGE, floatUid))
    {
        TPS_LOG_DEV_ERROR<<"Failed to get data source.";
        return TPS_ER_FAILURE;
    }

    TpsImage3DEntity* image = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(fixedUid, &image)){
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d data.";
        return TPS_ER_FAILURE;
    }

    Mcsf::Matrix4f matrix = mDataRepository->GetFusionSecondaryDataManager()->GetRigidResult();
//     if (!image->GetRegistrationMatrix(floatUid, matrix)){
//         TPS_LOG_DEV_ERROR<<"Failed to get registration matrix";
//         return TPS_ER_FAILURE;
//     }
    Mcsf::Point3f float_start = matrix.Transform(fixed_start);
    Mcsf::Point3f float_end = matrix.Transform(fixed_end);
    Mcsf::Point3f float_center = matrix.Transform(fixed_center);

    if (mActionType == Manual_Reg_Rotate){
        Mcsf::Vector3f vecStart(float_start - float_center);
        Mcsf::Vector3f vecEnd(float_end - float_center);
        Mcsf::Vector3f vecToCenter(-1 * float_center);
        Mcsf::Vector3f normal = vecEnd.CrossProduct(vecStart);

        Mcsf::Vector3f vecFixedStart(fixed_start - fixed_center);
        Mcsf::Vector3f vecFixedEnd(fixed_end - fixed_center);
        float angle = vecFixedEnd.AngleBetween(vecFixedStart);
        Mcsf::Quat quat(angle, normal);

        //translate to center
        matrix.Prepend(MakeTranslate(vecToCenter));
        //rotate
        matrix.Prepend(quat.ToMatix4x4());
        //translate back
        matrix.Prepend(MakeTranslate(-1 * vecToCenter));
    }
    else if (mActionType == Manual_Reg_Translate){
        Mcsf::Vector3f vecTranslate = float_end - float_start;
        matrix.Prepend(MakeTranslate(-1 * vecTranslate));
    }
    else {
        TPS_LOG_DEV_ERROR<<"Unrecognized manual registration type.";
        return TPS_ER_FAILURE;
    }
    //image->SetRegistrationMatrix(floatUid, matrix);
    mDataRepository->GetFusionSecondaryDataManager()->SetRigidResult(matrix);
    mRenderProxy->RefreshRegistrationResult();
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}
TPS_END_NAMESPACE  // end namespace tps