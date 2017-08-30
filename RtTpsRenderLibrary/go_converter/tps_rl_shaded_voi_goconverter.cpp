#include "StdAfx.h"

#pragma warning(disable:4996)
#pragma warning(disable:4702)

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_goconverter.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

TPS_BEGIN_NAMESPACE
ShadedVoiGraphicObjectConverter::ShadedVoiGraphicObjectConverter(
 GO_TYPE goType): TpsGOConverterBase(goType){

}

ShadedVoiGraphicObjectConverter::~ShadedVoiGraphicObjectConverter() {

}

GraphicObjectPtr ShadedVoiGraphicObjectConverter::CreateGraphicObject_i(){
    return std::shared_ptr<ShadedVoiGraphicObject>(
        new ShadedVoiGraphicObject());
}

bool ShadedVoiGraphicObjectConverter::Update_i(GraphicObjectPtr go){

    auto ssGo = std::dynamic_pointer_cast<ShadedVoiGraphicObject>(go);
    if (ssGo == nullptr){
        return false;
    }

	auto displayMng = mDataRepository->GetDisplayStateManager();
	ssGo->SetMprTransparency3D(displayMng->GetMPRTransparency3D());
	ssGo->SetMprSection(displayMng->GetMprSection());
    ssGo->SetDirty(true);

    return true;
}
 TPS_END_NAMESPACE
