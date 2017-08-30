//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  LI Song song.li@united-imaging.com
///
///  \file    tps_phantom_voi_delete_cmd.cpp
///  \brief   phantom delete command
///
///  \version 1.0
///  \date    Mar. 23, 2015
///  
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_phantom_delete.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE;

PhantomDeleteCmd::PhantomDeleteCmd(const std::string& strPatientUid, const std::string& strPhantomSeriesUid) :
m_strPatientUid(strPatientUid), m_strPhantomSeriesUid(strPhantomSeriesUid){

}

PhantomDeleteCmd::~PhantomDeleteCmd() {

}

int PhantomDeleteCmd::Execute() 
{

    _ASSERTE( _CrtCheckMemory() );

    //TpsImageDataManager *pImageManager = TpsImageDataManager::GetInstance();

    TpsImageDataManager *pImageManager = mDataRepository->GetImageDataManager();

    pImageManager->DeleteImage3D(m_strPhantomSeriesUid);

    int bResult = mSaveObjects->EraseObject(m_strPatientUid, RtDbDef::TYPE_PATIENT);
    // to do
    if (bResult != TPS_ER_SUCCESS) 
    {
        TPS_LOG_DEV_ERROR<<"PhantomDeleteCmd: Failed to delete phantom patient from TMS:" << m_strPatientUid;
        return TPS_ER_FAILURE;
    }

    std::string curImageUid("");
    mRenderProxy->GetDataSource(SOURCE_GROUP_ADMIN, IMAGE, curImageUid);
    if (curImageUid == m_strPhantomSeriesUid){
        mRenderProxy->RemoveDataSource(SOURCE_GROUP_ADMIN, PLAN);
        mRenderProxy->RemoveDataSource(SOURCE_GROUP_ADMIN, IMAGE);
    }

    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}
TPS_END_NAMESPACE

