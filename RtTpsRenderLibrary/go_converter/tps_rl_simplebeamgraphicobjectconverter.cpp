//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_rl_simplebeamgraphicobjectconverter.cpp
///  \brief  
///  \
///  class  TpsSimpleBeamGraphicObjectConverter
///  \version 1.0
///  \date  2014/11/24
///  \
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_simplebeamgraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_simplebeamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
//#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"



TPS_BEGIN_NAMESPACE

TpsSimpleBeamGraphicObjectConverter::TpsSimpleBeamGraphicObjectConverter(
    GO_TYPE goType) : TpsGOConverterBase(goType),mSimpleBeam(nullptr){

}

TpsSimpleBeamGraphicObjectConverter::~TpsSimpleBeamGraphicObjectConverter(){

}

GraphicObjectPtr TpsSimpleBeamGraphicObjectConverter::CreateGraphicObject_i(){
    return std::shared_ptr<TpsSimpleBeamGraphicObject>(new TpsSimpleBeamGraphicObject());
}

bool TpsSimpleBeamGraphicObjectConverter::PrepareSourceData_i(const GOConverterParameter& /*para*/){
    mSimpleBeam= nullptr;

    //TpsBeamManager* beamManager = mDataRepository->GetBeamManager();
    //mBeam = beamManager->GetBeam(uid);
    //if (mBeam == nullptr){
    //    TPS_LOG_DEV_ERROR<<"Failed to get beam: "<<uid;
    //    return false;
    //}

    return true;
}

bool TpsSimpleBeamGraphicObjectConverter::Update_i(GraphicObjectPtr go){
    using namespace Mcsf::MedViewer3D;
    std::shared_ptr<TpsSimpleBeamGraphicObject> beamGo = 
        std::dynamic_pointer_cast<TpsSimpleBeamGraphicObject>(go);
    if (mSimpleBeam == nullptr || beamGo == nullptr){
        return false;
    }

    //beamGo->RemoveAllSegments();
    //beamGo->SetIsJoined(mSimpleBeam->GetIsJoined());
    //beamGo->SetColorForMprWindow(mSimpleBeam->GetColor());
    //beamGo->SetColorForBevWindow(mSimpleBeam->GetColor());
    //beamGo->SetVisible(mSimpleBeam->GetIsVisible());

    //BEAM_TYPE type = mBeam->GetBeamType();
    //if(type == BURST_ARC || VMAT == type) {
    //    beamGo->SetBeamType(TpsSimpleBeamGraphicObject::ARC);
    //}
    //else {
    //    beamGo->SetBeamType(TpsSimpleBeamGraphicObject::IMRT);
    //}

    return true;
}
TPS_END_NAMESPACE