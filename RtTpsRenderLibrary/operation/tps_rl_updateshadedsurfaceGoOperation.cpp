////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  wenqiao.jiang  wenqiao.jiang@united-imaging.com
/// 
///  \file tps_rl_updateshadedsurfaceGoOperation.cpp
/// 
///  \brief Update ShadedSurface Go 
/// 
///  \version 1.0
/// 
///  \date    2016/03/23
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_updateshadedsurfaceGoOperation.h"

#include "RtTpsDataAccess/tps_da_voimanager.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobejctconverter.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsUpdateShadeSurfaceOperation::TpsUpdateShadeSurfaceOperation(
const std::string& seriesUid)
: mSeriesUid(seriesUid){
}

TpsUpdateShadeSurfaceOperation::~TpsUpdateShadeSurfaceOperation(){

}

bool TpsUpdateShadeSurfaceOperation::ModifyGraphicObject() const{

    auto surfaceGo = mModelWarehouse->GetModelObject(
        mSeriesUid + "|" + GOTypeHelper::ToString(GO_TYPE_SHADEDSURFCE));
    auto surface = dynamic_pointer_cast<ShadedVoiGraphicObject>(surfaceGo);
    if (nullptr == surfaceGo || nullptr == surface) {
        //why warning? Because no surface window sometimes
        TPS_LOG_DEV_WARNING << "failed to get surface go!" << mSeriesUid;
        return true;
    }

    auto converterShadedSurface = mConverterFactory->GetConverter(GO_TYPE_SHADEDSURFCE);
    if (converterShadedSurface == nullptr){
        TPS_LOG_DEV_ERROR << "Failed get shaded surface graphic object converter.";
        return false;
    }

    //TODO just add one item to GO
    GOConverterParameter para;
    para.data_source_map[IMAGE] = mSeriesUid;
    if (!converterShadedSurface->Update(para, surfaceGo)){
        TPS_LOG_DEV_ERROR << "The cache collection converter does not exist.";
        return false;
    }

    return true;
}
TPS_END_NAMESPACE