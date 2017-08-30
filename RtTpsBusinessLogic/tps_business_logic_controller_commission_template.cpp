//////////////////////////////////////////////////////////////////////////
/// \defgroup MS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  linqi.liu linqi.liu@united-imaging.com
///
///  \file    ms_comca_machiesettingimpl.cpp
///  \brief   IMsCommonController
///
///  \version 1.0
///  \date    OT.27, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include<stdio.h>
#include <vector>
//#include "iosfwd"
//#include <ios>
#include <fstream>

#include "tps_logger.h"
#include "RtTpsDataAccess/tps_da_machineconfigmanager.h"
#include "RtTpsBusinessLogic/tps_business_logic_controller.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_machinetables.h"
#include <boost/serialization/vector.hpp> 
#include "RtTpsDatabaseWrapper/rt_tps_database_uid_generater.h"
#include "RtTpsFramework/tps_fw_common_datastructure.h"

TPS_BEGIN_NAMESPACE

bool TpsBLController::SaveMachineAsTemplate(const std::string& machineUid, std::string machineTemplateName, const std::string& savePath)
{


	std::string saveFilePath = savePath + machineTemplateName + ".xml";

	MachineTemplate machineTemplate;
	TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();

	RtMachine machine;
	if (manager->GetTable<RtMachineTable>()->Get<Tag_UniQueKey>(boost::make_tuple(machineUid), machine) <= 0)
	{
		TPS_LOG_DEV_ERROR << "Fail to Get Machine From manager";
		return false;
	}
	std::string machine_Uid = machine.get_uid();
	machine.set_name(machineTemplateName);

	machineTemplate.vMachine.push_back(machine);
	manager->GetTable<RtTrayTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(machine_Uid), machineTemplate.vTray);
	manager->GetTable<RtApplicatorTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(machine_Uid), machineTemplate.vElectronApplicator);
	manager->GetTable<RtAccessoryTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(machine_Uid), machineTemplate.vAccessory);
	manager->GetTable<RtCommissionedunitTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(machine_Uid), machineTemplate.vCommissionUnit);
	

	for (auto commissionUnit = machineTemplate.vCommissionUnit.begin(); commissionUnit != machineTemplate.vCommissionUnit.end(); commissionUnit++)
	{
		std::string comUid = commissionUnit->get_uid();

		RtMcphocommissionedunit mcphocommissionedunit;
		if (manager->GetTable<RtMcphocommissionedunitTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUid), mcphocommissionedunit) > 0)
		{
			machineTemplate.vMcphocommissionedunit.push_back(mcphocommissionedunit);
		}

		std::vector<RtCommissionedunit_tray> unit_traylist;
		manager->GetTable<RtCommissionedunit_trayTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUid), unit_traylist);
		machineTemplate.vTrayFactor.insert(machineTemplate.vTrayFactor.end(), unit_traylist.begin(), unit_traylist.end());

		std::vector<RtGoldenstt> goldsstList;
		manager->GetTable<RtGoldensttTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUid), goldsstList);
		machineTemplate.vGoldsst.insert(machineTemplate.vGoldsst.end(), goldsstList.begin(), goldsstList.end());

		std::vector<RtAccessorychunk> accessoryChunkList;
		manager->GetTable<RtAccessorychunkTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUid), accessoryChunkList);
		machineTemplate.vAccessoryChunk.insert(machineTemplate.vAccessoryChunk.end(), accessoryChunkList.begin(), accessoryChunkList.end());

		std::vector<RtKernelweight> kernelWeightList;
		manager->GetTable<RtKernelweightTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUid), kernelWeightList);
		machineTemplate.vKernelWeight.insert(machineTemplate.vKernelWeight.end(), kernelWeightList.begin(), kernelWeightList.end());

		std::vector<RtMeasureddata> measuredataList;
		manager->GetTable<RtMeasureddataTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUid), measuredataList);
		machineTemplate.vMeasuerData.insert(machineTemplate.vMeasuerData.end(), measuredataList.begin(), measuredataList.end());

		std::vector<RtWedgefactor> wedgeFactorList;
		manager->GetTable<RtWedgefactorTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUid), wedgeFactorList);
		machineTemplate.vWedgeFactor.insert(machineTemplate.vWedgeFactor.end(), wedgeFactorList.begin(), wedgeFactorList.end());
	}

	std::ofstream fout(saveFilePath);// 把对象写到file.txt文件中  
	boost::archive::xml_oarchive oa(fout);
	oa << BOOST_SERIALIZATION_NVP(machineTemplate);
	fout.close();
	return true;
}

int TpsBLController::LoadMachineTemplate(const string& templateFilePath, std::vector<std::string>& machineUids)
{
	MachineTemplate machineTemplate;

	std::map<std::string, RtMachine*> copyMachineMap;
	std::map<std::string, RtTray*> copyTrayMap;
	std::map<std::string, RtApplicator*> copyElectronApplicatorMap;
	std::map<std::string, RtCommissionedunit*> copyCommissionUnitMap;
	std::map<std::string, RtAccessory*> copyAccessoryMap;

	std::map<std::string, RtMcphocommissionedunit*> copyMcphocommissionedunitMap;
	std::map<std::string, RtGoldenstt*> copygoldsstMap;
	std::map<std::string, RtAccessorychunk*>  copyaccessoryChunkMap;
	std::map<std::string, RtContour*> copyContourMap;

	std::map<std::string, RtKernelweight*>  copykernelWeightMap;
	std::map<std::string, RtMeasureddata*>  copyMeasuredDataMap;
	std::map<std::string, RtWedgefactor*> copyWedgeFactorMap;


	std::ifstream fin(templateFilePath);
	boost::archive::xml_iarchive ia(fin);//文本的输入归档类
	ia >> BOOST_SERIALIZATION_NVP(machineTemplate);//恢复到newobj对象
	fin.close();

	bool loadSuccess = true;
	mDatabaseWrapper->BeginTrans();

	for (auto machine = machineTemplate.vMachine.begin(); machine != machineTemplate.vMachine.end(); machine++)
	{
		copyMachineMap[machine->get_uid()] = machine._Ptr;
		machine->set_uid(RtUidGenerater::GeneraterUid());
		machine->setdirty_all_fields(true);
		machineUids.push_back(machine->get_uid());
		machine->set_deviceid("");

		std::string sMD5 = mDatabaseWrapper->CalcHash((const void*)machine._Ptr, RtDbDef::TYPE_MACHINE);
		machine->set_crc(sMD5);

		if (mDatabaseWrapper->InsertMachine(*machine) != DB_CODES_SUCCESS)
		{
			loadSuccess &= false;
			break;
		}

		for (auto tray = machineTemplate.vTray.begin(); tray != machineTemplate.vTray.end(); tray++)
		{
			copyTrayMap[tray->get_uid()] = tray._Ptr;
			tray->set_uid(RtUidGenerater::GeneraterUid());

			auto machineItem = copyMachineMap.find(tray->get_machineuid());
			if (machineItem == copyMachineMap.end())
			{
				loadSuccess &= false;
				break;
			}
			tray->set_machineuid(machineItem->second->get_uid());
			tray->setdirty_all_fields(true);
			if (mDatabaseWrapper->InsertTray(*tray) != DB_CODES_SUCCESS)
			{
				loadSuccess &= false;
				break;
			}

		}
		for (auto accessory = machineTemplate.vAccessory.begin(); accessory != machineTemplate.vAccessory.end(); accessory++)
		{
			copyAccessoryMap[accessory->get_uid()] = accessory._Ptr;
			accessory->set_uid(RtUidGenerater::GeneraterUid());

			auto machineItem = copyMachineMap.find(accessory->get_machineuid());
			if (machineItem == copyMachineMap.end())
			{
				loadSuccess &= false;
				break;
			}
			accessory->set_machineuid(machineItem->second->get_uid());
			accessory->setdirty_all_fields(true);
			if (mDatabaseWrapper->InsertAccessory(*accessory) != DB_CODES_SUCCESS)
			{
				loadSuccess &= false;
				break;
			}
		}
		for (auto electronApplicator = machineTemplate.vElectronApplicator.begin(); electronApplicator != machineTemplate.vElectronApplicator.end(); electronApplicator++)
		{
			copyElectronApplicatorMap[electronApplicator->get_uid()] = electronApplicator._Ptr;
			electronApplicator->set_uid(RtUidGenerater::GeneraterUid());

			auto machineItem = copyMachineMap.find(electronApplicator->get_machineuid());
			if (machineItem == copyMachineMap.end())
			{
				loadSuccess &= false;
				break;
			}
			electronApplicator->set_machineuid(machineItem->second->get_uid());
			electronApplicator->setdirty_all_fields(true);
			if (mDatabaseWrapper->InsertApplicator(*electronApplicator) != DB_CODES_SUCCESS)
			{
				loadSuccess &= false;
				break;
			}
		}
		for (auto commissionUnit = machineTemplate.vCommissionUnit.begin(); commissionUnit != machineTemplate.vCommissionUnit.end(); commissionUnit++)
		{
			copyCommissionUnitMap[commissionUnit->get_uid()] = commissionUnit._Ptr;
			commissionUnit->set_uid(RtUidGenerater::GeneraterUid());

			auto machineItem = copyMachineMap.find(commissionUnit->get_machineuid());
			if (machineItem == copyMachineMap.end())
			{
				loadSuccess &= false;
				break;
			}
			commissionUnit->set_machineuid(machineItem->second->get_uid());
			commissionUnit->setdirty_all_fields(true);
			if (mDatabaseWrapper->InsertCommissionedunit(*commissionUnit) != DB_CODES_SUCCESS)
			{
				loadSuccess &= false;
				break;
			}
		}

		for (auto mcphocommissionedunit = machineTemplate.vMcphocommissionedunit.begin(); mcphocommissionedunit != machineTemplate.vMcphocommissionedunit.end(); mcphocommissionedunit++)
		{
			copyMcphocommissionedunitMap[mcphocommissionedunit->get_uid()] = mcphocommissionedunit._Ptr;
			mcphocommissionedunit->set_uid(RtUidGenerater::GeneraterUid());

			auto commissionedunitItem = copyCommissionUnitMap.find(mcphocommissionedunit->get_commissionedunituid());
			if (commissionedunitItem == copyCommissionUnitMap.end())
			{
				loadSuccess &= false;
				break;
			}
			mcphocommissionedunit->set_commissionedunituid(commissionedunitItem->second->get_uid());
			mcphocommissionedunit->setdirty_all_fields(true);
			if (mDatabaseWrapper->InsertMcphocommissionedunit(*mcphocommissionedunit) != DB_CODES_SUCCESS)
			{
				loadSuccess &= false;
				break;
			}
		}

		for (auto goldsst = machineTemplate.vGoldsst.begin(); goldsst != machineTemplate.vGoldsst.end(); goldsst++)
		{
			copygoldsstMap[goldsst->get_uid()] = goldsst._Ptr;
			goldsst->set_uid(RtUidGenerater::GeneraterUid());

			auto commissionedunitItem = copyCommissionUnitMap.find(goldsst->get_commissionedunituid());
			if (commissionedunitItem == copyCommissionUnitMap.end())
			{
				loadSuccess &= false;
				break;
			}
			goldsst->set_commissionedunituid(commissionedunitItem->second->get_uid());

			auto accessoryItem = copyAccessoryMap.find(goldsst->get_accessoryuid());
			if (accessoryItem == copyAccessoryMap.end())
			{
				loadSuccess &= false;
				break;
			}
			goldsst->set_accessoryuid(accessoryItem->second->get_uid());

			goldsst->setdirty_all_fields(true);

			if (mDatabaseWrapper->InsertGoldenstt(*goldsst) != DB_CODES_SUCCESS)
			{
				loadSuccess &= false;
				break;
			}

		}

		for (auto accessoryChunk = machineTemplate.vAccessoryChunk.begin(); accessoryChunk != machineTemplate.vAccessoryChunk.end(); accessoryChunk++)
		{
			copyaccessoryChunkMap[accessoryChunk->get_uid()] = accessoryChunk._Ptr;
			accessoryChunk->set_uid(RtUidGenerater::GeneraterUid());

			auto commissionedunitItem = copyCommissionUnitMap.find(accessoryChunk->get_commissionedunituid());
			if (commissionedunitItem == copyCommissionUnitMap.end())
			{
				loadSuccess &= false;
				break;
			}
			accessoryChunk->set_commissionedunituid(commissionedunitItem->second->get_uid());

			auto accessoryItem = copyAccessoryMap.find(accessoryChunk->get_accessoryuid());
			if (accessoryItem == copyAccessoryMap.end())
			{
				loadSuccess &= false;
				break;
			}
			accessoryChunk->set_accessoryuid(accessoryItem->second->get_uid());

			accessoryChunk->setdirty_all_fields(true);

			if (mDatabaseWrapper->InsertAccessorychunk(*accessoryChunk) != DB_CODES_SUCCESS)
			{
				loadSuccess &= false;
				break;
			}
		}

		for (auto contour = machineTemplate.vContour.begin(); contour != machineTemplate.vContour.end(); contour++)
		{
			copyContourMap[contour->get_uid()] = contour._Ptr;
			contour->set_uid(RtUidGenerater::GeneraterUid());

			auto accessorychunkItem = copyaccessoryChunkMap.find(contour->get_accessorychunkuid());
			if (accessorychunkItem == copyaccessoryChunkMap.end())
			{
				loadSuccess &= false;
				break;
			}
			contour->set_accessorychunkuid(accessorychunkItem->second->get_uid());
			contour->setdirty_all_fields(true);
			if (mDatabaseWrapper->InsertContour(*contour) != DB_CODES_SUCCESS)
			{
				loadSuccess &= false;
				break;
			}
		}

		for (auto kernelWeight = machineTemplate.vKernelWeight.begin(); kernelWeight != machineTemplate.vKernelWeight.end(); kernelWeight++)
		{
			copykernelWeightMap[kernelWeight->get_uid()] = kernelWeight._Ptr;
			kernelWeight->set_uid(RtUidGenerater::GeneraterUid());

			auto commissionedunitItem = copyCommissionUnitMap.find(kernelWeight->get_commissionedunituid());
			if (commissionedunitItem == copyCommissionUnitMap.end())
			{
				loadSuccess &= false;
				break;
			}
			kernelWeight->set_commissionedunituid(commissionedunitItem->second->get_uid());
			kernelWeight->setdirty_all_fields(true);
			if (mDatabaseWrapper->InsertKernelweight(*kernelWeight) != DB_CODES_SUCCESS)
			{
				loadSuccess &= false;
				break;
			}
		}
			
		for (auto measureData = machineTemplate.vMeasuerData.begin(); measureData != machineTemplate.vMeasuerData.end(); measureData++)
		{
			copyMeasuredDataMap[measureData->get_uid()] = measureData._Ptr;
			measureData->set_uid(RtUidGenerater::GeneraterUid());

			auto commissionedunitItem = copyCommissionUnitMap.find(measureData->get_commissionedunituid());
			if (commissionedunitItem == copyCommissionUnitMap.end())
			{
				loadSuccess &= false;
				break;
			}
			measureData->set_commissionedunituid(commissionedunitItem->second->get_uid());
			measureData->setdirty_all_fields(true);
			if (mDatabaseWrapper->InsertMeasureddata(*measureData) != DB_CODES_SUCCESS)
			{
				loadSuccess &= false;
				break;
			}
		}

		for (auto wedgeFactor = machineTemplate.vWedgeFactor.begin(); wedgeFactor != machineTemplate.vWedgeFactor.end(); wedgeFactor++)
		{
			copyWedgeFactorMap[wedgeFactor->get_uid()] = wedgeFactor._Ptr;
			wedgeFactor->set_uid(RtUidGenerater::GeneraterUid());

			auto commissionedunitItem = copyCommissionUnitMap.find(wedgeFactor->get_commissionedunituid());
			if (commissionedunitItem == copyCommissionUnitMap.end())
			{
				loadSuccess &= false;
				break;
			}
			wedgeFactor->set_commissionedunituid(commissionedunitItem->second->get_uid());
			wedgeFactor->setdirty_all_fields(true);
			if (mDatabaseWrapper->InsertWedgefactor(*wedgeFactor) != DB_CODES_SUCCESS)
			{
				loadSuccess &= false;
				break;
			}
		}
			
		for (auto trayFactor = machineTemplate.vTrayFactor.begin(); trayFactor != machineTemplate.vTrayFactor.end(); trayFactor++)
		{
			auto commissionedunitItem = copyCommissionUnitMap.find(trayFactor->get_commissionedunituid());
			if (commissionedunitItem == copyCommissionUnitMap.end())
			{
				loadSuccess &= false;
				break;
			}
			trayFactor->set_commissionedunituid(commissionedunitItem->second->get_uid());

			auto trayItem = copyTrayMap.find(trayFactor->get_trayuid());
			if (trayItem == copyTrayMap.end())
			{
				loadSuccess &= false;
				break;
			}
			trayFactor->set_trayuid(trayItem->second->get_uid());

			if (mDatabaseWrapper->InsertCommissionedunit_tray(*trayFactor) != DB_CODES_SUCCESS)
			{
				loadSuccess &= false;
				break;
			}
		}

		if (loadSuccess)
		{
			mDatabaseWrapper->Commit();
			for (int i = 0; i < machineUids.size(); i++)
			{
				if (LoadMachineSettingFromDB(machineUids[i]))
				{
					return TPS_ER_SUCCESS;
				}
				else
				{
					return TPS_ER_FAILURE;
				}
			}
			return TPS_ER_SUCCESS;
		}
		else
		{
			machineUids.clear();
			mDatabaseWrapper->RollBack();
			return TPS_ER_FAILURE;
		}
	}
	return TPS_ER_FAILURE;
}


TPS_END_NAMESPACE
