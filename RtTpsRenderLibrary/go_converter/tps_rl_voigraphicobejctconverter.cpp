////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_voigraphicobejctconverter.cpp
/// 
///  \brief class TpsVOIGOConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/01/20
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobejctconverter.h"

#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobject.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "RtTpsFramework/tps_fw_signed_distance_field.h"
#include "RtTpsFramework/tps_fw_contour_set.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsVOIGOConverter::TpsVOIGOConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType), mSliceCount(0){

}

TpsVOIGOConverter::~TpsVOIGOConverter(){

}

GraphicObjectPtr TpsVOIGOConverter::CreateGraphicObject_i(){
    return std::shared_ptr<TpsVOIGraphicObject>(new TpsVOIGraphicObject());
}

bool TpsVOIGOConverter::PrepareSourceData_i(const GOConverterParameter& para){
    mRoi = mDataRepository->GetVoiManager()->GetROI(para.obj_uid);
    if (mRoi == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to ROI entity with UID: "<<para.obj_uid;
        return false;
    }

    std::string imageUID = mRoi->GetInnerRoi()->get_seriesuid();
    TpsImage3DEntity* image = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(imageUID, &image) || 
        image == nullptr){
        TPS_LOG_DEV_ERROR<<"The image data with uid: "<<imageUID<<" does not exist.";
        return false;
    }

    RtImage3DHeader *header = nullptr;
    image->GetImage3DHeader(header);
    if (header == nullptr){
        TPS_LOG_DEV_ERROR<<"Image header is empty.";
        return false;
    }
    mSliceCount = header->m_iSliceCount;
    return true;
}

bool TpsVOIGOConverter::Update_i(GraphicObjectPtr go)
{
    auto voiGO = std::dynamic_pointer_cast<TpsVOIGraphicObject>(go);
    if (mRoi == nullptr || mSliceCount == 0 || voiGO == nullptr){
        return false;
    }

    voiGO->SetSliceCount(mSliceCount);
    auto innerRoi = mRoi->GetInnerRoi();

    voiGO->SetHasInterpolate(true);
    bool *inters = nullptr;
    voiGO->GetInterpolateFlags(inters);
    auto interFlags = innerRoi->get_interpolate();
    if(interFlags.empty()) 
    {
        for(int i = 0; i < mSliceCount; ++i) 
        {
            interFlags.push_back(true);
        }
        innerRoi->set_interpolate(interFlags);
    }
    //×¢Òâstd::vector<bool>ÌØ»¯
    for (int i = 0; i < mSliceCount; ++i)
    {
        inters[i] = interFlags[i];
    }

    int slices[2] = {-1, -1};
    voiGO->SetSlices(slices, false);
    voiGO->SetVisibility(innerRoi->get_isvisible());
    voiGO->SetIsShaded(innerRoi->get_isshaded());
    voiGO->SetAlpha3D(innerRoi->get_alpha3d());
    const float* color = innerRoi->get_color();
    voiGO->SetColor(const_cast<float*>(color));
    voiGO->SetMeshDataDirty(true);

    *voiGO->GetSDF() = *mRoi->GetSDF();
    *voiGO->GetContourSet() = *mRoi->GetContourSet();
    return true;
}
TPS_END_NAMESPACE  // end namespace tps
