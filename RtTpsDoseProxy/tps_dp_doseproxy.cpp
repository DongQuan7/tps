//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file tps_dp_dosecalculation.cpp
/// 
///  \version 1.0
/// 
///  \date    2013/12/14
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RtTpsDoseProxy/tps_dp_doseproxy.h"

#include "McsfDicomConvertor/mcsf_dicom_convertor.h"
#include "McsfDicomConvertor/mcsf_dicom_convertor_factory.h"
#include "McsfDataHeader/mcsf_data_header_element_map_interface.h"
#include "McsfDataHeader/mcsf_data_header_element_interface.h"


#include <limits>
#include <process.h>
#include "alg/RtAlgDataLayer/rt_alg_dl_object_base.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_dosegrid.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_beam.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_voi.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_poi.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_base.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_normgroup.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_dvh.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_machine.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_commissionedunit.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_accessorychunk.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_kerneldata.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_kerneldatalist.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_kernelweight.h"
#include "alg/RtAlgDataLayer/rt_alg_dl_object_beamcontribution.h"

#include "alg/alg_dc_dosecalculationmanager.h"
#include "alg/alg_common_common.h"
#include "alg/alg_common_dbparam.h"

#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_fluence_map_entity.h"
#include "RtTpsFramework/tps_fw_table.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

#include "RtTpsROIUtility/tps_ru_roi_shape_converter.h"

#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_dvhmanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_tablemanager.h"
#include "RtTpsDataAccess/tps_da_tcpntcpmanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"


#include "poi_converter.h"
#include "voi_converter.h"
#include "beam_converter.h"
#include "normgroup_converter.h"
#include "dosegrid_converter.h"
#include "image3d_converter.h"
#include "dvh_converter.h"
#include "machine_converter.h"
#include "commissioned_unit_converter.h"
#include "accessory_converter.h"
#include "accessory_chunk_converter.h"
#include "kernel_data_converter.h"
#include "kernel_data_list_converter.h"
#include "kernel_weight_converter.h"
#include "mcphocommissioned_unit_converter.h"
#include "goldenstt_converter.h"
#include "enum_converter.h"



#include "tps_error.h"
#include "tps_logger.h"

TPS_BEGIN_NAMESPACE

 TpsDoseProxy::TpsDoseProxy() : mDoseCalculationManager(NULL)
{
    //Initialize();
}

TpsDoseProxy::~TpsDoseProxy()
{
    if (nullptr != mDoseCalculationManager)
    {
        mDoseCalculationManager->End();
        delete mDoseCalculationManager;
        mDoseCalculationManager = nullptr;
    }
}

bool TpsDoseProxy::Initialize()
{
    std::vector<RtMachine*>                   vMachineList;
    std::vector<RtCommissionedunit*>          vCommissionedunitList;
    std::vector<RtAccessorychunk*>            vRtAccessorychunkList;
    std::vector<RtAccessory*>                 vRtAccessoryList;
    std::vector<RtKerneldata*>                vKerneldata;
    std::vector<RtKerneldatalist*>            vKerneldtalist;
    std::vector<RtKernelweight*>              vKernelweight;
    std::vector<RtMcphocommissionedunit*>     vMcphocommissionedunitList;
    std::vector<RtGoldenstt*>                 vGoldensttList;

    mDataRepository->GetMachineManager()->GetAllMachine(vMachineList);
    mDataRepository->GetMachineManager()->GetAllCommissionedUnit(vCommissionedunitList);
    mDataRepository->GetMachineManager()->GetAllAccessory(vRtAccessoryList);
    mDataRepository->GetMachineManager()->GetAllAccessoryChunk(vRtAccessorychunkList);
    mDataRepository->GetMachineManager()->GetAllKerneldata(vKerneldata);
    mDataRepository->GetMachineManager()->GetAllKerneldatalist(vKerneldtalist);
    mDataRepository->GetMachineManager()->GetAllKernelweight(vKernelweight);
    mDataRepository->GetMachineManager()->GetAllMcphocommissionedunit(vMcphocommissionedunitList);
    mDataRepository->GetMachineManager()->GetAllGoldenstt(vGoldensttList);

    std::vector<RTFWK::RTFWKDLDOMachine*>                   algMachines;
    std::vector<RTFWK::RTFWKDLDOCommissionedunit*>          algUnits;
    std::vector<RTFWK::RTFWKDLDOAccessory*>                 algAccessorys;
    std::vector<RTFWK::RTFWKDLDOAccessorychunk*>            algAccessorychunks;
    std::vector<RTFWK::RTFWKDLDOKernelData*>                algKerneldatas;
    std::vector<RTFWK::RTFWKDLDOKernelDataList*>            algKerneldatalists;
    std::vector<RTFWK::RTFWKDLDOKernelWeight*>              algKernelweights;
    std::vector<RTFWK::RTFWKDLDOMcPhoCommissionedUnit*>     algMcPhoCommissionUnits;
    std::vector<RTFWK::RTFWKDLDOGoldenSTT*>                 algGoldenstts;

    // step 1. machine
    std::vector<RtTray*> vTray;
    mDataRepository->GetMachineManager()->GetAllTray(vTray);
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOMachine>> machineCarrier;
    MachineConverter::CreateALGList(vMachineList, vTray, &machineCarrier);
    for (int i = 0; i < machineCarrier.size(); ++i)
    {
        algMachines.push_back(machineCarrier[i].get());
    }
    // step 2. commissioned unit
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOCommissionedunit>> unitCarrier;
    CommissionedUnitConverter::CreateALGList(vCommissionedunitList, &unitCarrier);
    for (int i = 0; i < unitCarrier.size(); ++i)
    {
        algUnits.push_back(unitCarrier[i].get());
    }
    // step 3. accessory
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOAccessory>> accessoryCarrier;
    AccessoryConverter::CreateALGList(vRtAccessoryList, &accessoryCarrier);
    for (int i = 0; i < accessoryCarrier.size(); ++i)
    {
        algAccessorys.push_back(accessoryCarrier[i].get());
    }
    // step 4. accessory chunk
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOAccessorychunk>> accessorychunkCarrier;
    std::vector<RtContour*> vProfile;
    mDataRepository->GetMachineManager()->GetAllAccessoryChunkProfile(vProfile);
    AccessoryChunkConverter::CreateALGList(vRtAccessorychunkList, vProfile, &accessorychunkCarrier);
    for (int i = 0; i < accessorychunkCarrier.size(); ++i)
    {
        algAccessorychunks.push_back(accessorychunkCarrier[i].get());
    }
    // step 5. kernel data
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOKernelData>> kerneldataCarrier;
    KernelDataConverter::CreateALGList(vKerneldata, &kerneldataCarrier);
    for (int i = 0; i < kerneldataCarrier.size(); ++i)
    {
        algKerneldatas.push_back(kerneldataCarrier[i].get());
    }
    // step 6. kernel data list
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOKernelDataList>> kerneldatalistCarrier;
    KernelDataListConverter::CreateALGList(vKerneldtalist, &kerneldatalistCarrier);
    for (int i = 0; i < kerneldatalistCarrier.size(); ++i)
    {
        algKerneldatalists.push_back(kerneldatalistCarrier[i].get());
    }
    // step 7. kernel weight
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOKernelWeight>> kernelweightCarrier;
    KernelWeightConverter::CreateALGList(vKernelweight, &kernelweightCarrier);
    for (int i = 0; i < kernelweightCarrier.size(); ++i)
    {
        algKernelweights.push_back(kernelweightCarrier[i].get());
    }

    // step 8 mcphocommissioned unit
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOMcPhoCommissionedUnit>> mcphocommissionedunitCarrier;
    McPhoCommissionedUnitConverter::CreateALGList(
        vMcphocommissionedunitList, mDataRepository, &mcphocommissionedunitCarrier);
    for (int i = 0; i < mcphocommissionedunitCarrier.size(); ++i)
    {
        algMcPhoCommissionUnits.push_back(mcphocommissionedunitCarrier[i].get());
    }

    // step 9 goldenstt
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOGoldenSTT>> goldensttCarrier;
    GoldenSTTConverter::CreateALGList(vGoldensttList, &goldensttCarrier);
    for (int i = 0; i < goldensttCarrier.size(); ++i)
    {
        algGoldenstts.push_back(goldensttCarrier[i].get());
    }

    std::vector<RTFWK::RTFWKDLDOCT2DensityMap*> vCT2Density;
    RTALG::DbParam* para = RTALG::DbParam::GetInstance(true);
    int ret = 0;

    ret = para->Initialize(algMachines,
        algUnits,
        algAccessorys,
        algAccessorychunks,
        algKerneldatas,
        algKernelweights,
        algKerneldatalists,
        algMcPhoCommissionUnits,
        algGoldenstts);

    mDoseCalculationManager = new RTALG::DoseCalculationManager();
    return mDoseCalculationManager->Start();
}

void TpsDoseProxy::DumpBeamInfo_i(RtBeam* pBeam)
{
    if (pBeam == nullptr)
    {
        printf("pBeam is nullptr");
        return;
    }

    printf("\nBeam Uid:%s:\n", pBeam->get_uid().c_str());
    std::vector<RtBeamsegment*> segments= pBeam->get_beamsegments();
    for (int n = 0; n < segments.size(); n++) 
    {
        printf("segment[%d]:\n", n);
        const float* pElems = segments[n]->get_t_beam_to_pat();
        printf("b2p: \t%f\t%f\t%f\t%f \n \t%f\t%f\t%f\t%f \n \t%f\t%f\t%f\t%f \n \t%f\t%f\t%f\t%f \n",
            pElems[0], pElems[1], pElems[2], pElems[3], pElems[4], pElems[5], pElems[6], pElems[7],
            pElems[8], pElems[9], pElems[10], pElems[11], pElems[12], pElems[13], pElems[14], pElems[15]);
    }
}

int TpsDoseProxy::CalculateDoseForNormgroup(const std::string& normalGroupUID)
{
    TPS_LOG_DEV_INFO << "[INFO][TpsDoseProxy][CalculateDoseForNormgroup]: " 
        << "Calculate Dose of normalgroup start: [" << normalGroupUID << "]";


    RtNormgroup* normGroup = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(normalGroupUID, &normGroup) || normGroup == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The normal group[" << normalGroupUID << "] does not exist.";
        return TPS_ER_FAILURE;
    }

    if (normGroup->get_doseperfraction() <= 0.0)
    {
        TPS_LOG_DEV_ERROR << "The normal group[" << normalGroupUID << "] dose perfraction is lessthan or equal to zero.";
        return TPS_ER_FAILURE;
    }

    const std::string planUid = normGroup->get_planuid();
    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUid, &plan) || plan == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The Plan[" << planUid << "] does not exist.";
        return TPS_ER_FAILURE;
    }

    std::string sCurrentImageDataUid = plan->get_imageseriesuid();
    TpsImage3DEntity* image3D = nullptr;
    mDataRepository->GetImageDataManager()->GetImage3DData(sCurrentImageDataUid, &image3D);
    if (nullptr == image3D)
    {
        TPS_LOG_DEV_ERROR << "there is no image for series: " << sCurrentImageDataUid;
        return TPS_ER_FAILURE;
    }

    int iSkinNum = CheckSeriesSkin_i(*image3D);
    if (iSkinNum == -1)
    {
        TPS_LOG_DEV_ERROR << "CheckSeriesSkin_i error: " << image3D->GetUID();
        return TPS_ER_FAILURE;
    }
    else if (iSkinNum == 0)
    {
        TPS_LOG_DEV_ERROR << "there is no skin for series: " << image3D->GetUID();
        return TPS_ER_CALC_NO_SKIN;//TPS_CALC_NO_SKIN;         Mark By Gou Panjie
    }
    else if (iSkinNum > 1)
    {
        TPS_LOG_DEV_ERROR << "there are more than one skin in series: " << image3D->GetUID();
        return TPS_ER_FAILURE;
    }

    // step 1. get skin and all voi by series uid from voiMgr
    std::vector<RtVoi*> voiList;
    if (!RetrieveVoiList_i(*image3D, &voiList))
    {
        TPS_LOG_DEV_ERROR << "there is error in function RetrieveVoiList_i";
        return TPS_ER_FAILURE;
    }

    // step 2. get poi by series uid from poiMgr
    std::vector<RtPoi*> tpsPoiList = mDataRepository->GetPoiManager()->GetAllPoiInSeries(sCurrentImageDataUid);
    if (tpsPoiList.empty())
    {
        TPS_LOG_DEV_WARNING << "poiList is empty.";
        return TPS_ER_FAILURE;
    }

    // step 3 need ct ct2densityArray 电子密度wzd-2017-03-31
    float* pCt2DensityArray = const_cast<float*>(
        mDataRepository->GetImageDataManager()->GetCT2ReferenceEDensityArrayViaImage(sCurrentImageDataUid));
    if (nullptr == pCt2DensityArray)
    {
        TPS_LOG_DEV_ERROR << "GetCT2ReferenceEDensityArrayViaImage failed imageUid" << sCurrentImageDataUid;
        return TPS_ER_FAILURE;
    }

    // step 4 get beamList. if normal group 's beam list is empty, reset the dose grid & return
    std::vector<RtBeam*> vBeams = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(normalGroupUID);
    if (vBeams.empty())
    {
        TPS_LOG_DEV_WARNING << "The beam list to be calculated is empty, reset the dose grid.";
        normGroup->get_dosegrid()->clear_dosegrid_buffer();
        return TPS_ER_FAILURE;
    }

    // step 5. get dosegrid
    RtDosegrid* dosegrid = normGroup->get_dosegrid();
    if (dosegrid == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get dose grid of normal group: " << normalGroupUID;
        return TPS_ER_FAILURE;
    }

    // step 6 CheckInputParameters
    RtDbDef::DoseCalculateMode doseCalculateMode;
    mDataRepository->GetNormgroupManager()->GetDoseCalculateModeViaNgUid(normalGroupUID, doseCalculateMode);
    bool isLockMU = doseCalculateMode == RtDbDef::BASE_ON_MU;
    if (!CheckInputParameters(vBeams, isLockMU))
    {
        const std::string sLog = "CheckInputParameters before DoseCalc failed.";
        TPS_LOG_DEV_ERROR << sLog;
        printf("%s\n", sLog.c_str());
        return TPS_ER_FAILURE;
    }

    // step 7. convert tps data structure to alg data structure
    // beam
    BeamConverter::mDataRepository = mDataRepository;
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOBeam>> beamCarrier;
    BeamConverter::CreateALGBeamList(vBeams, &beamCarrier);
    std::vector<RTFWK::RTFWKDLDOBeam*> algBeamList;
    for (int i = 0; i < beamCarrier.size(); ++i)
    {
        algBeamList.push_back(beamCarrier[i].get());
    }
    // voi
    size_t dim[3];
    image3D->GetDimension(dim);
    Mcsf::Matrix4f matV2P;
    image3D->GetVolumeToPatientMatrix(matV2P);
    auto matP2V = TpsArithmeticConverter::ConvertToMatrix4x4(matV2P.Inverse());
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOVOI>> voiCarrier;
    VoiConverter::CreateALGVoiList(voiList, &voiCarrier);
    std::vector<RTFWK::RTFWKDLDOVOI*> algVoiList;
    ROIEntity* roi = nullptr;
    for (int i = 0; i < voiCarrier.size(); ++i)
    {
        RTFWK::RTFWKDLDOVOI* algVoi = voiCarrier[i].get();
        roi = mDataRepository->GetVoiManager()->GetROI(algVoi->GetVOIUID());
        auto subVolume = new McsfGeometry::SubVolume();
        ROIShapeConverter::SDFToSubVolume(*roi->GetSDF(), matP2V, dim, *subVolume);

        algVoi->SetReferenceSubVolume(subVolume);
        algVoiList.push_back(algVoi);
        TPS_DEL_PTR(subVolume);
    }

    // poi
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOPOI>> poiCarrier;
    PoiConverter::CreateALGPoiList(tpsPoiList, &poiCarrier);
    std::vector<RTFWK::RTFWKDLDOPOI*> algPoiList;
    for (int i = 0; i < poiCarrier.size(); ++i)
    {
        algPoiList.push_back(poiCarrier[i].get());
    }
    // alg type
    //TODO：该参数已经可以删除 等算法组更新接口 需要把以下代码删除
    RTFWK::DOSEALGORITHM_TYPE algType;
    if (vBeams.size() != 0)
    {
        algType = (RTFWK::DOSEALGORITHM_TYPE)vBeams[0]->get_dosealgorithmtype();
    }
    // norm group
    std::shared_ptr<RTFWK::RTFWKDLDONormgroup> normgroupCarrier;
    NormGroupConverter::ConvertToALG(*normGroup, &normgroupCarrier);
    RTFWK::RTFWKDLDONormgroup* algNormgroup = normgroupCarrier.get();
    // dose grid
    RTFWK::RTFWKDLDODoseGrid* algDoseGrid = nullptr;
    std::shared_ptr<RTFWK::RTFWKDLDODoseGrid> doseGridCarrier;
    DosegridConverter::ConvertToALG(*dosegrid, &doseGridCarrier);
    algDoseGrid = doseGridCarrier.get();
    // image  
    std::shared_ptr<RTFWK::RTFWKDLDOImage3D> image3dCarrier;
    Image3dConverter::ConvertToALG(*image3D, &image3dCarrier);
    RTFWK::RTFWKDLDOImage3D* algImage3d = image3dCarrier.get();


    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;

    ADD_WORKFLOW_KEYPOINT << "[Alg][CalculatePlanDose]: "
        << "Alg Dose Calculate of normalgroup start: [" << normalGroupUID << "]";

    int iRet = -1;
    std::map<std::string, std::map<std::string, float>> mBeamDoseContributionPerMuToPOI;
    iRet = mDoseCalculationManager->CalculatePlanDose(
        algVoiList, 
        algBeamList,
        algType,
        algImage3d,
        pCt2DensityArray,
        algNormgroup,
        algPoiList,
        algDoseGrid,
        isLockMU,
        nullptr,
        mBeamDoseContributionPerMuToPOI);
    int iTpsErrorCode = -1;
    if (iRet != (int)ALG_OK)
    {
        iTpsErrorCode = EnumConverter::AlgConvertToTPS((ALG_RETURN_TYPE)iRet);
        ADD_WORKFLOW_KEYPOINT << "[Alg][CalculatePlanDose]: "
            << "Dose calculation failed with error code" << iRet;
        return iTpsErrorCode;
    }

    TEST_PERFORMANCE_END("[ALG][DoseCalculationManager][CalculatePlanDose]");

    ADD_WORKFLOW_KEYPOINT << "[Alg][CalculatePlanDose]: " 
        << "Alg Dose Calculate of normalgroup end: [" << normalGroupUID << "]";

    // step 8 convert alg data structure to tps data structure
    // CX:更新POI点剂量
    if (!PoiConverter::ConvertToTPSPoiList(algPoiList, mBeamDoseContributionPerMuToPOI, &tpsPoiList))
    {
        TPS_LOG_DEV_ERROR << "Failed to convert alg poi to rt poi!";
        TPS_PRINTF("Failed to convert alg poi to rt poi!");
    }
    // CX:更新Beam的MU，Beam下所有Segment的MU,WEIGHT
    if (!BeamConverter::ConvertRange(algBeamList, vBeams))
    {
        TPS_LOG_DEV_ERROR << "Failed to convert alg beam to rt beam!";
        TPS_PRINTF("Failed to convert alg beam to rt beam!");
    }
    // QA Need Update NormGroup
    if (isLockMU && !NormGroupConverter::ConvertToTPS(*algNormgroup, normGroup))
    {
        TPS_LOG_DEV_ERROR << "Failed to convert alg normGroup to rt normGroup!";
        TPS_PRINTF("Failed to convert alg normGroup to rt normGroup!");
    }
    // CX：更新Dosegrid
    if (!DosegridConverter::ConvertToTPS(*algDoseGrid, dosegrid))
    {
        TPS_LOG_DEV_ERROR << "Failed to convert alg dosegrid to rt dosegrid!";
        TPS_PRINTF("Failed to convert alg dosegrid to rt dosegrid!");
    }

    // check valid dose grid
    if (!CheckValidDosegrid(*dosegrid))
    {
        TPS_LOG_DEV_WARNING << "CheckValidDosegrid failed!";
    }

    TPS_LOG_DEV_INFO << "[INFO][TpsDoseProxy][CalculateDoseForNormgroup]: "
        << "Calculate Dose of normalgroup end: [" << normalGroupUID << "]";

    return TPS_ER_SUCCESS;
}

//the value in dosegrid should >= 0
bool TpsDoseProxy::CheckValidDosegrid(RtDosegrid& dosegrid) const
{
    bool bRet = false;
    const int dataCnt = dosegrid.get_xcount() * dosegrid.get_ycount() * dosegrid.get_zcount();
    const float* pDoseBuffer = dosegrid.get_dosegrid_buffer();
    if (nullptr == pDoseBuffer)
    {
        printf("\nget_dosegridBuffer is null\n");
        TPS_LOG_DEV_ERROR << "get_dosegridBuffer is null.";
        return bRet;
    }

    float fMinDose = 1000.f;
    float fMaxDose = 0.f;
    int iMaxDose = 0;
    //for test
    const float fDvh[4] = {0.f, 250.f, 500.f, 1000.f};
    int iCount[4] = {0, 0, 0, 0};
    for (int g = 0; g < dataCnt; ++g)
    {
        if (pDoseBuffer[g] < 0)
        {
            printf("\ndose value in get_dosegridBuffer < 0 : %.6f\n at %d\n", pDoseBuffer[g], g);
            TPS_LOG_DEV_ERROR << "dose value in get_dosegridBuffer < 0 : " << pDoseBuffer[g];
            return false;
        }
        if (pDoseBuffer[g] < fMinDose)
        {
            fMinDose = pDoseBuffer[g];
        }
        if (pDoseBuffer[g] > fMaxDose)
        {
            fMaxDose = pDoseBuffer[g];
            iMaxDose = g;
        }
        if (pDoseBuffer[g] < fDvh[1])
        {
            ++iCount[0];
        }
        else if (pDoseBuffer[g] < fDvh[2])
        {
            ++iCount[1];
        }
        else if (pDoseBuffer[g] < fDvh[3])
        {
            ++iCount[2];
        }
        else 
        {
            ++iCount[3];
        }
    }
    printf("\n");
    for (int x = 0; x < 4; ++x)
    {
        printf("DoseGrid %.1f\t\t [%d]\n", fDvh[x], iCount[x]);
    }

    printf("\nCheckValidDosegrid:\nminDose:%f,maxDose:%f at %d \n", fMinDose, fMaxDose, iMaxDose);
    if (fMaxDose < 1e-6)
    { 
        printf("\nCheckValidDosegrid failed as the max dose is 0\n");
        TPS_LOG_DEV_ERROR << "CheckValidDosegrid failed as the max dose is 0";
        return bRet;
    }

    bRet = true;
    return bRet;
}

bool TpsDoseProxy::CheckInputParameters(const std::vector<RtBeam*> vBeams,bool isLockMU)
{
    float fSumBeamWeight = 0.f;
    for (auto itrBeam = vBeams.begin(); itrBeam != vBeams.end(); ++itrBeam)
    {
        fSumBeamWeight += (*itrBeam)->get_beamweight();
        std::vector<RtBeamsegment*> segments = (*itrBeam)->get_beamsegments();
        for (auto itrSeg = segments.cbegin(); itrSeg != segments.cend(); ++itrSeg)
        {
            const float* mBeam2Pat = (*itrSeg)->get_t_beam_to_pat();
            float fTatal = 0.f;
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    //printf("mBeam2Pat[%d][%d]=%f\n",i, j, mBeam2Pat[j*4+i]);
                    fTatal += mBeam2Pat[j * 4 + i];
                }
                //printf("\n");
            }
            if (fabs(fTatal) < 1e-6)
            {
                TPS_LOG_DEV_ERROR << "CheckInputParameters failed as invalidate Beam2Pat.";
                return false;
            }
        }
    }

    if (!isLockMU)
    {
        if (fSumBeamWeight < 1e-6)
        {
            TPS_LOG_DEV_ERROR << "CheckInputParameters failed as all beam weights is zero!";
            printf("\nCheckInputParameters failed as all beam weights is zero!\n");
            return false;
        }
    }

    return true;
}

void TpsDoseProxy::ReleaseDVHListMemory_i(std::vector<RTFWK::RTFWKDLDODVH*>* dvhList)
{
    if (dvhList == nullptr)
    {
        return;
    }

    for (auto itrDvh = dvhList->begin(); itrDvh != dvhList->end(); ++itrDvh)
    {
        if (nullptr != *itrDvh )
        {
            delete *itrDvh;
            *itrDvh = nullptr;
        }
    }
    dvhList->clear();
}

bool TpsDoseProxy::CalculateDVH(const std::string& planUid)
{
    // clear all DVH realated to this plan
    mDataRepository->GetDVHManager()->DeletePlanDvh(planUid);

    // get pixel volume
    float pixelVolume = 0.0;
    RtPlan* plan = nullptr;
    TpsImage3DEntity* image3D = nullptr;

    if (!mDataRepository->GetPatientManager()->GetPlan(planUid, &plan))
    {
        TPS_LOG_DEV_ERROR << "The Plan does not exist, uid: " << planUid;
        return false;
    }
    if (nullptr == plan)
    {
        TPS_LOG_DEV_ERROR << "plan instance is null, uid: " << planUid;
        return false;
    }
    std::string seriesUid = plan->get_imageseriesuid();
    mDataRepository->GetImageDataManager()->GetImage3DData(seriesUid, &image3D);
    if (nullptr == image3D)
    {
        TPS_LOG_DEV_ERROR << "there is no image for series: " << seriesUid;
        return false;
    }

    RtImage3DHeader* header = nullptr;
    if (!image3D->GetImage3DHeader(header) || header==nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get header info from image3d object: " << seriesUid;
        return false;
    }
    pixelVolume = header->m_dXSize * header->m_dYSize * header->m_fSliceSpacing;

    // plan dose grid
    RtDosegrid* dosegrid = mDataRepository->GetDoseManager()->GetDoseGrid(planUid, DOSE_GRID_PLAN);

    if (dosegrid == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get dose grid of plan: " << planUid;
        return false;
    }
    // if dose grid is un-calculated
    if (!dosegrid->get_isdosevalid())
    {
        TPS_LOG_DEV_INFO << "Dose grid is invalid, return;";
        return true;
    }

    float maxDose = dosegrid->get_max();
    //find a appropriate bind width for histogram
    float bindWidth = floor(maxDose / 500);
    if (bindWidth <= 1.f) bindWidth = 1.f;

    // voi dvh
    std::vector<RtVoi*> voiList;
    std::vector<RTFWK::RTFWKDLDODVH*> dvhList;
    auto vecVoiUids = mDataRepository->GetVoiManager()->GetROIUidListBySeries(seriesUid);

    TpsVOIManager *voiManager = mDataRepository->GetVoiManager();
    for (int n = 0; n < vecVoiUids.size(); ++n)
    {
        std::string voiUid = vecVoiUids[n];
        auto roi = voiManager->GetROI(voiUid);
        if (roi == nullptr)
        {
            TPS_LOG_DEV_ERROR << " voi[" << voiUid << "] dose not exit in voi manager.";
            return false;
        }

        voiList.push_back(roi->GetInnerRoi());

        // new DVH
        RTFWK::RTFWKDLDODVH* dvh = new RTFWK::RTFWKDLDODVH();
        dvh->mBinWidth = bindWidth;
        dvh->mVoiUID = voiUid;
        dvh->mMinDose=0.f;
        dvh->mMaxDose=0.f;
        dvh->mMeanDose =0.f;
        dvh->m_fVolume = 0.f;

        voiManager->SetVOIDoseInfo(dvh->mVoiUID, dvh->mMinDose, dvh->mMaxDose, dvh->mMeanDose);
        dvhList.push_back(dvh);
    }

    // voi
    size_t dim[3];
    image3D->GetDimension(dim);
    Mcsf::Matrix4f matV2P;
    image3D->GetVolumeToPatientMatrix(matV2P);
    auto matP2V = TpsArithmeticConverter::ConvertToMatrix4x4(matV2P.Inverse());
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOVOI>> voiCarrier;
    VoiConverter::CreateALGVoiList(voiList, &voiCarrier);
    std::vector<RTFWK::RTFWKDLDOVOI*> algVoiList;
    ROIEntity* roi = nullptr;
    for (int i = 0; i < voiCarrier.size(); ++i)
    {
        RTFWK::RTFWKDLDOVOI* algVoi = voiCarrier[i].get();
        roi = mDataRepository->GetVoiManager()->GetROI(algVoi->GetVOIUID());
        auto subVolume = new McsfGeometry::SubVolume();
        ROIShapeConverter::SDFToSubVolume(*roi->GetSDF(), matP2V, dim, *subVolume);

        algVoi->SetReferenceSubVolume(subVolume);
        algVoiList.push_back(algVoi);
        TPS_DEL_PTR(subVolume);
    }

    // dose grid
    RTFWK::RTFWKDLDODoseGrid* algDoseGrid = nullptr;
    std::shared_ptr<RTFWK::RTFWKDLDODoseGrid> doseGridCarrier;
    DosegridConverter::ConvertToALG(*dosegrid, &doseGridCarrier);
    algDoseGrid = doseGridCarrier.get();

    //added by qqzhou@20151126 max dose should be meaningful
    if (dosegrid->get_max() > 10000)
    {
        // SGXQ: 多次出现大于情况
        TPS_LOG_DEV_ERROR << "[ALG] plan uid " << planUid 
            << ", Dosegrid max value is too larger, the value is :" << dosegrid->get_max();
    }

    // calculate the dvh based on input voi uids
    if (!dvhList.empty() && !voiList.empty())
    {
        TEST_PERFORMANCE_INIT;
        TEST_PERFORMANCE_BEGIN;

        ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeDVHList]: " 
            << "Alg compute plan dvh start: [" << planUid << "]";

        int iRet = mDoseCalculationManager->ComputeDVHList(
            algVoiList,
            dvhList,
            algDoseGrid,
            pixelVolume);
        TEST_PERFORMANCE_END("[ALG][DoseCalculationManager][ComputeDVHList]");
        if (0 != iRet)
        {
            ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeDVHList]: "
                << "Alg compute plan dvh failed with error code" << iRet;
            ReleaseDVHListMemory_i(&dvhList);
            return false;
        }

        ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeDVHList]: " 
            << "Alg compute plan dvh end: [" << planUid << "]";
    }

    // update the dvh data in dvh manager
    try
    {
        // step 1. build empty dvh based on Alg output
        std::vector<RtDvh*> vDvh;
        std::vector<RTFWK::RTFWKDLDODVH*>::iterator itDvh = dvhList.begin();
        for (; itDvh != dvhList.end(); ++itDvh)
        {
            RtDvh* dvh = new RtDvh();
            dvh->set_uid((*itDvh)->mVoiUID);
            vDvh.push_back(dvh);
        }

        // step 2. convert Alg dvh to Tps dvh
        DVHConverter::ConvertToTPSDvhList(dvhList, mDataRepository, &vDvh);

        // step 3. update dvh mgr
        mDataRepository->GetDVHManager()->SetPlanDvhs(planUid, vDvh);
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "exception happened:" << ex.what();
        ReleaseDVHListMemory_i(&dvhList);
        return false;
    }

    ReleaseDVHListMemory_i(&dvhList);
    return true;
}

bool TpsDoseProxy::CalculateNormGroupDVH(
    const std::string& normGroupUid, 
    const std::vector<std::string>& voiUidList,
    std::vector<RtDvh*>& tpsDVHList)
{
    // get pixel volume
    float pixelVolume = 0.0;

    if (!GetPixelVolumeByNormalgroupUid_i(normGroupUid, pixelVolume))
    {
        TPS_LOG_DEV_ERROR << "GetPixelVolumeByNormalgroupUid_i, uid: " << normGroupUid;
        return false;
    }

    // normGroup dose grid
    RtDosegrid* dosegrid = mDataRepository->GetDoseManager()->GetDoseGrid(normGroupUid, DOSE_GRID_NORMGROUP);
    if (dosegrid == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get dose grid of normGroup: " << normGroupUid;
        return false;
    }

    // To alg dose grid
    RTFWK::RTFWKDLDODoseGrid* algDoseGrid = nullptr;
    std::shared_ptr<RTFWK::RTFWKDLDODoseGrid> doseGridCarrier;
    DosegridConverter::ConvertToALG(*dosegrid, &doseGridCarrier);
    algDoseGrid = doseGridCarrier.get();

    // if dose grid is un-calculated
    if (!dosegrid->get_isdosevalid())
    {
        TPS_LOG_DEV_INFO << "Dose grid is invalid, return;";
        return true;
    }

    // find a appropriate bind width for histogram
    float maxDose = dosegrid->get_max();
    float bindWidth = floor(maxDose / 500);
    if (bindWidth <= 1.f) bindWidth = 1.f;

    // voi dvh
    std::string seriesUid;
    std::vector<RtVoi*> voiList;
    std::vector<RTFWK::RTFWKDLDODVH*> dvhList;
    TpsVOIManager *voiManager = mDataRepository->GetVoiManager();
    for (int n = 0; n < voiUidList.size(); ++n)
    {
        std::string voiUid = voiUidList[n];
        auto roi = voiManager->GetROI(voiUid);
        if (roi == nullptr)
        {
            TPS_LOG_DEV_ERROR << " voi[" << voiUid << "] dose not exit in voi manager.";
            return false;
        }

        voiList.push_back(roi->GetInnerRoi());
        seriesUid = roi->GetInnerRoi()->get_seriesuid();

        // new DVH
        RTFWK::RTFWKDLDODVH* dvh = new RTFWK::RTFWKDLDODVH();
        dvh->mBinWidth = bindWidth;
        dvh->mVoiUID = voiUid;
        dvh->mMinDose=0.f;
        dvh->mMaxDose=0.f;
        dvh->mMeanDose =0.f;
        dvh->m_fVolume = 0.f;
        dvhList.push_back(dvh);
    }

    // voi
    TpsImage3DEntity* image3D = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(seriesUid, &image3D))
    {
        TPS_LOG_DEV_ERROR<<"Failed to find image 3d data.";
        return false;
    }
    size_t dim[3];
    image3D->GetDimension(dim);
    Mcsf::Matrix4f matV2P;
    image3D->GetVolumeToPatientMatrix(matV2P);
    auto matP2V = TpsArithmeticConverter::ConvertToMatrix4x4(matV2P.Inverse());
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOVOI>> voiCarrier;
    VoiConverter::CreateALGVoiList(voiList, &voiCarrier);
    std::vector<RTFWK::RTFWKDLDOVOI*> algVoiList;
    ROIEntity* roi = nullptr;
    for (int i = 0; i < voiCarrier.size(); ++i)
    {
        RTFWK::RTFWKDLDOVOI* algVoi = voiCarrier[i].get();
        roi = mDataRepository->GetVoiManager()->GetROI(algVoi->GetVOIUID());
        auto subVolume = new McsfGeometry::SubVolume();
        ROIShapeConverter::SDFToSubVolume(*roi->GetSDF(), matP2V, dim, *subVolume);

        algVoi->SetReferenceSubVolume(subVolume);
        algVoiList.push_back(algVoi);
        TPS_DEL_PTR(subVolume);
    }

    // calculate the dvh based on input voi uids
    if (!dvhList.empty() && !voiList.empty())
    {
        ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeDVHList]: " 
            << "Alg compute normalgroup dvh start: [" << normGroupUid << "]";

        // output dvhList,将它传到前端去
        int iRet = mDoseCalculationManager->ComputeDVHList(
            algVoiList,
            dvhList,
            algDoseGrid,
            pixelVolume);
        if (0 != iRet)
        {
            ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeDVHList]: " 
                << "Alg compute normalgroup dvh fail with error code: [" << iRet << "]";

            return false;
        }

        ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeDVHList]: "
            << "Alg compute normalgroup dvh end: [" << normGroupUid << "]";
    }

    // update the dvh data in dvh manager
    try
    {
        DVHConverter::ConvertToTPSDvhList(dvhList, mDataRepository, &tpsDVHList);
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "exception happened:" << ex.what();
        ReleaseDVHListMemory_i(&dvhList);
        return false;
    }

    ReleaseDVHListMemory_i(&dvhList);

    return true;
}

bool TpsDoseProxy::ComputeTcpNTcpByNormalGroup(
    const std::string& normalGroupUid, 
    const std::vector<std::string>& voiUidList,
    std::vector<ROIEntity*>& vVois)
{
    // get pixel volume
    float pixelVolume = 0.0;
    if (!GetPixelVolumeByNormalgroupUid_i(normalGroupUid, pixelVolume))
    {
        TPS_LOG_DEV_ERROR << "GetPixelVolumeByNormalgroupUid_i, uid: " << normalGroupUid;
        return false;
    }

    // normGroup dose grid
    RtDosegrid* dosegrid = mDataRepository->GetDoseManager()->GetDoseGrid(normalGroupUid, DOSE_GRID_NORMGROUP);
    if (dosegrid == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get dose grid of normGroup: " << normalGroupUid;
        return false;
    }

    RTFWK::RTFWKDLDODoseGrid* algDoseGrid = nullptr;
    std::shared_ptr<RTFWK::RTFWKDLDODoseGrid> doseGridCarrier;
    DosegridConverter::ConvertToALG(*dosegrid, &doseGridCarrier);
    algDoseGrid = doseGridCarrier.get();
    if (!dosegrid->get_isdosevalid())
    {
        TPS_LOG_DEV_INFO << "Dose grid is invalid, return;";
        return true;
    }

    // find a appropriate bind width for histogram
    float maxDose = dosegrid->get_max();
    float bindWidth = floor(maxDose / 500);
    if (bindWidth <= 1.f) bindWidth = 1.f;

    // voi dvh
    std::vector<RtVoi*> voiList;
    TpsVOIManager *voiManager = mDataRepository->GetVoiManager();
    std::vector<RTFWK::RTFWKDLDODVH*> dvhList;
    std::string seriesUid;
    for (int n = 0; n < voiUidList.size(); ++n)
    {
        std::string voiUid = voiUidList[n];
        auto roi = voiManager->GetROI(voiUid);
        if (roi == nullptr)
        {
            TPS_LOG_DEV_ERROR << " voi[" << voiUid << "] dose not exit in voi manager.";
            return false;
        }

        vVois.push_back(roi);
        voiList.push_back(roi->GetInnerRoi());
        seriesUid = roi->GetInnerRoi()->get_seriesuid();

        // new DVH
        RTFWK::RTFWKDLDODVH* dvh = new RTFWK::RTFWKDLDODVH();
        dvh->mBinWidth = bindWidth;
        dvh->mVoiUID = voiUid;
        dvh->mMinDose=0.f;
        dvh->mMaxDose=0.f;
        dvh->mMeanDose =0.f;
        dvh->m_fVolume = 0.f;
        dvhList.push_back(dvh);
    }


    // 传递bioparam参数
    std::string siteTemplateUid = GetSiteTemplateUidByNormalGroupUid_i(normalGroupUid);

    TpsImage3DEntity* image3D = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(seriesUid, &image3D))
    {
        TPS_LOG_DEV_ERROR<<"Failed to find image 3d data.";
        return false;
    }
    size_t dim[3];
    image3D->GetDimension(dim);
    Mcsf::Matrix4f matV2P;
    image3D->GetVolumeToPatientMatrix(matV2P);
    auto matP2V = TpsArithmeticConverter::ConvertToMatrix4x4(matV2P.Inverse());
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOVOI>> voiCarrier;
    VoiConverter::CreateALGVoiList(voiList, &voiCarrier);
    std::vector<RTFWK::RTFWKDLDOVOI*> algVoiList;
    ROIEntity* roi = nullptr;
    for (int i = 0; i < voiCarrier.size(); ++i)
    {
        RTFWK::RTFWKDLDOVOI* algVoi = voiCarrier[i].get();
        roi = mDataRepository->GetVoiManager()->GetROI(algVoi->GetVOIUID());
        auto subVolume = new McsfGeometry::SubVolume();
        ROIShapeConverter::SDFToSubVolume(*roi->GetSDF(), matP2V, dim, *subVolume);
        algVoi->SetReferenceSubVolume(subVolume);

        RtTcp* tcp = nullptr;
        if (mDataRepository->GetTcpNTcpManager()->GetTcpBySitTemplateUidAndName(siteTemplateUid, algVoi->GetName(), &tcp))
        {
            RTFWK::BioParams bioParam;
            bioParam.fD50 = tcp->get_d50();
            bioParam.fAlphaBeta = tcp->get_alpha();
            bioParam.fGamma = tcp->get_gamma();
            bioParam.fM = tcp->get_m();
            bioParam.fN = tcp->get_n();
            algVoi->SetBioParams(bioParam);
        }

        algVoiList.push_back(algVoi);
        TPS_DEL_PTR(subVolume);
    }

    int iRet = 0;
    // calculate the dvh based on input voi uids
    if (!dvhList.empty() && !vVois.empty())
    {
        ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeDVHList]: "
            << "Alg compute normalgroup dvh start: [" << normalGroupUid << "]";

        iRet = mDoseCalculationManager->ComputeDVHList(
            algVoiList,
            dvhList,
            algDoseGrid,
            pixelVolume);
        if (0 != iRet) 
        {
            ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeDVHList]: " 
                << "Alg compute normalgroup dvh fail with error code: [" << iRet << "]";
            return false;
        }

        ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeDVHList]: "
            << "Alg compute normalgroup dvh end: [" << normalGroupUid << "]";
    }

    // get fraction
    RtNormgroup* normalGroup = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(normalGroupUid, &normalGroup))
    {
        TPS_LOG_DEV_ERROR << "Failed to get normgroup from normgroup manager";
        return false;
    }

    int iFractionCnt = normalGroup->get_fraction();

    ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeBio]: " << "Alg compute bio start:";

    // 计算tcpntcp ，输出m_fTcpNtcp在DVHList中
    iRet = mDoseCalculationManager->ComputeBio(algVoiList, dvhList, iFractionCnt);
    if (0 != iRet)
    {
        ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeBio]: "
            << "Alg compute bio failed with error code" << iRet;
        return false;
    }

    ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeBio]: " << "Alg compute bio end:";

    // update tcpntcp
    if (dvhList.size() > 0)
    {
        for (std::vector<RTFWK::RTFWKDLDODVH*>::iterator it = dvhList.begin(); it != dvhList.end(); ++it) 
        {
            std::string voiUid = (*it)->mVoiUID;
            for (std::vector<ROIEntity*>:: iterator itVoi = vVois.begin(); itVoi != vVois.end(); ++ itVoi)
            {
                RtVoi *voi = (*itVoi)->GetInnerRoi();

                if (voiUid == (voi->get_uid()))
                {
                    voi->set_tcpntcp((*it)->m_fTcpNtcp);
                }
            }
        }
    }

    ReleaseDVHListMemory_i(&dvhList);

    return true;
}

bool TpsDoseProxy::GetPixelVolumeByNormalgroupUid_i(const std::string& normalGroupUid, float& pixelVolume)
{
    RtPlan* plan = nullptr;
    TpsImage3DEntity* image3D = nullptr;

    std::string planUid = mDataRepository->GetNormgroupManager()->GetPlanUIDByNormGroupUID(normalGroupUid);
    if (planUid.empty())
    {
        TPS_LOG_DEV_ERROR << "The Plan does not exist, uid: " << planUid;
        return false;
    }

    if (!mDataRepository->GetPatientManager()->GetPlan(planUid, &plan))
    {
        TPS_LOG_DEV_ERROR << "The Plan does not exist, uid: " << planUid;
        return false;
    }
    if (nullptr == plan)
    {
        TPS_LOG_DEV_ERROR << "plan instance is null, uid: " << planUid;
        return false;
    }
    std::string seriesUid = plan->get_imageseriesuid();
    mDataRepository->GetImageDataManager()->GetImage3DData(seriesUid, &image3D);
    if (nullptr == image3D)
    {
        TPS_LOG_DEV_ERROR << "there is no image for series: " << seriesUid;
        return false;
    }

    RtImage3DHeader* header = nullptr;
    if (!image3D->GetImage3DHeader(header) || header==nullptr) 
    {
        TPS_LOG_DEV_ERROR << "Failed to get header info from image3d object: " << seriesUid;
        return false;
    }
    pixelVolume = header->m_dXSize * header->m_dYSize * header->m_fSliceSpacing;

    return true;
}

std::string TpsDoseProxy::GetSiteTemplateUidByNormalGroupUid_i(const std::string& ngUid)
{
    std::string siteTemplateUid = "";

    RtNormgroup* normalGroup = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(ngUid, &normalGroup))
    {
        TPS_LOG_DEV_ERROR << "Failed to get normgroup from normgroup manager";
        return siteTemplateUid;
    }

    std::string prescriptionUid = normalGroup->get_prescriptionuid();

    RtPrescription* prescription = nullptr;

    if (!mDataRepository->GetPatientManager()->GetPrescriptionByUid(prescriptionUid, &prescription))
    {
        TPS_LOG_DEV_ERROR << "Failed to get pricription from patient manager";
        return siteTemplateUid;
    }

    std::string siteUid = prescription->get_siteuid();

    RtSite* site = nullptr;
    if (!mDataRepository->GetPatientManager()->GetSiteByUid(siteUid, &site))
    {
        TPS_LOG_DEV_ERROR << "Failed to get site from patient manager";
        return siteTemplateUid;
    }

    siteTemplateUid = site->get_sitetemplateuid();

    return siteTemplateUid;


}

bool TpsDoseProxy::ComposeDoseForPlan(const std::string& planUID)
{
    TPS_LOG_DEV_INFO << "[INFO][TpsDoseProxy][ComposeDoseForPlan]: "
        <<"Compose dose of plan: [" << planUID << "]";

    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUID, &plan) || plan == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The plan does not exist, uid: " << planUID;
        return false;
    }

    TpsNormGroupManager *pNormalGroupMgr = mDataRepository->GetNormgroupManager();

    std::vector<std::string> vecNormGroupUid = pNormalGroupMgr->GetNormGroupUIDListByPlan(planUID);
    std::vector<RtDosegrid*> sources;
    std::vector<float> rescaleSlopes;
    sources.reserve(vecNormGroupUid.size());
    rescaleSlopes.reserve(vecNormGroupUid.size());
    RtNormgroup* normGroup = nullptr;
    for (int i = 0; i < vecNormGroupUid.size(); ++i)
    {
        normGroup = nullptr;
        if (!pNormalGroupMgr->GetNormGroup(vecNormGroupUid[i], &normGroup) || normGroup == nullptr)
        {
            TPS_LOG_DEV_ERROR << "Failed to get plan which uid: " << vecNormGroupUid[i];
            return false;
        }
        //无效的dose不参与贡献 wzd
        if (normGroup->get_isjoined() && normGroup->get_dosegrid()->get_isdosevalid())
        {
            sources.push_back(normGroup->get_dosegrid());
            rescaleSlopes.push_back(1.f);
        }
    }
    bool res = this->ComposeDoseGrids_i(sources, rescaleSlopes, plan->get_dosegrid());
    TPS_LOG_DEV_INFO << "[INFO][TpsDoseProxy][ComposeDoseForPlan]: " << "Compose dose leave!";
    return res;
}

bool TpsDoseProxy::ComposeDoseForNormgroup(const std::string& ngUid)
{
    TPS_LOG_DEV_INFO << "[INFO][TpsDoseProxy][ComposeDoseForNormgroup]: "
        << "Compose dose of normal group: [" << ngUid << "]";

    RtNormgroup* normGroup = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(ngUid, &normGroup) || normGroup == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The normal group does not exist, uid: " << ngUid;
        return false;
    }
    int fracCount = normGroup->get_fraction();

    std::vector<std::string> vecBeamUid = mDataRepository->GetBeamManager()->GetBeamUIDListByNormgroupUID(ngUid);
    std::vector<RtDosegrid*> sources;
    std::vector<float> rescaleSlopes;
    sources.reserve(vecBeamUid.size());
    rescaleSlopes.reserve(vecBeamUid.size());
    RtBeam* beam = nullptr;
    for (int i = 0; i < vecBeamUid.size(); ++i)
    {
        beam = mDataRepository->GetBeamManager()->GetBeam(vecBeamUid[i]);
        if (beam == nullptr)
        {
            TPS_LOG_DEV_ERROR << "Failed to get plan which uid: " << vecBeamUid[i];
            return false;
        }
        if (beam->get_isjoincalc() && beam->get_dosegrid()->get_isdosevalid())
        {
            sources.push_back(beam->get_dosegrid());
            rescaleSlopes.push_back(beam->get_beammu() * fracCount);
        }
    }
    bool res = this->ComposeDoseGrids_i(sources, rescaleSlopes, normGroup->get_dosegrid());
    TPS_LOG_DEV_INFO << "[INFO][TpsDoseProxy][ComposeDoseForNormgroup]: " << "Compose dose leave!";
    return res;
}

//todo: will delete after alg provided
bool TpsDoseProxy::ComposeDoseGrids_i(const std::vector<RtDosegrid*>& sources, 
    const std::vector<float>& rescaleSlopes, RtDosegrid* target)
{
    target->set_isdosevalid(false);
    if (sources.size() != rescaleSlopes.size() || target == nullptr)
    {
        TPS_LOG_TRACE_ERROR << "Invalid parameters.";
        return false;
    }

    for (int i = 0; i < sources.size(); ++i)
    {
        if (sources[i] == nullptr)
        {
            TPS_LOG_TRACE_ERROR<<"Empty dose grid.";
            return false;
        }
        if (!CanCompose_i(target, sources[i]))
        {
            TPS_LOG_DEV_ERROR<<"The dose grids of normal group: can't compose.";
            return false;
        }
    }

    int size = target->get_xcount() * target->get_ycount() * target->get_zcount();
    float* gridBuffer = target->get_dosegrid_buffer();
    //reset plan grid buffer
    memset(gridBuffer, 0, sizeof(float) * size);
    target->set_max(0); 
    target->set_min(0);
    if (sources.empty()) return true;

    float* srcGridBuffer = nullptr;
    for (int i = 0; i < sources.size(); ++i)
    {
        srcGridBuffer = sources[i]->get_dosegrid_buffer();
        for (int j = 0; j < size; ++j)
        {
            gridBuffer[j] += srcGridBuffer[j] * rescaleSlopes[i];
        }
    }

    float maxDose = gridBuffer[0];
    float minDose = gridBuffer[0];
    for (int i = 1; i < size; ++i)
    {
        maxDose = TPS_MAX(maxDose, gridBuffer[i]);
        minDose = TPS_MIN(minDose, gridBuffer[i]);
    }

    target->set_max(maxDose);
    target->set_min(minDose);
    target->set_isdosevalid(true);
    return true;
}

bool TpsDoseProxy::RetrieveVoiList_i(const TpsImage3DEntity& image3D, std::vector<RtVoi*>* voiList)
{
    voiList->clear();
    auto voiMng = mDataRepository->GetVoiManager();
    auto roiList = voiMng->GetROIBySeries(image3D.GetUID());
    for (int i = 0; i < roiList.size(); ++i)
    {
        voiList->push_back(roiList[i]->GetInnerRoi());
    }
    return true;
}

int TpsDoseProxy::CheckSeriesSkin_i(const TpsImage3DEntity& image3D)
{
    std::vector<ROIEntity*> roiList = 
        mDataRepository->GetVoiManager()->GetROIBySeries(image3D.GetUID());
    int nSkinNum = 0;
    for (int i = 0; i < roiList.size(); ++i)
    {
        if (roiList[i]->GetInnerRoi()->get_planningrole() == PLANNING_ROLE_EXTERNAL)
        {
            nSkinNum++;
        }
    }
    return nSkinNum;
}

bool TpsDoseProxy::CalculateFluenceMap(const RtBeam* beam, FluenceMapEntity* fluenceMap)
{

    if (beam == nullptr || fluenceMap == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Parameters invalid!";
        return false;
    }

    if (beam->get_trayuid().empty() && 
        (nullptr != beam->get_shield_block() || nullptr != beam->get_aperture_block()))
    {
        TPS_LOG_DEV_ERROR << "block without tray!";
        return false;
    }

    RTALG::THREED_MAP flunceMapAlg;
    std::shared_ptr<RTFWK::RTFWKDLDOBeam> tmsbeam;
    BeamConverter::mDataRepository = mDataRepository;
    BeamConverter::ConvertToALG(*beam, &tmsbeam);

    ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeBeamFluenceMap]: " 
        << "Alg ComputeBeamFluenceMap start: [" << beam->get_uid() << "]";

    ALG_RETURN_TYPE errorCode = (ALG_RETURN_TYPE)mDoseCalculationManager->ComputeBeamFluenceMap(tmsbeam.get(), &flunceMapAlg);
    if (ALG_OK != errorCode)
    {
        TPS_LOG_DEV_ERROR << "Failed to execute ComputeBeamFluenceMap!";
        return false;
    }

    ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeBeamFluenceMap]: " 
        << "Alg ComputeBeamFluenceMap end: [" << beam->get_uid() << "]";

    fluenceMap->SetIntensityMap(flunceMapAlg.m_fx_start, flunceMapAlg.m_fy_start, 
        flunceMapAlg.m_fx_scale, flunceMapAlg.m_fy_scale, flunceMapAlg.m_ix_dim, 
        flunceMapAlg.m_iy_dim, flunceMapAlg.m_fdata);
    return true;
}

bool TpsDoseProxy::UpdateFluenceMap(const RtBeam* beam)
{
    if (beam == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The beam does not exist.";
        return false;
    }
    auto beamMng = mDataRepository->GetBeamManager();
    if (beamMng == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get beanManager";
        return false;
    }
    FluenceMapEntity* fluenceMap = beamMng->GetFluenceMap(beam->get_uid());
    if (fluenceMap == nullptr)
    {
        fluenceMap = new FluenceMapEntity();
        beamMng->AddFluenceMap(beam->get_uid(), fluenceMap);
    }
    if (!this->CalculateFluenceMap(beam, fluenceMap))
    {
        TPS_LOG_DEV_ERROR << "Failed to calculate fluence map.";
        return false;
    }
    return true;
}




bool TpsDoseProxy::MergeDose(RtPlan *primaryPlan, 
    RtPlan *secondPlan,
    int mergeType, 
    const Mcsf::Matrix4f& secondaryToPrimary,
    RtPlan *resultPlan ) 
{
    // rcc: 暂时注掉，等替换dosegrid数据结构的时候再打开
    RtDosegrid *firstDoseGrid = primaryPlan->get_dosegrid();
    RtDosegrid *secondDoseGrid = secondPlan->get_dosegrid();
    RtDosegrid *resultDoseGrid = resultPlan->get_dosegrid();

    RTFWK::RTFWKDLDODoseGrid *algFirstDosegrid = nullptr;
    RTFWK::RTFWKDLDODoseGrid *algSecondDosegrid = nullptr;
    RTFWK::RTFWKDLDODoseGrid *tempDosegrid = nullptr;
    std::shared_ptr<RTFWK::RTFWKDLDODoseGrid> firstDoseGridCarrier;
    std::shared_ptr<RTFWK::RTFWKDLDODoseGrid> secondDoseGridCarrier;
    std::shared_ptr<RTFWK::RTFWKDLDODoseGrid> tempDoseGridCarrier;
    DosegridConverter::ConvertToALG(*resultDoseGrid, &tempDoseGridCarrier);
    DosegridConverter::ConvertToALG(*firstDoseGrid, &firstDoseGridCarrier);
    DosegridConverter::ConvertToALG(*secondDoseGrid, &secondDoseGridCarrier);
    algFirstDosegrid = firstDoseGridCarrier.get();
    algSecondDosegrid = secondDoseGridCarrier.get();
    tempDosegrid = tempDoseGridCarrier.get();

    ADD_WORKFLOW_KEYPOINT << "[Alg][DoseMerge]: " << "Alg Dose Merge start:";
    if (ALG_OK != mDoseCalculationManager->DoseMerge(*algFirstDosegrid, *algSecondDosegrid,
        RTALG::ALG_MERGE_TYPE(mergeType), secondaryToPrimary, tempDosegrid))
    {
        ADD_WORKFLOW_KEYPOINT << "[Alg][DoseMerge]: " << "Alg Dose Merge failed";
        return false;
    }
    ADD_WORKFLOW_KEYPOINT << "[Alg][DoseMerge]: " << "Alg Dose Merge end:";

    // update dosegrid in result plan with tempDosegrid
    DosegridConverter::ConvertToTPS(*tempDosegrid, resultDoseGrid);

    //DoseMerge interface won't update the max/min dose value now, maybe need their further work
    //update max/min value
    float maxDose((numeric_limits<float>::min)()), minDose((numeric_limits<float>::max)());
    float* buffer = resultDoseGrid->get_dosegrid_buffer();
    int count = resultDoseGrid->get_xcount() * resultDoseGrid->get_ycount() * resultDoseGrid->get_zcount();
    for (int i = 0; i < count; ++i)
    {
        if (maxDose < buffer[i]) maxDose = buffer[i];
        if (minDose > buffer[i]) minDose = buffer[i];
    }
    resultDoseGrid->set_max(maxDose);
    resultDoseGrid->set_min(minDose);

    return true;
}


bool TpsDoseProxy::EasyPlanCalculateDose( RtBeam *beam,
    std::vector<RtPoi*> &poiList,
    float prescribeDose,
    int fractionCount,
    std::map<std::string, float>& poiUID2TotalDose)
{
    //Convert TPS Beam to ALG Beam
    std::shared_ptr<RTFWK::RTFWKDLDOBeam> algBeam;
    BeamConverter::mDataRepository = mDataRepository;
    BeamConverter::ConvertToALG(*beam, &algBeam);

    //Convert TPS POI to ALG POI
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOPOI>> poiCarrier;
    PoiConverter::CreateALGPoiList(poiList, &poiCarrier);
    std::vector<RTFWK::RTFWKDLDOPOI*> algPoiList;
    for (auto itr=poiCarrier.cbegin(); itr!=poiCarrier.cend(); ++itr)
    {
        algPoiList.push_back((*itr).get());
    }

    //get ssd
    float ssd = beam->get_ssd();

    ADD_WORKFLOW_KEYPOINT << "[Alg][CalculateEZPlanDose]: " << "Alg ezplan Dose Calculate start:";
    if (ALG_OK != mDoseCalculationManager->CalculateEZPlanDose(algBeam.get(), algPoiList, prescribeDose, ssd))
    {
        ADD_WORKFLOW_KEYPOINT << "[Alg][CalculateEZPlanDose]: " << "Alg ezplan Dose Calculate failed.";
        return false;
    }
    ADD_WORKFLOW_KEYPOINT << "[Alg][CalculateEZPlanDose]: " << "Alg ezplan Dose Calculate end:";
    BeamConverter::ConvertToTPS(*algBeam.get(), beam);

    // Get the relation map of beam's dose contribution to each poi.
    for (auto itPOI = algPoiList.begin(); itPOI != algPoiList.end(); ++itPOI)
    {
        poiUID2TotalDose[(*itPOI)->GetUID()] += (*itPOI)->GetBeamContributions()[0]->GetDose() * fractionCount;
        for (auto itTpsPoi = poiList.begin(); itTpsPoi != poiList.end(); ++itTpsPoi)
        {
            if ((*itTpsPoi)->get_uid() == (*itPOI)->GetUID())
            {
                (*itTpsPoi)->set_poidose(beam->get_uid(), (*itPOI)->GetBeamContributions()[0]->GetDose() * fractionCount);
            }
        }
    }
    return true;
}

bool TpsDoseProxy::CanCompose_i(RtDosegrid* target, RtDosegrid* source)
{
    bool bCanCompose = true;
    bCanCompose &= (target->get_xcoordinate3dstart() == source->get_xcoordinate3dstart());
    bCanCompose &= (target->get_ycoordinate3dinc() == source->get_ycoordinate3dinc());
    bCanCompose &= (target->get_xcount() == source->get_xcount());
    bCanCompose &= (target->get_ycount() == source->get_ycount());
    bCanCompose &= (target->get_zcount() == source->get_zcount());
    bCanCompose &= (target->get_dosegrid_buffer() != nullptr);
    bCanCompose &= (source->get_dosegrid_buffer() != nullptr);
    // CX:去除剂量场无效的条件，目的是允许一个有效的剂量场和一个无效的剂量场叠加
    //bCanCompose &= (source->get_isdosevalid());
    return bCanCompose;
}



std::string ConvertToString(float fNum)
{
    ostringstream oss;
    oss << fNum;
    string str(oss.str());
    return str;
}

bool TpsDoseProxy::ExportSliceDose(RtDosegrid* doseGrid, const std::string& filePath, void* pDoseByte, int size)
{
    if (doseGrid == nullptr || filePath.empty() || pDoseByte == nullptr)
    {
        TPS_LOG_DEV_ERROR << "ExportSliceDose: pDosegrid or sFilePath is empty!";
        return false;
    }

    if (size < 0)
    {
        TPS_LOG_DEV_ERROR << "ExportSliceDose: file length is 0!";
        return false;
    }

    Mcsf::ISimpleDICOMConvertorPtr pConvertor = Mcsf::DICOMConvertorFactory::Instance()->CreateSimpleConvertor();
    Mcsf::IDataHeaderElementMap* dataHeaderPtr = Mcsf::IDataHeaderElementMap::CreateDataHeader();

    if (dataHeaderPtr == nullptr)
    {
        TPS_LOG_DEV_ERROR << "ExportSliceDose: CreateDataHeader failed!";
        return false;
    }

    bool bRet = pConvertor->LoadFile(pDoseByte, size, dataHeaderPtr);
    if (!bRet)
    {
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_LOG_DEV_ERROR << "ExportSliceDose: pConvertor->LoadFile fail!";
        return false;
    }

    // Image Position
    float imagePositionArray[3] =
    {
        doseGrid->get_xcoordinate3dstart(),
        doseGrid->get_ycoordinate3dstart(),
        doseGrid->get_zcoordinate3dstart()
    };

    std::stringstream streamImagePosition;
    streamImagePosition << imagePositionArray[0] << "\\"  << imagePositionArray[1] << "\\"  << imagePositionArray[2];
    std::string strImagePosition = streamImagePosition.str();

    if (!dataHeaderPtr->SetStringByTag(Mcsf::kTagDcm_ImagePositionPatient, strImagePosition.c_str(), static_cast<int>(strImagePosition.length())))
    {
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_LOG_DEV_ERROR << "ExportSliceDose: SetStringByTag image position failed!";
        return false;
    }

    // Image Orientation
    auto matrix = TpsArithmeticConverter::ConvertToMatrix4f(doseGrid->get_grid_to_pat_t());
    Mcsf::Vector3f ortX = matrix.Transform(Mcsf::Vector3f(1, 0, 0));
    Mcsf::Vector3f ortY = matrix.Transform(Mcsf::Vector3f(0, 1, 0));
    ortX.Normalize();
    ortY.Normalize();
    float imageOrientationArray[6] =
    {
        ortX.GetX(), ortX.GetY(), ortX.GetZ(),
        ortY.GetX(), ortY.GetY(), ortY.GetZ()
    };

    std::string strImageOrientation = ConvertToString(imageOrientationArray[0]);
    for (int index = 1; index < 6; index ++)
    {
        strImageOrientation += "\\";
        strImageOrientation += ConvertToString(imageOrientationArray[index]);
    }

    if (!dataHeaderPtr->SetStringByTag(Mcsf::kTagDcm_ImageOrientationPatient, strImageOrientation.c_str(), static_cast<int>(strImageOrientation.length())))
    {
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_LOG_DEV_ERROR << "ExportSliceDose: SetStringByTag image oritentation failed!";
        return false;
    }

    // Pixel Spacing
    float pixelSpacingArray[2] =
    {
        doseGrid->get_xcoordinate3dinc(),
        doseGrid->get_ycoordinate3dinc()
    };
    std::stringstream streamPixelSpacing;
    streamPixelSpacing << pixelSpacingArray[0] << "\\" << pixelSpacingArray[1];
    std::string strPixelSpacing = streamPixelSpacing.str();

    if (!dataHeaderPtr->SetStringByTag(Mcsf::kTagDcm_PixelSpacing, strPixelSpacing.c_str(), static_cast<int>(strPixelSpacing.length())))
    {
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_LOG_DEV_ERROR << "ExportSliceDose: SetStringByTag pixel spacing failed!";
        return false;
    }

    // Offset Vector
    std::string offsetVector = "";
    float zPosition = 0;
    float finZ = doseGrid->get_zcoordinate3dinc();
    int iFrameCount = doseGrid->get_zcount();
    for (int i = 0; i < iFrameCount; i++)
    {
        if (!offsetVector.empty())
            offsetVector += "\\";
        offsetVector += ConvertToString(zPosition);
        zPosition += finZ;
    }
    if (!dataHeaderPtr->SetStringByTag(
        Mcsf::kTagDcm_GridFrameOffsetVector, offsetVector.c_str(), static_cast<int>(offsetVector.length())))
    {
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_LOG_DEV_ERROR << "ExportSliceDose: SetStringByTag offset vector failed!";
        return false;
    }

    // Rows
    if (!dataHeaderPtr->SetUInt16ByTag(
        Mcsf::kTagDcm_Rows, static_cast<Mcsf::UInt16>(doseGrid->get_ycount())))
    {
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_LOG_DEV_ERROR << "ExportSliceDose: SetUInt16ByTag rows failed!";
        return false;
    }

    // Colunmns
    if (!dataHeaderPtr->SetUInt16ByTag(
        Mcsf::kTagDcm_Columns, static_cast<Mcsf::UInt16>(doseGrid->get_xcount())))
    {
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_LOG_DEV_ERROR << "ExportSliceDose: SetUInt16ByTag colunmns failed!";
        return false;
    }

    // Number of Frames
    std::string strZCount = ConvertToString(doseGrid->get_zcount());
    if (!dataHeaderPtr->SetStringByTag(Mcsf::kTagDcm_NumberOfFrames, strZCount.c_str(), static_cast<int>(strZCount.length())))
    {
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_LOG_DEV_ERROR << "ExportSliceDose: SetStringByTag number of Frames failed!";
        return false;
    }

    // Bits Allocated 32
    if (!dataHeaderPtr->SetUInt16ByTag(Mcsf::kTagDcm_BitsAllocated, static_cast<Mcsf::UInt16>(doseGrid->get_bitsallocated())))
    {
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_LOG_DEV_ERROR << "ExportSliceDose: SetUInt16ByTag bits allocated failed!";
        return false;
    }

    // Bits Stored 32
    if (!dataHeaderPtr->SetUInt16ByTag(Mcsf::kTagDcm_BitsStored, static_cast<Mcsf::UInt16>(doseGrid->get_bitsstored())))
    {
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_LOG_DEV_ERROR << "ExportSliceDose: SetUInt16ByTag bits stored failed!";
        return false;
    }

    // High Bit 31
    if (!dataHeaderPtr->SetUInt16ByTag(Mcsf::kTagDcm_HighBit, static_cast<Mcsf::UInt16>(doseGrid->get_highbits())))
    {
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_LOG_DEV_ERROR << "ExportSliceDose: SetUInt16ByTag high bit failed!";
        return false;
    }

    // Dose Grid Scaling
    std::string strAccurancy;
    strAccurancy = ConvertToString(doseGrid->get_accuracy());
    if (!dataHeaderPtr->SetStringByTag(Mcsf::kTagDcm_DoseGridScaling, strAccurancy.c_str(), static_cast<int>(strAccurancy.length())))
    {
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_LOG_DEV_ERROR << "ExportSliceDose: SetStringByTag dose grid scaling failed!";
        return false;
    }

    // Pixel Representation
    if (!dataHeaderPtr->SetUInt16ByTag(Mcsf::kTagDcm_PixelRepresentation, 0))
    {
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_LOG_DEV_ERROR << "ExportSliceDose: SetUInt16ArrayByTag pixel representation failed!";
        return false;
    }

    // Samples per Pixel
    Mcsf::IDataHeaderElementMap::ConstElementPointer elementPointer = nullptr;
    if (!dataHeaderPtr->FindTag(Mcsf::kTagDcm_SamplesPerPixel, &elementPointer))
    {
        if (!dataHeaderPtr->InsertUInt16ValueByTag(Mcsf::kTagDcm_SamplesPerPixel, 1))
        {
            TPS_DEL_PTR(dataHeaderPtr);
            TPS_LOG_DEV_ERROR << "ExportSliceDose: SetUInt16ArrayByTag samples per pixel failed!";
            return false;
        }
    }

    // Photometric Interpretation
    std::string strPhotometricInterpretation = "MONOCHROME2";
    if (!dataHeaderPtr->FindTag(Mcsf::kTagDcm_PhotometricInterpretation, &elementPointer))
    {
        if (!dataHeaderPtr->InsertStringByTag(Mcsf::kTagDcm_PhotometricInterpretation,
            strPhotometricInterpretation.c_str(), static_cast<int>(strPhotometricInterpretation.length())))
        {
            TPS_DEL_PTR(dataHeaderPtr);
            TPS_LOG_DEV_ERROR << "ExportSliceDose: InsertStringByTag photometric interpretation failed!";
            return false;
        }
    }

    // Dose Summation Type
    if (dataHeaderPtr->FindTag(Mcsf::kTagDcm_DoseSummationType, &elementPointer))
    {
        std::string strSummationType = "MULTI_PLAN";
        if (!dataHeaderPtr->SetStringByTag(Mcsf::kTagDcm_DoseSummationType,
            strSummationType.c_str(), static_cast<int>(strSummationType.length())))
        {
            TPS_DEL_PTR(dataHeaderPtr);
            TPS_LOG_DEV_ERROR << "ExportSliceDose: SetStringByTag dose dummation type failed!";
            return false;
        }
    }

    // dose buffer
    float* pDosegridBuffer = doseGrid->get_dosegrid_buffer();
    if (nullptr == pDosegridBuffer)
    {
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_LOG_DEV_ERROR << "ExportSliceDose: pDosegridBuffer is nullptr!";
        return false;
    }
    int iLen = doseGrid->get_xcount() * doseGrid->get_ycount() * doseGrid->get_zcount();
    if (iLen <= 0)
    {
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_LOG_DEV_ERROR << "ExportSliceDose: iLen less than zero!";
        return false;
    }

    const float cAccurancy = 1 / doseGrid->get_accuracy();

    uint32_t* pConverterBuffer = new uint32_t[iLen];
    for (int i = 0; i < iLen; i++)
    {
        //multiply 0.01: cGy to Gy
        pConverterBuffer[i] = static_cast<uint32_t>(pDosegridBuffer[i] * cAccurancy * 0.01);
    }
    if (!dataHeaderPtr->SetUInt16ArrayByTag(Mcsf::kTagDcm_PixelData, 
        reinterpret_cast<Mcsf::UInt16*>(pConverterBuffer), iLen * sizeof(uint32_t)/sizeof(Mcsf::UInt16)))
    {
        TPS_LOG_DEV_ERROR << "ExportSliceDose: SetUInt16ArrayByTag dose buffer failed!";
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_DEL_ARRAY(pConverterBuffer);
        return false;
    }

    // save file
    if (!pConvertor->SaveFile(dataHeaderPtr, filePath))
    {
        TPS_LOG_DEV_ERROR << "ExportSliceDose: SaveFile failed!";
        TPS_DEL_PTR(dataHeaderPtr);
        TPS_DEL_ARRAY(pConverterBuffer);
        return false;
    }

    TPS_DEL_PTR(dataHeaderPtr);
    TPS_DEL_ARRAY(pConverterBuffer);

    return true;
}


int TpsDoseProxy::RescaleDosegrid(
    std::vector<RtDosegrid*>&          vSourceGrids,
    std::vector<float>&                vRescaleFactor,
    RtDosegrid*                        pResultGrid)
{
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDODoseGrid>> dosegridCarrier;
    DosegridConverter::CreateALGDosegridList(vSourceGrids, &dosegridCarrier);
    std::vector<RTFWK::RTFWKDLDODoseGrid*> algDosegridList;
    for (int i = 0; i < dosegridCarrier.size(); ++i)
    {
        algDosegridList.push_back(dosegridCarrier[i].get());
    }

    RTFWK::RTFWKDLDODoseGrid* algDoseGrid = nullptr;
    std::shared_ptr<RTFWK::RTFWKDLDODoseGrid> doseGridCarrier;
    DosegridConverter::ConvertToALG(*pResultGrid, &doseGridCarrier);
    algDoseGrid = doseGridCarrier.get();

    int iRet = mDoseCalculationManager->ComposeDoseGrids(algDosegridList, vRescaleFactor, algDoseGrid);

    if (iRet != (int)ALG_OK)
    {
        ADD_WORKFLOW_KEYPOINT << "[Alg][ComposeDoseGrids]: "
            << "ComposeDoseGrids failed with error code" << iRet;
        return iRet;
    }

    DosegridConverter::ConvertToTPS(*algDoseGrid, pResultGrid);

    return TPS_ER_SUCCESS;
}

int TpsDoseProxy::CalculateBeamMU(
    const TpsImage3DEntity& image3D, 
    const RtNormgroup* normalgroup, 
    ROIEntity* pVoiEntity, 
    std::vector<RtBeam*>* vBeamList)
{
    if (normalgroup == nullptr || pVoiEntity == nullptr || vBeamList->size() < 1)
    {
        return TPS_ER_FAILURE;
    }

    std::shared_ptr<RTFWK::RTFWKDLDONormgroup> normgroupCarrier;
    NormGroupConverter::ConvertToALG(*normalgroup, &normgroupCarrier);
    RTFWK::RTFWKDLDONormgroup* algNormgroup = normgroupCarrier.get();

    std::shared_ptr<RTFWK::RTFWKDLDOVOI> voiCarrier;
    VoiConverter::ConvertToALG(*(pVoiEntity->GetInnerRoi()), &voiCarrier);

    size_t dim[3];
    image3D.GetDimension(dim);
    Mcsf::Matrix4f matV2P;
    image3D.GetVolumeToPatientMatrix(matV2P);
    auto matP2V = TpsArithmeticConverter::ConvertToMatrix4x4(matV2P.Inverse());
    auto subVolume = new McsfGeometry::SubVolume();
    ROIShapeConverter::SDFToSubVolume(*pVoiEntity->GetSDF(), matP2V, dim, *subVolume);
    RTFWK::RTFWKDLDOVOI* algVoi = voiCarrier.get();
    algVoi->SetReferenceSubVolume(subVolume);
    TPS_DEL_PTR(subVolume);

    BeamConverter::mDataRepository = mDataRepository;
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOBeam>> beamCarrier;
    BeamConverter::CreateALGBeamList(*vBeamList, &beamCarrier);
    std::vector<RTFWK::RTFWKDLDOBeam*> algBeamList;
    for (int i = 0; i < beamCarrier.size(); ++i)
    {
        algBeamList.push_back(beamCarrier[i].get());
    }

    int iRet = mDoseCalculationManager->CalculateBeamMU(algNormgroup, algVoi, &algBeamList);

    if (iRet != (int)ALG_OK)
    {
        ADD_WORKFLOW_KEYPOINT << "[Alg][CalculateBeamMU]: "
            << "CalculateBeamMU for roi failed with error code" << iRet;
        return iRet;
    }

    BeamConverter::ConvertRange(algBeamList, *vBeamList);

    return TPS_ER_SUCCESS;
}


int TpsDoseProxy::CalculateBeamMU(
    const RtNormgroup* normalgroup, 
    const BeamDose2PoiMap& beamDose2PoiMap, 
    std::vector<RtBeam*>* vBeamList)
{
    if (normalgroup == nullptr || beamDose2PoiMap.size() < 1 || vBeamList->size() < 1)
    {
        return TPS_ER_FAILURE;
    }

    std::shared_ptr<RTFWK::RTFWKDLDONormgroup> normgroupCarrier;
    NormGroupConverter::ConvertToALG(*normalgroup, &normgroupCarrier);
    const RTFWK::RTFWKDLDONormgroup* algNormgroup = normgroupCarrier.get();

    BeamConverter::mDataRepository = mDataRepository;
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOBeam>> beamCarrier;
    BeamConverter::CreateALGBeamList(*vBeamList, &beamCarrier);
    std::vector<RTFWK::RTFWKDLDOBeam*> algBeamList;
    for (int i = 0; i < beamCarrier.size(); ++i)
    {
        algBeamList.push_back(beamCarrier[i].get());
    }

    int iRet = mDoseCalculationManager->CalculateBeamMU(algNormgroup, beamDose2PoiMap, &algBeamList);

    if (iRet != (int)ALG_OK)
    {
        ADD_WORKFLOW_KEYPOINT << "[Alg][CalculateBeamMU]: "
            << "CalculateBeamMU for poi failed with error code" << iRet;
        return iRet;
    }

    BeamConverter::ConvertRange(algBeamList, *vBeamList);


    return TPS_ER_SUCCESS;
}

int TpsDoseProxy::ComputeBeamWeight(
    const TpsImage3DEntity& image3D, 
    RtNormgroup* normalgroup, 
    ROIEntity* pVoiEntity, 
    std::vector<RtBeam*>* vBeamList)
{
    if (normalgroup == nullptr || pVoiEntity == nullptr || vBeamList->size() < 1)
    {
        return TPS_ER_FAILURE;
    }

    std::shared_ptr<RTFWK::RTFWKDLDONormgroup> normgroupCarrier;
    NormGroupConverter::ConvertToALG(*normalgroup, &normgroupCarrier);
    RTFWK::RTFWKDLDONormgroup* algNormgroup = normgroupCarrier.get();

    std::shared_ptr<RTFWK::RTFWKDLDOVOI> voiCarrier;
    VoiConverter::ConvertToALG(*(pVoiEntity->GetInnerRoi()), &voiCarrier);

    size_t dim[3];
    image3D.GetDimension(dim);
    Mcsf::Matrix4f matV2P;
    image3D.GetVolumeToPatientMatrix(matV2P);
    auto matP2V = TpsArithmeticConverter::ConvertToMatrix4x4(matV2P.Inverse());
    auto subVolume = new McsfGeometry::SubVolume();
    ROIShapeConverter::SDFToSubVolume(*pVoiEntity->GetSDF(), matP2V, dim, *subVolume);
    RTFWK::RTFWKDLDOVOI* algVoi = voiCarrier.get();
    algVoi->SetReferenceSubVolume(subVolume);
    TPS_DEL_PTR(subVolume);

    BeamConverter::mDataRepository = mDataRepository;
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOBeam>> beamCarrier;
    BeamConverter::CreateALGBeamList(*vBeamList, &beamCarrier);
    std::vector<RTFWK::RTFWKDLDOBeam*> algBeamList;
    for (int i = 0; i < beamCarrier.size(); ++i)
    {
        algBeamList.push_back(beamCarrier[i].get());
    }

    int iRet = mDoseCalculationManager->ComputeBeamWeight(algNormgroup, algVoi, &algBeamList);

    if (iRet != (int)ALG_OK)
    {
        ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeBeamWeight]: "
            << "ComputeBeamWeight for roi failed with error code" << iRet;
        return iRet;
    }

    BeamConverter::ConvertRange(algBeamList, *vBeamList);

    NormGroupConverter::ConvertToTPS(*algNormgroup, normalgroup);

    return TPS_ER_SUCCESS;
}

int TpsDoseProxy::ComputeBeamWeight(
    RtNormgroup* normalgroup, 
    const BeamDose2PoiMap& beamDose2PoiMap, 
    std::vector<RtBeam*>* vBeamList)
{
    if (normalgroup == nullptr || beamDose2PoiMap.size() < 1 || vBeamList->size() < 1)
    {
        return TPS_ER_FAILURE;
    }

    std::shared_ptr<RTFWK::RTFWKDLDONormgroup> normgroupCarrier;
    NormGroupConverter::ConvertToALG(*normalgroup, &normgroupCarrier);
    RTFWK::RTFWKDLDONormgroup* algNormgroup = normgroupCarrier.get();

    BeamConverter::mDataRepository = mDataRepository;
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOBeam>> beamCarrier;
    BeamConverter::CreateALGBeamList(*vBeamList, &beamCarrier);
    std::vector<RTFWK::RTFWKDLDOBeam*> algBeamList;
    for (int i = 0; i < beamCarrier.size(); ++i)
    {
        algBeamList.push_back(beamCarrier[i].get());
    }

    int iRet = mDoseCalculationManager->ComputeBeamWeight(algNormgroup, beamDose2PoiMap, &algBeamList);

    if (iRet != (int)ALG_OK)
    {
        ADD_WORKFLOW_KEYPOINT << "[Alg][ComputeBeamWeight]: "
            << "ComputeBeamWeight for poi failed with error code" << iRet;
        return iRet;
    }

    BeamConverter::ConvertRange(algBeamList, *vBeamList);
    NormGroupConverter::ConvertToTPS(*algNormgroup, normalgroup);
    return TPS_ER_SUCCESS;
}


TPS_END_NAMESPACE
