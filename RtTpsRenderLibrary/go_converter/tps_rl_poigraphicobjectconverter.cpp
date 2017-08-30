#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectconverter.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobject.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"

TPS_BEGIN_NAMESPACE
PoiGraphicObjectConverter::PoiGraphicObjectConverter(GO_TYPE goType)
    :TpsGOConverterBase(goType), mPoi(nullptr){

}

PoiGraphicObjectConverter::~PoiGraphicObjectConverter() {

}

GraphicObjectPtr PoiGraphicObjectConverter::CreateGraphicObject_i(){
    return std::shared_ptr<TpsPoiGraphicObject>(new TpsPoiGraphicObject());
}

bool PoiGraphicObjectConverter::PrepareSourceData_i(const GOConverterParameter& para){
    mPoi = nullptr;
    mPoi = mDataRepository->GetPoiManager()->GetPOI(para.obj_uid);
    if (mPoi == nullptr){
        TPS_LOG_DEV_ERROR<<"cannot find poi in poi manager,Poi Uid:"<<para.obj_uid;
        return false;
    }
    return true;
}

bool PoiGraphicObjectConverter::Update_i(GraphicObjectPtr go)
{
    auto poiGo = std::dynamic_pointer_cast<TpsPoiGraphicObject>(go);
    if (mPoi == nullptr || poiGo == nullptr){
        return false;
    }

    float color[4] ={0.f};
    //mPoi->GetColor(color);
    color[0] = mPoi->get_red();
    color[1] = mPoi->get_green();
    color[2] = mPoi->get_blue();
    color[3] = mPoi->get_alpha();

    Mcsf::Point3f poiPosition(mPoi->get_coordinate3dx(),
        mPoi->get_coordinate3dy(), mPoi->get_coordinate3dz()); // position in Patient Coordinate
    poiGo->SetPosition(TpsArithmeticConverter::ConvertToPoint3D(poiPosition));
    poiGo->SetColor(color);
    poiGo->SetVisible(mPoi->get_isvisible());
    poiGo->SetPoiType((RtDbDef::POI_TYPE)mPoi->get_type());
    poiGo->SetPoiName(mPoi->get_name());
    return true;
}

MaxPointGraphicObjectConverter::MaxPointGraphicObjectConverter(GO_TYPE goType)
    :PoiGraphicObjectConverter(goType)
{

}

MaxPointGraphicObjectConverter::~MaxPointGraphicObjectConverter()
{

}

bool MaxPointGraphicObjectConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    mPoi = nullptr;
    mPoi = mDataRepository->GetPoiManager()->GetMaxPoint(para.obj_uid);
    if (mPoi == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"MaxPointGraphicObjectConverter cannot find poi in poi manager,Poi Uid:"<<para.obj_uid;
        return false;
    }
    return true;
}
TPS_END_NAMESPACE
