//////////////////////////////////////////////////////////////////////////
/// \defgroup MS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  linqi.liu linqi.liu@united-imaging.com
///
///  \file    ms_machineSetting_manager.cpp
///  \brief   entry off machine setting operator
///
///  \version 1.0
///  \date    Dec. 12, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tps_logger.h"

#include "RtTpsDataAccess/tps_da_machineconfigmanager.h"


#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mcphocommissionedunit.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_tray.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_commissionedunit.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_measuredprofile.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_applicator.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_accessory.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_accessorychunk.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_commissionedunit_tray.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_kernelweight.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_measureddata.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_measuredprofile.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_wedgefactor.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_uid_generater.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_tray.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_goldenstt.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_measureddata.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_contour.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_factory.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"
#include "RtTpsDataAccess/tps_da_machinetables.h"

TPS_BEGIN_NAMESPACE


TpsMachineSettingManager::TpsMachineSettingManager(void){
}

bool TpsMachineSettingManager::ClearMachineSettingData()
{
    return true;
}

TpsMachineSettingManager::~TpsMachineSettingManager(void) {

    for (auto i = mtableMap.begin(); i != mtableMap.end(); ++i)
    {
        void* temp = (*i).second;
        DEL_PTR(temp);
    }
    mtableMap.clear();
}

void TpsMachineSettingManager::KernelWeightDataFormat(std::vector<RtKernelweight*>& vkernelWeightList)
{
    for (auto it = vkernelWeightList.begin(); it != vkernelWeightList.end(); it++)
    {

        //std::string kernelidlistfilepath = (*it)->get_kernelidlistfilepath();
        std::string strKernelWeightList = (*it)->get_kernelidlist();

        //std::string strKernelWeightList;
        std::vector<std::string> vecValue;
        std::vector<float>vecFValue;
        std::vector<int> vecIValue;

        //ReadFile_i(kernelweightfilepath,strKernelWeightList);
        if (strKernelWeightList != ""){

            (void)boost::split(vecValue, strKernelWeightList, boost::is_any_of(","));
            for (int i = 0; i < vecValue.size(); i++){

                if (vecValue[i] != "")
                    vecFValue.push_back(static_cast<float>(atof(vecValue[i].c_str())));
            }

            (*it)->set_kernelweight_list(vecFValue);
            vecValue.clear();
        }

        strKernelWeightList = (*it)->get_kernelidlist();
        //ReadFile_i(kernelidlistfilepath,strKernelWeightList);
        if (strKernelWeightList != ""){

            (void)boost::split(vecValue, strKernelWeightList, boost::is_any_of(","));
            for (int i = 0; i < vecValue.size(); i++){

                if (vecValue[i] != "")
                    vecIValue.push_back(static_cast<int>(atof(vecValue[i].c_str())));
            }

            (*it)->set_kernelid_list(vecIValue);
            vecValue.clear();

        }
    }
}

void TpsMachineSettingManager::MeasureProfileDataFormat(std::vector<RtMeasuredprofile*>& measuredProfileList)
{
    for (auto it = measuredProfileList.begin(); it != measuredProfileList.end(); ++it){


        std::string strMeaPointDoseList = (*it)->get_meapointdoselist();
        //ReadFile_i(filePath,strMeaPointDoseList);

        if (strMeaPointDoseList != ""){
            std::vector<std::string> vecValue;
            (void)boost::split(vecValue, strMeaPointDoseList, boost::is_any_of("|"));
            int ilen = static_cast<int>(vecValue.size());

            std::vector<PointDose*> measPoinDoseList;
            // measPoinDoseList.resize(ilen);

            // (*it)->GetMeaPointDoseList().resize(ilen);
            for (int i = 0; i < ilen; i++){
                std::vector<std::string> strValue;
                PointDose* pointDose = new PointDose();
                (void)boost::split(strValue, vecValue[i], boost::is_any_of(","));

                pointDose->m_fDose = static_cast<float>(atof(strValue[0].c_str()));
                pointDose->m_fX = static_cast<float>(atof(strValue[1].c_str()));
                pointDose->m_fY = static_cast<float>(atof(strValue[2].c_str()));
                pointDose->m_fZ = static_cast<float>(atof(strValue[3].c_str()));

                measPoinDoseList.push_back(pointDose);
                //strValue.clear();
            }
            (*it)->SetMeaPointDoseList(measPoinDoseList);

            // vecValue.clear();
        }

        std::string strCalPointDoseList = (*it)->get_calcpointdoselist();
        //std::string strCalPointDoseList;
        //ReadFile_i(calFilePath,strCalPointDoseList);

        if (strCalPointDoseList != ""){

            std::vector<PointDose*> calPoinDoseList;

            std::vector<std::string> vecValue;
            (void)boost::split(vecValue, strCalPointDoseList, boost::is_any_of("|"));
            int ilen = static_cast<int>(vecValue.size());

            //calPoinDoseList.resize(ilen);

            //(*it)->GetCalcPointDoseList().resize(ilen);
            for (int i = 0; i < ilen; i++){

                std::vector<std::string> strValue;
                PointDose* pointDose = new PointDose();
                (void)boost::split(strValue, vecValue[i], boost::is_any_of(","));

                pointDose->m_fDose = static_cast<float>(atof(strValue[0].c_str()));
                pointDose->m_fX = static_cast<float>(atof(strValue[1].c_str()));
                pointDose->m_fY = static_cast<float>(atof(strValue[2].c_str()));
                pointDose->m_fZ = static_cast<float>(atof(strValue[3].c_str()));

                calPoinDoseList.push_back(pointDose);

                //strValue.clear();
            }
            (*it)->SetCalcPointDoseList(calPoinDoseList);
            vecValue.clear();
        }

        //std::string strCurveResultAnalyse = "";
        std::string strCurveResultAnalyse = (*it)->get_curveresultanalyselist();

        // ReadFile_i(errorFilePath,strCurveResultAnalyse);

        if (strCurveResultAnalyse != "")
        {
            std::vector<CurveResultAnalyse*> curveResultList;

            std::vector<std::string> vecValue;
            (void)boost::split(vecValue, strCurveResultAnalyse, boost::is_any_of("|"));
            int ilen = static_cast<int>(vecValue.size());

            //curveResultList.resize(ilen);

            //(*it)->GetCurveResultAnalyse().resize(ilen);
            for (int i = 0; i < ilen; i++)
            {
                std::vector<std::string> strValue;
                CurveResultAnalyse* resultAnalyse = new CurveResultAnalyse();
                (void)boost::split(strValue, vecValue[i], boost::is_any_of(","));

                resultAnalyse->m_sName = strValue[0];
                resultAnalyse->m_fAverageError = static_cast<float>(atof(strValue[1].c_str()));
                resultAnalyse->m_fErrorLimit = static_cast<float>(atof(strValue[2].c_str()));
                resultAnalyse->m_fFailurePoits = static_cast<float>(atof(strValue[3].c_str()));
                resultAnalyse->m_fMaxError = static_cast<float>(atof(strValue[4].c_str()));
                //strValue.clear();
                curveResultList.push_back(resultAnalyse);
            }

            (*it)->SetCurveResultAnalyse(curveResultList);
            vecValue.clear();
        }
    }
}

bool TpsMachineSettingManager::LoadMachineSetting(){
    try
    {
        IDatabaseFactory dbFactory;
        mDatabaseWrapper = dbFactory.CreateDBWrapper();
        if (mDatabaseWrapper == nullptr){
            TPS_LOG_DEV_ERROR<<"Failed to create database wrapper.";
            return false;
        }

        if(!mDatabaseWrapper->Initialize()){
            TPS_LOG_DEV_ERROR<<"Can't connect to database! \n Probably due to DB Version conflict \n";
            return false;
        }

        std::vector<RtMachine*> machineList;
        std::vector<RtCommissionedunit*> commissionedunitList;
        std::vector<RtTray*> trayList;
        std::vector<RtApplicator*> applicatorList;
        std::vector<RtAccessory*> accessoryList;

        std::vector<RtMcphocommissionedunit*> mcphocommissionedunitList;
        std::vector<RtMeasuredprofile*> measuredprofileList;
        std::vector<RtWedgefactor*> wedgefactorList;
        std::vector<RtKernelweight*> kernelweightList;
        std::vector<RtMeasureddata*> measureddataList;
        std::vector<RtCommissionedunit_tray*> commissionedunit_trayList;
        std::vector<RtGoldenstt*> goldensttList;
        std::vector<RtAccessorychunk*> accessorychunkList;

        if (mDatabaseWrapper->GetAllMachines(machineList))
        {
            TPS_LOG_DEV_ERROR << "database wrapper Get AllMachines Failed.";
            return false;
        }

        if (mDatabaseWrapper->GetAllCommissionedunitObjects(commissionedunitList)){
            TPS_LOG_DEV_ERROR << "database wrapper GetAllCommissionedunitObjects Failed.";
            return false;
        }

        if (mDatabaseWrapper->GetAllTrays(trayList)){
            TPS_LOG_DEV_ERROR << "database wrapper GetAllTrays Failed.";
            return false;
        }

        if (mDatabaseWrapper->GetAllApplicators(applicatorList)){
            TPS_LOG_DEV_ERROR << "database wrapper GetAllApplicators Failed.";
            return false;
        }

        if (mDatabaseWrapper->GetAllAccessorys(accessoryList)){
            TPS_LOG_DEV_ERROR << "database wrapper GetAllApplicators Failed.";
            return false;
        }

        if (mDatabaseWrapper->GetAllMcphocommissionedunits(mcphocommissionedunitList)){
            TPS_LOG_DEV_ERROR << "database wrapper GetAllMcphocommissionedunits Failed.";
            return false;
        }

        if (mDatabaseWrapper->GetAllMeasuredprofile(measuredprofileList)){
            TPS_LOG_DEV_ERROR << "database wrapper GetAllMeasuredprofile Failed.";
            return false;
        }

        if (mDatabaseWrapper->GetAllWedgefactors(wedgefactorList)){
            TPS_LOG_DEV_ERROR << "database wrapper GetAllWedgefactors Failed.";
            return false;
        }

        if (mDatabaseWrapper->GetAllKernelWeights(kernelweightList)){
            TPS_LOG_DEV_ERROR << "database wrapper GetAllKernelWeights Failed.";
            return false;
        }

        if (mDatabaseWrapper->GetAllMeasureDatas(measureddataList)){
            TPS_LOG_DEV_ERROR << "database wrapper GetAllKernelWeights Failed.";
            return false;
        }

        if (mDatabaseWrapper->GetAllCommissionedunit_trays(commissionedunit_trayList)){
            TPS_LOG_DEV_ERROR << "database wrapper GetAllCommissionedunit_trays Failed.";
            return false;
        }

        if (mDatabaseWrapper->GetAllGoldenstt(goldensttList)){
            TPS_LOG_DEV_ERROR << "database wrapper GetAllGoldenstt Failed.";
            return false;
        }

        if (mDatabaseWrapper->GetAllAccessorychunk(accessorychunkList)){
            TPS_LOG_DEV_ERROR << "database wrapper GetAllAccessorychunk Failed.";
            return false;
        }

        RtMachineTable* machineTable = this->GetTable<RtMachineTable>();
        RtCommissionedunitTable* commissionedunitTable = this->GetTable<RtCommissionedunitTable>();
        RtTrayTable* trayTable = this->GetTable<RtTrayTable>();
        RtApplicatorTable* applicatorTable = this->GetTable<RtApplicatorTable>();
        RtAccessoryTable* accessoryTable = this->GetTable<RtAccessoryTable>();

        RtMcphocommissionedunitTable* mcphocommissionedunitTable = this->GetTable<RtMcphocommissionedunitTable>();
        RtMeasuredprofileTable* measuredprofileTable = this->GetTable<RtMeasuredprofileTable>();
        RtWedgefactorTable* wedgefactoTable = this->GetTable<RtWedgefactorTable>();
        RtKernelweightTable* kernelweightTable = this->GetTable<RtKernelweightTable>();
        RtMeasureddataTable* measureddataTable = this->GetTable<RtMeasureddataTable>();
        RtCommissionedunit_trayTable* commissionedunit_trayTable = this->GetTable<RtCommissionedunit_trayTable>();
        RtGoldensttTable* goldensttTable = this->GetTable<RtGoldensttTable>();
        RtContourTable* contourTable = this->GetTable<RtContourTable>();
        RtAccessorychunkTable* accessorychunkTable = this->GetTable<RtAccessorychunkTable>();

        std::string sMD5("");
        for (auto it = machineList.begin(); it != machineList.end(); ++it)
        {
            sMD5 = mDatabaseWrapper->CalcHash((const void*)(*it), RtDbDef::TYPE_MACHINE);
            (*it)->set_crc_valid(sMD5 == (*it)->get_crc());
            machineTable->Add(**it);
        }
        DEL_PTRVECTOR(machineList);

        for (auto it = commissionedunitList.begin(); it != commissionedunitList.end(); ++it){
            commissionedunitTable->Add(**it);
        }
        DEL_PTRVECTOR(commissionedunitList);

        for (auto it = trayList.begin(); it != trayList.end(); ++it){
            trayTable->Add(**it);
        }
        DEL_PTRVECTOR(trayList);

        for (auto it = applicatorList.begin(); it != applicatorList.end(); ++it){
            applicatorTable->Add(**it);
        }
        DEL_PTRVECTOR(applicatorList);

        for (auto it = accessoryList.begin(); it != accessoryList.end(); ++it){
            accessoryTable->Add(**it);
        }
        DEL_PTRVECTOR(accessoryList);

        for (auto it = mcphocommissionedunitList.begin(); it != mcphocommissionedunitList.end(); ++it){
            mcphocommissionedunitTable->Add(**it);
        }
        DEL_PTRVECTOR(mcphocommissionedunitList);

        for (auto it = measuredprofileList.begin(); it != measuredprofileList.end(); ++it){
            measuredprofileTable->Add(**it);
        }
        DEL_PTRVECTOR(measuredprofileList);

        for (auto it = wedgefactorList.begin(); it != wedgefactorList.end(); ++it){
            wedgefactoTable->Add(**it);
        }
        DEL_PTRVECTOR(wedgefactorList);

        //KernelWeight 特殊处理
        for (auto it = kernelweightList.begin(); it != kernelweightList.end(); ++it){
            kernelweightTable->Add(**it);
        }
        DEL_PTRVECTOR(kernelweightList);

        //MeasureProfile 特殊处理
        for (auto it = measureddataList.begin(); it != measureddataList.end(); ++it){
            measureddataTable->Add(**it);
        }
        DEL_PTRVECTOR(measureddataList);

        for (auto it = commissionedunit_trayList.begin(); it != commissionedunit_trayList.end(); ++it){
            commissionedunit_trayTable->Add(**it);
        }
        DEL_PTRVECTOR(commissionedunit_trayList);

        for (auto it = goldensttList.begin(); it != goldensttList.end(); ++it){
            goldensttTable->Add(**it);
        }
        DEL_PTRVECTOR(goldensttList);

        for (auto it = accessorychunkList.begin(); it != accessorychunkList.end(); ++it){
            accessorychunkTable->Add(**it);

            RtContour* contour;
            //Contour 特殊处理
            if (mDatabaseWrapper->GetContourByAccessorychunkuid((*it)->get_uid(), contour) == DB_CODES_SUCCESS){
                contourTable->Add(*contour);
                DEL_PTR(contour);
            }
        }
        DEL_PTRVECTOR(accessorychunkList);
    }
    catch (std::exception& ex)
    {
        printf("exception:%s",ex.what());
        return false;
    }

    return true;
}

//bool TpsMachineSettingManager::DeleteMachine(const std::string& uid){
//
//    auto it = mMachineMap.find(uid);
//    if (it == mMachineMap.end()){
//        TPS_LOG_DEV_ERROR << "The machine does not exist which UID: " << uid;
//        return true;
//    }
//    DEL_PTR(it->second);
//    mMachineMap.erase(it);
//    return true;
//}

//bool TpsMachineSettingManager::GetMachine(const std::string& uid, RtMachine*& machine){
//
//    auto it = mMachineMap.find(uid);
//    if (it == mMachineMap.end()){
//        TPS_LOG_DEV_ERROR<<"The machine does not exist which UID: "<<uid;
//        return false;
//    }
//    machine = it->second;
//
//    return true;
//}
//
//bool TpsMachineSettingManager::AddMachineInfo(const RtMachine &addMachine){
//
//    const std::string machineUID = addMachine.get_uid();
//    auto it = mMachineMap.find(machineUID);
//    if (it != mMachineMap.end()){
//        TPS_LOG_DEV_ERROR<<"The machine with UID: "<<machineUID << "already exists";
//        return false;
//    }
//    RtMachine* cloneaddMachine  = new RtMachine(addMachine);
//    mMachineMap[machineUID] = cloneaddMachine;
//
//    return true;
//}


//template <typename T>
//bool TpsMachineSettingManager::Add_MapVector(std::map<std::string, std::vector<T*>>& mDataMap, const T& item)
//{
//    std::string commissionUid = item.get_commissionedunituid();
//
//    auto it = mDataMap.find(commissionUid);
//    if (it == mDataMap.end()){
//        std::vector<T*> *vect = new std::vector<T*>();
//        mDataMap[commissionUid] = *vect;
//    }
//
//    for (auto it = mDataMap[commissionUid].begin(); it != mDataMap[commissionUid].end(); it++)
//    {
//        if (item.get_uid() == (*it)->get_uid())
//        {
//            TPS_LOG_DEV_ERROR << "Item already exists";
//            return false;
//        }
//    }
//
//    T* cloneItem = new T(item);
//
//    mDataMap[commissionUid].push_back(cloneItem);
//    return true;
//}
//
//
//template <typename T>
//bool TpsMachineSettingManager::Delete_MapVector(std::map<std::string, std::vector<T*>>& mDataMap, const T& item)
//{
//    std::string commissionUid = item.get_commissionedunituid();
//    auto it = mDataMap.find(commissionUid);
//    if (it == mDataMap.end()){
//        TPS_LOG_DEV_ERROR << "Item does not exist";
//        return false;
//    }
//
//    for (auto it = mDataMap[commissionUid].begin(); it != mDataMap[commissionUid].end(); it++)
//    {
//        if (item.get_uid() == (*it)->get_uid())
//        {
//            DEL_PTR(*it);
//            return true;
//        }
//    }
//    return false;
//}
//
//template <typename T>
//bool TpsMachineSettingManager::Delete_MapVectorByFKey(std::map<std::string, std::vector<T*>>& mDataMap, const std::string& key)
//{
//    auto it = mDataMap.find(key);
//    if (it == mDataMap.end()){
//        TPS_LOG_DEV_ERROR << "Item does not exist";
//        return false;
//    }
//    
//    DEL_PTRVECTOR(it->second);
//    mDataMap.erase(it);
//    return true;
//}
//
//template <typename T>
//bool TpsMachineSettingManager::Delete_MapVectorByPKey(std::map<std::string, std::vector<T*>>& mDataMap, const std::string& key)
//{
//    for (auto mapItem = mDataMap.begin(); mapItem != mDataMap.end(); mapItem++)
//    {
//        for (auto it = mapItem->second.begin(); it != mapItem->second.end(); it++)
//        {
//            if (key == (*it)->get_uid())
//            {
//                DEL_PTR(*it);
//                return true;
//            }
//        }
//    }
//    return false;
//}
//
//template <typename T>
//bool TpsMachineSettingManager::Update_MapVector(std::map<std::string, std::vector<T*>>& mDataMap, const T& item)
//{
//    std::string commissionUid = item.get_commissionedunituid();
//    auto it = mDataMap.find(commissionUid);
//    if (it == mDataMap.end()){
//        TPS_LOG_DEV_ERROR << "Item does not exist";
//        return false;
//    }
//
//    for (auto it = mDataMap[commissionUid].begin(); it != mDataMap[commissionUid].end(); it++)
//    {
//        if (item.get_uid() == (*it)->get_uid())
//        {
//            **it = item;
//            return true;
//        }
//    }
//    return false;
//}
//
//template <typename T>
//bool TpsMachineSettingManager::Get_MapVectorByFKey(std::map<std::string, std::vector<T*>>& mDataMap,const std::string& key, std::vector<T*>& voutPut)
//{
//    auto it = mDataMap.find(key);
//    if (it == mDataMap.end()){
//        TPS_LOG_DEV_ERROR << "Item does not exist";
//        return false;
//    }
//
//    voutPut = it->second;
//    return true;
//}
//
//template <typename T>
//bool TpsMachineSettingManager::Get_MapVectorByPKey(std::map<std::string, std::vector<T*>>& mDataMap, const std::string& key, T*& voutPut)
//{
//    for (auto mapItem = mDataMap.begin(); mapItem != mDataMap.end();mapItem++)
//    {
//        for (auto it = mapItem->second.begin(); it != mapItem->second.end(); it++)
//        {
//            if (key == (*it)->get_uid())
//            {
//                voutPut = (*it);
//                return true;
//            }
//        }
//    }
//
//    TPS_LOG_DEV_ERROR << "Item does not exist";
//    return false;
//}



//bool TpsMachineSettingManager::AddGoldenSTT(RtGoldenstt& golden){
//    return Add_MapVector(mGoldenSTTMap, golden);
//}
//
//bool TpsMachineSettingManager::UpdateGoldenSTT(RtGoldenstt* const golden){
//
//    return Update_MapVector(mGoldenSTTMap, *golden);
//}

//bool TpsMachineSettingManager::GetGoldenSTT(std::string goldenUID,RtGoldenstt*& golden){
//
//    return Get_MapVectorByPKey(mGoldenSTTMap,goldenUID, golden);
//}
//
//bool TpsMachineSettingManager::DeleteGoldenSTT(std::string commissionUid){
//    return Delete_MapVectorByFKey(mGoldenSTTMap, commissionUid);
//}

//bool TpsMachineSettingManager::GetGoldenSTT(std::string commissionUid, std::vector<RtGoldenstt*>& vgolden){
//
//    return Get_MapVectorByFKey(mGoldenSTTMap, commissionUid, vgolden);
//}

//bool TpsMachineSettingManager::GetUnit(const std::string unitUID, RtCommissionedunit*& machineUnit){
//
//    auto it = mMachineUniteMap.find(unitUID);
//    if (it == mMachineUniteMap.end()){
//        TPS_LOG_DEV_ERROR<<"The commissionUnit does not exist which UID: "<< unitUID;
//        return false;
//    }
//    machineUnit = it->second;
//
//    return true;
//}

//bool TpsMachineSettingManager::GetCommissionUnitList(std::string uid,std::vector<RtCommissionedunit*>&commissionUnitList){
//
//    auto machineUnit = mMachineUnitToMultiCommissionMap.find(uid);
//    if (machineUnit == mMachineUnitToMultiCommissionMap.end()){
//        TPS_LOG_DEV_ERROR<<"The machine not exist which UID: "<< uid;
//        return false;
//    }
//    commissionUnitList =machineUnit->second;
//
//    return true;
//}



//bool TpsMachineSettingManager::AddCommissionInfo(const RtCommissionedunit &addCom){
//
//    const std::string comUID = addCom.get_uid();
//    auto it = mMachineUniteMap.find(comUID);
//    if (it != mMachineUniteMap.end()){
//        TPS_LOG_DEV_ERROR<<"The commission with UID: "<<comUID << "already exists";
//        return false;
//    }
//    RtCommissionedunit* cloneaddCommission  = new RtCommissionedunit(addCom);
//    mMachineUniteMap[comUID] = cloneaddCommission;
//
//    return true;
//}

//bool TpsMachineSettingManager::DeleteCommissionInfo(std::string commissionInfoUid){
//
//    auto it = mMachineUniteMap.find(commissionInfoUid);
//    if (it == mMachineUniteMap.end()){
//        TPS_LOG_DEV_ERROR<<"The commissionInfo not exist which UID: "<< commissionInfoUid;
//        return false;
//    }
//
//    DEL_PTR(it->second);
//    mMachineUniteMap.erase(it);
//
//    return true;
//}

//bool TpsMachineSettingManager::AddPhoCommissionInfo(const RtMcphocommissionedunit &addCom){
//
//    const std::string comUID = addCom.get_commissionedunituid();
//    auto it = mPhoCommissionUnitMap.find(comUID);
//    if (it != mPhoCommissionUnitMap.end()){
//        TPS_LOG_DEV_ERROR<<"The pho commission  with UID: "<<comUID << "already exists";
//        return false;
//    }
//    RtMcphocommissionedunit* cloneaddCommission  = new RtMcphocommissionedunit(addCom);
//    mPhoCommissionUnitMap[comUID] = cloneaddCommission;
//
//    return true;
//}

//bool TpsMachineSettingManager::DeletePhoCommissionInfo(std::string commissionInfoUid){
//
//    auto it = mPhoCommissionUnitMap.find(commissionInfoUid);
//    if (it == mPhoCommissionUnitMap.end()){
//        TPS_LOG_DEV_ERROR<<"The commissionInfo not exist which UID: "<< commissionInfoUid;
//        return false;
//    }
//
//    DEL_PTR(it->second);
//    mPhoCommissionUnitMap.erase(it);
//    return true;
//}

//bool TpsMachineSettingManager::UpdateCommission(RtCommissionedunit* const updateCom){
//
//    auto it = mMachineUniteMap.find(updateCom->get_uid());
//    if (it == mMachineUniteMap.end()){
//        TPS_LOG_DEV_ERROR << "The machine does not exist which UID: " << updateCom->get_uid();
//        return false;
//    }
//    *it->second = *updateCom;
//
//    return true;
//}

//bool TpsMachineSettingManager::DeleteAccessoryList(std::string uid){
//
//    auto it = mMachineUnitToMultiAccessoryMap.find(uid);
//    if (it == mMachineUnitToMultiAccessoryMap.end()){
//        TPS_LOG_DEV_WARNING << "The machine not exist which UID: " << uid;
//        return true;
//    }
//    DEL_PTRVECTOR(it->second);
//    mMachineUnitToMultiAccessoryMap.erase(it);
//
//    return true;
//}

//bool TpsMachineSettingManager::GetAccessoryList(std::string uid,std::vector<RtAccessory*>&accessoryList){
//
//    auto machineUnit = mMachineUnitToMultiAccessoryMap.find(uid);
//    if (machineUnit == mMachineUnitToMultiAccessoryMap.end()){
//        TPS_LOG_DEV_ERROR<<"The machine not exist which UID: "<< uid;
//        return false;
//    }
//    accessoryList =machineUnit->second;
//
//    return true;
//}

//bool TpsMachineSettingManager::GetAccessory(const std::string& accessoryUID, RtAccessory*& accessory){
//
//    auto it = mAccessoryMap.find(accessoryUID);
//    if (it == mAccessoryMap.end()){
//        TPS_LOG_DEV_ERROR<<"The accessory does not exist which UID: "<< accessoryUID;
//        return false;
//    }
//    accessory = it->second;
//
//    return true;
//}

//bool TpsMachineSettingManager::AddAccessory(RtAccessory& accessory){
//
//    const std::string acceUID = accessory.get_uid();
//    auto it = mAccessoryMap.find(acceUID);
//    if (it != mAccessoryMap.end()){
//        TPS_LOG_DEV_ERROR<<"The accessory does not exist which UID: "<<acceUID;
//        return false;
//    }
//    RtAccessory* cloneaddAccessory  = new RtAccessory(accessory);
//    mAccessoryMap[acceUID] = cloneaddAccessory;
//
//    return true;
//}

//bool TpsMachineSettingManager::UpdateAccessory(RtAccessory* const accessory){
//
//    auto it = mAccessoryMap.find(accessory->get_uid());
//    if (it == mAccessoryMap.end()){
//        TPS_LOG_DEV_ERROR<<"The machine does not exist which UID: "<< accessory->get_uid();
//        return false;
//    }
//    it->second = accessory;
//
//    return true;
//}

//bool TpsMachineSettingManager::DeleteApplicatorList(std::string machineUid){
//
//    auto it = mMachineUnitToMultiApplicatorMap.find(machineUid);
//    if (it == mMachineUnitToMultiApplicatorMap.end()){
//        TPS_LOG_DEV_ERROR << "The machine not exist which UID: " << machineUid;
//        return false;
//    }
//    DEL_PTRVECTOR(it->second);
//    mMachineUnitToMultiApplicatorMap.erase(it);
//
//    return true;
//}

//bool TpsMachineSettingManager::GetApplicatorList(std::string uid,std::vector<RtApplicator*>&applicatorList){
//
//    auto machineUnit = mMachineUnitToMultiApplicatorMap.find(uid);
//    if (machineUnit == mMachineUnitToMultiApplicatorMap.end()){
//        TPS_LOG_DEV_ERROR<<"The machine not exist which UID: "<< uid;
//        return false;
//    }
//    applicatorList =machineUnit->second;
//
//    return true;
//}

//bool TpsMachineSettingManager::GetElectronApplicator(const std::string& appliectorUID,RtApplicator*& electronApp ){
//
//    auto it = mElectronMap.find(appliectorUID);
//    if (it == mElectronMap.end())
//    {
//        TPS_LOG_DEV_ERROR<<"The eletronApplicator does not exist which UID: "<< appliectorUID;
//        return false;
//    }
//
//    electronApp = it->second;
//
//    return true;
//}

//bool TpsMachineSettingManager::AddElectronApplicator(RtApplicator& applicator){
//    const std::string acceUID = applicator.get_uid();
//
//    auto it = mElectronMap.find(acceUID);
//    if (it != mElectronMap.end()){
//        TPS_LOG_DEV_ERROR<<"The applicator does not exist which UID: "<<acceUID;
//        return false;
//    }
//    RtApplicator* cloneApplicator  = new RtApplicator(applicator);
//    mElectronMap[acceUID] = cloneApplicator;
//
//    return true;
//}
//
//bool TpsMachineSettingManager::UpdateElectronApplicator( RtApplicator* const eletronApplicator){
//
//    auto it = mElectronMap.find(eletronApplicator->get_uid());
//    if (it == mElectronMap.end()){
//        TPS_LOG_DEV_ERROR<<"The machine does not exist which UID: "<< eletronApplicator->get_uid();
//        return false;
//    }
//    it->second = eletronApplicator;
//
//    return true;
//}


//void TpsMachineSettingManager::GetAccessoryUIDMap(std::map<std::string, std::string> &accessoryUID ) {
//
//    accessoryUID = mAccessoryUIDMap;
//}

//bool TpsMachineSettingManager::GetMachineInfo(std::vector<RtMachine*>& machineInfoList){
//
//    for (auto it =mMachineMap.begin(); it != mMachineMap.end(); ++it){
//        machineInfoList.push_back(&(*it->second));
//    }
//
//    return true;
//}

//bool TpsMachineSettingManager::GetMeasureData(std::string commissionInfoUid,std::vector<RtMeasureddata*>& measureDataList ){
//
//    return Get_MapVectorByFKey(mMeasureDataMap, commissionInfoUid, measureDataList);
//}

//bool TpsMachineSettingManager::AddMeasureData(RtMeasureddata& measureData){
//
//    return Add_MapVector(mMeasureDataMap, measureData);
//}

//bool TpsMachineSettingManager::DeleteMeasureData(std::string commissionInfoUid){
//
//    return Delete_MapVectorByFKey(mMeasureDataMap, commissionInfoUid);
//}

//bool TpsMachineSettingManager::UpdateMeasureProfile(RtMeasuredprofile* measureProfile)
//{
//    return Update_MapVector(mMeasureProfileList, *measureProfile);
//}

//bool TpsMachineSettingManager::GetMeasureProfile(std::string commissionuid, std::vector<RtMeasuredprofile*>& measureProfileList){
//
//    return Get_MapVectorByFKey(mMeasureProfileList,commissionuid, measureProfileList);
//}

bool TpsMachineSettingManager::ReadFile_i(std::string sFilePath,string& fileContent){


    ifstream ifile(sFilePath);
    //将文件读入到ostringstream对象buf中
    ostringstream buf;
    char ch;
    while(buf&&ifile.get(ch))
        buf.put(ch);
    //返回与流对象buf关联的字符串
    fileContent =  buf.str();



    //char* buffer = nullptr;
    //std::ifstream ifs;
    //ifs.open(sFilePath, std::ios::in);
    //std::size_t* size = 0;
    //if (!ifs) {
    //    TPS_LOG_DEV_ERROR<<"Failed to open file["<<sFilePath<<"].";
    //    return false;
    //}
    //std::filebuf* pbuf = ifs.rdbuf();
    //*size = pbuf->pubseekoff (0,ifs.end,ifs.in);
    //if (*size > 0){
    //    pbuf->pubseekpos (0,ifs.in);
    //    buffer=new char[*size];
    //    pbuf->sgetn (buffer,*size);
    //}
    //fileContent = buffer;
    //ifs.close();

    return true;
}

bool TpsMachineSettingManager::WriteFile_i(std::string sFilePath,string& fileContent){

    std::ofstream ofs;
    ofs.open(sFilePath, std::ios::out | std::ios::binary);
    if (!ofs) {
        TPS_LOG_DEV_ERROR<<"Failed to open file["<<sFilePath<<"].";
        return false;
    }

    ofs.write(fileContent.c_str(), fileContent.size());
    ofs.close();
    return true;
}


//bool TpsMachineSettingManager::DeleteMeasureProfiles(const std::string& commissionUid)
//{
//    return Delete_MapVectorByFKey(mMeasureProfileList, commissionUid);
//}


//bool TpsMachineSettingManager::DeleteMeasureProfile(RtMeasuredprofile*& measureProfile)
//{
//    return Delete_MapVector(mMeasureProfileList, *measureProfile);
//}

//bool TpsMachineSettingManager::GetMeasureProfile(std::string measureProfileuid, RtMeasuredprofile*& measureProfile){
//
//    return Get_MapVectorByPKey(mMeasureProfileList, measureProfileuid, measureProfile);
//}

//bool TpsMachineSettingManager::AddMeasureProfile(RtMeasuredprofile& measureProfile){
//
//    return Add_MapVector(mMeasureProfileList, measureProfile);
//}

//bool TpsMachineSettingManager::GetAccessoryUID(std::vector<std::string>& accessoryUid){
//
//    for (auto it = mAccessoryMap.begin(); it != mAccessoryMap.end(); ++it){
//        accessoryUid.push_back(it->first);
//    }
//    return true;
//}

//bool TpsMachineSettingManager::AddKernelWeight(RtKernelweight* kernelWeight){
//
//    return Add_MapVector(mKernelWeightMap, *kernelWeight);
//}

//bool TpsMachineSettingManager::DeleteKernelWeight(std::string commissionUID){
//
//    return Delete_MapVectorByFKey(mKernelWeightMap,commissionUID);
//}
//
//bool TpsMachineSettingManager::GetKernelWeight(std::string uid, std::vector<RtKernelweight*> &kernelWeightList){
//    return Get_MapVectorByFKey(mKernelWeightMap, uid, kernelWeightList);
//}

bool TpsMachineSettingManager::ParseStringToVector(std::string& kernelweightfilepath,std::string& kernelidlistfilepath,
    std::vector<float>& vecFValue,std::vector<int>& vecIValue ){

    std::string strKernelWeightList;
    std::vector<std::string> vecValue;

    ReadFile_i(kernelweightfilepath,strKernelWeightList);
    if (strKernelWeightList !=""){

        (void)boost::split(vecValue,strKernelWeightList, boost::is_any_of(","));
        for (int i=0; i<vecValue.size();i++){

            if(vecValue[i]!="")
                vecFValue.push_back(static_cast<float>(atof(vecValue[i].c_str())));
        }

        vecValue.clear();
    }

    strKernelWeightList = "";
    ReadFile_i(kernelidlistfilepath,strKernelWeightList);
    if (strKernelWeightList !=""){

        (void)boost::split(vecValue,strKernelWeightList, boost::is_any_of(","));
        for (int i=0; i<vecValue.size();i++){

            if(vecValue[i]!="")
                vecIValue.push_back(static_cast<int>(atof(vecValue[i].c_str())));
        }
        vecValue.clear();

    }
    return true;
}

//bool TpsMachineSettingManager::DeleteWedgeFactorList(std::string commissionUid){
//
//    return Delete_MapVectorByFKey(mWedgeFactorList, commissionUid);
//}


//bool TpsMachineSettingManager::GetWedgeFactorList(std::string uid,std::vector<RtWedgefactor*>& wedgeFactorList ){
//
//    return Get_MapVectorByFKey(mWedgeFactorList, uid, wedgeFactorList);
//}


//bool TpsMachineSettingManager::GetTrayFactorList(std::string uid,std::vector<RtCommissionedunit_tray*>& trayFactorList ){
//
//    auto trayFactor = mTrayFactorMap.find(uid);
//    if (trayFactor == mTrayFactorMap.end()){
//        TPS_LOG_DEV_ERROR<<"The commission uid does not exist which UID: "<< uid;
//        return false;
//    }
//    trayFactorList = trayFactor->second;
//
//    return true;
//}

//bool TpsMachineSettingManager::UpdateTrayFactor(RtCommissionedunit_tray& trayFactor){
//
//    std::string commissionUID = trayFactor.get_commissionedunituid();
//    auto comTrayFactor = mTrayFactorMap.find(commissionUID);
//    if (comTrayFactor == mTrayFactorMap.end()){
//        TPS_LOG_DEV_ERROR << "the commission does not exist which UID: " << commissionUID;
//        return false;
//    }
//
//    for (auto item = mTrayFactorMap[commissionUID].begin(); item != mTrayFactorMap[commissionUID].end(); item++)
//    {
//        if ((*item)->get_trayuid() == trayFactor.get_trayuid())
//        {
//            *(*item) = trayFactor;
//            return true;
//        }
//    }
//    return false;
//}

//bool TpsMachineSettingManager::AddTrayFactor(RtCommissionedunit_tray& trayFactor){
//
//    const std::string commissUID = trayFactor.get_commissionedunituid();
//    auto it = mTrayFactorMap.find(commissUID);
//    if (it == mTrayFactorMap.end()){
//        std::vector<RtCommissionedunit_tray*>* vect = new std::vector<RtCommissionedunit_tray*>();
//        mTrayFactorMap[commissUID] = *vect;
//    }
//
//    for (auto item = mTrayFactorMap[commissUID].begin(); item != mTrayFactorMap[commissUID].end(); item++)
//    {
//        if ((*item)->get_trayuid() == trayFactor.get_trayuid())
//        {
//            TPS_LOG_DEV_WARNING << "The trayFactor exist which UID: " << trayFactor.get_trayuid();
//            return false;
//        }
//
//    }
//
//    RtCommissionedunit_tray* cloneaddunittray = new RtCommissionedunit_tray(trayFactor);
//    mTrayFactorMap[commissUID].push_back(cloneaddunittray);
//
//    return true;
//}

//
//bool TpsMachineSettingManager::DeleteTrayFactor(std::string commissionUID){
//
//    return Delete_MapVectorByFKey(mTrayFactorMap, commissionUID);
//}

//bool TpsMachineSettingManager::GetPhoCommissionUnit(const std::string& commissionUID,RtMcphocommissionedunit*& phoCommUnit){
//
//    auto it = mPhoCommissionUnitMap.find(commissionUID);
//    if (it == mPhoCommissionUnitMap.end()){
//        TPS_LOG_DEV_ERROR<<"The mcphCommissionUnit does not exist which UID: "<< commissionUID;
//        return false;
//    }
//
//    phoCommUnit = it->second;
//
//    return true;
//}

//bool TpsMachineSettingManager::GetContourInfo(std::string& contourUID,RtContour*& contour){
//
//    auto it = mContourMap.find(contourUID);
//    if (it == mContourMap.end()){
//        TPS_LOG_DEV_ERROR<<"The contourUID does not exist which UID: "<< contourUID;
//        return false;
//    }
//    contour = it->second;
//
//    return true;
//
//}
//
//bool TpsMachineSettingManager::AddContourInfo(const RtContour& contour){
//
//    const std::string contourUID = contour.get_uid();
//    auto it = mContourMap.find(contourUID);
//    if (it != mContourMap.end()){
//        TPS_LOG_DEV_ERROR<<"The contourUID exist which UID: "<<contourUID;
//        return false;
//    }
//
//    RtContour* cloneContour  = new RtContour(contour);
//    mContourMap[contourUID] = cloneContour;
//
//    return true;
//}

//bool TpsMachineSettingManager::AddMachineUIDToTray(std::string& machineUID,std::vector<RtTray*>vTrayMachine){
//
//    auto it = mMachineUIDToTray.find(machineUID);
//    if (it != mMachineUIDToTray.end()){
//        TPS_LOG_DEV_ERROR<<"The machine already exist which UID: "<< machineUID;
//        return false;
//    }
//
//    mMachineUIDToTray[machineUID] = vTrayMachine;
//
//    return true;
//}

//bool TpsMachineSettingManager::DeleteTray(std::string machineUID){
//
//    auto it = mMachineUIDToTray.find(machineUID);
//    if (it == mMachineUIDToTray.end()){
//        TPS_LOG_DEV_WARNING << "The machine does not exist which UID: " << machineUID;
//        return true;
//    }
//
//    DEL_PTRVECTOR(it->second);
//    mMachineUIDToTray.erase(it);
//    return true;
//}


//bool TpsMachineSettingManager::GetTray(std::string machineUID,std::vector<RtTray*>&trayUIDList){
//
//    auto it = mMachineUIDToTray.find(machineUID);
//    if (it == mMachineUIDToTray.end()){
//        TPS_LOG_DEV_ERROR<<"The machine does not exist which UID: "<< machineUID;
//        return false;
//    }
//
//    trayUIDList = it->second;
//
//    return true;
//}

//bool TpsMachineSettingManager::UpdateTray(std::string& machineUID,std::vector<RtTray*>&trayList){
//
//    auto it = mMachineUIDToTray.find(machineUID);
//    if (it == mMachineUIDToTray.end()){
//        TPS_LOG_DEV_ERROR<<"The machine does not exist which UID: "<< machineUID;
//        return false;
//    }
//
//    mMachineUIDToTray[machineUID] = trayList;
//
//    return true;
//}

//bool TpsMachineSettingManager::AddTray(const RtTray& tray){
//
//    auto it = mMachineUIDToTray.find(tray.get_machineuid());
//    if (it == mMachineUIDToTray.end()){
//        std::vector<RtTray*> trayList;
//        RtTray* clonetray = new RtTray(tray);
//        trayList.push_back(clonetray);
//        mMachineUIDToTray[tray.get_machineuid()] = trayList;
//    }
//    else
//    {
//        auto item = mMachineUIDToTray[tray.get_machineuid()].begin();
//        for (; item != mMachineUIDToTray[tray.get_machineuid()].end(); item++)
//        {
//            if ((*item)->get_uid() == tray.get_uid())
//                break;
//        }
//
//        if (item != mMachineUIDToTray[tray.get_machineuid()].end())
//        {
//            TPS_LOG_DEV_WARNING << "The tray exist which UID:" << tray.get_uid();
//            return false;
//        }
//        else
//        {
//            RtTray* cloneaddTray = new RtTray(tray);
//            mMachineUIDToTray[tray.get_machineuid()].push_back(cloneaddTray);
//        }
//    }
//    return true;
//}

//bool TpsMachineSettingManager::GetAccessoryUID(std::string& accessoryName,std::string& accessoryUID){
//
//    auto it = mAccessoryNameToAccessoryUID.find(accessoryName);
//    if (it == mAccessoryNameToAccessoryUID.end()){
//        TPS_LOG_DEV_ERROR<<"The accessory does not exist which UID: "<< accessoryUID;
//        return false;
//    }
//
//    accessoryUID = it->second;
//
//    return true;
//}
//
//bool TpsMachineSettingManager::UpdateAccessoryUID(std::string& accessoryName,std::string accessoryUID){
//
//    auto it = mAccessoryNameToAccessoryUID.find(accessoryName);
//    if (it == mAccessoryNameToAccessoryUID.end()){
//        TPS_LOG_DEV_ERROR<<"The accessory does not exist which UID: "<< accessoryUID;
//        return false;
//    }
//
//    mAccessoryNameToAccessoryUID[accessoryName] = accessoryUID;
//
//    return true;
//}

//bool TpsMachineSettingManager::AddAccessoryUID(std::string& accessoryName,std::string accessoryUID){
//
//    auto it = mAccessoryNameToAccessoryUID.find(accessoryName);
//    if (it != mAccessoryNameToAccessoryUID.end()){
//        TPS_LOG_DEV_ERROR<<"The accessory  exist which UID: "<< accessoryUID;
//        return false;
//    }
//
//    mAccessoryNameToAccessoryUID[accessoryName] = accessoryUID;
//
//    return true;
//}

//bool TpsMachineSettingManager::DeleteAccessoryUID(std::string accessoryName){
//
//    auto it = mAccessoryNameToAccessoryUID.find(accessoryName);
//    if (it == mAccessoryNameToAccessoryUID.end()){
//        TPS_LOG_DEV_ERROR<<"The accessory not exist which UID: "<< accessoryName;
//        return false;
//    }
//
//    mAccessoryNameToAccessoryUID.erase(it);
//
//    return true;
//}

//bool TpsMachineSettingManager::GetTrayuidToTray(std::string trayUID,RtTray*& tray){
//
//    auto it = mTrayUIDtoTray.find(trayUID);
//    if (it == mTrayUIDtoTray.end()){
//        TPS_LOG_DEV_ERROR<<"The tray does not exist which UID: "<< trayUID;
//        return false;
//    }
//
//    tray = it->second;
//
//    return true;
//}

//bool TpsMachineSettingManager::UpdateTrayuidToTray(RtTray* tray){
//
//    mTrayUIDtoTray[tray->get_uid()] = tray;
//
//    return true;
//}

//bool TpsMachineSettingManager::AddTrayuidToTray(RtTray* tray){
//
//    auto it = mTrayUIDtoTray.find(tray->get_uid());
//    if (it != mTrayUIDtoTray.end()){
//        TPS_LOG_DEV_ERROR<<"The tray  exist which UID: "<< tray->get_uid();
//        return false;
//    }
//
//    mTrayUIDtoTray[tray->get_uid()] = tray;
//
//    return true;
//}

//bool TpsMachineSettingManager::AddWedgeFactor(const RtWedgefactor& wedgeFactor){
//
//    return Add_MapVector(mWedgeFactorList, wedgeFactor);
//}
//
//bool TpsMachineSettingManager::DeleteAccessoryChunk(const std::string commissionuid)
//{
//    return Delete_MapVectorByPKey(mAccessoryChunkMap, commissionuid);
//}
//
//bool TpsMachineSettingManager::GetAccessoryChunkByCommissionUid(const std::string commissionUid, std::vector<RtAccessorychunk*> &accessorychunkList)
//{
//    return Get_MapVectorByFKey(mAccessoryChunkMap, commissionUid, accessorychunkList);
//}
//
//bool TpsMachineSettingManager::AddAccessoryChunk(const RtAccessorychunk& accessorychunk)
//{
//    return Add_MapVector(mAccessoryChunkMap, accessorychunk);
//}

TPS_END_NAMESPACE
