//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
///
///  \file    tps_pca_mprpageturn_planningcmd.cpp
///  \brief   MPR pageturn command for PlanningApp
///
///  \version 1.0
///  \date    Jan. 7, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_auto_contouring.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "boost/date_time/posix_time/posix_time.hpp"

//ZHENGHE
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

//TPS
#include "tps_logger.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

#include "RtTpsCertifiedLib/rt_tps_certifiedautovois.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"

#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_mask.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_entity_converter.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"

#include "RtTpsRenderController/tps_rc_mpr_render_controller.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_delete_voi.h"

#include "RtTpsFramework/tps_fw_progress.h"
#include "RtTpsFramework/tps_fw_active_responser_interface.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsROIUtility/tps_ru_roi_shape_converter.h"

TPS_BEGIN_NAMESPACE;

AutoCoutouringCmd::AutoCoutouringCmd(CertifiedAutoVois &certifiedVois) 
: mCertifiedAutoVois(certifiedVois),mProgressResponser(nullptr), mProgress(nullptr)
{
    mProgress = new TpsProgress();
}


AutoCoutouringCmd::~AutoCoutouringCmd(void)
{
    DEL_PTR(mProgress);
}

TPS_ORGAN_TYPE String2Enum_ORGAN_TYPE(const char* user_input)
{
    for (int t = 0; t < UNKNOWN; ++t){    
        if (0 == strcmp(Enum2String_ORGAN_TYPE[t], user_input)){
            return (TPS_ORGAN_TYPE)t;
        }
    }
    return UNKNOWN;
};

void AutoCoutouringCmd::SetProgressResponser(
    const std::shared_ptr<ProgressActiveResponser>& responser)
{
    mProgressResponser = responser;
}

int AutoCoutouringCmd::Execute()
{
    if (mCertifiedAutoVois.VOIList.empty())
    {
        return TPS_ER_SUCCESS;
    }
    std::string sTargetImageDataUid = mCertifiedAutoVois.VOIList[0].mSeriesUid;

    TpsImage3DEntity* image3D = nullptr;
    //TODO using current image data UID
    TpsImageDataManager* imgMgr = mDataRepository->GetImageDataManager();
    imgMgr->GetImage3DData(sTargetImageDataUid, &image3D);
    if (nullptr == image3D){
        TPS_LOG_DEV_ERROR<<"no image3d with the image UID:"<<sTargetImageDataUid;
        return TPS_ER_FAILURE;
    }
    
    Mcsf::Matrix4f matP2V;
    if (!image3D->GetVolumeToPatientMatrix(matP2V))
    {
        TPS_LOG_DEV_ERROR<<"Failed to get volume to patient matrix of image:"<<sTargetImageDataUid;
        return TPS_ER_FAILURE;
    }
    matP2V = matP2V.Inverse();

    //GetAutoContour
    auto pAutoContour = image3D->GetAutoContour();
    if (nullptr == pAutoContour){
        image3D->CreateAutoContour();
    }

    size_t dim[3];
    image3D->GetDimension(dim);
    auto roi = mDataRepository->GetVoiManager()->GetExternal(sTargetImageDataUid);
    McsfGeometry::SubVolume* skinSV = nullptr;
    if (roi)
    {
        skinSV = new McsfGeometry::SubVolume();
        ROIShapeConverter::SDFToSubVolume(*roi->GetSDF(), 
            TpsArithmeticConverter::ConvertToMatrix4x4(matP2V), dim, *skinSV);
    }
    //TODO:自动分割接口不支持超出体数据的subvolume
    if(!image3D->InitializationAutoContour(skinSV)){
        TPS_LOG_DEV_ERROR<<"InitializationAutoContour failed.";
        return TPS_ER_FAILURE;
    }

    mProgress->SetCurrent(0.0);
    if (mProgressResponser) 
    {
        mProgress->SetProgressResponser(mProgressResponser);
    }

    typedef std::vector<CertifiedVOI> CertVoiVector;
    CertVoiVector voilist = mCertifiedAutoVois.VOIList;
    int voiCount = voilist.size();
    int i = 0;
    for(CertVoiVector::iterator itr = voilist.begin(); itr != voilist.end(); ++i) 
    {
        mProgress->SetProgressWeight(1 / (double)voiCount);
        mProgress->SetProgressStartValue( i / (double)voiCount);

        if (!ProcessOneROI_i(*itr, image3D, matP2V)) 
        {
            TPS_LOG_DEV_ERROR<<"Failed to auto-segment voi["<<itr->mName<<"] with uid["
                <<itr->mVoiUid<<"].";
            itr = voilist.erase(itr);
        }
        else 
        {  
            TPS_LOG_DEV_INFO<<"auto-segmentation for voi["<<itr->mName<<"] is DONE!";
            ++itr;
        }  

    }
    TPS_DEL_PTR(skinSV);
    mRenderProxy->Render();
    if (!mProgressResponser) 
    {
        mProgress->SetCurrent(1.0);
    }

    mCertifiedAutoVois.VOIList = voilist;
    return TPS_ER_SUCCESS;
}

TPS_ORGAN_TYPE AutoCoutouringCmd::ConvertName2OrganTypeEnum(const std::string& strVal)
{
    std::string sOrganName(strVal);
    std::string sOrganNameToupper(sOrganName);
    (void)std::transform(sOrganName.begin(), sOrganName.end(), sOrganNameToupper.begin(),
        static_cast<int(*)(int)>(std::toupper));
    TPS_ORGAN_TYPE organType = String2Enum_ORGAN_TYPE(sOrganNameToupper.c_str());
    return organType;
}

bool AutoCoutouringCmd::ProcessOneROI_i(CertifiedVOI& certfiedVOI,
    TpsImage3DEntity* image3D, 
    const Mcsf::Matrix4f& matP2V)
{
    std::string voiUID = certfiedVOI.mVoiUid;
    RtVoi* innerRoi = new RtVoi(true);
    innerRoi->set_uid(voiUID);
    innerRoi->set_structuresetuid(voiUID);
    innerRoi->set_seriesuid(image3D->GetUID());
    innerRoi->set_isvisible(certfiedVOI.mIsVisible);
    innerRoi->set_color(certfiedVOI.mColor);
    innerRoi->set_name(certfiedVOI.mName);
    innerRoi->set_planningrole(certfiedVOI.mPlanningRole);
    innerRoi->set_subrole(certfiedVOI.mSubrole);
    innerRoi->set_locktype(certfiedVOI.mLockType);
    innerRoi->set_description(certfiedVOI.mDescription);
    innerRoi->set_isoverride(certfiedVOI.mIsOverride);
    float arrP2V[16];
    matP2V.GetElements(arrP2V);
    innerRoi->set_pat2volumematrix(arrP2V);

    TpsVOIManager *voi_manager = mDataRepository->GetVoiManager();
    ROIEntity* roi_entity = voi_manager->CreateROIEntity(innerRoi);
    roi_entity->SetIsQA(false);

    TPS_ORGAN_TYPE organType = (TPS_ORGAN_TYPE)certfiedVOI.mOrganType;
    SubVolume* sv = new SubVolume();
    int iRet = 1;
    try 
    {
        switch(organType)
        {
        case SKIN:
            iRet = SubVolumeHelperAPI::SkinSegmentation(*image3D,sv, (Mcsf::IProgress*)mProgress);
            break;
        case LUNGS:
            iRet = SubVolumeHelperAPI::LungsSegmentation(*image3D, 0, sv, (Mcsf::IProgress*)mProgress);
            break;
        case LUNG_LEFT:
            iRet = SubVolumeHelperAPI::LungsSegmentation(*image3D, 1, sv, (Mcsf::IProgress*)mProgress);
            break;
        case LUNG_RIGHT:
            iRet = SubVolumeHelperAPI::LungsSegmentation(*image3D, 2, sv, (Mcsf::IProgress*)mProgress);
            break;
        case SPINAL_CORD:
            iRet = SubVolumeHelperAPI::SpinalCordSegmentation(*image3D, sv, (Mcsf::IProgress*)mProgress);
            break;
        case HEART:
            iRet = SubVolumeHelperAPI::HeartSegmentation(*image3D, sv, (Mcsf::IProgress*)mProgress);
            break;
        case LIVER:
            iRet = SubVolumeHelperAPI::LiverSegmentation(*image3D, sv, (Mcsf::IProgress*)mProgress);
            break;
        case SPLEEN:
            iRet = SubVolumeHelperAPI::SpleenSegmentation(*image3D, sv, (Mcsf::IProgress*)mProgress);
            break;
        case KIDNEYS:     
            iRet = SubVolumeHelperAPI::KidneySegmentation(*image3D, 0, sv, (Mcsf::IProgress*)mProgress);
            break;
        case KIDNEY_LEFT:
            iRet = SubVolumeHelperAPI::KidneySegmentation(*image3D, 1, sv, (Mcsf::IProgress*)mProgress);
            break;
        case KIDNEY_RIGHT:
            iRet = SubVolumeHelperAPI::KidneySegmentation(*image3D, 2, sv, (Mcsf::IProgress*)mProgress);
            break;
        case PAROTIDS:
            iRet = SubVolumeHelperAPI::ParotidSegmentation(*image3D, 0, sv, (Mcsf::IProgress*)mProgress);
            break; 
        case PAROTID_LEFT:
            iRet = SubVolumeHelperAPI::ParotidSegmentation(*image3D, 1, sv, (Mcsf::IProgress*)mProgress);
            break;
        case PAROTID_RIGHT:
            iRet = SubVolumeHelperAPI::ParotidSegmentation(*image3D, 2, sv, (Mcsf::IProgress*)mProgress);
            break;
        case BRAINSTEM:
            iRet = SubVolumeHelperAPI::BrainstemSegmentation(*image3D, sv, (Mcsf::IProgress*)mProgress);
            break;
        case MANDIBLE:
            iRet = SubVolumeHelperAPI::MandibleSegmentation(*image3D, sv, (Mcsf::IProgress*)mProgress);
            break;
        case EYES:
            iRet = SubVolumeHelperAPI::EyeballSegmentation(*image3D, 0, sv, (Mcsf::IProgress*)mProgress);
            break;
        case EYE_LEFT:
            iRet = SubVolumeHelperAPI::EyeballSegmentation(*image3D, 1, sv, (Mcsf::IProgress*)mProgress);
            break;
        case EYE_RIGHT:
            iRet = SubVolumeHelperAPI::EyeballSegmentation(*image3D, 2, sv, (Mcsf::IProgress*)mProgress);
            break;
        case FEMORAL_HEAD://SFS_TPS_APP_PatientAnatomyModelling_AutoContour_Whirlbone_MCSF  
            iRet = SubVolumeHelperAPI::FemoralheadSegmentation(*image3D, sv, (Mcsf::IProgress*)mProgress);
            break;
        case BRAIN:
            iRet = SubVolumeHelperAPI::BrainSegmentation(*image3D, sv, (Mcsf::IProgress*)mProgress);
            break;
        case RECTUM:
            iRet = SubVolumeHelperAPI::RectumSegmentation(*image3D, sv, (Mcsf::IProgress*)mProgress);
            break;
        case PROSTATE:
            {
                SubVolume* rectumSV = new SubVolume();
                iRet = SubVolumeHelperAPI::ProstateSegmentation(*image3D, false, rectumSV, sv, (Mcsf::IProgress*)mProgress);
                TPS_DEL_PTR(rectumSV);
                break;
            }
        case TEMPOROMANDIBULAR_JOINT:
            iRet = SubVolumeHelperAPI::JawJointSegmentation(*image3D, sv, (Mcsf::IProgress*)mProgress);
            break;
        case PANCREAS:
            iRet = SubVolumeHelperAPI::PancreasSegmentation(*image3D, sv, (Mcsf::IProgress*)mProgress);
            break;
        case BLADDER://NOT delivered
            //break;
        case BONES: //NOT delivered
            //break;
        default:
            TPS_LOG_DEV_ERROR<<"unsupported auto-segmentation for VOI type: "<<organType;
            TPS_DEL_PTR(roi_entity);
            TPS_DEL_PTR(sv);
            mProgress->SetCancelFlag(true);
            return false;
        }
    }
    catch(std::exception& e) 
    {
        mProgress->SetCancelFlag(true);
        TPS_LOG_DEV_ERROR<<"Exception happened during algorithm execution:"<<e.what();
        TPS_DEL_PTR(roi_entity);
        TPS_DEL_PTR(sv);
        return false;
    }
    catch(...)
    {
        mProgress->SetCancelFlag(true);
        TPS_LOG_DEV_ERROR<<"Exception happened during algorithm execution.";
        TPS_DEL_PTR(roi_entity);
        TPS_DEL_PTR(sv);
        return false;
    }

    if (TPS_ER_SUCCESS != iRet) 
    {
        TPS_DEL_PTR(roi_entity);
        TPS_DEL_PTR(sv);
        TPS_LOG_DEV_ERROR<<"wrong organ type! or auto segmentation failed!";
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    auto sdf = roi_entity->GetSDF();
    MAT4 p2v = TpsArithmeticConverter::ConvertToMatrix4x4(matP2V);
    ROIShapeConverter::SubVolumeToSDF(*sv, p2v, *sdf);
    TPS_DEL_PTR(sv);
    ROIShapeConverter::SDFToContourSet(*sdf, 2, *roi_entity->GetContourSet());

    auto imageUID = image3D->GetUID();
    //update voi statistical information
    RtCt2density* pCt2Density = mDataRepository->GetImageDataManager(
        )->GetCT2DensityTableByImageUid(imageUID);
    if (!SubVolumeHelperAPI::UpdateVOIStatisticalInformation(pCt2Density, *image3D, roi_entity))
    {
        TPS_LOG_DEV_ERROR<<"Failed to update statistical information.";
        TPS_DEL_PTR(roi_entity);
        return false;
    }

    if(TPS_ER_SUCCESS != mSaveObjects->SaveRoi(*roi_entity))
    {
        TPS_DEL_PTR(roi_entity);
        TPS_LOG_DEV_ERROR<<"Failed to save roi: "<<voiUID <<" to database.";
        return TPS_ER_FAILURE;
    }
    voi_manager->AddROI(roi_entity);

    if (!mRenderProxy->AddVOI(voiUID, imageUID))
    {
        TPS_LOG_DEV_ERROR<<"Failed to add roi: "<<voiUID<<" to render proxy.";
        return TPS_ER_FAILURE;
    }

    //prepare certified voi for FE
    EntityConverter::GetInstance()->ROIEntiyToCertifiedVOI(*roi_entity, &certfiedVOI);
    return true;
}

int AutoCoutouringCmd::DeleteVoi_i(const std::string& sVoiUid) 
{
    DeleteVoiCmd cmd(sVoiUid);
    cmd.SetDataRepository(mDataRepository);
    cmd.SetRenderProxy(mRenderProxy);
    cmd.SetLoadObjects(mLoadObjects);
    cmd.SetDatabaseWrapper(mDatabaseWrapper);
    cmd.SetSaveObjects(mSaveObjects);
    if (cmd.RunCommand() != TPS_ER_SUCCESS) {
        TPS_LOG_DEV_ERROR << "delete voi command error.";
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
