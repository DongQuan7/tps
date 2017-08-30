//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  LI Song  song.li@united-imaging.com
///
///  \file    tps_cca_contouring_controllerapp_voi_advanced.cpp
///  \brief   Contouring Controller Application
///
///  \version 1.0
///  \date    Feb. 3, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include"stdafx.h"
#include "RtTpsBusinessLogic/tps_business_logic_controller.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsBusinessLogic/tps_bl_add_roi_advanced_parameter_structrues.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"

#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"

#include "RtTpsAlgorithmProxy/tps_ap_voi_helper.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

int TpsBLController::AddRoiBasedOnRoi(
    const AddRoiBasedOnRoiPara& /*para*/, ROIEntity*& /*roi*/)
{
    //if (para.vecOpParas.empty())
    //{
    //    TPS_LOG_DEV_ERROR<<"AddRoiBasedOnRoi error, there is no operators.";
    //    return TPS_ER_FAILURE;
    //}

    //TpsVOIManager* voiMng = mDataRepository->GetVoiManager();
    //VOIEntity *srcRoi = nullptr;
    //voiMng->GetVOI(para.sourceRoiUid, &srcRoi);
    //if (srcRoi == nullptr)
    //{
    //    TPS_LOG_DEV_ERROR<<"Source roi does not exist.";
    //    return TPS_ER_FAILURE;
    //}
    //if (srcRoi->IsEmpty())
    //{
    //    TPS_LOG_DEV_ERROR<<"Source roi is empty.";
    //    return TPS_ER_FAILURE;
    //}
    //auto sv = srcRoi->GetSubVolume();
    //if (sv == nullptr)
    //{
    //    TPS_LOG_DEV_ERROR<<"Source roi has no sub-volume.";
    //    return TPS_ER_FAILURE;
    //}

    //std::string imgUID = srcRoi->GetSeriesUid();
    //TpsImage3DEntity* image3D = nullptr;
    //if (!mDataRepository->GetImageDataManager()->GetImage3DData(imgUID, &image3D)
    //    || image3D == nullptr)
    //{
    //    TPS_LOG_DEV_ERROR << "Failed to get image from manager with series uid:" << imgUID;
    //    return TPS_ER_FAILURE;
    //}

    ////prepare sub-volumes
    //if (!srcRoi->IsSubVolumeMinimumBounded())
    //{
    //    size_t dim[3];
    //    image3D->GetDimension(dim);
    //    SubVolumeHelperAPI::RefineSubvolume(dim, srcRoi);
    //}

    //int size[3];
    //sv->GetSize(size[0], size[1], size[2]);
    //int offset[3];
    //sv->GetOffSet(offset[0], offset[1], offset[2]);
    //char* buffer = sv->GetData();
    //SubVolume* srcSV = new SubVolume();
    //srcSV->SetOffSet(offset[0], offset[1], offset[2]);
    //srcSV->SetSize(size[0], size[1], size[2]);
    //memcpy(srcSV->GetData(), buffer, size[0] * size[1] * size[2] * sizeof(char));
    //SubVolume* tgtSV = new SubVolume();

    //for (int i = 0; i < para.vecOpParas.size(); ++i)
    //{
    //    switch (para.vecOpParas[i].mode) 
    //    {
    //    case Mirror:
    //        MirrorRoi_i(image3D, srcSV, para.vecOpParas[i].MirrorPara(), tgtSV);
    //        break;
    //    case Expand:
    //        ExpandRoi_i(image3D, srcSV, para.vecOpParas[i].ExpandPara(), true, tgtSV);
    //        break;
    //    case Shrink:
    //        ExpandRoi_i(image3D, srcSV, para.vecOpParas[i].ExpandPara(), false, tgtSV);
    //        break;
    //    case Combine:
    //        CombineRoi_i(srcSV, para.vecOpParas[i].CombinePara(), tgtSV);
    //        break;
    //    }
    //    swap(srcSV, tgtSV);
    //}

    //int iRet = para.isPreview ? 
    //    PreviewRoi_i(image3D, para.targetRoiUid, para.roiColor, srcSV) : 
    //    AddRoi_i(image3D, para.targetRoiUid, para.roiName, para.planningRole, para.subRole, para.roiColor, srcSV, roi);

    //TPS_DEL_PTR(srcSV);
    //TPS_DEL_PTR(tgtSV);
    //return iRet;
    return TPS_ER_FAILURE;
}

int TpsBLController::AddRoiBasedOnDose(
    const AddRoiBasedOnDosePara& /*para*/, ROIEntity*& /*roi*/)
{
    //TpsImage3DEntity* pImage3D = nullptr;
    //if (!mDataRepository->GetImageDataManager()->GetImage3DData(para.imageUid, &pImage3D)
    //    || pImage3D == nullptr)
    //{
    //    TPS_LOG_DEV_ERROR << "Failed to get image from manager with series uid:" << para.imageUid;
    //    return TPS_ER_FAILURE;
    //}

    //RtPlan* pPlan = nullptr;
    //if (!mDataRepository->GetPatientManager()->GetPlan(para.planUid, &pPlan)
    //    || pPlan == nullptr)
    //{
    //    TPS_LOG_DEV_ERROR << "Failed to get plan from manager with plan uid:" << para.planUid;
    //    return TPS_ER_FAILURE;
    //}

    //SubVolume* sv = new SubVolume();
    //float minVal = TPS_MIN(para.fromDose, para.toDose);
    //float maxVal = TPS_MAX(para.fromDose, para.toDose);
    //TpsVoiHelper::GenerateVoiFromDose(pImage3D, pPlan->get_dosegrid(), minVal, maxVal, sv);

    //int iRet = para.isPreview ? 
    //    PreviewRoi_i(pImage3D, para.targetRoiUid, para.roiColor, sv) : 
    //    AddRoi_i(pImage3D, para.targetRoiUid, para.roiName, para.planningRole, para.subRole, para.roiColor, sv, roi);

    //TPS_DEL_PTR(sv);
    //return iRet;
    return TPS_ER_FAILURE;
}

int TpsBLController::PreviewRoi_i(
    TpsImage3DEntity* /*image3D*/,
    const std::string& /*targetUID*/, 
    const float* /*color*/,
    McsfGeometry::SubVolume*& /*sv*/)
{
    //TpsVOIManager* voiMng = mDataRepository->GetVoiManager();
    //VOIEntity* pDstVoi = voiMng->CreateVOIEntity(new RtVoi(true));
    //pDstVoi->SetVoiUid(targetUID);
    //pDstVoi->SetStructureSetUid(targetUID);
    //std::string imgUID = image3D->GetUID();
    //pDstVoi->SetSeriesUid(imgUID);
    //float rgba[4];
    //memcpy(rgba, color, 3 * sizeof(float));
    //rgba[3] = 0.0f;
    //pDstVoi->SetColor(rgba);
    //pDstVoi->SetVisibility(true);

    ////保留原有逻辑
    //m_vecPreviewVOI.push_back(pDstVoi); // will be deleted when create new Preview ViewModel or in controller destructor
    //if (m_vecPreviewVOI.size() >= 3)
    //{
    //    std::vector<VOIEntity*> vecTemp;
    //    vecTemp.push_back(m_vecPreviewVOI[1]);
    //    vecTemp.push_back(m_vecPreviewVOI[2]);
    //    delete m_vecPreviewVOI[0];
    //    m_vecPreviewVOI = vecTemp;
    //}

    //Mcsf::Point3f center;
    //if (TPS_ER_SUCCESS == SubVolumeHelperAPI::GetSubVolumeCenter(*sv, center))
    //{
    //    Mcsf::Matrix4f matV2P;
    //    image3D->GetVolumeToPatientMatrix(matV2P);
    //    center = matV2P.Transform(center);
    //    mRenderProxy->VoiPreviewJumpTo(imgUID, center[0], center[1], center[2]); 
    //}

    //pDstVoi->SwapSubVolume(sv);

    //if (!mRenderProxy->PreviewVOI(pDstVoi, imgUID))
    //{
    //    TPS_LOG_DEV_ERROR<<"Failed to add voi: "<<targetUID<<" to render proxy.";
    //    TPS_DEL_PTR(pDstVoi);
    //    return TPS_ER_FAILURE;
    //}
    //mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

int TpsBLController::AddRoi_i(
    TpsImage3DEntity* /*image3D*/, 
    const std::string& /*targetUID*/, 
    const std::string& /*roiName*/,
    int /*roiPlanningRole*/,
    int /*roiSubRole*/,
    const float* /*color*/,
    McsfGeometry::SubVolume*& /*sv*/, 
    ROIEntity*& /*roi*/)
{
    //TpsVOIManager* voiMng = mDataRepository->GetVoiManager();
    //bool isTargetNew = false;
    //VOIEntity* pDstVoi = nullptr;
    //if (!voiMng->GetVOI(targetUID, &pDstVoi) || pDstVoi == nullptr)
    //{
    //    pDstVoi= voiMng->CreateVOIEntity(new RtVoi(true));
    //    pDstVoi->SetVoiUid(targetUID);
    //    pDstVoi->SetStructureSetUid(targetUID);
    //    isTargetNew = true;
    //}

    //std::string imgUID = image3D->GetUID();
    //if (isTargetNew)
    //{
    //    pDstVoi->SetSeriesUid(imgUID);
    //    float rgba[4];
    //    memcpy(rgba, color, 3 * sizeof(float));
    //    rgba[3] = 0.0f;
    //    pDstVoi->SetColor(rgba);
    //    pDstVoi->SetName(roiName);
    //    pDstVoi->SetVisibility(true);
    //    pDstVoi->SetIsOverrided(false);
    //    pDstVoi->SetAlpha3D(0.f);
    //    pDstVoi->SetPlanningRole((RtDbDef::PLANNING_ROLE_TYPE)roiPlanningRole);
    //    pDstVoi->SetSubRole((RtDbDef::SUBROLE_TYPE)roiSubRole);
    //    pDstVoi->SetLockType((RtDbDef::RTDB_VOI_LOCKTYPE)RtDbDef::UNLOCK); // 和yige.zhang确认过没有该需求，将NORMALLOCK，改为UNLOCK。
    //}
    //pDstVoi->SwapSubVolume(sv);

    //RtCt2density* pCt2density =
    //    mDataRepository->GetImageDataManager()->GetCT2DensityTableByImageUid(imgUID);
    //if( TpsVoiHelper::PopulateVoiContourGivenSubvolume(pDstVoi, image3D, pCt2density)) {
    //    TPS_LOG_DEV_ERROR<<"Failed to populate contours for voi.";
    //}

    //if(TPS_ER_SUCCESS != mSaveObjects->SaveVoi(*pDstVoi)){
    //    TPS_LOG_DEV_ERROR<<"Failed to save voi: "<<targetUID<<" to database.";
    //    if (isTargetNew) TPS_DEL_PTR(pDstVoi);
    //    return TPS_ER_FAILURE;
    //}

    //if(isTargetNew)
    //{
    //    voiMng->AddVOI(pDstVoi); // pDstVoi owner changed to VOI manager, will be released in its destructor
    //    if (!mRenderProxy->AddVOI(targetUID, imgUID)){
    //        TPS_LOG_DEV_ERROR<<"Failed to add voi: "<<targetUID<<" to render proxy.";
    //        TPS_DEL_PTR(pDstVoi);
    //        return TPS_ER_FAILURE;
    //    }
    //}
    //else
    //{
    //    mRenderProxy->UpdateVOI(imgUID, targetUID);
    //}

    //roi = pDstVoi;
    //mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

int TpsBLController::MirrorRoi_i(
    TpsImage3DEntity* image3D, 
    McsfGeometry::SubVolume* srcSV, 
    const MirrorRoiPara& para, 
    McsfGeometry::SubVolume* tgtSV)
{
    return TpsVoiHelper::GenerateVoiViaMirror(image3D, srcSV, para.direction, tgtSV);
}

int TpsBLController::ExpandRoi_i(
    TpsImage3DEntity* image3D, 
    McsfGeometry::SubVolume* srcSV, 
    const ExpandRoiPara& para, 
    bool isExpansion,
    McsfGeometry::SubVolume* tgtSV)
{
    RtImage3DHeader* header = nullptr;
    image3D->GetImage3DHeader(header);
    if (header == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Image header is null.";
        return TPS_ER_FAILURE;
    }
    size_t volumeDim[3] = { header->m_iXDim, header->m_iYDim, header->m_iSliceCount };
    Mcsf::Matrix4f pat2Vol;
    image3D->GetVolumeToPatientMatrix(pat2Vol);
    pat2Vol = pat2Vol.Inverse();

    //由patient中的偏移，计算在volume中的像素偏移
    float margin[6] = 
    {
        para.isotropicMargin, para.isotropicMargin, para.isotropicMargin, 
        para.isotropicMargin, para.isotropicMargin, para.isotropicMargin, 
    };
    if (!para.isIsotropic) memcpy(margin, para.margin, 6 * sizeof(float));

    Mcsf::Vector3f vecOffset[6] = 
    {
        Mcsf::Vector3f(-margin[0], 0, 0),
        Mcsf::Vector3f(margin[1], 0, 0),
        Mcsf::Vector3f(0, -margin[2], 0),
        Mcsf::Vector3f(0, margin[3], 0),
        Mcsf::Vector3f(0, 0, -margin[4]),
        Mcsf::Vector3f(0, 0, margin[5]),
    };
    int sign = isExpansion ? 1 : -1;
    float fDelta[6] = { 0.f };
    for (int i = 0; i < 6; ++i)
    {
        vecOffset[i] = pat2Vol.Transform(sign * vecOffset[i]);
        for (int j = 0; j < 3; ++j)
        {
            if (vecOffset[i][j] > 0)
            {
                fDelta[2 * j + 1] += vecOffset[i][j];
            }
            else
            {
                fDelta[2 * j] += vecOffset[i][j];
            }
        }
    }
    int delta[6];
    for (int i = 0; i < 6; ++i)
    {
        delta[i] = TPS_ROUND(fDelta[i]);
    }

    auto expSV1 = para.isToCreateRing ? new McsfGeometry::SubVolume() : tgtSV;
    int iRet = TpsVoiHelper::GenerateVoiViaExpansion(
        srcSV, para.is2D, isExpansion, delta, volumeDim, expSV1);
    if (TPS_ER_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR<<"Failed to generate voi via expansion.";
        if (para.isToCreateRing) TPS_DEL_PTR(expSV1);
        return iRet;
    }
    if (!para.isToCreateRing) return TPS_ER_SUCCESS;

    for (int i = 0; i < 6; ++i)
    {
        margin[i] += para.ringSize;
    }
    vecOffset[0] = Mcsf::Vector3f(-margin[0], 0, 0);
    vecOffset[1] = Mcsf::Vector3f(margin[1], 0, 0);
    vecOffset[2] = Mcsf::Vector3f(0, -margin[2], 0);
    vecOffset[3] = Mcsf::Vector3f(0, margin[3] , 0);
    vecOffset[4] = Mcsf::Vector3f(0, 0, -margin[4]);
    vecOffset[5] = Mcsf::Vector3f(0, 0, margin[5]);

    memset(fDelta, 0, 6 * sizeof(float));
    for (int i = 0; i < 6; ++i)
    {
        vecOffset[i] = pat2Vol.Transform(sign * vecOffset[i]);
        for (int j = 0; j < 3; ++j)
        {
            if (vecOffset[i][j] > 0)
            {
                fDelta[2 * j + 1] += vecOffset[i][j];
            }
            else
            {
                fDelta[2 * j] += vecOffset[i][j];
            }
        }
    }
    for (int i = 0; i < 6; ++i)
    {
        delta[i] = TPS_ROUND(fDelta[i]);
    }

    auto expSV2 = new McsfGeometry::SubVolume();
    iRet = TpsVoiHelper::GenerateVoiViaExpansion(
        srcSV, para.is2D, isExpansion, delta, volumeDim, expSV2);
    if (TPS_ER_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR<<"Failed to generate voi via expansion.";
        TPS_DEL_PTR(expSV1);
        TPS_DEL_PTR(expSV2);
        return iRet;
    }

    iRet = TpsVoiHelper::GenerateVoiViaCombination(expSV1, expSV2, XOR, tgtSV);
    TPS_DEL_PTR(expSV1);
    TPS_DEL_PTR(expSV2);
    return iRet;
}

int TpsBLController::CombineRoi_i(
    McsfGeometry::SubVolume* /*srcSV*/, 
    const CombineRoiPara& /*para*/, 
    McsfGeometry::SubVolume* /*tgtSV*/)
{
//     TpsVOIManager* voiMng = mDataRepository->GetVoiManager();
//     std::string combineRoiUid;
// 
//     VOIEntity *combineRoi = nullptr;
//     McsfGeometry::SubVolume* combineSV = nullptr;
//     for (int i = 0; i < para.elements.size(); ++i)
//     {
//         combineRoiUid = para.elements[i].roiUid;
//         voiMng->GetVOI(combineRoiUid, &combineRoi);
//         if (combineRoi == nullptr)
//         {
//             TPS_LOG_DEV_ERROR<<"Source roi does not exist.";
//             return TPS_ER_FAILURE;
//         }
// 
//         combineSV = combineRoi->GetSubVolume();
//         if (combineSV == nullptr)
//         {
//             TPS_LOG_DEV_ERROR<<"Combine roi has no sub-volume.";
//             return TPS_ER_FAILURE;
//         }
// 
//         if (TPS_ER_SUCCESS != TpsVoiHelper::GenerateVoiViaCombination(
//             srcSV, combineSV, para.elements[i].mode, tgtSV))
//         {
//             TPS_LOG_DEV_ERROR<<"Failed to generate roi via combination.";
//             return TPS_ER_FAILURE;
//         }
//     }
    return TPS_ER_SUCCESS;
}

int TpsBLController::VoiRemovePreviewWindow()
{
    // delete preview VOIs
    if (m_vecPreviewVOI.size() > 0)
    {
        for(auto iter = m_vecPreviewVOI.begin(); iter != m_vecPreviewVOI.end(); iter++) {
            delete *iter;
        }
        m_vecPreviewVOI.clear();
    }
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
