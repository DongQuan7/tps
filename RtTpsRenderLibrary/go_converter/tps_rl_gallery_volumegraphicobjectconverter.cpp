////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rl_gallery_cornerinfographicobjectconverter.cpp
///  \brief   
///
///  \version 1.0
///
///  \date    Dec. 29, 2014
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_volumegraphicobjectconverter.h"

#include "McsfMedViewer3DDataRepresentation/image_data.h"
#include "McsfMedViewer3DDataRepresentation/data_config.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_volume_api.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"

#include "RtTpsFramework/tps_fw_arithmeticconverter.h"


//#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

//using namespace Mcsf::MedViewer3D;
TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsGalleryVolumeGOConverter::TpsGalleryVolumeGOConverter(GO_TYPE goType) 
    : TpsVolumeGOConverter(goType)
{

}

TpsGalleryVolumeGOConverter::~TpsGalleryVolumeGOConverter()
{

}

bool TpsGalleryVolumeGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;

    mImage = nullptr;
    if (!mDataRepository->GetGalleryImageManager()->GetImage3DData(it->second, &mImage))
    {
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d data with UID: "<<it->second;
        return false;
    }
    return true;
}

bool TpsGalleryVolumeGOConverter::Update_i(GraphicObjectPtr go)
{
    if (TpsVolumeGOConverter::Update_i(go))
    {
        auto volume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(go);
        size_t tempBr[3] = {32,32,32};
        if (!volume->SetBrickSizeForVolume(tempBr))
        {
            TPS_LOG_DEV_ERROR<<"Failed to SetBrickSizeForVolume with UID: "<<mImage;
            return false;
        }
        return true;
    }
    return false;
}

TpsGalleryMaskGOConverter::TpsGalleryMaskGOConverter(GO_TYPE goType)
    : TpsMaskGOConverter(goType)
{

}

TpsGalleryMaskGOConverter::~TpsGalleryMaskGOConverter()
{

}

bool TpsGalleryMaskGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;

    TpsImage3DEntity* image;
    if (!mDataRepository->GetGalleryImageManager()->GetImage3DData(it->second, &image))
    {
        TPS_LOG_DEV_ERROR<<"Image 3d entity with UID: "<<para.obj_uid << " does not exist.";
        return false;
    }
    return true;
}
TPS_END_NAMESPACE  // end namespace tps
