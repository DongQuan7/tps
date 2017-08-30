////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_query_current_mpr_slice_operation.cpp
/// 
///  \brief class QueryMprSliceInfo implementation 
/// 
///  \version 1.0
/// 
///  \date    2017/02/13
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_query_current_mpr_slice_operation.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

QueryMprSliceInfoOperation::QueryMprSliceInfoOperation(
    const std::string& seriesUID, FIRST_POSTFIX_COMPONENT section, bool isViewOrder): 
    mSeriesUID(seriesUID), mSectionType(section), mIsViewOrder(isViewOrder)
{

}

QueryMprSliceInfoOperation::~QueryMprSliceInfoOperation()
{

}

bool QueryMprSliceInfoOperation::ModifyGraphicObject() const
{
    TpsImage3DEntity* image = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(mSeriesUID, &image) || image == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d entity.";
        return false;
    }
    int startSliceZ = image->GetStartSliceNumber();

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType);
    std::string key = mSeriesUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(key);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The mpr graphic object is empty.";
        return false;
    }

    std::string volumeKey = mSeriesUID + "|"+ GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(goVolume);
    if(volume == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get volume go!"<<mSeriesUID;
        return false;
    }
    if (!mIsViewOrder)
    {
        TpsMprHelper::PlaneTypeInVol planeType;
        TpsMprHelper::GetSliceNumber(volume, mpr, mCurrentSliceNumber, mTotalSliceNumber, planeType);
        mStartSliceNumber = planeType == TpsMprHelper::Z ? startSliceZ : 1;
    }
    else
    {
        TpsMprHelper::GetSliceNumberInViewOrder(volume, mpr, mCurrentSliceNumber, mTotalSliceNumber);
    }
    return true;
}

void QueryMprSliceInfoOperation::GetSliceInfo(int& current, int& total, int& start) const
{
    current = mCurrentSliceNumber;
    total = mTotalSliceNumber;
    start = mStartSliceNumber;
}
TPS_END_NAMESPACE  // end namespace tps
