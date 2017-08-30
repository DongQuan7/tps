//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenqiao.jiang (mailto:wenqiao.jiang@united-imaging.com)
///
///  \brief   delete image
///
///  \version 1.0
///  \date  2015/10/30  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "algorithm"
#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_easyplan_delete_image.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsFramework/tps_fw_command.h"

#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"


TPS_BEGIN_NAMESPACE

TpsDeleteEasyPlanImageDataCmd::TpsDeleteEasyPlanImageDataCmd(const std::string& sEzPlanUID) :mPlanUid(sEzPlanUID)
{

}

TpsDeleteEasyPlanImageDataCmd::~TpsDeleteEasyPlanImageDataCmd()
{

}



int TpsDeleteEasyPlanImageDataCmd::Execute()
{
    TpsImageDataManager* imageDataManager = mDataRepository->GetImageDataManager();
    std::vector<RtBeam*> beamsVec = mDataRepository->GetBeamManager()->GetBeamsByPlan(mPlanUid);
    std::vector<RtBeam*>::iterator itBeam = beamsVec.begin();
    for (; itBeam != beamsVec.end(); ++itBeam)
    {
        RtBeam* beam = *itBeam;
        TpsImage2DEntity* image2DEntity = nullptr;
        bool iRet = mDataRepository->GetImageDataManager()->Get2DImageDataByBeamUID(beam->get_uid(), image2DEntity);

        //beam没有关联的drr是正常情况
        if (!iRet) continue;

        std::string imageUID = "";
        if (!image2DEntity->GetUID(imageUID))
        {
            TPS_LOG_DEV_ERROR<<"Failed get image2D uid.";
            return TPS_ER_FAILURE;
        }

        if(TPS_ER_SUCCESS != mSaveObjects->EraseObject(imageUID, RtDbDef::TYPE_RTIMAGE)){
            TPS_LOG_DEV_ERROR<<"Failed to delete RtImage from database";
            return TPS_ER_DB_ERROR;
        }

        if (!imageDataManager->DeleteImage2D(imageUID)) {
            TPS_LOG_DEV_ERROR << "Failed to delete image of uid:" << imageUID;
            return TPS_ER_FAILURE;
        }

        mRenderProxy->DeleteDrrImage(beam->get_uid(), imageUID);
    }

    mRenderProxy->Render();
	
	//TODO: delte image entity, beam, points
    return TPS_ER_SUCCESS;
}

TPS_END_NAMESPACE