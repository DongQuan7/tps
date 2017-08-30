////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_voigraphicobejctcollectionconverter.cpp
/// 
///  \brief class TpsVOIGOCollectionConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/01/20
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobejctcollectionconverter.h"

#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobejctconverter.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobject.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsVOIGOCollectionConverter::TpsVOIGOCollectionConverter(
    GO_TYPE goType, TpsVOIGOConverter* converter)
    : TpsGOConverterBase(goType), mVOIConverter(converter)
{

}

TpsVOIGOCollectionConverter::~TpsVOIGOCollectionConverter()
{

    if (mVOIConverter){
        delete mVOIConverter;
        mVOIConverter = nullptr;
    }
}

void TpsVOIGOCollectionConverter::SetModelWarehouse(ModelWarehouse* modelWarehouse)
{
    mModelWarehouse = modelWarehouse;
    if (mVOIConverter){
        mVOIConverter->SetModelWarehouse(modelWarehouse);
    }
}

void TpsVOIGOCollectionConverter::SetDataRepository(ITpsDataRepository* dataRepository)
{
    mDataRepository = dataRepository;
    if (mVOIConverter){
        mVOIConverter->SetDataRepository(dataRepository);
    }
}

GraphicObjectPtr TpsVOIGOCollectionConverter::CreateGraphicObject_i()
{
    return std::shared_ptr<TpsVOIGOCollection>(new TpsVOIGOCollection());
}

bool TpsVOIGOCollectionConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;

    mImage = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(
        it->second, &mImage) || mImage == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The fixed image does not exist which uid: "<<it->second;
        return false;
    }

    mRoiList = mDataRepository->GetVoiManager()->GetROIBySeries(it->second);
    return true;
}

bool TpsVOIGOCollectionConverter::Update_i(GraphicObjectPtr go)
{
    auto voiGOCollection = std::dynamic_pointer_cast<TpsVOIGOCollection>(go);
    if (mImage == nullptr || voiGOCollection == nullptr || mVOIConverter == nullptr){
        return false;
    }

    voiGOCollection->RemoveAllVOI();
    RtImage3DHeader* header = nullptr;
    if (!mImage->GetImage3DHeader(header) || header == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d header.";
        return false;
    }
    mVOIConverter->SetSliceCount(header->m_iSliceCount);

    GraphicObjectPtr voiGO = nullptr;
    for (int i=0; i<mRoiList.size(); ++i)
    {
        mVOIConverter->SetVoi(mRoiList[i]);
        voiGO = mVOIConverter->Convert();
        if (voiGO == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to convert voi to roi graphic object.";
            return false;
        }

        voiGOCollection->AddVOI(mRoiList[i]->GetUid(), 
            std::dynamic_pointer_cast<TpsVOIGraphicObject>(voiGO));
    }
    return true;
}

TPS_END_NAMESPACE  // end namespace tps