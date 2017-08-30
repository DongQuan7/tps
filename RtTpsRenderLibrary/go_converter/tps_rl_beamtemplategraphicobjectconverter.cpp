//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_rl_beamtemplategraphicobjectconverter.h 
///  \brief  
///  \
///  class  TpsBeamTemplateGraphicObjectConverter
///  \version 1.0
///  \date  2014/11/21
///  \
//////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"

//#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_simplebeamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_simplebeamgraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_beamtemplategraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamtemplategraphicobjectconverter.h"
#include "RtTpsDataAccess/tps_da_configuremanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsFramework/tps_fw_beamstructure.h"
#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"


TPS_BEGIN_NAMESPACE
TpsBeamTemplateGraphicObjectConverter::TpsBeamTemplateGraphicObjectConverter(
        GO_TYPE goType): TpsGOConverterBase(goType){

}

TpsBeamTemplateGraphicObjectConverter::~TpsBeamTemplateGraphicObjectConverter(){

}

void TpsBeamTemplateGraphicObjectConverter::SetModelWarehouse(ModelWarehouse* modelWarehouse){
    mModelWarehouse = modelWarehouse;

    if (mSimpleBeamGoConverter){
        mSimpleBeamGoConverter->SetModelWarehouse(modelWarehouse);
    }
}

void TpsBeamTemplateGraphicObjectConverter::SetDataRepository(ITpsDataRepository* dataRepository){
    mDataRepository = dataRepository;

    if (mSimpleBeamGoConverter){
        mSimpleBeamGoConverter->SetDataRepository(dataRepository);
    }
}

GraphicObjectPtr TpsBeamTemplateGraphicObjectConverter::CreateGraphicObject_i(){
    return std::shared_ptr<TpsBeamTemplateGraphicObject>(new TpsBeamTemplateGraphicObject());
}

bool TpsBeamTemplateGraphicObjectConverter::PrepareSourceData_i(const GOConverterParameter& para){

    mDataRepository->GetConfigurationManager()->GetBeamTemplate(para.obj_uid, &mBeamTemplate);

    return true;
}
//
//bool TpsBeamTemplateGraphicObjectConverter::PrepareSourceData_i(const std::string& combined){
//    int pos = combined.find_first_of('¨ˆ');
//    std::string imguId = combined.substr(0, pos-1);
//    std::string templateName = combined.substr(pos+1, combined.size()-1);
//
//    mDataRepository->GetConfigurationManager()->GetBeamTemplate(templateName, mBeamTemplate);
//
//    // we have to get NormGroup so that we know the BeamType(Technique), RadiationType
//    // because we need compute BeamToPatient Matrix according to different 
//    // it's not likely we can get NormGroup at this moment, we can set a default BeamType
//    
//
//
//    TpsImage3DEntity* image = nullptr;
//    if (!mDataRepository->GetImageDataManager()->GetImage3DData(imguId, &image) || image == nullptr){
//        TPS_LOG_DEV_ERROR<<"Failed to get image 3d data with uid: "<<imguId;
//        return false;
//    }
//    mImage = image;
//
//    
//
//    return true;
//}

bool TpsBeamTemplateGraphicObjectConverter::Update_i(GraphicObjectPtr go){
    std::shared_ptr<TpsBeamTemplateGraphicObject> beamTemplateGo = 
        std::dynamic_pointer_cast<TpsBeamTemplateGraphicObject>(go);

    if(beamTemplateGo == nullptr /*|| mSimpleBeamGoConverter == nullptr*/){
        return false;
    }

    beamTemplateGo->InitJawPosition();
    beamTemplateGo->InitLeafPosition();


    //RtMachine* machine = nullptr;
    //if (!mDataRepository->GetMachineManager()->GetMachine(normGroup->GetMachineUID(), machine)){
    //    TPS_LOG_DEV_ERROR<<"Failed to get machine.";
    //    return false;
    //}

    for(auto it = mBeamTemplate->mBeamCollection.begin(); 
        it != mBeamTemplate->mBeamCollection.end(); ++it){
            //simple_beam = (SimpleBeam *)&(*it);

            GraphicObjectPtr sGo = 
                std::shared_ptr<TpsSimpleBeamGraphicObject>(new TpsSimpleBeamGraphicObject());

            if (sGo == nullptr){
                return false;
            }

            std::shared_ptr<TpsSimpleBeamGraphicObject> simpleBeamGo = 
                std::dynamic_pointer_cast<TpsSimpleBeamGraphicObject>(sGo);
            if(simpleBeamGo == nullptr){
                return false;
            }

            simpleBeamGo->SetGantryStartAngle(it->mGantryStartAngle);
            simpleBeamGo->SetGantryArcLength(it->mArcLength);
            simpleBeamGo->SetCollimatorAngle(it->mCollimatorAngle);
            simpleBeamGo->SetCouchAngle(it->mCouchAngle);
            simpleBeamGo->SetGantryIsClockWise(it->mIsClockwise);

            
            beamTemplateGo->AddBeamGo(simpleBeamGo);

    }

    return true;
}
TPS_END_NAMESPACE