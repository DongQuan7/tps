////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  wenlong zhao wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_setisocenter_operation.cpp 
/// 
///  \brief class TpsAddPoiOperation
/// 
///  \version 1.0
/// 
///  \date    2015/07/18
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_setnormpoint_operation.h"


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
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_rtimage.h"


TPS_BEGIN_NAMESPACE
TpsEasyPlanSetNormPointOperation::TpsEasyPlanSetNormPointOperation(const std::string &planUid,const std::string &poiUid, const std::string &imageUid,
int displayWidth, int displayHeight) : mPlanUid(planUid),
    mPoiUid(poiUid), mImageUid(imageUid), mWidth(displayWidth), mHeight(displayHeight) {
}

TpsEasyPlanSetNormPointOperation::~TpsEasyPlanSetNormPointOperation() {

}

bool TpsEasyPlanSetNormPointOperation::ModifyGraphicObject() const {

    auto go = mModelWarehouse->GetModelObject(
        mPlanUid + "|" + GOTypeHelper::ToString(GO_TYPE_POI_COLLECTION));
    if(nullptr == go) {
        TPS_LOG_DEV_ERROR<<"failed to get poi go collection ! plan uid:" <<mPlanUid;
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

    // if the new point is an isocenter poi, then delete the previous isocenter poi.
    // I did this because the actual isocenter UID sent here is different everytime, 
    // so based on the fact that every plan 
    goCollection->DeletePoi(mPoiUid);
    goCollection->AddPoi(mPoiUid, poiGo);
    goCollection->SetDirty(true);
    return true;
}



TPS_END_NAMESPACE