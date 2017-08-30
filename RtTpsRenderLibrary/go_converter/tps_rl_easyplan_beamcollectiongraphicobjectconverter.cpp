////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_segmentcollectiongraphicobjectconverter.cpp
/// 
///  \brief class TpsEasyPlanBeamCollectionGOConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/25
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_easyplan_beamcollectiongraphicobjectconverter.h"

#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"

#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

//TpsEasyPlanBeamCollectionGOConverter::TpsEasyPlanBeamCollectionGOConverter(
//    GO_TYPE goType, TpsEasyPlanSegmentGOConverter* segmentConverter)
//    : TpsGOConverterBase(goType), mSegmentConverter(segmentConverter), mNormGroupUID(""){
//
//}

TpsEasyPlanBeamCollectionGOConverter::TpsEasyPlanBeamCollectionGOConverter( GO_TYPE goType ): TpsGOConverterBase(goType){

}

TpsEasyPlanBeamCollectionGOConverter::~TpsEasyPlanBeamCollectionGOConverter(){

    //if (mSegmentConverter){
    //    delete mSegmentConverter;
    //    mSegmentConverter = nullptr;
    //}
}

GraphicObjectPtr TpsEasyPlanBeamCollectionGOConverter::CreateGraphicObject_i(){

    return std::shared_ptr<TpsBeamGraphicObjectCollection>(
        new TpsBeamGraphicObjectCollection());
}

bool TpsEasyPlanBeamCollectionGOConverter::Update_i(GraphicObjectPtr go){

	go;
    //std::shared_ptr<TpsBeamGraphicObjectCollection> segmentCollection = 
    //    std::dynamic_pointer_cast<TpsBeamGraphicObjectCollection>(go);
    //if (segmentCollection == nullptr || mSegmentConverter == nullptr){
    //    return false;
    //}

    //std::vector<std::string> segmentList = 
    //    TpsNormGroupManager::GetInstance()->GetBeamListByNormGroup(mNormGroupUID);

    ////segmentCollection->SetIsJoined(mNormalGroup->GetIsJoined());
    //segmentCollection->RemoveAll();

    //GraphicObjectPtr segment = nullptr;
    //std::string segmentUID;
    //for (auto it = segmentList.begin(); it != segmentList.end(); ++it){
    //    segmentUID = (*it);
    //    segment = mSegmentConverter->Convert(segmentUID);
    //    if (segment == nullptr){
    //        TPS_LOG_DEV_ERROR<<"Failed to convert segment to segment GO, which uid: "<<segmentUID;
    //        return false;
    //    }
    //    segmentCollection->AddSegment(segmentUID, std::dynamic_pointer_cast<TpsSegmentGraphicObj>(segment));
    //}
    return true;
}
TPS_END_NAMESPACE  // end namespace tps