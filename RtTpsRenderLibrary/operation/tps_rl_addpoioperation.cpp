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
#include "RtTpsRenderLibrary/tps_rl_addpoioperation.h"


//#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_editsetuppoigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"


TPS_BEGIN_NAMESPACE;
TpsAddPoiOperation::TpsAddPoiOperation(
    const std::string &seriesUid, 
    const std::string &poiUid) 
    : mSeriesUid(seriesUid),
    mPoiUid(poiUid) {
}

TpsAddPoiOperation::~TpsAddPoiOperation() {

}

bool TpsAddPoiOperation::ModifyGraphicObject() const 
{

    //GO_TYPE type = mPlanUid == "ADDQAPLANPOI" ? GO_TYPE_QA_TEMP_POI_COLLECTION : 
    //    GO_TYPE_POI_COLLECTION;
    GO_TYPE type = 0;
    std::string sUid("");
    if (mSeriesUid.empty()){
        TPS_LOG_DEV_ERROR<<"Series uid is empty when add poi operation ! poiuid:" <<mPoiUid;
        return false;
    }
    type = GO_TYPE_POI_COLLECTION;
    sUid = mSeriesUid;

    auto go = mModelWarehouse->GetModelObject(
        sUid + "|" + GOTypeHelper::ToString(type));
    if(nullptr == go) {
        TPS_LOG_DEV_ERROR<<"failed to get poi go collection ! uid:" <<sUid;
        return false;
    }
    auto goCollection = dynamic_pointer_cast<TpsPoiGraphicObjectCollection>(go);
    auto converter = mConverterFactory->GetConverter(GO_TYPE_POI);
    if(nullptr == converter) {
        TPS_LOG_DEV_ERROR<<"failed to get poi go converter!";
        return false;
    }
    GOConverterParameter para;
    para.obj_uid = mPoiUid;
    std::shared_ptr<Mcsf::MedViewer3D::IGraphicObject> poi = converter->Convert(para);
    if(nullptr == poi) {
        TPS_LOG_DEV_ERROR<<"failed to convert poi go"<<mPoiUid;
        return false;
    }
    auto poiGo = dynamic_pointer_cast<TpsPoiGraphicObject>(poi);
   
    poiGo->SetVisible(true);
    goCollection->AddPoi(mPoiUid, poiGo);
    goCollection->SetDirty(true);
    return true;
}



TPS_END_NAMESPACE
