//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_easyplan_take_igrt.cpp 
///  \brief  take igrt image
///
///  \version 1.0
///  \date  2015/12/01  
///  \
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "RtTpsBusinessLogic/tps_bl_cmd_easyplan_take_igrt.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"

#include "RtTpsFramework/tps_fw_renderproxybase.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"

TPS_BEGIN_NAMESPACE
TpsTakeIgrtCmd::TpsTakeIgrtCmd(std::string planUID)
:m_pRtImage(nullptr), mPlanUID(planUID){

}

TpsTakeIgrtCmd::~TpsTakeIgrtCmd()
{

}

int TpsTakeIgrtCmd::Execute()
{
    RtRtimage tempOpposedRtImage(true);
    RtRtimage *opposedRtImage = new RtRtimage(*m_pRtImage);
    opposedRtImage->set_uid(tempOpposedRtImage.get_uid());

    std::string imageUID = m_pRtImage->get_uid();
    std::string opposedImageUID = opposedRtImage->get_uid();

    TpsImageDataManager* imageDataManager = mDataRepository->GetImageDataManager();
    imageDataManager->SetEasyPlanImageUID(mPlanUID, imageUID);
    imageDataManager->SetEasyPlanOpposedImageUID(mPlanUID, opposedImageUID);

    TpsImage2DEntity *pTpsImage_DRR = nullptr;
    TpsImage2DEntity *pTpsOpposedImage_DRR = nullptr;
    if (false == imageDataManager->GetImage2DData(imageUID, &pTpsImage_DRR)) {
        pTpsImage_DRR = new TpsImage2DEntity();
        pTpsImage_DRR->SetRTImage(m_pRtImage); // shallow copy
        imageDataManager->AddImage2D(imageUID, pTpsImage_DRR);
    }
    if(false == imageDataManager->GetImage2DData(opposedImageUID, &pTpsOpposedImage_DRR)) {
        pTpsOpposedImage_DRR = new TpsImage2DEntity();
        pTpsOpposedImage_DRR->SetRTImage(opposedRtImage); //shallow copy
        imageDataManager->AddImage2D(opposedImageUID, pTpsOpposedImage_DRR);
    }

    mRenderProxy->SetDataSource(SOURCE_GROUP_EZPLAN, IMAGE, imageUID);
    mRenderProxy->SetEasyPlanPixelSpace(m_pRtImage->get_firstimageplanepixelspacing(), m_pRtImage->get_secondimageplanepixelspacing());
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

void TpsTakeIgrtCmd::SetRtImage( RtRtimage *rtImage )
{
    m_pRtImage = rtImage;
}

TPS_END_NAMESPACE