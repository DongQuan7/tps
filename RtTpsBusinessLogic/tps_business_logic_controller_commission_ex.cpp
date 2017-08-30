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
#include "RtTpsAlgorithmProxy/tps_ap_utility.h"
#include "RtTpsDataAccess/tps_da_machinetables.h"

#define MAX_SAR_DEPTHS		(40)
#define MAX_SAR_RADII		(30)
#define MAX_FLATNESS_DEPTHS	(5)
#define MAX_FLATNESS_RADII	(40)
#define LOAD_FROM_DB_CONVERTER (10)



TPS_BEGIN_NAMESPACE



    int TpsBLController::AddTray( std::string machineUID,ms::proto::RT_MS_MachineTray* protoMachineTray,std::string& trayUID){

        RtTray trayInfo(true);
        trayInfo.set_machineuid(machineUID);
        trayInfo.set_trayid(protoMachineTray->trayid());
        trayInfo.set_code(protoMachineTray->traycode());

        trayUID = trayInfo.get_uid();

        if (mDatabaseWrapper->InsertTray(trayInfo)) {
            TPS_LOG_DEV_ERROR<<"save tray info to DB failed:";
            return false;
        }

        TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
        if (!manager->GetTable<RtTrayTable>()->Add(trayInfo)){
            TPS_LOG_DEV_ERROR<<"add tray to manager is failure:";
            return false;
        }

        //这是要考虑中间表的建立。
        std::vector<RtCommissionedunit> vRtCommissionedunitList;
        manager->GetTable<RtCommissionedunitTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(machineUID), vRtCommissionedunitList);

        if (vRtCommissionedunitList.size() > 0){
            for (auto it  = vRtCommissionedunitList.begin(); it != vRtCommissionedunitList.end(); ++it){

                RtCommissionedunit_tray  commTray;
                commTray.set_commissionedunituid(it->get_uid());
                commTray.set_trayuid(trayUID);
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

        return true;
}


int TpsBLController::AddaCurveProfile(ms::proto::RT_MS_ImportFromFileInfo* protoImportData,std::string& measureDataUID){

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    RtMeasuredprofile measureProfile(true);
    measureDataUID = measureProfile.get_uid();

    measureProfile.set_commissionedunituid(protoImportData->parentuid());

    //for (auto itt = vAccessoryUID.begin(); itt != vAccessoryUID.end(); ++itt){    //判断当前wedge的角度和文件中的角度是否匹配。如果不匹配就不处理。
    //acsFile.FromStringExtractData(*itt,angle);
    //if (angle == it->mWedge){

    measureProfile.set_wedgeangle(protoImportData->degree());

    measureProfile.set_viewdisplay(protoImportData->isdisplay());
    measureProfile.set_depth(protoImportData->depth()*10);
    measureProfile.set_viewstatus(protoImportData->status());
    measureProfile.set_offsetx(protoImportData->offsetx()*10);
    measureProfile.set_offsety(protoImportData->offsety()*10);
    measureProfile.set_fieldtype(protoImportData->type());
    measureProfile.set_curvetype(protoImportData->modulation());
    std::string wedgeaccName = protoImportData->wedgeaccid();

    if (wedgeaccName == ""){
        measureProfile.set_accessoryuid(wedgeaccName);

    }
    else{
        RtCommissionedunit commissionUnit;
        if (manager->GetTable<RtCommissionedunitTable>()->Get<Tag_UniQueKey>(boost::make_tuple(protoImportData->parentuid()), commissionUnit) <= 0){
            TPS_LOG_DEV_ERROR << "get commissionUnit from manager by uid is failure!";
            return TPS_ER_FAILURE;
        }

        RtAccessory accessory;
        if (manager->GetTable<RtAccessoryTable>()->Get<Tag_NonUniqueKey1>(boost::make_tuple(commissionUnit.get_machineuid(), wedgeaccName), accessory) <= 0){
            TPS_LOG_DEV_ERROR << "get accessoryUID from manager by accessoryName is failure:";
            return TPS_ER_FAILURE;
        }

        measureProfile.set_accessoryuid(accessory.get_uid());
    }
    measureProfile.set_fieldsizex(protoImportData->fieldsizex()*10);
    measureProfile.set_fieldsizey(protoImportData->fieldsizey()*10);

    //RtMeasuredprofile retProfile(true);
    //GenerateCurveProfilePath(measureProfile);

    if (mDatabaseWrapper->InsertMeasuredprofile(measureProfile)) {
        TPS_LOG_DEV_ERROR << "save measure profile to DB failed:";
        return TPS_ER_FAILURE;
    }

    if (!manager->GetTable<RtMeasuredprofileTable>()->Add(measureProfile)){
        TPS_LOG_DEV_ERROR<<"add measure profile to manager is failure:";
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int TpsBLController::DeleteSelectedCurves(ms::proto::RT_MS_ImportFromFileList* protoImportDataList){

    RtMeasuredprofile measuredProfile;
    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    for (int count = 0; count < protoImportDataList->importfromfilelist_size(); ++count){
        std::string uID = protoImportDataList->importfromfilelist(count).uid();
        std::string commissionUID = protoImportDataList->importfromfilelist(count).parentuid();

        int bRet = mDatabaseWrapper->EraseObject(uID, RtDbDef::TYPE_MEASUREDPROFILE);

        if (bRet) {
            TPS_LOG_DEV_ERROR<<"Failed to delete selected curves from DB";
            return false;
        }

        if (!manager->GetTable<RtMeasuredprofileTable>()->Delete<Tag_UniQueKey>(boost::make_tuple(uID)))
        {
            TPS_LOG_DEV_ERROR << "Failed to delete selected curves from manage";
            return false;
        }
    }

    return true;
}

int TpsBLController::ClearMeasureData(ms::proto::RT_MS_ImportFromFileInfo* protoImportData){

    RtMeasuredprofile measuredProfile;
    std::string measureDataUID = protoImportData->uid();

    if (mDataRepository->GetMachineSettingManager()->GetTable<RtMeasuredprofileTable>()->Get<Tag_UniQueKey>(boost::make_tuple(measureDataUID), measuredProfile) > 0)
    {
        measuredProfile.set_commissionedunituid(protoImportData->parentuid());                        //这里有问题的？？？？？？因为这里现在前端传递的是commission。
        // std::vector<PointDose*> vMeaPointDoseList;                                          //这里相当于把测量数据清空了。
        //measuredProfile.SetMeaPointDoseList(vMeaPointDoseList);

        std::string pathFile = measuredProfile.get_meapointdoselist();                   //清除文件中的内容。
        std::ofstream ofs;
        ofs.open(pathFile, std::ofstream::binary | std::ofstream::trunc | std::ofstream::out);

        if (!mDataRepository->GetMachineSettingManager()->GetTable<RtMeasuredprofileTable>()->Update<Tag_UniQueKey>(boost::make_tuple(measuredProfile.get_uid()), measuredProfile)){
            TPS_LOG_DEV_ERROR << "update measure profile to manager failed:";
            return false;
        }

        if (mDatabaseWrapper->UpdateMeasuredprofile(measuredProfile)) {
            TPS_LOG_DEV_ERROR << "update measure profile to DB failed:";
            return false;
        }
        return true;
    }
    return false;
}

int TpsBLController::UpdateCurveProfile(const RtMeasuredprofile& measurePro){

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    RtMeasuredprofile measureProfile;

    if (manager->GetTable<RtMeasuredprofileTable>()->Get<Tag_UniQueKey>(boost::make_tuple(measurePro.get_uid()), measureProfile) > 0)
    {
        measureProfile.set_uid(measurePro.get_uid());
        measureProfile.set_commissionedunituid(measurePro.get_commissionedunituid());
        measureProfile.set_viewdisplay(measurePro.get_viewdisplay());                                      //以后修改。
        measureProfile.set_offsetx(measurePro.get_offsetx());
        measureProfile.set_offsety(measurePro.get_offsety());
        measureProfile.set_depth(measurePro.get_depth());
        measureProfile.set_wedgeangle(measurePro.get_wedgeangle());
        measureProfile.set_fieldtype(measurePro.get_fieldtype());
        measureProfile.set_curvetype(measurePro.get_curvetype());
        measureProfile.set_viewstatus(measurePro.get_viewstatus());
        measureProfile.set_fieldsizex(measurePro.get_fieldsizex());
        measureProfile.set_fieldsizey(measurePro.get_fieldsizey());
        measureProfile.set_meapointdoselist(measurePro.get_meapointdoselist());
        measureProfile.set_calcpointdoselist(measurePro.get_calcpointdoselist());
        measureProfile.set_curveresultanalyselist(measurePro.get_curveresultanalyselist());

        RtAccessory accessory;
        if (manager->GetTable<RtAccessoryTable>()->Get<Tag_UniQueKey>(boost::make_tuple(measurePro.get_accessoryuid()), accessory) > 0){
            measureProfile.set_accessoryuid(accessory.get_uid());
        }

        if (!manager->GetTable<RtMeasuredprofileTable>()->Update<Tag_UniQueKey>(boost::make_tuple(measureProfile.get_uid()), measureProfile)){
            TPS_LOG_DEV_ERROR << "update measure profile to manager failed:";
            return TPS_ER_FAILURE;
        }

        if (mDatabaseWrapper->UpdateMeasuredprofile(measureProfile)) {
            TPS_LOG_DEV_ERROR << "update measure profile to DB failed:";
            return TPS_ER_FAILURE;
        }
    }
    return TPS_ER_SUCCESS;
}


int TpsBLController::IsNormalSucess_i(int& type, int& depth, std::vector<PointDose*>& pointDoseList,float& kk, int& failureDepthValue ){

    float pos =0, pos2=0;
    float dose;
    int idx = -1;
    for (int i = 0; i != pointDoseList.size()-1; ++i){
        if (type == ZZ ){
            pos = pointDoseList[i]->m_fZ;
            pos2 = pointDoseList[i+1]->m_fZ;
        }
        else if (type == X ){
            pos = pointDoseList[i]->m_fX;
            pos2 = pointDoseList[i+1]->m_fX;
        }
        else if (type == Y ){
            pos = pointDoseList[i]->m_fY;
            pos2 = pointDoseList[i+1]->m_fY;
        }
        if (depth >= pos && depth <= pos2){
            idx = i;
            dose = (pos2 - depth) / (pos2 - pos) * (pointDoseList[idx]->m_fDose) +
                (depth - pos) / (pos2 - pos) * (pointDoseList[idx + 1]->m_fDose);
            kk = 1.0f / dose;
        }
    }

    failureDepthValue = idx;                    //failureDepthValue表示规化深度点不在测量点范围内。
    return true;
}

/*
enum VWInterfaceRet {
MERGE_VW_SUCCESS,
MERGE_VW_CP_FALSE,
MERGE_VW_SEG_FALSE,
FILLIN_GSTT_SUCCESS,
NO_MATCH_FILE,
NO_MATCH_MACHINE,
NO_MATCH_ENERGY,
NO_MATCH_ANGLE_AND_ORIENTATION
};
*/

int TpsBLController::ImportDataFromGSTTFile(std::string& pathFile,ms::proto::RT_MS_ImportGSTTFromFile* importGSTTInfo,CertifiedGSTTInfo& gsttInfo){

    CertifiedGSTTInfo certifiedGSTTInfo;
    certifiedGSTTInfo.Convert(*importGSTTInfo);
    std::string accessoryID = certifiedGSTTInfo.sAccessoryUID;//ID 就是Name，这个命名有问题
    std::string commissionUID = certifiedGSTTInfo.sCommissionedunitUID;
    std::string fileName = pathFile;

    //此处逻辑在原来的设计中存在设计bug，accessory name 行为上被定为唯一，但是数据库并没有加以限制，暂时不做处理
    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();

    RtCommissionedunit commissionUnit;
    if (manager->GetTable<RtCommissionedunitTable>()->Get<Tag_UniQueKey>(boost::make_tuple(commissionUID), commissionUnit) <= 0){
        TPS_LOG_DEV_ERROR << "Failed to get commissionUnit from manager.";
        return TPS_ER_FAILURE;
    }

    RtAccessory accessory;
    if (manager->GetTable<RtAccessoryTable>()->Get<Tag_NonUniqueKey1>(boost::make_tuple(commissionUnit.get_machineuid(), accessoryID), accessory) <= 0){
        TPS_LOG_DEV_ERROR << "Failed to get accessoryUID from manager.";
        return TPS_ER_FAILURE;
    }

    RTFWK::RTFWKDLDOCommissionedunit algCommissionedUnit;
    DbCommissionConvertToAlgCommission(&algCommissionedUnit, commissionUnit);

    RtMachine machine;
    if (manager->GetTable<RtMachineTable>()->Get<Tag_UniQueKey>(boost::make_tuple(commissionUnit.get_machineuid()), machine) <= 0)
    {
        TPS_LOG_DEV_ERROR << "Failed to get machine.";
        return TPS_ER_FAILURE;
    }

    RTFWK::RTFWKDLDOAccessory algAccessory;
    GetAccessoryConvertToAlgData(&accessory,&algAccessory);

    RtGoldenstt goldenSTT(true);
    bool isDataExists = false;
    if (manager->GetTable<RtGoldensttTable>()->Get<Tag_NonUniqueKey2>(boost::make_tuple(commissionUID, accessoryID), goldenSTT) <= 0)
    {
        isDataExists = false;
    }
    else
    {
        isDataExists = true;
    }

    RTFWK::RTFWKDLDOGoldenSTT algGoldenSTT(true);
    DBGoldenSttConvertToAlgType(&goldenSTT, algGoldenSTT);

    float wedgeFactor;
    int ret = RTALG::FillInGoldenSTT(algAccessory, algCommissionedUnit, machine.get_deviceid(), fileName, wedgeFactor, algGoldenSTT);

    AlgTypeConvertToDBGoldenStt(&algGoldenSTT,goldenSTT);

    std::string str = RTALG::VWStateDescription(ret);
    TPS_LOG_TRACE_INFO << str;
#ifdef _DEBUG
    printf("\nVirtual Wedge Return :  %s\n", str.c_str());
#endif

    gsttInfo.wedgefactor = wedgeFactor;
    gsttInfo.goldenUID = goldenSTT.get_uid();

    std::vector<float>accumulateMu;
    std::string strAccumulatedMU = goldenSTT.get_accumulatedmu();
    TpsUtility::StringToVector(strAccumulatedMU, accumulateMu);

    std::vector<float> movJawPos;
    std::string strMovJAWPos =  goldenSTT.get_movjawpos();
    TpsUtility::StringToVector(strMovJAWPos, movJawPos);

    float fixJawPos = goldenSTT.get_fixjawpos();



    for (int i = 0; i < accumulateMu.size(); ++i){
        std::tr1::shared_ptr<CertifiedVirtualFactor> virtualFactor(new CertifiedVirtualFactor());
        virtualFactor->collimatorX1Position = movJawPos[i];
        virtualFactor->collimatorX2Position = fixJawPos;
        virtualFactor->relativeMU = accumulateMu[i];

        gsttInfo.virtualWedgeFactorList.push_back(virtualFactor);
    }

    if (isDataExists)
    {
        if (mDatabaseWrapper->UpdateGoldenstt(goldenSTT)) {
            TPS_LOG_DEV_ERROR << "update goldenstt to DB failed!";
            return TPS_ER_FAILURE;
        }

        if (!manager->GetTable<RtGoldensttTable>()->Update<Tag_UniQueKey>(boost::make_tuple(goldenSTT.get_uid()), goldenSTT))
        {
            TPS_LOG_DEV_ERROR << "update goldenstt to manager failed!";
            return TPS_ER_FAILURE;
        }
    }
    else
    {
        if (mDatabaseWrapper->InsertGoldenstt(goldenSTT)) {
            TPS_LOG_DEV_ERROR << "add goldenstt to DB failed!";
            return TPS_ER_FAILURE;
        }

        if (manager->GetTable<RtGoldensttTable>()->Add(goldenSTT)){
            TPS_LOG_DEV_ERROR << "Failed to add golden to manager!";
            return TPS_ER_FAILURE;
        }
    }

    return TPS_ER_SUCCESS;
}

bool TpsBLController::DBGoldenSttConvertToAlgType(RtGoldenstt* dBGoldenSTT,RTFWK::RTFWKDLDOGoldenSTT& goldenSTT){

    goldenSTT.SetAccessoryUID(dBGoldenSTT->get_accessoryuid());
    goldenSTT.SetCommissionedunitUID(dBGoldenSTT->get_commissionedunituid());
    goldenSTT.SetUID(dBGoldenSTT->get_uid());

    std::vector<float>accumulateMu;
    std::string strAccumulatedMU = dBGoldenSTT->get_accumulatedmu();
    TpsUtility::StringToVector(strAccumulatedMU, accumulateMu);

    std::vector<float> movJawPos;
    std::string strMovJAWPos =  dBGoldenSTT->get_movjawpos();
    TpsUtility::StringToVector(strMovJAWPos, movJawPos);
    float fixJawPos = dBGoldenSTT->get_fixjawpos();

    std::vector<float> convFactor;
    std::string strConv = dBGoldenSTT->get_correctionfactorconvolution();
    TpsUtility::StringToVector(strConv, convFactor);

    std::vector<float> monteFactor;
    std::string strMonte = dBGoldenSTT->get_correctionfactormontecarlo();
    TpsUtility::StringToVector(strMonte, monteFactor);

    std::vector<float> pencilFactor;
    std::string strPencil = dBGoldenSTT->get_correctionfactorpencilbeam();
    TpsUtility::StringToVector(strPencil, pencilFactor);

    goldenSTT.SetAccumulatedMU(accumulateMu);
    goldenSTT.SetMovJAWPos(movJawPos);
    goldenSTT.SetFixJAWPos(fixJawPos);
    goldenSTT.SetCorrectionFactorConvolution(convFactor);
    goldenSTT.SetCorrectionFactorMonteCarlo(monteFactor);
    goldenSTT.SetCorrectionFactorPencilBeam(pencilFactor);
    goldenSTT.SetWedgeFactor(dBGoldenSTT->get_wedgeangle());
    goldenSTT.SetWedgeFactor(dBGoldenSTT->get_wedgefactor());

    return true;
}

bool TpsBLController::AlgTypeConvertToDBGoldenStt(RTFWK::RTFWKDLDOGoldenSTT* algGolden,RtGoldenstt& goldenSTT){

    goldenSTT.set_uid(algGolden->GetUID());
    goldenSTT.set_accessoryuid(algGolden->GetAccessoryUID());
    goldenSTT.set_commissionedunituid(algGolden->GetCommissionedunitUID());
    //goldenSTT.set_uid(algGolden->GetUID());

    std::vector<float>accumulateMu;
    accumulateMu= algGolden->GetAccumulatedMU();
    std::string strAccumulatedMU;
    FloatToStringFunc(accumulateMu,strAccumulatedMU);
    goldenSTT.set_accumulatedmu(strAccumulatedMU);

    std::vector<float> movJawPos;
    std::string strMovJAWPos;
    movJawPos = algGolden->GetMovJAWPos();
    FloatToStringFunc(movJawPos,strMovJAWPos);
    goldenSTT.set_movjawpos(strMovJAWPos);
    goldenSTT.set_fixjawpos(algGolden->GetFixJAWPos());
    std::vector<float> convFactor = algGolden->GetCorrectionFactorConvolution();
    std::string strConv;
    FloatToStringFunc(convFactor,strConv);
    goldenSTT.set_correctionfactorconvolution(strConv);

    std::vector<float> monteFactor= algGolden->GetCorrectionFactorMonteCarlo() ;
    std::string strMonte;
    FloatToStringFunc(monteFactor,strMonte);
    goldenSTT.set_correctionfactormontecarlo(strMonte);

    std::vector<float> pencilFactor = algGolden->GetCorrectionFactorPencilBeam();
    std::string strPencil;
    FloatToStringFunc(pencilFactor,strPencil);
    goldenSTT.set_correctionfactorpencilbeam(strPencil);
    goldenSTT.set_wedgeangle(algGolden->GetWedgeAngle());
    goldenSTT.set_wedgefactor(algGolden->GetWedgeFactor());

    float* exppara;
    std::string strexppara;
    exppara = algGolden->GetExpPara();

    //此处算法接口有问题，没有办法判断出返回数组的长度，只能硬编码
    strexppara = strexppara + boost::lexical_cast<std::string>(exppara[0]) + ",";
    strexppara = strexppara + boost::lexical_cast<std::string>(exppara[1]) + ",";
    strexppara = strexppara + boost::lexical_cast<std::string>(exppara[2]);

    goldenSTT.set_exppara(strexppara);

    return true;
}

int TpsBLController::CreateIntersectionMatrix(std::string& commissionUID){

    if (AlgCommissionProxyInit(commissionUID, mAlgCommissionProxy) == TPS_ER_SUCCESS)
    {
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

bool TpsBLController::DbMcphocommissionConvertToAlgCommission(RtMcphocommissionedunit* dbphoCommUnit,
    RTFWK::RTFWKDLDOMcPhoCommissionedUnit& phoCommUnit){

        phoCommUnit.SetCommissionedUnitUID(dbphoCommUnit->get_commissionedunituid());
        phoCommUnit.SetUID(dbphoCommUnit->get_uid());
        phoCommUnit.SetMcPhoXJawUpPosition(dbphoCommUnit->get_mcphoxjawupposition());
        phoCommUnit.SetMcPhoXJawThickness(dbphoCommUnit->get_mcphoyjawthickness());
        phoCommUnit.SetMcPhoMLCCenterPosition(dbphoCommUnit->get_mcphomlccenterposition());
        phoCommUnit.SetMcPhoMLCThickness(dbphoCommUnit->get_mcphomlcthickness());
        phoCommUnit.SetMcPhoMLLeafEndRadius(dbphoCommUnit->get_mcphomlleafendradius());
        phoCommUnit.SetMcPhoMLCAirGapWidth(dbphoCommUnit->get_mcphomlcairgapwidth());
        phoCommUnit.SetMcPhoMLCTGWidth(dbphoCommUnit->get_mcphomlctgwidth());
        phoCommUnit.SetMcPhoMuFactor(dbphoCommUnit->get_mcphomufactor());
        phoCommUnit.SetMcPhoYJawUpPosition(dbphoCommUnit->get_mcphoyjawupposition());
        phoCommUnit.SetMcPhoYJawThickness(dbphoCommUnit->get_mcphoyjawthickness());

        std::vector<RTFWK::RTFWKDLDOMcPhoMonoEnergyWeight*> algphoMonoEnergyWeightList;
        std::vector<RtMcphomonoenergyweight*> phoMonoEnergyWeightList;
        mDatabaseWrapper->GetMcphomonoenergyweightListByMcphocommissionedunituid(dbphoCommUnit->get_uid(),phoMonoEnergyWeightList);
        for (int i = 0; i < phoMonoEnergyWeightList.size(); i++){
            algphoMonoEnergyWeightList[i]->SetInjectElectronEnergy(phoMonoEnergyWeightList[i]->get_injectelectronenergy());
            algphoMonoEnergyWeightList[i]->SetInjectElectronWeight(phoMonoEnergyWeightList[i]->get_injectelectronweight());
        }
        phoCommUnit.SetMcPhoFilePath(dbphoCommUnit->get_mcpholist());
        phoCommUnit.SetMcPhoNorm10x10(dbphoCommUnit->get_mcphonorm10x10());

        std::vector<McPhoAbsCorrectionFactor*> absFactor;
        std::string correctionFactor = dbphoCommUnit->get_mcphoabscorrectionfactor();

        if (correctionFactor !=""){
            std::vector<std::string> vecValue;
            (void)boost::split(vecValue,correctionFactor, boost::is_any_of("|"));
            int ilen = static_cast<int>(vecValue.size());
            absFactor.resize(ilen);
            for (int i=0;i<ilen;i++){
                std::vector<std::string> strValue;
                absFactor[i] = new McPhoAbsCorrectionFactor();
                (void)boost::split(strValue,vecValue[i], boost::is_any_of(","));

                absFactor[i]->m_fMcPhoAbsCorrectionFieldSize =static_cast<float>(atof(strValue[0].c_str()));
                absFactor[i]->m_fMcPhoAbsCorrectionFactor =static_cast<float>(atof(strValue[1].c_str()));
                strValue.clear();
            }
            vecValue.clear();
        }

        std::vector<RTFWK::RTFWKDLDOMcPhoAbsCorrectionFactor*> algabsFactor;
        for (int i= 0; i < absFactor.size(); i++ ){
            RTFWK::RTFWKDLDOMcPhoAbsCorrectionFactor algabsFactorSingle;
            algabsFactorSingle.m_fMcPhoAbsCorrectionFactor = absFactor[i]->m_fMcPhoAbsCorrectionFactor;
            // wzd-2017-01-10 算法组修改数据结构 此处代码需要重写
            //algabsFactorSingle.m_fMcPhoAbsCorrectionFieldSize = absFactor[i]->m_fMcPhoAbsCorrectionFieldSize;
            algabsFactor.push_back(&algabsFactorSingle);
        }
        phoCommUnit.SetMcPhoAbsCorrectionFactor(algabsFactor);

        std::string engery = dbphoCommUnit->get_mcphoenergy();
        char b = ',';
        std::vector<float>num;
        StringSplit_i(engery,b,num);
        phoCommUnit.SetMcPhoEnergy(num);

        phoCommUnit.SetMcPhoOn(dbphoCommUnit->get_mcphoon() == 1);
        phoCommUnit.SetMcPhoDm(dbphoCommUnit->get_mcphodm()*10);
        phoCommUnit.SetMcPhoFfs10(dbphoCommUnit->get_mcphoffs10());
        phoCommUnit.SetMcPhoK(dbphoCommUnit->get_mcphok());
        phoCommUnit.SetMcPhoA(dbphoCommUnit->get_mcphoa());
        phoCommUnit.SetMcPhoDF(dbphoCommUnit->get_mcphodf());
        phoCommUnit.SetMcPhoSF(dbphoCommUnit->get_mcphosf());
        phoCommUnit.SetMcPhoC1(dbphoCommUnit->get_mcphoc1());
        phoCommUnit.SetMcPhoC2(dbphoCommUnit->get_mcphoc2());
        phoCommUnit.SetMcPhoC3(dbphoCommUnit->get_mcphoc3());

        //PDD
        phoCommUnit.SetPrimaryEnergystart(dbphoCommUnit->get_mcprimaryenergystart());
        phoCommUnit.SetPrimaryEnergyend(dbphoCommUnit->get_mcprimaryenergyend());
        phoCommUnit.SetPrimaryEnergybin(dbphoCommUnit->get_mcprimaryenergybin());
        phoCommUnit.SetPrimaryEnergyrestart(dbphoCommUnit->get_mcprimaryenergyrestart());
        phoCommUnit.SetSecondaryEnergystart(dbphoCommUnit->get_mcsecondaryenergystart());
        phoCommUnit.SetSecondaryEnergyend(dbphoCommUnit->get_mcsecondaryenergyend());
        phoCommUnit.SetSecondaryEnergybin(dbphoCommUnit->get_mcsecondaryenergybin());
        phoCommUnit.SetSecondaryEnergyrestart(dbphoCommUnit->get_mcsecondaryenergyrestart());
        phoCommUnit.SetElectronEngery(dbphoCommUnit->get_mcelectronenergy());
        phoCommUnit.SetCommiParticleNumber(dbphoCommUnit->get_mccommiparticlenumber());
        //completefilesnumber
        phoCommUnit.SetMcPhoParam10(dbphoCommUnit->get_mcphoparam10());
        std::string strType ;
        std::string strType1;
        strType = dbphoCommUnit->get_mcprimaryenergylist();
        strType1 = dbphoCommUnit->get_mcprimaryweightlist();
        std::vector<std::string> vecValue;
        std::vector<std::string> vecValue1;
        size_t pmciLen;
        size_t pmciLen1;
        std::vector<float> vPrimaryEnergyList;
        std::vector<float> vPrimaryWeightList;
        if (strType != "")
        {
            (void)boost::split(vecValue,strType, boost::is_any_of(","));
            (void)boost::split(vecValue1,strType1, boost::is_any_of(","));
            pmciLen = vecValue.size();
            pmciLen1 = vecValue1.size();
            if (pmciLen == pmciLen1)
            {
                vPrimaryEnergyList.resize(pmciLen);
                vPrimaryWeightList.resize(pmciLen);
                for (int i=0; i < pmciLen;i++){
                    if(vecValue[i]!="")
                    {
                        vPrimaryEnergyList[i] = boost::lexical_cast<float>(vecValue[i]);
                        vPrimaryWeightList[i] = boost::lexical_cast<float>(vecValue1[i]);
                    }
                }
                vecValue.clear();
                vecValue1.clear();
            }
            else
            {
                TPS_LOG_DEV_ERROR<<"mcprimaryenergylist and mcprimaryweightlist length are not same";
            }
        }
        phoCommUnit.SetPrimaryEnergyList(vPrimaryEnergyList);
        phoCommUnit.SetPrimaryWeightList(vPrimaryWeightList);


        strType = dbphoCommUnit->get_mcsecondaryenergylist();
        strType1 = dbphoCommUnit->get_mcsecondaryweightlist();
        std::vector<float> vSecondaryEnergyList;
        std::vector<float> vSecondaryWeightList;
        if (strType != "")
        {
            (void)boost::split(vecValue,strType, boost::is_any_of(","));
            (void)boost::split(vecValue1,strType1, boost::is_any_of(","));
            pmciLen = vecValue.size();
            pmciLen1 = vecValue1.size();
            if (pmciLen == pmciLen1)
            {
                vSecondaryEnergyList.resize(pmciLen);
                vSecondaryWeightList.resize(pmciLen);
                for (int i=0; i < pmciLen;i++){
                    if(vecValue[i]!="")
                    {
                        vSecondaryEnergyList[i] = boost::lexical_cast<float>(vecValue[i]);
                        vSecondaryWeightList[i] = boost::lexical_cast<float>(vecValue1[i]);
                    }
                }
                vecValue.clear();
                vecValue1.clear();
            }
            else
            {
                TPS_LOG_DEV_ERROR<<"mcsecondaryenergylist and mcsecondaryweightlist length are not same";
            }
        }
        phoCommUnit.SetSecondaryEnergyList(vSecondaryEnergyList);
        phoCommUnit.SetSecondaryWeightList(vSecondaryWeightList);

        strType = dbphoCommUnit->get_mcelecenergylist();
        strType1 = dbphoCommUnit->get_mcelecweightlist();
        std::vector<float> vElecEnergyList;
        std::vector<float> vElecWeightList;
        if (strType != "")
        {
            (void)boost::split(vecValue,strType, boost::is_any_of(","));
            (void)boost::split(vecValue1,strType1, boost::is_any_of(","));
            pmciLen = vecValue.size();
            pmciLen1 = vecValue1.size();
            if (pmciLen == pmciLen1)
            {
                vElecEnergyList.resize(pmciLen);
                vElecWeightList.resize(pmciLen);
                for (int i=0; i < pmciLen;i++){
                    if(vecValue[i]!="")
                    {
                        vElecEnergyList[i]= boost::lexical_cast<float>(vecValue[i]);
                        vElecWeightList[i]= boost::lexical_cast<float>(vecValue1[i]);
                    }
                }
                vecValue.clear();
                vecValue1.clear();
            }
            else
            {
                TPS_LOG_DEV_ERROR<<"mcelecenergylist and gmcelecweightlist length are not same";
            }
        }
        phoCommUnit.SetElecEnergyList(vElecEnergyList);
        phoCommUnit.SetElecWeightList(vElecWeightList);

        //in field
        phoCommUnit.SetPrimarySoftening(dbphoCommUnit->get_mcprimarysoftening());
        phoCommUnit.SetSofteningTunningF(dbphoCommUnit->get_mcsofteningtunningf());
        phoCommUnit.SetSecondarySourceType(dbphoCommUnit->get_mcsecondarysourcetype());
        phoCommUnit.SetScatteringSoftening(dbphoCommUnit->get_mcscatteringcompton());
        strType = dbphoCommUnit->get_mcsecondarysourcepara();
        std::vector<float> vFlatFilterA;
        std::vector<float> vFlatFilterB;
        (void)boost::split(vecValue,strType, boost::is_any_of(","));
        if (strType != "")
        {
            pmciLen = vecValue.size();
            if (pmciLen%2 == 0)
            {
                vFlatFilterA.resize(pmciLen/2);
                vFlatFilterB.resize(pmciLen/2);
            } 
            else
            {
                TPS_LOG_DEV_ERROR<<"Load mcsecondarysourcepara from tmsmcphocommissionedunit table failed";
            }

            //mcsecondarysourcepara奇数位给A，偶数位给B
            //例如1234这个数值，13给A,24给B
            for (int i=0; i < pmciLen;i++){
                if (i%2 == 0)
                {
                    vFlatFilterA[i/2] = boost::lexical_cast<float>(vecValue[i]);
                } 
                else
                {
                    vFlatFilterB[(i-1)/2] = boost::lexical_cast<float>(vecValue[i]);
                }
            }
            vecValue.clear();
        }
        phoCommUnit.SetFlatFilterA(vFlatFilterA);
        phoCommUnit.SetFlatFilterB(vFlatFilterB);

        //out field
        phoCommUnit.SetFlatFilterDist(dbphoCommUnit->get_mcprimaryflatfilterdist());
        phoCommUnit.SetJawDistance(dbphoCommUnit->get_mcprimaryjawdistance());
        phoCommUnit.SetSigmaP(dbphoCommUnit->get_mcsourcesigmaprimary());
        phoCommUnit.SetSigmaS(dbphoCommUnit->get_mcsourcesigmasecondary());
        phoCommUnit.SetTailCor(dbphoCommUnit->get_mclargefieldtailcor());
        phoCommUnit.SetScatterCor(dbphoCommUnit->get_mcrectanglescacor());
        phoCommUnit.SetMlcLeakage(dbphoCommUnit->get_mcmlcleakage());
        phoCommUnit.SetGaussianBlurSwitch(dbphoCommUnit->get_mcgaussianblurswitch());
        phoCommUnit.SetGaussianSigmaX(dbphoCommUnit->get_mcgaussiansigmax());
        phoCommUnit.SetGaussianSigmaY(dbphoCommUnit->get_mcgaussiansigmay());

        //electron contamination
        strType = dbphoCommUnit->get_mceleccorfieldsize();
        strType1 = dbphoCommUnit->get_mcphotonfluence();
        std::vector<float> vElecCorFieldSize;
        std::vector<float> vElecCorFactor;
        (void)boost::split(vecValue,strType, boost::is_any_of(","));
        (void)boost::split(vecValue1,strType1, boost::is_any_of(","));
        if (strType != "")
        {
            pmciLen = vecValue.size();
            pmciLen1 = vecValue1.size();
            if (pmciLen == pmciLen1)
            {
                vElecCorFieldSize.resize(pmciLen);
                vElecCorFactor.resize(pmciLen);
                for (int i=0; i < pmciLen;i++){
                    if(vecValue[i]!="")
                    {
                        vElecCorFieldSize[i] = boost::lexical_cast<float>(vecValue[i]);
                        vElecCorFactor[i] = boost::lexical_cast<float>(vecValue1[i]);
                    }
                }
                vecValue.clear();
                vecValue1.clear();
            }
            else
            {
                TPS_LOG_DEV_ERROR<<"mceleccorfieldsize and mceleccorfactor length are not same";
            }
        }
        phoCommUnit.SetElecCorFieldSize(vElecCorFieldSize);
        phoCommUnit.SetElecCorFactor(vElecCorFactor);

        //EasyOffAxisVec
        std::vector<RTFWK::RTFWKDLDOEasyOffAxis*> algEasyOffAxisVec;
        std::vector<std::string> tpsEasyOffAxisVec;
        strType = dbphoCommUnit->get_phoflulist();
        if (strType.find(",") != -1)
        {
            //同一个前半段放m_fRadius,同一个后半段放m_fValue
            //这两个字段间用，分隔
            //不同的用'|'分隔
            (void)boost::split(vecValue,strType, boost::is_any_of("|"));
            pmciLen = vecValue.size();
            algEasyOffAxisVec.resize(pmciLen);

            for (int i=0; i < pmciLen;i++){
                (void)boost::split(tpsEasyOffAxisVec,vecValue[i], boost::is_any_of(","));
                algEasyOffAxisVec[i] = new RTFWK::RTFWKDLDOEasyOffAxis();
                algEasyOffAxisVec[i]->m_fRadius = boost::lexical_cast<float>(tpsEasyOffAxisVec[0]);
                algEasyOffAxisVec[i]->m_fValue = boost::lexical_cast<float>(tpsEasyOffAxisVec[1]);
            }
            vecValue.clear();
        }
        phoCommUnit.SetEasyOffAxisVec(algEasyOffAxisVec);

        return true;
}

int TpsBLController::InitilizePhoEngergyWeight(std::string& commissionUID, float& minEnergy, float& maxEnergy, std::vector<CertifiedEnergyWeight>&vEnergyWeight){


    if (!AlgCommissionProxyInit(commissionUID, mAlgCommissionProxy) == TPS_ER_SUCCESS)
    {

        return TPS_ER_FAILURE;
    }


    std::vector<RtMcphomonoenergyweight*> pMcPhoMonoEnergyWeight;
    for (auto it = vEnergyWeight.begin(); it != vEnergyWeight.end(); ++it){
        RtMcphomonoenergyweight energyWeight;
        energyWeight.set_injectelectronenergy((*it).energy);
        energyWeight.set_injectelectronweight((*it).weight);
        pMcPhoMonoEnergyWeight.push_back(&energyWeight);
    }

    std::vector<RTFWK::RTFWKDLDOMcPhoMonoEnergyWeight*> algphoMonoEnergyWeightList;
    for (int i = 0; i < pMcPhoMonoEnergyWeight.size(); i++){
        algphoMonoEnergyWeightList[i]->SetInjectElectronEnergy(pMcPhoMonoEnergyWeight[i]->get_injectelectronenergy());
        algphoMonoEnergyWeightList[i]->SetInjectElectronWeight(pMcPhoMonoEnergyWeight[i]->get_injectelectronweight());
    }

    std::vector<RtMonocompressedphasespacedata*> vMonoCompressedPhasespacedataList;
    std::vector<RTFWK::RTFWKDLDOMonoCompressedPhasespacedata*> vAlgMonoCompressedPhasespacedataList;

    if (mDatabaseWrapper->GetAllMonocompressedphasespacedataList(vMonoCompressedPhasespacedataList)) {
        TPS_LOG_DEV_ERROR << " get compressed phase spaced data to DB failed:";
        return TPS_ER_FAILURE;
    }

    for (int j = 0; j < vMonoCompressedPhasespacedataList.size(); j++){

        vAlgMonoCompressedPhasespacedataList[j]->SetUID(vMonoCompressedPhasespacedataList[j]->get_uid());
        vAlgMonoCompressedPhasespacedataList[j]->SetFilePath(vMonoCompressedPhasespacedataList[j]->get_filepath());
        vAlgMonoCompressedPhasespacedataList[j]->SetInjectElectronEnergy(vMonoCompressedPhasespacedataList[j]->get_injectelectronenergy());
        vAlgMonoCompressedPhasespacedataList[j]->SetPhspDistanceFromTarget(vMonoCompressedPhasespacedataList[j]->get_phspdistancefromtarget());
        vAlgMonoCompressedPhasespacedataList[j]->SetPhspFilePath(vMonoCompressedPhasespacedataList[j]->get_phspfilepath());
    }

    //ALG 于20160812删除该接口
    // mAlgCommission->McPhoInitMonoEnergyWeight(minEnergy,maxEnergy,algphoMonoEnergyWeightList);
    minEnergy = 0;
    maxEnergy = 0;

    return TPS_ER_SUCCESS;
}

bool TpsBLController::AutoCalPBSCorrection(std::string commissionUID)
{

    if (!AlgCommissionProxyInit(commissionUID, mAlgCommissionProxy) == TPS_ER_SUCCESS)
    {
        return false;
    }
    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    std::vector<RtMeasureddata> measureDataList;
    //set MeasureData
    if (manager->GetTable<RtMeasureddataTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), measureDataList) <= 0){
        TPS_LOG_DEV_ERROR << "get measure data  to DB failed:";
        return false;
    }
    if (measureDataList.size() != 1)//表设计缺陷，是一一对应关系
    {
        TPS_LOG_DEV_ERROR << "get measure data  to DB failed:";
        return false;
    }

    RtCommissionedunit comUnit;
    if (manager->GetTable<RtCommissionedunitTable>()->Get<Tag_UniQueKey>(boost::make_tuple(commissionUID), comUnit) <= 0){
        TPS_LOG_DEV_ERROR << "Failed to get commission to manager.";
        return false;
    }
    mAlgCommissionProxy->CalcPBScCorrection(&measureDataList[0], comUnit);

    if (mDatabaseWrapper->UpdateCommissionedunit(comUnit)) {
        TPS_LOG_DEV_ERROR << "update commissionUnit failed in db wrapper!";
        return false;
    }

    if (!manager->GetTable<RtCommissionedunitTable>()->Update<Tag_UniQueKey>(boost::make_tuple(comUnit.get_uid()), comUnit)){
        TPS_LOG_DEV_ERROR << "update commissionUnit failed in  manager!";
        return false;
    }
    return true;
}

int TpsBLController::DoFluenceCorrection(std::string& commissionUID, std::string& measureDataUID, float& depth){

    depth = 0;
    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();

    if (!AlgCommissionProxyInit(commissionUID, mAlgCommissionProxy) == TPS_ER_SUCCESS)
    {
        return TPS_ER_FAILURE;
    }
    std::vector< RtMeasuredprofile*> profileList;
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOMeasuredProfile>> measuredProfileList;
    RtMeasuredprofile meaprof;
    if (manager->GetTable<RtMeasuredprofileTable>()->Get<Tag_UniQueKey>(boost::make_tuple(measureDataUID), meaprof) > 0){
        profileList.clear();
        profileList.push_back(&meaprof);
        mAlgCommissionProxy->SetMeasureProfileList(profileList);
    }

    //ALG 于20160812删除该接口
    //mAlgCommission->McPhoDoFluenceCorrection(depth);
    return TPS_ER_SUCCESS;
}

int TpsBLController::EnergyFlunenceCorrection(std::string commissionUID, std::string measureUID, float depth, std::vector<float>& energyList){

    depth = 0;
    energyList.push_back(1.0);
    if (!AlgCommissionProxyInit(commissionUID, mAlgCommissionProxy) == TPS_ER_SUCCESS)
    {
        return TPS_ER_FAILURE;

    }
    //RtMeasuredprofile* measuredProfile = nullptr;
    std::vector< RtMeasuredprofile*> profileList;
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOMeasuredProfile>> measuredProfileList;
    RtMeasuredprofile meaprof;
    if (mDataRepository->GetMachineSettingManager()->GetTable<RtMeasuredprofileTable>()->Get<Tag_UniQueKey>(boost::make_tuple(measureUID), meaprof) > 0){
        profileList.clear();
        profileList.push_back(&meaprof);
        mAlgCommissionProxy->SetMeasureProfileList(profileList);
    }

    //ALG 于20160812删除该接口
    //mAlgCommission->McPhoDoEneFluenceCorrection(energyList,depth);
    return TPS_ER_SUCCESS;

}

int TpsBLController::GenerateFinalFile(std::string& commissionUID){
    if (!AlgCommissionProxyInit(commissionUID, mAlgCommissionProxy) == TPS_ER_SUCCESS)
    {
        return TPS_ER_FAILURE;

    }
    //ALG 于20160812删除该接口
    //mAlgCommission->McPhoGenerateCombinedPhspFile();
    return TPS_ER_SUCCESS;
}
int TpsBLController::AddWedgeFieldInfo(std::string& accessoryUid, std::string& commissionUid, int algType){

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    RtAccessory accessory;
    if (manager->GetTable<RtAccessoryTable>()->Get<Tag_UniQueKey>(boost::make_tuple(accessoryUid), accessory) <= 0){
        TPS_LOG_DEV_ERROR << "get accessoryUID from manager by accessoryName is failure:";
        return TPS_ER_FAILURE;
    }

    RtAccessorychunk wedgeChunk(true), carrierChunk(true);
    RtContour wedgeContour(true), carrierContour(true);

    const int accuracy = 4;
    //wedge
    wedgeContour.set_accessorychunkuid(wedgeChunk.get_uid());
    wedgeContour.set_accuracy(accuracy);

    wedgeChunk.set_accessoryuid(accessoryUid);
    wedgeChunk.set_commissionedunituid(commissionUid);
    wedgeChunk.set_datatype((int)RtDbDef::PHYSICALWEDGE_WEDGE);//wedge
    wedgeChunk.set_algorithmtype(algType);

    //carrier
    carrierContour.set_accessorychunkuid(carrierChunk.get_uid());
    carrierContour.set_accuracy(accuracy);

    carrierChunk.set_accessoryuid(accessoryUid);
    carrierChunk.set_commissionedunituid(commissionUid);
    carrierChunk.set_datatype((int)RtDbDef::PHYSICALWEDGE_CARRIER);//carrier
    carrierChunk.set_algorithmtype(algType);


    if (mDatabaseWrapper->InsertAccessorychunk(wedgeChunk)) {
        TPS_LOG_DEV_ERROR << "add accessory chunck to DB failed:";
        return TPS_ER_FAILURE;
    }

    if (mDatabaseWrapper->InsertAccessorychunk(carrierChunk)) {
        TPS_LOG_DEV_ERROR << "add accessory chunck2 to DB failed:";
        return TPS_ER_FAILURE;
    }

    if (!manager->GetTable<RtAccessorychunkTable>()->Add(wedgeChunk))
    {
        TPS_LOG_DEV_ERROR << "add accessory chunck to manager failed:";
        return TPS_ER_FAILURE;
    }


    if (!manager->GetTable<RtAccessorychunkTable>()->Add(carrierChunk))
    {
        TPS_LOG_DEV_ERROR << "add accessory chunck2 to manager failed:";
        return TPS_ER_FAILURE;
    }


    if (mDatabaseWrapper->InsertContour(wedgeContour)) {
        TPS_LOG_DEV_ERROR << "add contour to DB failed:";
        return TPS_ER_FAILURE;
    }

    if (mDatabaseWrapper->InsertContour(carrierContour)) {
        TPS_LOG_DEV_ERROR << "add contour2 to DB failed:";
        return false;
    }

    if (!manager->GetTable<RtContourTable>()->Add(wedgeContour))
    {
        TPS_LOG_DEV_ERROR << "add contour to manager failed:";
        return TPS_ER_FAILURE;
    }

    if (!manager->GetTable<RtContourTable>()->Add(carrierContour))
    {
        TPS_LOG_DEV_ERROR << "add contour2 to manager failed:";
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

bool TpsBLController::GetMeasureDataMaxandMinXY(std::vector<RtMeasuredprofile*>& measureProfileList, float& minX,float& maxX,float& minY,float& maxY){

    std::vector<float> zPoint;
    std::vector<float> xPoint;
    std::vector<float> yPoint;
    std::vector<float> dosePoint;

    if (measureProfileList.size() != 0){
        for (auto measureProfile = measureProfileList.begin(); measureProfile != measureProfileList.end(); ++measureProfile){
            /*float minDepth,maxDepth;*/
            std::vector<PointDose*>meaPointDoseList;
            std::vector<PointDose*>calcPointDoseList;

            meaPointDoseList = (*measureProfile)->GetMeaPointDoseList();
            calcPointDoseList = (*measureProfile)->GetCalcPointDoseList();
            for(auto it = meaPointDoseList.begin(); it != meaPointDoseList.end(); it++){
                xPoint.push_back((*it)->m_fX);
                yPoint.push_back((*it)->m_fY);
                zPoint.push_back((*it)->m_fZ);
                dosePoint.push_back((*it)->m_fDose);
            }

            for(auto it = calcPointDoseList.begin(); it != calcPointDoseList.end(); it++){
                xPoint.push_back((*it)->m_fX);
                yPoint.push_back((*it)->m_fY);
                zPoint.push_back((*it)->m_fZ);
                dosePoint.push_back((*it)->m_fDose);
            }
        }
        std::sort(xPoint.begin(),xPoint.end());
        std::sort(yPoint.begin(),yPoint.end());
        std::sort(zPoint.begin(),zPoint.end());
        std::sort(dosePoint.begin(),dosePoint.end());

        std::vector<float>vPointX,vPointY;
        for (auto measureProfile = measureProfileList.begin(); measureProfile != measureProfileList.end(); ++measureProfile){
            switch((*measureProfile)->get_curvetype()){
            case RtDbDef::CURVETYPE_PDD:                                //z轴。
                if (zPoint.size() != 0){
                    vPointX.push_back(zPoint[0]);
                    vPointX.push_back(zPoint[zPoint.size()-1]);
                    vPointY.push_back(dosePoint[0]);
                    vPointY.push_back(dosePoint[dosePoint.size()-1]);
                    break;
                }
            case RtDbDef::CURVETYPE_CROSSPLANE:
                if (xPoint.size() != 0){
                    vPointX.push_back(xPoint[0]);
                    vPointX.push_back(xPoint[xPoint.size()-1]);
                    vPointY.push_back(dosePoint[0]);
                    vPointY.push_back(dosePoint[dosePoint.size()-1]);
                    break;
                }
            case RtDbDef::CURVETYPE_INPLANE:
                if (yPoint.size() != 0){
                    vPointX.push_back(yPoint[0]);
                    vPointX.push_back(yPoint[yPoint.size()-1]);
                    vPointY.push_back(dosePoint[0]);
                    vPointY.push_back(dosePoint[dosePoint.size()-1]);
                    break;
                }
            case RtDbDef::CURVETYPE_DIAGONAL:
                if ((xPoint.size() != 0)&&(vPointY.size() != 0)){
                    vPointX.push_back(std::sqrt(xPoint[0]*xPoint[0]+vPointY[0]*vPointY[0]));
                    vPointX.push_back(std::sqrt(xPoint[xPoint.size()-1]*xPoint[xPoint.size()-1]+vPointY[vPointY.size()-1]*vPointY[vPointY.size()-1]));
                    vPointY.push_back(dosePoint[0]);
                    vPointY.push_back(dosePoint[dosePoint.size()-1]);
                    break;
                }
            }
        }
        std::sort(vPointX.begin(),vPointX.end());
        std::sort(vPointY.begin(),vPointY.end());
        if ((vPointX.size() != 0)&&(vPointY.size() != 0)){
            minX = vPointX[0];
            maxX = vPointX[vPointX.size()-1];
            minY = vPointY[0];
            maxY = vPointY[vPointY.size()-1];
        }

    }
    return true;
}

bool TpsBLController::CopyMachineAndCommission(std::string machineUid, std::string machineName, std::string deviceId, std::string manufactory, std::string& copyMachineUid)
{

    try
    {
        if (nullptr == mDataRepository)
        {
            TPS_LOG_DEV_ERROR << "GetMachineSettingInfo failed as mDataRepository is null.";
            return false;
        }

        std::map<std::string, RtMachine> copyMachineMap;
        std::map<std::string, RtTray> copyTrayMap;
        std::map<std::string, RtApplicator> copyElectronApplicatorMap;
        std::map<std::string, RtCommissionedunit> copyCommissionUnitMap;
        std::map<std::string, RtAccessory> copyAccessoryMap;

        std::map<std::string, RtMcphocommissionedunit> copyMcphocommissionedunitMap;
        std::vector<RtCommissionedunit_tray> copyTrayFactorList;
        std::map<std::string, RtGoldenstt> copygoldsstMap;
        std::map<std::string, RtAccessorychunk>  copyaccessoryChunkMap;
        std::map<std::string, RtContour> copyContour;

        std::map<std::string, RtKernelweight>  copykernelWeightMap;
        std::map<std::string, RtMeasureddata>  copyMeasuerDataMap;
        std::map<std::string, RtWedgefactor> copyWedgeFactorMap;


        //tmsmeasuredprofile,tmscurveresulananlyse
        TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();



        //复制Machine
        RtMachine copyMachine;
        if (manager->GetTable<RtMachineTable>()->Get<Tag_UniQueKey>(boost::make_tuple(machineUid), copyMachine) <= 0)
        {
            TPS_LOG_DEV_ERROR << "Failed to get machine.";
            return TPS_ER_FAILURE;
        }

        std::string machine_Uid = copyMachine.get_uid();
        copyMachine.set_uid(RtUidGenerater::GeneraterUid());

        //copymachine 的特殊处理
        std::vector<RtMachine> vmachineList;
        manager->GetTable<RtMachineTable>()->GetAll(vmachineList);
        copyMachine.set_name(machineName);
        copyMachine.set_deviceid(deviceId);
		copyMachine.set_manufacturer(manufactory);
        copyMachine.set_state(1);

        copyMachineMap[machine_Uid] = copyMachine;
        copyMachineUid = copyMachine.get_uid();

        std::vector<RtTray> vTrayList;
        manager->GetTable<RtTrayTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(machine_Uid), vTrayList);
        for (auto tray = vTrayList.begin(); tray != vTrayList.end(); ++tray){
            std::string uid = tray->get_uid();
            tray->set_uid(RtUidGenerater::GeneraterUid());
            tray->set_machineuid(copyMachine.get_uid());
            copyTrayMap[uid] = *tray;
        }

        std::vector<RtApplicator> ElectronApplicatorList;
        manager->GetTable<RtApplicatorTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(machine_Uid), ElectronApplicatorList);
        for (auto electronApplicator = ElectronApplicatorList.begin(); electronApplicator != ElectronApplicatorList.end(); ++electronApplicator){
            std::string uid = electronApplicator->get_uid();
            electronApplicator->set_uid(RtUidGenerater::GeneraterUid());
            electronApplicator->set_machineuid(copyMachine.get_uid());
            copyElectronApplicatorMap[uid] = *electronApplicator;
        }

        std::vector<RtAccessory> accessoryList;
        manager->GetTable<RtAccessoryTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(machine_Uid), accessoryList);
        for (auto accessory = accessoryList.begin(); accessory != accessoryList.end(); ++accessory)
        {
            std::string uid = accessory->get_uid();
            accessory->set_uid(RtUidGenerater::GeneraterUid());
            accessory->set_machineuid(copyMachine.get_uid());


            ////accessory name 特殊处理，不能重名，但是没有唯一性约束。
            //std::vector<RtAccessory> vAccessoryList;
            //manager->GetTable<RtAccessoryTable>()->GetAll(vAccessoryList);
            //int copycount = 1;
            //string endstring = "_" + std::to_string((long long)copycount);
            //std::string copyAccessoryName = "Copy of " + accessory->get_name();
            //for (auto it = vAccessoryList.begin(); it != vAccessoryList.end(); it++)
            //{
            //    if (it->get_name() == (copyAccessoryName + endstring))
            //    {
            //        copycount++;
            //        endstring = "_" + std::to_string((long long)copycount);
            //    }
            //}
            //copyAccessoryName = copyAccessoryName + endstring;
            //accessory->set_name(copyAccessoryName);

            copyAccessoryMap[uid] = *accessory;
        }

        //复制RtCommissionedunit 和 RtMcphocommissionedunit
        std::vector<RtCommissionedunit> commissionUnitList;
        manager->GetTable<RtCommissionedunitTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(machine_Uid), commissionUnitList);
        for (auto commissionUnit = commissionUnitList.begin(); commissionUnit != commissionUnitList.end(); commissionUnit++)
        {
            std::string comUid = commissionUnit->get_uid();
            commissionUnit->set_uid(RtUidGenerater::GeneraterUid());
            commissionUnit->set_machineuid(copyMachine.get_uid());
            copyCommissionUnitMap[comUid] = *commissionUnit;

            //mcphomonoenergyweight这张关联的表没有copy，目前不确定其关系。
            RtMcphocommissionedunit mcphocommissionedunit;
            if (manager->GetTable<RtMcphocommissionedunitTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUid), mcphocommissionedunit) > 0)
            {
                std::string mcpcomUid = mcphocommissionedunit.get_uid();
                mcphocommissionedunit.set_uid(RtUidGenerater::GeneraterUid());
                mcphocommissionedunit.set_commissionedunituid(commissionUnit->get_uid());
                copyMcphocommissionedunitMap[mcpcomUid] = mcphocommissionedunit;
            }

            std::vector<RtCommissionedunit_tray> unit_traylist;
            manager->GetTable<RtCommissionedunit_trayTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUid), unit_traylist);
            for (auto unit_tray = unit_traylist.begin(); unit_tray != unit_traylist.end(); ++unit_tray)
            {
                auto tray = copyTrayMap.find(unit_tray->get_trayuid());
                if (tray != copyTrayMap.end())
                {
                    unit_tray->set_trayuid(copyTrayMap[unit_tray->get_trayuid()].get_uid());
                    unit_tray->set_commissionedunituid(copyCommissionUnitMap[comUid].get_uid());
                    copyTrayFactorList.push_back(*unit_tray);
                }
            }

            std::vector<RtGoldenstt> goldsstList;
            manager->GetTable<RtGoldensttTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUid), goldsstList);
            for (auto goldsst = goldsstList.begin(); goldsst != goldsstList.end(); ++goldsst)
            {
                auto accessory = copyAccessoryMap.find(goldsst->get_accessoryuid());
                if (accessory != copyAccessoryMap.end())
                {
                    std::string uid = goldsst->get_uid();
                    goldsst->set_uid(RtUidGenerater::GeneraterUid());
                    goldsst->set_accessoryuid(copyAccessoryMap[goldsst->get_accessoryuid()].get_uid());
                    goldsst->set_commissionedunituid(copyCommissionUnitMap[comUid].get_uid());
                    copygoldsstMap[uid] = *goldsst;
                }
            }

            std::vector<RtAccessorychunk> accessoryChunkList;
            manager->GetTable<RtAccessorychunkTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUid), accessoryChunkList);
            for (auto accessorychunk = accessoryChunkList.begin(); accessorychunk != accessoryChunkList.end(); ++accessorychunk)
            {
                auto accessory = copyAccessoryMap.find(accessorychunk->get_accessoryuid());
                if (accessory != copyAccessoryMap.end())
                {
                    std::string uid = accessorychunk->get_uid();
                    accessorychunk->set_uid(RtUidGenerater::GeneraterUid());
                    accessorychunk->set_accessoryuid(copyAccessoryMap[accessorychunk->get_accessoryuid()].get_uid());
                    accessorychunk->set_commissionedunituid(copyCommissionUnitMap[comUid].get_uid());
                    copyaccessoryChunkMap[uid] = *accessorychunk;

                    RtContour contour;
                    if (manager->GetTable<RtContourTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(uid), contour) > 0)
                    {
                        std::string uid = contour.get_uid();
                        contour.set_uid(RtUidGenerater::GeneraterUid());
                        contour.set_accessorychunkuid(accessorychunk->get_uid());
                        copyContour[uid] = contour;
                    }
                }
            }


            std::vector<RtKernelweight> kernelWeightList;
            manager->GetTable<RtKernelweightTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUid), kernelWeightList);
            for (auto kernelWeight = kernelWeightList.begin(); kernelWeight != kernelWeightList.end(); ++kernelWeight)
            {
                std::string uid = kernelWeight->get_uid();
                kernelWeight->set_uid(RtUidGenerater::GeneraterUid());
                kernelWeight->set_commissionedunituid(copyCommissionUnitMap[comUid].get_uid());
                copykernelWeightMap[uid] = *kernelWeight;
            }

            std::vector<RtMeasureddata> measuredataList;
            manager->GetTable<RtMeasureddataTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUid), measuredataList);
            for (auto measuredata = measuredataList.begin(); measuredata != measuredataList.end(); ++measuredata)
            {
                std::string uid = measuredata->get_uid();
                measuredata->set_uid(RtUidGenerater::GeneraterUid());
                measuredata->set_commissionedunituid(copyCommissionUnitMap[comUid].get_uid());
                copyMeasuerDataMap[uid] = *measuredata;
            }

            std::vector<RtWedgefactor> wedgeFactorList;
            manager->GetTable<RtWedgefactorTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comUid), wedgeFactorList);
            for (auto wedgeFactor = wedgeFactorList.begin(); wedgeFactor != wedgeFactorList.end(); ++wedgeFactor)
            {
                std::string uid = wedgeFactor->get_uid();
                wedgeFactor->set_uid(RtUidGenerater::GeneraterUid());
                wedgeFactor->set_commissionedunituid(copyCommissionUnitMap[comUid].get_uid());
                copyWedgeFactorMap[uid] = *wedgeFactor;
            }
        }

        for (auto mac = copyMachineMap.begin(); mac != copyMachineMap.end(); ++mac)
        {
            std::string sMD5 = mDatabaseWrapper->CalcHash((const void*)(&(mac->second)), RtDbDef::TYPE_MACHINE);
            mac->second.set_crc(sMD5);
            mac->second.set_crc_valid(true);
            mac->second.setdirty_all_fields(true);
            if (mDatabaseWrapper->InsertMachine(mac->second) != DB_CODES_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "Machine insert db error!";
                return false;
            }

            if (!manager->GetTable<RtMachineTable>()->Add(mac->second))
            {
                TPS_LOG_DEV_ERROR << "Machine insert manager error!";
                return false;
            }
        }

        for (auto electronApplicator = copyElectronApplicatorMap.begin(); electronApplicator != copyElectronApplicatorMap.end(); ++electronApplicator)
        {
            electronApplicator->second.setdirty_all_fields(true);
            if (mDatabaseWrapper->InsertApplicator(electronApplicator->second) != DB_CODES_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "electronapplicator insert error!";
                return false;
            }
            if (!manager->GetTable<RtApplicatorTable>()->Add(electronApplicator->second))
            {
                TPS_LOG_DEV_ERROR << "electronApplicator insert manager error!";
                return false;
            }
        }

        for (auto tray = copyTrayMap.begin(); tray != copyTrayMap.end(); ++tray)
        {
            tray->second.setdirty_all_fields(true);
            if (mDatabaseWrapper->InsertTray(tray->second) != DB_CODES_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "tray insert error!";
                return false;
            }
            if (!manager->GetTable<RtTrayTable>()->Add(tray->second))
            {
                TPS_LOG_DEV_ERROR << "tray insert manager error!";
                return false;
            }
        }

        for (auto accessory = copyAccessoryMap.begin(); accessory != copyAccessoryMap.end(); ++accessory)
        {
            accessory->second.setdirty_all_fields(true);
            if (mDatabaseWrapper->InsertAccessory(accessory->second) != DB_CODES_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "accessory insert error!";
                return false;
            }
            if (!manager->GetTable<RtAccessoryTable>()->Add(accessory->second))
            {
                TPS_LOG_DEV_ERROR << "accessory insert manager error!";
                return false;
            }
        }

        for (auto commissionuint = copyCommissionUnitMap.begin(); commissionuint != copyCommissionUnitMap.end(); ++commissionuint)
        {
            commissionuint->second.setdirty_all_fields(true);
            if (mDatabaseWrapper->InsertCommissionedunit(commissionuint->second) != DB_CODES_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "commissionuint insert error!";
                return false;
            }

            if (!manager->GetTable<RtCommissionedunitTable>()->Add(commissionuint->second))
            {
                TPS_LOG_DEV_ERROR << "commissionuint insert manager error!";
                return false;
            }
        }

        for (auto mcpcommissionunit = copyMcphocommissionedunitMap.begin(); mcpcommissionunit != copyMcphocommissionedunitMap.end(); ++mcpcommissionunit)
        {
            mcpcommissionunit->second.setdirty_all_fields(true);
            if (mDatabaseWrapper->InsertMcphocommissionedunit(mcpcommissionunit->second) != DB_CODES_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "mcpcommissionunit insert error!";
                return false;
            }

            if (!manager->GetTable<RtMcphocommissionedunitTable>()->Add(mcpcommissionunit->second))
            {
                TPS_LOG_DEV_ERROR << "mcpcommissionunit insert manager error!";
                return false;
            }
        }

        for (auto wedgefactor = copyWedgeFactorMap.begin(); wedgefactor != copyWedgeFactorMap.end(); ++wedgefactor)
        {
            wedgefactor->second.setdirty_all_fields(true);
            if (mDatabaseWrapper->InsertWedgefactor(wedgefactor->second) != DB_CODES_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "wedgefactor insert error!";
                return false;
            }
            if (!manager->GetTable<RtWedgefactorTable>()->Add(wedgefactor->second))
            {
                TPS_LOG_DEV_ERROR << "wedgefactor insert manager error!";
                return false;
            }
        }

        for (auto measuredata = copyMeasuerDataMap.begin(); measuredata != copyMeasuerDataMap.end(); ++measuredata)
        {
            measuredata->second.setdirty_all_fields(true);
            if (mDatabaseWrapper->InsertMeasureddata(measuredata->second) != DB_CODES_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "measuredata insert error!";
                return false;
            }
            if (!manager->GetTable<RtMeasureddataTable>()->Add(measuredata->second))
            {
                TPS_LOG_DEV_ERROR << "measuredata insert manager error!";
                return false;
            }
        }

        for (auto kernelweight = copykernelWeightMap.begin(); kernelweight != copykernelWeightMap.end(); ++kernelweight)
        {
            kernelweight->second.setdirty_all_fields(true);
            if (mDatabaseWrapper->InsertKernelweight(kernelweight->second) != DB_CODES_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "kernelweight insert error!";
                return false;
            }
            if (!manager->GetTable<RtKernelweightTable>()->Add(kernelweight->second))
            {
                TPS_LOG_DEV_ERROR << "kernelweight insert manager error!";
                return false;
            }
        }

        //accessorychunk 
        for (auto accessorychunk = copyaccessoryChunkMap.begin(); accessorychunk != copyaccessoryChunkMap.end(); ++accessorychunk)
        {
            accessorychunk->second.setdirty_all_fields(true);
            if (mDatabaseWrapper->InsertAccessorychunk(accessorychunk->second) != DB_CODES_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "accessorychunk insert error!";
                return false;
            }

            if (!manager->GetTable<RtAccessorychunkTable>()->Add(accessorychunk->second))
            {
                TPS_LOG_DEV_ERROR << "accessorychunk insert manager error!";
                return false;
            }
        }

        //contour 
        for (auto contour = copyContour.begin(); contour != copyContour.end(); ++contour)
        {
            contour->second.setdirty_all_fields(true);
            if (mDatabaseWrapper->InsertContour(contour->second) != DB_CODES_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "contour insert error!";
                return false;
            }

            if (!manager->GetTable<RtContourTable>()->Add(contour->second))
            {
                TPS_LOG_DEV_ERROR << "contour insert manager error!";
                return false;
            }
        }


        for (auto goldsst = copygoldsstMap.begin(); goldsst != copygoldsstMap.end(); ++goldsst)
        {
            goldsst->second.setdirty_all_fields(true);
            if (mDatabaseWrapper->InsertGoldenstt(goldsst->second) != DB_CODES_SUCCESS)
            {
                TPS_LOG_DEV_ERROR << "goldsst insert error!";
                return false;
            }
            if (!manager->GetTable<RtGoldensttTable>()->Add(goldsst->second))
            {
                TPS_LOG_DEV_ERROR << "goldsst insert manager error!";
                return false;
            }
        }

        for (auto trayfactor = copyTrayFactorList.begin(); trayfactor != copyTrayFactorList.end(); ++trayfactor)
        {
            if (mDatabaseWrapper->InsertCommissionedunit_tray(*trayfactor) != DB_CODES_SUCCESS)
            {

                TPS_LOG_DEV_ERROR << "trayfactor insert error!";
                return false;
            }

            if (!manager->GetTable<RtCommissionedunit_trayTable>()->Add(*trayfactor))
            {
                TPS_LOG_DEV_ERROR << "goldsst insert manager error!";
                return false;
            }
        }
        //mDataRepository->GetMachineSettingManager()->LoadMachineSetting(copyMachineUid);
    }
    catch (std::exception& ex){
        TPS_LOG_DEV_ERROR << "TpsBLController::CopyMachineAndCommission:" << ex.what();
        return false;
    }
    return true;
}

//WJ重构20161027
bool TpsBLController::GetMachineSettingInfomation(std::vector<RtMachine>& machineInfoList)
{
    if (nullptr == mDataRepository)
    {
        TPS_LOG_DEV_ERROR << "GetMachineSettingInfo failed as mDataRepository is null.";
        return false;
    }

    if (mDataRepository->GetMachineSettingManager()->GetTable<RtMachineTable>()->GetAll(machineInfoList) < 0)
    {
        TPS_LOG_DEV_ERROR << "GetMachineSettingInfo from manager failed";
        return false;
    }
    return true;
}

bool TpsBLController::GetTrayListByMachineUid(std::string machineUid, std::vector<RtTray>& vTrayList)
{
    if (mDataRepository->GetMachineSettingManager()->GetTable<RtTrayTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(machineUid), vTrayList) < 0)
    {
        TPS_LOG_DEV_ERROR << "GetTray from manager failed";
        return false;
    }
    return true;
}


bool TpsBLController::GetAccessoryListByMachineUid(std::string machineUid, std::vector<RtAccessory>& accessoryList)
{
    if (mDataRepository->GetMachineSettingManager()->GetTable<RtAccessoryTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(machineUid), accessoryList) < 0)
    {
        TPS_LOG_DEV_ERROR << "GetAccessoryList from manager failed";
        return false;
    }
    return true;
}

bool TpsBLController::GetCommissionListByMachineUid(std::string machineUid, std::vector<RtCommissionedunit>& commissionUnitList)
{
    commissionUnitList.clear();
    if (mDataRepository->GetMachineSettingManager()->GetTable<RtCommissionedunitTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(machineUid), commissionUnitList) < 0)
    {
        TPS_LOG_DEV_ERROR << "GetCommissionUnitList from manager failed";
        return false;
    }
    return true;
}

bool TpsBLController::GetCommissionedunit_trayListByUnitUid(const std::vector<RtCommissionedunit>& commissionUnitList, std::vector<RtCommissionedunit_tray>& trayFactorList)
{
    for (auto comItem = commissionUnitList.begin(); comItem != commissionUnitList.end(); ++comItem)
    {
        if (mDataRepository->GetMachineSettingManager()->GetTable<RtCommissionedunit_trayTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(comItem->get_uid()), trayFactorList) < 0)
        {
            TPS_LOG_DEV_ERROR << "GetCommissionedunit_trayListByUnitUid from manager failed";
            return false;
        }
    }
    return true;
}

bool TpsBLController::GetMachineSettingInfoByMachineUid(const std::string& machineUid, RtMachine& machine)
{
    if (mDataRepository->GetMachineSettingManager()->GetTable<RtMachineTable>()
        ->Get<Tag_UniQueKey>(boost::make_tuple(machineUid), machine) <= 0)
    {
        return false;
    }
    return true;
}

bool TpsBLController::GetAccessoryByUid(const std::string& uid, RtAccessory& accessory){

    TpsMachineSettingManager *manager = mDataRepository->GetMachineSettingManager();
    if (manager->GetTable<RtAccessoryTable>()->Get<Tag_UniQueKey>(boost::make_tuple(uid), accessory) > 0){
        return true;
    }
    else
    {
        return false;
    }
}

bool TpsBLController::GetImportFileDataInfo(std::string& uid, std::vector<RtMeasuredprofile>& measureProfileList,
    float &depth, bool &isNormalDepth, float& minAxisX, float& maxAxisX, float& minAxisY, float& maxAxisY){

        depth;
        isNormalDepth;
        //RTFWK::RTFWKDLDOCommissionedunit* machineUnit = nullptr;
        // mDataRepository->GetMachineSettingManager()->GetUnit(uid,machineUnit);
        //depth = machineUnit->GetViewCustomizeDepth();
        //isNormalDepth = machineUnit->GetViewNormAtCaliDepth();

        mDataRepository->GetMachineSettingManager()->GetTable<RtMeasuredprofileTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(uid), measureProfileList);
        float minX = 0;
        float maxX = 0;
        float minY = 0;
        float maxY = 0;

        std::vector<RtMeasuredprofile*> meaProf;

        for (auto it = measureProfileList.begin(); it != measureProfileList.end(); it++)
        {
            meaProf.push_back(&(*it));
        }

        if (measureProfileList.size() != 0){

            GetMeasureDataMaxandMinXY(meaProf, minX, maxX, minY, maxY);
            minAxisX = minX;
            maxAxisX = maxX;
            minAxisY = minY;
            maxAxisY = maxY;
        }

        return true;
}


bool TpsBLController::GetScpDataInfo(std::string& commissionUID,RtMeasureddata& measureData){

    //注意，这里是一个commission对应一个measureData的。
    if (mDataRepository->GetMachineSettingManager()->GetTable<RtMeasureddataTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), measureData) <= 0)
    {
        TPS_LOG_DEV_ERROR << "Fail to get RtmeasureData from manager by commissionuid";
        return false;
    }

    return true;
}

bool TpsBLController::GetWedgeFactorDataInfo(std::string& commissionUID, RtWedgefactor& wedgeFactor){

    std::vector<RtWedgefactor> wedgeFactorList;                          
    mDataRepository->GetMachineSettingManager()->GetTable<RtWedgefactorTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), wedgeFactorList);   //注意，这里是一个commission对应一个wedgefactor的。

    if (wedgeFactorList.size() != 0){
        wedgeFactor = wedgeFactorList[0];
        return true;
    }
    else
    {
        return false;
    }
}

bool TpsBLController::ReadFile_i(std::string sFilePath,string& fileContent){

    char* buffer = nullptr;
    std::ifstream ifs;
    ifs.open(sFilePath, std::ios::in);
    std::size_t* size = 0;
    if (!ifs) {
        TPS_LOG_DEV_ERROR<<"Failed to open file["<<sFilePath<<"].";
        return false;
    }
    std::filebuf* pbuf = ifs.rdbuf();
    *size = pbuf->pubseekoff (0,ifs.end,ifs.in);
    if (*size > 0){
        pbuf->pubseekpos (0,ifs.in);
        buffer=new char[*size];
        pbuf->sgetn (buffer,*size);
    }
    fileContent = buffer;
    ifs.close();

    return true;
}

bool TpsBLController:: GetVirtualWedgeFactorDataInfo(std::string& commission,std::list<std::string>&virtualWedgeUID, std::vector<CertifiedGSTTInfo>& gsttInfo){

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    std::vector<RtGoldenstt> vAccessoryGoldenSTTList;
    for (auto virtualUID = virtualWedgeUID.begin(); virtualUID != virtualWedgeUID.end(); ++virtualUID){
        vAccessoryGoldenSTTList.clear();
        CertifiedGSTTInfo virturalInfo;
        RtAccessory accessory;
        if (manager->GetTable<RtAccessoryTable>()->Get<Tag_UniQueKey>(boost::make_tuple(*virtualUID), accessory) > 0){
            //根据一个accUid和commission只能找到一条GoldenSTT数据
            manager->GetTable<RtGoldensttTable>()->Get<Tag_NonUniqueKey2>(boost::make_tuple(commission, accessory.get_uid()), vAccessoryGoldenSTTList);
        }
        if (vAccessoryGoldenSTTList.size() != 0){

            virturalInfo.sAccessoryUID = accessory.get_name();
            virturalInfo.goldenUID = vAccessoryGoldenSTTList[0].get_uid();

            std::vector<std::string> vecValue;
            virturalInfo.wedgefactor = vAccessoryGoldenSTTList[0].get_wedgefactor();

            std::vector<float> vRelativeMU;
            float vCollimatorX2=vAccessoryGoldenSTTList[0].get_fixjawpos();

            std::vector<float> vCollimatorX1;
            std::string strMovJAWPos =  vAccessoryGoldenSTTList[0].get_movjawpos();
            TpsUtility::StringToVector(strMovJAWPos, vCollimatorX1);

            std::string strAccumulatedMU = vAccessoryGoldenSTTList[0].get_accumulatedmu();
            TpsUtility::StringToVector(strAccumulatedMU, vRelativeMU);

            std::vector<float> PBFactor;
            std::string strPBFactor =  vAccessoryGoldenSTTList[0].get_correctionfactorpencilbeam();
            TpsUtility::StringToVector(strPBFactor, PBFactor);

            std::vector<float> CCFactor;
            std::string strCCFactor =  vAccessoryGoldenSTTList[0].get_correctionfactorconvolution();
            TpsUtility::StringToVector(strCCFactor, CCFactor);

            std::vector<float> MCFactor;
            std::string strMCFactor =  vAccessoryGoldenSTTList[0].get_correctionfactormontecarlo();
            TpsUtility::StringToVector(strMCFactor, MCFactor);

            int count = vCollimatorX1.size();
            for (int i =0; i < count;  ++i){
                std::tr1::shared_ptr<CertifiedVirtualFactor>pVirturalField(new CertifiedVirtualFactor());
                pVirturalField->collimatorX1Position = vCollimatorX1[i];
                pVirturalField->collimatorX2Position = vCollimatorX2;
                pVirturalField->relativeMU = vRelativeMU[i];
                if (PBFactor.size() == count)
                {
                    pVirturalField->factor = PBFactor[i];
                }
                if (CCFactor.size() == count)
                {
                    pVirturalField->convolutionFactor = CCFactor[i];
                }
                if (MCFactor.size() == count)
                {
                    pVirturalField->monteCarloFactor = MCFactor[i];
                }
                virturalInfo.virtualWedgeFactorList.push_back(pVirturalField);
            }
            gsttInfo.push_back(virturalInfo);
        }
    }

    return true;
}

bool TpsBLController::GetAccessoryListByCommissionUid(std::string& commissionUID, std::vector<RtAccessory>& accessoryList)
{
    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    RtCommissionedunit comUnit;
    if (manager->GetTable<RtCommissionedunitTable>()->Get<Tag_UniQueKey>(boost::make_tuple(commissionUID), comUnit) <= 0)
    {
        TPS_LOG_DEV_ERROR << "Failed to get commission from manager.";
        return false;
    }
    GetAccessoryListByMachineUid(comUnit.get_machineuid(),accessoryList);
    return true;
}



bool TpsBLController::GetConvolutionDataInfo(std::list<std::string>&physicalWedgeNameList,std::list<std::string>&virtualWedgeNameList,std::string& commissionUID,
    CertifiedConvolution& vCertifiedConvolution)
{
        std::vector<RtKernelweight> kernelWeightList;
        TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
        manager->GetTable<RtKernelweightTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), kernelWeightList);   //一个commission对应数据库中两条记录，分别为0,40

        std::vector<int> kernelIDList;
        float fieldSizeZero,fieldSizeFourty;
        std::vector<float> kernelWeightListZeroList,kernelWeightListFourthy;

        if (kernelWeightList.size() == 2)
        {
            TpsUtility::StringToVector(kernelWeightList[0].get_kernelidlist(), kernelIDList);
            for (auto kernelWeight = kernelWeightList.begin(); kernelWeight != kernelWeightList.end(); kernelWeight++)
            {
                if (kernelWeight->get_fieldsize() == 0)
                {
                    fieldSizeZero = kernelWeight->get_fieldsize() / 10;
                    TpsUtility::StringToVector(kernelWeight->get_kernelweightlist(), kernelWeightListZeroList);
                }
                else
                {
                    fieldSizeFourty = kernelWeight->get_fieldsize() / 10;
                    TpsUtility::StringToVector(kernelWeight->get_kernelweightlist(), kernelWeightListFourthy);
                }
            }
        }
        else
        {
            TPS_LOG_DEV_ERROR << "kernelWeightList size is not 2.";
            return false;
        }

        for (int count = 0; count < kernelWeightListZeroList.size(); ++count )
        {
            std::tr1::shared_ptr<CertifiedPDDSpectrum> spectrum(new Spectrum());
            if (kernelWeightList.size() != 0){
                spectrum->uid0 = kernelWeightList[0].get_uid();
                spectrum->fieldSize = 0;
                spectrum->energy0 = kernelIDList[count];
                spectrum->kernelWeight0 =kernelWeightListZeroList[count];
            }
            vCertifiedConvolution.spectrumList.push_back(spectrum);                          //到前端基于FieldSize来区分的0或40.
        }

        for (int count = 0; count < kernelWeightListFourthy.size(); ++count )
        {
            std::tr1::shared_ptr<CertifiedPDDSpectrum> spectrum(new Spectrum());
            if (kernelWeightList.size() != 0 && kernelWeightList.size()==2)
            {
                spectrum->uid40 = kernelWeightList[1].get_uid();
                spectrum->fieldSize = 40;
                spectrum->energy40 =  kernelIDList[count];
                spectrum->kernelWeight40 = kernelWeightListFourthy[count];
            }
            vCertifiedConvolution.spectrumList.push_back(spectrum);                          //到前端基于FieldSize来区分的0或40.
        }

        RtCommissionedunit commissionUnit;
        if (manager->GetTable<RtCommissionedunitTable>()->
            Get<Tag_UniQueKey>(boost::make_tuple(commissionUID), commissionUnit) <= 0)
        {
            TPS_LOG_DEV_ERROR << "Failed to get commission from manager.";
            return false;
        }

        std::vector<float> muDataXList;
        string mudatax = commissionUnit.get_mudatax();
        StringToVectorFunc(mudatax, muDataXList);

        //int Num = muDataXList.size();
        //for (int count = 0; count < Num; ++count)
        //{
        //    std::tr1::shared_ptr<CertifiedPDDAttenuationFactors> attentionFactor(new PDDFactor());
        //    attentionFactor->fieldSize = muDataXList[count] / 10;
        //    /*if (count == 0)
        //    {
        //        attentionFactor->muBase = commissionUnit.get_mu();
        //        attentionFactor->dmudx = commissionUnit.get_dmu_dx();
        //    }
        //    else
        //    {
        //        attentionFactor->muBase = 0;
        //            attentionFactor->dmudx = 0;
        //    }*/

        //    std::vector<float> muDataY;
        //    string mudataY = commissionUnit.get_mudatay();
        //    StringToVectorFunc(mudataY, muDataY);
        //    attentionFactor->dmuBase = muDataY[count];

        //    std::vector<float> ddmudY;
        //    string dmudataY = commissionUnit.get_dmudatay();
        //    StringToVectorFunc(dmudataY, ddmudY);
        //    attentionFactor->ddmudx = ddmudY[count];

        //    std::vector<float> ddmudr;
        //    string sddmudr = commissionUnit.get_ddmudr();
        //    StringToVectorFunc(sddmudr, ddmudr);
        //    attentionFactor->ddmudr = ddmudr[count];

        //    std::vector<float> absoluteDoseCorrection;
        //    string sabsoluteDoseCorrection = commissionUnit.get_outputmuy();
        //    StringToVectorFunc(sabsoluteDoseCorrection, absoluteDoseCorrection);
        //    attentionFactor->absoluteDoseCorrectionFactor = absoluteDoseCorrection[count];
        //    vCertifiedConvolution.attenuationFactorsList.push_back(attentionFactor);
        //}

        //for (int count = 0; count < Num; ++count){
        //    std::tr1::shared_ptr<CertifiedInFieldAttenuationFactor>inFieldAttentionFactor(new InFieldFactor());

        //    std::vector<float> mudataX;
        //    string fieldSize = commissionUnit.get_mudatax();
        //    StringToVectorFunc(fieldSize, mudataX);
        //    inFieldAttentionFactor->fieldSize = mudataX[count] / 10;

        //    if (count == 0){
        //        inFieldAttentionFactor->dmudr = commissionUnit.get_dmu_dr();
        //    }
        //    else{
        //        inFieldAttentionFactor->dmudr = 0;
        //    }

        //    //inFieldAttentionFactor->ddmudr = conDataStruct->GetDMuDataY()[count];
        //    std::vector<float> vddmudr;
        //    string sddmudr = commissionUnit.get_ddmudr();
        //    StringToVectorFunc(sddmudr, vddmudr);
        //    inFieldAttentionFactor->ddmudr = vddmudr[count] / 10;

        //    vCertifiedConvolution.inFieldAttenuationFactorList.push_back(inFieldAttentionFactor);
        //}

        std::vector<float> vOffaxisDataX;
        string sOffX = commissionUnit.get_offaxisdatax();
        StringToVectorFunc(sOffX, vOffaxisDataX);

        std::vector<float> vOffaxisDataY;
        string sOffy = commissionUnit.get_offaxisdatay();
        StringToVectorFunc(sOffy, vOffaxisDataY);

        for (int count = 0; count < vOffaxisDataX.size(); ++count){

            std::tr1::shared_ptr<CertifiedInFieldOffaxisVector>offaxisVector(new InFieldVector());
            offaxisVector->radius = vOffaxisDataX[count] / 10;
            offaxisVector->offaxisRatio = vOffaxisDataY[count];
            vCertifiedConvolution.inFieldVectorList.push_back(offaxisVector);
        }

        vCertifiedConvolution.diamOne = commissionUnit.get_diam1() / 10;
        vCertifiedConvolution.diamTwo = commissionUnit.get_diam2() / 10;
        vCertifiedConvolution.srcRatio = commissionUnit.get_ratio1();
        vCertifiedConvolution.transmission = commissionUnit.get_transmission();
        vCertifiedConvolution.psd = commissionUnit.get_psd() / 10;
        vCertifiedConvolution.srcDepth = commissionUnit.get_src_d() / 10;
        vCertifiedConvolution.srcAtten = commissionUnit.get_src_atten();
        vCertifiedConvolution.sddXJaw = commissionUnit.get_convdatastruct_sdd1() / 10;
        vCertifiedConvolution.sddYJaw = commissionUnit.get_convdatastruct_sdd2() / 10;
        vCertifiedConvolution.normTenMultiTen = commissionUnit.get_norm_10x10();

        vCertifiedConvolution.electronContaminationFlag = (commissionUnit.get_ion() == 1);
        vCertifiedConvolution.electronMaxDepth = commissionUnit.get_dm() / 10;
        vCertifiedConvolution.surfaceDose = commissionUnit.get_ffs10();
        vCertifiedConvolution.depthCoefficientK = commissionUnit.get_k();
        vCertifiedConvolution.offaxisCoefficientA = commissionUnit.get_a();
        vCertifiedConvolution.df = commissionUnit.get_df();
        vCertifiedConvolution.sf = commissionUnit.get_sf();
        vCertifiedConvolution.cOne = commissionUnit.get_c1();
        vCertifiedConvolution.cTwo = commissionUnit.get_c2();
        vCertifiedConvolution.cThree = commissionUnit.get_c3();

        vCertifiedConvolution.pddMuBase = commissionUnit.get_mu();
        vCertifiedConvolution.pddDmumx = commissionUnit.get_dmu_dx();

        vCertifiedConvolution.dmudr = commissionUnit.get_dmu_dr();
        vCertifiedConvolution.mudatax = commissionUnit.get_mudatax();
        vCertifiedConvolution.ddmudr = commissionUnit.get_ddmudr();
        vCertifiedConvolution.mudatay = commissionUnit.get_mudatay();
        vCertifiedConvolution.dmudatay = commissionUnit.get_dmudatay();
        vCertifiedConvolution.outputmuy = commissionUnit.get_outputmuy();

        GetPhysicalWedgeFieldInfo_i(physicalWedgeNameList,commissionUID,vCertifiedConvolution.physicalWedgeList,2);
        GetVirtualWedgeFieldInfo_i(virtualWedgeNameList,commissionUID ,2,vCertifiedConvolution.virtualWedgeList);        //Convolution ==== 2;
        return true;
}


bool TpsBLController::GetVirtualWedgeFieldInfo_i(std::list<std::string>& virtualWedgeNameList, std::string& commissionUID,
    int algType, std::vector< std::tr1::shared_ptr<CertifiedVirtualWedgeInfo>>& virtualWedgeList){

        TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();

        RtCommissionedunit commissionUnit;
        if (manager->GetTable<RtCommissionedunitTable>()->Get<Tag_UniQueKey>(boost::make_tuple(commissionUID), commissionUnit) <= 0){
            TPS_LOG_DEV_ERROR << "get commissionUnit from manager by uid is failure!";
            return false;
        }

        std::vector<RtGoldenstt> vAccessoryGoldenSTTList;
        for (auto virtualWedgeName = virtualWedgeNameList.begin(); virtualWedgeName != virtualWedgeNameList.end(); ++virtualWedgeName){

            vAccessoryGoldenSTTList.clear();

            RtAccessory accessory;
            if (manager->GetTable<RtAccessoryTable>()->Get<Tag_NonUniqueKey1>(boost::make_tuple(commissionUnit.get_machineuid(), *virtualWedgeName), accessory) <= 0){
                //根据一个accUid和commission只能找到一条GoldenSTT数据
                continue;
            }

            manager->GetTable<RtGoldensttTable>()->Get<Tag_NonUniqueKey2>(boost::make_tuple(commissionUID, accessory.get_uid()), vAccessoryGoldenSTTList);
            if (vAccessoryGoldenSTTList.size() == 1){
                std::tr1::shared_ptr<VirtualWedgeInfo>pVirturalInfo(new VirtualWedgeInfo());
                pVirturalInfo->virtualUID = *virtualWedgeName;
                pVirturalInfo->goldenUID = vAccessoryGoldenSTTList[0].get_uid();

                std::vector<float> vCollimatorX1, vFactor, vRelativeMU;
                std::string strMovJAWPos = vAccessoryGoldenSTTList[0].get_movjawpos();
                TpsUtility::StringToVector(strMovJAWPos, vCollimatorX1);

                float vCollimatorX2 = 0.0;
                vCollimatorX2 = vAccessoryGoldenSTTList[0].get_fixjawpos();
                std::string strAccumulatedMU = vAccessoryGoldenSTTList[0].get_accumulatedmu();
                TpsUtility::StringToVector(strAccumulatedMU, vRelativeMU);

                std::string str;
                if (algType == 1){
                    str = vAccessoryGoldenSTTList[0].get_correctionfactorpencilbeam();
                    TpsUtility::StringToVector(str, vFactor);
                }
                else if (algType == 2){
                    str = vAccessoryGoldenSTTList[0].get_correctionfactorconvolution();
                    TpsUtility::StringToVector(str, vFactor);
                }
                else                                                      // algType ==3{
                    str = vAccessoryGoldenSTTList[0].get_correctionfactormontecarlo();
                TpsUtility::StringToVector(str, vFactor);

                if (vFactor.size() == vCollimatorX1.size() && vFactor.size() == vRelativeMU.size() && vFactor.size() != 0)
                {
                    for (int i = 0; i < vCollimatorX1.size(); ++i){
                        std::tr1::shared_ptr<VirtualWedgeField>pVirturalField(new VirtualWedgeField());
                        pVirturalField->uid = "";
                        pVirturalField->collimatorX1Position = vCollimatorX1[i];
                        pVirturalField->collimatorX2Position = vCollimatorX2;
                        pVirturalField->relativeMU = vRelativeMU[i];
                        pVirturalField->correctionFactor = vFactor[i];
                        pVirturalInfo->virtualWedgeList.push_back(pVirturalField);
                    }
                    virtualWedgeList.push_back(pVirturalInfo);
                }
            }
        }
        return true;
}

bool TpsBLController::GetMCVirtualWedgeFieldInfo_i(
    std::string& commissionUID,
    shared_ptr<ms::proto::RT_MS_PhotonMonteCarloInformation> protoPhotonMonteCarlo)
{
    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();

    RtCommissionedunit commissionUnit;
    if (manager->GetTable<RtCommissionedunitTable>()->
        Get<Tag_UniQueKey>(boost::make_tuple(commissionUID), commissionUnit) <= 0){
        TPS_LOG_DEV_ERROR << "get commissionUnit from manager by uid is failure!";
        return false;
    }

    std::vector<RtGoldenstt> vAccessoryGoldenSTTList;
    manager->GetTable<RtGoldensttTable>()->
        Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), vAccessoryGoldenSTTList);

    std::vector<RtGoldenstt> mcAccessoryGoldenSTTList;
    for(vector<RtGoldenstt>::iterator iter=vAccessoryGoldenSTTList.begin();iter!=vAccessoryGoldenSTTList.end();iter++)
    {
        if ((*iter).has_field(GOLDENSTT_CORRECTIONFACTORMONTECARLO) && (*iter).get_correctionfactormontecarlo() != "")
        {
            mcAccessoryGoldenSTTList.push_back(*iter);
        }
    }

    auto wedgeFieldParameter = protoPhotonMonteCarlo->mutable_wedgefieldparameter();
    ms::proto::RT_MS_PMCVirtualWedge* virtualWedge = nullptr;
    for(int i = 0; i < mcAccessoryGoldenSTTList.size(); ++i)
    {
        virtualWedge = wedgeFieldParameter->add_virtualwedgecollection();

        auto wedgeUid = mcAccessoryGoldenSTTList[i].get_accessoryuid();
        virtualWedge->set_wedgeuid(wedgeUid);
        virtualWedge->set_collimatorfixedposition(mcAccessoryGoldenSTTList[i].get_fixjawpos());

        RtAccessory acc;
        if (manager->GetTable<RtAccessoryTable>()->Get<Tag_UniQueKey>(boost::make_tuple(wedgeUid), acc) <= 0){
            TPS_LOG_DEV_ERROR << "get accessory from manager by uid failed!";
            return false;
        }
        virtualWedge->set_wedgename(acc.get_name());

        std::vector<float> vCollimatorMovingPosition, vRelativeMU , vFactor;

        std::string collimatorMovingPositionStr = mcAccessoryGoldenSTTList[i].get_movjawpos();
        StringToVectorFunc(collimatorMovingPositionStr,vCollimatorMovingPosition);
        std::string relativeMUStr = mcAccessoryGoldenSTTList[i].get_accumulatedmu();
        StringToVectorFunc(relativeMUStr,vRelativeMU);
        std::string correctionFactorMontecarloStr = mcAccessoryGoldenSTTList[i].get_correctionfactormontecarlo();
        StringToVectorFunc(correctionFactorMontecarloStr,vFactor);

        if(vCollimatorMovingPosition.size() != vRelativeMU.size() ||
            vRelativeMU.size() != vFactor.size())
        {
            TPS_LOG_DEV_ERROR << "Virtual wedge goldenstt data error!";
            return false;
        }

        ms::proto::RT_MS_PMCVirtualWedgeTable* virtualWedgeItem = nullptr;
        for(int i = 0; i < vFactor.size(); ++i)
        {
            virtualWedgeItem = virtualWedge->add_virtualwedgetableitemcollection();
            virtualWedgeItem->set_collimatormovingposition(vCollimatorMovingPosition[i]);
            virtualWedgeItem->set_relativemu(vRelativeMU[i]);
            virtualWedgeItem->set_correctionfactor(vFactor[i]);
        }
    }

    return true;
}


bool TpsBLController::GetMCPhysicalWedgeFieldInfo_i(
    std::string& commissionUID,
    std::shared_ptr<ms::proto::RT_MS_PhotonMonteCarloInformation> protoPhotonMonteCarlo)
{
    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    std::vector<RtAccessorychunk> vRtAccessorychunkList;
    if(manager->GetTable<RtAccessorychunkTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), vRtAccessorychunkList) <= 0)
    {
        printf("Do not find Accessorychunk of commission uid:",commissionUID);
    }
    std::vector<RtAccessorychunk> physicalWedgeAccessoryChunkList;
    for(vector<RtAccessorychunk>::iterator iter=vRtAccessorychunkList.begin();iter!=vRtAccessorychunkList.end();iter++)
    {
        if ((*iter).get_algorithmtype() == (int)RtDbDef::MONTECARLO)
        {
            physicalWedgeAccessoryChunkList.push_back(*iter);
        }
    }

    auto cmp = [](RtAccessorychunk chunk1, RtAccessorychunk chunk2)
    {  
        return chunk1.get_accessoryuid() < chunk2.get_accessoryuid();
    };

    auto physicalWedgeCollection = protoPhotonMonteCarlo->mutable_wedgefieldparameter();
    ms::proto::RT_MS_PMCPhysicalWedge* physicalWedge = nullptr;
    if (physicalWedgeAccessoryChunkList.size() != 0 && 
        physicalWedgeAccessoryChunkList.size() % 2 == 0)
    {
        std::sort(physicalWedgeAccessoryChunkList.begin(),
            physicalWedgeAccessoryChunkList.end(),cmp);

        for (int i = 1; i < physicalWedgeAccessoryChunkList.size(); i = i + 2)
        {
            physicalWedge = physicalWedgeCollection->add_physicalwedgecollection();

            auto wedgeUid = physicalWedgeAccessoryChunkList[i-1].get_accessoryuid();
            if (physicalWedgeAccessoryChunkList[i-1].get_accessoryuid()==
                physicalWedgeAccessoryChunkList[i].get_accessoryuid())
            {
                //这里的wedgeUid塞了accessoryuid
                physicalWedge->set_wedgeuid(wedgeUid);
            }

            RtAccessory acc;
            if (manager->GetTable<RtAccessoryTable>()->Get<Tag_UniQueKey>(boost::make_tuple(wedgeUid), acc) <= 0){
                TPS_LOG_DEV_ERROR << "get accessory from manager by uid failed!";
                return false;
            }
            physicalWedge->set_wedgename(acc.get_name());

            std::shared_ptr<RtContour> physicalWedgeContour1 = nullptr;
            std::shared_ptr<RtContour> physicalWedgeContour2 = nullptr;

            if (physicalWedgeAccessoryChunkList[i-1].get_datatype() == (int)RtDbDef::PHYSICALWEDGE_WEDGE &&
                physicalWedgeAccessoryChunkList[i].get_datatype() == (int)RtDbDef::PHYSICALWEDGE_CARRIER)
            {
                if (manager->GetTable<RtContourTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(physicalWedgeAccessoryChunkList[i-1].get_uid()),
                    physicalWedgeContour1) <= 0){
                    TPS_LOG_DEV_ERROR << "get physicalWedgeContour from manager by AccessoryChunkUid failed!";
                    return false;
                }
                if (manager->GetTable<RtContourTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(physicalWedgeAccessoryChunkList[i].get_uid()),
                    physicalWedgeContour2) <= 0){
                        TPS_LOG_DEV_ERROR << "get physicalWedgeContour from manager by AccessoryChunkUid failed!";
                        return false;
                }
            } 
            else if(physicalWedgeAccessoryChunkList[i].get_datatype() == (int)RtDbDef::PHYSICALWEDGE_WEDGE &&
                physicalWedgeAccessoryChunkList[i-1].get_datatype() == (int)RtDbDef::PHYSICALWEDGE_CARRIER)
            {
                if (manager->GetTable<RtContourTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(physicalWedgeAccessoryChunkList[i-1].get_uid()),
                    physicalWedgeContour2) <= 0){
                        TPS_LOG_DEV_ERROR << "get physicalWedgeContour from manager by AccessoryChunkUid failed!";
                        return false;
                }
                if (manager->GetTable<RtContourTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(physicalWedgeAccessoryChunkList[i].get_uid()),
                    physicalWedgeContour1) <= 0){
                        TPS_LOG_DEV_ERROR << "get physicalWedgeContour from manager by AccessoryChunkUid failed!";
                        return false;
                }
            }
            ms::proto::RT_MS_Point2D* pointSingle = nullptr;
            std::vector<db_Point3f> contourPoints1 = physicalWedgeContour1->get_contour_points();
            for(auto onePoint = contourPoints1.begin(); onePoint != contourPoints1.end(); ++onePoint)
            {
                pointSingle = physicalWedge->add_contourvertexpointcollection();
                pointSingle->set_x(onePoint->x);
                pointSingle->set_y(onePoint->y);
            }
            physicalWedge->set_wedgethickness(physicalWedgeContour1->get_thickness());
            ms::proto::RT_MS_Point2D* pointSingle2 = nullptr;
            std::vector<db_Point3f> contourPoints2 = physicalWedgeContour2->get_contour_points();
            for(auto onePoint = contourPoints2.begin(); 
                onePoint != contourPoints2.end(); ++onePoint)
            {
                pointSingle2 = physicalWedge->add_carriercontourvertexpointcollection();
                pointSingle2->set_x(onePoint->x);
                pointSingle2->set_y(onePoint->y);
            }
            physicalWedge->set_carrierthickness(physicalWedgeContour2->get_thickness());
        }
    }
    return true;
}

bool TpsBLController::GetPhysicalWedgeFieldInfo_i(std::list<std::string>& physicalWedgeNameList, std::string& commissionUID,
    std::vector< std::tr1::shared_ptr<CommonPhysicalWedgeField> >& physicalWedgeList, int algType){

        TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();

        std::vector<RtAccessorychunk> vAccessoryChunkObjectList;
        std::vector<RtAccessorychunk> vChunkObjectList;

        RtCommissionedunit commissionUnit;
        if (manager->GetTable<RtCommissionedunitTable>()->Get<Tag_UniQueKey>(boost::make_tuple(commissionUID), commissionUnit) <= 0){
            TPS_LOG_DEV_ERROR << "get commissionUnit from manager by uid is failure!";
            return false;
        }

        for (auto physicalName = physicalWedgeNameList.begin(); physicalName != physicalWedgeNameList.end(); ++physicalName)
        {
            vAccessoryChunkObjectList.clear();
            vChunkObjectList.clear();

            RtAccessory accessory;
            if (manager->GetTable<RtAccessoryTable>()->Get<Tag_NonUniqueKey1>(boost::make_tuple(commissionUnit.get_machineuid(), *physicalName), accessory) <= 0){
                TPS_LOG_DEV_ERROR << "get Accessory from manager failed!";
                return false;
            }

            manager->GetTable<RtAccessorychunkTable>()->Get<Tag_NonUniqueKey2>(boost::make_tuple(accessory.get_uid(), commissionUID), vChunkObjectList);

            std::tr1::shared_ptr<CommonPhysicalWedgeField> pPhysicalWedgeField(new PhysicalWedgeField());
            pPhysicalWedgeField->wedgeId = *physicalName;

            for (auto it = vChunkObjectList.begin(); it != vChunkObjectList.end(); it++)
            {
                if (it->get_algorithmtype() == algType)
                {
                    vAccessoryChunkObjectList.push_back(*it);
                }
            }

            if (vAccessoryChunkObjectList.size() == 2){
                RtAccessorychunk* pPhysicalWedgeFieldWedge;
                RtAccessorychunk* pPhysicalWedgeFieldCarrier;
                //dataType为1时  为wedge参数，为2时  为carrier参数
                if (vAccessoryChunkObjectList[0].get_datatype() == 1)
                {
                    pPhysicalWedgeFieldWedge = &vAccessoryChunkObjectList[0];
                    pPhysicalWedgeFieldCarrier = &vAccessoryChunkObjectList[1];
                }
                else
                {
                    pPhysicalWedgeFieldWedge = &vAccessoryChunkObjectList[1];
                    pPhysicalWedgeFieldCarrier = &vAccessoryChunkObjectList[0];
                }

                pPhysicalWedgeField->wedgeUID = pPhysicalWedgeFieldWedge->get_uid();
                pPhysicalWedgeField->mu = pPhysicalWedgeFieldWedge->get_mu();
                pPhysicalWedgeField->dmudx = pPhysicalWedgeFieldWedge->get_mu_dx();
                pPhysicalWedgeField->dmudr = pPhysicalWedgeFieldWedge->get_mu_dr();
                pPhysicalWedgeField->dmuda = pPhysicalWedgeFieldWedge->get_mu_da();
                pPhysicalWedgeField->hvlSlope = pPhysicalWedgeFieldWedge->get_hvl_slope();
                pPhysicalWedgeField->density = pPhysicalWedgeFieldWedge->get_density();


                RtContour contour;
                std::vector<db_Point3f> vPoint;
                if (manager->GetTable<RtContourTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(pPhysicalWedgeFieldWedge->get_uid()), contour) > 0)
                {
                    pPhysicalWedgeField->contourUID1 = contour.get_uid();
                    pPhysicalWedgeField->wedgeThick = contour.get_thickness();
                    vPoint = contour.get_contour_points();

                    for (int count = 0; count < vPoint.size(); ++count){
                        std::tr1::shared_ptr<CommonContourVerter>contourVerter(new ContourVerter());
                        contourVerter->contourVerterx = vPoint[count].x;
                        contourVerter->contourVertery = vPoint[count].y;

                        pPhysicalWedgeField->wedgeContourVerterList.push_back(contourVerter);
                    }
                    pPhysicalWedgeField->carrierUID = pPhysicalWedgeFieldCarrier->get_uid();
                    pPhysicalWedgeField->carriermu = pPhysicalWedgeFieldCarrier->get_mu();
                    pPhysicalWedgeField->carrierdmudx = pPhysicalWedgeFieldCarrier->get_mu_dx();
                    pPhysicalWedgeField->carrierdmudr = pPhysicalWedgeFieldCarrier->get_mu_dr();
                    pPhysicalWedgeField->carrierdmuda = pPhysicalWedgeFieldCarrier->get_mu_da();
                    pPhysicalWedgeField->carrierhvlSlope = pPhysicalWedgeFieldCarrier->get_hvl_slope();
                }
                if (manager->GetTable<RtContourTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(pPhysicalWedgeFieldCarrier->get_uid()), contour) > 0)
                {
                    pPhysicalWedgeField->contourUID2 = contour.get_uid();
                    pPhysicalWedgeField->carrierThick = contour.get_thickness();
                    vPoint = contour.get_contour_points();
                    for (int count = 0; count < vPoint.size(); ++count){
                        std::tr1::shared_ptr<CommonContourVerter>contourVerter(new ContourVerter());
                        contourVerter->contourVerterx = vPoint[count].x;
                        contourVerter->contourVertery = vPoint[count].y;

                        pPhysicalWedgeField->carrierContourVerterList.push_back(contourVerter);
                    }
                }

                physicalWedgeList.push_back(pPhysicalWedgeField);
            }
        }
        return true;
}

bool TpsBLController::GetPencilBeamDataInfo(
    std::list<std::string>&physicalWedgeUID,
    std::list<std::string>&virtualWedgeUID,
    std::string& commissionUID,
    CertifiedPencilBeam& cerPencilBeam)
{
    RtCommissionedunit commissionUnit;
    if (mDataRepository->GetMachineSettingManager()
        ->GetTable<RtCommissionedunitTable>()->Get<Tag_UniQueKey>(boost::make_tuple(commissionUID), commissionUnit) <= 0)
    {
        TPS_LOG_DEV_ERROR << "Failed to get commission from manager.";
        return false;
    }

    int depthCount = commissionUnit.get_tar0depthcount();
    depthCount;
    std::vector<float>fdepth_vec;
    std::vector<float>fTAR0;
    string strDepthVec = commissionUnit.get_tar0_depth_vec();
    StringToVectorFunc(strDepthVec, fdepth_vec);

    string strTar = commissionUnit.get_tar0();
    StringToVectorFunc(strTar, fTAR0);

    for (int count = 0; count < fdepth_vec.size(); ++count)
    {
        std::tr1::shared_ptr<Tmr> pTMRInfo(new Tmr());
        pTMRInfo->uid = "";
        pTMRInfo->depth = fdepth_vec[count] / DB_EXCHANGE_FRONT;
        pTMRInfo->value = fTAR0[count];
        cerPencilBeam.tMRInfoList.push_back(pTMRInfo);
    }

    cerPencilBeam.depthvec = commissionUnit.get_depthvec();
    cerPencilBeam.radiusvec = commissionUnit.get_radiusvec();
    cerPencilBeam.sar = commissionUnit.get_sar();

    cerPencilBeam.flatnessradiusvec = commissionUnit.get_flatnessradiusvec();
    cerPencilBeam.flatnessdepthvec = commissionUnit.get_flatnessdepthvec();
    cerPencilBeam.flatness = commissionUnit.get_flatness();

    cerPencilBeam.transmission = commissionUnit.get_tran();
    cerPencilBeam.diamOne = commissionUnit.get_srcdiam1() / DB_EXCHANGE_FRONT;
    cerPencilBeam.diamTwo = commissionUnit.get_srcdiam2() / DB_EXCHANGE_FRONT;
    cerPencilBeam.diamThree = commissionUnit.get_srcdiam3() / DB_EXCHANGE_FRONT;
    cerPencilBeam.srcRatio = commissionUnit.get_srcratio1();
    cerPencilBeam.srcRatio3 = commissionUnit.get_srcratio3();
    cerPencilBeam.eeqdiam = commissionUnit.get_eeqdiam() / DB_EXCHANGE_FRONT;
    cerPencilBeam.psd = commissionUnit.get_psd() / DB_EXCHANGE_FRONT;
    cerPencilBeam.sddXJaw = commissionUnit.get_xsdd() / DB_EXCHANGE_FRONT;
    cerPencilBeam.sddYJaw = commissionUnit.get_ysdd() / DB_EXCHANGE_FRONT;

    cerPencilBeam.scXPositions = commissionUnit.get_scxpositions();
    cerPencilBeam.scYPositions = commissionUnit.get_scypositions();
    cerPencilBeam.scBlocb = commissionUnit.get_sc();
    cerPencilBeam.referencefluence = commissionUnit.get_referencefluence();

    GetPhysicalWedgeFieldInfo_i(physicalWedgeUID, commissionUID, cerPencilBeam.physicalWedgeList, 1);
    GetVirtualWedgeFieldInfo_i(virtualWedgeUID, commissionUID, 1, cerPencilBeam.virtualWedgeList);
    return true;
}

bool TpsBLController::GetMonteCarloDataInfo(std::list<std::string>&physicalWedgeUID,std::list<std::string>&virtualWedgeUID,std::string commissionUID,
    CertifiedMonteCarlo& monteCarlo)
{
        RtMcphocommissionedunit* phoCommUnit = nullptr;
        RtMcphocommissionedunit pmcPhoCommUnitV;
        if (mDataRepository->GetMachineSettingManager()->GetTable<RtMcphocommissionedunitTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), pmcPhoCommUnitV) <= 0)
        {
            TPS_LOG_DEV_ERROR << "Fail to get RtMcphocommissionedunit from manager";
            return false;
        }
        phoCommUnit = &pmcPhoCommUnitV;
        if (phoCommUnit != nullptr){

            monteCarlo.parentUID = commissionUID;

            monteCarlo.uid = phoCommUnit->get_uid();
            monteCarlo.xJawTopPosition = phoCommUnit->get_mcphoxjawupposition()/10;
            monteCarlo.xJawThickness = phoCommUnit->get_mcphoxjawthickness()/10;
            monteCarlo.mlcCenterPosition = phoCommUnit->get_mcphomlccenterposition()/10;
            monteCarlo.mlcThickness = phoCommUnit->get_mcphomlcthickness()/10;
            monteCarlo.mlcLeafendRadius = phoCommUnit->get_mcphomlleafendradius()/10;

            monteCarlo.airGapWidth = phoCommUnit->get_mcphomlcairgapwidth()/10;
            monteCarlo.tongueGrooveWidth = phoCommUnit->get_mcphomlctgwidth()/10;
            monteCarlo.muCorrectionFactor = phoCommUnit->get_mcphomufactor();
            monteCarlo.yJawTopPosition = phoCommUnit->get_mcphoyjawupposition()/10;
            monteCarlo.yJawThickness = phoCommUnit->get_mcphoyjawthickness()/10;

            std::vector<RtMcphomonoenergyweight*> phoMonoEnergyWeight;

            mDatabaseWrapper->GetMcphomonoenergyweightListByMcphocommissionedunituid(phoCommUnit->get_uid(), phoMonoEnergyWeight);

            for (auto it = phoMonoEnergyWeight.begin(); it != phoMonoEnergyWeight.end(); ++it){
                std::tr1::shared_ptr<CertifiedSpectrum>pSpectrumInfo(new CertifiedSpectrum());
                pSpectrumInfo->energy = (*it)->get_injectelectronenergy();
                pSpectrumInfo->weight = (*it)->get_injectelectronweight();
                monteCarlo.spectrumList.push_back(pSpectrumInfo);
            }

            monteCarlo.phoFilePath = phoCommUnit->get_mcpholist();
            monteCarlo.normTen = phoCommUnit->get_mcphonorm10x10();

            std::vector<McPhoAbsCorrectionFactor*> absFactor;
            std::string correctionFactor = phoCommUnit->get_mcphoabscorrectionfactor();

            if (correctionFactor !=""){
                std::vector<std::string> vecValue;
                (void)boost::split(vecValue,correctionFactor, boost::is_any_of("|"));
                int ilen = static_cast<int>(vecValue.size());
                absFactor.resize(ilen);
                for (int i=0;i<ilen;i++){
                    std::vector<std::string> strValue;
                    absFactor[i] = new McPhoAbsCorrectionFactor();
                    (void)boost::split(strValue,vecValue[i], boost::is_any_of(","));

                    absFactor[i]->m_fMcPhoAbsCorrectionFieldSize =static_cast<float>(atof(strValue[0].c_str()));
                    absFactor[i]->m_fMcPhoAbsCorrectionFactor =static_cast<float>(atof(strValue[1].c_str()));
                    strValue.clear();
                }
                vecValue.clear();
            }

            for (auto it = absFactor.begin(); it != absFactor.end(); ++it){
                std::tr1::shared_ptr<MonteCarloFieldSize>pFieldSizeInfo(new MonteCarloFieldSize());
                pFieldSizeInfo->fieldSize = (*it)->m_fMcPhoAbsCorrectionFieldSize;
                pFieldSizeInfo->absoluteDoseCorrectionFactor = (*it)->m_fMcPhoAbsCorrectionFactor;
                monteCarlo.fieldSizeList.push_back(pFieldSizeInfo);
            }

            std::vector<float>vEnergy;
            std::string phoEnergy =  phoCommUnit->get_mcphoenergy();
            StringToVectorFunc(phoEnergy,vEnergy);
            std::string energy;
            for (auto it = vEnergy.begin(); it != vEnergy.end(); ++it){
                energy =+ (*it);
            }
            monteCarlo.energyValue = energy;
            monteCarlo.electronContaminationFlag = (phoCommUnit->get_mcphoon()==1);
            monteCarlo.electronMaxDepth = phoCommUnit->get_mcphodm();
            monteCarlo.surfaceDose = phoCommUnit->get_mcphoffs10();
            monteCarlo.depthCoefficientK =phoCommUnit->get_mcphok();
            monteCarlo.offaxisCoefficientA = phoCommUnit->get_mcphoa();
            monteCarlo.df = phoCommUnit->get_mcphodf();
            monteCarlo.sf = phoCommUnit->get_mcphosf();
            monteCarlo.cOne = phoCommUnit->get_mcphoc1();
            monteCarlo.cTwo = phoCommUnit->get_mcphoc2();
            monteCarlo.cThree = phoCommUnit->get_mcphoc3();

            GetPhysicalWedgeFieldInfo_i(physicalWedgeUID,commissionUID,monteCarlo.physicalWedgeList,3);
            GetVirtualWedgeFieldInfo_i(virtualWedgeUID,commissionUID ,3,monteCarlo.virtualWedgeList);
        }
        return true;
}

bool TpsBLController::GetPhotonMonteCarloDataInfo(
    std::string commissionUID,
    shared_ptr<ms::proto::RT_MS_PhotonMonteCarloInformation>& protoPhotonMonteCarlo)
{
    //PMC part
    //查找和该commission相关的PMC数据,正常情况下一个对应一条数据
    std::shared_ptr<RtMcphocommissionedunit> mcphocommissionedunit;
    if (mDataRepository->GetMachineSettingManager()
        ->GetTable<RtMcphocommissionedunitTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), mcphocommissionedunit) <= 0)
    {
        TPS_LOG_DEV_ERROR << "Failed to get commission from manager.";
        return false;
    }

    protoPhotonMonteCarlo.reset(new ms::proto::RT_MS_PhotonMonteCarloInformation());
    if (mcphocommissionedunit != nullptr)
    {
        std::vector<std::string> vecValue;
        std::vector<std::string> vecValue1;
        std::string strType ;
        std::string strType1 ;
        size_t pmciLen;
        size_t pmciLen1;

        protoPhotonMonteCarlo->set_commissionuid(mcphocommissionedunit->get_commissionedunituid());
        //PDD
        ms::proto::RT_MS_PMCPDD* pddparameter = protoPhotonMonteCarlo->mutable_pddparameter();
        pddparameter->set_primaryenergystart(mcphocommissionedunit->get_mcprimaryenergystart());
        pddparameter->set_primaryenergyend(mcphocommissionedunit->get_mcprimaryenergyend());
        pddparameter->set_primaryenergybin(mcphocommissionedunit->get_mcprimaryenergybin());
        pddparameter->set_primaryenergyrestart(mcphocommissionedunit->get_mcprimaryenergyrestart());
        pddparameter->set_secondaryenergystart(mcphocommissionedunit->get_mcsecondaryenergystart());
        pddparameter->set_secondaryenergyend(mcphocommissionedunit->get_mcsecondaryenergyend());
        pddparameter->set_secondaryenergybin(mcphocommissionedunit->get_mcsecondaryenergybin());
        pddparameter->set_secondaryenergyrestart(mcphocommissionedunit->get_mcsecondaryenergyrestart());
        pddparameter->set_electronenergy(mcphocommissionedunit->get_mcelectronenergy());
        pddparameter->set_particlenumber(mcphocommissionedunit->get_mccommiparticlenumber());
        if (mcphocommissionedunit->get_mcphoparam10() == "")
        {
            pddparameter->set_completefilesnumber(0);
        } 
        else
        {
            pddparameter->set_completefilesnumber(boost::lexical_cast<int>(mcphocommissionedunit->get_mcphoparam10()));
        }

        strType = mcphocommissionedunit->get_mcprimaryenergylist();
        strType1 = mcphocommissionedunit->get_mcprimaryweightlist();
        ms::proto::RT_MS_PMCEnergyAndSpectrum* primarySpectrum = nullptr;
        if (strType != "")
        {
            (void)boost::split(vecValue,strType, boost::is_any_of(","));
            (void)boost::split(vecValue1,strType1, boost::is_any_of(","));
            pmciLen = vecValue.size();
            pmciLen1 = vecValue1.size();
            if (pmciLen == pmciLen1)
            {
                for (int i=0; i < pmciLen;i++){
                    if(vecValue[i]!="")
                    {
                        primarySpectrum = pddparameter->add_primaryspectrumcollection();
                        primarySpectrum->set_energy(boost::lexical_cast<float>(vecValue[i]));
                        primarySpectrum->set_weight(boost::lexical_cast<float>(vecValue1[i]));
                    }
                }
                vecValue.clear();
                vecValue1.clear();
            }
            else
            {
                TPS_LOG_DEV_ERROR<<"mcprimaryenergylist and mcprimaryweightlist length are not same";
                return false;
            }
        }


        strType = mcphocommissionedunit->get_mcsecondaryenergylist();
        strType1 = mcphocommissionedunit->get_mcsecondaryweightlist();
        ms::proto::RT_MS_PMCEnergyAndSpectrum* secondarySpectrum = nullptr;
        if (strType != "")
        {
            (void)boost::split(vecValue,strType, boost::is_any_of(","));
            (void)boost::split(vecValue1,strType1, boost::is_any_of(","));
            pmciLen = vecValue.size();
            pmciLen1 = vecValue1.size();
            if (pmciLen == pmciLen1)
            {
                for (int i=0; i < pmciLen;i++){
                    if(vecValue[i]!="")
                    {
                        secondarySpectrum = pddparameter->add_secondaryspectrumcollection();
                        secondarySpectrum->set_energy(boost::lexical_cast<float>(vecValue[i]));
                        secondarySpectrum->set_weight(boost::lexical_cast<float>(vecValue1[i]));
                    }
                }
                vecValue.clear();
                vecValue1.clear();
            }
            else
            {
                TPS_LOG_DEV_ERROR<<"mcsecondaryenergylist and mcsecondaryweightlist length are not same";
                return false;
            }
        }


        strType = mcphocommissionedunit->get_mcelecenergylist();
        strType1 = mcphocommissionedunit->get_mcelecweightlist();
        ms::proto::RT_MS_PMCEnergyAndSpectrum* electronSpectrum = nullptr;
        if (strType != "")
        {
            (void)boost::split(vecValue,strType, boost::is_any_of(","));
            (void)boost::split(vecValue1,strType1, boost::is_any_of(","));
            pmciLen = vecValue.size();
            pmciLen1 = vecValue1.size();
            if (pmciLen == pmciLen1)
            {
                for (int i=0; i < pmciLen;i++){
                    if(vecValue[i]!="")
                    {
                        electronSpectrum = pddparameter->add_electronspectrumcollection();
                        electronSpectrum->set_energy(boost::lexical_cast<float>(vecValue[i]));
                        electronSpectrum->set_weight(boost::lexical_cast<float>(vecValue1[i]));
                    }
                }
                vecValue.clear();
                vecValue1.clear();
            }
            else
            {
                TPS_LOG_DEV_ERROR<<"mcelecenergylist and gmcelecweightlist length are not same";
                return false;
            }
        }


        //In field
        auto inFieldParameter = protoPhotonMonteCarlo->mutable_infieldparameter();
        inFieldParameter->set_primarysoften(mcphocommissionedunit->get_mcprimarysoftening());
        inFieldParameter->set_softenparameter(mcphocommissionedunit->get_mcsofteningtunningf());
        inFieldParameter->set_selectedsecondarysourcetype(mcphocommissionedunit->get_mcsecondarysourcetype());
        inFieldParameter->set_scattersoften(mcphocommissionedunit->get_mcscatteringcompton());

        strType = mcphocommissionedunit->get_mcsecondarysourcepara();
        ms::proto::RT_MS_PMCSecondarySourceParameter* secondarySourceParameter = nullptr;
        (void)boost::split(vecValue,strType, boost::is_any_of(","));
        if (strType != "")
        {
            pmciLen = vecValue.size();
            if (pmciLen%2 != 0)
            {
                TPS_LOG_DEV_ERROR<<"mcsecondarysourcepara value in tmsmachinetable is wrong!";
                return false;
            }
            for (int i = 0; i < pmciLen - 1;)
            {
                secondarySourceParameter = inFieldParameter->add_secondarysourceparametercollection();
                //mcsecondarysourcepara奇数位给A，偶数位给B
                //例如1234这个数值，13给A,24给B
                secondarySourceParameter->set_flatfiltera(boost::lexical_cast<float>(vecValue[i]));
                secondarySourceParameter->set_flatfilterb(boost::lexical_cast<float>(vecValue[i+1]));
                i = i + 2;
            }
            vecValue.clear();
        }


        //Out Field
        auto outFieldParameter = protoPhotonMonteCarlo->mutable_outfieldparameter();
        outFieldParameter->set_flatdistance(mcphocommissionedunit->get_mcprimaryflatfilterdist()/LOAD_FROM_DB_CONVERTER);
        outFieldParameter->set_jawdistance(mcphocommissionedunit->get_mcprimaryjawdistance()/LOAD_FROM_DB_CONVERTER);
        outFieldParameter->set_sigmap(mcphocommissionedunit->get_mcsourcesigmaprimary());
        outFieldParameter->set_sigmas(mcphocommissionedunit->get_mcsourcesigmasecondary());
        outFieldParameter->set_tailcorrection(mcphocommissionedunit->get_mclargefieldtailcor());
        outFieldParameter->set_rectanglescattercorrection(mcphocommissionedunit->get_mcrectanglescacor());
        outFieldParameter->set_mlcleakage(mcphocommissionedunit->get_mcmlcleakage()); 
        outFieldParameter->set_gaussianblurswitch(mcphocommissionedunit->get_mcgaussianblurswitch());
        outFieldParameter->set_gaussiansigmax(mcphocommissionedunit->get_mcgaussiansigmax());
        outFieldParameter->set_gaussiansigmay(mcphocommissionedunit->get_mcgaussiansigmay());

        //Electron Contamination
        strType = mcphocommissionedunit->get_mcphotonfluence();
        auto electronContaminationParameter = protoPhotonMonteCarlo->mutable_electroncontaminationparameter();

        (void)boost::split(vecValue,strType, boost::is_any_of(","));
        if (vecValue.size() == 4)
        {
            electronContaminationParameter->set_standardradius(boost::lexical_cast<float>(vecValue[0]));
            electronContaminationParameter->set_sigma(boost::lexical_cast<float>(vecValue[1]));
            electronContaminationParameter->set_offset(boost::lexical_cast<float>(vecValue[2]));
            electronContaminationParameter->set_powerfactor(boost::lexical_cast<float>(vecValue[3]));
        }

        //Absolute dose
        auto absoluteDoseParameter = protoPhotonMonteCarlo->mutable_absolutedoseparameter();
		absoluteDoseParameter->set_norm10dose(mcphocommissionedunit->get_mcphoc3());
        absoluteDoseParameter->set_absolutedosecollectionstr(mcphocommissionedunit->get_mcphoabscorrectionfactor());

        //已改为二维表，这里的load逻辑需要修改
        //std::vector<std::string> absCorrectionFactor;
        //strType = mcphocommissionedunit->get_mcphoabscorrectionfactor();
        //ms::proto::RT_MS_PMCFieldSizeAndAbsoluteDose* pmcabsoluteDose = nullptr;
        //if (strType.find(",") != -1)
        //{
        //    //同一个前半段放fieldsize,同一个后半段放factor
        //    //这两个字段间用，分隔
        //    //不同的用'|'分隔
        //    (void)boost::split(vecValue,strType, boost::is_any_of("|"));
        //    pmciLen = vecValue.size();
        //    for (int i=0; i < pmciLen;i++){
        //        (void)boost::split(absCorrectionFactor,vecValue[i], boost::is_any_of(","));
        //        pmcabsoluteDose = absoluteDoseParameter->add_absolutedosecollection();
        //        pmcabsoluteDose->set_fieldsize(boost::lexical_cast<float>(absCorrectionFactor[0]));
        //        pmcabsoluteDose->set_absolutedose(boost::lexical_cast<float>(absCorrectionFactor[1]));
        //    }
        //    vecValue.clear();
        //}
        
        //Wedge
        GetMCPhysicalWedgeFieldInfo_i(commissionUID,protoPhotonMonteCarlo);
        GetMCVirtualWedgeFieldInfo_i(commissionUID,protoPhotonMonteCarlo);
    }
    return true;
}

int TpsBLController::MonteCarloAlg(std::string& commissionUID){

    if (!AlgCommissionProxyInit(commissionUID, mAlgCommissionProxy) == TPS_ER_SUCCESS)
    {
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

//此处update 只是添加一个字段的值，调用Update，表设计缺陷导致
int TpsBLController::AddVirtualWedge(std::string& commissionUID, std::string& accUID,int algType){

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    RtGoldenstt goldenSTT;
    if (manager->GetTable<RtGoldensttTable>()->Get<Tag_NonUniqueKey2>(boost::make_tuple(commissionUID, accUID), goldenSTT) <= 0)
    {
        TPS_LOG_DEV_ERROR << "fail to get goldenstt from manager for commissionUid:" << commissionUID << "and accuid" << accUID;
        return TPS_ER_FAILURE;
    }

    std::vector<float>accumulateMu;
    std::string strAccumulatedMU = goldenSTT.get_accumulatedmu();
    TpsUtility::StringToVector(strAccumulatedMU, accumulateMu);

    std::vector<float> movJawPos;
    std::string strMovJAWPos = goldenSTT.get_movjawpos();
    TpsUtility::StringToVector(strMovJAWPos, movJawPos);

    if (accumulateMu.size() == 0 || movJawPos.size() == 0 || accumulateMu.size() != movJawPos.size())
    {
        TPS_LOG_DEV_ERROR << "accumulateMu or movJawPos size is right!";
        return TPS_ER_FAILURE;
    }
    else
    {
        int size = accumulateMu.size();
        std::vector<float> factor;
        for (int i = 0; i < size; i++)
        {
            factor.push_back(0);
        }
        std::string strfactor;
        FloatToStringFunc(factor, strfactor);

        switch (algType)
        {
        case RtDbDef::CONVOLUTION:
            goldenSTT.set_correctionfactorconvolution(strfactor);
            break;
        case RtDbDef::PENCILBEAM:
            goldenSTT.set_correctionfactorpencilbeam(strfactor);
            break;
        case RtDbDef::MONTECARLO:
            goldenSTT.set_correctionfactormontecarlo(strfactor);
            break;
        default:
            break;
        }

        if (mDatabaseWrapper->UpdateGoldenstt(goldenSTT)){

            TPS_LOG_DEV_ERROR << "Fail to add virtualwedge to DB";
            return TPS_ER_FAILURE;
        }

        if (!mDataRepository->GetMachineSettingManager()->GetTable<RtGoldensttTable>()->Update<Tag_UniQueKey>(boost::make_tuple(goldenSTT.get_uid()), goldenSTT)){

            TPS_LOG_DEV_ERROR << "Fail to add virtualwedge to Manager";
            return TPS_ER_FAILURE;
        }
    }
    return TPS_ER_SUCCESS;
}

//此处delete 只是删除一个字段的值，调用Update，表设计缺陷导致
int TpsBLController::DeleteVirtualWedge(std::string& commissionUID, std::string& accUID, int algType){

    vector<RtGoldenstt> newGoldenSttList;
    mDataRepository->GetMachineSettingManager()->GetTable<RtGoldensttTable>()->Get<Tag_NonUniqueKey2>(boost::make_tuple(commissionUID, accUID), newGoldenSttList);

    if (newGoldenSttList.size() == 1)
    {
        std::string strfactor = "";

        switch (algType)
        {
        case RtDbDef::CONVOLUTION:
            newGoldenSttList[0].set_correctionfactorconvolution(strfactor);
            break;
        case RtDbDef::PENCILBEAM:
            newGoldenSttList[0].set_correctionfactorpencilbeam(strfactor);
            break;
        case RtDbDef::MONTECARLO:
            newGoldenSttList[0].set_correctionfactormontecarlo(strfactor);
            break;
        default:
            break;
        }

        if (mDatabaseWrapper->UpdateGoldenstt(newGoldenSttList[0])){

            TPS_LOG_DEV_ERROR << "Fail to remove virtualwedge from db!";
            return TPS_ER_FAILURE;
        }

        if (!mDataRepository->GetMachineSettingManager()->GetTable<RtGoldensttTable>()->Update<Tag_UniQueKey>(boost::make_tuple(newGoldenSttList[0].get_uid()), newGoldenSttList[0])){

            TPS_LOG_DEV_ERROR << "Fail to add virtualwedge from Manager!";
            return TPS_ER_FAILURE;
        }
    }
    else
    {
        TPS_LOG_DEV_ERROR << "Db error ,more than one goldenstt founded!";
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}


int TpsBLController::DeleteWedgeField(std::string& commissionUID, std::string& accUID, int algType){

    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    std::vector<RtAccessorychunk> accessoryChunkList;
    if (manager->GetTable<RtAccessorychunkTable>()->Get<Tag_NonUniqueKey3>(boost::make_tuple(accUID, commissionUID, algType), accessoryChunkList) == 2)//一次查询对应wedge 和carrier 两条数据
    {
        for (auto it = accessoryChunkList.begin(); it != accessoryChunkList.end(); it++)
        {
            RtContour contour;
            if (manager->GetTable<RtContourTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(it->get_uid()), contour) <= 0)
            {
                TPS_LOG_DEV_ERROR << "Fail to get contour from manager";
                return TPS_ER_FAILURE;
            }

            if (mDatabaseWrapper->EraseObject(contour.get_uid(), RtDbDef::TYPE_CONTOUR))
            {
                TPS_LOG_DEV_ERROR << "Fail to delete contour from DB";
                return TPS_ER_FAILURE;
            }

            if (manager->GetTable<RtContourTable>()->Delete<Tag_UniQueKey>(boost::make_tuple(contour.get_uid())) <= 0)
            {
                TPS_LOG_DEV_ERROR << "Fail to get contour from manager";
                return TPS_ER_FAILURE;
            }

            if (mDatabaseWrapper->EraseObject(it->get_uid(), RtDbDef::TYPE_ACCESSORYCHUNK))
            {
                TPS_LOG_DEV_ERROR << "Fail to delete accessorychunk from DB";
                return TPS_ER_FAILURE;
            }

            if (manager->GetTable<RtAccessorychunkTable>()->Delete<Tag_UniQueKey>(boost::make_tuple(it->get_uid())) <= 0)
            {
                TPS_LOG_DEV_ERROR << "Fail to get contour from manager";
                return TPS_ER_FAILURE;
            }
        }
    }
    return TPS_ER_SUCCESS;
}


bool TpsBLController::LoadMachineSettingFromDB(const std::string& machineUid)
{
    RtMachine* machine = nullptr;
    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    mDatabaseWrapper->GetMachine(machineUid, machine);

    if (machine == nullptr)
    {
        return false;
    }

	std::string sMD5("");
	sMD5 = mDatabaseWrapper->CalcHash((const void*)(machine), RtDbDef::TYPE_MACHINE);
	machine->set_crc_valid(sMD5 == machine->get_crc());

    if (!manager->GetTable<RtMachineTable>()->Add(*machine))
    {
        return false;
    }

    DEL_PTR(machine);

    std::vector<RtTray*> vTray;
    mDatabaseWrapper->GetTrayListByMachineuid(machineUid, vTray);
    for (auto tray = vTray.begin(); tray != vTray.end(); ++tray){

        if (!manager->GetTable<RtTrayTable>()->Add(**tray))
        {
            return false;
        }
    }
    DEL_PTRVECTOR(vTray);

    std::vector<RtApplicator*> vElectronApplicator;
    mDatabaseWrapper->GetApplicatorListByMachineuid(machineUid, vElectronApplicator);
    for (auto applicator = vElectronApplicator.begin(); applicator != vElectronApplicator.end(); ++applicator){

        if (!manager->GetTable<RtApplicatorTable>()->Add(**applicator))
        {
            return false;
        }
    }
    DEL_PTRVECTOR(vElectronApplicator);

    std::vector<RtAccessory* >vAccessoryList;
    mDatabaseWrapper->GetAccessoryListByMachineuid(machineUid, vAccessoryList);
    for (auto accessory = vAccessoryList.begin(); accessory != vAccessoryList.end(); ++accessory){

        if (!manager->GetTable<RtAccessoryTable>()->Add(**accessory))
        {
            return false;
        }
    }
    DEL_PTRVECTOR(vAccessoryList);

    std::vector<RtCommissionedunit*> vRtCommissionedunitList;
    std::vector<string> machineList;
    machineList.push_back(machineUid);
    mDatabaseWrapper->GetCommissionedunitListByMachineUidList(machineList, vRtCommissionedunitList);
    for (auto commission = vRtCommissionedunitList.begin(); commission != vRtCommissionedunitList.end(); ++commission){
        if (!manager->GetTable<RtCommissionedunitTable>()->Add(**commission))
        {
            return false;
        }

        RtMcphocommissionedunit* mcphocommissioned = nullptr;
        mDatabaseWrapper->GetMcphocommissionedunitByUnituid((*commission)->get_uid(), mcphocommissioned);
        if (mcphocommissioned != nullptr)
        {
            if (!manager->GetTable<RtMcphocommissionedunitTable>()->Add(*mcphocommissioned))
            {
                return false;
            }
        }
        DEL_PTR(mcphocommissioned);

        std::vector<RtWedgefactor*> vWedgeFactor;
        vWedgeFactor.clear();
        mDatabaseWrapper->GetWedgefactorListByUnituid((*commission)->get_uid(), vWedgeFactor);
        for (auto factor = vWedgeFactor.begin(); factor != vWedgeFactor.end(); ++factor){
            if (!manager->GetTable<RtWedgefactorTable>()->Add(**factor))
            {
                return false;
            }
        }
        DEL_PTRVECTOR(vWedgeFactor);

        std::vector<RtCommissionedunit_tray*> unit_trayList;
        mDatabaseWrapper->GetCommissionedunit_trayListByUnitUid((*commission)->get_uid(), unit_trayList);
        for (auto unit_tray = unit_trayList.begin(); unit_tray != unit_trayList.end(); ++unit_tray){
            if (!manager->GetTable<RtCommissionedunit_trayTable>()->Add(**unit_tray))
            {
                return false;
            }
        }
        DEL_PTRVECTOR(unit_trayList);

        RtMeasureddata* measureData = nullptr;
        mDatabaseWrapper->GetMeasureddataByUnituid((*commission)->get_uid(), measureData);
        if (measureData != nullptr)
        {
            if (!manager->GetTable<RtMeasureddataTable>()->Add(*measureData))
            {
                return false;
            }
        }
        DEL_PTR(measureData);

        std::vector<RtKernelweight*> vkernelWeightList;
        mDatabaseWrapper->GetKernelweightListByUnituid((*commission)->get_uid(), vkernelWeightList);
        for (auto kernelWeight = vkernelWeightList.begin(); kernelWeight != vkernelWeightList.end(); ++kernelWeight){
            if (!manager->GetTable<RtKernelweightTable>()->Add(**kernelWeight))
            {
                return false;
            }
        };
        DEL_PTRVECTOR(vkernelWeightList);

        std::vector<RtAccessorychunk*> vaccessorychunkList;
        mDatabaseWrapper->GetAccessorychunkListByCommissionedunitUid((*commission)->get_uid(), vaccessorychunkList);
        for (auto accessorychunk = vaccessorychunkList.begin(); accessorychunk != vaccessorychunkList.end(); ++accessorychunk){
            if (!manager->GetTable<RtAccessorychunkTable>()->Add(**accessorychunk))
            {
                return false;
            }

            RtContour* contour = nullptr;
            mDatabaseWrapper->GetContourByAccessorychunkuid((*accessorychunk)->get_uid(), contour);
            if (contour != nullptr)
            {
                if (!manager->GetTable<RtContourTable>()->Add(*contour))
                {
                    return false;
                }
            }
            DEL_PTR(contour);
        };
        DEL_PTRVECTOR(vaccessorychunkList);

        std::vector<RtGoldenstt*> vgoldensttList;
        std::vector<std::string> commissionUidList;
        commissionUidList.push_back((*commission)->get_uid());
        mDatabaseWrapper->GetGoldensttListByCommissionedunitUidList(commissionUidList, vgoldensttList);
        for (auto goldenstt = vgoldensttList.begin(); goldenstt != vgoldensttList.end(); ++goldenstt){
            if (!manager->GetTable<RtGoldensttTable>()->Add(**goldenstt))
            {
                return false;
            }
        };
        DEL_PTRVECTOR(vgoldensttList);
    }
    DEL_PTRVECTOR(vRtCommissionedunitList);
    return true;
}

bool TpsBLController::GenerateCurveProfilePath(RtMeasuredprofile& profile){

    std::string sPath = mDatabaseWrapper->GetSavePath() + "/MeasureProfile/";
    if (!mDatabaseWrapper->CreatePath(sPath)){
        return false;
    }
    if (!boost::filesystem::exists(sPath)){
        boost::filesystem::create_directory(sPath);
    }

    string str = sPath + profile.get_uid();
    profile.set_meapointdoselist(str);


    sPath = mDatabaseWrapper->GetSavePath() +"/CalProfile/";
    if (!mDatabaseWrapper->CreatePath(sPath)){
        return true;
    }
    if (!boost::filesystem::exists(sPath)){
        boost::filesystem::create_directory(sPath);
    }
    str = sPath + profile.get_uid();
    profile.set_calcpointdoselist(str);


    sPath = mDatabaseWrapper->GetSavePath() + "/CurveErrorResult/";
    if (!mDatabaseWrapper->CreatePath(sPath)){
        return false;
    }
    if (!boost::filesystem::exists(sPath)){
        boost::filesystem::create_directory(sPath);
    }

    str = sPath + profile.get_uid();
    profile.set_curveresultanalyselist(str);

    return true;

}

bool TpsBLController::UpdateFileList(RtMeasuredprofile * profile,RtMeasuredprofile& retProfile){

    //std::string filePath = profile->get_meapointdoselist();

    int iMeaPointDoseList = static_cast<int>(profile->GetMeaPointDoseList().size());
    std::string str;
    for (int i=0;i<iMeaPointDoseList;i++){
        if (str != ""){
            str = str + "|";
        }
        str = str + boost::lexical_cast<std::string>(profile->GetMeaPointDoseList()[i]->m_fDose) + ",";
        str = str + boost::lexical_cast<std::string>(profile->GetMeaPointDoseList()[i]->m_fX) + ",";
        str = str + boost::lexical_cast<std::string>(profile->GetMeaPointDoseList()[i]->m_fY) + ",";
        str = str + boost::lexical_cast<std::string>(profile->GetMeaPointDoseList()[i]->m_fZ);
    }

    //WriteFile_i(filePath,str);
    profile->set_meapointdoselist(str);


    // filePath = profile->get_calcpointdoselistfilepath();
    str = "";
    int iCalcPointDoseLists = static_cast<int>(profile->GetCalcPointDoseList().size());
    for (int i=0;i<iCalcPointDoseLists;i++){
        if (str != ""){
            str = str + "|";
        }
        str = str + boost::lexical_cast<std::string>(profile->GetCalcPointDoseList()[i]->m_fDose) + ",";
        str = str + boost::lexical_cast<std::string>(profile->GetCalcPointDoseList()[i]->m_fX) + ",";
        str = str + boost::lexical_cast<std::string>(profile->GetCalcPointDoseList()[i]->m_fY) + ",";
        str = str + boost::lexical_cast<std::string>(profile->GetCalcPointDoseList()[i]->m_fZ);
    }

    //WriteFile_i(filePath,str);
    profile->set_calcpointdoselist(str);

    //filePath = profile->get_curveresultanalysefilepath();
    str = "";
    int icureErrorLists = static_cast<int>(profile->GetCurveResultAnalyse().size());
    for (int i=0;i<icureErrorLists;i++){
        if (str != ""){
            str = str + "|";
        }
        str = str + boost::lexical_cast<std::string>(profile->GetCurveResultAnalyse()[i]->m_sName) + ",";
        str = str + boost::lexical_cast<std::string>(profile->GetCurveResultAnalyse()[i]->m_fMaxError) + ",";
        str = str + boost::lexical_cast<std::string>(profile->GetCurveResultAnalyse()[i]->m_fFailurePoits) + ",";
        str = str + boost::lexical_cast<std::string>(profile->GetCurveResultAnalyse()[i]->m_fErrorLimit) + ",";
        str = str + boost::lexical_cast<std::string>(profile->GetCurveResultAnalyse()[i]->m_fAverageError);
    }

    // WriteFile_i(filePath,str);
    profile->set_curveresultanalyselist(str);
    retProfile = *profile;
    return true;

}

bool TpsBLController::WriteFile_i(std::string sFilePath,string& fileContent){

    std::ofstream ofs;
    ofs.open(sFilePath ,std::ofstream::binary|std::ofstream::trunc|std::ofstream::out);
    if (!ofs) {
        TPS_LOG_DEV_ERROR<<"Failed to open file["<<sFilePath<<"].";
        return false;
    }
    ofs<< fileContent;
    //ofs.write(fileContent.c_str(), fileContent.size());
    ofs.close();
    return true;
}

bool TpsBLController::GetGoldensttListByAccUidAndUnitUid(std::string commissionUID,std::string selectedAcessoryUID,std::string& goldenUID){

    std::vector<RtGoldenstt*> goldenSttList;
    if (!mDatabaseWrapper->GetGoldensttListByAccUidAndUnitUid(selectedAcessoryUID,commissionUID,goldenSttList))
    {
        if (goldenSttList.size()== 1)
        {
            goldenUID = goldenSttList[0]->get_uid();
            return true;
        }
    }
    return false;
}


bool TpsBLController::UpdatePDDThreeTables(std::string commissionUID, ms::proto::RT_MS_ImportFromFileList* measureUIDList,
    std::vector<EnergyAndWeight>& primarySpectrumCollection,
    std::vector<EnergyAndWeight>& secondarySpectrumCollection,
    std::vector<EnergyAndWeight>& electronSpectrumCollection)
{
    if (!AlgCommissionProxyInit(commissionUID, mAlgCommissionProxy) == TPS_ER_SUCCESS)
    {
        return TPS_ER_FAILURE;
    }


    //MeasuredProfileList
    std::string uID;
    int  algType;
    std::vector<RtMeasuredprofile*> tempMeasuredProfileList;
    std::string operatorType = measureUIDList->objectoperationtype();

    for (int count = 0; count < measureUIDList->importfromfilelist_size(); ++count)
    {
        uID = measureUIDList->importfromfilelist(count).uid();
        algType = measureUIDList->importfromfilelist(count).algtype();
        RtMeasuredprofile* measuredProfile = new RtMeasuredprofile;
        if (mDataRepository->GetMachineSettingManager()->GetTable<RtMeasuredprofileTable>()->Get<Tag_UniQueKey>(boost::make_tuple(uID), *measuredProfile) <= 0)
        {
            TPS_LOG_DEV_ERROR << "fail to get measureprofile from manager.";
            return false;
        }
        tempMeasuredProfileList.push_back(measuredProfile);
    }
    mAlgCommissionProxy->SetMeasureProfileList(tempMeasuredProfileList);

    std::vector<RTALG::EnergyAndWeight> OutPWeight;
    std::vector<RTALG::EnergyAndWeight> OutSWeight;
    std::vector<RTALG::EnergyAndWeight> OutEWeight;
    int result = mAlgCommissionProxy->MonteCarloOptSpectrum(OutPWeight, OutSWeight, OutEWeight);

    if (result != 0)
    {
        TPS_LOG_DEV_ERROR << "ALG Monte Carlo Opt failed:";
        return false;
    }

    CommissionedUnitConverter::ConvertToALG(OutPWeight, primarySpectrumCollection);
    CommissionedUnitConverter::ConvertToALG(OutSWeight, secondarySpectrumCollection);
    CommissionedUnitConverter::ConvertToALG(OutEWeight, electronSpectrumCollection);

    //save to DB
    std::string primaryEnergyListString;
    std::string primaryWeightListString;
    for (int i = 0; i < primarySpectrumCollection.size(); ++i)
    {
        primaryEnergyListString += boost::lexical_cast<std::string>(primarySpectrumCollection[i].energy);


        primaryWeightListString += boost::lexical_cast<std::string>(primarySpectrumCollection[i].weight);

        //最后个值后面不加“，”
        if (i != primarySpectrumCollection.size() - 1)
        {
            primaryEnergyListString += ",";
            primaryWeightListString += ",";
        }

    }

    RtMcphocommissionedunit pmcPhoCommUnit;
    if (mDataRepository->GetMachineSettingManager()->GetTable<RtMcphocommissionedunitTable>()->
        Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), pmcPhoCommUnit) <= 0)
    {
        TPS_LOG_DEV_ERROR << "Failed to get phoCommUnit from manager.";
        return false;
    }

    pmcPhoCommUnit.set_mcprimaryenergylist(primaryEnergyListString);
    pmcPhoCommUnit.set_mcprimaryweightlist(primaryWeightListString);

    std::string secondaryEnergyListString;
    std::string secondaryWeightListString;
    for (int i = 0; i < secondarySpectrumCollection.size(); ++i)
    {
        secondaryEnergyListString += boost::lexical_cast<std::string>(secondarySpectrumCollection[i].energy);
        secondaryWeightListString += boost::lexical_cast<std::string>(secondarySpectrumCollection[i].weight);
        if (i != secondarySpectrumCollection.size() - 1)
        {
            secondaryEnergyListString += ",";
            secondaryWeightListString += ",";
        }

    }
    pmcPhoCommUnit.set_mcsecondaryenergylist(secondaryEnergyListString);
    pmcPhoCommUnit.set_mcsecondaryweightlist(secondaryWeightListString);

    std::string elecEnergyListString;
    std::string elecWeightListString;
    for (int i = 0; i < electronSpectrumCollection.size(); ++i)
    {
        elecEnergyListString += boost::lexical_cast<std::string>(electronSpectrumCollection[i].energy);
        elecWeightListString += boost::lexical_cast<std::string>(electronSpectrumCollection[i].weight);
        if (i < electronSpectrumCollection.size() - 1)
        {
            elecEnergyListString += ",";
            elecWeightListString += ",";
        }

    }
    pmcPhoCommUnit.set_mcelecenergylist(elecEnergyListString);
    pmcPhoCommUnit.set_mcelecweightlist(elecWeightListString);

    if (mDatabaseWrapper->UpdateMcphocommissionedunit(pmcPhoCommUnit)) {
        TPS_LOG_DEV_ERROR << "save PhotonMonteCarloInformation to DB failed:";
        return false;
    }
    return true;
}


bool  TpsBLController::GenerateCorrectionFactor(std::string commissionUID, int algType, std::string selectedAcessoryUID,
    std::vector<float>&correctionFactor){

    if (!AlgCommissionProxyInit(commissionUID, mAlgCommissionProxy) == TPS_ER_SUCCESS)
    {
        return TPS_ER_FAILURE;

    }
    TpsMachineSettingManager* manager = mDataRepository->GetMachineSettingManager();
    std::vector<RtMeasuredprofile> measuredProfileList;
    manager->GetTable<RtMeasuredprofileTable>()->Get<Tag_NonUniqueKey0>(boost::make_tuple(commissionUID), measuredProfileList);

    std::vector<RtMeasuredprofile*> meaList;

    for (auto it = measuredProfileList.begin(); it != measuredProfileList.end(); it++)
    {
        meaList.push_back(&(*it));
    }
    mAlgCommissionProxy->SetMeasureProfileList(meaList);


    RtAccessory accessory;
    if (manager->GetTable<RtAccessoryTable>()->Get<Tag_UniQueKey>(boost::make_tuple(selectedAcessoryUID), accessory) <= 0){
        TPS_LOG_DEV_ERROR << "Failed to get accessory from manager.";
        return TPS_ER_FAILURE;
    }

    RTFWK::RTFWKDLDOAccessory* algAccessory = new RTFWK::RTFWKDLDOAccessory();
    RTFWK::RTFWKDLDOGoldenSTT *pGoldenSTT = new RTFWK::RTFWKDLDOGoldenSTT();

    std::string goldenUID;
    if (!GetGoldensttListByAccUidAndUnitUid(commissionUID, selectedAcessoryUID, goldenUID))
    {
        TPS_LOG_DEV_ERROR << "Failed to get goldenUID from DB.";
        return TPS_ER_FAILURE;
    }

    RtGoldenstt goldenManagerBuffer;
    if (manager->GetTable<RtGoldensttTable>()->Get<Tag_UniQueKey>(boost::make_tuple(goldenUID), goldenManagerBuffer) <= 0){
        TPS_LOG_DEV_ERROR << "Failed to get golden from manager.";
        return TPS_ER_FAILURE;
    }

    DBGoldenSttConvertToAlgType(&goldenManagerBuffer, *pGoldenSTT);
    GetAccessoryConvertToAlgData(&accessory, algAccessory);

    mAlgCommissionProxy->SetAccessory(algAccessory);
    mAlgCommissionProxy->CalcGsttCorrectionFactor(pGoldenSTT, (RTFWK::DOSEALGORITHM_TYPE)algType);
    if (algType == 1){
        correctionFactor = pGoldenSTT->GetCorrectionFactorPencilBeam();
    }
    else if (algType == 2){
        correctionFactor = pGoldenSTT->GetCorrectionFactorConvolution();
    }
    else{
        correctionFactor = pGoldenSTT->GetCorrectionFactorMonteCarlo();
    }

    AlgTypeConvertToDBGoldenStt(pGoldenSTT, goldenManagerBuffer);

    if (!manager->GetTable<RtGoldensttTable>()->Update<Tag_UniQueKey>(boost::make_tuple(goldenUID), goldenManagerBuffer))
    {
        TPS_LOG_DEV_ERROR << "update golden stt to Manager failed:";
        return TPS_ER_FAILURE;
    }

    if (!mDatabaseWrapper->QueryDBInTable(goldenManagerBuffer.get_uid(), RtDbDef::TYPE_GOLDENSTT)){
        if (0 != mDatabaseWrapper->InsertGoldenstt(goldenManagerBuffer)){
            TPS_LOG_DEV_ERROR << "insert golden stt to DB failed:";
            return TPS_ER_FAILURE;
        }
    }
    else if (0 != mDatabaseWrapper->UpdateGoldenstt(goldenManagerBuffer)){
        TPS_LOG_DEV_ERROR << "update golden stt to DB failed:";
        return TPS_ER_FAILURE;
    }

    DEL_PTR(algAccessory);
    DEL_PTR(pGoldenSTT);

    return TPS_ER_SUCCESS;
}

bool  TpsBLController::GetAccessoryConvertToAlgData( RtAccessory* accessory, RTFWK::RTFWKDLDOAccessory* algAccessory ){

    algAccessory->SetUID(accessory->get_uid());
    algAccessory->SetName(accessory->get_name());
    algAccessory->SetCode(accessory->get_code());
    algAccessory->SetAccessoryType((RTFWK::ACCESSORY_TYPE)accessory->get_accessorytype());
    algAccessory->SetMachineUID(accessory->get_machineuid());
    algAccessory->SetWedgeAngle(accessory->get_wedgeangle());
    algAccessory->SetWedgeOrientation((RTFWK::WEDGE_ORIENTATION)accessory->get_wedgeorientation());

    return true;
}


TPS_END_NAMESPACE
