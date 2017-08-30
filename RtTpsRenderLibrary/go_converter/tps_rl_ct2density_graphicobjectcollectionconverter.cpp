////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Qiangqiang ZHOU qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_rl_ct2density_graphicobjectconverter.cpp
/// 
///  \brief class Ct2DensityGOConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    July 24, 2014
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_ct2density_graphicobjectcollectionconverter.h"
#include "RtTpsRenderLibrary/tps_rl_ct2density_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_ct2densitygocollection.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_ct2density.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"

TPS_BEGIN_NAMESPACE


Ct2DensityGOCollectionConverter::Ct2DensityGOCollectionConverter(GO_TYPE goType):
    TpsGOConverterBase(goType) 
{

}

Ct2DensityGOCollectionConverter::~Ct2DensityGOCollectionConverter() 
{

}

GraphicObjectPtr Ct2DensityGOCollectionConverter::CreateGraphicObject_i(){
    return std::shared_ptr<Ct2DensityGraphicObejctCollection>(
        new Ct2DensityGraphicObejctCollection());
}

bool Ct2DensityGOCollectionConverter::Update_i(GraphicObjectPtr goCollection){
    auto ct2densitycollectionGO = std::dynamic_pointer_cast<Ct2DensityGraphicObejctCollection>(goCollection);
    if (ct2densitycollectionGO == nullptr) return false;

    MapOfCT2DensityTable *ct2DensityMap = mDataRepository->GetImageDataManager()->GetSystemCT2DensityTables();
    float ct2denMap[6025];

    for (auto iter = ct2DensityMap->begin(); iter != ct2DensityMap->end(); ++iter)
    {
        auto go = std::shared_ptr<Ct2DensityGraphicObejct>(
            new Ct2DensityGraphicObejct());
        go->SetCT2DensityTableUid(iter->first);
        const float* sysct2denMap = (iter->second)->get_ct2densitybuffer();
        if (nullptr == sysct2denMap) {
            TPS_LOG_DEV_ERROR<<"Invalid ct2density map in imagedata manager";
            continue;
        }
        memcpy(ct2denMap, sysct2denMap, sizeof(float)*6025);
        go->SetCt2Density(ct2denMap, 6025);
        ct2densitycollectionGO->AddCt2DensityGo(go);
    }
    return true;
}

TPS_END_NAMESPACE
