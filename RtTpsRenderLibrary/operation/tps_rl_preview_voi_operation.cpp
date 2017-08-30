////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_preview_voi_operation.cpp
/// 
///  \brief class TpsPreviewVOIOperation implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/06/27
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_preview_voi_operation.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobejctconverter.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsPreviewVOIOperation::TpsPreviewVOIOperation(
    const std::string& strImageUID, ROIEntity* pVOI) : 
    m_strImageUID(strImageUID), m_pVOI(pVOI){

}

TpsPreviewVOIOperation::~TpsPreviewVOIOperation(){

}

bool TpsPreviewVOIOperation::ModifyGraphicObject() const{

    if (m_strImageUID.empty() || m_pVOI == nullptr){
        TPS_LOG_DEV_ERROR<<"TpsPreviewVOIOperation: Failed due to empty image or voi.";
        return false;
    }

    std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> spGo = mModelWarehouse->GetModelObject(
        m_strImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOI_COLLECTION_PREVIEW));
    std::shared_ptr<TpsVOIGOCollection> spVoiCollection = dynamic_pointer_cast<TpsVOIGOCollection>(spGo);
    if (spGo == nullptr || spVoiCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"TpsPreviewVOIOperation: Failed get VOI preview collection of image: "<<m_strImageUID;
        return false;
    }

    TpsVOIGOConverter* pVoiConverter = dynamic_cast<TpsVOIGOConverter*>(
        mConverterFactory->GetConverter(GO_TYPE_VOI));
    if (pVoiConverter == nullptr){
        TPS_LOG_DEV_ERROR<<"TpsPreviewVOIOperation: Failed get VOI graphic object converter.";
        return false;
    }

    TpsImage3DEntity* pImageEntity = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(m_strImageUID, &pImageEntity)){
        TPS_LOG_DEV_ERROR<<"TpsPreviewVOIOperation: Failed to get image entity with uid: "<<m_strImageUID;
        return false;
    }
    else if(pImageEntity == nullptr) {
        TPS_LOG_DEV_ERROR<<"TpsPreviewVOIOperation: Failed to get image entity with uid: "<<m_strImageUID;
        return false;
    }
    RtImage3DHeader* header = nullptr;
    if (!pImageEntity->GetImage3DHeader(header) || header == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get image header.";
        return false;
    }

    spGo = mModelWarehouse->GetModelObject(m_strImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME));
    auto spVolume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(spGo);
    if (spVolume == nullptr){
        TPS_LOG_DEV_ERROR<<"TpsPreviewVOIOperation: Failed to get volume graphic object with uid: "<<m_strImageUID;
        return false;
    }

    pVoiConverter->SetSliceCount(header->m_iSliceCount);
    pVoiConverter->SetVoi(m_pVOI);
    spGo = pVoiConverter->Convert();
    std::shared_ptr<TpsVOIGraphicObject> spVoiGo = dynamic_pointer_cast<TpsVOIGraphicObject>(spGo);
    if (spVoiGo == nullptr){
        TPS_LOG_DEV_ERROR<<"TpsPreviewVOIOperation: Failed convert VOI graphic object.";
        return false;
    }

    spVoiCollection->AddVOI(m_pVOI->GetInnerRoi()->get_name(), spVoiGo);
    spVoiGo->SetDirty(true);
    spVoiCollection->SetDirty(true);
    return true;
}
TPS_END_NAMESPACE  // end namespace tps
