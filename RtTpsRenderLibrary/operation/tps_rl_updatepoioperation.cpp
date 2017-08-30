////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_addpoioperation.cpp
/// 
///  \brief class TpsAddPoiOperation
/// 
///  \version 1.0
/// 
///  \date    2014/2/14
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_updatepoioperation.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"

#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"

#include "boost/lexical_cast.hpp"

TPS_BEGIN_NAMESPACE;
TpsUpdatePoiOperation::TpsUpdatePoiOperation(
    const std::string &strSeriesUid,
    const std::string &strPoiUid) : 
    m_strSeriesUid(strSeriesUid),
    m_strPoiUid(strPoiUid)
{

}

TpsUpdatePoiOperation::~TpsUpdatePoiOperation() 
{

}

bool TpsUpdatePoiOperation::ModifyGraphicObject() const
{
    TpsPOIManager *poiManager = mDataRepository->GetPoiManager();
    RtPoi *poi = poiManager->GetPOI(m_strPoiUid);
    if(nullptr == poi) {
        TPS_LOG_DEV_ERROR<<"Failed to get poi from poi manager!";
        return false;
    }

    GO_TYPE type = 0;
    std::string sUid("");
    if (m_strSeriesUid.empty()){
        TPS_LOG_DEV_ERROR<<"failed to get series uid when update poi operation!";
        return false;
    }
    type = GO_TYPE_POI_COLLECTION;
    sUid = m_strSeriesUid;

    auto go = mModelWarehouse->GetModelObject(sUid + "|" + GOTypeHelper::ToString(type));
    if(nullptr == go) {
        TPS_LOG_DEV_ERROR<<"failed to get poi go collection! series uid:" << m_strSeriesUid;
        return false;
    }
    auto goCollection = dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(go);
    auto poiGo = goCollection->SelectPoi(m_strPoiUid);
    if(nullptr == poiGo) {
        poiGo = std::shared_ptr<TpsPoiGraphicObject>(new TpsPoiGraphicObject());
        goCollection->AddPoi(m_strPoiUid, poiGo);
    }

    float color[4] ={0.f};
    //mPoi->GetColor(color);
    color[0] = poi->get_red();
    color[1] = poi->get_green();
    color[2] = poi->get_blue();
    color[3] = poi->get_alpha();

    poiGo->SetVisible(poi->get_isvisible());
    Mcsf::Point3f poi3f(poi->get_coordinate3dx(),
        poi->get_coordinate3dy(), poi->get_coordinate3dz());
    poiGo->SetPosition(TpsArithmeticConverter::ConvertToPoint3D(poi3f));
    poiGo->SetColor(color);
    poiGo->SetPoiName(poi->get_name());
    poiGo->SetPoiType((RtDbDef::POI_TYPE)poi->get_type());
    goCollection->SetDirty(true);

    return true;
}

TpsUpdateMaxPointOperation::TpsUpdateMaxPointOperation(
    const std::string& strPlanUid,
    const std::string& strPoiUid) : 
    m_strPlanUid(strPlanUid),
    m_strPoiUid(strPoiUid)
{

}

TpsUpdateMaxPointOperation::~TpsUpdateMaxPointOperation() 
{

}

bool TpsUpdateMaxPointOperation::ModifyGraphicObject() const
{
    RtPoi *poi = mDataRepository->GetPoiManager()->GetMaxPoint(m_strPoiUid);
    if(nullptr == poi) {
        TPS_LOG_DEV_ERROR<<"TpsUpdateMaxPointOperation Failed to get poi from poi manager!"<<m_strPoiUid;
        return false;
    }

    if (m_strPlanUid.empty()){
        TPS_LOG_DEV_ERROR<<"TpsUpdateMaxPointOperation failed to get plan uid when update poi operation!";
        return false;
    }

    auto go = mModelWarehouse->GetModelObject(m_strPlanUid + "|" + GOTypeHelper::ToString(GO_TYPE_MAX_POINT_COLLECTION));
    if(nullptr == go) {
        TPS_LOG_DEV_ERROR<<"failed to get poi go collection! plan uid:" << m_strPlanUid;
        return false;
    }
    auto goCollection = dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(go);
    auto poiGo = goCollection->SelectPoi(m_strPoiUid);
    if(nullptr == poiGo) {
        poiGo = std::shared_ptr<TpsPoiGraphicObject>(new TpsPoiGraphicObject());
        goCollection->AddPoi(m_strPoiUid, poiGo);
    }

    float color[4] ={0.f};
    //mPoi->GetColor(color);
    color[0] = poi->get_red();
    color[1] = poi->get_green();
    color[2] = poi->get_blue();
    color[3] = poi->get_alpha();

    poiGo->SetVisible(poi->get_isvisible());
    Mcsf::Point3f poi3f(poi->get_coordinate3dx(), poi->get_coordinate3dy(), poi->get_coordinate3dz());
    poiGo->SetPosition(TpsArithmeticConverter::ConvertToPoint3D(poi3f));
    poiGo->SetColor(color);
    poiGo->SetPoiName(poi->get_name());
    goCollection->SetDirty(true);

    return true;
}
TPS_END_NAMESPACE
