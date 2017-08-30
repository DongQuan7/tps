//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  jiandong.zhou     jiandong.zhou@united-imaging.com
/// 
///  \file tps_configure_manager.cpp
/// 
///  \brief  base class of backend configure

///  \version 1.0
///  \date    Aug. 6, 2013
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RtTpsDataAccess/tps_da_configuremanager.h"
#include "RtTpsFramework/tps_fw_doselinedatastructure.h"
#include "RtTpsFramework/tps_fw_beamstructure.h"
#include "RtTpsFramework/tps_fw_voistructure.h"
#include "RtTpsFramework/tps_fw_optimizer.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_template.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE
TpsConfigurationManager::TpsConfigurationManager(void) 
{
}


TpsConfigurationManager::~TpsConfigurationManager(void) 
{
    Dispose();
}

void TpsConfigurationManager::Dispose() {

    auto itDoseLineTemplate = mDoseLineTemplateCollection.begin();
    while (itDoseLineTemplate != mDoseLineTemplateCollection.end()) {
        delete itDoseLineTemplate->second;
        itDoseLineTemplate->second = nullptr;
        itDoseLineTemplate++;
    }

    mDoseLineTemplateCollection.clear();

    auto itBeamTemplate = mBeamTemplateCollection.begin();
    while (itBeamTemplate != mBeamTemplateCollection.end()) {
        delete itBeamTemplate->second;
        itBeamTemplate->second = nullptr;
        itBeamTemplate++;
    }

    mBeamTemplateCollection.clear();

    auto itVoiTemplate = mVOITemplateCollection.begin();
    while (itVoiTemplate != mVOITemplateCollection.end()) {
        delete itVoiTemplate->second;
        itVoiTemplate->second = nullptr;
        itVoiTemplate++;
    }

    mVOITemplateCollection.clear();

    auto itOptimizerTemplate = mOptimizerTemplateCollection.begin();
    while (itOptimizerTemplate != mOptimizerTemplateCollection.end()) {
        delete itOptimizerTemplate->second;
        itOptimizerTemplate->second = nullptr;
        itOptimizerTemplate++;
    }

    mOptimizerTemplateCollection.clear();

    auto itSystemconfig = mSystemconfigCollection.begin();
    while (itSystemconfig != mSystemconfigCollection.end()) {
        delete itSystemconfig->second;
        itSystemconfig->second = nullptr;
        itSystemconfig++;
    }

    mSystemconfigCollection.clear();
}

bool TpsConfigurationManager::AddSystemConfig(const RtTemplate& systemConfig) {

    auto itFind = mSystemconfigCollection.find(systemConfig.get_uid());
    if (itFind != mSystemconfigCollection.end()) {
        // Already exist
        return false;
    }

    RtTemplate* clone = new RtTemplate(systemConfig);
    mSystemconfigCollection[systemConfig.get_uid()] = clone;

    return true;
}

bool TpsConfigurationManager::RemoveSystemConfig(const std::string& systemConfigUID) {
    auto itFind = mSystemconfigCollection.find(systemConfigUID);
    if (itFind == mSystemconfigCollection.end()) {
        return false;
    }

    if (itFind->second != nullptr) {
        delete itFind->second;
        itFind->second = nullptr;
    }

    mSystemconfigCollection.erase(itFind);

    return true;
}

bool TpsConfigurationManager::UpdateSystemConfig(const RtTemplate& systemConfig) {
    auto itFind = mSystemconfigCollection.find(systemConfig.get_uid());
    if (itFind == mSystemconfigCollection.end()) {
        return false;
    }

    RtTemplate* clone = new RtTemplate(systemConfig);
    mSystemconfigCollection[systemConfig.get_uid()] = clone;

    return true;
}

bool TpsConfigurationManager::GetSystemConfig(const std::string& systemConfigUID,
    RtTemplate** systemConfig) {
        auto itFind = mSystemconfigCollection.find(systemConfigUID);
        if (itFind == mSystemconfigCollection.end()) {
            return false;
        }

        *systemConfig = itFind->second;

        return true;
}

void TpsConfigurationManager::GetAllSystemConfig(std::vector<RtTemplate*>& allSystemConfig) 
{
    allSystemConfig.clear();
    auto itGet = mSystemconfigCollection.begin();
    for (; itGet != mSystemconfigCollection.end(); ++itGet) {
        allSystemConfig.push_back(itGet->second);
    }
}

void TpsConfigurationManager::GetSystemConfigFromDatatype(const int dataType, std::vector<RtTemplate*>& systemConfig) 
{
    systemConfig.clear();
    auto itGet = mSystemconfigCollection.begin();
    for (; itGet != mSystemconfigCollection.end(); ++itGet) {
        if (itGet->second->get_datatype() == dataType)
        {
            systemConfig.push_back(itGet->second);
        }
    }

}

bool TpsConfigurationManager::AddDoseLineTemplate(const DoseLineTemplate& doseLineGroup) {

    auto itFind = mDoseLineTemplateCollection.find(doseLineGroup.GetDoseLineTemplateUID());
    if (itFind != mDoseLineTemplateCollection.end()) {
        return false;
    }

    DoseLineTemplate* clone = new DoseLineTemplate(doseLineGroup);
    mDoseLineTemplateCollection[doseLineGroup.GetDoseLineTemplateUID()] = clone;

    return true;
}

bool TpsConfigurationManager::RemoveDoseLineTemplate(const std::string& groupUid) {

    auto itFind = mDoseLineTemplateCollection.find(groupUid);
    if (itFind == mDoseLineTemplateCollection.end()) {
        return false;
    }

    if (itFind->second != nullptr) {
        delete itFind->second;
        itFind->second = nullptr;
    }

    mDoseLineTemplateCollection.erase(itFind);

    return true;
}

bool TpsConfigurationManager::GetDoseLineTemplateList(std::vector<DoseLineTemplate*>& doseLineInfoGroup) {
    std::map<std::string, DoseLineTemplate*>::iterator itGet;

    itGet = mDoseLineTemplateCollection.begin();
    for (; itGet != mDoseLineTemplateCollection.end(); ++itGet) {
        doseLineInfoGroup.push_back(itGet->second);
    }

    return true;
}

bool TpsConfigurationManager::AddVOITemplate(VOITemplate* voiTemplate){

    if (voiTemplate == nullptr || voiTemplate->mTemplateUid.empty()) {
        return false;
    }

    auto it = mVOITemplateCollection.find(voiTemplate->mTemplateUid);
    if(it != mVOITemplateCollection.end()){
        TPS_LOG_DEV_INFO << "template uid:" << voiTemplate->mTemplateUid << " already exists!";
        return false;
    }

    mVOITemplateCollection[voiTemplate->mTemplateUid] = voiTemplate;

    return true;
}

void TpsConfigurationManager::GetVOITemplateList(std::vector<VOITemplate*> *voiTemplates) const{
    for(auto iter = mVOITemplateCollection.begin(); iter != mVOITemplateCollection.end(); ++iter){
        voiTemplates->push_back(iter->second);
    }
}

bool TpsConfigurationManager::GetVOITemplate(const std::string voiTemplateUid, VOITemplate** voiTemplate) const{
    auto it = mVOITemplateCollection.find(voiTemplateUid);
    if(it == mVOITemplateCollection.end()){
        TPS_LOG_DEV_INFO << "Cannot find voi template with uid:" << voiTemplate;
        return false;
    }

    *voiTemplate = it->second;
    return true;

}

bool TpsConfigurationManager::RemoveVOITemplate(const std::string voiTemplateUid){
    auto it = mVOITemplateCollection.find(voiTemplateUid);
    if(it == mVOITemplateCollection.end()){
        TPS_LOG_DEV_INFO<<"Cannot find the specified voi template.";
        return false;
    }

    if (it->second != nullptr) {
        delete it->second;
        it->second = nullptr;
    }

    mVOITemplateCollection.erase(it);
    return true;
}


void TpsConfigurationManager::ClearVOITemplateList(){
    auto it = mVOITemplateCollection.begin();
    for(; it != mVOITemplateCollection.end(); ++it){
        it->second->mVOICollection.clear();
    }
    mVOITemplateCollection.clear();
}

bool TpsConfigurationManager::AddBeamTemplate(BeamTemplate* beamTemplate){

    if (beamTemplate == nullptr || beamTemplate->mTemplateUID.empty()) {
        return false;
    }

    auto it = mBeamTemplateCollection.find(beamTemplate->mTemplateUID);
    if(it != mBeamTemplateCollection.end()){
        // template name already exist
        return false;
    }

    mBeamTemplateCollection[beamTemplate->mTemplateUID] = beamTemplate;

    return true;
}

void TpsConfigurationManager::GetBeamTemplateList(std::vector<BeamTemplate*> *btList) const{
    for (auto iter = mBeamTemplateCollection.begin(); iter != mBeamTemplateCollection.end(); ++iter) {
        btList->push_back(iter->second);
    }
}

bool TpsConfigurationManager::GetBeamTemplate(const std::string& beamTemplateUid, BeamTemplate **beamTemplate) const{

    auto it = mBeamTemplateCollection.find(beamTemplateUid);
    if (it == mBeamTemplateCollection.end()){
        TPS_LOG_DEV_INFO << "Cannot find voi template with uid:" << beamTemplateUid;
        return false;
    }

    *beamTemplate = it->second;

    return true;
}

bool TpsConfigurationManager::RemoveBeamTemplate(const std::string& beamTemplateUid){
    auto it = mBeamTemplateCollection.find(beamTemplateUid);
    if(it == mBeamTemplateCollection.end()){
        TPS_LOG_DEV_INFO<<"Cannot find the specified voi template.";
        return false;
    }

    if (it->second != nullptr) {
        delete it->second;
        it->second = nullptr;
    }

    mBeamTemplateCollection.erase(it);
    return true;
}

void TpsConfigurationManager::ClearBeamTemplateList(){
    auto it = mBeamTemplateCollection.begin();
    for(; it != mBeamTemplateCollection.end(); ++it){
        it->second->mBeamCollection.clear();
    }
    mBeamTemplateCollection.clear();
}

std::string TpsConfigurationManager::GetFirstBeamTemplateName() const{
    //if(!mBeamTemplateCollection.empty()){
    //    return mBeamTemplateCollection.begin()->second->mTemplateName;
    //}
    //else{
    //    return "";
    //}
    return mBeamTemplateCollection.empty()?"":(mBeamTemplateCollection.begin()->second)->mTemplateName;
}


bool TpsConfigurationManager::AddOptimizerTemplate(OptimizerTemplate* optimizerTemplate){

	if (optimizerTemplate == nullptr || optimizerTemplate->mTemplateUID.empty()) {
		return false;
	}

	auto it = mOptimizerTemplateCollection.find(optimizerTemplate->mTemplateUID);
	if(it != mOptimizerTemplateCollection.end()){
		TPS_LOG_DEV_INFO << "template uid:" << optimizerTemplate->mTemplateUID << " already exists!";
		return false;
	}

	mOptimizerTemplateCollection[optimizerTemplate->mTemplateUID] = optimizerTemplate;

    return true;
}

void TpsConfigurationManager::GetOptimizerTemplateList(std::vector<OptimizerTemplate*> *optmList) const{
    for (auto iter = mOptimizerTemplateCollection.begin(); iter != mOptimizerTemplateCollection.end(); ++iter) {
        optmList->push_back(iter->second);
    }
}

bool TpsConfigurationManager::GetOptimizerTemplate(const std::string& templateName, OptimizerTemplate *&optm) const{
    for (auto iter = mOptimizerTemplateCollection.begin(); iter != mOptimizerTemplateCollection.end(); ++iter) {
        if(iter->first == templateName){
            optm = iter->second;
            return true;
        }
    }
    return false;
}

bool TpsConfigurationManager::RemoveOptimizerTemplate(const std::string& templateName){
    auto it = mOptimizerTemplateCollection.find(templateName);
    if(it == mOptimizerTemplateCollection.end()){
        TPS_LOG_DEV_INFO<<"Cannot find the specified voi template.";
        return false;
    }

    if (it->second != nullptr) {
        delete it->second;
        it->second = nullptr;
    }

    mOptimizerTemplateCollection.erase(it);

    return true;
}

void TpsConfigurationManager::ClearOptimizerTemplateList(){
    auto it = mOptimizerTemplateCollection.begin();
    for(; it != mOptimizerTemplateCollection.end(); ++it){
        it->second->mOptimizerCollection.clear();
    }
    mOptimizerTemplateCollection.clear();
}

TPS_END_NAMESPACE
