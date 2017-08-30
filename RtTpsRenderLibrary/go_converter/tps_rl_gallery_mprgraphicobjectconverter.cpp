//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rl_gallery_mprgraphicobjectconverter.cpp
///  \brief   
///
///  \version 1.0
///  \date    Nov. 24, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"   

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_mpr_api.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"

#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_mprgraphicobjectconverter.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"


#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsGalleryMPRGOConverter::TpsGalleryMPRGOConverter(GO_TYPE goType)
: TpsMPRGOConverter(goType){
}

TpsGalleryMPRGOConverter::~TpsGalleryMPRGOConverter(){

}


bool TpsGalleryMPRGOConverter::PrepareSourceData_i(const GOConverterParameter& para){
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;

    mVolume = nullptr;

    std::string volumeKey = it->second + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
    auto go = mModelWarehouse->GetModelObject(volumeKey);
    mVolume = std::dynamic_pointer_cast<GraphicObjVolume>(go);
    if (mVolume == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][TpsMPRGOConverter][PrepareSourceData_i]: "
            <<"The volume data with UID: "<<it->second<<" does not exist.";
        return false;
    }
    TpsImage3DEntity* imageData = nullptr;
    if (!mDataRepository->GetGalleryImageManager()->GetImage3DData(it->second, &imageData)){
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d data which UID: "<<it->second;
        return nullptr;
    }

    RtImage3DHeader* header;
    if (!imageData->GetImage3DHeader(header)){
        TPS_LOG_DEV_ERROR<<"Failed to get current image 3d header.";
        return nullptr;
    }
    std::string sPP(header->m_cPatientPosition);
    boost::to_upper(sPP);
    bool isHF = sPP == "HFS" || sPP == "HFP" || sPP == "HFDR" || sPP == "HFDL";
    MPR_VIEW_DIRECTION axialDirt = isHF ? F2H : H2F;
    mGOSectionToPatienCSAxisMap[AXIAL] = axialDirt;
    return true;
}

bool TpsGalleryMPRGOConverter::Update_i(GraphicObjectPtr go){
    if(TpsMPRGOConverter::Update_i(go))
    {
        mVolume = nullptr;
        return true;
    }
    return false;
}

TPS_END_NAMESPACE  // end namespace tps
