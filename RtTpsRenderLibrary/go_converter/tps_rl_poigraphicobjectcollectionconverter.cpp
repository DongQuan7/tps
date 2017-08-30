#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollectionconverter.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "boost/lexical_cast.hpp"


TPS_BEGIN_NAMESPACE
PoiGraphicObjectCollectionConverter::PoiGraphicObjectCollectionConverter(
    GO_TYPE goType, PoiGraphicObjectConverter* converter) 
     : TpsGOConverterBase(goType),mPoiGoConverter(converter){

}

PoiGraphicObjectCollectionConverter::~PoiGraphicObjectCollectionConverter() {
    if (mPoiGoConverter){
        delete mPoiGoConverter;
        mPoiGoConverter = nullptr;
    }
}

void PoiGraphicObjectCollectionConverter::SetModelWarehouse(ModelWarehouse* modelWarehouse){
    mModelWarehouse = modelWarehouse;

    if (mPoiGoConverter){
        mPoiGoConverter->SetModelWarehouse(modelWarehouse);
    }
}

void PoiGraphicObjectCollectionConverter::SetDataRepository(ITpsDataRepository* dataRepository){
    mDataRepository = dataRepository;

    if (mPoiGoConverter){
        mPoiGoConverter->SetDataRepository(dataRepository);
    }
}

GraphicObjectPtr PoiGraphicObjectCollectionConverter::CreateGraphicObject_i(){
    return std::shared_ptr<TpsPoiGraphicObjectCollection>(
        new TpsPoiGraphicObjectCollection());
}

bool PoiGraphicObjectCollectionConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;

    mPOIList = mDataRepository->GetPoiManager()->GetPoiUidListBySeriesUid(it->second);
    if (mPOIList.empty()){//phantom poi
        std::vector<RtPoi*> vList = mDataRepository->GetPoiManager()->GetAllPoiInSeries(it->second);
        for (auto iter=vList.cbegin(); iter!=vList.cend(); ++iter){
            mPOIList.push_back((*iter)->get_uid());
        }
    }
    return true;
}

bool PoiGraphicObjectCollectionConverter::Update_i(GraphicObjectPtr go)
{
    auto collection = std::dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(go);
    if (collection == nullptr || mPoiGoConverter == nullptr)
    {
        return false;
    }

    collection->RemoveAll();

    GOConverterParameter para;
    for(auto it = mPOIList.begin(); it != mPOIList.end() ; ++it)
    {
        para.obj_uid = *it;
        GraphicObjectPtr poiGo = mPoiGoConverter->Convert(para);
        if(nullptr == poiGo)
        {
            TPS_LOG_DEV_ERROR<<"failed to get convert poi to poi go";
            return false;
        }
        collection->AddPoi(para.obj_uid,std::dynamic_pointer_cast<TpsPoiGraphicObject>(poiGo));
    }
    return true;
}

//MaxPointGraphicObjectCollectionConverter
MaxPointGraphicObjectCollectionConverter::MaxPointGraphicObjectCollectionConverter(
    GO_TYPE goType, MaxPointGraphicObjectConverter* converter) 
    : PoiGraphicObjectCollectionConverter(goType, converter)
{

}

MaxPointGraphicObjectCollectionConverter::~MaxPointGraphicObjectCollectionConverter() {
    if (mPoiGoConverter){
        delete mPoiGoConverter;
        mPoiGoConverter = nullptr;
    }
}

bool MaxPointGraphicObjectCollectionConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(PLAN);
    if (it == para.data_source_map.end()) return false;
    mPOIList.clear();
    RtPoi* poi = mDataRepository->GetPoiManager()->GetMaxPoint(it->second);
    if (poi != nullptr)
    {
        mPOIList.push_back(poi->get_uid());
    }
    return true;
}
TPS_END_NAMESPACE
