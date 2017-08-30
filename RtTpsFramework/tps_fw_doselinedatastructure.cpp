////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  liu linqi  linqi.liu@united-imaging.com
/// 
///  \file tps_fw_doselinedatastructure.cpp
/// 
///  \brief class DoseLineInfo declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/09/01
////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include"StdAfx.h"
#include "RtTpsFramework/tps_fw_doselinedatastructure.h"

TPS_BEGIN_NAMESPACE

DoseLineTemplateItem::DoseLineTemplateItem() :
    mDosePercent(0.0f){
        memset(mColor, 0, 4 * sizeof(float));
}

DoseLineTemplateItem::DoseLineTemplateItem(const DoseLineTemplateItem& doseLine) {
    mDosePercent = doseLine.mDosePercent;
    memcpy(mColor, doseLine.mColor, 4 * sizeof(float));
}

float DoseLineTemplateItem::GetDosePercent() const { return mDosePercent; }
void DoseLineTemplateItem::SetDosePercent(const float& fDosePercent) { mDosePercent = fDosePercent; }

const float* DoseLineTemplateItem::GetColor() const { return mColor; }
void DoseLineTemplateItem::SetColor(const float fColor[4]) { memcpy(mColor, fColor, 4 * sizeof(float)); }

DoseLineTemplate::DoseLineTemplate() : mDoseLineTemplateUID(""),
    mDoseLineTemplateName("") {
        mDoseLineTemplateItemList.clear();
}

std::string DoseLineTemplate::GetDoseLineTemplateUID() const
{ return mDoseLineTemplateUID; }

void DoseLineTemplate::SetDoseLineTemplateUID(std::string groupUid)
{ mDoseLineTemplateUID = groupUid; }

std::string DoseLineTemplate::GetDoseLineTemplateName() const
{ return mDoseLineTemplateName; }

void DoseLineTemplate::SetDoseLineTemplateName(std::string groupName)
{ mDoseLineTemplateName = groupName; }

std::string DoseLineTemplate::GetDoseLineTemplateDescription() const
{ return mDoseLineTemplateDescription; }

void DoseLineTemplate::SetDoseLineTemplateDescription(std::string description)
{ mDoseLineTemplateDescription = description; }

int DoseLineTemplate::GetDoseLineTemplateDisplayMode() const
{
    return mDisplayMode; 
}
void DoseLineTemplate::SetDoseLineTemplateDisplayMode(int displayMode)
{
    mDisplayMode = displayMode;
}

int DoseLineTemplate::GetDoseLineTemplateHundredPercentDefinition() const
{
    return mHundredPercentDefinition; 
}
void DoseLineTemplate::SetDoseLineTemplateHundredPercentDefinition(int hundredPercentDefinition)
{
    mHundredPercentDefinition = hundredPercentDefinition;
}

double DoseLineTemplate::GetDoseLineTemplateSpecifiedDose() const
{
    return mSpecifiedDose; 
}
void DoseLineTemplate::SetDoseLineTemplateSpecifiedDose(double specifiedDose)
{
    mSpecifiedDose = specifiedDose;
}

void DoseLineTemplate::AddDoseLineTemplateItem(DoseLineTemplateItem &doseLineInfo){
    mDoseLineTemplateItemList.push_back(doseLineInfo);
}

void DoseLineTemplate::GetDoseLineTemplateItemList(std::list<DoseLineTemplateItem>&doseLineList){
    doseLineList = mDoseLineTemplateItemList;
}

void DoseLineTemplate::ClearDoseLineTemplateItemList(){
    mDoseLineTemplateItemList.clear();
}

TPS_END_NAMESPACE
