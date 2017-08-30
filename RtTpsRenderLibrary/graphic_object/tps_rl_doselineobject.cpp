
////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  liu linqi  linqi.liu@united-imaging.com
/// 
///  \file tps_rl_doselineobject.h
/// 
///  \brief class TpsDoseLineobject declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/08/04
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_doselineobject.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"



TPS_BEGIN_NAMESPACE

TpsDoseLineGraphicObject::TpsDoseLineGraphicObject()
: mDoseMode(TOTALGOALS), mSpecifiedDose(0.f), mVisionMode(LINE), mDisPlayMode(RELATIVE_VALUE){

}

TpsDoseLineGraphicObject::~TpsDoseLineGraphicObject() {

}

void TpsDoseLineGraphicObject::GetDisplayAttributes(
    std::vector<DoseLineAttribute>& vecDoseLine) const{
    vecDoseLine = mVecDoseLine;
}

bool TpsDoseLineGraphicObject::SetDisplayAttributes(
    const  std::vector<DoseLineAttribute>& vecDoseLine) {
    mVecDoseLine = vecDoseLine;
    return true;
}

void TpsDoseLineGraphicObject::SetDisplayMode(DOSELINE_DISPLAYMODE dispMode){
    mDisPlayMode = dispMode;
}

DOSELINE_DISPLAYMODE TpsDoseLineGraphicObject::GetDisplayMode(){
    return mDisPlayMode;
}

float TpsDoseLineGraphicObject::GetOneHundredDoseValue() const
{
    float oneHundredDoseValue;
    switch (mDoseMode){
    case TOTALGOALS:
        oneHundredDoseValue = mTotalGoal;
        break;
    case MAXDOSE:
        oneHundredDoseValue = mMaxDose;
        break;
    case SPECIFIEDDOSE:
        oneHundredDoseValue = mSpecifiedDose;
        break;
    default:
        oneHundredDoseValue = INVALID_DOSE_VALUE;
    }
    return oneHundredDoseValue;
}
TPS_END_NAMESPACE