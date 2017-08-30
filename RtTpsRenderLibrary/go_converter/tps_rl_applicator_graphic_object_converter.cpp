////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_applicator_graphic_object_converter.cpp
/// 
///  \brief class ApplicatorGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/03/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_applicator_graphic_object_converter.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_applicator_graphic_object.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_applicator.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

ApplicatorGOConverter::ApplicatorGOConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType), mApplicator(nullptr){

}

ApplicatorGOConverter::~ApplicatorGOConverter(){

}

GraphicObjectPtr ApplicatorGOConverter::CreateGraphicObject_i(){

    return std::shared_ptr<ApplicatorGraphicObject>(new ApplicatorGraphicObject());
}

bool ApplicatorGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    mApplicator = nullptr;
    if (!mDataRepository->GetMachineManager()->GetApplicator(para.obj_uid, mApplicator) || mApplicator == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get  applicator with uid: "<<para.obj_uid;
        return false;
    }
    return true;
}

bool ApplicatorGOConverter::Update_i(GraphicObjectPtr go){
    auto applicator = std::dynamic_pointer_cast<ApplicatorGraphicObject>(go);
    if (applicator == nullptr || mApplicator == nullptr){
        return false;
    }

    applicator->SetName(mApplicator->get_name());
    auto shape = mApplicator->get_applicatorapertureshape();
    applicator->SetShapeType(RtDbDef::APPLICATOR_APERTURE_SHAPE(shape));
    if (shape == RtDbDef::SYM_CIRCULAR || shape == RtDbDef::SYM_SQUARE){
        applicator->SetOpening(mApplicator->get_applicatoropening());
    }
    else if (shape == RtDbDef::SYM_RECTANGLE){
        applicator->SetOpeningX(mApplicator->get_applicatoropeningx());
        applicator->SetOpeningY(mApplicator->get_applicatoropeningy());
    }
    else {
        TPS_LOG_DEV_ERROR<<"Unrecognized applicator aperture type.";
        return false;
    }
    return true;
}
TPS_END_NAMESPACE  // end namespace tps
