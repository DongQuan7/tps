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
#include<stdlib.h>
#include<string.h>
#include <math.h>
#include <vector>

#include "tps_logger.h"

#include "RtTpsBusinessLogic/tps_business_logic_controller.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsBusinessLogic/tps_bl_load_objects.h"

#include "alg/alg_commission.h"
#include "boost/filesystem.hpp"//create directory
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"

#include "boost/lexical_cast.hpp"
using boost::lexical_cast;

#include "alg/RtAlgDataLayer/rt_alg_dl_object_base.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_machine.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_mlcsetting.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_jawlimits.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_commissionedunit.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_accessory.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_applicator.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_mlcshape.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_measureddata.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_wedgefactor.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_measuredprofile.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_kernelweight.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_accessorychunk.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_convdatastruct.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_convolutiondata.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_sartable.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_machineunit.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_goldenstt.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_mcphomonoenergyweight.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_monocompressedphasespacedata.h"

#include "alg/RtAlgDataLayer/rt_alg_dl_object_contour.h"
#include "alg/alg_vw_interface.h"
#include "alg/alg_lsvw_common.h"
#include "alg/alg_commission_prenotifier.h"


#include "RtTpsDataAccess/tps_da_machineconfigmanager.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsCertifiedLib/rt_tps_certifiedmachinesetting.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedimportfromfile.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedwedgefactor.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedconvolution.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedpencilbeam.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedmontecarlo.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedimportgsttfromfile.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedelectroncontamination.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedenergyweight.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedwedgefield.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedscpinformation.h"


#include "RtTpsProtoLib/rt_ms_convolution.pb.h"
#include "RtTpsProtoLib/rt_ms_pencilbeam.pb.h"
#include "RtTpsProtoLib/rt_ms_montecarlo.pb.h"
#include "RtTpsProtoLib/rt_ms_importfromgsstfile.pb.h"
#include "RtTpsProtoLib/rt_ms_wedgefactorSetting.pb.h"
#include "RtTpsProtoLib/rt_ms_proto.pb.h"
#include "RtTpsProtoLib/rt_ms_machinesettingconfig.pb.h"
#include "RtTpsProtoLib/rt_ms_photonsmontecarlo.pb.h"


#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mcphomonoenergyweight.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_monocompressedphasespacedata.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_uid_generater.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_data.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "../RtTpsDoseProxy/machine_converter.h"
#include "../RtTpsDoseProxy/commissioned_unit_converter.h"
#include "../RtTpsDoseProxy/accessory_converter.h"
#include "../RtTpsDoseProxy/accessory_chunk_converter.h"
#include "../RtTpsDoseProxy/kernel_weight_converter.h"
#include "../RtTpsDoseProxy/mcphocommissioned_unit_converter.h"
#include "../RtTpsDoseProxy/goldenstt_converter.h"
#include "RtTpsDoseProxy/tps_dp_algcommissionproxy.h"
#include "RtTpsFramework/tps_fw_prepare_notifier.h"
#include "../RtTpsDoseProxy/measureprofile_converter.h"
#include "RtTpsProtoLib/rt_ms_importfromfile.pb.h"
#include "RtTpsAlgorithmProxy/tps_ap_utility.h"
#include "RtTpsDataAccess/tps_da_machinetables.h"

#define MAX_SAR_DEPTHS		(40)
#define MAX_SAR_RADII		(30)
#define MAX_FLATNESS_DEPTHS	(5)
#define MAX_FLATNESS_RADII	(40)
#define SAVE_TO_DB_CONVERTER (10)




TPS_BEGIN_NAMESPACE



void CertifiedToRTAccessoryConverter(
RtAccessory* accessory, std::tr1::shared_ptr<CertifiedWedgeInfo> pWedgeInfoList,
std::string machineUid,
float sourceToWedgeDistance)
{
    accessory->set_machineuid(machineUid);
    accessory->set_name(pWedgeInfoList->wedgeACCID);
    accessory->set_code(pWedgeInfoList->wedgeACCcode);
    accessory->set_wedgeangle(pWedgeInfoList->degree);
    accessory->set_accessorytype(pWedgeInfoList->wedgeType);

    std::string filtertobeam = "";
    RtDbDef::WEDGE_ORIENTATION orientationType = RtDbDef::WEDGE_ORIENTATION_UNKNOWN;
    if (pWedgeInfoList->orientationType == "In")
    {
        orientationType = RtDbDef::TOE_IN;
        filtertobeam = "0,-1,0,0,0,0,1,0,1,0,0,0,0,0," + boost::lexical_cast<std::string>(sourceToWedgeDistance)+",1";
    }
    if (pWedgeInfoList->orientationType == "Out")
    {
        orientationType = RtDbDef::TOE_OUT;
        filtertobeam = "0,1,0,0,0,0,1,0,1,0,0,0,0,0," + boost::lexical_cast<std::string>(sourceToWedgeDistance)+",1";
    }
    if (pWedgeInfoList->orientationType == "Left")
    {
        orientationType = RtDbDef::TOE_LEFT;
        filtertobeam = "1,0,0,0,0,0,1,0,0,1,0,0,0,0," + boost::lexical_cast<std::string>(sourceToWedgeDistance)+",1";
    }
    if (pWedgeInfoList->orientationType == "Right")
    {
        orientationType = RtDbDef::TOE_RIGHT;
        filtertobeam = "-1,0,0,0,0,0,1,0,0,1,0,0,0,0," + boost::lexical_cast<std::string>(sourceToWedgeDistance)+",1";
    }

    accessory->set_filtertobeam(filtertobeam);
    accessory->set_wedgeorientation((int)orientationType);

    accessory->set_min1_jaw1(pWedgeInfoList->leftmin);
    accessory->set_max1_jaw1(pWedgeInfoList->leftmax);

    accessory->set_min2_jaw1(pWedgeInfoList->rightmin);
    accessory->set_max2_jaw1(pWedgeInfoList->rightmax);

    accessory->set_min1_jaw2(pWedgeInfoList->topmin);
    accessory->set_max1_jaw2(pWedgeInfoList->topmax);

    accessory->set_min2_jaw2(pWedgeInfoList->bottommin);
    accessory->set_max2_jaw2(pWedgeInfoList->bottommax);
}


    int TpsBLController::UpdateMachineSetting(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig)
    {
        TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
        RtMachine machine;
        if (manager->GetTable<RtMachineTable>()->Get<Tag_UniQueKey>(boost::make_tuple(protoMachineConfig->machinegeneralinfo().uid()), machine) <= 0)
        {
            TPS_LOG_DEV_ERROR << "Failed to get machine.";
            return TPS_ER_FAILURE;
        }

        MachineProtoToRTMachineConverter(protoMachineConfig, &machine);

        std::vector<RtTray> trayList;
        auto machineAcc = protoMachineConfig->machineaccessories();
        for (int g = 0; g < machineAcc.traylist_size(); ++g){
            RtTray tray;
            tray.set_uid(machineAcc.traylist(g).trayuid());
            tray.set_code(machineAcc.traylist(g).traycode());
            tray.set_machineuid(machine.get_uid());
            tray.set_trayid(machineAcc.traylist(g).trayid());

            trayList.push_back(tray);
        }

        for (auto it = trayList.begin(); it != trayList.end(); it++)
        {
            if (mDatabaseWrapper->UpdateTray(*it)){
                TPS_LOG_DEV_ERROR << "save tray to DB failed:";
                return TPS_ER_FAILURE;
            }

            if (!manager->GetTable<RtTrayTable>()->Update<Tag_UniQueKey>(boost::make_tuple(it->get_uid()), *it))
            {
                TPS_LOG_DEV_ERROR << "save tray to manager failed:";
                return TPS_ER_FAILURE;
            }
        }
       
        std::string sMD5 = mDatabaseWrapper->CalcHash((const void*)(&(machine)), RtDbDef::TYPE_MACHINE);
        machine.set_crc(sMD5);
        machine.set_crc_valid(true);
        if (mDatabaseWrapper->UpdateMachine(machine)){
            TPS_LOG_DEV_ERROR<<"save machine  to DB failed:";
            return TPS_ER_FAILURE;
        }

        if (!manager->GetTable<RtMachineTable>()->Update<Tag_UniQueKey>(boost::make_tuple(machine.get_uid()), machine))
        {
            TPS_LOG_DEV_ERROR << "save machine to manager failed:";
            return TPS_ER_FAILURE;
        }
        return TPS_ER_SUCCESS;
}

//manager 中没有更新unit/electronApplicator/accessery/
int TpsBLController::AddMachineSetting(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig,std::string& machineUID) {

    RtMachine machine(true);
    MachineProtoToRTMachineConverter(protoMachineConfig, &machine);

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();

    std::vector<RtTray> trayList;
    auto machineAcc = protoMachineConfig->machineaccessories();
    for (int g = 0; g < machineAcc.traylist_size(); ++g){
        RtTray tray;
        tray.set_uid(machineAcc.traylist(g).trayuid());
        tray.set_code(machineAcc.traylist(g).traycode());
        tray.set_machineuid(machine.get_uid());
        tray.set_trayid(machineAcc.traylist(g).trayid());

        trayList.push_back(tray);
    }

    for (auto it = trayList.begin(); it != trayList.end(); it++)
    {
        if (mDatabaseWrapper->UpdateTray(*it)){
            TPS_LOG_DEV_ERROR << "save tray to DB failed:";
            return TPS_ER_FAILURE;
        }

        if (!manager->GetTable<RtTrayTable>()->Add(*it))
        {
            TPS_LOG_DEV_ERROR << "save tray to manager failed:";
            return TPS_ER_FAILURE;
        }
    }

    std::string sMD5 = mDatabaseWrapper->CalcHash((const void*)&machine, RtDbDef::TYPE_MACHINE);
    machine.set_crc(sMD5);
    machine.set_crc_valid(true);
    if (mDatabaseWrapper->InsertMachine(machine)){
        TPS_LOG_DEV_ERROR << "save machine  to DB failed:";
        return TPS_ER_FAILURE;
    }

    if (!manager->GetTable<RtMachineTable>()->Add(machine)){
        TPS_LOG_DEV_ERROR<<"Failed to add machine to manager.";
        return TPS_ER_FAILURE;
    }

    machineUID = machine.get_uid();
    return TPS_ER_SUCCESS;
}

int TpsBLController::MachineProtoToRTMachineConverter(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig, RtMachine* pMachine)
{
    if (pMachine == nullptr)
    {
        return TPS_ER_FAILURE;
    }
    pMachine->set_state(protoMachineConfig->isactive());
    if (protoMachineConfig->has_machinegeneralinfo()) {
        auto machineGeneralInfo = protoMachineConfig->machinegeneralinfo();
        //General
        if (machineGeneralInfo.has_machinename())
        {
            pMachine->set_name(machineGeneralInfo.machinename());
        }
        else
        {
            pMachine->disable_field(MACHINE_NAME);
        }
        if (machineGeneralInfo.has_manufactory())
        {
            pMachine->set_manufacturer(machineGeneralInfo.manufactory());
        }
        else
        {
            pMachine->disable_field(MACHINE_MANUFACTURER);
        }
        if (machineGeneralInfo.has_deviceid())
        {
            pMachine->set_deviceid(machineGeneralInfo.deviceid());
        }
        else
        {
            pMachine->disable_field(MACHINE_DEVICEID);
        }
        if (machineGeneralInfo.has_comments())
        {
            pMachine->set_comments(machineGeneralInfo.comments());
        }
        else
        {
            pMachine->disable_field(MACHINE_COMMENTS);
        }
    }

    //Geometry
    if (protoMachineConfig->has_machinegeometry()) {
        auto machineGeometry = protoMachineConfig->machinegeometry();
        if (machineGeometry.has_gantryanglemin())
        {
            pMachine->set_gantrymin(machineGeometry.gantryanglemin());
        }
        else
        {
            pMachine->disable_field(MACHINE_GANTRYMIN);
        }
        if (machineGeometry.has_gantryanglemax())
        {
            pMachine->set_gantrymax(machineGeometry.gantryanglemax());
        }
        else
        {
            pMachine->disable_field(MACHINE_GANTRYMAX);
        }
        if (machineGeometry.has_maxspeedlimit())
        {
            pMachine->set_maximumgantryspeed_degreepersecond(machineGeometry.maxspeedlimit());
        }
        else
        {
            pMachine->disable_field(MACHINE_MAXIMUMGANTRYSPEED_DEGREEPERSECOND);
        }
        if (machineGeometry.has_sad())
        {
            pMachine->set_sad(machineGeometry.sad()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_SAD);
        }
        if (machineGeometry.has_cwlimit())
        {
            pMachine->set_gantryclockwiserange(machineGeometry.cwlimit());
        }
        else
        {
            pMachine->disable_field(MACHINE_GANTRYCLOCKWISERANGE);
        }
        if (machineGeometry.has_ccwlimit())
        {
            pMachine->set_gantrycounterclockwiserange(machineGeometry.ccwlimit());
        }
        else
        {
            pMachine->disable_field(MACHINE_GANTRYCOUNTERCLOCKWISERANGE);
        }
        if (machineGeometry.has_collimatorlow())
        {
            pMachine->set_collimatoranglemin(machineGeometry.collimatorlow());
        }
        else
        {
            pMachine->disable_field(MACHINE_COLLIMATORANGLEMIN);
        }
        if (machineGeometry.has_collimatorhigh())
        {
            pMachine->set_collimatoranglemax(machineGeometry.collimatorhigh());
        }
        else
        {
            pMachine->disable_field(MACHINE_COLLIMATORANGLEMAX);
        }
        if (machineGeometry.has_lateralmotionmax())
        {
            pMachine->set_tablelateralmax(machineGeometry.lateralmotionmax()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_TABLELATERALMAX);
        }
        if (machineGeometry.has_lateralmotionmin())
        {
            pMachine->set_tablelateralmin(machineGeometry.lateralmotionmin()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_TABLELATERALMIN);
        }
        if (machineGeometry.has_longitudemotionmin())
        {
            pMachine->set_tablelongitudinalmin(machineGeometry.longitudemotionmin()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_TABLELONGITUDINALMIN);
        }
        if (machineGeometry.has_longitudemotionmax())
        {
            pMachine->set_tablelongitudinalmax(machineGeometry.longitudemotionmax()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_TABLELONGITUDINALMAX);
        }
        if (machineGeometry.has_verticalmotionmax())
        {
            pMachine->set_tableverticalmax(machineGeometry.verticalmotionmax()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_TABLEVERTICALMAX);
        }
        if (machineGeometry.has_verticalmotionmin())
        {
            pMachine->set_tableverticalmin(machineGeometry.verticalmotionmin()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_TABLEVERTICALMIN);
        }
        if (machineGeometry.has_rtnmax())
        {
            pMachine->set_tableanglemax(machineGeometry.rtnmax());
        }
        else
        {
            pMachine->disable_field(MACHINE_TABLEANGLEMAX);
        }
        if (machineGeometry.has_rtnmin())
        {
            pMachine->set_tableanglemin(machineGeometry.rtnmin());
        }
        else
        {
            pMachine->disable_field(MACHINE_TABLEANGLEMIN);
        }

		if (machineGeometry.has_mingantryspeed_degreepersecond())
		{
			pMachine->set_mingantryspeed_degreepersecond(machineGeometry.mingantryspeed_degreepersecond());
		}
		else
		{
			pMachine->disable_field(MACHINE_MINGANTRYSPEED_DEGREEPERSECOND);
		}
    }

    if (protoMachineConfig->has_machinejaw()) {
        auto machineJaw = protoMachineConfig->machinejaw();
        // Jaw X
        if (machineJaw.has_isxasym())
        {
            pMachine->set_jawxissymmetry(machineJaw.isxasym() == 1);
            //数据库冗余字段
            //pMachine->set_xindependent(machineJaw.isxasym());
        }
        else
        {
            pMachine->disable_field(MACHINE_JAWXISSYMMETRY);
        }
        if (machineJaw.has_isyasym())
        {
            pMachine->set_jawyissymmetry(machineJaw.isyasym() == 1);
            //数据库冗余字段
            //pMachine->set_yindependent(machineJaw.isyasym());
        }
        else
        {
            pMachine->disable_field(MACHINE_JAWYISSYMMETRY);
        }
        if (machineJaw.has_maxjawspeed())
        {
            pMachine->set_maximumjawspeed_mmpersecond(machineJaw.maxjawspeed());
        }
        else
        {
            pMachine->disable_field(MACHINE_MAXIMUMJAWSPEED_MMPERSECOND);
        }
        if (machineJaw.has_isxjaw())
        {
            pMachine->set_xjawnum(machineJaw.isxjaw() == 1);
        }
        else
        {
            pMachine->disable_field(MACHINE_XJAWNUM);
        }
        if (machineJaw.has_isyjaw())
        {
            pMachine->set_yjawnum(machineJaw.isyjaw() == 1);
        }
        else
        {
            pMachine->disable_field(MACHINE_YJAWNUM);
        }
        if (machineJaw.has_leftjawlabel())
        {
            pMachine->set_xname_1(machineJaw.leftjawlabel());
        }
        else
        {
            pMachine->disable_field(MACHINE_XNAME_1);
        }
        if (machineJaw.has_rightjawlabel())
        {
            pMachine->set_xname_2(machineJaw.rightjawlabel());
        }
        else
        {
            pMachine->disable_field(MACHINE_XNAME_2);
        }
        if (machineJaw.has_lateralinsidejawedge())
        {
            pMachine->set_xmax_w(machineJaw.lateralinsidejawedge()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_XMAX_W);
        }
        if (machineJaw.has_lateraljawoutsideedge())
        {
            pMachine->set_xmin_w(machineJaw.lateraljawoutsideedge()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_XMIN_W);
        }
        if (machineJaw.has_minleftjawposition())
        {
            pMachine->set_xmin_1(machineJaw.minleftjawposition()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_XMIN_1);
        }
        if (machineJaw.has_maxleftjawposition())
        {
            pMachine->set_xmax_1(machineJaw.maxleftjawposition()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_XMAX_1);
        }
        if (machineJaw.has_maxrightjawposition())
        {
            pMachine->set_xmax_2(machineJaw.maxrightjawposition()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_XMAX_2);
        }
        if (machineJaw.has_minrightjawposition())
        {
            pMachine->set_xmin_2(machineJaw.minrightjawposition()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_XMIN_2);
        }
        if (machineJaw.has_sourcelateraljawdistance())
        {
            pMachine->set_xsdd(machineJaw.sourcelateraljawdistance()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_XSDD);
        }
        // Jaw Y
        if (machineJaw.has_maxtopjawposition())
        {
            pMachine->set_ymax_1(machineJaw.maxtopjawposition()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_YMAX_1);
        }
        if (machineJaw.has_maxbottomjawposition())
        {
            pMachine->set_ymax_2(machineJaw.maxbottomjawposition()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_YMAX_2);
        }
        if (machineJaw.has_longitudejawinsideedge())
        {
            pMachine->set_ymax_w(machineJaw.longitudejawinsideedge()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_YMAX_W);
        }
        if (machineJaw.has_mintopjawposition())
        {
            pMachine->set_ymin_1(machineJaw.mintopjawposition()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_YMIN_1);
        }
        if (machineJaw.has_minbottomjawposition())
        {
            pMachine->set_ymin_2(machineJaw.minbottomjawposition()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_YMIN_2);
        }
        if (machineJaw.has_longitudejawoutsideedge())
        {
            pMachine->set_ymin_w(machineJaw.longitudejawoutsideedge()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_YMIN_W);
        }
        if (machineJaw.has_topjawlabel())
        {
            pMachine->set_yname_1(machineJaw.topjawlabel());
        }
        else
        {
            pMachine->disable_field(MACHINE_YNAME_1);
        }
        if (machineJaw.has_bottomjawlabel())
        {
            pMachine->set_yname_2(machineJaw.bottomjawlabel());
        }
        else
        {
            pMachine->disable_field(MACHINE_YNAME_2);
        }
        if (machineJaw.has_sourcelongitudejawdistance())
        {
            pMachine->set_ysdd(machineJaw.sourcelongitudejawdistance()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_YSDD);
        }
        if (machineJaw.has_jawgap())
        {
            pMachine->set_jawtrackinggap(machineJaw.jawgap()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_JAWTRACKINGGAP);
        }
        if (machineJaw.has_lateraltracking())
        {
            pMachine->set_dynamictrackingjawx(machineJaw.lateraltracking());
        }
        else
        {
            pMachine->disable_field(MACHINE_DYNAMICTRACKINGJAWX);
        }
        if (machineJaw.has_longitudinaltranking())
        {
            pMachine->set_dynamictrackingjawy(machineJaw.longitudinaltranking());
        }
        else
        {
            pMachine->disable_field(MACHINE_DYNAMICTRACKINGJAWY);
        }
        if (machineJaw.has_maxjawspeed())
        {
            pMachine->set_maximumjawspeed_mmpersecond(machineJaw.maxjawspeed()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_MAXIMUMJAWSPEED_MMPERSECOND);
        }
    }

    if (protoMachineConfig->has_machinemlc()) {
        auto machineMLC = protoMachineConfig->machinemlc();

        if (machineMLC.has_pairs())
        {
            pMachine->set_leafnum(machineMLC.pairs());
        }
        else
        {
            pMachine->disable_field(MACHINE_LEAFNUM);
        }
        if (machineMLC.has_sourcetomlctopdistance())
        {
            pMachine->set_sourcetomlctopdistance(machineMLC.sourcetomlctopdistance()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_SOURCETOMLCTOPDISTANCE);
        }
        if (machineMLC.has_minleafjawoverlap())
        {
            pMachine->set_minimumjawgap(machineMLC.minleafjawoverlap()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
			pMachine->disable_field(MACHINE_MINIMUMJAWGAP);
        }
        if (machineMLC.has_mingap())
        {
            pMachine->set_minleafendgap(machineMLC.mingap()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
			pMachine->disable_field(MACHINE_MINLEAFENDGAP);
        }
        if (machineMLC.has_maxleafspeed())
        {
            pMachine->set_maximumleafspeed_mmpersecond(machineMLC.maxleafspeed()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_MAXIMUMLEAFSPEED_MMPERSECOND);
        }
        if (machineMLC.has_mintipposition())
        {
            pMachine->set_positiveleafrangemin(machineMLC.mintipposition()* SAVE_TO_DB_CONVERTER);
            //数据库冗余字段
            //pMachine->set_negativeleafrangemax(-machineMLC.mintipposition()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_POSITIVELEAFRANGEMIN);
        }
        if (machineMLC.has_maxtipposition())
        {
            pMachine->set_positiveleafrangemax(machineMLC.maxtipposition()* SAVE_TO_DB_CONVERTER);
            //数据库冗余字段
            //pMachine->set_negativeleafrangemin(-machineMLC.maxtipposition()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_POSITIVELEAFRANGEMAX);
        }
        if (machineMLC.has_leafinterdigitation())
        {
            pMachine->set_isinterdigitation(machineMLC.leafinterdigitation());
        }
        else
        {
            pMachine->disable_field(MACHINE_ISINTERDIGITATION);
        }
        if (machineMLC.has_maxleafoutofcarriagedistance())
        {
            pMachine->set_leafspan(machineMLC.maxleafoutofcarriagedistance()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_LEAFSPAN);
        }
        if (machineMLC.has_mlcdirection())
        {
            pMachine->set_mlcdirectiontype(machineMLC.mlcdirection());
        }
        else
        {
            pMachine->disable_field(MACHINE_MLCDIRECTIONTYPE);
        }

		if (machineMLC.has_maxleafacc_mmpersec2())
		{
			pMachine->set_maxleafacc_mmpersec2(machineMLC.maxleafacc_mmpersec2());
		}
		else
		{
			pMachine->disable_field(MACHINE_MAXLEAFACC_MMPERSEC2);
		}

        std::vector<double> pairList;
        for (int i = 0; i < machineMLC.pairinfolist_size(); ++i){
            if (machineMLC.pairinfolist(i).has_leafpairs())
            {
                pairList.push_back(machineMLC.pairinfolist(i).leafpairs()* SAVE_TO_DB_CONVERTER);
            }
            else
            {
                // 超过1000000就认为是空
                pairList.push_back(1000000);
            }
        }
        pMachine->set_leafboundaries_list(pairList);
    }


    //ACC wedge
    if (protoMachineConfig->has_machineaccessories()) {
        auto machineAcc = protoMachineConfig->machineaccessories();
        if (machineAcc.has_sourcetowedgedistance())
        {
            pMachine->set_sourcetowedgetopdistance(machineAcc.sourcetowedgedistance()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_SOURCETOWEDGETOPDISTANCE);
        }
        if (machineAcc.has_isapertureblock())
        {
            pMachine->set_supportapetureblock(machineAcc.isapertureblock());
        }
        else
        {
            pMachine->disable_field(MACHINE_SUPPORTAPETUREBLOCK);
        }
        if (machineAcc.has_isshieldblock())
        {
            pMachine->set_supportshieldblock(machineAcc.isshieldblock());
        }
        else
        {
            pMachine->disable_field(MACHINE_SUPPORTSHIELDBLOCK);
        }
        if (machineAcc.has_sourcetoblocktrydistance())
        {
            pMachine->set_sourcetoblocktraydistance(machineAcc.sourcetoblocktrydistance()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_SOURCETOBLOCKTRAYDISTANCE);
        }
        if (machineAcc.has_height())
        {
            pMachine->set_blockheight(machineAcc.height() * 10);
        }
        else
        {
            pMachine->disable_field(MACHINE_BLOCKHEIGHT);
        }
        if (machineAcc.has_sourcetoapplicatorlowdistance())
        {
            pMachine->set_sourcetoapplicatordistance(machineAcc.sourcetoapplicatorlowdistance()* SAVE_TO_DB_CONVERTER);
        }
        else
        {
            pMachine->disable_field(MACHINE_SOURCETOAPPLICATORDISTANCE);
        }
    }

    return TPS_ER_SUCCESS;
}


int TpsBLController::DeleteObject(std::string& uid,std::string& objectType){

    if (objectType == "MachineConfig"){
        if ( !DeleteMachine_i(uid)){
            TPS_LOG_DEV_ERROR <<" delete machine config ERROR !";
            return false;
        }
    }

    if (objectType == "CommissionInfo"){
        if ( !DeleteCommission_i(uid)){
            TPS_LOG_DEV_ERROR <<" delete commission info ERROR !";
            return false;
        }
    }

    if (objectType == "DeleteWedge"){
        if ( !DeleteWedgeInfo_i(uid)){
            TPS_LOG_DEV_ERROR <<" delete wedge info ERROR !";
            return false;
        }
    }
    if (objectType == "DeleteApplicator"){
        if ( !DeleteApplicatorInfo_i(uid)){
            TPS_LOG_DEV_ERROR <<" delete applicator info ERROR !";
            return false;
        }
        return true;
    }
    if (objectType == "DeleteTray"){
        if ( !DeleteTrayInfo_i(uid)){
            TPS_LOG_DEV_ERROR <<" delete tray info ERROR !";
            return false;
        }
        return true;
    }

    return true;
}

int TpsBLController::DeleteTrayInfo_i(std::string& uid){

    int bRet = mDatabaseWrapper->EraseObject(uid,RtDbDef::TYPE_TRAY);
    if (bRet) {
        TPS_LOG_DEV_ERROR<<"Failed to delete tray from DB";
        return false;
    }
    return true;
}

int TpsBLController::DeleteApplicatorInfo_i(std::string& uid){

    RtApplicator electronApp;
    TpsMachineSettingManager* manager=mDataRepository->GetMachineSettingManager();
    if (manager->GetTable<RtApplicatorTable>()->Get<Tag_UniQueKey>(boost::make_tuple(uid), electronApp) <= 0)
    {
        TPS_LOG_DEV_ERROR << "Failed to delete applicator from Manager";
        return false;
    }
    int bRet = mDatabaseWrapper->EraseObject(electronApp.get_uid(), RtDbDef::TYPE_APPLICATOR);
    if (bRet) {
        TPS_LOG_DEV_ERROR << "Failed to delete applicator from DB";
        return false;
    }

    if (!manager->GetTable<RtApplicatorTable>()->Delete<Tag_UniQueKey>(boost::make_tuple(electronApp.get_uid())))
    {
        TPS_LOG_DEV_ERROR << "Failed to delete applicator from Manager";
        return false;
    }

    return true;
}


int TpsBLController::DeleteWedgeInfo_i(std::string& uid){

    int bRet = mDatabaseWrapper->EraseObject(uid, RtDbDef::TYPE_ACCESSORY);
    if (bRet) {
        TPS_LOG_DEV_ERROR << "Failed to delete accessory from DB";
        return false;
    }

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    if (manager->GetTable<RtAccessoryTable>()->Delete<Tag_UniQueKey>(boost::make_tuple(uid)) <= 0){
        TPS_LOG_DEV_ERROR << "Failed to get accessory to manager.";
        return false;
    }
    return true;
}

int TpsBLController::DeleteCommission_i(std::string& uid){

    int bRet = mDatabaseWrapper->EraseObject(uid, RtDbDef::TYPE_COMMISSIONEDUNIT);
    if (bRet) {
        TPS_LOG_DEV_ERROR << "Failed to delete commission from DB";
        return false;
    }

    TpsMachineSettingManager*  manager = mDataRepository->GetMachineSettingManager();
    if (manager == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get manager.";
        return false;
    }

    manager->GetTable<RtCommissionedunitTable>()->Delete<Tag_UniQueKey>(boost::make_tuple(uid));

    manager->GetTable<RtMcphocommissionedunitTable>()->Delete<Tag_NonUniqueKey0>(boost::make_tuple(uid));

    manager->GetTable<RtMeasureddataTable>()->Delete<Tag_NonUniqueKey0>(boost::make_tuple(uid));

    manager->GetTable<RtCommissionedunit_trayTable>()->Delete<Tag_NonUniqueKey0>(boost::make_tuple(uid));

    manager->GetTable<RtKernelweightTable>()->Delete<Tag_NonUniqueKey0>(boost::make_tuple(uid));

    return true;
    }

int TpsBLController::DeleteMachine_i(std::string& machineUid){

    int bRet = mDatabaseWrapper->EraseObject(machineUid, RtDbDef::TYPE_MACHINE);
    if (bRet) {
        TPS_LOG_DEV_ERROR << "Failed to delete machine from DB";
        return false;
    }
    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();


    //delete commission
    std::vector<RtCommissionedunit> commissionUnitList;
    manager->GetTable<RtCommissionedunitTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(machineUid), commissionUnitList);
    for (auto commissionUnit = commissionUnitList.begin(); commissionUnit != commissionUnitList.end(); commissionUnit++)
    {
        std::string commissionUid = commissionUnit->get_uid();

        //RtContour与Accessorychunk没有外键关联关系,表设计缺陷，需特殊处理
        std::vector<RtAccessorychunk> accessoryChunkList;
        manager->GetTable<RtAccessorychunkTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUid), accessoryChunkList);
        for (auto accessoryChunk = accessoryChunkList.begin(); accessoryChunk != accessoryChunkList.end(); accessoryChunk++)
        {
            RtContour contour;
            if (manager->GetTable<RtContourTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(accessoryChunk->get_uid()), contour) > 0)
            {
                int bRet = mDatabaseWrapper->EraseObject(contour.get_uid(), RtDbDef::TYPE_CONTOUR);
                if (bRet) {
                    TPS_LOG_DEV_ERROR << "Failed to delete contour from DB";
                    return false;
                }
                manager->GetTable<RtAccessorychunkTable>()->Delete<Tag_UniQueKey>(boost::make_tuple(contour.get_uid()));
            }
        }

        //delete accessorychunk
        manager->GetTable<RtAccessorychunkTable>()->Delete<Tag_NonUniqueKey0>(boost::make_tuple(commissionUid));

        //delete contour
        // mDataRepository->GetMachineSettingManager()->DeleteContour((*commissionUnit)->get_uid());

        //delete PhoCommissionInfo
        manager->GetTable<RtMcphocommissionedunitTable>()->Delete<Tag_NonUniqueKey0>(boost::make_tuple(commissionUid));

        //delete trayfactor
        manager->GetTable<RtTrayTable>()->Delete<Tag_NonUniqueKey0>(boost::make_tuple(commissionUid));

        //delete goldenstt
        manager->GetTable<RtGoldensttTable>()->Delete<Tag_NonUniqueKey0>(boost::make_tuple(commissionUid));

        //delete kernelweight
        manager->GetTable<RtKernelweightTable>()->Delete<Tag_NonUniqueKey0>(boost::make_tuple(commissionUid));

        //delete wedgefactorList
        manager->GetTable<RtWedgefactorTable>()->Delete<Tag_NonUniqueKey0>(boost::make_tuple(commissionUid));

        //delete measureProfile
        manager->GetTable<RtMeasuredprofileTable>()->Delete<Tag_NonUniqueKey0>(boost::make_tuple(commissionUid));

        //delete measuredata
        manager->GetTable<RtMeasureddataTable>()->Delete<Tag_NonUniqueKey0>(boost::make_tuple(commissionUid));

        //delete commissionunit
        manager->GetTable<RtCommissionedunitTable>()->Delete<Tag_UniQueKey>(boost::make_tuple(commissionUid));
    }

    //delete tray
    manager->GetTable<RtTrayTable>()->Delete<Tag_NonUniqueKey0>(boost::make_tuple(machineUid));

    //delete applicator
    manager->GetTable<RtApplicatorTable>()->Delete<Tag_NonUniqueKey0>(boost::make_tuple(machineUid));

    //delete accessory
    manager->GetTable<RtAccessoryTable>()->Delete<Tag_NonUniqueKey0>(boost::make_tuple(machineUid));

    //delete machine
    manager->GetTable<RtMachineTable>()->Delete<Tag_UniQueKey>(boost::make_tuple(machineUid));
    return true;
}

int TpsBLController::AddCommissionInfo(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig , 
    std::string & commissionUID,std::string& measureUID,std::string& phoCommissionUID,CertifiedConvolution& vCertifiedConvolution){

        CertifiedMachineSetting machineSetting;
        machineSetting.Convert(*protoMachineConfig);
        std::tr1::shared_ptr<CertifiedCommissionInfo> pVecCommissionInfo = machineSetting.commissionInfoList[0];

        RtCommissionedunit comUnit(true);
        commissionUID = comUnit.get_uid();
        comUnit.set_radiationtype(pVecCommissionInfo->radiationType);
        comUnit.set_machineuid(pVecCommissionInfo->parentUID);
        comUnit.set_commissionunitname(pVecCommissionInfo->commissionName);
        //machine里面名字存在commissionunitname字段里，直接读上来,energy字段没有使用
        //但是TPS里自己拼名字，读energy字段
        comUnit.set_energy(pVecCommissionInfo->energy);
        comUnit.set_isflattened(pVecCommissionInfo->flatteningFilterType == 1);//FF--true,FFF-false
        comUnit.set_isconvolutionsupport(pVecCommissionInfo->isConvolution);
        comUnit.set_ispencilebeamsupport(pVecCommissionInfo->isPencilBeam);
        comUnit.set_ismontecarlosupport(pVecCommissionInfo->isMonteCarlo);
        comUnit.set_allowcontinuousdoserate(pVecCommissionInfo->isDoseRateVarSupport);

        std::vector<int> photoPlanType;
        photoPlanType.push_back(pVecCommissionInfo->isPlan2D);
        photoPlanType.push_back(pVecCommissionInfo->isPlan3D);
        photoPlanType.push_back(pVecCommissionInfo->isPlanStepShoot);
        photoPlanType.push_back(pVecCommissionInfo->isPlanSlidingWindow);
        photoPlanType.push_back(pVecCommissionInfo->isPlanSimpleArc);
        photoPlanType.push_back(pVecCommissionInfo->isPlanConformalArc);
        photoPlanType.push_back(pVecCommissionInfo->isPlanVMATArc);
        photoPlanType.push_back(pVecCommissionInfo->isPlanBurstArc);

        std::string str = "";
        for (int i=0;i<photoPlanType.size();i++)
        {
            if ( str !="")
                str = str + ",";
            str = str + boost::lexical_cast<std::string>(photoPlanType[i]);
        }
        comUnit.set_availablephotonplantype(str);

        std::vector<int> vElectronPlanType;
        std::string strElectron;
        vElectronPlanType.push_back(pVecCommissionInfo->electornType);
        for (int i=0;i<vElectronPlanType.size();i++){
            if ( strElectron !="")
                strElectron = strElectron + ",";
            strElectron = strElectron + boost::lexical_cast<std::string>(vElectronPlanType[i]);
        }
        comUnit.set_availableelectronplantype(strElectron);
        comUnit.set_viewhalfprofile(pVecCommissionInfo->isCheckedHalfProfile);
        comUnit.set_calibrationfsx(pVecCommissionInfo->calibrationFieldSizeX*10);
        comUnit.set_calibrationfsy(pVecCommissionInfo->calibrationFieldSizeY*10);
        comUnit.set_commissionedunit_dmax(pVecCommissionInfo->calibrationDmax*10);
        comUnit.set_ssd(pVecCommissionInfo->sSD*10);
        comUnit.set_phantomsizedepth(pVecCommissionInfo->depth*10);
        comUnit.set_phantomsizelateral(pVecCommissionInfo->lateral*10);
        comUnit.set_blocktraypenumbra(pVecCommissionInfo->penumbra*10);
        comUnit.set_blockattenfactor(pVecCommissionInfo->blockAttenFactor);
        comUnit.set_mlcfactor(pVecCommissionInfo->MLCFactor);
        comUnit.set_xjawfactor(pVecCommissionInfo->xJawFactor);
        comUnit.set_yjawfactor(pVecCommissionInfo->yJawFactor);

        std::vector<float> vDoseRate;
        for (auto it = pVecCommissionInfo->doseRateList.begin(); it != pVecCommissionInfo->doseRateList.end(); ++it){
            vDoseRate.push_back((*it)->doseRate);
        }
        comUnit.set_discretedoserate(vDoseRate);

        TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
        if (!manager->GetTable<RtCommissionedunitTable>()->Add(comUnit)){
            TPS_LOG_DEV_ERROR << "Failed to add commission to manager.";
            return false;
        }

        int bRet = mDatabaseWrapper->InsertCommissionedunit(comUnit);
        if (bRet) {
            TPS_LOG_DEV_ERROR << "Failed to save commission from DB";
            return false;
        }

        RtMcphocommissionedunit mcphoCommission;
        mcphoCommission.set_commissionedunituid(commissionUID);
        mcphoCommission.set_uid(mDatabaseWrapper->GeneraterUid());
        mcphoCommission.set_mcprimaryenergyend(20);
        mcphoCommission.set_mcsecondaryenergyend(20);

        bRet = mDatabaseWrapper->InsertMcphocommissionedunit(mcphoCommission);
        phoCommissionUID = mcphoCommission.get_uid();
        if (bRet) {
            TPS_LOG_DEV_ERROR<<"Failed to save  mcpho commission tray from DB";
            return false;
        }

        if(!manager->GetTable<RtMcphocommissionedunitTable>()->Add(mcphoCommission)){
            TPS_LOG_DEV_ERROR<<"Failed to add mcpho commission to manager.";
            return false;
        }

        RtMeasureddata measureData(true);
        measureData.set_commissionedunituid(commissionUID);

        if (!manager->GetTable<RtMeasureddataTable>()->Add(measureData)){
            TPS_LOG_DEV_ERROR<<"Failed to add measure data to manager.";
            return false;
        }
        measureUID = measureData.get_uid();
        bRet = mDatabaseWrapper->InsertMeasureddata(measureData);
        if (bRet) {
            TPS_LOG_DEV_ERROR<<"Failed to save measure data from DB";
            return false;
        }

        std::vector<RtTray>trayList;
        manager->GetTable<RtTrayTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(pVecCommissionInfo->parentUID), trayList);

        if (trayList.size() > 0){
            for (auto it  = trayList.begin(); it != trayList.end(); ++it){

                RtCommissionedunit_tray commTray;
                commTray.set_commissionedunituid(commissionUID);
                commTray.set_trayuid((it)->get_uid());
                commTray.set_trayfactor(0);

                if (mDatabaseWrapper->InsertCommissionedunit_tray(commTray)) {
                    TPS_LOG_DEV_ERROR<<"insert commission tray to DB failed:";
                    return false;
                }

                if (!manager->GetTable<RtCommissionedunit_trayTable>()->Add(commTray)){
                    TPS_LOG_DEV_ERROR<<"update commission tray is failure:";
                    return false;
                }
            }
        }

        RtKernelweight zerokernelWeight(true);                //以后修改读数据库中的默认数据表。
        RtKernelweight fourthkernelWeight(true);

        std::vector<int>vecIntKernelID;
        std::vector<float>vecFloatKernelWeight;
        vecIntKernelID.push_back(100);
        vecIntKernelID.push_back(200);
        vecIntKernelID.push_back(300);
        vecIntKernelID.push_back(400);
        vecIntKernelID.push_back(500);
        vecIntKernelID.push_back(600);
        vecIntKernelID.push_back(800);
        vecIntKernelID.push_back(1000);
        vecIntKernelID.push_back(1250);
        vecIntKernelID.push_back(1500);
        vecIntKernelID.push_back(2000);
        vecIntKernelID.push_back(3000);
        vecIntKernelID.push_back(4000);
        vecIntKernelID.push_back(5000);
        vecIntKernelID.push_back(6000);

        for (int count = 0; count < vecIntKernelID.size(); ++count ){
            std::tr1::shared_ptr<CertifiedPDDSpectrum> spectrum(new Spectrum());

            spectrum->uid0 = zerokernelWeight.get_uid();
            spectrum->fieldSize = 0;
            spectrum->energy0 = vecIntKernelID[count];
            spectrum->kernelWeight0 =0;
            vecFloatKernelWeight.push_back(0);
            vCertifiedConvolution.spectrumList.push_back(spectrum);
        }

        zerokernelWeight.set_commissionedunituid(commissionUID);
        zerokernelWeight.set_fieldsize(0);

        std::string strkernelWeightZero,kernelWeightFourth,kernelIDZero;
        FloatToStringFunc(vecFloatKernelWeight,strkernelWeightZero);
        IntToStringFunc(vecIntKernelID,kernelIDZero);

        zerokernelWeight.set_kernelidlist(kernelIDZero);
        zerokernelWeight.set_kernelweightlist(strkernelWeightZero);

        if(!manager->GetTable<RtKernelweightTable>()->Add(zerokernelWeight)){
            TPS_LOG_DEV_ERROR<<"add  kernel weight is failure:";
            return false;
        }

        if (mDatabaseWrapper->InsertKernelweight(zerokernelWeight)) {
            TPS_LOG_DEV_ERROR<<"save kernel weight to DB failed:";
            return false;
        }

        vecFloatKernelWeight.clear();
        for(int count = 0; count < vecIntKernelID.size(); ++count ){
            std::tr1::shared_ptr<CertifiedPDDSpectrum> spectrum(new Spectrum());

            spectrum->uid40 = fourthkernelWeight.get_uid();
            spectrum->fieldSize = 40;
            spectrum->energy40 =  vecIntKernelID[count];
            spectrum->kernelWeight40 = 0;
            vecFloatKernelWeight.push_back(0);
            vCertifiedConvolution.spectrumList.push_back(spectrum);
        }

        //std::string strFourthKernelFilePath = sPath + fourthkernelWeight.get_uid();

        FloatToStringFunc(vecFloatKernelWeight,kernelWeightFourth);
       // WriteFile_i(strFourthKernelFilePath,kernelWeightFourth);

        fourthkernelWeight.set_commissionedunituid(commissionUID);
        fourthkernelWeight.set_fieldsize(40);
        fourthkernelWeight.set_kernelid_list(vecIntKernelID);
        fourthkernelWeight.set_kernelidlist(kernelIDZero);
        fourthkernelWeight.set_kernelweight_list(vecFloatKernelWeight);
        fourthkernelWeight.set_kernelweightlist(kernelWeightFourth);

        if (!manager->GetTable<RtKernelweightTable>()->Add(fourthkernelWeight)){
            TPS_LOG_DEV_ERROR<<"add  kernel weight to manager failure:";
            return false;
        }

        if (mDatabaseWrapper->InsertKernelweight(fourthkernelWeight)) {
            TPS_LOG_DEV_ERROR<<"save kernel weight to DB failed:";
            return false;
        }

        return true;
}


int TpsBLController::UpdateCommissionInfo(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig ){

    CertifiedMachineSetting machineSetting;
    machineSetting.Convert(*protoMachineConfig);
    std::tr1::shared_ptr<CertifiedCommissionInfo> pVecCommissionInfo = machineSetting.commissionInfoList[0];
    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    if (manager == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get commission from manager.";
        return TPS_ER_FAILURE;
    }

    RtCommissionedunit comUnit;
    if (manager->GetTable<RtCommissionedunitTable>()->Get<Tag_UniQueKey>(boost::make_tuple(pVecCommissionInfo->commissionUID), comUnit) <= 0){
        TPS_LOG_DEV_ERROR << "Failed to get commission from manager.";
        return TPS_ER_FAILURE;
    }

    comUnit.set_radiationtype(pVecCommissionInfo->radiationType);
    comUnit.set_machineuid(pVecCommissionInfo->parentUID);
    comUnit.set_commissionunitname(pVecCommissionInfo->commissionName);
        //machine里面名字存在commissionunitname字段里，直接读上来,energy字段没有使用
        //但是TPS里自己拼名字，读energy字段
    comUnit.set_energy(pVecCommissionInfo->energy);
    comUnit.set_isflattened(pVecCommissionInfo->flatteningFilterType == 1);//FF--true,FFF-false
    comUnit.set_isconvolutionsupport(pVecCommissionInfo->isConvolution);
    comUnit.set_ispencilebeamsupport(pVecCommissionInfo->isPencilBeam);
    comUnit.set_ismontecarlosupport(pVecCommissionInfo->isMonteCarlo);
    comUnit.set_allowcontinuousdoserate(pVecCommissionInfo->isDoseRateVarSupport);

        std::vector<int> photoPlanType;
        photoPlanType.push_back(pVecCommissionInfo->isPlan2D);
        photoPlanType.push_back(pVecCommissionInfo->isPlan3D);
        photoPlanType.push_back(pVecCommissionInfo->isPlanStepShoot);
        photoPlanType.push_back(pVecCommissionInfo->isPlanSlidingWindow);
        photoPlanType.push_back(pVecCommissionInfo->isPlanSimpleArc);
        photoPlanType.push_back(pVecCommissionInfo->isPlanConformalArc);
        photoPlanType.push_back(pVecCommissionInfo->isPlanVMATArc);
        photoPlanType.push_back(pVecCommissionInfo->isPlanBurstArc);

        std::string str = "";
    for (int i = 0; i < photoPlanType.size(); i++)
        {
        if (str != "")
                str = str + ",";
            str = str + boost::lexical_cast<std::string>(photoPlanType[i]);
        }
    comUnit.set_availablephotonplantype(str);

        std::vector<int> vElectronPlanType;
        std::string strElectron;
        vElectronPlanType.push_back(pVecCommissionInfo->electornType);

    for (int i = 0; i < vElectronPlanType.size(); i++){
        if (strElectron != "")
                strElectron = strElectron + ",";
            strElectron = strElectron + boost::lexical_cast<std::string>(vElectronPlanType[i]);
        }

    comUnit.set_availableelectronplantype(strElectron);
    comUnit.set_viewhalfprofile(pVecCommissionInfo->isCheckedHalfProfile);
    comUnit.set_calibrationfsx(pVecCommissionInfo->calibrationFieldSizeX * 10);
    comUnit.set_calibrationfsy(pVecCommissionInfo->calibrationFieldSizeY * 10);
    comUnit.set_commissionedunit_dmax(pVecCommissionInfo->calibrationDmax * 10);
    comUnit.set_ssd(pVecCommissionInfo->sSD * 10);
    comUnit.set_phantomsizedepth(pVecCommissionInfo->depth * 10);
    comUnit.set_phantomsizelateral(pVecCommissionInfo->lateral * 10);
    comUnit.set_blocktraypenumbra(pVecCommissionInfo->penumbra * 10);
    comUnit.set_blockattenfactor(pVecCommissionInfo->blockAttenFactor);
    comUnit.set_mlcfactor(pVecCommissionInfo->MLCFactor);
    comUnit.set_xjawfactor(pVecCommissionInfo->xJawFactor);
    comUnit.set_yjawfactor(pVecCommissionInfo->yJawFactor);
	comUnit.set_mindoserate_mupermin(pVecCommissionInfo->mindoserate_mupermin);

        std::vector<float> vDoseRate;
        int i = 0;
        for (auto it = pVecCommissionInfo->doseRateList.begin(); it != pVecCommissionInfo->doseRateList.end(); ++it){
            vDoseRate.push_back((*it)->doseRate);
            if ((*it)->isDoseRateDefault == true){
            comUnit.set_defaultdoserateindex(i);
            }
             ++i;
        }

    comUnit.set_discretedoserate(vDoseRate);

    std::vector<RtCommissionedunit_tray> commissionTrayList;
    if (!manager->GetTable<RtCommissionedunit_trayTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(pVecCommissionInfo->commissionUID), commissionTrayList)){
        TPS_LOG_TRACE_INFO << " the commission is not tray ";
        }

        if (commissionTrayList.size() != 0){
            int count = 0;
        for (auto commissionTray = commissionTrayList.begin(); commissionTray != commissionTrayList.end(); ++commissionTray){
                if (commissionTrayList.size() == pVecCommissionInfo->trayFactorList.size()){
                commissionTray->set_commissionedunituid(pVecCommissionInfo->commissionUID);
                RtTray tray;
                if (manager->GetTable<RtTrayTable>()->Get<Tag_UniQueKey>(boost::make_tuple(commissionTray->get_trayuid()), tray) <= 0)
                {
                    TPS_LOG_DEV_ERROR << "Failed to get tray to manager.";
                        return false;
                    }

                commissionTray->set_trayuid(tray.get_uid());
                commissionTray->set_trayfactor(pVecCommissionInfo->trayFactorList[count]);

                int bRet = mDatabaseWrapper->UpdateCommissionedunit_tray(*commissionTray);
                    if (bRet) {
                    TPS_LOG_DEV_ERROR << "Failed to udpate commission tray from DB";
                        return false;
                    }
                    count++;

                if (!manager->GetTable<RtCommissionedunit_trayTable>()->Update<Tag_UniQueKey>(boost::make_tuple(commissionTray->get_commissionedunituid(), commissionTray->get_trayuid()), *commissionTray)){
                    TPS_LOG_DEV_ERROR << "Failed to update commission Tray to manager.";
                return false;
            }
        }
        }
    }

    if (!manager->GetTable<RtCommissionedunitTable>()->Update<Tag_UniQueKey>(boost::make_tuple(comUnit.get_uid()), comUnit)){
        TPS_LOG_DEV_ERROR << "Failed to update commission .";
            return false;
        }

    int bRet = mDatabaseWrapper->UpdateCommissionedunit(comUnit);
        if (bRet) {
        TPS_LOG_DEV_ERROR << "Failed to udpate commission from DB";
            return false;
        }
    return true;
}

int TpsBLController::AddWedgeInfo(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig, std::string& wedgeUID)
{
    CertifiedMachineSetting machineSetting;
    machineSetting.Convert(*protoMachineConfig);
    std::tr1::shared_ptr<CertifiedWedgeInfo> pWedgeInfoList = machineSetting.wedgeInfoList[0];

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    RtMachine machine;
    float sourceToWedgeDistance = 0;
    if (manager->GetTable<RtMachineTable>()->Get<Tag_UniQueKey>
        (boost::make_tuple(protoMachineConfig->machinegeneralinfo().uid()), machine) == 1)
    {
        sourceToWedgeDistance = machine.get_sourcetowedgetopdistance();
    }
    RtAccessory accessory(true);
    CertifiedToRTAccessoryConverter(&accessory ,pWedgeInfoList,machineSetting.uid,sourceToWedgeDistance);

    if (!manager->GetTable<RtAccessoryTable>()->Add(accessory)){
        TPS_LOG_DEV_ERROR << "Failed to add accessory.";
        return false;
    }

    int bRet = mDatabaseWrapper->InsertAccessory(accessory);
    if (bRet) {
        TPS_LOG_DEV_ERROR << "save accessory info to DB failed:";
        return false;

    }

    wedgeUID = accessory.get_uid();

    return true;
}

int TpsBLController::UpdateWedgeInfo(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig)
{
    CertifiedMachineSetting machineSetting;
    machineSetting.Convert(*protoMachineConfig);
    std::tr1::shared_ptr<CertifiedWedgeInfo> pWedgeInfoList = machineSetting.wedgeInfoList[0];
    RtAccessory* accessory = new RtAccessory;
    accessory->setdirty_all_fields(false);
    accessory->set_uid(pWedgeInfoList->wedgeUID);

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    RtMachine machine;
    float sourceToWedgeDistance = 0;
    if (manager->GetTable<RtMachineTable>()->Get<Tag_UniQueKey>
        (boost::make_tuple(protoMachineConfig->machinegeneralinfo().uid()), machine) == 1)
    {
        sourceToWedgeDistance = machine.get_sourcetowedgetopdistance();
    }

    CertifiedToRTAccessoryConverter(accessory,pWedgeInfoList,machineSetting.uid,sourceToWedgeDistance);

    if (!manager->GetTable<RtAccessoryTable>()->Update<Tag_UniQueKey>(boost::make_tuple(accessory->get_uid()), *accessory)){
        TPS_LOG_DEV_ERROR << "Failed to update accessory.";
        return false;
    }

    int bRet = mDatabaseWrapper->UpdateAccessory(*accessory);
    if (bRet) {
        TPS_LOG_DEV_ERROR<<"update accessory info to DB failed:";
        return false;
    }

    return true;
}


int TpsBLController::AddApplicatorInfo(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig,std::string& applicatorUID){

    CertifiedMachineSetting machineSetting;
    machineSetting.Convert(*protoMachineConfig);
    std::tr1::shared_ptr<CertifiedElectronApplicator> certifiedApplicator = machineSetting.electronApplicatorList[0];

    RtApplicator eletronApplicator(true);
    eletronApplicator.set_machineuid(machineSetting.uid);
    eletronApplicator.set_name(certifiedApplicator->applicatorACCID);
    eletronApplicator.set_applicatorid(certifiedApplicator->applicatorACCID);

    if ((certifiedApplicator)->sizeX == (certifiedApplicator)->sizeY/2){
        eletronApplicator.set_applicatortype((int)RtDbDef::ELECTRON_RECT);
        eletronApplicator.set_applicatorapertureshape((int)RtDbDef::SYM_RECTANGLE);
        eletronApplicator.set_applicatoropening(((certifiedApplicator)->sizeX)*10);
    }
    else{
        eletronApplicator.set_applicatortype((int)RtDbDef::ELECTRON_SQUARE);
        eletronApplicator.set_applicatorapertureshape((int)RtDbDef::SYM_SQUARE);
        eletronApplicator.set_applicatoropeningx((certifiedApplicator)->sizeX*10);
        eletronApplicator.set_applicatoropeningy((certifiedApplicator)->sizeY*10);
    }

    eletronApplicator.set_jawxissymmetry(machineSetting.isXasym ==1);
    eletronApplicator.set_jawyissymmetry(machineSetting.isYasym ==1);
    eletronApplicator.set_xlowerjawpos(-(certifiedApplicator)->sizeX/2);
    eletronApplicator.set_xupperjawpos((certifiedApplicator)->sizeX/2);
    eletronApplicator.set_ylowerjawpos(-(certifiedApplicator)->sizeY/2);
    eletronApplicator.set_yupperjawpos((certifiedApplicator)->sizeY/2);


    if (!mDataRepository->GetMachineSettingManager()->GetTable<RtApplicatorTable>()->Add(eletronApplicator)) {
        TPS_LOG_DEV_ERROR<<" add electron applicator to manager failed:";
        return false;
    }
 
    int bRet = mDatabaseWrapper->InsertApplicator(eletronApplicator);
    if (bRet) {
        TPS_LOG_DEV_ERROR<<"save applicator info to DB failed:";
        return false;
    }
    applicatorUID = eletronApplicator.get_uid();
    return true;
}

int TpsBLController::UpdateApplicatorInfo(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig){

    CertifiedMachineSetting machineSetting;
    machineSetting.Convert(*protoMachineConfig);
    std::tr1::shared_ptr<CertifiedElectronApplicator> certifiedApplicator = machineSetting.electronApplicatorList[0];

    RtApplicator eletronApplicator;
    eletronApplicator.set_uid(certifiedApplicator->applicatorUID);
    eletronApplicator.set_machineuid(machineSetting.uid);
    eletronApplicator.set_name(certifiedApplicator->applicatorACCID);
    eletronApplicator.set_applicatorid(certifiedApplicator->applicatorACCID);

    if ((certifiedApplicator)->sizeX == (certifiedApplicator)->sizeY/2){
        eletronApplicator.set_applicatortype((int)RtDbDef::ELECTRON_RECT);
        eletronApplicator.set_applicatorapertureshape((int)RtDbDef::SYM_RECTANGLE);
        eletronApplicator.set_applicatoropening((certifiedApplicator->sizeX)*10);
    }
    else{
        eletronApplicator.set_applicatortype((int)RtDbDef::ELECTRON_SQUARE);
        eletronApplicator.set_applicatorapertureshape((int)RtDbDef::SYM_SQUARE);
        eletronApplicator.set_applicatoropeningx((certifiedApplicator)->sizeX*10);
        eletronApplicator.set_applicatoropeningy((certifiedApplicator)->sizeY*10);
    }

    eletronApplicator.set_jawxissymmetry(machineSetting.isXasym ==1);
    eletronApplicator.set_jawyissymmetry(machineSetting.isYasym ==1);
    eletronApplicator.set_xlowerjawpos(-(certifiedApplicator)->sizeX/2);
    eletronApplicator.set_xupperjawpos((certifiedApplicator)->sizeX/2);
    eletronApplicator.set_ylowerjawpos(-(certifiedApplicator)->sizeY/2);
    eletronApplicator.set_yupperjawpos((certifiedApplicator)->sizeY/2);

    if (!mDataRepository->GetMachineSettingManager()->GetTable<RtApplicatorTable>()->Update<Tag_UniQueKey>(boost::make_tuple(eletronApplicator.get_uid()), eletronApplicator)) {
        TPS_LOG_DEV_ERROR<<" update electron applicator to manager failed:";
        return false;
    }

    int bRet = mDatabaseWrapper->UpdateApplicator(eletronApplicator);
    if (bRet) {
        TPS_LOG_DEV_ERROR<<"update applicator info to DB failed:";
        return false;
    }

    return true;
}
int TpsBLController::UpdateLeafInfo(ms::proto::RT_MS_MachineConfigInfo* protoMachineConfig){
    protoMachineConfig;
    //CertifiedMachineSetting machineSetting;
    //machineSetting.Convert(*protoMachineConfig);

    //RTFWK::RTFWKDLDOMachine* machine =nullptr;
    //if(!mDataRepository->GetMachineSettingManager()->GetMachine(machineSetting.uid,machine)){
    //    TPS_LOG_DEV_ERROR<<"get machine setting to manager failed:";
    //    return false;
    //}
    //RTFWK::RTFWKDLDOMLCSetting* mlcSetting= new RTFWK::RTFWKDLDOMLCSetting();

    //std::vector<double> pairList;
    //for (auto it =machineSetting.pairInfoList.begin(); it != machineSetting.pairInfoList.end(); ++it ){
    //    pairList.push_back((*it)->leafPairs*10);
    //}
    //mlcSetting->SetLeafBoundaries(pairList);
    //machine->SetMLCSetting(mlcSetting);

    //if (!RTFWK::RTFWKDLDataAccess::GetInstance()->SaveObjectToDB(machine)) {
    //    TPS_LOG_DEV_ERROR<<"save electron applicator to TMS failed:";
    //    return false;
    //}
    //delete mlcSetting;
    return true;
}

int TpsBLController::ImportDataFromFile( std::vector<std::string>& fileList, ms::proto::RT_MS_Proto* proto,float& minAxisX,float& maxAxisX,float& minAxisY,float& maxAixsY , int &importedNumber){
        std::string commissionUID = proto->parent_uid();
        std::string machineuid = proto->uid();

        std::vector<CurveProfile>*vCurveProfile = nullptr;
        std::cout<<"start AcsFileRead!!!\n"; 
        //AcsFileRead acsFile(filePath,vCurveProfile);
        AcsFileRead acsFile(fileList,vCurveProfile);
        std::cout<<"end AcsFileRead!!!\n"; 
        std::vector<std::string> vAccessoryUID;
        // int angle;


        TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
        std::vector<RtAccessory>accessoryList;
        if (manager->GetTable<RtAccessoryTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(machineuid), accessoryList) < 0) {
            TPS_LOG_DEV_ERROR << " get accessory uid to manager failed:";
            return TPS_ER_FAILURE;
        }

        acsFile.GetCurveProfile(vCurveProfile);                                         //从文件中把数据取出。
        mFileInfoList.clear();
        std::vector<float> vPointX,vPointY;                                              //这两个集合为了前端画曲线时，需要的范围限定使用。
        vector<float> vecX,vecY,vecZ,vecDose;
        int improtedCurveNumbers = 0;
        for (auto it = vCurveProfile->begin(); it != vCurveProfile->end(); ++it){
            RtCommissionedunit commissionUnit = nullptr;
            if (manager->GetTable<RtCommissionedunitTable>()->Get<Tag_UniQueKey>(boost::make_tuple(commissionUID), commissionUnit) < 0){
                TPS_LOG_DEV_ERROR << "Failed to Get commissionUnit.";
                return TPS_ER_FAILURE;
            }

            float ssd = commissionUnit.get_ssd();
            if (!abs(ssd - it->mSsd) - 0.01 < 1e-6)
                return TPS_ER_MACHINE_SSDNOTMATCH;

            vecX.clear();
            vecY.clear();
            vecZ.clear();
            vecDose.clear();

            FieldSize fieldsize;
            fieldsize.m_fFieldSizeX = (it)->mFszx;
            fieldsize.m_fFieldSizeY = (it)->mFszy;
            RtMeasuredprofile measureProfile(true);
            RtDbDef::CURVETYPE coodiDir;
            vector<CoordinateData> coordinateData = (*it).vWedgeInfoList;
            float xa;
            float ya;
            int vSize = coordinateData.size();
            if (vSize > 0){
                std::vector<PointDose*> pointDose;
                std::string str;
                for (auto itt = coordinateData.begin(); itt != coordinateData.end(); ++itt){

                    xa = itt->y;
                    ya = -itt->x;
                    vecX.push_back(xa);
                    vecY.push_back(ya);
                    vecZ.push_back(itt->z);
                    vecDose.push_back(itt->dose);

                    if (str != ""){
                        str = str + "|";
                    }
                    str = str + boost::lexical_cast<std::string>(itt->dose) + ",";
                    str = str + boost::lexical_cast<std::string>(xa)+",";
                    str = str + boost::lexical_cast<std::string>(ya)+",";
                    str = str + boost::lexical_cast<std::string>(itt->z);

                }
                measureProfile.set_meapointdoselist(str);

                std::sort(vecX.begin(), vecX.end());     //默认是升序。
                std::sort(vecY.begin(), vecY.end());
                std::sort(vecZ.begin(), vecZ.end());
                std::sort(vecDose.begin(), vecDose.end());

                float maxX = vecX[vSize - 1];
                float minX = vecX[0];
                float maxY = vecY[vSize - 1];
                float minY = vecY[0];
                float maxZ = vecZ[vSize - 1];
                float minZ = vecZ[0];

                int x1 = maxX - minX;
                int y1 = maxY - minY;
                int z1 = maxZ - minZ;

                float maxDose = 0;
                float offsetX = 0;
                float offsetY = 0;

                //根据Phantom Location Angle做坐标变换：在解析.asc文件时， x = xw*cos(angle) – yw * sin(angle)， y = xw * sin(angle) + yw * cos(angle).然后根据x和y的值判断曲线类型和offset。
                //若该曲线x坐标的最大值和最小值差值大于a，且y坐标最大值和最小值差值不超过b，且z坐标最大值和最小值差值不超过b，判定为crossplane曲线。
                //若该曲线y坐标的最大值和最小值差值大于a，且x坐标最大值和最小值差值不超过b，且z坐标最大值和最小值差值不超过b，判定为inplane曲线。
                //若该曲线z坐标的最大值和最小值差值大于a，且x坐标最大值和最小值差值不超过b，且y坐标最大值和最小值差值不超过b，判定为PDD曲线。
                //其余为无效曲线，不导入。

                if (x1 > 1 && y1 <= 1 && z1 <= 1){

                    coodiDir = RtDbDef::CURVETYPE_CROSSPLANE;
                    GetOffSetDose_i(coordinateData, vecDose, maxDose, (int)coodiDir, fieldsize, offsetX, offsetY);
                    //coodiDir = RTFWK::CURVETYPE_INPLANE;               //修改下。x轴坐标。
                    measureProfile.set_offsetx(offsetX);
                    measureProfile.set_offsety(offsetY);

                    vPointX.push_back(vecX[0]);
                    vPointX.push_back(vecX[vecX.size() - 1]);
                    vPointY.push_back(vecDose[0]);
                    vPointY.push_back(vecDose[vecDose.size() - 1]);
                }
                else if (x1 < 1 && y1 >= 1 && z1 <= 1){
                    coodiDir = RtDbDef::CURVETYPE_INPLANE;               //修改下。Y轴坐标。
                    GetOffSetDose_i(coordinateData, vecDose, maxDose, (int)coodiDir, fieldsize, offsetX, offsetY);
                    measureProfile.set_offsetx(offsetX);
                    measureProfile.set_offsety(offsetY);

                    vPointX.push_back(vecY[0]);
                    vPointX.push_back(vecY[vecY.size() - 1]);
                    vPointY.push_back(vecDose[0]);
                    vPointY.push_back(vecDose[vecDose.size() - 1]);
                }
                else if (x1 < 1 && y1 <= 1 && z1 >= 1){                         //z坐标系。
                    coodiDir = RtDbDef::CURVETYPE_PDD;
                    measureProfile.set_offsetx(0);
                    measureProfile.set_offsety(0);

                    vPointX.push_back(vecZ[0]);
                    vPointX.push_back(vecZ[vecZ.size() - 1]);
                    vPointY.push_back(vecDose[0]);
                    vPointY.push_back(vecDose[vecDose.size() - 1]);
                }
                //2016-8-10 需求判断这类曲
                //else if (x1> 1 && y1 > 1 && z1 < 1){                                    //X,Y轴坐标
                //    coodiDir = RtDbDef::CURVETYPE_DIAGONAL;
                //    GetOffSetDose_i(coordinateData, vecDose, maxDose, (int)coodiDir, fieldsize, offsetX, offsetY);
                //    measureProfile.set_offsetx(offsetX * 10);
                //    measureProfile.set_offsety(offsetY * 10);
                //    importFile.offsetX = offsetX;
                //    importFile.offsetY = offsetY;
                //    std::vector<float>xPoint;
                //    for (auto it = pointDose.begin(); it != pointDose.end(); ++it){
                //        xPoint.push_back((*it)->m_fX);
                //    }
                //    std::sort(xPoint.begin(), xPoint.end());
                //    for (int count = 0; count < xPoint.size(); ++count){
                //        // pointDose[count]->m_fX = xPoint[count];
                //    }
                //    measureProfile.SetMeaPointDoseList(pointDose);
                //    vPointX.push_back(std::sqrt(vecY[0] * vecY[0] + vecX[0] * vecX[0]));
                //    vPointX.push_back(std::sqrt(vecX[vecX.size() - 1] * vecX[vecX.size() - 1] + vecY[vecY.size() - 1] * vecY[vecY.size() - 1]));
                //    vPointY.push_back(vecDose[0]);
                //    vPointY.push_back(vecDose[vecDose.size() - 1]);
                //}
                //else{
                //    COORDINATRDIRCTION coodiDir = OTHER;                    //数据库中少个枚举。
                //}
                else
                {
                    continue;
                }

                measureProfile.set_commissionedunituid(commissionUID);
                measureProfile.set_curvetype(coodiDir);
                measureProfile.set_viewdisplay(false);
                measureProfile.set_depth((it)->mZ);
                measureProfile.set_viewstatus(NotCompute);
                measureProfile.set_fieldsizex((it)->mFszx);
                measureProfile.set_fieldsizey((it)->mFszy);
         
                //char a[8], b[8];
                //itoa((it)->mFszx, a, 10);
                //itoa((it)->mFszy, b, 10);
                //std::string fszx(a), fszy(b);
                //importFile.fieldSize = fszx + "*" + fszy;

                bool hasWedge=false;
                RtDbDef::FIELDTYPE type;
                if ((it)->mWedge == 0){

                    type = RtDbDef::FIELDTYPE_OPEN;
                    /*    importFile.degree = -1;
                    importFile.wedgeACCID = "";*/
                }
                else{
                    type = RtDbDef::FIELDTYPE_WEDGE;
                    for (auto accessory = accessoryList.begin(); accessory != accessoryList.end(); accessory++)
                    {
                        if (abs(accessory->get_wedgeangle() - it->mWedge) <= 1e-6 && accessory->get_accessorytype() == RtDbDef::ACCESSORY_PHYSICALWEDGE)
                        {
                            //TODO: 这3行有需求问题, 需要和算法组同事确定文件格式
                            measureProfile.set_wedgeangle((it)->mWedge);
                            measureProfile.set_accessoryuid((accessory)->get_uid());
                            hasWedge = true;
                            break;
                        }
                        else
                        {
                            hasWedge = false;
                        }
                    }
                    if (!hasWedge)
                    {
                        TPS_LOG_TRACE_INFO << "The wedge angle of machine can't fit for the wedge angle of import file";
#ifdef _DEBUG
                        printf("\n The wedge angle of machine can't fit for the wedge angle of import file");
#endif
                        return TPS_ER_FAILURE;
                    }
                }
                measureProfile.set_fieldtype((int)type);

                //RtMeasuredprofile retProfile;
                //GenerateCurveProfilePath(measureProfile,retProfile);
                //UpdateFileList(&measureProfile, retProfile);

                if (mDatabaseWrapper->InsertMeasuredprofile(measureProfile) != DB_CODES_SUCCESS)
                {
                    TPS_LOG_DEV_ERROR << "save measure profile to DB failed:";
                    return TPS_ER_FAILURE;
                }

                if (!manager->GetTable<RtMeasuredprofileTable>()->Add(measureProfile)){
                    TPS_LOG_DEV_ERROR << "add measure profile to manager is failure:";
                    return TPS_ER_FAILURE;
                }

                improtedCurveNumbers++;
            }
            else
            {
                continue;
            }
        }
        std::sort(vPointX.begin(), vPointX.end());
        std::sort(vPointY.begin(), vPointY.end());
        if (vPointX.size() != 0 && vPointY.size() != 0){
            minAxisX = vPointX[0];
            maxAxisX = vPointX[vPointX.size() - 1];
            minAxisY = vPointY[0];
            maxAixsY = vPointY[vPointY.size() - 1];
        }
        importedNumber = improtedCurveNumbers;
        return TPS_ER_SUCCESS;
}

bool TpsBLController::GetOffSetDose_i(std::vector<CoordinateData>& coordinateData,std::vector<float>& vecDose, 
    float& doseMax, int coodiDir,FieldSize fieldSize, float& offX,float& offY){
        int vSize = coordinateData.size();
        doseMax = vecDose[vSize-1];
        int count = 0;//index of maximum dose point

        if (coodiDir ==  RtDbDef::CURVETYPE_PDD)
        {
            return true;
        }

        float halfMax = 0.0f;// half of maximum dose
        bool  bPosOfHalfDoseLeft = false;//R50L exist
        float fPosOfHalfDoseLeft = 0.0f;//R50L
        float fXOfHalfDoseLeft = 0.0f;
        float fYOfHalfDoseLeft = 0.0f;

        float fPosOfHalfDoseRight = 0.0f;//R50R
        bool  bPosOfHalfDoseRight = false;//R50R exist
        float fXOfHalfDoseRight = 0.0f;
        float fYOfHalfDoseRight = 0.0f;

        float f1 = 0.0f, f2 = 0.0f;
        halfMax = doseMax * 0.5f;

        //求dose最大值的下标所在位置。
        for (auto itt =coordinateData.begin(); itt != coordinateData.end(); ++itt){
            if (abs(itt->dose - doseMax) < 1E-6){
                break;
            }
            count++;
            //CoordinateData d1   = itt[count];
        }
        int i = 0;

        for (i =0; i < count; ++i){
            if (coordinateData[i].dose > halfMax && coordinateData[i+1].dose < halfMax ||
                coordinateData[i].dose < halfMax && coordinateData[i+1].dose > halfMax){
                    bPosOfHalfDoseLeft = true;
                    f1 = (halfMax - coordinateData[i].dose ) / (coordinateData[i+1].dose - coordinateData[i].dose);
                    f2 = (coordinateData[i+1].dose - halfMax) / (coordinateData[i+1].dose - coordinateData[i].dose);
                    fPosOfHalfDoseLeft = f2 * GetPos_i(coordinateData[i], coodiDir) + f1 * GetPos_i(coordinateData[i + 1], coodiDir);
                    fXOfHalfDoseLeft = f2 * coordinateData[i].x + f1 * coordinateData[i + 1].x;
                    fYOfHalfDoseLeft = f2 * coordinateData[i].y + f1 * coordinateData[i + 1].y;
                    break;
            }
        }
        //int iHalfDoseL = i;
        for (i =vSize -1; i > count; -- i){
            if (coordinateData[i-1].dose > halfMax && coordinateData[i].dose < halfMax ||
                coordinateData[i-1].dose < halfMax && coordinateData[i].dose > halfMax){
                    bPosOfHalfDoseRight = true;
                    f1 = (halfMax - coordinateData[i - 1].dose ) / (coordinateData[i].dose - coordinateData[i - 1].dose);
                    f2 = (coordinateData[i].dose - halfMax) / (coordinateData[i].dose - coordinateData[i - 1].dose);
                    fPosOfHalfDoseRight = f2 * GetPos_i(coordinateData[i - 1], coodiDir) + f1 * GetPos_i(coordinateData[i], coodiDir);
                    fXOfHalfDoseRight = f2 * coordinateData[i - 1].x + f1 * coordinateData[i].x;
                    fYOfHalfDoseRight = f2 * coordinateData[i - 1].y + f1 * coordinateData[i].y;
                    break;
            }
        }
        float offsetX = 0.0f;
        float offsetY = 0.0f;
        float fs = 0.0f;
        if (coodiDir ==  RtDbDef::CURVETYPE_CROSSPLANE)
        {
            if (bPosOfHalfDoseLeft && bPosOfHalfDoseRight)
            {
                offsetX = (fXOfHalfDoseLeft + fXOfHalfDoseRight) * 0.5f;
                fs = fXOfHalfDoseRight - fXOfHalfDoseLeft;
                if (abs(fs - fieldSize.m_fFieldSizeX) > 1.0f){
                    TPS_LOG_DEV_ERROR<<"It's wrong field size!\n";
                }
            }
            else
            {
                offsetX = 0.0f;
            }
            offsetY = 0.0f;
        }
        if (coodiDir == RtDbDef::CURVETYPE_INPLANE)
        {
            if (bPosOfHalfDoseLeft && bPosOfHalfDoseRight)
            {
                offsetY = (fYOfHalfDoseLeft + fYOfHalfDoseRight) * 0.5f;
                fs = fYOfHalfDoseRight - fYOfHalfDoseLeft;
                if (abs(fs - fieldSize.m_fFieldSizeY) > 1.0f){
                    TPS_LOG_DEV_ERROR<<"It's wrong field size!\n";
                }
            }
            else
            {
                offsetY = 0.0f;
            }
            offsetX = 0.0f;
        }
        if (coodiDir ==  RtDbDef::CURVETYPE_DIAGONAL)
        {
            if (bPosOfHalfDoseLeft && bPosOfHalfDoseRight)
            {
                offsetX = (fXOfHalfDoseLeft + fXOfHalfDoseRight) * 0.5f;
                offsetY = (fYOfHalfDoseLeft + fYOfHalfDoseRight) * 0.5f;
                float fsX = fXOfHalfDoseRight - fXOfHalfDoseLeft;
                float fsY = fYOfHalfDoseRight - fYOfHalfDoseLeft;
                if (abs(fsX - fieldSize.m_fFieldSizeX) > 1.0f || abs(fsY - fieldSize.m_fFieldSizeY) > 1.0f){
                    TPS_LOG_DEV_ERROR<<"It's wrong field size!\n";
                }
            }
            else
            {
                offsetX = 0.0f;
                offsetY = 0.0f;
            }
        }
        if (!(bPosOfHalfDoseLeft && bPosOfHalfDoseRight))//曲线不全
        {
            //give warning??
        }
        offX = offsetX;
        offY = offsetY;

        return true;
}

float  TpsBLController::GetPos_i(CoordinateData& oPoint, int& coodiDir){

    float pos = 0.0f;
    if (coodiDir ==  RtDbDef::CURVETYPE_PDD)
    {
        pos = oPoint.z;
    }
    if (coodiDir ==  RtDbDef::CURVETYPE_CROSSPLANE)
    {
        pos = oPoint.x;
    }
    if (coodiDir ==  RtDbDef::CURVETYPE_INPLANE)
    {
        pos = oPoint.y;
    }
    if (coodiDir ==  RtDbDef::CURVETYPE_DIAGONAL)
    {
        pos = sqrt(oPoint.x * oPoint.x + oPoint.y * oPoint.y);
    }

    return pos;
}

int TpsBLController::UpdateConvolution(ms::proto::RT_MS_Convolution* protoConvolution)
{
    CertifiedConvolution cerConvolution;
    cerConvolution.Convert(*protoConvolution);

    std::string commissionUID = cerConvolution.commissionUID;
    std::string accessoryUID  = cerConvolution.accessoryUID;

    std::vector<RtKernelweight> kernelWeightList;
    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    manager->GetTable<RtKernelweightTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), kernelWeightList);

    std::vector<int> kernelIDZeroList;
    std::vector<float> kernelWeightZeroList,kernelWeightFourthList;
    std::string uidZero,uidForth;
    for (auto it = cerConvolution.spectrumList.begin(); it != cerConvolution.spectrumList.end(); ++it){
        uidZero = (*it)->uid0;
        uidForth = (*it)->uid40;
        kernelIDZeroList.push_back((*it)->energy0);
        kernelWeightZeroList.push_back((*it)->kernelWeight0);
        kernelWeightFourthList.push_back((*it)->kernelWeight40);
    }

    std::string kernelIDZero, strkernelWeightZero, kernelWeightFourth;
    IntToStringFunc(kernelIDZeroList, kernelIDZero);
    FloatToStringFunc(kernelWeightZeroList, strkernelWeightZero);
    FloatToStringFunc(kernelWeightFourthList, kernelWeightFourth);

    for (auto kernel = kernelWeightList.begin(); kernel != kernelWeightList.end(); ++kernel)//UIH-6MV-FF.0
    { 
        if (kernel->get_uid() == uidZero)
        {
            RtKernelweight kernelWeightZero;
            kernelWeightZero.set_commissionedunituid(commissionUID);
            kernelWeightZero.set_fieldsize(0);
            kernelWeightZero.set_uid(uidZero);

            kernelWeightZero.set_kernelidlist(kernelIDZero);
            kernelWeightZero.set_kernelweightlist(strkernelWeightZero);

            if (mDatabaseWrapper->UpdateKernelweight(kernelWeightZero))
            {
                TPS_LOG_DEV_ERROR<<"save kernel weight to DB failed:";
                return false;
            }

            if (!manager->GetTable<RtKernelweightTable>()->Update<Tag_UniQueKey>(boost::make_tuple(kernelWeightZero.get_uid()), kernelWeightZero))
            {
                TPS_LOG_DEV_ERROR << "save kernel weight to manager failed:";
                return false;
            }
        }
        else
        {

            RtKernelweight kernelWeightFourthy;
            kernelWeightFourthy.set_commissionedunituid(commissionUID);
            kernelWeightFourthy.set_fieldsize(40);
            kernelWeightFourthy.set_uid(uidForth);

            kernelWeightFourthy.set_kernelidlist(kernelIDZero);
            kernelWeightFourthy.set_kernelweightlist(kernelWeightFourth);

            if (mDatabaseWrapper->UpdateKernelweight(kernelWeightFourthy)) {
                TPS_LOG_DEV_ERROR<<"save kernel weight to DB failed:";
                return false;
            }

            if (!manager->GetTable<RtKernelweightTable>()->Update<Tag_UniQueKey>(boost::make_tuple(kernelWeightFourthy.get_uid()), kernelWeightFourthy))
            {
                TPS_LOG_DEV_ERROR << "save kernel weight to manager failed:";
                return false;
            }
        }
    }

    RtCommissionedunit commissionUnit;
    if (manager->GetTable<RtCommissionedunitTable>()->Get<Tag_UniQueKey>(boost::make_tuple(commissionUID), commissionUnit) <= 0)
    {
        TPS_LOG_DEV_ERROR << "Failed to get commission from manager.";
        return false;
    }

    //PDD tab
    commissionUnit.set_mu(cerConvolution.pddMuBase);
    commissionUnit.set_dmu_dx(cerConvolution.pddDmumx);

    commissionUnit.set_mudatax(cerConvolution.mudatax);
    commissionUnit.set_ddmudr(cerConvolution.ddmudr);
    commissionUnit.set_mudatay(cerConvolution.mudatay);
    commissionUnit.set_dmudatay(cerConvolution.dmudatay);


    //In field tab
    commissionUnit.set_dmu_dr(cerConvolution.dmudr);
    commissionUnit.set_outputmuy(cerConvolution.outputmuy);

    std::vector<float> radiusVecotor,axisRatio;
    for (int count = 0; count < cerConvolution.inFieldVectorList.size(); ++count)
    {
        radiusVecotor.push_back(cerConvolution.inFieldVectorList[count]->radius*10);
        axisRatio.push_back(cerConvolution.inFieldVectorList[count]->offaxisRatio);
    }

    std::string strradiusVecotor;
    FloatToStringFunc(radiusVecotor,strradiusVecotor);
    commissionUnit.set_offaxisdatax(strradiusVecotor);

    std::string straxisRatio;
    FloatToStringFunc(axisRatio,straxisRatio);
    commissionUnit.set_offaxisdatay(straxisRatio);
    commissionUnit.set_diam1(cerConvolution.diamOne*10);
    commissionUnit.set_diam2(cerConvolution.diamTwo*10);
    commissionUnit.set_ratio1(cerConvolution.srcRatio);
    commissionUnit.set_transmission(cerConvolution.transmission);
    commissionUnit.set_psd(cerConvolution.psd*10);
    commissionUnit.set_src_d(cerConvolution.srcDepth*10);
    commissionUnit.set_src_atten(cerConvolution.srcAtten);
    commissionUnit.set_convdatastruct_sdd1(cerConvolution.sddXJaw*10);
    commissionUnit.set_convdatastruct_sdd2(cerConvolution.sddYJaw*10);
    commissionUnit.set_norm_10x10(cerConvolution.normTenMultiTen);
    commissionUnit.set_ion(cerConvolution.electronContaminationFlag == 1);
    commissionUnit.set_dm(cerConvolution.electronMaxDepth*10);
    commissionUnit.set_ffs10(cerConvolution.surfaceDose);
    commissionUnit.set_k(cerConvolution.depthCoefficientK);
    commissionUnit.set_a(cerConvolution.offaxisCoefficientA);
    commissionUnit.set_df(cerConvolution.df);
    commissionUnit.set_sf(cerConvolution.sf);
    commissionUnit.set_c1(cerConvolution.cOne);
    commissionUnit.set_c2(cerConvolution.cTwo);
    commissionUnit.set_c3(cerConvolution.cThree);

    if (mDatabaseWrapper->UpdateCommissionedunit(commissionUnit)) 
    {
        TPS_LOG_DEV_ERROR<<"save commission unit to DB failed:";
        return false;
    }


    if (!manager->GetTable<RtCommissionedunitTable>()->Update<Tag_UniQueKey>(boost::make_tuple(commissionUnit.get_uid()), commissionUnit)) 
    {
        TPS_LOG_DEV_ERROR << "save commission unit to DB failed:";
        return false;
    }

    std::vector<RtMeasuredprofile> measureProfileList;
    manager->GetTable<RtMeasuredprofileTable>()->Get<Tag_UniQueKey>(boost::make_tuple(commissionUID), measureProfileList);

    for (auto measureProfile = measureProfileList.begin(); measureProfile != measureProfileList.end(); ++measureProfile)
    {
        if ((measureProfile)->get_viewstatus() == NewComputeResult)
        {
            (measureProfile)->set_viewstatus(OldComputeResult);

            if (mDatabaseWrapper->UpdateMeasuredprofile(*(measureProfile))) 
            {
                TPS_LOG_DEV_ERROR << "save measureProfile to DB failed:";
                return false;
            }

            if (!manager->GetTable<RtMeasuredprofileTable>()->
                Update<Tag_UniQueKey>(boost::make_tuple(measureProfile->get_uid()), *measureProfile))
            {
                TPS_LOG_DEV_ERROR << "Failed to measureProfileList.";
                return false;
            }
        }
    }

    int algType = 2;
    UpdatePhysicalWedgeField_i(cerConvolution.physicalWedgeList,commissionUID,algType);

    return true;
}

int TpsBLController::UpdateVirtualWedgeFieldInfo(ms::proto::RT_MS_VirtualWedgeInfo* protoVirtualWedgeField){

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    RtGoldenstt* goldenSTT;
    RtGoldenstt goldenSTTV;
    if (manager->GetTable<RtGoldensttTable>()->Get<Tag_UniQueKey>(boost::make_tuple(protoVirtualWedgeField->goldenuid()), goldenSTTV) < 0){
        TPS_LOG_DEV_ERROR << "Failed to get golden from manager.";
        return false;
    }
    goldenSTT = &goldenSTTV;

    goldenSTT->set_accessoryuid(protoVirtualWedgeField->virtualuid());
    goldenSTT->set_commissionedunituid(protoVirtualWedgeField->parentuid());
    goldenSTT->set_uid(protoVirtualWedgeField->goldenuid());

    std::vector<float> vfCorrectionFactor;
    for (int count = 0; count < protoVirtualWedgeField->virtualwedgefieldlist_size(); ++count){
        vfCorrectionFactor.push_back(protoVirtualWedgeField->virtualwedgefieldlist(count).correctionfactor());
    }

    if (protoVirtualWedgeField->algtype() == 1){                          //PENCILBEAM
        std::string str;
        FloatToStringFunc(vfCorrectionFactor,str);
        goldenSTT->set_correctionfactorpencilbeam(str);
    }
    else if (protoVirtualWedgeField->algtype() == 2){                    //CONVOLUTION
        std::string str;
        FloatToStringFunc(vfCorrectionFactor,str);
        goldenSTT->set_correctionfactorconvolution(str);
    }
    else{                                                                //MONTECARLO
        std::string str;
        FloatToStringFunc(vfCorrectionFactor,str);
        goldenSTT->set_correctionfactormontecarlo(str);
    }

    if (!manager->GetTable<RtGoldensttTable>()->Update<Tag_UniQueKey>(boost::make_tuple(goldenSTT->get_uid()), *goldenSTT))
    {
        TPS_LOG_DEV_ERROR << "update golden stt to Manager failed:";
        return false;
    }

    if (mDatabaseWrapper->UpdateGoldenstt(*goldenSTT)) {
        TPS_LOG_DEV_ERROR<<"save golden stt to TMS failed:";
        return false;
    }
    return true;
}

bool TpsBLController::FloatToStringFunc(std::vector<float> vfCorrectionFactor,std::string & str){

    for (int i=0;i<vfCorrectionFactor.size();i++)
    {
        if ( str !="")
            str = str + ",";
        str = str + boost::lexical_cast<std::string>(vfCorrectionFactor[i]);
    }
    return true;
}

bool TpsBLController::IntToStringFunc(std::vector<int> vfCorrectionFactor,std::string & str)
{
    for (int i=0;i<vfCorrectionFactor.size();i++)
    {
        if ( str !="")
            str = str + ",";
        str = str + boost::lexical_cast<std::string>(vfCorrectionFactor[i]);
    }
    return true;
}

bool TpsBLController::StringToVectorFunc( std::string& strMuDataX, std::vector<float>& DataList){

    std::vector<std::string> vecValue;
    (void)boost::split(vecValue, strMuDataX, boost::is_any_of(","));
    size_t iLen = vecValue.size();
    DataList.resize(iLen);
    for (int i=0;i<iLen;i++)
    {
        if(vecValue[i]!="")
            DataList[i] = (float)atof(vecValue[i].c_str());
    }
    vecValue.clear();

    return true;
}


int TpsBLController::UpdatePencilBeam(ms::proto::RT_MS_PencilBeam* protoPencilBeam)
{
    CertifiedPencilBeam cerPencilBeam;
    cerPencilBeam.Convert(*protoPencilBeam);

    std::string accessoryUID = cerPencilBeam.accessoryUID;
    std::string commissionUID = cerPencilBeam.commissionUID;

    RtCommissionedunit commUnit;
    if (mDataRepository->GetMachineSettingManager()->
        GetTable<RtCommissionedunitTable>()->Get<Tag_UniQueKey>(boost::make_tuple(commissionUID), commUnit) <= 0)
    {
        TPS_LOG_DEV_ERROR << "TpsBLController::UpdatePencilBeam:Get commisson Failed!";
        return TPS_ER_FAILURE;
    }
    commUnit.set_tar0depthcount(cerPencilBeam.tMRInfoList.size());

    std::vector<float> fdepth_vec;
    std::vector<float> fTAR0;

    for (int count = 0; count < cerPencilBeam.tMRInfoList.size(); ++count)
    {
		fdepth_vec.push_back(cerPencilBeam.tMRInfoList[count]->depth * 10);
		fTAR0.push_back(cerPencilBeam.tMRInfoList[count]->value);
    }

    std::string strfdepth_vec;
    FloatToStringFunc(fdepth_vec,strfdepth_vec);
    commUnit.set_tar0_depth_vec(strfdepth_vec);

    std::string strfTAR0;
    FloatToStringFunc(fTAR0, strfTAR0);
    commUnit.set_tar0(strfTAR0);

    //SMR
    commUnit.set_depthvec(cerPencilBeam.depthvec);
    commUnit.set_radiusvec(cerPencilBeam.radiusvec);
    commUnit.set_sar(cerPencilBeam.sar);

    //In Field
    commUnit.set_flatnessdepthvec(cerPencilBeam.flatnessdepthvec);
    commUnit.set_flatnessradiusvec(cerPencilBeam.flatnessradiusvec);
    commUnit.set_flatness(cerPencilBeam.flatness);

    commUnit.set_tran(cerPencilBeam.transmission);
    commUnit.set_srcdiam1(cerPencilBeam.diamOne*DB_EXCHANGE_FRONT);
    commUnit.set_srcdiam2(cerPencilBeam.diamTwo*DB_EXCHANGE_FRONT);
    commUnit.set_srcdiam3(cerPencilBeam.diamThree * DB_EXCHANGE_FRONT);
    commUnit.set_srcratio1(cerPencilBeam.srcRatio);
    commUnit.set_srcratio3(cerPencilBeam.srcRatio3);
    commUnit.set_eeqdiam(cerPencilBeam.eeqdiam * DB_EXCHANGE_FRONT);
    commUnit.set_psd(cerPencilBeam.psd * DB_EXCHANGE_FRONT);
    commUnit.set_xsdd(cerPencilBeam.sddXJaw*DB_EXCHANGE_FRONT);
    commUnit.set_ysdd(cerPencilBeam.sddYJaw*DB_EXCHANGE_FRONT);

    //update pencil beam Sc correction
    commUnit.set_scxpositions(cerPencilBeam.scXPositions);
    commUnit.set_scypositions(cerPencilBeam.scYPositions);
    commUnit.set_referencefluence(cerPencilBeam.referencefluence);
    commUnit.set_sc(cerPencilBeam.scBlocb);

    if (mDatabaseWrapper->UpdateCommissionedunit(commUnit)) {
        TPS_LOG_DEV_ERROR<<"save commission unit to DB failed:";
        return TPS_ER_FAILURE;
    }

    if (!mDataRepository->GetMachineSettingManager()->
        GetTable<RtCommissionedunitTable>()->Update<Tag_UniQueKey>(boost::make_tuple(commUnit.get_uid()), commUnit))
    {
        TPS_LOG_DEV_ERROR << "save commission unit to manager failed:";
        return TPS_ER_FAILURE;
    }
    int algType = 1;
    UpdatePhysicalWedgeField_i(cerPencilBeam.physicalWedgeList,commissionUID,algType);

    return TPS_ER_SUCCESS;
}

bool TpsBLController::UpdatePhotonMonteCarlo(ms::proto::RT_MS_PhotonMonteCarloInformation* protoPhotonMonteCarlo)
{
    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    std::string commissionUID = protoPhotonMonteCarlo->commissionuid();

    RtMcphocommissionedunit* pmcPhoCommUnit = nullptr;
    RtMcphocommissionedunit pmcPhoCommUnitV;
    if (manager->GetTable<RtMcphocommissionedunitTable>()
        ->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), pmcPhoCommUnitV) <= 0)
    {
        return false;
    }
    pmcPhoCommUnit = &pmcPhoCommUnitV;

    //PDD
    auto pddParameter = protoPhotonMonteCarlo->pddparameter();
    pmcPhoCommUnit->set_mcprimaryenergystart(pddParameter.primaryenergystart());
    pmcPhoCommUnit->set_mcprimaryenergyend(pddParameter.primaryenergyend());
    pmcPhoCommUnit->set_mcprimaryenergybin(pddParameter.primaryenergybin());
    pmcPhoCommUnit->set_mcprimaryenergyrestart(pddParameter.primaryenergyrestart());
    pmcPhoCommUnit->set_mcsecondaryenergystart(pddParameter.secondaryenergystart());
    pmcPhoCommUnit->set_mcsecondaryenergyend(pddParameter.secondaryenergyend());
    pmcPhoCommUnit->set_mcsecondaryenergybin(pddParameter.secondaryenergybin());
    pmcPhoCommUnit->set_mcsecondaryenergyrestart(pddParameter.secondaryenergyrestart());
    pmcPhoCommUnit->set_mcelectronenergy(pddParameter.electronenergy());
    pmcPhoCommUnit->set_mccommiparticlenumber(pddParameter.particlenumber());
    pmcPhoCommUnit->set_mcphoparam10(boost::lexical_cast<string>(pddParameter.completefilesnumber()));

    std::string primaryEnergyListString;
    std::string primaryWeightListString;
    for (int i = 0; i < pddParameter.primaryspectrumcollection_size(); ++i)
    {
        primaryEnergyListString += boost::lexical_cast<std::string>(pddParameter.primaryspectrumcollection(i).energy());
        primaryWeightListString += boost::lexical_cast<std::string>(pddParameter.primaryspectrumcollection(i).weight());

        //最后个值后面不加“，”
        if (i != pddParameter.primaryspectrumcollection_size()-1)
        {
            primaryEnergyListString += ",";
            primaryWeightListString += ","; 
        }
        
    }
    pmcPhoCommUnit->set_mcprimaryenergylist(primaryEnergyListString);
    pmcPhoCommUnit->set_mcprimaryweightlist(primaryWeightListString);

    std::string secondaryEnergyListString;
    std::string secondaryWeightListString;
    for (int i = 0; i < pddParameter.secondaryspectrumcollection_size(); ++i)
    {
        secondaryEnergyListString += boost::lexical_cast<std::string>(pddParameter.secondaryspectrumcollection(i).energy());
        secondaryWeightListString += boost::lexical_cast<std::string>(pddParameter.secondaryspectrumcollection(i).weight());
        if (i != pddParameter.secondaryspectrumcollection_size()-1)
        {
            secondaryEnergyListString += ",";
            secondaryWeightListString += ","; 
        }
        
    }
    pmcPhoCommUnit->set_mcsecondaryenergylist(secondaryEnergyListString);
    pmcPhoCommUnit->set_mcsecondaryweightlist(secondaryWeightListString);

    std::string elecEnergyListString;
    std::string elecWeightListString;
    for (int i = 0; i < pddParameter.electronspectrumcollection_size(); ++i)
    {
        elecEnergyListString += boost::lexical_cast<std::string>(pddParameter.electronspectrumcollection(i).energy());
        elecWeightListString += boost::lexical_cast<std::string>(pddParameter.electronspectrumcollection(i).weight());
        if (i < pddParameter.electronspectrumcollection_size()-1)
        {
            elecEnergyListString += ",";
            elecWeightListString += ","; 
        }
        
    }
    pmcPhoCommUnit->set_mcelecenergylist(elecEnergyListString);
    pmcPhoCommUnit->set_mcelecweightlist(elecWeightListString);

    //In field
    auto inFieldParameter = protoPhotonMonteCarlo->infieldparameter();
    pmcPhoCommUnit->set_mcprimarysoftening(inFieldParameter.primarysoften());
    pmcPhoCommUnit->set_mcsofteningtunningf(inFieldParameter.softenparameter());
    pmcPhoCommUnit->set_mcsecondarysourcetype(inFieldParameter.selectedsecondarysourcetype());
    pmcPhoCommUnit->set_mcscatteringcompton(inFieldParameter.scattersoften());

    std::string secondarySourceParaA;
    std::string secondarySourceParaB;//flatfiltera存奇数位，b存偶数位
    for (int i = 0; i < inFieldParameter.secondarysourceparametercollection_size(); ++i)
    {
        secondarySourceParaA += boost::lexical_cast<std::string>(inFieldParameter.secondarysourceparametercollection(i).flatfiltera());
        secondarySourceParaA +=",";
        secondarySourceParaB = boost::lexical_cast<std::string>(inFieldParameter.secondarysourceparametercollection(i).flatfilterb());
        if (i != inFieldParameter.secondarysourceparametercollection_size()-1)
        {
            secondarySourceParaB+=",";
        }
        secondarySourceParaA += secondarySourceParaB;
    }
    pmcPhoCommUnit->set_mcsecondarysourcepara(secondarySourceParaA);

    //Out field
    auto outFieldParameter = protoPhotonMonteCarlo->outfieldparameter();
    pmcPhoCommUnit->set_mcprimaryflatfilterdist(outFieldParameter.flatdistance() * 10);
    pmcPhoCommUnit->set_mcprimaryjawdistance(outFieldParameter.jawdistance() * 10);
    pmcPhoCommUnit->set_mcsourcesigmaprimary(outFieldParameter.sigmap());
    pmcPhoCommUnit->set_mcsourcesigmasecondary(outFieldParameter.sigmas());
    pmcPhoCommUnit->set_mclargefieldtailcor(outFieldParameter.tailcorrection());
    pmcPhoCommUnit->set_mcrectanglescacor(outFieldParameter.rectanglescattercorrection());
    pmcPhoCommUnit->set_mcmlcleakage(outFieldParameter.mlcleakage());
    pmcPhoCommUnit->set_mcgaussianblurswitch(outFieldParameter.gaussianblurswitch());
    pmcPhoCommUnit->set_mcgaussiansigmax(outFieldParameter.gaussiansigmax());
    pmcPhoCommUnit->set_mcgaussiansigmay(outFieldParameter.gaussiansigmay());

    //Electron Contamination
    auto electronContaminationParameter = protoPhotonMonteCarlo->electroncontaminationparameter();
    std::string elecCorFactorString;
    ostringstream s1;
    s1<<electronContaminationParameter.standardradius()<<","<<
        electronContaminationParameter.sigma()<<","<<
        electronContaminationParameter.offset()<<","<<
        electronContaminationParameter.powerfactor();
    elecCorFactorString = s1.str();
    pmcPhoCommUnit->set_mcphotonfluence(elecCorFactorString);

    //Absolute Dose
    auto absoluteDoseParameter = protoPhotonMonteCarlo->absolutedoseparameter();
    pmcPhoCommUnit->set_mcphoc3(absoluteDoseParameter.norm10dose());
    std::string absoluteDoseFieldSizeString;
    std::string absoluteDoseString;
    std::string absoluteDoseFieldSizeStringTotal;
    pmcPhoCommUnit->set_mcphoabscorrectionfactor(absoluteDoseParameter.absolutedosecollectionstr());

    //Wedge field
    auto wedgeFieldParameter = protoPhotonMonteCarlo->wedgefieldparameter();
        //(PhysicalWedge)
    std::vector<RtAccessorychunk*> accessoryChunkList;
    std::vector<RtAccessorychunk*> physicalWedgeAccessoryChunkList;

    for (int i = 0; i < wedgeFieldParameter.physicalwedgecollection_size(); ++i)
    {
        std::string sAccUid = wedgeFieldParameter.physicalwedgecollection(i).wedgeuid();
        std::string sUnitUid = protoPhotonMonteCarlo->commissionuid();
        //根据一个accUid和commission可能找到多条AccessoryChunk数据
        //再根据algorithmtype得到两条数据
        mDatabaseWrapper->GetAccessorychunkListByAccUidAndUnitUid(sAccUid,sUnitUid,accessoryChunkList);
        physicalWedgeAccessoryChunkList.clear();
        for(vector<RtAccessorychunk*>::iterator iter=accessoryChunkList.begin();iter!=accessoryChunkList.end();iter++)
        {
            if ((*iter)->get_algorithmtype() == (int)RtDbDef::MONTECARLO)
            {
                physicalWedgeAccessoryChunkList.push_back(*iter);
            }
        }

        if (physicalWedgeAccessoryChunkList.size() == 2)
        {
            //update contour table
            RtContour* physicalWedgeContour1 = nullptr;
            RtContour* physicalWedgeContour2 = nullptr;
            
            if (physicalWedgeAccessoryChunkList[0]->get_datatype() == (int)RtDbDef::PHYSICALWEDGE_WEDGE &&
                physicalWedgeAccessoryChunkList[1]->get_datatype() == (int)RtDbDef::PHYSICALWEDGE_CARRIER)
            {
                mDatabaseWrapper->GetContourByAccessorychunkuid(physicalWedgeAccessoryChunkList[0]->get_uid(),physicalWedgeContour1);
                mDatabaseWrapper->GetContourByAccessorychunkuid(physicalWedgeAccessoryChunkList[1]->get_uid(),physicalWedgeContour2);
            } 
            else if(physicalWedgeAccessoryChunkList[1]->get_datatype() == (int)RtDbDef::PHYSICALWEDGE_WEDGE &&
                    physicalWedgeAccessoryChunkList[0]->get_datatype() == (int)RtDbDef::PHYSICALWEDGE_CARRIER)
            {
                mDatabaseWrapper->GetContourByAccessorychunkuid(physicalWedgeAccessoryChunkList[0]->get_uid(),physicalWedgeContour2);
                mDatabaseWrapper->GetContourByAccessorychunkuid(physicalWedgeAccessoryChunkList[1]->get_uid(),physicalWedgeContour1);
            }

            physicalWedgeContour1->set_thickness(wedgeFieldParameter.physicalwedgecollection(i).wedgethickness());
            physicalWedgeContour2->set_thickness(wedgeFieldParameter.physicalwedgecollection(i).carrierthickness());

            db_Point3f oneDbPoints1;
            std::vector<db_Point3f> dbPoints1;
            db_Point3f oneDbPoints2;
            std::vector<db_Point3f> dbPoints2;
            dbPoints1.clear();
            for (auto item = wedgeFieldParameter.physicalwedgecollection(i).contourvertexpointcollection().begin();
                item != wedgeFieldParameter.physicalwedgecollection(i).contourvertexpointcollection().end();
                item++)
            {
                oneDbPoints1.x = (*item).x();
                oneDbPoints1.y = (*item).y();
                oneDbPoints1.z = 0;
                dbPoints1.push_back(oneDbPoints1);
            }
            physicalWedgeContour1->set_contour_points(dbPoints1);
            

            dbPoints2.clear();
            for (auto item = wedgeFieldParameter.physicalwedgecollection(i).carriercontourvertexpointcollection().begin();
                item != wedgeFieldParameter.physicalwedgecollection(i).carriercontourvertexpointcollection().end();
                item++)
            {
                oneDbPoints2.x = (*item).x();
                oneDbPoints2.y = (*item).y();
                oneDbPoints2.z = 0;
                dbPoints2.push_back(oneDbPoints2);
            }
            physicalWedgeContour2->set_contour_points(dbPoints2);

            if (mDatabaseWrapper->UpdateContourObject(*physicalWedgeContour1) ||
                mDatabaseWrapper->UpdateContourObject(*physicalWedgeContour2)) 
            {
                TPS_LOG_DEV_ERROR<<"update physicalWedgeContour to DB failed:";
                for (auto itr=accessoryChunkList.begin(); itr!=accessoryChunkList.end(); ++itr)
                {
                    TPS_LOG_DEV_ERROR << "update physicalWedgeContour to DB failed:";
                    return false;
                }
            }
        }
    }


        //(VirtualWedge)
    std::vector<RtGoldenstt*> goldenSttList;
    for (int i = 0; i < wedgeFieldParameter.virtualwedgecollection_size(); ++i)
    {
        std::string sAccUid = wedgeFieldParameter.virtualwedgecollection(i).wedgeuid();
        std::string sUnitUid = protoPhotonMonteCarlo->commissionuid();
        auto virtualwedgecollection = wedgeFieldParameter.virtualwedgecollection(i);
        //根据一个accUid和commission只能找到一条GoldenSTT数据
        mDatabaseWrapper->GetGoldensttListByAccUidAndUnitUid(sAccUid,sUnitUid,goldenSttList);
        vector<float>correctionFactorMontecarlo;

        for (int it = 0; it <virtualwedgecollection.virtualwedgetableitemcollection_size(); ++it){
                correctionFactorMontecarlo.push_back(virtualwedgecollection.virtualwedgetableitemcollection(it).correctionfactor());
            }
        std::string correctionFactorMontecarloStr;
        FloatToStringFunc(correctionFactorMontecarlo,correctionFactorMontecarloStr);

        if (goldenSttList.size()== 1)
        {
            goldenSttList[0]->set_correctionfactormontecarlo(correctionFactorMontecarloStr);

            if (mDatabaseWrapper->UpdateGoldenstt(*goldenSttList[0])) {
                TPS_LOG_DEV_ERROR<<"update goldenStt to DB failed:";
                for (auto itr=goldenSttList.begin(); itr!=goldenSttList.end(); ++itr)
                {
                    DEL_PTR(*itr);
                }
                return false;
            }
        }
    }

    if (mDatabaseWrapper->UpdateMcphocommissionedunit(*pmcPhoCommUnit)) 
    {
        TPS_LOG_DEV_ERROR<<"save PhotonMonteCarloInformation to DB failed:";
        return false;
    }

    if (!mDataRepository->GetMachineSettingManager()->GetTable<RtMcphocommissionedunitTable>()->
        Update<Tag_UniQueKey>
        (boost::make_tuple(pmcPhoCommUnit->get_uid()), *pmcPhoCommUnit))
    {
            TPS_LOG_DEV_ERROR << "update pmcPhoCommUnit to manager failed:";
            return false;
    }
    //if (manager->GetTable<RtMcphocommissionedunitTable>()->Delete<Tag_UniQueKey>
    //    (boost::make_tuple(pmcPhoCommUnit->get_uid())) > 0){
    //}

    //if (manager->GetTable<RtMcphocommissionedunitTable>()->Add(*pmcPhoCommUnit)){
    //}

    for (auto itr=goldenSttList.begin(); itr!=goldenSttList.end(); ++itr)
    {
        DEL_PTR(*itr);
    }

    return true;
}


int TpsBLController::AlgCommissionProxyInit(std::string& commissionUID, AlgCommissionProxy* mAlgCommissionProxy)
{
    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    //set CommissionUnit
    RtCommissionedunit commissionUnit;
    if (manager->GetTable<RtCommissionedunitTable>()->Get<Tag_UniQueKey>(boost::make_tuple(commissionUID), commissionUnit) <= 0)
    {
        TPS_LOG_DEV_ERROR << "Get CommissionUint Error!";
        return TPS_ER_FAILURE;
    }
    CommissionUintSuit commissionUnitSuit(&commissionUnit, manager);

    //set Machine
    RtMachine machineV;
    if (manager->GetTable<RtMachineTable>()->Get<Tag_UniQueKey>(boost::make_tuple(commissionUnit.get_machineuid()), machineV) <= 0)
    {
        TPS_LOG_DEV_ERROR << "Failed to get machine.";
        return TPS_ER_FAILURE;
    }
    RtMachine* machine = &machineV;

    MachineSuit machineSuit(machine, manager);

    mAlgCommissionProxy->Init(&machineSuit, &commissionUnitSuit);

    return TPS_ER_SUCCESS;
}

int TpsBLController::PrepareMonoPDD(std::string& commissionUID, TpsMCPrepareNotifier* pNotifier){

    if (!AlgCommissionProxyInit(commissionUID, mAlgCommissionProxy) == TPS_ER_SUCCESS)
    {
        return TPS_ER_FAILURE;
    }
    return mAlgCommissionProxy->PrepareMonoPDD(pNotifier);
}

int TpsBLController::ClearMCPrePareFileFolder(std::string& machineUID, std::string& commissionUID)
{
    machineUID;
    if (!AlgCommissionProxyInit(commissionUID, mAlgCommissionProxy) == TPS_ER_SUCCESS)
    {
        return TPS_ER_FAILURE;

    }
    return mAlgCommissionProxy->ClearMCPraPareFileFolder();
}

int TpsBLController::GetCompletePrepareFiles(std::string& machineUID, std::string& commissionUID, int& iFileNum)
{
    machineUID;
    if (!AlgCommissionProxyInit(commissionUID, mAlgCommissionProxy) == TPS_ER_SUCCESS)
    {
        return TPS_ER_FAILURE;
    }
    return mAlgCommissionProxy->ReadEneSpecStorNumber(iFileNum);
}

int TpsBLController::UpdateMonteCarlo(ms::proto::RT_MS_MonteCarlo* protoMonteCarlo){

    CertifiedMonteCarlo cerMonteCarlo;
    cerMonteCarlo.Convert(*protoMonteCarlo);
    std::string accessoryUID = cerMonteCarlo.accessory;
    std::string uid = cerMonteCarlo.uid;
    std::string parentUID = cerMonteCarlo.parentUID;

    RtMcphocommissionedunit* phoCommUnit = nullptr;
    RtMcphocommissionedunit pmcPhoCommUnitV;
    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    if (manager->GetTable<RtMcphocommissionedunitTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(parentUID), pmcPhoCommUnitV) <= 0)
    {
        TPS_LOG_DEV_ERROR << "Fail to get RtMcphocommissionedunit from manager";
        return false;
    }
    phoCommUnit = &pmcPhoCommUnitV;

    if (phoCommUnit != nullptr){
        phoCommUnit->set_commissionedunituid(parentUID);
        phoCommUnit->set_uid(uid);
        phoCommUnit->set_mcphoxjawupposition(cerMonteCarlo.xJawTopPosition*10);
        phoCommUnit->set_mcphoxjawthickness(cerMonteCarlo.xJawThickness*10);
        phoCommUnit->set_mcphomlccenterposition(cerMonteCarlo.mlcCenterPosition*10);
        phoCommUnit->set_mcphomlcthickness(cerMonteCarlo.mlcThickness*10);
        phoCommUnit->set_mcphomlleafendradius(cerMonteCarlo.mlcLeafendRadius*10);
        phoCommUnit->set_mcphomlcairgapwidth(cerMonteCarlo.airGapWidth*10);
        phoCommUnit->set_mcphomlctgwidth(cerMonteCarlo.tongueGrooveWidth*10);
        phoCommUnit->set_mcphomufactor(cerMonteCarlo.muCorrectionFactor);
        phoCommUnit->set_mcphoyjawupposition(cerMonteCarlo.yJawTopPosition*10);
        phoCommUnit->set_mcphoyjawthickness(cerMonteCarlo.yJawThickness*10);
        phoCommUnit->set_mcpholist(cerMonteCarlo.phoFilePath);
        phoCommUnit->set_mcphonorm10x10(cerMonteCarlo.normTen);

        std::vector<McPhoAbsCorrectionFactor*> absFactor;
        for (auto it = cerMonteCarlo.fieldSizeList.begin(); it != cerMonteCarlo.fieldSizeList.end(); ++it ){
            McPhoAbsCorrectionFactor correctionFactor;
            correctionFactor.m_fMcPhoAbsCorrectionFactor = (*it)->absoluteDoseCorrectionFactor;
            correctionFactor.m_fMcPhoAbsCorrectionFieldSize = (*it)->fieldSize;
            absFactor.push_back(&correctionFactor);
        }

        int iMcPhoAbsCorrectionFactors = static_cast<int>(absFactor.size());
        std::string str;
        for (int i=0;i<iMcPhoAbsCorrectionFactors;i++){
            if (str != ""){
                str = str + "|";
            }
            str = str + boost::lexical_cast<std::string>(absFactor[i]->m_fMcPhoAbsCorrectionFieldSize) + ",";
            str = str + boost::lexical_cast<std::string>(absFactor[i]->m_fMcPhoAbsCorrectionFactor);
        }
        phoCommUnit->set_mcphoabscorrectionfactor(str);

        std::string engery = cerMonteCarlo.energyValue;
        char b = ',';
        std::vector<float>num;
        StringSplit_i(engery,b,num);

        phoCommUnit->set_mcphoenergy(engery);
        phoCommUnit->set_mcphoon(cerMonteCarlo.electronContaminationFlag == 1);
        phoCommUnit->set_mcphodm(cerMonteCarlo.electronMaxDepth*10);
        phoCommUnit->set_mcphoffs10(cerMonteCarlo.surfaceDose);
        phoCommUnit->set_mcphok(cerMonteCarlo.depthCoefficientK);
        phoCommUnit->set_mcphoa(cerMonteCarlo.offaxisCoefficientA);
        phoCommUnit->set_mcphodf(cerMonteCarlo.df);
        phoCommUnit->set_mcphosf(cerMonteCarlo.sf);
        phoCommUnit->set_mcphoc1(cerMonteCarlo.cOne);
        phoCommUnit->set_mcphoc2(cerMonteCarlo.cTwo);
        phoCommUnit->set_mcphoc3(cerMonteCarlo.cThree);

        if (mDatabaseWrapper->InsertMcphocommissionedunit(*phoCommUnit)) {
            TPS_LOG_DEV_ERROR<<"save pho commission to DB failed:";
            return false;
        }

        if (!manager->GetTable<RtMcphocommissionedunitTable>()->Update<Tag_UniQueKey>(boost::make_tuple(phoCommUnit->get_uid()), *phoCommUnit)){
            TPS_LOG_DEV_ERROR << "update pho commission to manager failed:";
            return false;
    }
    }

    int algType = 3;
    UpdatePhysicalWedgeField_i(cerMonteCarlo.physicalWedgeList,parentUID,algType);

    return true;
}

bool TpsBLController::UpdatePhysicalWedgeField_i( std::vector<std::tr1::shared_ptr<CommonPhysicalWedgeField >> physicalWedgeList,
    std::string& commissionUID, int& algType){

    commissionUID;
    algType;
    if (physicalWedgeList.size() > 0){
        TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();

            RtAccessorychunk accessoryChunck;
        if (manager->GetTable<RtAccessorychunkTable>()->Get<Tag_UniQueKey>(boost::make_tuple(physicalWedgeList[0]->wedgeUID), accessoryChunck) <= 0)
        {
            TPS_LOG_DEV_ERROR << "Failed to Get Accessorychunk from manager";
            return false;
        }

            accessoryChunck.set_mu(physicalWedgeList[0]->mu);
            accessoryChunck.set_mu_dx(physicalWedgeList[0]->dmudx);
            accessoryChunck.set_mu_da(physicalWedgeList[0]->dmuda);
            accessoryChunck.set_mu_dr(physicalWedgeList[0]->dmudr);
            accessoryChunck.set_hvl_slope(physicalWedgeList[0]->hvlSlope);
            accessoryChunck.set_density(physicalWedgeList[0]->density);

        RtContour contour;
        if (manager->GetTable<RtContourTable>()->Get<Tag_UniQueKey>(boost::make_tuple(physicalWedgeList[0]->contourUID1), contour) <= 0)
        {
            TPS_LOG_DEV_ERROR << "Failed to Get RtContour from manager";
            return false;
        }
            contour.set_thickness(physicalWedgeList[0]->wedgeThick);

            std::vector<db_Point3f> vPoints;
            for (int count = 0; count < physicalWedgeList[0]->wedgeContourVerterList.size(); ++count){

                db_Point3f point;
                point.x = physicalWedgeList[0]->wedgeContourVerterList[count]->contourVerterx;
                point.y = physicalWedgeList[0]->wedgeContourVerterList[count]->contourVertery;
                vPoints.push_back(point);
            }

            contour.set_contour_points(vPoints);

        if (mDatabaseWrapper->UpdateAccessorychunk(accessoryChunck)) {          //这里有待考虑。
            TPS_LOG_DEV_ERROR << "save accessory chunck to DB failed:";
                return false;
            }

        if (!manager->GetTable<RtAccessorychunkTable>()->Update<Tag_UniQueKey>(boost::make_tuple(accessoryChunck.get_uid()), accessoryChunck))
        {
            TPS_LOG_DEV_ERROR << "save accessory chunck to Manager failed:";
                return false;
            }

        if (mDatabaseWrapper->UpdateContourObject(contour)) {
            TPS_LOG_DEV_ERROR << "save contour to DB failed:";
            return false;
        }

        if (!manager->GetTable<RtContourTable>()->Update<Tag_UniQueKey>(boost::make_tuple(contour.get_uid()), contour))
        {
            TPS_LOG_DEV_ERROR << "save contour chunck to Manager failed:";
            return false;
        }

        RtAccessorychunk accessoryChunckCarrier;
        if (manager->GetTable<RtAccessorychunkTable>()->Get<Tag_UniQueKey>(boost::make_tuple(physicalWedgeList[0]->carrierUID), accessoryChunckCarrier) <= 0)
        {
            TPS_LOG_DEV_ERROR << "Failed to Get Accessorychunk from manager";
            return false;
        }

        accessoryChunckCarrier.set_mu(physicalWedgeList[0]->carriermu);
        accessoryChunckCarrier.set_mu_dx(physicalWedgeList[0]->carrierdmudx);
        accessoryChunckCarrier.set_mu_da(physicalWedgeList[0]->carrierdmuda);
        accessoryChunckCarrier.set_mu_dr(physicalWedgeList[0]->carrierdmudr);
        accessoryChunckCarrier.set_hvl_slope(physicalWedgeList[0]->carrierhvlSlope);

        RtContour contourCarrier;
        if (manager->GetTable<RtContourTable>()->Get<Tag_UniQueKey>(boost::make_tuple(physicalWedgeList[0]->contourUID2), contourCarrier) <= 0)
        {
            TPS_LOG_DEV_ERROR << "Failed to Get RtContour from manager";
            return false;
        }

        contourCarrier.set_thickness(physicalWedgeList[0]->carrierThick);
            std::vector<db_Point3f> vCarrierPoints;
            for (int count = 0; count < physicalWedgeList[0]->carrierContourVerterList.size(); ++count){

                db_Point3f point;
                point.x = physicalWedgeList[0]->carrierContourVerterList[count]->contourVerterx;
                point.y = physicalWedgeList[0]->carrierContourVerterList[count]->contourVertery;
                vCarrierPoints.push_back(point);
            }
        contourCarrier.set_contour_points(vCarrierPoints);

        if (mDatabaseWrapper->UpdateAccessorychunk(accessoryChunckCarrier)) {          //这里有待考虑。
            TPS_LOG_DEV_ERROR << "save accessory chunck to DB failed:";
                return false;
            }

        if (!manager->GetTable<RtAccessorychunkTable>()->Update<Tag_UniQueKey>(boost::make_tuple(accessoryChunckCarrier.get_uid()), accessoryChunckCarrier))
        {
            TPS_LOG_DEV_ERROR << "save accessory chunck to Manager failed:";
                return false;
            }

        if (mDatabaseWrapper->UpdateContourObject(contourCarrier)) {
            TPS_LOG_DEV_ERROR << "save contour to DB failed:";
            return false;
        }

        if (!manager->GetTable<RtContourTable>()->Update<Tag_UniQueKey>(boost::make_tuple(contourCarrier.get_uid()), contourCarrier))
        {
            TPS_LOG_DEV_ERROR << "save contour chunck to Manager failed:";
            return false;
        }

    }

        return true;
}

bool TpsBLController::StringSplit_i(std::string str,char a,std::vector<float>num){
    std::istringstream iss(str);
    float value;
    while (iss && iss >> value){
        num.push_back(value);
        iss >> a;
    }
    return true;
}


bool TpsBLController::DbCommissionConvertToAlgCommission(RTFWK::RTFWKDLDOCommissionedunit* algCommissionedUnit,
    RtCommissionedunit& tpsCommissionedUnit){

        algCommissionedUnit->SetUID(tpsCommissionedUnit.get_uid());
        algCommissionedUnit->SetMachineUID(tpsCommissionedUnit.get_machineuid());
        algCommissionedUnit->SetKernelDataListUID(tpsCommissionedUnit.get_kerneldatalistuid());
        algCommissionedUnit->SetRadiationType((RTFWK::RADIATION_TYPE)tpsCommissionedUnit.get_radiationtype());
        algCommissionedUnit->SetEnergy(tpsCommissionedUnit.get_energy());
        algCommissionedUnit->SetRadiationType((RTFWK::RADIATION_TYPE)tpsCommissionedUnit.get_radiationtype());
        algCommissionedUnit->SetMaximumDoseRate_MUperMinute(tpsCommissionedUnit.get_maximumdoserate_muperminute());
        RTFWK::RTFWKDLDOMachineUnit* machineUnit = new RTFWK::RTFWKDLDOMachineUnit();
        SetMachineUnit_i(machineUnit, tpsCommissionedUnit);
        algCommissionedUnit->SetMachineUnit(machineUnit);
        RTFWK::RTFWKDLDOSartable* sar = new RTFWK::RTFWKDLDOSartable();
        SetSarTable_i(sar, tpsCommissionedUnit);
        algCommissionedUnit->SetSartable(sar);
        RTFWK::RTFWKDLDOTimecalParameters* pTimecal = new RTFWK::RTFWKDLDOTimecalParameters();
        //SetTimecal_i(pTimecal, tpsCommissionedUnit);
        algCommissionedUnit->SetTimeCal(pTimecal);
        RTFWK::RTFWKDLDOConvolutionData* pConvolutionData = new RTFWK::RTFWKDLDOConvolutionData();
        SetConvolutionData_i(pConvolutionData, tpsCommissionedUnit);
        algCommissionedUnit->SetConvolutionData(pConvolutionData);
        algCommissionedUnit->SetAllowContinuousDoseRate(tpsCommissionedUnit.get_allowcontinuousdoserate());
        algCommissionedUnit->SetDiscreteDoseRate(tpsCommissionedUnit.get_discretedoserate());
        algCommissionedUnit->SetIsFlattened(tpsCommissionedUnit.get_isflattened());
        algCommissionedUnit->SetIsConvolutionSupport(tpsCommissionedUnit.get_isconvolutionsupport());
        algCommissionedUnit->SetIsPencileBeamSupport(tpsCommissionedUnit.get_ispencilebeamsupport());
        algCommissionedUnit->SetIsMonteCarloSupport(tpsCommissionedUnit.get_ismontecarlosupport());
        // Set AvailablePhotonPlanType
        std::vector<std::string> vecValue;
        std::vector<RTFWK::PLAN_TYPE> vAvailablePhotonPlanType;
        std::string strAvailablePhotonPlanType = tpsCommissionedUnit.get_availablephotonplantype();
        (void)boost::split(vecValue,strAvailablePhotonPlanType, boost::is_any_of(","));
        size_t iLen = vecValue.size();
        vAvailablePhotonPlanType.resize(iLen);
        for (int i=0; i<iLen;i++)
        {
            if(vecValue[i]!="")
                vAvailablePhotonPlanType[i] = static_cast<RTFWK::PLAN_TYPE>(atoi(vecValue[i].c_str()));
        }
        vecValue.clear();
        algCommissionedUnit->SetAvailablePhotonPlanType(vAvailablePhotonPlanType);
        // Set AvailableElectronPlanType 
        std::vector<RTFWK::PLAN_ELECTORN_TYPE> vAvailableElectronPlanType;
        std::string strAvailableElectronPlanType = tpsCommissionedUnit.get_availableelectronplantype();
        (void)boost::split(vecValue,strAvailableElectronPlanType, boost::is_any_of(","));
        iLen = vecValue.size();
        vAvailableElectronPlanType.resize(iLen);
        for (int i=0; i<iLen;i++)
        {
            if(vecValue[i]!="")
                vAvailableElectronPlanType[i] = static_cast<RTFWK::PLAN_ELECTORN_TYPE>(atoi(vecValue[i].c_str()));
        }
        vecValue.clear();
        algCommissionedUnit->SetAvailableElectronPlanType(vAvailableElectronPlanType);
        algCommissionedUnit->SetCalibrationFsX(tpsCommissionedUnit.get_calibrationfsx());
        algCommissionedUnit->SetCalibrationFsY(tpsCommissionedUnit.get_calibrationfsy());
        algCommissionedUnit->SetCommissionedUnit_Dmax(tpsCommissionedUnit.get_commissionedunit_dmax());
        algCommissionedUnit->SetSSD(tpsCommissionedUnit.get_ssd());
        algCommissionedUnit->SetPhantomSizeDepth(tpsCommissionedUnit.get_phantomsizedepth());
        algCommissionedUnit->SetPhantomSizeLateral(tpsCommissionedUnit.get_phantomsizelateral());
        algCommissionedUnit->SetBlockTrayPenumbra(tpsCommissionedUnit.get_blocktraypenumbra());
        algCommissionedUnit->SetBlockAttenFactor(tpsCommissionedUnit.get_blockattenfactor());
        //algCommissionedUnit->SetBlockTrayTrayFactor(tpsCommissionedUnit.get_blocktraytrayfactor());
        algCommissionedUnit->SetBlockAttenFactor(tpsCommissionedUnit.get_blockattenfactor());
        // Set TrayFactor
        std::map<std::string, float> trays = tpsCommissionedUnit.get_discrete_trayfactor();
        std::vector<RTFWK::RTFWKDLDOTrayFactor*> vTrayFactor;
        for(std::map<std::string, float>::iterator it = trays.begin(); it != trays.end(); ++it){
            RTFWK::RTFWKDLDOTrayFactor* trayFactor = new RTFWK::RTFWKDLDOTrayFactor();
            trayFactor->m_sTrayUID = it->first;
            trayFactor->m_fTrayFactor = it->second;
            vTrayFactor.push_back(trayFactor);
        }
        algCommissionedUnit->SetTrayFactor(vTrayFactor);
        algCommissionedUnit->SetNormalizedDose(tpsCommissionedUnit.get_normalizeddose());
        algCommissionedUnit->SetXJawFactor(tpsCommissionedUnit.get_xjawfactor());
        algCommissionedUnit->SetYJawFactor(tpsCommissionedUnit.get_yjawfactor());
        algCommissionedUnit->SetMLCFactor(tpsCommissionedUnit.get_mlcfactor());

        //algCommissionedUnit->SetMcPhoCommissionedUnit()
        return true;
}

void TpsBLController::SetMachineUnit_i(RTFWK::RTFWKDLDOMachineUnit* machineUnit, RtCommissionedunit& tpsCommissionedUnit){

    machineUnit->SetHidden(tpsCommissionedUnit.get_hidden());
    machineUnit->SetName(tpsCommissionedUnit.get_name());
    machineUnit->SetDmax(tpsCommissionedUnit.get_dmax());
    machineUnit->SetSAD(tpsCommissionedUnit.get_sad());
    machineUnit->SetPSD(tpsCommissionedUnit.get_psd());
    machineUnit->Setsrc_diam1(tpsCommissionedUnit.get_srcdiam1());
    machineUnit->Setsrc_diam2(tpsCommissionedUnit.get_srcdiam2());
    machineUnit->Setsrc_diam3(tpsCommissionedUnit.get_srcdiam3());
    machineUnit->Setsrc_ratio1(tpsCommissionedUnit.get_srcratio1());
    machineUnit->Setsrc_ratio3(tpsCommissionedUnit.get_srcratio3());
    machineUnit->Sete_eq_diam(tpsCommissionedUnit.get_eeqdiam());
    machineUnit->Setx_jaw_count(tpsCommissionedUnit.get_xjawcount());
    machineUnit->Sety_jaw_count(tpsCommissionedUnit.get_yjawcount());
    machineUnit->Setx_jaw_type(tpsCommissionedUnit.get_xjawtype());
    machineUnit->Sety_jaw_type(tpsCommissionedUnit.get_yjawtype());
    machineUnit->SetXSDD(tpsCommissionedUnit.get_xsdd());
    machineUnit->SetYSDD(tpsCommissionedUnit.get_ysdd());
    machineUnit->Setgantry_sense(tpsCommissionedUnit.get_gantrysense());
    machineUnit->Setgantry_offset(tpsCommissionedUnit.get_gantryoffset());
    machineUnit->Settable_offset(tpsCommissionedUnit.get_tableoffset());
    machineUnit->Settable_sense(tpsCommissionedUnit.get_tablesense());
    machineUnit->Setcollimator_offset(tpsCommissionedUnit.get_collimatoroffset());
    machineUnit->Setcollimator_sense(tpsCommissionedUnit.get_collimatorsense());
    machineUnit->SetReferenceFluence(tpsCommissionedUnit.get_referencefluence());
    machineUnit->SetReferenceDose(tpsCommissionedUnit.get_referencedose());
}

bool TpsBLController::SetConvolutionData_i(RTFWK::RTFWKDLDOConvolutionData* convolutionData, RtCommissionedunit& tpsCommissionedUnit){

    std::vector<std::string> vecValue;
    RTFWK::RTFWKDLDOConvDataStruct convDataStruct;
    convDataStruct.SetDiam1(tpsCommissionedUnit.get_diam1());
    convDataStruct.SetDiam2(tpsCommissionedUnit.get_diam2());
    convDataStruct.SetRatio1(tpsCommissionedUnit.get_ratio1());
    convDataStruct.SetTransmission(tpsCommissionedUnit.get_transmission());
    convDataStruct.SetMu(tpsCommissionedUnit.get_mu());
    convDataStruct.SetDmu_dx(tpsCommissionedUnit.get_dmu_dx());
    convDataStruct.SetDmu_dr(tpsCommissionedUnit.get_dmu_dr());
    std::string strOffAxisDataX = tpsCommissionedUnit.get_offaxisdatax();
    (void)boost::split(vecValue, strOffAxisDataX, boost::is_any_of(","));
    size_t iLen = vecValue.size();
    convDataStruct.m_vOffAxisDataX.resize(iLen);
    for (int i=0;i<iLen;i++)
    {
        if(vecValue[i]!="")
            convDataStruct.m_vOffAxisDataX[i] = static_cast<float>(atof(vecValue[i].c_str()));
    }
    vecValue.clear();
    std::string strOffAxisDataY = tpsCommissionedUnit.get_offaxisdatay();
    (void)boost::split(vecValue, strOffAxisDataY, boost::is_any_of(","));

    iLen = vecValue.size();
    convDataStruct.m_vOffAxisDataY.resize(iLen);
    for (int i=0;i<iLen;i++)
    {
        if(vecValue[i]!="")
            convDataStruct.m_vOffAxisDataY[i] = static_cast<float>(atof(vecValue[i].c_str()));
    }
    vecValue.clear();
    std::string strMuDataX = tpsCommissionedUnit.get_mudatax();
    (void)boost::split(vecValue, strMuDataX, boost::is_any_of(","));

    iLen = vecValue.size();
    convDataStruct.m_vMuDataX.resize(iLen);
    for (int i=0;i<iLen;i++)
    {
        if(vecValue[i]!="")
            convDataStruct.m_vMuDataX[i] = (float)atof(vecValue[i].c_str());
    }
    vecValue.clear();
    std::string strMuDataY = tpsCommissionedUnit.get_mudatay();
    (void)boost::split(vecValue, strMuDataY, boost::is_any_of(","));
    iLen = vecValue.size();
    convDataStruct.m_vMuDataY.resize(iLen);
    for (int i=0;i<iLen;i++)
    {
        if(vecValue[i]!="")
            convDataStruct.m_vMuDataY[i] = (float)atof(vecValue[i].c_str());
    }
    vecValue.clear();
    std::string strDMuDataY = tpsCommissionedUnit.get_dmudatay();
    (void)boost::split(vecValue, strDMuDataY, boost::is_any_of(","));

    iLen = vecValue.size();
    convDataStruct.m_vDMuDataY.resize(iLen);
    for (int i=0;i<iLen;i++)
    {
        if(vecValue[i]!="")
            convDataStruct.m_vDMuDataY[i] = (float)atof(vecValue[i].c_str());
    }
    // wzd-2017-01-10 此处需重写
    /* vecValue.clear();
    std::string strOutputMuY = tpsCommissionedUnit.get_outputmuy();
    (void)boost::split(vecValue, strOutputMuY, boost::is_any_of(","));
    iLen = vecValue.size();
    convDataStruct.m_vOutputMuY.resize(iLen);
    for (int i=0;i<iLen;i++)
    {
    if(vecValue[i]!="")
    convDataStruct.m_vOutputMuY[i] = (float)atof(vecValue[i].c_str());
    }*/
    vecValue.clear();
    std::string strDDMuDR = tpsCommissionedUnit.get_ddmudr();
    (void)boost::split(vecValue, strDDMuDR, boost::is_any_of(","));

    iLen = vecValue.size();
    convDataStruct.m_vDDMuDR.resize(iLen);
    for (int i=0;i<iLen;i++)
    {
        if(vecValue[i]!="")
            convDataStruct.m_vDDMuDR[i] = (float)atof(vecValue[i].c_str());
    }
    vecValue.clear();
    convDataStruct.SetSrc_d(tpsCommissionedUnit.get_src_d());
    convDataStruct.SetSrc_atten(tpsCommissionedUnit.get_src_atten());
    convDataStruct.SetNorm_10x10(tpsCommissionedUnit.get_norm_10x10());
    //convDataStruct.SetOn(tpsCommissionedUnit.get_on);
    convDataStruct.SetDm(tpsCommissionedUnit.get_dm());
    convDataStruct.SetFfs10(tpsCommissionedUnit.get_ffs10());
    convDataStruct.SetK(tpsCommissionedUnit.get_k());
    convDataStruct.SetA(tpsCommissionedUnit.get_a());
    convDataStruct.SetDF(tpsCommissionedUnit.get_df());
    convDataStruct.SetSF(tpsCommissionedUnit.get_sf());
    convDataStruct.SetC1(tpsCommissionedUnit.get_c1());
    convDataStruct.SetC2(tpsCommissionedUnit.get_c2());
    convDataStruct.SetC3(tpsCommissionedUnit.get_c3());
    convDataStruct.SetConvDataStruct_PSD(tpsCommissionedUnit.get_convdatastruct_psd());
    convDataStruct.SetConvDataStruct_SDD1(tpsCommissionedUnit.get_convdatastruct_sdd1());
    convDataStruct.SetConvDataStruct_SDD2(tpsCommissionedUnit.get_convdatastruct_sdd2());
    convolutionData->SetConvDataStruct(convDataStruct);


    return true;

}

bool TpsBLController::SetSarTable_i(RTFWK::RTFWKDLDOSartable* sar, RtCommissionedunit& tpsCommissionedUnit){
    std::vector<std::string> vecValue;
    sar->SetType(tpsCommissionedUnit.get_type());
    sar->Setversion(tpsCommissionedUnit.get_version());
    sar->Settran(tpsCommissionedUnit.get_tran());
    sar->Setflatness_depth_count(tpsCommissionedUnit.get_flatnessdepthcount());
    sar->Setflatness_radius_count(tpsCommissionedUnit.get_flatnessradiuscount());
    float m_fflatness_depth_vec[MAX_FLATNESS_DEPTHS];
    std::string strflatness_depth_vec = tpsCommissionedUnit.get_flatnessdepthvec();
    (void)boost::split(vecValue,strflatness_depth_vec, boost::is_any_of(","));
    for (int i=0;i<MAX_FLATNESS_DEPTHS && i<vecValue.size();i++)
    {
        if(vecValue[i]!="")
            m_fflatness_depth_vec[i] = (float)atof(vecValue[i].c_str());
    }
    vecValue.clear();
    sar->Setflatness_depth_vec(m_fflatness_depth_vec);
    float m_fflatness_radius_vec[MAX_FLATNESS_RADII];
    std::string strflatness_radius_vec = tpsCommissionedUnit.get_flatnessradiusvec();
    (void)boost::split(vecValue,strflatness_radius_vec, boost::is_any_of(","));
    for (int i=0;i<MAX_FLATNESS_RADII && i<vecValue.size();i++)
    {
        if(vecValue[i]!="")
            m_fflatness_radius_vec[i] = (float)atof(vecValue[i].c_str());
    }
    vecValue.clear();
    sar->Setflatness_radius_vec(m_fflatness_radius_vec);
    sar->Setdepth_count(tpsCommissionedUnit.get_depthcount());
    sar->Setradius_count(tpsCommissionedUnit.get_radiuscount());
    float m_fdepth_vec[MAX_SAR_DEPTHS];
    std::string strdepth_vec = tpsCommissionedUnit.get_depthvec();
    (void)boost::split(vecValue, strdepth_vec, boost::is_any_of(","));
    for (int i=0;i<MAX_SAR_DEPTHS && i<vecValue.size();i++)
    {
        if(vecValue[i]!="")
            m_fdepth_vec[i] = (float)atof(vecValue[i].c_str());
    }
    vecValue.clear();
    sar->Setdepth_vec(m_fdepth_vec);
    float m_fradius_vec[MAX_SAR_RADII];
    std::string strradius_vec = tpsCommissionedUnit.get_radiusvec();
    (void)boost::split(vecValue, strradius_vec, boost::is_any_of(","));
    for (int i=0;i<MAX_SAR_RADII && i<vecValue.size();i++)
    {
        if(vecValue[i]!="")
            m_fradius_vec[i] = (float)atof(vecValue[i].c_str());
    }
    vecValue.clear();
    sar->Setradius_vec(m_fradius_vec);
    float m_fSAR[MAX_SAR_DEPTHS][MAX_SAR_RADII];
    std::string strSAR = tpsCommissionedUnit.get_sar();
    (void)boost::split(vecValue, strSAR, boost::is_any_of(","));
    int iCount = 0;
    for (int i=0;i<MAX_SAR_DEPTHS && i<tpsCommissionedUnit.get_depthcount();i++)
    {
        for (int j=0;j<MAX_SAR_RADII && j<tpsCommissionedUnit.get_radiuscount();j++)
        {
            if(vecValue[iCount]!="")
            {
                m_fSAR[i][j] = (float)atof(vecValue[iCount].c_str());
                iCount++;
            }
        }
    }
    vecValue.clear();
    sar->SetSAR(m_fSAR);
    sar->SetTAR0_depth_count(tpsCommissionedUnit.get_tar0depthcount());
    float m_fTAR0_depth_vec[MAX_SAR_DEPTHS];
    std::string strTAR0_depth_vec = tpsCommissionedUnit.get_tar0_depth_vec();
    (void)boost::split(vecValue, strTAR0_depth_vec, boost::is_any_of(","));
    for (int i=0;i<MAX_SAR_DEPTHS && i<vecValue.size();i++)
    {
        if(vecValue[i]!="")
            m_fTAR0_depth_vec[i] = (float)atof(vecValue[i].c_str());
    }
    vecValue.clear();
    sar->SetTAR0_depth_vec(m_fTAR0_depth_vec);
    float m_fTAR0[MAX_SAR_DEPTHS];
    std::string strTAR0 = tpsCommissionedUnit.get_tar0();
    (void)boost::split(vecValue, strTAR0, boost::is_any_of(","));
    for (int i=0;i<MAX_SAR_DEPTHS && i<vecValue.size();i++)
    {
        if(vecValue[i]!="")
            m_fTAR0[i] = (float)atof(vecValue[i].c_str());
    }
    vecValue.clear();
    sar->SetTAR0(m_fTAR0);
    float m_fflatness[MAX_FLATNESS_RADII][MAX_FLATNESS_DEPTHS];
    std::string strflatness = tpsCommissionedUnit.get_flatness();
    (void)boost::split(vecValue,strflatness, boost::is_any_of(","));
    iCount = 0;
    for (int i=0;i<MAX_FLATNESS_RADII && i<tpsCommissionedUnit.get_flatnessradiuscount();i++)
    {
        for (int j=0;j<MAX_FLATNESS_DEPTHS && j<tpsCommissionedUnit.get_flatnessdepthcount();j++)
        {
            if(vecValue[iCount]!="")
            {
                m_fflatness[i][j] = (float)atof(vecValue[iCount].c_str());
                iCount++;
            }
        }
    }
    vecValue.clear();
    sar->Setflatness(m_fflatness);
    float m_fSp[MAX_SAR_RADII];
    std::string strSp = tpsCommissionedUnit.get_sp();
    (void)boost::split(vecValue,strSp, boost::is_any_of(","));
    for (int i=0;i<MAX_SAR_RADII && i<vecValue.size();i++)
    {
        if(vecValue[i]!="")
            m_fSp[i] = (float)atof(vecValue[i].c_str());
    }
    vecValue.clear();
    sar->SetSp(m_fSp);
    sar->SetSp0(tpsCommissionedUnit.get_sp0());
    float m_fSp_prime[MAX_SAR_RADII];
    std::string strSp_prime = tpsCommissionedUnit.get_sp_prime();
    (void)boost::split(vecValue, strSp_prime, boost::is_any_of(","));
    for (int i=0;i<MAX_SAR_RADII && i<vecValue.size();i++)
    {
        if(vecValue[i]!="")
            m_fSp_prime[i] = (float)atof(vecValue[i].c_str());
    }
    vecValue.clear();
    sar->SetSp_prime(m_fSp_prime);

    return true;
}

bool TpsBLController::DbMachineConvertToAlgMachine(RTFWK::RTFWKDLDOMachine* algMachine, RtMachine& tpsMachine, std::vector<RtTray>& vTray){
    algMachine->SetUID(tpsMachine.get_uid());
    algMachine->SetGantryMax(tpsMachine.get_gantrymax());
    algMachine->SetGantryMin(tpsMachine.get_gantrymin());
    algMachine->SetTableAngleMax(tpsMachine.get_tableanglemax());
    algMachine->SetTableAngleMin(tpsMachine.get_tableanglemin());
    //SetMLCSetting
    RTFWK::RTFWKDLDOMLCSetting* pMLC = new RTFWK::RTFWKDLDOMLCSetting();
    SetMLCSetting_i(pMLC, tpsMachine);
    algMachine->SetMLCSetting(pMLC);
    algMachine->SetMaximumGantrySpeed_degreePerSecond(tpsMachine.get_maximumgantryspeed_degreepersecond());
    algMachine->SetMaximumLeafSpeed_mmPerSecond(tpsMachine.get_maximumleafspeed_mmpersecond());
    algMachine->SetDeviceID(tpsMachine.get_deviceid());
    algMachine->SetCollimatorAngleMax(tpsMachine.get_collimatoranglemax());
    algMachine->SetCollimatorAngleMin(tpsMachine.get_collimatoranglemin());
    algMachine->SetGantryClockWiseRange(tpsMachine.get_gantryclockwiserange());
    algMachine->SetGantryCounterClockWiseRange(tpsMachine.get_gantrycounterclockwiserange());
    // Set AvailablePlanType
    std::vector<RTFWK::PLAN_TYPE> m_vAvailablePlanType;
    std::vector<std::string> vecValue;
    std::string strAvailablePlanType = tpsMachine.get_availableplantype();
    (void)boost::split(vecValue,strAvailablePlanType, boost::is_any_of(","));
    size_t iLen = vecValue.size();
    m_vAvailablePlanType.resize(iLen);
    for (int i=0; i<iLen;i++)
    {
        if(vecValue[i]!="")
            m_vAvailablePlanType[i] = static_cast<RTFWK::PLAN_TYPE>(atoi(vecValue[i].c_str()));
    }
    vecValue.clear();
    algMachine->SetAvailablePlanType(m_vAvailablePlanType);
    algMachine->SetSupportApetureBlock(tpsMachine.get_supportapetureblock());
    algMachine->SetSupportShieldBlock(tpsMachine.get_supportshieldblock());
    algMachine->SetSourceToBlockTrayDistance(tpsMachine.get_sourcetoblocktraydistance());
    algMachine->SetBlockHeight(tpsMachine.get_blockheight());
    // SetTrayObjectList
    std::vector<RTFWK::RTFWKDLDOTray*> vAlgTray;
    for (std::vector<RtTray>::iterator it = vTray.begin(); it != vTray.end(); ++it) {
        if ((it)->get_machineuid() == tpsMachine.get_uid()){
            RTFWK::RTFWKDLDOTray* tray = new RTFWK::RTFWKDLDOTray();
            tray->SetCode((it)->get_code());
            tray->SetMachineUID((it)->get_machineuid());
            tray->SetTrayID((it)->get_trayid());
            tray->SetUID((it)->get_uid());
            vAlgTray.push_back(tray);
        }
    }
    algMachine->SetTrayObjectList(vAlgTray);
    algMachine->SetSourceToWedgeTopDistance(tpsMachine.get_sourcetowedgetopdistance());
    algMachine->SetSourceToApplicatorDistance(tpsMachine.get_sourcetoapplicatordistance());
    algMachine->SetBlockThickness(tpsMachine.get_blockthickness());
    algMachine->SetSAD(tpsMachine.get_sad());

    return true;
}


bool TpsBLController::SetMLCSetting_i(RTFWK::RTFWKDLDOMLCSetting* algMlcSetting, RtMachine& tpsMachine){

    algMlcSetting->SetXJawNum(tpsMachine.get_xjawnum());
    algMlcSetting->SetYJawNum(tpsMachine.get_yjawnum());
    algMlcSetting->SetLeaivesNum(tpsMachine.get_leafnum());
    algMlcSetting->SetLeafBoundaries(tpsMachine.get_leafboundaries_list());
    algMlcSetting->SetNegativeLeafRangeMax(!tpsMachine.get_negativeleafrangemax());
    algMlcSetting->SetNegativeLeafRangeMin(!tpsMachine.get_negativeleafrangemin());
    algMlcSetting->SetPositiveLeafRangeMax(tpsMachine.get_positiveleafrangemax());
    algMlcSetting->SetPositiveLeafRangeMin(tpsMachine.get_positiveleafrangemin());
    algMlcSetting->SetMLCLeafDirection((RTFWK::MLC_LEAF_DIRECTION_TYPE)tpsMachine.get_mlcdirectiontype());
    algMlcSetting->SetJawXIsSymmetry(tpsMachine.get_jawxissymmetry());
    algMlcSetting->SetJawYIsSymmetry(tpsMachine.get_jawyissymmetry());
    algMlcSetting->SetInterdigitation(tpsMachine.get_isinterdigitation());
    algMlcSetting->SetLeafSpan(tpsMachine.get_leafspan());
    algMlcSetting->SetMinLeafEndGap(tpsMachine.get_minleafendgap());
    algMlcSetting->SetMaximumJawSpeed_mmPerSecond(tpsMachine.get_maximumjawspeed_mmpersecond());
    algMlcSetting->SetMinMLCJawCover(tpsMachine.get_minimumjawgap());
    algMlcSetting->SetDynamicTrackingJawX(tpsMachine.get_dynamictrackingjawx());
    algMlcSetting->SetDynamicTrackingJawY(tpsMachine.get_dynamictrackingjawy());
    algMlcSetting->SetSourceToMLCTopDistance(tpsMachine.get_sourcetomlctopdistance());
    algMlcSetting->SetJawTrackingGap(tpsMachine.get_jawtrackinggap());
    RTFWK::RTFWKDLDOJawLimits* jawLimitx = new RTFWK::RTFWKDLDOJawLimits();
    jawLimitx->SetIsXJawLimit(true);
    jawLimitx->Setindependent(!tpsMachine.get_jawxissymmetry());
    jawLimitx->Setmax_1(tpsMachine.get_xmax_1());
    jawLimitx->Setmax_2(tpsMachine.get_xmax_2());
    jawLimitx->Setmax_w(tpsMachine.get_xmax_w());
    jawLimitx->Setmin_1(tpsMachine.get_xmin_1());
    jawLimitx->Setmin_2(tpsMachine.get_xmin_2());
    jawLimitx->Setmin_w(tpsMachine.get_xmin_w());
    jawLimitx->SetSDD(tpsMachine.get_xsdd());
    jawLimitx->Setname_1(tpsMachine.get_xname_1());
    jawLimitx->Setname_2(tpsMachine.get_xname_2());
    RTFWK::RTFWKDLDOJawLimits* jawLimity = new RTFWK::RTFWKDLDOJawLimits();
    jawLimity->SetIsXJawLimit(false);
    jawLimitx->Setindependent(!tpsMachine.get_jawyissymmetry());
    jawLimity->Setmax_1(tpsMachine.get_ymax_1());
    jawLimity->Setmax_2(tpsMachine.get_ymax_2());
    jawLimity->Setmax_w(tpsMachine.get_ymax_w());
    jawLimity->Setmin_1(tpsMachine.get_ymin_1());
    jawLimity->Setmin_2(tpsMachine.get_ymin_2());
    jawLimity->Setmin_w(tpsMachine.get_ymin_w());
    jawLimity->SetSDD(tpsMachine.get_ysdd());
    jawLimity->Setname_1(tpsMachine.get_yname_1());
    jawLimity->Setname_2(tpsMachine.get_yname_2());
    algMlcSetting->SetJawLimitX(jawLimitx);
    algMlcSetting->SetJawLimitY(jawLimity);

    return true;
}

//归一化MeasureData。
int TpsBLController::RescaleMeasureDataInfo(bool isAbsMode, std::string commissionUID){

    if (!AlgCommissionProxyInit(commissionUID, mAlgCommissionProxy) == TPS_ER_SUCCESS)
    {
        return TPS_ER_FAILURE;
    }

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    //set MeasureProfile
    std::vector<RtMeasuredprofile> measuredProfileList;
    manager->GetTable<RtMeasuredprofileTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), measuredProfileList);

    if (measuredProfileList.size() == 0)
    {
        TPS_LOG_DEV_ERROR << "TpsBLController::RescaleMeasureDataInfo:MeasuredProfileList size is 0!Return.";
        return false;
    }

    std::vector<RtMeasuredprofile*> vMeasuredProfile;
    for (auto it = measuredProfileList.begin(); it != measuredProfileList.end(); it++)
    {
        vMeasuredProfile.push_back(&(*it));
    }
    mAlgCommissionProxy->SetMeasureProfileList(vMeasuredProfile);

    //set MeasureData and wedgefactor
    std::vector<RtMeasureddata> measureDataList;
    manager->GetTable<RtMeasureddataTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), measureDataList);
    if (measureDataList.size() != 1)//表设计缺陷，是一一对应关系
    {
        TPS_LOG_DEV_ERROR << "get measure data  to DB failed:";
        return false;
    }
    std::vector<RtWedgefactor> wedgefactorList;
    manager->GetTable<RtWedgefactorTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), wedgefactorList);
    RtWedgefactor* wedgefactor = nullptr;
    if (wedgefactorList.size() != 1)//表设计缺陷，是一一对应关系
    {
        TPS_LOG_DEV_WARNING << "get wedgefactor data from manager failed!";
    }
    else
    {
        wedgefactor = &wedgefactorList[0];
    }

    mAlgCommissionProxy->RescaleCurves(isAbsMode, &measureDataList[0], wedgefactor);

    std::vector<std::shared_ptr<RtMeasuredprofile>> dbCalcProfileList;
    mAlgCommissionProxy->GetMeasureProfileList(dbCalcProfileList);

    for (auto measureProfile = dbCalcProfileList.begin(); measureProfile != dbCalcProfileList.end(); ++measureProfile)
    {
        int status = (*measureProfile)->get_viewstatus();
        //Rescale 以后要更新statu状态
        if (status == NewComputeResult)
            (*measureProfile)->set_viewstatus(OldComputeResult);

        if (!manager->GetTable<RtMeasuredprofileTable>()->Update<Tag_UniQueKey>(boost::make_tuple((*measureProfile)->get_uid()), **measureProfile)){
            TPS_LOG_DEV_ERROR << "update measure profile to manager failed:";
            return false;
        }
        if (mDatabaseWrapper->UpdateMeasuredprofile(**measureProfile)) {
            TPS_LOG_DEV_ERROR << "update measure profile to DB failed:";
            return false;
        }
    }
    return true;
}

//计算改变的是计算集合。
int TpsBLController::ComputerMeasureDataInfo(ms::proto::RT_MS_ImportFromFileList* measureUIDList, const bool& isCalcAbsOrRel, std::vector<shared_ptr<RtMeasuredprofile>>& importFromDataList,
    float& minAxisX, float& maxAxisX, float& minAxisY, float& maxAxisY){

    std::string commissionUID;
    commissionUID = measureUIDList->commissionuid();

    if (!AlgCommissionProxyInit(commissionUID, mAlgCommissionProxy) == TPS_ER_SUCCESS)
    {
        return TPS_ER_FAILURE;
    }

    int algType = -1;
    algType = measureUIDList->algtype();

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();

    //set MeasureProfile
    std::vector<RtMeasuredprofile> measuredProfileListV;
    for (int count = 0; count < measureUIDList->importfromfilelist_size(); ++count){
        RtMeasuredprofile measureProfile;
        if (manager->GetTable<RtMeasuredprofileTable>()->Get<Tag_UniQueKey>(boost::make_tuple(measureUIDList->importfromfilelist(count).uid()), measureProfile) > 0)
        {
            measuredProfileListV.push_back(measureProfile);
        }
    }
    std::vector<RtMeasuredprofile*> measuredProfileList;
    for (auto it = measuredProfileListV.begin(); it != measuredProfileListV.end(); it++)
    {
        measuredProfileList.push_back(&(*it));
    }
    mAlgCommissionProxy->SetMeasureProfileList(measuredProfileList);


    float minDepth, maxDepth;
    GetMeasureDataDepthMaxAndMin_i(measuredProfileList, 1, minDepth, maxDepth);     //1表示是测量集合变化了。

    int result;
    result = mAlgCommissionProxy->CalcCommiDose((RTFWK::DOSEALGORITHM_TYPE)algType, isCalcAbsOrRel);
    if (result != 0)
    {
        return TPS_ER_FAILURE;
    }

    mAlgCommissionProxy->GetMeasureProfileList(importFromDataList);

    std::vector<RtMeasuredprofile*> dbCalcProfileList;
    for (auto calPro = importFromDataList.begin(); calPro != importFromDataList.end(); calPro++)
    {
        (*calPro)->set_viewstatus(NewComputeResult);
        dbCalcProfileList.push_back(calPro->get());
    }

    GetMeasureDataMaxandMinXY(dbCalcProfileList, minAxisX, maxAxisX, minAxisY, maxAxisY);

    for (auto measureProfile = dbCalcProfileList.begin(); measureProfile != dbCalcProfileList.end(); ++measureProfile)
    {
        if (mDatabaseWrapper->UpdateMeasuredprofile(**measureProfile))
        {
            TPS_LOG_DEV_ERROR << "update measure profile to DB failed!";
            importFromDataList.clear();
            return TPS_ER_FAILURE;
        }

        if (!manager->GetTable<RtMeasuredprofileTable>()->Update<Tag_UniQueKey>(boost::make_tuple((*measureProfile)->get_uid()), **measureProfile))
        {
            TPS_LOG_DEV_ERROR << "update measure profile to manager failed!";
            importFromDataList.clear();
            return TPS_ER_FAILURE;
        }
    }
    return TPS_ER_SUCCESS;
}


int TpsBLController::GetMeasureDataDepthMaxAndMin_i(std::vector<RtMeasuredprofile*>& measureProfileList, int pointDoseType,float& min,float& max){

    std::vector<float> zPoint;
    std::vector<float> vMin,vMax;
    for (auto measureProfile = measureProfileList.begin(); measureProfile != measureProfileList.end(); ++measureProfile){
        float minDepth,maxDepth;
        std::vector<PointDose*>pointDoseList;
        int typeMode = (*measureProfile)->get_curvetype();
        if (pointDoseType ==1){
            pointDoseList = (*measureProfile)->GetMeaPointDoseList();
        }
        else{
            pointDoseList = (*measureProfile)->GetCalcPointDoseList();
        }
        if (typeMode == ZZ ){                                                          //只对pdd规划的.

            for (auto it = pointDoseList.begin(); it != pointDoseList.end(); ++it){
                zPoint.push_back((*it)->m_fZ);
            }
            std::sort(zPoint.begin(),zPoint.end());
            minDepth = zPoint[0];
            maxDepth = zPoint[zPoint.size()-1];
        }
        vMin.push_back(minDepth);
        vMax.push_back(maxDepth);
    }
    std::sort(vMin.begin(),vMin.end());
    std::sort(vMax.begin(),vMax.end());
    if (vMin.size() > 0){
        min = vMin[0];
        max = vMax[vMax.size()-1];
    }

    return true;
}

//norm depth与曲线的depth没有关系，只适用于曲线为PDD的情况，其他曲线没有意义。
int TpsBLController::NormMeasureDataInfo(ms::proto::RT_MS_ImportFromFileList* measureUIDList,std::list<CertifiedImportFileInfo>& importFromDataList){

    measureUIDList;
    importFromDataList;
    //int depth = 0;
    //std::string uID;
    //bool isNormalDepth;
    //std::vector<RTFWK::RTFWKDLDOMeasuredProfile*> measuredProfileList;
    //RTFWK::RTFWKDLDOMeasuredProfile* measuredProfile = nullptr;
    //depth = measureUIDList->depth();
    //isNormalDepth = measureUIDList->isnormaldepth();
    //for (int count = 0; count < measureUIDList->importfromfilelist_size(); ++count){
    //    uID = measureUIDList->importfromfilelist(count).uid();
    //    mDataRepository->GetMachineSettingManager()->GetMeasureProfile(uID,measuredProfile);
    //    measuredProfileList.push_back(measuredProfile);
    //}
    //float minDepth,maxDepth;
    //GetMeasureDataDepthMaxAndMin_i(measuredProfileList, 1,minDepth,maxDepth);     //1表示是测量集合变化了,。

    //for (auto measureProfile = measuredProfileList.begin(); measureProfile != measuredProfileList.end(); ++measureProfile){
    //    CertifiedImportFileInfo importInfo;
    //    importInfo.uID = (*measureProfile)->GetUID();
    //    importInfo.isDisplay = ((*measureProfile)->GetViewDisplay()==1);
    //    importInfo.type = (*measureProfile)->GetCurveType();
    //    importInfo.offsetX = (*measureProfile)->GetOffsetX();
    //    importInfo.offsetY = (*measureProfile)->GetOffsetY();
    //    importInfo.depth = (*measureProfile)->GetDepth();
    //    importInfo.modulation = (*measureProfile)->GetFieldType();            //枚举
    //    importInfo.wedgeACCID = (*measureProfile)->GetAccessoryUID();
    //    importInfo.status = ((*measureProfile)->GetViewStatus()==1);

    //    std::vector<RTFWK::PointDose*> calcPointDoseList = (*measureProfile)->GetCalcPointDoseList();
    //    std::vector<RTFWK::PointDose*> measureDoseList = (*measureProfile)->GetMeaPointDoseList();
    //    std::vector<RTFWK::PointDose*> normMeasureDoseList = (*measureProfile)->GetNormMeasurePointDoseList();     //这里修改数据库中的接口。即规化测量点集。
    //    std::vector<RTFWK::PointDose*> normCalcDoseList    = (*measureProfile)->GetNormCalcPointDoseList();

    //    for (auto it = calcPointDoseList.begin(); it != calcPointDoseList.end(); ++it){              //计算点集
    //        std::tr1::shared_ptr<CoodrinationPoint> coodPoint(new CoodrinationPoint());
    //        coodPoint->x = (*it)->m_fX;
    //        coodPoint->y = (*it)->m_fY;
    //        coodPoint->z = (*it)->m_fZ;
    //        coodPoint->dose = (*it)->m_fDose;
    //        importInfo.calcpointList.push_back(coodPoint);
    //    }
    //    for (auto it = measureDoseList.begin(); it != measureDoseList.end(); ++it){             //测量点集。
    //        std::tr1::shared_ptr<CoodrinationPoint> spectrum(new CoodrinationPoint());
    //        spectrum->x = (*it)->m_fX;
    //        spectrum->y = (*it)->m_fY;
    //        spectrum->z = (*it)->m_fZ;
    //        spectrum->dose = (*it)->m_fDose;
    //        importInfo.pointList.push_back(spectrum);
    //    }

    //    //开始规划，因为是计算点集变化了。
    //    float kk = 0;
    //    int failureDepthValue;
    //    IsNormalSucess_i(importInfo.type, depth, calcPointDoseList, kk, failureDepthValue );
    //    if ((!(depth > minDepth && depth < maxDepth))||(failureDepthValue==-1)) {                           //这里有待改善下。
    //        for (auto it = normMeasureDoseList.begin(); it != normMeasureDoseList.end(); ++it){
    //            DEL_PTR(*it);
    //        }
    //        for (auto it = normCalcDoseList.begin(); it != normCalcDoseList.end(); ++it){
    //            DEL_PTR(*it);
    //        }
    //        normMeasureDoseList.clear();
    //        normCalcDoseList.clear();
    //        importInfo.normCalcPointList.clear();
    //        importInfo.normPointList.clear();

    //        (*measureProfile)->SetNormCalcPointDoseList(normCalcDoseList);
    //        (*measureProfile)->SetNormMeasurePointDoseList(normMeasureDoseList);
    //        importFromDataList.push_back(importInfo);
    //        if (!RTFWK::RTFWKDLDataAccess::GetInstance()->SaveObjectToDB(measuredProfile)) {                    //数据库中保存的数据是排序过的。
    //            TPS_LOG_DEV_ERROR<<"save measure profile to TMS failed:";
    //            return false;
    //        }
    //    }
    //    else{
    //        for (auto it = calcPointDoseList.begin(); it != calcPointDoseList.end(); ++it){
    //            std::tr1::shared_ptr<CoodrinationPoint> coodPoint(new CoodrinationPoint());
    //            coodPoint->x = (*it)->m_fX;
    //            coodPoint->y = (*it)->m_fY;
    //            coodPoint->z = (*it)->m_fZ;
    //            coodPoint->dose *= kk;
    //            importInfo.normCalcPointList.push_back(coodPoint);
    //            RTFWK::PointDose* pPointDose = new RTFWK::PointDose();
    //            pPointDose->m_fX = (*it)->m_fX;
    //            pPointDose->m_fY = (*it)->m_fY;
    //            pPointDose->m_fZ = (*it)->m_fZ;
    //            pPointDose->m_fDose *= kk;
    //            normCalcDoseList.push_back(pPointDose);
    //        }
    //        (*measureProfile)->SetNormCalcPointDoseList(normCalcDoseList);

    //        for (auto it = measureDoseList.begin(); it != measureDoseList.end(); ++it){             //测量规化点集。
    //            std::tr1::shared_ptr<CoodrinationPoint> spectrum(new CoodrinationPoint());
    //            spectrum->x = (*it)->m_fX;
    //            spectrum->y = (*it)->m_fY;
    //            spectrum->z = (*it)->m_fZ;
    //            spectrum->dose *= kk;
    //            importInfo.normPointList.push_back(spectrum);
    //            RTFWK::PointDose* pPointDose = new RTFWK::PointDose();

    //            pPointDose->m_fX = (*it)->m_fX;
    //            pPointDose->m_fY = (*it)->m_fY;
    //            pPointDose->m_fZ = (*it)->m_fZ;
    //            pPointDose->m_fDose *= kk;
    //            normMeasureDoseList.push_back(pPointDose);
    //        }
    //        (*measureProfile)->SetNormMeasurePointDoseList(normMeasureDoseList);
    //        importFromDataList.push_back(importInfo);
    //        if (!RTFWK::RTFWKDLDataAccess::GetInstance()->SaveObjectToDB(measuredProfile)) {                    //数据库中保存的数据是排序过的。
    //            TPS_LOG_DEV_ERROR<<"save measure profile to TMS failed:";
    //            return false;
    //        }
    //    }

    //}
    return false;
}


int TpsBLController::CurveErrorAnalyseFunction(ms::proto::RT_MS_ImportFromFileList* measureUIDList, std::vector<shared_ptr<RtMeasuredprofile>>& vMeasuredprofileList){

    std::string uID;
    std::string commissionUID = measureUIDList->commissionuid();
    std::vector<RtMeasuredprofile*> measuredProfileList;
    TpsMachineSettingManager * manager = mDataRepository->GetMachineSettingManager();
    RtMeasuredprofile measuredProfile;
    for (int count = 0; count < measureUIDList->importfromfilelist_size(); ++count){
        uID = measureUIDList->importfromfilelist(count).uid();
        if (manager->GetTable<RtMeasuredprofileTable>()->Get<Tag_UniQueKey>(boost::make_tuple(uID), measuredProfile) > 0)
        {
            measuredProfileList.push_back(&measuredProfile);
        }
    }

    //std::string filePath = measuredProfile->get_curveresultanalysefilepath();

    std::vector<std::string> strCurveResultList;
    for (int i = 0; i < 5; i++){
        std::string strCurveResult = "";
        if (i == 0){
            strCurveResult = strCurveResult + "Penumbra";
        }
        if (i==1){ 
            strCurveResult = strCurveResult+ "Out of field";
        }
        if (i == 2){
            strCurveResult = strCurveResult + "In field";
        }
        if (i == 3){
            strCurveResult = strCurveResult + "Build up";
        }
        if (i == 4){
            strCurveResult = strCurveResult + "Fall off";
        }
        strCurveResult = strCurveResult + ",0,0,0,0";
        strCurveResultList.push_back(strCurveResult);
    }
    measuredProfile.set_curveresultanalyselist(boost::join(strCurveResultList, "|"));

    RTALG::AlgCommission algCommission;
    algCommission.SetCommissionedUnitUID(commissionUID);
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOMeasuredProfile>> algMeasuredProfileList;
    MeasureProfileConverter::ConvertToALGList(measuredProfileList, algMeasuredProfileList);
    algCommission.SetMeasureProfile(algMeasuredProfileList[0].get());
    algCommission.DoCurveResultAnalyse();

    std::vector<RTFWK::RTFWKDLDOMeasuredProfile*> aLGmeasuredProfileList;
    aLGmeasuredProfileList.push_back(algCommission.GetMeasureProfile());

    MeasureProfileConverter::ConverBackFromALGList(aLGmeasuredProfileList, vMeasuredprofileList);

    if (mDatabaseWrapper->UpdateMeasuredprofile(*(vMeasuredprofileList[0]))) {
        TPS_LOG_DEV_ERROR<<"save measure profile to TMS failed:";
        return false;
    }

    if (!manager->GetTable<RtMeasuredprofileTable>()->Update<Tag_UniQueKey>(boost::make_tuple(vMeasuredprofileList[0]->get_uid()), *(vMeasuredprofileList[0]))) {
        TPS_LOG_DEV_ERROR << "save measure profile to TMS failed:";
        return false;
    }
    return true;
}


//Filp操作只对测量数据集操作。
int TpsBLController::CommonOperationCurveFunction(ms::proto::RT_MS_ImportFromFileList* measureUIDList, std::vector<std::shared_ptr<RtMeasuredprofile>>& importFromDataList){

    std::string uID;
    int  algType;
    std::vector<RtMeasuredprofile*> tempMeasuredProfileList;
    RtMeasuredprofile measuredProfile;
    std::string commissionUID = measureUIDList->commissionuid();
    std::string operatorType = measureUIDList->objectoperationtype();

    for (int count = 0; count < measureUIDList->importfromfilelist_size(); ++count){
        uID = measureUIDList->importfromfilelist(count).uid();
        algType = measureUIDList->importfromfilelist(count).algtype();
        if (mDataRepository->GetMachineSettingManager()->GetTable<RtMeasuredprofileTable>()->Get<Tag_UniQueKey>(boost::make_tuple(uID), measuredProfile) > 0)
        {
            tempMeasuredProfileList.push_back(&measuredProfile);
        }
    }

    std::string measureSureFile = measuredProfile.get_meapointdoselist();
    std::string calFile = measuredProfile.get_calcpointdoselist();
    std::string curveFile = measuredProfile.get_curveresultanalyselist();

    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOMeasuredProfile>> algMeasuredProfileList;
    MeasureProfileConverter::ConvertToALGList(tempMeasuredProfileList, algMeasuredProfileList);

    RTALG::AlgCommission algCommission;
    algCommission.SetCommissionedUnitUID(commissionUID);
    algCommission.SetMeasureProfile(algMeasuredProfileList[0].get());


    if (operatorType == "AutoSymmetricFunction"){
        algCommission.AutoSymmeMeasuredProfile();
    }
    if (operatorType == "AutoShfitFunction"){
        algCommission.AutoShiftMeasuredProfile();
    }
    if (operatorType == "FlipFunction"){
        algCommission.FlipMeasuredProfile();
    }
    if (operatorType == "InterpolateFunction"){
        algCommission.InterpMeasuredProfile();
    }
    if (operatorType == "SmoothFunction"){
        algCommission.SmoothMeasuredProfile();
    }

    std::vector<RTFWK::RTFWKDLDOMeasuredProfile*> aLGmeasuredProfileListV;
    aLGmeasuredProfileListV.push_back(algCommission.GetMeasureProfile());
    MeasureProfileConverter::ConverBackFromALGList(aLGmeasuredProfileListV, importFromDataList);

    //考虑更新manager的数据。
    if (mDatabaseWrapper->UpdateMeasuredprofile(*importFromDataList[0])) {
        TPS_LOG_DEV_ERROR<<"update measure profile to DB failed:";
        return false;
    }

    if (!mDataRepository->GetMachineSettingManager()->GetTable<RtMeasuredprofileTable>()->Update<Tag_UniQueKey>(boost::make_tuple(importFromDataList[0]->get_uid()), *importFromDataList[0]))
    {
        TPS_LOG_DEV_ERROR << "update measure profile to manager failed:";
        return false;
    }
    return true;

}

int TpsBLController::SortToCollectionData(int curveType,std::vector<PointDose*>& pointDoseList,std::vector<PointDose*>& sortPointDoseList){

    std::vector<float> xPoint,yPoint,zPoint;
    if (curveType == X ){
        for (auto it = pointDoseList.begin(); it != pointDoseList.end(); ++it){
            xPoint.push_back((*it)->m_fX);
        }
        std::sort(xPoint.begin(),xPoint.end());
        for (int count = 0; count < xPoint.size(); ++count ){
            sortPointDoseList[count]->m_fX = xPoint[count];
        }
    }

    else if(curveType == Y ){
        for (auto it = pointDoseList.begin(); it != pointDoseList.end(); ++it){
            yPoint.push_back((*it)->m_fY);
        }
        std::sort(yPoint.begin(),yPoint.end());
        for (int count = 0; count < yPoint.size(); ++count ){
            sortPointDoseList[count]->m_fY = yPoint[count];
        }

    }
    else if (curveType == ZZ ){

        for (auto it = pointDoseList.begin(); it != pointDoseList.end(); ++it){
            zPoint.push_back((*it)->m_fZ);
        }
        std::sort(zPoint.begin(),zPoint.end());
        for (int count = 0; count < zPoint.size(); ++count ){
            sortPointDoseList[count]->m_fZ = zPoint[count];
        }

    }
    else if (curveType == CATORCORNER){                    //这里是对角线的按照x排序的。

        for (auto it = pointDoseList.begin(); it != pointDoseList.end(); ++it){
            xPoint.push_back((*it)->m_fX);
        }
        std::sort(xPoint.begin(),xPoint.end());
        for (int count = 0; count < xPoint.size(); ++count ){
            sortPointDoseList[count]->m_fX = xPoint[count];
        }
    }

    return true;
}

int TpsBLController::OptimizeFunction(bool bCalcAbsOrRel, ms::proto::RT_MS_Proto* protoInfo, CertifiedElectronContInfo& electronContInfo){

    std::string commissionUid = protoInfo->uid();
    int algType = protoInfo->algtype();

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    if (!AlgCommissionProxyInit(commissionUid, mAlgCommissionProxy) == TPS_ER_SUCCESS)
    {
        return TPS_ER_FAILURE;
    }

    std::vector<RtMeasuredprofile> measuredProfileList;
    //set MeasureData
    manager->GetTable<RtMeasuredprofileTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUid), measuredProfileList);
    std::vector<RtMeasuredprofile*> meaList;
    for (auto it = measuredProfileList.begin(); it != measuredProfileList.end(); it++)
    {
        if ((RTFWK::CURVETYPE)it->get_curvetype() == RtDbDef::CURVETYPE_PDD && it->get_fieldtype() == RtDbDef::FIELDTYPE_OPEN)
        {
            meaList.push_back(&(*it));
        }
    }
    mAlgCommissionProxy->SetMeasureProfileList(meaList);
    mAlgCommissionProxy->CalcOptConvolutionEC((RtDbDef::DOSEALGORITHM_TYPE)algType, bCalcAbsOrRel, electronContInfo);

    RtCommissionedunit commissionUnit;
    if (manager->GetTable<RtCommissionedunitTable>()->Get<Tag_UniQueKey>(boost::make_tuple(protoInfo->uid()), commissionUnit) <= 0){
        TPS_LOG_DEV_ERROR << "Failed to get commission from manager.";
        return TPS_ER_FAILURE;
    }

    commissionUnit.set_ion(electronContInfo.electronContaminationFlag == 1);
    commissionUnit.set_dm(electronContInfo.electronMaxDepth);
    commissionUnit.set_ffs10(electronContInfo.surfaceDose);
    commissionUnit.set_k(electronContInfo.depthCoefficientK);
    commissionUnit.set_a(electronContInfo.offaxisCoefficientA);
    commissionUnit.set_df(electronContInfo.df);
    commissionUnit.set_sf(electronContInfo.sf);
    commissionUnit.set_c1(electronContInfo.cOne);
    commissionUnit.set_c2(electronContInfo.cTwo);
    commissionUnit.set_c3(electronContInfo.cThree);

    if (mDatabaseWrapper->UpdateCommissionedunit(commissionUnit)) {
        TPS_LOG_DEV_ERROR << "save commission unit to DB failed:";
        return TPS_ER_FAILURE;
    }

    if (!manager->GetTable<RtCommissionedunitTable>()->Update<Tag_UniQueKey>(boost::make_tuple(protoInfo->uid()), commissionUnit)) {
        TPS_LOG_DEV_ERROR << "save commission unit to DB failed:";
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int TpsBLController::ScpInfoFunction(const std::string& commissionUID,const RtMeasureddata& measureData){

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();

    RtMeasureddata measureDataTemp;

    if (manager->GetTable<RtMeasureddataTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), measureDataTemp) <= 0)
    {
        TPS_LOG_DEV_ERROR << "Fail to get RtMeasureData in manager";
        return TPS_ER_FAILURE;
    }

    measureDataTemp.set_outputinwaterlist(measureData.get_outputinwaterlist());
    measureDataTemp.set_scpdepth(measureData.get_scpdepth());

    if (mDatabaseWrapper->UpdateMeasureddata(measureDataTemp)) {
        TPS_LOG_DEV_ERROR<<"update measure data to DB failed:";
        return false;
    }

    if (!manager->GetTable<RtMeasureddataTable>()->Update<Tag_UniQueKey>(boost::make_tuple(measureDataTemp.get_uid()), measureDataTemp))
    {
        TPS_LOG_DEV_ERROR << "Fail to update RtMeasureData in manager";
        return TPS_ER_FAILURE;
    }

    return true;

}

int TpsBLController::UpdateWedgeFactorInfo(RtWedgefactor& wedgefactor, std::string& wedgeFactorUID,const float& wedgefactorDepth){

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    if (wedgefactor.get_uid() == ""){
        wedgefactor.set_uid(RtUidGenerater::GeneraterUid());
        if (mDatabaseWrapper->InsertWedgefactor(wedgefactor)) {
            TPS_LOG_DEV_ERROR << "wedge factor to DB failed:";
            return TPS_ER_FAILURE;
        }

        if (!manager->GetTable<RtWedgefactorTable>()->Add(wedgefactor))
        {
            TPS_LOG_DEV_ERROR << "wedge factor to manager failed:";
            return TPS_ER_FAILURE;
        }

        wedgeFactorUID = wedgefactor.get_uid();
    }
    else{
        if (mDatabaseWrapper->UpdateWedgefactor(wedgefactor)) {
            TPS_LOG_DEV_ERROR << "update wedge factor to DB failed!";
            return TPS_ER_FAILURE;
        }

        if (!manager->GetTable<RtWedgefactorTable>()->Update<Tag_UniQueKey>(boost::make_tuple(wedgefactor.get_uid()), wedgefactor))
        {
            TPS_LOG_DEV_ERROR << "wedge factor to manager failed!";
            return TPS_ER_FAILURE;
        }
        wedgeFactorUID = "";
    }

    RtMeasureddata measuredata;
    std::string commissionUid=wedgefactor.get_commissionedunituid();
    if (manager->GetTable<RtMeasureddataTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUid), measuredata) <= 0)
    {
        TPS_LOG_DEV_ERROR << "fail to get measuredata from manager!";
        return TPS_ER_FAILURE;
}

    measuredata.set_wedgefactordepth(wedgefactorDepth);

    if (mDatabaseWrapper->UpdateMeasureddata(measuredata)) {
        TPS_LOG_DEV_ERROR << "update measuredata to DB failed!";
        return TPS_ER_FAILURE;
    }

    if (!manager->GetTable<RtMeasureddataTable>()->Update<Tag_UniQueKey>(boost::make_tuple(measuredata.get_uid()), measuredata))
    {
        TPS_LOG_DEV_ERROR << "update measuredata to manager failed!";
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

//bool TpsBLController::updateWedgeFactorFile( RtWedgefactor* wedgeFactor, RtWedgefactor& retWedgeFactor){
//
//   // std::string filePath = wedgeFactor->get_wedgefactorlistfilepath();
//    std::string str = "";
//    int iWedgeFactors = static_cast<int>(wedgeFactor->GetWedgeFactorList().size());
//
//    for (int i=0;i<iWedgeFactors;i++){
//        if (str !="")
//            str += "|";
//        str = str + wedgeFactor->GetWedgeFactorList()[i]->m_sAccessoryUID+",";
//        str = str + boost::lexical_cast<std::string>(wedgeFactor->GetWedgeFactorList()[i]->m_fFieldSizeX)+",";
//        str = str + boost::lexical_cast<std::string>(wedgeFactor->GetWedgeFactorList()[i]->m_fFieldSizeY)+",";
//        str = str + boost::lexical_cast<std::string>(wedgeFactor->GetWedgeFactorList()[i]->m_fValue);
//    }
//
//   // WriteFile_i(filePath,str);
//    wedgeFactor->set_wedgefactorlistfilepath(str);
//    retWedgeFactor = *wedgeFactor;
//    return true;
//}

//bool TpsBLController::AddWedgeFactorFile( RtWedgefactor* wedgeFactor, RtWedgefactor& retWedgeFactor){  //是生成目录就可以了。
//
//    //std::string sPath = mDatabaseWrapper->GetSavePath() + "/WedgeFactorFile/";
//    //if (!mDatabaseWrapper->CreatePath(sPath)){
//    //    return false;
//    //}
//    //if (!boost::filesystem::exists(sPath)){
//    //    boost::filesystem::create_directory(sPath);
//    //}
//    //std::string strFilePath = sPath + wedgeFactor->get_uid();
//
//    //wedgeFactor->set_wedgefactorlistfilepath(strFilePath);
//
//    std::string str = "";
//    int iWedgeFactors = static_cast<int>(wedgeFactor->GetWedgeFactorList().size());
//
//    for (int i=0;i<iWedgeFactors;i++){
//        if (str !="")
//            str += "|";
//        str = str + wedgeFactor->GetWedgeFactorList()[i]->m_sAccessoryUID+",";
//        str = str + boost::lexical_cast<std::string>(wedgeFactor->GetWedgeFactorList()[i]->m_fFieldSizeX)+",";
//        str = str + boost::lexical_cast<std::string>(wedgeFactor->GetWedgeFactorList()[i]->m_fFieldSizeY)+",";
//        str = str + boost::lexical_cast<std::string>(wedgeFactor->GetWedgeFactorList()[i]->m_fValue);
//    }
//
//    //WriteFile_i(sPath,str);
//    wedgeFactor->set_wedgefactorlistfilepath(str);
//    retWedgeFactor = *wedgeFactor;
//    return true;
    //}


TPS_END_NAMESPACE
