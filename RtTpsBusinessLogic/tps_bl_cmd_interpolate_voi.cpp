////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  WANG Jie  jie.wang@united-imaging.com
/// 
///  \file tps_bl_cmd_interpolate_voi.cpp
/// 
///  \brief class InterpolateVoiCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2017/06/14
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_interpolate_voi.h"

#include "McsfGeometry/mcsf_geometry_sub_volume.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_signed_distance_field.h"
#include "RtTpsFramework/tps_fw_contour_set.h"

#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsROIUtility/tps_ru_roi_shape_converter.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

InterpolateVoiCmd::InterpolateVoiCmd(const std::string& voiUid)
: TpsVOIRedoUndoSubVolumeCmd(voiUid)
{
}

InterpolateVoiCmd::~InterpolateVoiCmd()
{
}

int InterpolateVoiCmd::Execute()
{
    if(mVoiUid.empty())
    {
        TPS_LOG_DEV_ERROR<<"voi uid is empty!";
        return TPS_ER_FAILURE;
    }

    ROIEntity* roi = mDataRepository->GetVoiManager()->GetROI(mVoiUid);
    if(roi == nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get voi by uid:"
            <<mVoiUid;
        return TPS_ER_FAILURE;
    }

    std::string imageUid = roi->GetSeriesUid();
    if(imageUid.empty()) {
        TPS_LOG_DEV_ERROR<<"Failed to get image uid";
        return TPS_ER_FAILURE;
    }

    TpsImage3DEntity * image = nullptr;
    TpsImageDataManager* imageMng = mDataRepository->GetImageDataManager();
    if(!imageMng->GetImage3DData(imageUid,&image)) {
        TPS_LOG_DEV_ERROR<<"Failed to get image 3D";
        return TPS_ER_FAILURE;
    }

    Mcsf::Matrix4f matV2P;
    image->GetVolumeToPatientMatrix(matV2P);

    SignedDistanceField* sdf = roi->GetSDF();
    MAT4 vol2sdf = sdf->GetPatToSDF();
    vol2sdf.Append(TpsArithmeticConverter::ConvertToMatrix4x4(matV2P));
    VEC3 vecZ = vol2sdf.Transform(VEC3(0, 0, 1));
    if (abs(vecZ.z - 1.0) > 1e-6)
    {
        //if current main axis is not z in volume, do nothing.
        return TPS_ER_SUCCESS;
    }

    int size[3];
    sdf->GetSize(size);
    int start[3];
    sdf->GetStart(start);
    char* inter_flag_mask = new char[size[2] / 8 + 1];
    memset(inter_flag_mask, 0, (size[2] / 8 + 1) * sizeof(char));
    std::vector<bool> interFlags = roi->GetInnerRoi()->get_interpolate();
    for (int i = 0; i < interFlags.size(); ++i)
    {
        int idx = i - start[2];
        if (idx < 0 || idx >= size[2]) continue;
        if (interFlags[i])
        {
            inter_flag_mask[idx/8] |= (0x01 << (idx%8));
        }
    }
    for (int i = 0; i < size[2]; ++i)
    {
        if (inter_flag_mask[i/8] & (0x01 << (i%8)))
        {
            inter_flag_mask[i/8] &= ~(0x01 << (i%8));
        }
        else break;
    }
    for (int i = size[2] - 1; i >= 0; --i)
    {
        if (inter_flag_mask[i/8] & (0x01 << (i%8)))
        {
            inter_flag_mask[i/8] &= ~(0x01 << (i%8));
        }
        else break;
    }
    ROIShapeConverter::InterpolateROI(inter_flag_mask, *sdf, *roi->GetContourSet());
    TPS_DEL_ARRAY(inter_flag_mask);

    //update roi statistical information
    RtCt2density* pCt2Density = imageMng->GetCT2DensityTableByImageUid(imageUid);
    if (!SubVolumeHelperAPI::UpdateVOIStatisticalInformation(pCt2Density, *image, roi))
    {
        TPS_LOG_DEV_ERROR<<"Failed to update statistical information.";
        return TPS_ER_FAILURE;
    }

    //save voi to database
    int iRet = mSaveObjects->SaveRoi(*roi);
    if (TPS_ER_SUCCESS != iRet){
        TPS_LOG_DEV_ERROR<<"Failed to save voi to database.";
        return iRet;
    }

    mRenderProxy->UpdateVOI(imageUid,mVoiUid);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

TpsRedoUndoCommand* InterpolateVoiCmd::Clone()
{
    InterpolateVoiCmd* cmd = new InterpolateVoiCmd(mVoiUid);
    cmd->SetDatabaseWrapper(mDatabaseWrapper);
    cmd->SetDataRepository(mDataRepository);
    cmd->SetLoadObjects(mLoadObjects);
    cmd->SetSaveObjects(mSaveObjects);
    cmd->SetRenderProxy(mRenderProxy);
    cmd->mSubvolume = std::move(mSubvolume);
    cmd->mResponser = mResponser;
    return cmd;
}
TPS_END_NAMESPACE  // end namespace tps