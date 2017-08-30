#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_roi_add.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsCertifiedLib/rt_tps_certifiedvoi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

TPS_BEGIN_NAMESPACE

AddROICmd::AddROICmd(CertifiedVOI& certifiedVOI) : mCertifiedVOI(certifiedVOI)
{
}


AddROICmd::~AddROICmd(void) 
{

}

int AddROICmd::Execute() 
{
    TPS_LOG_DEV_INFO<<"[INFO][AddROICmd][Execute]: "<<"AddROICmd execute!";
    const std::string imageUID = mCertifiedVOI.mSeriesUid;
    const std::string voiUID = mCertifiedVOI.mVoiUid;
    int dim[3];
    TpsImage3DEntity *tpsImage3DEntity = nullptr;
    if(!mDataRepository->GetImageDataManager()->GetImage3DData(
        imageUID, &tpsImage3DEntity) || tpsImage3DEntity == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The image 3d does not exist: "<<imageUID;
        return TPS_ER_FAILURE;
    }
    RtImage3DHeader *header = nullptr;
    tpsImage3DEntity->GetImage3DHeader(header);
    dim[0] = header->m_iXDim;
    dim[1] = header->m_iYDim;
    dim[2] = header->m_iSliceCount;

    Mcsf::Matrix4f matP2V;
    if (!tpsImage3DEntity->GetVolumeToPatientMatrix(matP2V))
    {
        TPS_LOG_DEV_ERROR<<"Failed to get volume to patient matrix of image:"<<imageUID;
        return TPS_ER_FAILURE;
    }
    matP2V = matP2V.Inverse();
    float arrP2V[16];
    matP2V.GetElements(arrP2V);

    RtVoi* innerRoi = new RtVoi(true);
    innerRoi->set_uid(voiUID);
    innerRoi->set_structuresetuid(voiUID);
    innerRoi->set_seriesuid(imageUID);
    innerRoi->set_isvisible(mCertifiedVOI.mIsVisible);
    innerRoi->set_color(mCertifiedVOI.mColor);
    innerRoi->set_name(mCertifiedVOI.mName);
    innerRoi->set_planningrole(mCertifiedVOI.mPlanningRole);
    innerRoi->set_subrole(mCertifiedVOI.mSubrole);
    innerRoi->set_locktype(mCertifiedVOI.mLockType);
    innerRoi->set_description(mCertifiedVOI.mDescription);
    innerRoi->set_isoverride(mCertifiedVOI.mIsOverride);
    innerRoi->set_pat2volumematrix(arrP2V);
    std::vector<bool> interFlags;
    for(int i = 0; i < dim[2]; ++i) 
    {
        interFlags.push_back(true);
    }
    innerRoi->set_interpolate(interFlags);

    TpsVOIManager *voi_manager = mDataRepository->GetVoiManager();
    ROIEntity* roi_entity = voi_manager->CreateROIEntity(innerRoi);
    roi_entity->SetIsQA(false);

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

    mCertifiedVOI.mVoiUid = voiUID;
    mRenderProxy->Render();
    TPS_LOG_DEV_INFO<<"[INFO][AddROICmd][Execute]: "<<"AddROICmd leave!";
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE
