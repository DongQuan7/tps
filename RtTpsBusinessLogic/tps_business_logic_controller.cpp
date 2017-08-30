////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_business_logic_controller.cpp
/// 
///  \brief class TpsBLComponentsManager implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/09/28
////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RtTpsBusinessLogic/tps_business_logic_controller.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_helper.h"

// Boost
#include <memory.h>
#include <algorithm>
#include <functional>
#include <boost/archive/text_oarchive.hpp>
#include <boost/date_time/gregorian/formatters.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/thread.hpp>

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "RtTpsAlgorithmProxy/tps_pl_datastructure.h"

#include "RtTpsBusinessLogic/tps_bl_cmd_start_registration.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsBusinessLogic/tps_bl_load_objects.h"
#include "RtTpsBusinessLogic/tps_bl_opt_observer.h" 

#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_datarepositoryfactory.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_fusionsecondarydatamanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_dvhmanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_hot_cold_spots_manager.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_series_group4d.h"
#include "RtTpsDataAccess/tps_da_optgoalmanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_configuremanager.h"
#include "RtTpsDataAccess/tps_da_machineconfigmanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"
#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"
#include "RtTpsDataAccess/tps_da_gallery_patientmanager.h"
#include "RtTpsDataAccess/tps_da_tablemanager.h"
#include "RtTpsDataAccess/tps_da_tcpntcpmanager.h"


#include "RtTpsRenderController/tps_rc_renderproxy.h"
#include "RtTpsRenderController/tps_rc_mpr_render_controller.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_factory.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_template.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dvh.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_template.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsDoseProxy/tps_dp_doseproxy.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsFramework/tps_fw_renderobserver.h"
#include "RtTpsFramework/tps_fw_rendercontrollerbase.h"
#include "RtTpsFramework/tps_fw_displaycache.h"
#include "RtTpsFramework/tps_fw_datasourcegroup.h"
#include "RtTpsFramework/tps_fw_doselinedatastructure.h"
#include "RtTpsFramework/tps_fw_voistructure.h"
#include "RtTpsFramework/tps_fw_beamstructure.h"
#include "RtTpsFramework/tps_fw_optimizer.h"
#include "RtTpsFramework/tps_fw_voiredoundocommandbase.h"

#include "RtTpsBusinessLogic/tps_bl_dosecalculationimpl.h"

#include "RtTpsCertifiedLib/rt_tps_certified_registation_feedback.h"

#include "RtTpsRenderLibrary/tps_rl_volume4d_change_current_operation.h"
#include "RtTpsRenderLibrary/tps_rl_setrulervisibility_operation.h"

#include "RtTpsFramework/tps_fw_commandmanager.h"
#include "RtTpsDoseProxy/tps_dp_optproxy.h"
#include "RtTpsCertifiedLib/rt_tps_doseoptimizationparam.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedplan.h"
#include "RtTpsProtoLib/rt_tps_doselinesetting.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedvoi.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedmlceditinfo.h"
#include "RtTpsCertifiedLib/rt_tps_cert_cornertext_bev.h"
#include "RtTpsCertifiedLib/rt_tps_cert_cornertext_mpr.h"
#include "RtTpsCertifiedLib/rt_tps_cert_cornertext_mpr_gallery.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedautovois.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_auto_contouring.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_create_beam.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_easyplan_nudge_aperture.h"
#include "RtTpsProtoLib/rt_tps_proto_beam.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_beam_operate.pb.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedgridsize.h"

#include "RtTpsFramework/tps_fw_renderproxybase.h"

#include "alg/alg_commission.h"

#include "tps_bl_mprprinter.h"

#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsAlgorithmProxy/tps_pl_ct2density_helper.h"

#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include <stdio.h>
#ifdef _WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
#include "RtTpsDoseProxy/tps_dp_algcommissionproxy.h"
//#include "RtTpsDoseProxy/table_converter.h"
#include "RtTpsAlgorithmProxy/tps_ap_voi_helper.h"

//#include "VLD/vld.h"
//#pragma comment(lib,"../../../../UIH/lib_debug/external/vld.lib")


TPS_BEGIN_NAMESPACE   // begin namespace tps

    TpsBLController::TpsBLController()
    : mInitialized(false) , mRenderProxy(nullptr)
    , mDataRepository(nullptr) , mDatabaseWrapper(nullptr)
    , mDataRepositoryFactory(nullptr), mAlgCommissionProxy(nullptr)
    ,mMPRPrinter(nullptr){
}

TpsBLController::~TpsBLController()
{
    Finalize();
}

bool TpsBLController::ConfigLog()
{

    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
    {
        TPS_PRINTF("GetCurrentDir failed.\n");
        return false;
    }
    std::string sRootPath(cCurrentPath);
    sRootPath = sRootPath.substr(0,sRootPath.find_last_of("\\"));

    std::string sTpsLog = "appdata/rt/tps/config/rt_tps_logger_be.xml";
    sTpsLog = sRootPath + "/" + sTpsLog;
    //TPS_PRINTF("GetCurrentDir:%s\n",sTpsLog.c_str());

    char* buffer = nullptr;
    std::ifstream ifs;// (sFilePath, std::ifstream::binary);
    ifs.open(sTpsLog, std::ios::in | std::ios::binary);
    size_t filesize = 0;
    if (!ifs)
    {
        TPS_PRINTF("Failed to open file %s\n", sTpsLog.c_str());
        return false;
    }

    // get pointer to associated buffer object
    std::filebuf* pbuf = ifs.rdbuf();
    // get file size using buffer's members
    filesize = pbuf->pubseekoff (0,ifs.end,ifs.in);
    if (filesize > 0)
    {
        pbuf->pubseekpos (0,ifs.in);
        // allocate memory to contain file data
        buffer=new char[filesize+1];
        buffer[filesize] = 0;
        // get file data
        pbuf->sgetn (buffer,filesize);
    }
    ifs.close();

    std::string logxml(buffer);
    //TPS_PRINTF("logxml:%s\n",logxml.c_str());
    DEL_ARRAY(buffer);

    std::string fp_start("<LOG_TRACE>");
    std::string fp_end("</LOG_TRACE>");
    size_t found_start = logxml.find(fp_start);
    size_t found_end = logxml.find(fp_end);
    std::string sValue;
    if (found_start != std::string::npos &&
        found_end != std::string::npos)
    {
        sValue = logxml.substr(found_start + fp_start.length(), found_end - found_start - fp_start.length());
    }
    if (sValue == "On")
    {
        LOG_TRACE_INFO_ON();
    }
    else
    {
        LOG_TRACE_INFO_OFF();
    }

    //LOG_LOAD_CONFIG(sTpsLog);//for mcsf log
    LOG_SET_SOURCE("RT_TPS_BE");
    return true;
}

bool TpsBLController::Initialize()
{
    if (mInitialized) return true;

    InitPlayMovie_i();

    // config log
    if (!ConfigLog())
    {
        TPS_PRINTF("TpsBLController::Initialize config log failed.\n");
        return false;
    }

    bool bRes = InitDatabaseWrapper_i() && 
        InitDataRepository_i()  && 
        InitRenderModule_i()    && 
        InitDoseProxy_i()       && 
        InitFreeType_i()&&
        InitDoseCalculationImpl_i() &&
        InitDoseOpt_i();
    if (bRes)
    {
        mInitialized = true;
        TPS_LOG_DEV_INFO<<"----TpsBLController::Initialize OK.----";
        printf("\n----TpsBLController::Initialize OK.----\n");
        LoadMachineSetting();
        mAlgCommissionProxy = new AlgCommissionProxy();
    }
    else
    {
        TPS_LOG_DEV_ERROR<<"----TpsBLController::Initialize failed!!----";
        printf("\n----TpsBLController::Initialize failed!!----\n");
    }

    mLoadObjects->LoadTcpNTcpParameter();

    return bRes;
}

void TpsBLController::Finalize()
{
    if (!mInitialized) return;

    if (mDataRepository != nullptr)
    {
        mDataRepository->Dispose();
        delete mDataRepository;
        mDataRepository = nullptr;
    }

    if (mDataRepositoryForPlanReport != nullptr)
    {
        mDataRepositoryForPlanReport->Dispose();
        delete mDataRepositoryForPlanReport;
        mDataRepositoryForPlanReport = nullptr;
    }

    if (mDataRepositoryFactory != nullptr)
    {
        delete mDataRepositoryFactory;
        mDataRepositoryFactory = nullptr;
    }

    if (mDatabaseWrapper != nullptr)
    {
        delete mDatabaseWrapper;
        mDatabaseWrapper = nullptr;
    }

    if (mRenderProxy != nullptr)
    {
        mRenderProxy->Finalize();
        delete mRenderProxy;
        mRenderProxy = nullptr;
    }

    if (mRenderProxyForPlanReport != nullptr)
    {
        mRenderProxyForPlanReport->Finalize();
        delete mRenderProxyForPlanReport;
        mRenderProxyForPlanReport = nullptr;
    }

    if (mDoseCalculator != nullptr)
    {
        mDoseCalculator->Finalize();
        delete mDoseCalculator;
        mDoseCalculator = nullptr;
    }

    if (mdoseCalculatorForPlanReport != nullptr)
    {
        mdoseCalculatorForPlanReport->Finalize();
        delete mdoseCalculatorForPlanReport;
        mdoseCalculatorForPlanReport = nullptr;
    }

    if (mMPRPrinter != nullptr)
    {
        delete mMPRPrinter;
        mMPRPrinter = nullptr;
    }

    DEL_PTR(mCtrlOptObserver);
    DEL_PTR(mDoseProxy);
    DEL_PTR(mDoseProxyForPlanReport);
    DEL_PTR(mOptProxy);
    DEL_PTR(mSaveObjects);
    DEL_PTR(mLoadObjects);
    DEL_PTR(mLoadObjectsForPlanReport);
    DEL_PTR(mAlgCommissionProxy);
    mInitialized = false;

    if (m_mutex != nullptr)
    {
        delete m_mutex;
        m_mutex = nullptr;
    }
    if (m_thread != nullptr)
    {
        delete m_thread;
        m_thread = nullptr;
    }

    if (m_galleryMutex != nullptr)
    {
        delete m_galleryMutex;
        m_galleryMutex = nullptr;
    }
    if (m_galleryThread != nullptr)
    {
        delete m_galleryThread;
        m_galleryThread = nullptr;
    }
}

TpsRenderProxy* TpsBLController::GetRenderProxy(){
    return mRenderProxy;
}

ITpsDataRepository* TpsBLController::GetDataRepository(){
    return mDataRepository;
}

IRtDatabaseWrapper* TpsBLController::GetDatabaseWrapper(){
    return mDatabaseWrapper;
}

void TpsBLController::SetDatabaseWrapper(IRtDatabaseWrapper* dbWrapper){
    mDatabaseWrapper =  dbWrapper;
}

void TpsBLController::SetRedoUndoActiveResponser(const RedoUndoResponserPtr& responser)
{
    if (!mInitialized){
        TPS_LOG_DEV_ERROR<<"The bl controller has not been initialized.";
        return;
    }
    mDataRepository->GetCommandManager()->SetActiveResponser(responser);
}

void TpsBLController::InitPlayMovie_i()
{
    mPlayState = STOP;
    mPlaySpeed = SLOW;
    mPlayMode = THREED;
    mGalleryPlayState = STOP;
    mGalleryPlaySpeed = SLOW;
    mGalleryPlayMode = THREED;

    m_thread = nullptr;
    m_mutex = new boost::mutex;

    m_galleryThread = nullptr;
    m_galleryMutex = new boost::mutex;
}

bool TpsBLController::InitDatabaseWrapper_i()
{
    IDatabaseFactory dbFactory;
    mDatabaseWrapper = dbFactory.CreateDBWrapper();
    if (mDatabaseWrapper == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to create database wrapper.";
        return false;
    }

    if(!mDatabaseWrapper->Initialize())//Initialize
    {
        TPS_LOG_DEV_ERROR<<"Can't connect to database! \n Probably due to DB Version conflict \n";
        return false;
    }
    mSaveObjects = new TpsSaveObjects();
    mSaveObjects->SetDatabaseWrapper(mDatabaseWrapper);

    mLoadObjects = new TpsLoadObjects();
    mLoadObjects->SetDatabaseWrapper(mDatabaseWrapper);

    //For plan report
    mLoadObjectsForPlanReport = new TpsLoadObjects();
    mLoadObjectsForPlanReport->SetDatabaseWrapper(mDatabaseWrapper);
    return true;
}

bool TpsBLController::InitDataRepository_i(){
    mDataRepositoryFactory = new TpsDataRepositoryFactory();
    mDataRepository = mDataRepositoryFactory->CreateDataRepository();
    if (!mDataRepository->Initialize()) {
        TPS_LOG_DEV_ERROR<<"Failed to initialize the data repository.";
        return false;
    }

    mDataRepositoryForPlanReport = mDataRepositoryFactory->CreateDataRepository();
    if (!mDataRepositoryForPlanReport->Initialize())
    {
        //in order not to affect TPS, never return false here
        TPS_LOG_DEV_ERROR<<"Failed to initialize the data repository for plan report.";
    }

    mSaveObjects->SetDataRepository(mDataRepository);
    mLoadObjects->SetDataRepository(mDataRepository);
    mLoadObjects->LoadMachineTree();
    //for plan report
    mLoadObjectsForPlanReport->SetDataRepository(mDataRepositoryForPlanReport);
    mLoadObjectsForPlanReport->LoadMachineTree();

    return true;
}

bool TpsBLController::ClearManagerData_i() {

    // clear patient manager
    mDataRepository->GetPatientManager()->ClearPatientData();

    // clear image data
    mDataRepository->GetImageDataManager()->ClearPatientData();

    // clear voi manager
    mDataRepository->GetVoiManager()->ClearPatientData();

    // clear poi manager
    mDataRepository->GetPoiManager()->ClearPatientData();

    // clear normgroup manager
    mDataRepository->GetNormgroupManager()->ClearPatientData();

    // clear beam manager
    mDataRepository->GetBeamManager()->ClearPatientData();

    // clear hot/cold spot
    mDataRepository->GetHotColdSpotsManager()->ClearPatientData();

    // clear dose manager
    mDataRepository->GetDoseManager()->ClearPatientData();

    // clear dvh manager
    mDataRepository->GetDVHManager()->ClearPatientData();

    // clear machinesettingmanager
    mDataRepository->GetMachineSettingManager()->ClearMachineSettingData();

    // clear table
    mDataRepository->GetTableManager()->ClearPatientData();

    // clear tcpntcp
    mDataRepository->GetTcpNTcpManager()->ClearPatientData();

    // clear optgoal
    mDataRepository->GetOptGoalManager()->ClearPatientData();

    return true;
}

bool TpsBLController::ClearManagerDataForPlanReport_i() {

    // clear patient manager
    mDataRepositoryForPlanReport->GetPatientManager()->ClearPatientData();

    // clear image data
    mDataRepositoryForPlanReport->GetImageDataManager()->ClearPatientData();

    // clear voi manager
    mDataRepositoryForPlanReport->GetVoiManager()->ClearPatientData();

    // clear poi manager
    mDataRepositoryForPlanReport->GetPoiManager()->ClearPatientData();

    // clear normgroup manager
    mDataRepositoryForPlanReport->GetNormgroupManager()->ClearPatientData();

    // clear beam manager
    mDataRepositoryForPlanReport->GetBeamManager()->ClearPatientData();

    // clear hot/cold spot
    mDataRepositoryForPlanReport->GetHotColdSpotsManager()->ClearPatientData();

    // clear dose manager
    mDataRepositoryForPlanReport->GetDoseManager()->ClearPatientData();

    // clear dvh manager
    mDataRepositoryForPlanReport->GetDVHManager()->ClearPatientData();

    mDataRepositoryForPlanReport->GetMachineSettingManager()->ClearMachineSettingData();

    return true;
}

//bool TpsBLController::ClearPatientTreeData() {
//
//    return ClearManagerData_i() && ClearRenderData_i();
//}
//
//bool TpsBLController::ClearRenderData_i() 
//{
//    mRenderProxy->RemoveAllDataSources();
//    mRenderProxy->GetModelWarehouse()->ClearAll();
//}

bool TpsBLController::InitRenderModule_i(){
    //for plan report
    //in order not to effect TPS, do not return false
    mRenderProxyForPlanReport = new TpsRenderProxy();
    mRenderProxyForPlanReport->Initialize(mDataRepositoryForPlanReport);

    mRenderProxy = new TpsRenderProxy();
    return mRenderProxy->Initialize(mDataRepository);
}

bool TpsBLController::InitDoseProxy_i(){
    mDoseProxy = new TpsDoseProxy();
    mLoadObjects->SetDoseProxy(mDoseProxy);
    //for plan report
    mDoseProxyForPlanReport = new TpsDoseProxy();
    mLoadObjectsForPlanReport->SetDoseProxy(mDoseProxyForPlanReport);
    return true;
}

bool TpsBLController::InitFreeType_i(){
    return true;
}

bool TpsBLController::InitDoseCalculationImpl_i(){
    mDoseCalculator = new DoseCalculationImpl();
    mDoseCalculator->SetDataRepository(mDataRepository);
    mDoseCalculator->SetRenderProxy(mRenderProxy);
    mDoseCalculator->SetSaveCmd(mSaveObjects);
    mDoseCalculator->SetDoseProxy(mDoseProxy);

    mLoadObjects->SetDoseCalculation(mDoseCalculator);

    //for plan report
    mdoseCalculatorForPlanReport = new DoseCalculationImpl();
    mdoseCalculatorForPlanReport->SetDataRepository(mDataRepositoryForPlanReport);
    mdoseCalculatorForPlanReport->SetRenderProxy(mRenderProxyForPlanReport);
    mdoseCalculatorForPlanReport->SetSaveCmd(mSaveObjects);
    mdoseCalculatorForPlanReport->SetDoseProxy(mDoseProxyForPlanReport);
    mLoadObjectsForPlanReport->SetDoseCalculation(mdoseCalculatorForPlanReport);

    bool bRet = mDoseCalculator->Initialize();

    //for plan report
    if(!mdoseCalculatorForPlanReport->Initialize()){
        //in order not to affect TPS, never return false here
        TPS_LOG_DEV_ERROR<<"Failed to initialize the dose calculator for plan report.";
    }
    return bRet;
}

bool TpsBLController::InitDoseOpt_i(){
    mOptProxy = new TpsOptProxy();
    mOptProxy->SetDataRepository(mDataRepository);
    mCtrlOptObserver = new TpsPlanningCtrlOptObserver();
    mCtrlOptObserver->SetSaveCmd(mSaveObjects);
    mCtrlOptObserver->SetDataRepository(mDataRepository);
    mCtrlOptObserver->SetDoseCalcImpl(mDoseCalculator);
    mOptProxy->SetObserver(*mCtrlOptObserver);
    return mOptProxy->Initialize();
}

int TpsBLController::ProcessCommand(TpsCommand* cmd)
{
    if (!mInitialized) {
        TPS_LOG_DEV_ERROR<<"The BLController has not been initialized.";
        return TPS_ER_FAILURE; 
    }
    if (cmd == nullptr) return TPS_ER_INPUT_INVALID;

    cmd->SetDataRepository(mDataRepository);
    cmd->SetRenderProxy(mRenderProxy);
    cmd->SetDatabaseWrapper(mDatabaseWrapper);
    cmd->SetSaveObjects(mSaveObjects);
    cmd->SetLoadObjects(mLoadObjects);
    cmd->SetDoseProxy(mDoseProxy);
    cmd->SetDoseCalculator(mDoseCalculator);
    return cmd->RunCommand();
}

TPS_ERROR_CODES TpsBLController::CanSwitchToTps(const std::string& patientUid, int targetModule)
{
    int count = 0;
    int code = mDatabaseWrapper->GetSeriesCountByPatientUid(patientUid, &count);
    if (code == DB_CODES_FK_UID_NOT_EXIST)
    {
        return TPS_ER_PATIENT_NOT_EXIST;
    }
    else if (code != DB_CODES_SUCCESS)
    {
        return TPS_ER_DB_ERROR;
    }
    else if (targetModule != EASYPLAN && count == 0)
    {
        return TPS_ER_NO_SERIES_IN_PATIENT;
    }
    else
    {
        return TPS_ER_SUCCESS;
    }
}

TPS_ERROR_CODES TpsBLController::SaveTableAsROI(const std::string& /*seriesUid*/,
	const std::string& /*tableUid*/,const std::vector<float>& /*voicolor*/, const std::string & /*name*/, ROIEntity *&/*tableVoi*/)
{
	//if (seriesUid.empty() || tableUid.empty())
	//{
	//	TPS_LOG_DEV_ERROR << "Parameters error!";
	//	return TPS_ER_FAILURE;
	//}

	////check image 3d
	//TpsImage3DEntity* image3D = nullptr;
	//if (!mDataRepository->GetImageDataManager()->GetImage3DData(
	//	seriesUid, &image3D) || image3D == nullptr)
	//{
	//	TPS_LOG_DEV_ERROR << "The image 3d does not exist;";
	//	return TPS_ER_FAILURE;
	//}
	////check series table
	//auto tblMng = mDataRepository->GetTableManager();
	//auto seriesTable = tblMng->GetSeriesTable(seriesUid);
	//if (seriesTable == nullptr)
	//{
	//	TPS_LOG_DEV_ERROR << "The series table does not exist.";
	//	return TPS_ER_FAILURE;
	//}
	////check rt table if table uid is not empty
	//auto rtTable = tblMng->GetRtTable(tableUid);
	//if (!tableUid.empty() && rtTable == nullptr)
	//{
	//	TPS_LOG_DEV_ERROR << "The RtTable does not exist.";
	//	return TPS_ER_FAILURE;
	//}

	//RtImage3DHeader* header = nullptr;
	//if (!image3D->GetImage3DHeader(header) || header == nullptr)
	//{
	//	TPS_LOG_DEV_ERROR << "Failed to get image header of image: " << seriesUid;
	//	return TPS_ER_FAILURE;
	//}

	//tableVoi = new VOIEntity(new RtVoi(true));
	//
	//TpsVoiHelper::ConvertSeriesTableToVoi(*rtTable, *image3D, *seriesTable, tableVoi);

	//if (voicolor.size() != 4)
	//{
	//	DEL_PTR(tableVoi);
	//	TPS_LOG_DEV_ERROR << "Voi Color set error!";
	//	return TPS_ER_FAILURE;
	//}

	//tableVoi->SetColor((float *)(&voicolor[0]));
	//tableVoi->SetName(name);

	//RtCt2density* pCt2Density = mDataRepository->GetImageDataManager()->GetCT2DensityTableByImageUid(seriesUid);
	//if (!SubVolumeHelperAPI::UpdateVOIStatisticalInformation(pCt2Density, *image3D, tableVoi))
	//{
	//	DEL_PTR(tableVoi);
	//	TPS_LOG_DEV_ERROR << "Failed to update statistical information.";
	//	return TPS_ER_FAILURE;

	//}

	//std::string voiuid = tableVoi->GetVoiUid();
	////for latest requirement: save voi with contours.
	//if (TPS_ER_SUCCESS != mSaveObjects->SaveVoi(*tableVoi)){

	//	DEL_PTR(tableVoi);
	//	TPS_LOG_DEV_ERROR << "Failed to save voi: " << voiuid << " to database.";
	//	return TPS_ER_FAILURE;
	//}

	//if (!mDataRepository->GetVoiManager()->AddVOI(tableVoi))
	//{
	//	DEL_PTR(tableVoi);
	//	TPS_LOG_DEV_ERROR << "Save ROI In Manager failed!";
	//	return TPS_ER_FAILURE;
	//}

	////add voi to manager
	//if (!mRenderProxy->AddVOI(voiuid, image3D->GetUID())){
	//	DEL_PTR(tableVoi);
	//	TPS_LOG_DEV_ERROR << "Failed to add voi: " << voiuid << " to render proxy.";
	//	return TPS_ER_FAILURE;
	//}

	//mRenderProxy->Render();
	return TPS_ER_SUCCESS;
}

int TpsBLController::GetRelatedPatientCountByCt2DensityUid(const std::string& ct2DensityUid, std::vector<std::string>& relatedPatientNameList)
{
    int code = mDatabaseWrapper->GetRelatedPatientCountByCt2DensityUid(ct2DensityUid,relatedPatientNameList);
    return code;
}

bool TpsBLController::CreateCoreViewModule(const std::string& wndUid, 
    int windowType, int width, int height, const WindowResponserPtr& activeResponser)
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    if(nullptr == mRenderProxy)
    {
        TPS_LOG_DEV_ERROR << "mRenderProxy is null.";
        return false;
    }

    if (!mRenderProxy->CheckFBO()){
        TPS_LOG_DEV_ERROR << "CheckFBO failed.";
        return false;
    }

    DISPLAY_SIZE disSize;
    disSize.height = height;
    disSize.width = width;
    windowType = windowType;

    std::shared_ptr<RenderObserver> renderObserver(new RenderObserver());
    renderObserver->AttachResponser(activeResponser);
    mRenderProxy->CreateViewModule(wndUid, (WINDOW_TYPE)windowType, disSize, renderObserver);

    TpsRenderControllerBase* renderController = mRenderProxy->GetRenderController(
        (WINDOW_TYPE)windowType, wndUid);
    //int cacheId = renderController->GetDisplayCache()->GetCacheID();
    renderObserver->SetDisplayCache(renderController->GetDisplayCache());
    TEST_PERFORMANCE_END("BE TpsBLController::CreateCoreViewModule");
    return true;
}

int TpsBLController::DestoryCoreViewModule(WINDOW_TYPE windowType, const std::string& windowUid) {

    mRenderProxy->RemoveRenderController(windowType, windowUid);

    return TPS_ER_SUCCESS;
}

void TpsBLController::ActiveWindow(const std::string& wndUid, int wndType)
{
    mRenderProxy->ActiveWindow(wndUid, (WINDOW_TYPE)wndType);
    mRenderProxy->Render();
}

void TpsBLController::DeactiveWindow(const std::string& wndUid, int wndType)
{
    mRenderProxy->DeactiveWindow(wndUid, (WINDOW_TYPE)wndType);
}

bool TpsBLController::SwitchSeries(int groupType, 
    const std::string& strTargetUid, const std::string& floatUID)
{
    auto img_mgr = mDataRepository->GetImageDataManager();
    if(!img_mgr->IsSeriesExists(strTargetUid)){
        TPS_LOG_DEV_ERROR<<"Image3D does not exist.";
        return false;
    }

    //if the series belongs to a 4D CT group, set the series as main phase
    std::shared_ptr<TpsSeriesGroup4d> seriesGroup = img_mgr->GetSeriesGroup4d(strTargetUid);
    if (seriesGroup != nullptr)
    {
        img_mgr->SetMainPhase(seriesGroup->m_strGroupUid, strTargetUid);
        SaveMainPhaseToDBSeriesGroup(seriesGroup->m_strGroupUid, strTargetUid);
    }

    auto group = mRenderProxy->RequestDataSourceGroup((DATA_SOURCE_GROUP_ID)groupType);
    if (group == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to request common source group.";
        return false;
    }
    group->SetDataSource(IMAGE, strTargetUid);

    //set float image
    std::string currentFloatUID = "";
    if (!floatUID.empty() && img_mgr->IsSeriesExists(floatUID)){
        group->SetDataSource(FLOAT_IMAGE, floatUID);
    }
    else if (group->GetDataSource(FLOAT_IMAGE, currentFloatUID)){
        group->RemoveDataSource(FLOAT_IMAGE);
    }

    //fire
    mRenderProxy->Render();
    return true;
}

bool TpsBLController::SaveMainPhaseToDBSeriesGroup(const std::string& SeriesGroupUid, const std::string& MainPhaseUid)
{
    RtSeriesgroup* pSeriesGroup = nullptr;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->GetSeriesgroupByUid(SeriesGroupUid, pSeriesGroup)) 
    {
        return false;
    }

    pSeriesGroup->set_mainseriesuid(MainPhaseUid);
    if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateSeriesgroup(*pSeriesGroup)) 
    {
        return false;
    }
    return true;
}

bool TpsBLController::RemoveDataSource(DATA_SOURCE_GROUP_ID dataSourceGroupID, DATA_SOURCE_ID dataSourceToRemove) {
    // if target image series is null, then clear the medviewers.
    auto group = mRenderProxy->RequestDataSourceGroup(dataSourceGroupID);
    if (group == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to request common source group.";
        return false;
    }
    group->RemoveDataSource(dataSourceToRemove);
    //fire render
    mRenderProxy->Render();
    return true;
}

int TpsBLController::ChangePlanGoalSelection(int groupType, const std::string& uid, PLAN_GOAL_SELECTION_LEVEL level)
{
    if (!mRenderProxy->ChangePlanGoalSelection((DATA_SOURCE_GROUP_ID)groupType, level, uid))
    {
        TPS_LOG_DEV_ERROR<<"Failed to change plan goal selection.";
        return TPS_ER_FAILURE;
    }

    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}



int TpsBLController::UpdateCrosshair(
    WINDOW_TYPE windowType, const std::string& windowUid, 
    double xStart, double yStart, double xEnd, double yEnd, int crossMode, int hitMode, double rotateX, double rotateY) {

        mRenderProxy->UpdateMPRCornerInformation(windowType, windowUid, xEnd, yEnd);
        mRenderProxy->UpdateCrosshair(windowType, windowUid, xStart, yStart, xEnd, yEnd, crossMode, hitMode, rotateX, rotateY);
        mRenderProxy->Render();
        return 0;
}

//根据windowType 和windowuid 来重置MPR底图
bool TpsBLController::ResetMPRObject(
    WINDOW_TYPE windowType, const std::string& windowUid)
{
    if (!mRenderProxy->ResetMPRObject(windowType, windowUid)) {
        TPS_LOG_DEV_ERROR << "Failed to reset mpr.";
        return false;
    }
    else {
        mRenderProxy->Render();
        return true;
    }
}


bool TpsBLController::SetCrosshairHittedMode(int wndType, const std::string& wndUid, int mode){
    bool bRes = mRenderProxy->SetCrosshairHittedMode(wndType, wndUid, mode);
    if (bRes) mRenderProxy->Render();
    return bRes;
}

int TpsBLController::RotateShadedSurface(const std::string& seriesUID, 
    float prePtX, float prePtY, float curPtX, float curPtY){

        mRenderProxy->RotateShadedSurface(seriesUID, 
            Mcsf::Point2f(prePtX, prePtY), Mcsf::Point2f(curPtX, curPtY));
        mRenderProxy->Render();
        return 0;
}

int TpsBLController::ShadedSurfaceZoom(const std::string& seriesUID, float fZoomFactor){
    mRenderProxy->ShadedSurfaceZoom(seriesUID, fZoomFactor);
    mRenderProxy->Render();
    return 0;
}

int TpsBLController::Pan3DWindow(WINDOW_TYPE window, const std::string& wndUid, 
    float startx, float starty, float stopx, float stopy)
{
    mRenderProxy->Pan3DWindow(window, wndUid, startx, starty, stopx, stopy);
    mRenderProxy->Render();
    return 0;
}

bool TpsBLController::TransformPoint(WINDOW_TYPE window, const std::string& wndUid, 
    float ptInX, float ptInY, float* ptOutX, float* ptOutY, float* ptOutZ){

        Mcsf::Point2f ptIn(ptInX, ptInY);
        Mcsf::Point3f ptOut;
        if (!mRenderProxy->TransformPoint(window, wndUid, ptIn, ptOut)){
            TPS_LOG_DEV_ERROR<<"Failed to transform point.";
            return false;
        }
        *ptOutX = ptOut.GetX();
        *ptOutY = ptOut.GetY();
        *ptOutZ = ptOut.GetZ();
        return true;
}

bool TpsBLController::TransformPoint(WINDOW_TYPE window, const std::string& wndUid, 
    float ptInX, float ptInY, float ptInZ, float* ptOutX, float* ptOutY){

        Mcsf::Point3f ptIn(ptInX, ptInY, ptInZ);
        Mcsf::Point2f ptOut;
        if (!mRenderProxy->TransformPoint(window, wndUid, ptIn, ptOut)){
            TPS_LOG_DEV_ERROR<<"Failed to transform point.";
            return false;
        }
        *ptOutX = ptOut.GetX();
        *ptOutY = ptOut.GetY();
        return true;
}

bool TpsBLController::GetCrosshairProjectionPosition(WINDOW_TYPE windowType, 
    const std::string& windowUid, float& x2D, float& y2D)
{
    if (!mRenderProxy->GetCrosshairProjectionPosition(windowType, windowUid,
        x2D, y2D)){
            TPS_LOG_DEV_ERROR<<"Failed to GetCrosshairProjectionPosition.";
            return false;
    }
    return true;
}

bool TpsBLController::GetCrossXYPoints(WINDOW_TYPE windowType,
    const std::string& windowUid, double& x1, double& y1, double& x2, double& y2, double& x3, double& y3, double& x4, double& y4)
{
    if (!mRenderProxy->GetCrossXYPoints(
        (WINDOW_TYPE)windowType, windowUid, x1, y1, x2, y2, x3, y3, x4, y4)){
            TPS_LOG_DEV_ERROR << "Failed to GetCrosshairProjectionPosition.";
            return false;
    }
    return true;
}

bool TpsBLController::GetMprViewDirecrion(WINDOW_TYPE windowType, const std::string& windowUid,
    double& x1, double& y1, double& z1, double& x2, double& y2, double& z2) {
        if (!mRenderProxy->GetMprViewDirecrion(windowType, windowUid, x1, y1, z1, x2, y2, z2)) {
            TPS_LOG_DEV_ERROR << "Failed to GetMprViewDirecrion.";
            return false;
        }
        return true;

}


double TpsBLController::DistancePointToPlane(WINDOW_TYPE windowType, 
    const std::string& windowUid, float x3D, float y3D, float z3D) {

        return mRenderProxy->DistancePointToPlane(windowType, windowUid, x3D, y3D, z3D);
}

void TpsBLController::RefreshAll(){
    mRenderProxy->Render(true);
}

int TpsBLController::CalculateDVH(const std::string& planUID)
{
    TpsNormGroupManager& ngMgr = *mDataRepository->GetNormgroupManager();

    RtNormgroup* pNormgroup = nullptr;

    std::vector<std::string> normGroupUID = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(planUID);

    if (normGroupUID.size() == 0 || !ngMgr.GetNormGroup(normGroupUID[0], &pNormgroup) || pNormgroup == nullptr) {
        TPS_LOG_DEV_ERROR << "The plan does not exist.";
        return TPS_ER_FAILURE;
    }

    //std::string planUID = mDataRepository->GetNormgroupManager()->GetPlanUIDByNormGroupUID(normGroupUID);


    if (!mDoseCalculator->CalculateDvhAndUpdateVoiDose(planUID)){
        TPS_LOG_DEV_ERROR<<"Failed to calculate dvh of plan: "<<planUID;
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int TpsBLController::CalculateNormGroupDVH(const std::string& normGroupUid,const std::vector<std::string>& voiUidList,std::vector<RtDvh*>& tpsDVHList)
{

    if (!mDoseCalculator->CalculateNormGroupDvh(normGroupUid, voiUidList,tpsDVHList)){
        TPS_LOG_DEV_ERROR<<"Failed to calculate dvh of normGroup: "<<normGroupUid;
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int TpsBLController::CalculateBioParams(const std::string& normGroupUid,
    const std::vector<std::string>& voiUidList,
    std::vector<ROIEntity*>& vVois)
{
    if (!mDoseCalculator->CalculateBioParams(normGroupUid, voiUidList, vVois))
    {
        TPS_LOG_DEV_ERROR << "Failed to calculate BioParams of normGroup: " << normGroupUid;
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

//For plan report
int TpsBLController::CalculateDVHForPlanReport(const std::string& planUID)
{
    TpsNormGroupManager& ngMgr = *mDataRepositoryForPlanReport->GetNormgroupManager();

    RtNormgroup* pNormgroup = nullptr;

    std::vector<std::string> normGroupUID = mDataRepositoryForPlanReport->GetNormgroupManager()->GetNormGroupUIDListByPlan(planUID);

    if (normGroupUID.size() == 0||!ngMgr.GetNormGroup(normGroupUID[0], &pNormgroup) || pNormgroup == nullptr) {
        TPS_LOG_DEV_ERROR<<"The plan does not exist.";
        return TPS_ER_FAILURE;
    }

    if (!mdoseCalculatorForPlanReport->CalculateDvhAndUpdateVoiDose(planUID)){
        TPS_LOG_DEV_ERROR<<"Failed to calculate dvh of plan: "<<planUID;
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

bool TpsBLController::MapMilimeter2Pixel(int wndType, const std::string& wndUid,
    double lengthInmm, double* pixelinXNP, double* pixelinYNP){

        return mRenderProxy->MapMilimeter2Pixel(wndType, 
            wndUid, lengthInmm, pixelinXNP, pixelinYNP);
}

TPS_ERROR_CODES TpsBLController::GetPatient2ViewMat(const WINDOW_TYPE& wndType, const std::string& wndUid, Mcsf::MedViewer3D::Matrix4x4& patient2ViewMat)
{
    return mRenderProxy->GetPatient2ViewMat(wndType, wndUid, patient2ViewMat);
}

bool TpsBLController::LoadSystemCT2DensityTables(std::vector<RtCt2density*>& vRtCt2densityList){
    //return mDataRepository->GetImageDataManager()->LoadSystemCT2DensityTables(vRtCt2densityList);
    if(!mLoadObjects->LoadSystemCT2DensityTables()){
        TPS_LOG_DEV_ERROR<<"Failed to LoadSystemCT2DensityTables.";
        return false;
    }
    mDataRepository->GetImageDataManager()->GetAllCT2DensityMap(vRtCt2densityList);
    return true;
}

int TpsBLController::AddCT2DensityTable(RtCt2density& ct2densityTable)
{
    int iRet = mSaveObjects->SaveCt2Density(ct2densityTable);
    if (TPS_ER_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "Failed to Add CT2Density to DB!";
        return iRet;
    }

    if (!mDataRepository->GetImageDataManager()->AddCT2DensityMap(ct2densityTable))
    {
        TPS_LOG_DEV_ERROR << "Failed to Add CT2Density to manager!";
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

bool TpsBLController::AddTemplate(RtTemplate* sTemplate)
{
    if (mSaveObjects->SaveTemplate(sTemplate) != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to add SystemConfig to DB!";
        return false;
    }

    if (!mDataRepository->GetConfigurationManager()->AddSystemConfig(*sTemplate))
    {
        TPS_LOG_DEV_WARNING << "Failed to add SystemConfig to manager!";
        return false;
    }
    return true;
}

bool TpsBLController::UpdateTemplate(RtTemplate* sTemplate)
{
    if (mSaveObjects->UpdateTemplate(sTemplate) != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to update SystemConfig to DB!";
        return false;
    }

    if (!mDataRepository->GetConfigurationManager()->UpdateSystemConfig(*sTemplate))
    {
        TPS_LOG_DEV_WARNING << "Failed to update SystemConfig to manager!";
        return false;
    }

    return true;
}

bool TpsBLController::RemoveTemplate(std::string& templateUid)
{
    if (mSaveObjects->RemoveTemplate(templateUid) != TPS_ER_SUCCESS)
    {
        return false;
    }

    return true;
}


int TpsBLController::RemoveCT2DensityTable(const std::string c2dUid)
{
    int iRet = mSaveObjects->EraseObject(c2dUid, RtDbDef::TYPE_CT2DENSITY);
    if (TPS_ER_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "Failed to Remove CT2Density to DB!";
        return iRet;
    }

    if (!mDataRepository->GetImageDataManager()->RemoveCT2Density(c2dUid))
    {
        TPS_LOG_DEV_ERROR << "Failed to Remove CT2Density to manager!";
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}
int TpsBLController::UpdateCT2DensityTable(RtCt2density& ct2densityTable)
{
    if (nullptr == mDatabaseWrapper)
    {
        return TPS_ER_FAILURE;
    }

    int iRet = mDatabaseWrapper->UpdateCt2densityObject(ct2densityTable);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "Failed to Update CT2Density to DB!";
        return iRet;
    }

    if (!mDataRepository->GetImageDataManager()->UpdateCT2Density(ct2densityTable))
    {
        TPS_LOG_DEV_ERROR << "Failed to Update CT2Density to manager!";
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

/*!\fn TpsBLController::ValidateCT2DensityOfSeries(RtSeries* series)
* <key> \n
* PRA: Yes \n
* Name:bool TpsBLController::ValidateCT2DensityOfSeries(RtSeries* series) \n
* Traced from: 46366 DS_PRA_TPS_UpdateCTToDensity \n
* Description: check whether ct2densitytable data is updated \n
* Short Description: check ct2densitytable data \n
* Component: TPS \n
* </key> \n
*******************************************************************************/
bool TpsBLController::ValidateCT2DensityOfSeries(const std::string& strPatientUid)
{
    //比较ct2density表的更新时间和相关的dose的更新时间，
    //如果ct2density表的时间较新则dosegrid是过期的无效的,需要前端提示
    RtPatient* pPatient = nullptr;
    int iRet = mDatabaseWrapper->GetPatientByUid(strPatientUid, pPatient);
    if (DB_CODES_SUCCESS != iRet) return false;

    std::vector<RtCourse*> vecRtCourses;
    vecRtCourses = mDataRepository->GetPatientManager()->GetAllCourse();
    std::vector<RtCourse*>::iterator itCourse = vecRtCourses.begin();
    for (; itCourse != vecRtCourses.end(); ++itCourse)
    {
        RtCourse* pCourse = *itCourse;
        std::string courseUid = pCourse->get_uid();
        std::vector<std::string> vPlanUid = mDataRepository->GetPatientManager()->GetPlanUIDListByCourseUID(courseUid);

        for (int i = 0; i < vPlanUid.size(); i++)
        {
            RtPlan* plan = nullptr;
            mDataRepository->GetPatientManager()->GetPlan(vPlanUid[i], &plan);
            if (plan == nullptr)
            {
                TPS_LOG_DEV_ERROR << "plan is null when validate CT2DensityOfSeries";
                return false;
            }

            if (plan->get_plansourcetype() != (int)RtDbDef::TreatmentPlan)
            {
                continue;
            }

            std::string seriesUid = mDataRepository->GetPatientManager()->GetSeriesUidByPlanUid(vPlanUid[i]);
            RtSeries *pSeries = nullptr;
            mDataRepository->GetImageDataManager()->GetImageSeries(seriesUid, &pSeries);
            if (pSeries == nullptr)
            {
                TPS_LOG_DEV_ERROR << "series is null when validate CT2DensityOfSeries";
                return false;
            }

            if (pSeries->get_modality() != "CT")
            {
                continue;
            }

            std::string ct2desityUid = pSeries->get_ct2densityuid();
            if (ct2desityUid.empty())
            {
                TPS_LOG_DEV_ERROR << "ct2desityUid is empty. Failed to get_ct2densityuid";
                return false;
            }
            RtCt2density* ct2densityTable = nullptr;
            ct2densityTable = mDataRepository->GetImageDataManager()->GetCT2DensityTableByUid(ct2desityUid);
            if (ct2densityTable == nullptr)
            {
                TPS_LOG_DEV_ERROR << "ct2densityTable is null. Failed to GetCT2DensityTableByUid with uid"
                    << ct2desityUid;
                return false;
            }
            DATETIME_BOOST ct2desityUpdateTime = ct2densityTable->get_updatetime();
            std::string ct2densityUpdateTimeStr(to_simple_string(ct2desityUpdateTime));
            std::vector<RtBeam*> vRtBeamList;
            vRtBeamList = mDataRepository->GetBeamManager()->GetBeamsByPlan(vPlanUid[i]);
            std::vector<RtBeam*>::iterator itBeam = vRtBeamList.begin();
            for (; itBeam != vRtBeamList.end(); ++itBeam)
            {
                RtBeam* pBeam = *itBeam;
                RtDosegrid* pDosegridBeam = pBeam ->get_dosegrid();
                DATETIME_BOOST doseUpdateTime = pDosegridBeam->get_updatetime();
                std::string doseUpdateTimeStr(to_simple_string(doseUpdateTime));
                if (doseUpdateTime < ct2desityUpdateTime)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

bool TpsBLController::CreateOptGoal(RtDoseoptgoalbase& optGoal){
    if (mDataRepository->GetOptGoalManager()->AddOptGoal(optGoal)){
        if (nullptr != mDatabaseWrapper){
            if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertDoseoptgoalbaseObject(optGoal)){
                return false;
            }
        }
    }
    return true;
}

int TpsBLController::CreateOptGoal(const std::vector<RtDoseoptgoalbase*>& vecOptGoal) {
    for (auto it = vecOptGoal.begin(); it != vecOptGoal.end(); ++it)
    {
        if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertDoseoptgoalbaseObject(**it)){
            return TPS_ER_FAILURE;
        }
    }

    for (auto it = vecOptGoal.begin(); it != vecOptGoal.end(); ++it)
    {
        if (!mDataRepository->GetOptGoalManager()->AddOptGoal(**it)){
            return TPS_ER_FAILURE;
        }
    }

    return TPS_ER_SUCCESS;
}


bool TpsBLController::RemoveOptGoal(const std::string optGoalUid){
    if (mDataRepository->GetOptGoalManager()->RemoveOptGoal(optGoalUid)){
        if (TPS_ER_SUCCESS == mSaveObjects->EraseObject(optGoalUid, RtDbDef::TYPE_DOSEOPTGOALBASE))
            return true;
    }
    return false;
}

bool TpsBLController::UpdateOptGoal(RtDoseoptgoalbase& optGoal){
    if (mDataRepository->GetOptGoalManager()->UpdateOptGoal(optGoal)){
        if (nullptr != mDatabaseWrapper){
            if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateDoseoptgoalbaseObject(optGoal)){
                return false;
            }
        }
    }
    return true;
}

#pragma region Fusion Methods

bool TpsBLController::SetFixedImage(const std::string& imageUID, 
    CertifiedRegistrationFeedback& feedback)
{
    auto mng = mDataRepository->GetFusionSecondaryDataManager();
    mng->SetFixedImageUID(imageUID);
    int status = mng->ImagesStatus();
    feedback.mFeedbackType = REGISTRATION_IMAGES_STATUS;
    feedback.mMessage = boost::lexical_cast<std::string>(status);
    if(imageUID.empty()) {
        //remove fixed image
        mRenderProxy->RemoveDataSource(SOURCE_GROUP_FUSION_FIXED, IMAGE);
        mRenderProxy->RemoveDataSource(SOURCE_GROUP_FUSION_MIXED, IMAGE);
        mRenderProxy->Render();
        return true;
    }

    mRenderProxy->SetDataSource(SOURCE_GROUP_FUSION_FIXED, IMAGE, imageUID);
    if (!(status & FLOAT_IMAGE_READY)){
        mRenderProxy->SetDataSource(SOURCE_GROUP_FUSION_MIXED, IMAGE, imageUID);
        mRenderProxy->Render();
        return true;
    }
    //if all ready, start register using default registration matrix.
    StartRegistrationCmd cmd(imageUID, mng->GetFloatImageUID(), INIT_SETUP);
    return this->ProcessCommand(&cmd) == TPS_ER_SUCCESS;
}

bool TpsBLController::SetFloatImage(const std::string& imageUID, 
    CertifiedRegistrationFeedback& feedback)
{
    auto mng = mDataRepository->GetFusionSecondaryDataManager();
    mng->SetFloatImageUID(imageUID);
    int status = mng->ImagesStatus();
    feedback.mFeedbackType = REGISTRATION_IMAGES_STATUS;
    feedback.mMessage = boost::lexical_cast<std::string>(status);
    if(imageUID.empty()) {
        //remove float image
        mRenderProxy->RemoveDataSource(SOURCE_GROUP_FUSION_FLOAT, IMAGE);
        mRenderProxy->RemoveDataSource(SOURCE_GROUP_FUSION_MIXED, FLOAT_IMAGE);
        mRenderProxy->Render();
        return true;
    }

    mRenderProxy->SetDataSource(SOURCE_GROUP_FUSION_FLOAT, IMAGE, imageUID);
    if (!(status & FIXED_IMAGE_READY)){
        mRenderProxy->Render();
        return true;
    }
    //if all ready, start register using default registration matrix.
    StartRegistrationCmd cmd(mng->GetFixedImageUID(), imageUID, INIT_SETUP);
    return this->ProcessCommand(&cmd) == TPS_ER_SUCCESS;
}

void TpsBLController::ChangeFusionType(const std::string& fixedUid, 
    const std::string floatUid, int fusionType)
{
    mRenderProxy->RefreshFusionType(fixedUid, floatUid, fusionType);
    mRenderProxy->Render();
}

void TpsBLController::ChangeFusionFactor(const std::string& fixedUid, 
    const std::string& floatUid, double fusionFactor)
{
    mRenderProxy->RefreshFusionFactor(fixedUid, floatUid, fusionFactor);
    mRenderProxy->Render();
}

void TpsBLController::ChangeFusionType(int fusionType)
{
    mRenderProxy->RefreshFusionType(fusionType);
    mRenderProxy->Render();
}

void TpsBLController::ChangeFusionFactor(double fusionFactor)
{
    mRenderProxy->RefreshFusionFactor(fusionFactor);
    mRenderProxy->Render();
}

void TpsBLController::ChangeRegistrationAlgorithm(int registrationAlgorithm, 
    CertifiedRegistrationFeedback& feedback)
{
    auto fsMng = mDataRepository->GetFusionSecondaryDataManager();

    if (fsMng->IsRegionVisible(FUSION_ROLE_FIXED) || 
        fsMng->IsRegionVisible(FUSION_ROLE_FLOAT)){
            feedback.mFeedbackType = HIDE_REGISTRATION_REGION;
            fsMng->SetRegionVisible(FUSION_ROLE_FIXED, false);
            fsMng->SetRegionVisible(FUSION_ROLE_FLOAT, false);
            mRenderProxy->UpdateFusionRegion();
            mRenderProxy->Render();
    }
    fsMng->SetCurrentRegistrationAlgorithm(
        (REIGISTRATION_ALGORITHM)registrationAlgorithm);
}

bool TpsBLController::DisplayFusionRegion(bool editFixed, bool visible)
{
    auto manager = mDataRepository->GetFusionSecondaryDataManager();
    std::string fixedImageUID = manager->GetFixedImageUID();
    std::string floatImageUID = manager->GetFloatImageUID();
    if (fixedImageUID.empty() || floatImageUID.empty()){
        TPS_LOG_DEV_ERROR<<"Fixed image or float image hasn't set.";
        return false;
    }

    TpsImage3DEntity* fixedImage =nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(
        fixedImageUID, &fixedImage) || fixedImage == nullptr){
            TPS_LOG_DEV_ERROR<<"The fixed image is empty.";
            return false;
    }

    TpsImage3DEntity* floatImage =nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(
        floatImageUID, &floatImage) || floatImage == nullptr){
            TPS_LOG_DEV_ERROR<<"The float image is empty.";
            return false;
    }

    if (visible && !manager->IsRegionReady()){
        RtImage3DHeader& fixedHeader = *fixedImage->GetRtSeries()->get_header();
        RtImage3DHeader& floatHeader = *floatImage->GetRtSeries()->get_header();
        int region[12] = { 0, fixedHeader.m_iXDim-1, 
            0, fixedHeader.m_iYDim-1, 0, fixedHeader.m_iSliceCount-1, 
            0, floatHeader.m_iXDim-1, 0, floatHeader.m_iYDim-1, 
            0, floatHeader.m_iSliceCount-1};
        manager->SetRegion(region);
        manager->SetRegionReady(true);
    }
    manager->SetRegionVisible(editFixed ? FUSION_ROLE_FIXED : FUSION_ROLE_FLOAT, visible);
    mRenderProxy->UpdateFusionRegion(editFixed ? OnlyFixedRegion : OnlyFloatRegion);
    mRenderProxy->Render();
    return true;
}

void TpsBLController::SetPseudoColor(const std::string& imageUid, const std::string& pseudoColorName)
{
    mRenderProxy->SetPseudoColor(imageUid, pseudoColorName);
    mRenderProxy->Render();
}

int TpsBLController::AddLandmarkPoint(WINDOW_TYPE wndType, const std::string& wndUid, 
    double x, double y, int index, double& patX, double& patY,double& patZ, CertifiedRegistrationFeedback& feedback)
{
    bool addToFixed = index <= LANDMARK_POINT_PAIR_COUNT;
    if ((addToFixed && 
        wndType != WINDOW_FUSION_FIXED_MPR_CORONAL && 
        wndType != WINDOW_FUSION_FIXED_MPR_SAGITTAL && 
        wndType != WINDOW_FUSION_FIXED_MPR_AXIAL) || 
        (!addToFixed && 
        wndType != WINDOW_FUSION_FLOAT_MPR_CORONAL && 
        wndType != WINDOW_FUSION_FLOAT_MPR_SAGITTAL &&
        wndType != WINDOW_FUSION_FLOAT_MPR_AXIAL)) {
            TPS_LOG_DEV_WARNING<<"Add fixed landmark in float window, \
                                 or add float landmark in fixed window.";
            return TPS_ER_FAILURE;
    }

    std::string imageUid;
    if (!mRenderProxy->GetDataSource(wndType, IMAGE, imageUid)){
        TPS_LOG_DEV_ERROR<<"Failed to get IMAGE data source uid.";
        return TPS_ER_FAILURE;
    }
    TpsImage3DEntity* image = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(imageUid, &image)){
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d entity.";
        return TPS_ER_FAILURE;
    }
    RtImage3DHeader* header = nullptr;
    image->GetImage3DHeader(header);
    Mcsf::Matrix4f matV2P;
    if (!image->GetVolumeToPatientMatrix(matV2P)){
        TPS_LOG_DEV_ERROR<<"Failed to get volume to patient matrix.";
        return TPS_ER_FAILURE;
    }
    Mcsf::Point3f point;
    if (!mRenderProxy->TransformPoint(wndType, wndUid, Mcsf::Point2f(x, y), point)){
        TPS_LOG_DEV_ERROR<<"Failed to transform point.";
        return TPS_ER_FAILURE;
    }

    patX = point.GetX();
    patY = point.GetY();
    patZ = point.GetZ();

    //one landmark point added, write feedback
    feedback.mFeedbackType = LANDMARK_POINTS_MARKED_FLAG_CHANGED;

    return TPS_ER_SUCCESS;
}

bool TpsBLController::ShowFusionImage(const std::string& fixedSeriesUid, 
    const std::string& floatSeriesUid, bool displayOrHide)
{
    DATA_SOURCE_GROUP_ID groupsSupportFusion[4] = {
        SOURCE_GROUP_COMMON, SOURCE_GROUP_EV_1, 
        SOURCE_GROUP_EV_2, SOURCE_GROUP_EV_RESULT
    };
    //check if the specified series is current displaying series

    //get groups that match fixed series
    std::vector<DATA_SOURCE_GROUP_ID> vecGroups;
    std::string currentImageUid;
    for (int i = 0; i < 4; ++i){
        if(!mRenderProxy->GetDataSource(groupsSupportFusion[i], IMAGE, currentImageUid)) continue;
        if (currentImageUid == fixedSeriesUid) vecGroups.push_back(groupsSupportFusion[i]);
    }
    if (vecGroups.empty()) return true;

    //check if the float image has registered with current image
    auto image3dMng = mDataRepository->GetImageDataManager();
    TpsImage3DEntity* image = nullptr;
    if (!image3dMng->GetImage3DData(fixedSeriesUid, &image) || image == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d data.";
        return false;
    }
    Mcsf::Matrix4f matrix;
    if (!image->GetRegistrationMatrix(floatSeriesUid, matrix)){
        TPS_LOG_DEV_ERROR<<"The specified float image \
                           has no registration relationship with current image";
        return false;
    }

    if (displayOrHide) {
        if (!image3dMng->IsSeriesExists(floatSeriesUid)){
            TPS_LOG_DEV_ERROR<<"The specified float image does not exist.";
            return false;
        }
        for (int i = 0; i < vecGroups.size(); ++i){
            mRenderProxy->SetDataSource(vecGroups[i], FLOAT_IMAGE, floatSeriesUid);
        }
    }
    else {
        for (int i = 0; i < vecGroups.size(); ++i){
            mRenderProxy->RemoveDataSource(vecGroups[i], FLOAT_IMAGE);
        }
    }

    mRenderProxy->Render();
    return true;
}

void TpsBLController::EditChess(WINDOW_TYPE wndType, const std::string& wndUid, 
    double x, double y, int mouseActionType)
{
    mRenderProxy->EditChessBoard(wndType, wndUid, x, y, mouseActionType);
    mRenderProxy->Render();
}
#pragma endregion

bool TpsBLController::SetAdminPhantomImageDataSource(const std::string& phantomSeriesUid)
{
    mRenderProxy->SetDataSource(SOURCE_GROUP_ADMIN, IMAGE, phantomSeriesUid);
    mRenderProxy->Render();
    return true;
}

bool TpsBLController::LoadQASeries(const std::string& qaSeriesUid)
{
    mRenderProxy->SetDataSource(SOURCE_GROUP_PLANQA, IMAGE, qaSeriesUid);
    mRenderProxy->Render();
    return true;
}

int TpsBLController::RescaleDoseForBeamWeight(const std::string ngUid)
{
     RescaleDosePara rescaleDoseParameter;
     rescaleDoseParameter.doseRescaleReasonType = BEAM_WEIGHT_CHANGED;
     rescaleDoseParameter.muRescaleFactor = 1.f;

     return mDoseCalculator->RescaleDose(ngUid, rescaleDoseParameter);
}

int TpsBLController::RescaleDoseForBeamMU(const std::string ngUid)
{
    RescaleDosePara rescaleDoseParameter;
    rescaleDoseParameter.doseRescaleReasonType = BEAM_MU_CHANGED;
    rescaleDoseParameter.muRescaleFactor = 1.f;

    return mDoseCalculator->RescaleDose(ngUid, rescaleDoseParameter);
}

int TpsBLController::UpdateDoseCalclateMode(const std::string& ngUid, int nDoseCalculateMode)
{
    if (!mDataRepository->GetNormgroupManager()->UpdateDoseCalculateMode(
        ngUid, (RtDbDef::DoseCalculateMode)nDoseCalculateMode))
    {
        TPS_LOG_DEV_ERROR << "UpdateDoseCalculateMode fail with nguid." << ngUid;
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int TpsBLController::CalculateDose(const int targetLevel, const std::string uid)
{
    std::vector<RtBeam*> beamList;
    switch (targetLevel)
    {
        case 0:
            // plan
            return mDoseCalculator->CalculateDoseForPlan(uid);
        case 1: 
            // normgroup
            return mDoseCalculator->CalculateDoseForNormgroup(uid);
        default:
            return TPS_ER_FAILURE;
    }
}



int TpsBLController::ClearDose(const int targetLevel, const std::string uid, const bool isSave/* = true */)
{
    int iRet = TPS_ER_FAILURE;
    switch (targetLevel)
    {
    case 0: // plan
        {
            iRet = mDoseCalculator->ClearDoseForPlan(uid, isSave);
        }
        break;
    case 1:
        {
            iRet = mDoseCalculator->ClearDoseForNormgroup(uid, isSave);
        }
        break;
    default:
        TPS_LOG_DEV_ERROR << "Unknown target level for ClearDosegrid cmd.";
        iRet = TPS_ER_FAILURE;
    }

    if (iRet != (int)TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to ClearDose in businesslogic ";
    }
    return iRet;
}

bool TpsBLController::IsDoseValid(const int targetLevel, const std::string uid)
{
    switch(targetLevel) 
    {
    case 0: // plan
        {
            return mDoseCalculator->CheckDoseValidForPlan(uid);
        }
        break;
    case 1: //normalgroup
        {
            return mDoseCalculator->CheckDoseValidForNormalgroup(uid);
        }
        break;
    default:
        TPS_LOG_DEV_ERROR<<"Unknown target level for ClearDosegrid cmd.";
        return false;
    }
}

bool TpsBLController::ReComposeNgDose(const std::string uid)
{
    if (!mDoseCalculator->ReComposeDoseForNormgroup(uid)) {
        TPS_LOG_DEV_ERROR<<"Failed to re-compose dose for normgroup" << uid<<"].";
        return false;
    }
    return true;
}

int TpsBLController::RecomposePlanDose(const std::string& planUID)
{
    return mDoseCalculator->RecomposePlanDose(planUID);
}


int TpsBLController::UpdateNormVoi(const std::string& ngUid)
{
    return mDoseCalculator->UpadteNormalVoi(ngUid);
}

int TpsBLController::DeleteNormVoi(const std::string& ngUid)
{
    return mDoseCalculator->DeleteNormalVoi(ngUid);
}

int TpsBLController::UpdatePlanVoi(const std::string& planUid)
{
    return mDoseCalculator->UpdatePlanVoi(planUid);
}

int TpsBLController::DeletePlanVoi(const std::string& planUid)
{
    return mDoseCalculator->DeletePlanVoi(planUid);
}

bool TpsBLController::GetBeamsInfo(const int targetLevel, const std::string uid, std::vector<RtBeam*>& vBeams){
    if (&vBeams == nullptr){
        TPS_LOG_DEV_ERROR<<"beam vector is null.";
    }
    switch (targetLevel) {
    case 0: 
        {// plan
            vBeams = mDataRepository->GetBeamManager()->GetBeamsByPlan(uid);
        }
        break;
    case 1: 
        {// normgroup
            vBeams = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(uid);
        }
        break;
    default:
        TPS_LOG_DEV_ERROR<< "targetLevel is not correct: " << targetLevel;
        return false;
    }
    return true;
}

bool TpsBLController::GetVoisInfo(const int targetLevel, const std::string uid, std::vector<ROIEntity*>& vVois){
    if (&vVois == nullptr){
        TPS_LOG_DEV_ERROR<<"voi vector is null.";
    }
    
    std::string planUid = GetPlanUid_i(targetLevel, uid);
    std::string imgUid = mDataRepository->GetPatientManager()->GetSeriesUidByPlanUid(planUid);

    vVois = mDataRepository->GetVoiManager()->GetROIBySeries(imgUid);

    std::vector<RtDvh*> vDVHList = mDataRepository->GetDVHManager()->GetPlanDvhs(planUid);
    if (vDVHList.size() > 0) {
        ROIEntity *voi = nullptr;
        for (std::vector<RtDvh*>::iterator it = vDVHList.begin(); it != vDVHList.end(); ++it) {
            std::string voiUid = (*it)->get_uid();
            voi = mDataRepository->GetVoiManager()->GetROI(voiUid);
            if (voi == nullptr) {
                TPS_LOG_DEV_ERROR<<"Failed to get VOI["<<voiUid<<"] from voi manager";
                return false;
            }
            voi->SetMinDose((*it)->get_mindose());
            voi->SetMaxDose((*it)->get_maxdose());
            voi->SetMeanDose((*it)->get_meandose());
            voi->GetInnerRoi()->set_standarddeviation((*it)->get_standarddeviation());
        }
    }

    return true;
}

// 此方法获取的VOI中的Dose信息不准确，如需更新VOI dose，请使用GetVoisInfo
void TpsBLController::GetVoisInfoBySerisesUid(const std::string& seriesUid, std::vector<ROIEntity*>& vVois)
{
    vVois = mDataRepository->GetVoiManager()->GetROIBySeries(seriesUid);
}

bool TpsBLController::GetPoisInfo(const int targetLevel, const std::string uid, std::vector<RtPoi*>& vPois){
    if (&vPois == nullptr){
        TPS_LOG_DEV_ERROR<<"poi vector is null.";
    }
    std::string planUid = GetPlanUid_i(targetLevel, uid);
    std::string seriesUid = mDataRepository->GetPatientManager()->GetSeriesUidByPlanUid(planUid);
    vPois = mDataRepository->GetPoiManager()->GetAllPoiInSeries(seriesUid);

    //Get Plan MaxPoint
    RtPoi* poi = mDataRepository->GetPoiManager()->GetMaxPoint(planUid);
    if (poi != nullptr)
    {
        vPois.push_back(poi);
    }
    return true; 
}

int TpsBLController::GetDefaultDoseGridSize(const std::string seriesUID, CertifiedGridSize& pocoGridSize)
{
    //spacing使用默认值3mm
    DosegridInfo info;
    info.spacingx = 3.0;
    info.spacingy = 3.0;
    info.spacingz = 3.0;

    TpsImage3DEntity* image3DEntity = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(seriesUID, &image3DEntity)){
        TPS_LOG_DEV_ERROR<<"Failed to get the image data.";
        return TPS_ER_FAILURE;
    }

    ROIEntity* skinVoi = mDataRepository->GetVoiManager()->GetExternal(seriesUID);

    RtDosegrid dosegrid;
    if (!mDataRepository->GetDoseManager()->InitializeDosegrid(info, image3DEntity, skinVoi, dosegrid, false))
    {
        TPS_LOG_DEV_ERROR<<"Failed to get default Dosegrid [" << seriesUID << "].";
        return TPS_ER_FAILURE;
    }

    pocoGridSize.mstartx = dosegrid.get_xcoordinate3dstart();
    pocoGridSize.mstarty = dosegrid.get_ycoordinate3dstart();
    pocoGridSize.mstartz = dosegrid.get_zcoordinate3dstart();
    pocoGridSize.mx = dosegrid.get_xcoordinate3dinc();
    pocoGridSize.my = dosegrid.get_ycoordinate3dinc();
    pocoGridSize.mz = dosegrid.get_zcoordinate3dinc();
    pocoGridSize.msizex = dosegrid.get_xcoordinate3dinc() * dosegrid.get_xcount();
    pocoGridSize.msizey = dosegrid.get_ycoordinate3dinc() * dosegrid.get_ycount();
    pocoGridSize.msizez = dosegrid.get_zcoordinate3dinc() * dosegrid.get_zcount();

    return TPS_ER_SUCCESS;
}


bool TpsBLController::GetNormGroupsInfo(const int targetLevel, const std::string uid, std::vector<RtNormgroup*>& vNormGroups){
    if (&vNormGroups == nullptr){
        TPS_LOG_DEV_ERROR<<"NormGroups vector is null.";
    }
    std::string planUid = GetPlanUid_i(targetLevel, uid);
    vNormGroups = mDataRepository->GetNormgroupManager()->GetNormalGroupList(planUid);
    return true; 
}

bool TpsBLController::GetPlanInfo(const int targetLevel, const std::string uid, RtPlan*& vPlan){
    if (&vPlan == nullptr){
        TPS_LOG_DEV_ERROR<<"Plan vector is null.";
    }
    std::string planUid = GetPlanUid_i(targetLevel, uid);
    mDataRepository->GetPatientManager()->GetPlan(planUid, &vPlan);
    return true; 
}

bool TpsBLController::GetDvhsInfo(const std::string planUid, std::vector<RtDvh*>& vDvhs, bool reCalculate  /*false*/){

    if (reCalculate)
    {
        mDoseProxy->CalculateDVH(planUid);
    }

    if (&vDvhs == nullptr){
        TPS_LOG_DEV_ERROR << "dvh vector is null.";
    }
    vDvhs = mDataRepository->GetDVHManager()->GetPlanDvhs(planUid);
    return true;
}

//for plan report
bool TpsBLController::GetDvhsInfoForPlanReport(const std::string planUid, std::vector<RtDvh*>& vDvhs){
    if (&vDvhs == nullptr){
        TPS_LOG_DEV_ERROR<<"dvh vector is null.";
    }
    vDvhs = mDataRepositoryForPlanReport->GetDVHManager()->GetPlanDvhs(planUid);
    return true;
}

std::string TpsBLController::GetPlanUid_i(const int targetLevel, const std::string uid){
    std::string planUid;
    switch (targetLevel) {
    case 0: 
        {// plan
            planUid = uid;
        }
        break;
    case 1: 
        {// normgroup
            planUid = mDataRepository->GetNormgroupManager()->GetPlanUIDByNormGroupUID(uid);
        }
        break;
    default:
        TPS_LOG_DEV_ERROR<< "targetLevel is not correct: " << targetLevel;
    }
    return planUid;
}

#pragma region [***************** Mpr PageTurn, Reset, Zoom, Pan, Windowing***********************]
bool TpsBLController::MprPageTurn(WINDOW_TYPE wndType, const std::string& wndUid, int iPageturn)
{
    std::string imageUid;
    if (!mRenderProxy->GetDataSource(wndType, IMAGE, imageUid))
    {
        TPS_LOG_DEV_ERROR<<"Failed to get IMAGE data source uid.";
        return TPS_ER_FAILURE;
    }
    TpsImage3DEntity* pImage = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(imageUid, &pImage) || nullptr == pImage)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d entity.";
        return TPS_ER_FAILURE;
    }
    RtImage3DHeader* pHeader = nullptr;
    pImage->GetImage3DHeader(pHeader);
    if (nullptr == pHeader)
    {
        TPS_LOG_DEV_ERROR<<"pHeader is null.";
        return false;
    }
    bool* pInter = nullptr;
    int iCountNew = 0;
    pHeader->GetInterpolation(pInter, &iCountNew);

    if (iCountNew > pHeader->m_vScanInfoList.size())
    {

        int slice = 0;
        int startSliceNumber = 0;
        int totalSlice = 0;
        bool bRet = mRenderProxy->GetCurrentMprSlice(wndType, wndUid, slice, startSliceNumber,  totalSlice);
        if(!bRet)
        {  
            TPS_LOG_DEV_ERROR<<"Failed to GetCurrent MprSlice."; 
            return false;
        }
        const int indexNew = slice - startSliceNumber;
        if (indexNew >=0 && indexNew < iCountNew)
        {
            if (pInter[indexNew])
            {
                int iSkipCount = 0;
                if (iPageturn < 0)
                {
                    for (int i = indexNew+1; i < iCountNew; ++i)
                    {
                        if (pInter[i])
                        {
                            ++iSkipCount;
                        }
                        else
                        {
                            break;
                        }
                    }
                    iPageturn -= iSkipCount;
                } 
                else
                {
                    for (int i = indexNew-1; i >= 0; --i)
                    {
                        if (pInter[i])
                        {
                            ++iSkipCount;
                        }
                        else
                        {
                            break;
                        }
                    }
                    iPageturn += iSkipCount;
                }
            }
        }
    }

    if (!mRenderProxy->MprPageTurn(wndType, wndUid, iPageturn)) 
    {
        TPS_LOG_DEV_ERROR<<"Failed to page mpr."; 
        return false;
    } 

    mRenderProxy->Render();
    return true;
}

bool TpsBLController::MprReset(WINDOW_TYPE wndType, const std::string& wndUid, int acType) {
    if (!mRenderProxy->MprReset(wndType, wndUid, acType)) {
        TPS_LOG_DEV_ERROR<<"Failed to reset mpr."; 
        return false;
    } else {
        mRenderProxy->Render();
        return true;
    }
}

bool TpsBLController::MprZoom(WINDOW_TYPE wndType, const std::string& wndUid, float scaleFactor) {
    if (!mRenderProxy->MprZoom(wndType, wndUid, scaleFactor)) {
        TPS_LOG_DEV_ERROR<<"Failed to zoom mpr."; 
        return false;
    } else {
        mRenderProxy->Render();
        return true;
    }
}

bool TpsBLController::MprZoom(WINDOW_TYPE wndType, const std::string& wndUid, float startx, float starty, float stopx, float stopy) {
    if (!mRenderProxy->MprZoom(wndType, wndUid, startx, starty, stopx, stopy)) {
        TPS_LOG_DEV_ERROR<<"Failed to zoom mpr."; 
        return false;
    } else {
        mRenderProxy->Render();
        return true;
    }
}

bool TpsBLController::MprPan(WINDOW_TYPE wndType, const std::string& wndUid, 
    float startX, float startY, float stopX, float stopY) {
        if (!mRenderProxy->MprPan(wndType, wndUid, startX, startY, stopX, stopY)) {
            TPS_LOG_DEV_ERROR<<"Failed to pan mpr."; 
            return false;
        } else {
            mRenderProxy->Render();
            return true;
        }
}

bool TpsBLController::MprWindowing(WINDOW_TYPE wndType, const std::string& wndUid, 
    float deltaWidth, float deltaCenter, bool updateAll) {
        if (!mRenderProxy->MprWindowing(wndType, wndUid, deltaWidth, deltaCenter, updateAll)) {
            TPS_LOG_DEV_ERROR<<"Failed to windonwing mpr."; 
            return false;
        } else {
            mRenderProxy->Render();
            return true;
        }
}
#pragma endregion

#pragma region [***************** Doseline display related ***********************]
int TpsBLController::ChangeDoselineDisplayMode(string planUID, bool isLine, bool isColorwash,
	float transparency,float transparency3D) 
{
	DOSELINE_RENDERMODE currentRenderMode = GetDataRepository()->GetDisplayStateManager()->GetDoselineRenderModeType();
	DOSELINE_RENDERMODE targetRenderMode = LINE;
	if (isLine && isColorwash)
	{
		targetRenderMode = BOTH;
	} 
	else if (isLine)
	{
		targetRenderMode = LINE;
	}
	else if (isColorwash)
	{
		targetRenderMode = COLORWASH;
	}
	else
	{
		targetRenderMode = NONE;
	}

	float currentTransparency = GetDataRepository()->GetDisplayStateManager()->GetDoselineTransparency();
	float currentTransparency3D = GetDataRepository()->GetDisplayStateManager()->GetDoselineTransparency3D();

	if (currentRenderMode == targetRenderMode &&
		currentTransparency == transparency &&
		currentTransparency3D == transparency3D) 
	{
		return TPS_ER_SUCCESS;
	}

	GetDataRepository()->GetDisplayStateManager()->SetDoselineRenderModeType(targetRenderMode);
	GetDataRepository()->GetDisplayStateManager()->SetDoselineTransparency(transparency);
	GetDataRepository()->GetDisplayStateManager()->SetDoselineTransparency3D(transparency3D);

	mRenderProxy->RefreshDoseLine(planUID);
	mRenderProxy->Render();

	return TPS_ER_SUCCESS;
}

#pragma endregion


#pragma region [***************** Bev Pan, Zoom, DrrWindowing, Bev2Cev, ResetBev***********************]

bool TpsBLController::BevPan(WINDOW_TYPE wndType, const std::string& wndUid, 
    float startX, float startY, float stopX, float stopY) {

        if (!mRenderProxy->BEVPan(wndType, wndUid, startX, startY, stopX, stopY)) {
            TPS_LOG_DEV_ERROR<<"Failed to pan bev."; 
            return false;
        } else {
            mRenderProxy->Render();
            return true;
        }		  
}

bool TpsBLController::BevZoom(WINDOW_TYPE wndType,  float zoomFactor) {

    if (!mRenderProxy->BEVZoom(wndType, zoomFactor)) {
        TPS_LOG_DEV_ERROR<<"Failed to zoom bev."; 
        return false;	
    } else {
        mRenderProxy->Render();
        return true;
    }	
}

bool TpsBLController::BevWindowing(WINDOW_TYPE wndType, float startX, float startY, 
    float stopX, float stopY) {

        float deltaX = stopX - startX;
        float deltaY = stopY - startY;
        const int scale = 200;
        int deltaWindowCenter = int(deltaY * scale + 0.5);
        int deltaWindowWidth = int(deltaX * scale + 0.5);

        if (!mRenderProxy->BEVWindowing(wndType, deltaWindowWidth, deltaWindowCenter)) {
            TPS_LOG_DEV_ERROR<<"Failed to windowing bev."; 
            return false;
        } else {
            mRenderProxy->Render();
            return true;
        }		
}

int TpsBLController::BevCevSwitch(WINDOW_TYPE wndType, bool isBev) {

    BEV_VIEW_TYPE currentType = GetDataRepository()->GetDisplayStateManager()->GetBevViewType();
    BEV_VIEW_TYPE targetType = isBev ? BEV_VIEW : CEV_VIEW;
    if (currentType == targetType) return TPS_ER_SUCCESS;
    GetDataRepository()->GetDisplayStateManager()->SetBevViewType(isBev ? BEV_VIEW : CEV_VIEW);

    if (!mRenderProxy->BEVCEVSwitch(wndType)) {
        TPS_LOG_DEV_ERROR<<"Failed to switch BEVCEV.";
        return TPS_ER_FAILURE;	
    } else {
        mRenderProxy->Render();	
        return TPS_ER_SUCCESS;
    }	
}

int TpsBLController::BevReset(WINDOW_TYPE wndType, int actionType) {

    if (!mRenderProxy->BEVReset(wndType, actionType)) {
        TPS_LOG_DEV_ERROR<<"Failed to reset BEV.";
        return TPS_ER_FAILURE;	
    } else {
        mRenderProxy->Render();	
        return TPS_ER_SUCCESS;
    }
}

int TpsBLController::SetDrrVisibility(bool visibility)
{

    if (!mRenderProxy->SetDrrVisibility(visibility)) {
        TPS_LOG_DEV_ERROR<<"Failed to set DRR visibility."; 
        return TPS_ER_FAILURE;	
    } else {
        mRenderProxy->Render();
        return TPS_ER_SUCCESS;
    }	

}

bool TpsBLController::BevSwitchDisplayMode(int displayMode)
{
    if (!mRenderProxy->BevSwitchDisplayMode(displayMode)){
        TPS_LOG_DEV_ERROR<<"Failed to switch bev display mode.";
        return false;
    }
    mRenderProxy->Render();
    return true;
}

int TpsBLController::SwitchSegment(const std::string& beamUid, int page) {

    auto ngUid = mDataRepository->GetBeamManager()->GetNormalGroupUIDViaBeamUID(beamUid);
    auto planUid = mDataRepository->GetNormgroupManager()->GetPlanUIDByNormGroupUID(ngUid);
    mRenderProxy->SwitchSegment(beamUid, page);
    mRenderProxy->ResetBeamViewCameras(planUid);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

void TpsBLController::DowmSampleDrr(WINDOW_TYPE wndType, 
    const std::string& wndUid, float downSampleRate)
{
    if (downSampleRate <= 1e-6) return;
    mRenderProxy->DownSampleDrr(wndType, wndUid, downSampleRate);
}
#pragma endregion

bool TpsBLController::PreviewTurnPage(int turnDirection, const std::string& imageUid, 
    const void*& buffer, int& bufferSize)
{
    TpsImage3DEntity* pTpsImage3d = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(
        imageUid, &pTpsImage3d) || pTpsImage3d == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The image entity does not exist.";
        return false;
    }
    if (pTpsImage3d->GetRtSeries() == nullptr){
        TPS_LOG_DEV_ERROR<<"RtImage3D is null.";
        return false;
    }

    if(!pTpsImage3d->InitSliceBuffer(turnDirection)){
        TPS_LOG_DEV_ERROR<<"Failed to Initialize slice buffer.";
        return false;
    }
    auto header = pTpsImage3d->GetRtSeries()->get_header();

    buffer = pTpsImage3d->GetSliceBuffer();
    bufferSize = header->m_iXDim * header->m_iYDim;
    return true;
}

void TpsBLController::Redo()
{
    TpsCommandManager *commandMgr = mDataRepository->GetCommandManager();
    if (nullptr == commandMgr) {
        TPS_LOG_DEV_ERROR<<"Command manager is null.";
        return;
    }
    commandMgr->Redo();
}

void TpsBLController::Undo()
{
    TpsCommandManager *commandMgr = mDataRepository->GetCommandManager();
    if (nullptr == commandMgr) {
        TPS_LOG_DEV_ERROR<<"Command manager is null.";
        return;
    }
    commandMgr->Undo();
}

HitTestResult TpsBLController::HitTest(WINDOW_TYPE wndType, const std::string& wndUid, double x, double y)
{
    HitTestResult hitRes = mRenderProxy->HitTest(wndType, wndUid, x, y);
    if (hitRes._isDirty) mRenderProxy->Render();
    return hitRes;
}

void TpsBLController::ChangeBevEditTarget(BEV_EDIT_TARGET target){
    mDataRepository->GetDisplayStateManager()->SetBevEditTarget(target);
}

bool TpsBLController::StartOptimize(const std::string& ngUid, 
    const std::vector<std::string>& vecBasedOnNgUids, 
    const std::vector<RtDoseoptgoalbase*> optGoals, 
    const DoseOptimizationParam optParam, const OptResponserPtr& optResponser)
{
    RtNormgroup* normGroup = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(ngUid, &normGroup)){
        TPS_LOG_DEV_ERROR<<"Failed to get plan.";
        return false;
    }
    RtDbDef::PLAN_TYPE plan_type = static_cast<RtDbDef::PLAN_TYPE>(normGroup->get_plantype());
    mCtrlOptObserver->AttachResponser(optResponser);
    mOptProxy->OptStart(plan_type, ngUid, vecBasedOnNgUids, optGoals, optParam);
    return true;
}

bool TpsBLController::OptimizeAbort(){
    mOptProxy->OptAbort();
    return true;
}

int TpsBLController::LoadDoseLineTemplate(std::vector<DoseLineTemplate*>& doseLineTemplateList) {

    return  mLoadObjects->LoadDoseLineTemplate(doseLineTemplateList);
}

int TpsBLController::LoadTemplate(std::vector<VOITemplate*>& voiTemplateList,
    std::vector<DoseLineTemplate*>& doseLineTemplateList,
    std::vector<BeamTemplate*>& beamTemplateList,
    std::vector<OptimizerTemplate*>& optimizerTemplateList,
    std::vector<RtTemplate*>& windowingTemplateList) {

        int result = mLoadObjects->LoadTemplate();
        if (result != TPS_ER_SUCCESS) {
            return result;
        }

        result = mLoadObjects->LoadVOITemplate(voiTemplateList);
        if (result != TPS_ER_SUCCESS) {
            return result;
        }

        result = mLoadObjects->LoadDoseLineTemplate(doseLineTemplateList);
        if (result != TPS_ER_SUCCESS) {
            return result;
        }

        result = mLoadObjects->LoadBeamTemplate(beamTemplateList);
        if (result != TPS_ER_SUCCESS) {
            return result;
        }

        result = mLoadObjects->LoadOptimizerTemplateList(optimizerTemplateList);
        if (result != TPS_ER_SUCCESS) {
            return result;
        }

        result = mLoadObjects->LoadWindowingTemplateList(windowingTemplateList);
        if (result != TPS_ER_SUCCESS) {
            return result;
        }

        return result;
}

int TpsBLController::UpdateDoseLineParams(std::string planUID, std::vector<RtDoseline*>& doseLineList,
    RtTpsDoselineSetting& doselineSetting, bool isTemporaryUpdated)
{
    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(planUID, &plan)){
        TPS_LOG_DEV_ERROR << "Fail to get plan with uid: " << planUID;
        return TPS_ER_FAILURE;
    }

    plan->set_displaymode(doselineSetting.m_eDisplayMode);
    plan->set_hundredpercentdefinition(doselineSetting.m_eHundredPercentDefinition);
    plan->set_specifieddose(doselineSetting.m_fSpecifiedDose);
    plan->set_rendermode(doselineSetting.m_eRenderMode);
    plan->set_transparency(doselineSetting.m_fTransParency);
    plan->set_transparency3d(doselineSetting.m_fTransParency3D);
    plan->set_enableskindoseview(doselineSetting.m_bEnableSkinDoseView);
    plan->set_maxskindose(doselineSetting.m_fMaxSkinDose);

    if (!mDataRepository->GetDoseManager()->UpdateDoseLineList(planUID, doseLineList)) {
        TPS_LOG_DEV_ERROR << "Fail to update doseLine";
        return TPS_ER_FAILURE;
    }

    DOSELINE_DISPLAYMODE disp_mode = RELATIVE_VALUE;
    if (doselineSetting.m_eDisplayMode == RtDbDef::Absolute){
        disp_mode = ABSOLUTE_VALUE;
    }

    if (!isTemporaryUpdated) {
        if (DB_CODES_SUCCESS != mDatabaseWrapper->EraseDoselineByPlanUid(planUID)) {
            TPS_LOG_DEV_ERROR << "Failed to delete doseline params to database.";
            return TPS_ER_DB_ERROR;
        }

        for (auto itDoseLine = doseLineList.begin(); itDoseLine != doseLineList.end(); ++itDoseLine) {
            if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertDoselineObject(*itDoseLine)) {
                TPS_LOG_DEV_ERROR << "Fail to save doseLine into db";
                return TPS_ER_DB_ERROR;
            }
        }

        if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdatePlanObject(*plan)) {
            TPS_LOG_DEV_ERROR << "Fail to save plan into db";
            return TPS_ER_DB_ERROR;
        }
    }

    mRenderProxy->RefreshDoseLine(planUID);
    mRenderProxy->Render();

    return TPS_ER_SUCCESS;
}


int TpsBLController::LoadSeries(const std::vector<std::string>& seriesUIDList,
    std::vector<TpsImage3DEntity*>& seriesList,
    std::vector<std::shared_ptr<TpsSeriesGroup4d>>& groupList,
    std::vector<ROIEntity*>& voiList)
{
    int iRet = mLoadObjects->LoadSeries(seriesUIDList);
    if (iRet != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR<<"Failed to load series list.";
        return iRet;
    }

    seriesList.clear();
    voiList.clear();
    auto imgMng = mDataRepository->GetImageDataManager();
    auto voiMng = mDataRepository->GetVoiManager();
    TpsImage3DEntity* tmpImage;
    std::vector<ROIEntity*> tmpVoiList;
    for (auto it = seriesUIDList.begin(); it != seriesUIDList.end(); ++it){
        if (!imgMng->GetImage3DData(*it, &tmpImage))
        {
            TPS_LOG_DEV_ERROR<<"Failed to get image 3d entity: "<<*it;
            continue;
        }
        seriesList.push_back(tmpImage);
        tmpVoiList = std::move(voiMng->GetROIBySeries(*it));
        voiList.insert(voiList.end(), tmpVoiList.begin(), tmpVoiList.end());
    }
    imgMng->GetAllSeriesGroup(groupList);
    return TPS_ER_SUCCESS;
}

int TpsBLController::LoadRois(const std::vector<std::string>& structureSetUIDList,
    std::vector<ROIEntity*>& voiList)
{
    int iRet = mLoadObjects->LoadROIs(structureSetUIDList);
    if (iRet != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR<<"Failed to load voi list.";
        return iRet;
    }

    voiList = std::move(mDataRepository->GetVoiManager()->GetROIListViaStructureUids(structureSetUIDList));
    bool anySucceeded = false;
    for (int i = 0; i < voiList.size(); ++i){
        //这里添加VOI GO并不一定成功，如果VOI所属图像GO尚未被创建，则添加VOI GO失败，属于正常情况。
        anySucceeded |= mRenderProxy->AddVOI(voiList[i]->GetUid(), voiList[i]->GetSeriesUid());
    }
    if (anySucceeded) mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

int TpsBLController::GetNormalgroupBeamOperateInfo(RtNormgroup* normgroup, proto::RtTpsProtoRepeatedBeamOperate& beamOperateList)
{
    const std::string sNormgroupUid = normgroup->get_uid();
    RtNormgroup* pPreviousNg = nullptr;
    mDataRepository->GetNormgroupManager()->GetNormGroup(sNormgroupUid, &pPreviousNg);
    if (nullptr == pPreviousNg || nullptr == normgroup)
    {
        TPS_LOG_DEV_ERROR << "normgroup is null.";
        return TPS_ER_FAILURE;
    }

    bool isMachineChanged = normgroup->get_machineuid() != pPreviousNg->get_machineuid();
    bool isRadiationTypeChanged = normgroup->get_radiationtype() != pPreviousNg->get_radiationtype();
    bool isTechniqueTypeChanged = normgroup->get_plantype() != pPreviousNg->get_plantype();

    if (!isMachineChanged && !isRadiationTypeChanged && !isTechniqueTypeChanged)
    {
        return TPS_ER_SUCCESS;
    }

    int nPreviousPlanType = pPreviousNg->get_plantype();
    int nCurrentPlanType = normgroup->get_plantype();
    RtMachine* preMachine = nullptr;
    if (!mDataRepository->GetMachineManager()->GetMachine(pPreviousNg->get_machineuid(), preMachine))
    {
        TPS_LOG_DEV_ERROR << "Failed to get machine[" << pPreviousNg->get_machineuid() << "]";
        return TPS_ER_FAILURE;
    }
    RtMachine* curMachine = nullptr;
    if (!mDataRepository->GetMachineManager()->GetMachine(normgroup->get_machineuid(), curMachine))
    {
        TPS_LOG_DEV_ERROR << "Failed to get machine[" << normgroup->get_machineuid() << "]";
        return TPS_ER_FAILURE;
    }

    std::vector<RtBeam*> vecBeams = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(sNormgroupUid);

    for (size_t i = 0; i < vecBeams.size(); ++i)
    {
        RtBeam* beam = vecBeams[i];
        auto protoBeamOperate = beamOperateList.add_beamoperatelist();
        protoBeamOperate->set_beam_uid(beam->get_uid());
        protoBeamOperate->set_beam_name(beam->get_name());
        protoBeamOperate->set_normal_group_uid(sNormgroupUid);
        if (isRadiationTypeChanged)
        {
            protoBeamOperate->set_beam_operate_type((int)RtDbDef::BEAM_OPERATE_DELETE);
            protoBeamOperate->set_beam_delete_reason_type((int)RtDbDef::RADIATION_CHANGE);
            continue;
        }
        if (isTechniqueTypeChanged)
        {
            if (TpsCmdHelper::IsArcBeam(RtDbDef::PLAN_TYPE(nPreviousPlanType))
                != TpsCmdHelper::IsArcBeam(RtDbDef::PLAN_TYPE(nCurrentPlanType)))
            {
                protoBeamOperate->set_beam_operate_type((int)RtDbDef::BEAM_OPERATE_DELETE);
                protoBeamOperate->set_beam_delete_reason_type((int)RtDbDef::TECHNIQUE_SWITCH_ARCNONARC);
                continue;
            }
            else
            {
                protoBeamOperate->set_beam_operate_type((int)RtDbDef::BEAM_OPERATE_MODIFY);
                protoBeamOperate->add_beam_modify_typelist((int)RtDbDef::SEGMENT);
            }
        }

        if (isMachineChanged)
        {
            if (!TpsCmdHelper::ValidateMachineMlcJawParameter(preMachine, curMachine))
            {
                protoBeamOperate->set_beam_operate_type((int)RtDbDef::BEAM_OPERATE_DELETE);
                protoBeamOperate->set_beam_delete_reason_type((int)RtDbDef::MACHINE_CORE_PARAMETER_CHANGE);
                continue;
            }

            if (!TpsCmdHelper::ValidateMachineGeometryParameter(beam, curMachine, (RtDbDef::PLAN_TYPE)nCurrentPlanType))
            {
                protoBeamOperate->set_beam_operate_type((int)RtDbDef::BEAM_OPERATE_DELETE);
                protoBeamOperate->set_beam_delete_reason_type((int)RtDbDef::BEAMANGLE_NOT_REASONABLE);
                continue;
            }

            RtCommissionedunit* commissionUnit = nullptr;
            if (!mDataRepository->GetMachineManager()->GetCommissionedUnit(beam->get_commissionedunituid(), commissionUnit))
            {
                TPS_LOG_DEV_ERROR << "The commissioned unit does not exist.";
                return TPS_ER_FAILURE;
            }

            std::vector<RtCommissionedunit*> vCommissionedunitList;
            mDataRepository->GetMachineManager()->GetSpecifiedCommissonedUnit(
                curMachine->get_uid(),
                normgroup->get_radiationtype(),
                nCurrentPlanType,
                vCommissionedunitList);

            std::vector<RtCommissionedunit*>::iterator itr;
            itr = std::find_if(vCommissionedunitList.begin(), vCommissionedunitList.end(),
                std::bind(TpsCmdHelper::CommissionedUnitComparision, std::placeholders::_1, commissionUnit));

            if (itr == vCommissionedunitList.end())
            {
                protoBeamOperate->set_beam_operate_type((int)RtDbDef::BEAM_OPERATE_DELETE);
                protoBeamOperate->set_beam_delete_reason_type((int)RtDbDef::COMMISSIONEDUNIT_NOT_MATCHED);
                continue;
            }
            else
            {
                protoBeamOperate->set_beam_operate_type((int)RtDbDef::BEAM_OPERATE_MODIFY);
                protoBeamOperate->add_beam_modify_typelist((int)RtDbDef::COMMISSIONED_UNIT);
                protoBeamOperate->add_beam_modify_typelist((int)RtDbDef::ACCESSORY);
                std::vector<float> doserateList = (*itr)->get_discretedoserate();
                vector<float>::iterator itDoseRateList;
                itDoseRateList = std::find(doserateList.begin(), doserateList.end(), beam->get_doserate());
                if (itDoseRateList == doserateList.end())
                {
                    protoBeamOperate->add_beam_modify_typelist((int)RtDbDef::DOSE_RATE);
                }

                std::vector<int> algorithmTypeList;
                if ((*itr)->get_ispencilebeamsupport())
                {
                    algorithmTypeList.push_back(1);
                }
                if ((*itr)->get_isconvolutionsupport())
                {
                    algorithmTypeList.push_back(2);
                }
                if ((*itr)->get_ismontecarlosupport())
                {
                    algorithmTypeList.push_back(3);
                }

                vector<int>::iterator itAlgorithmTypeList;
                itAlgorithmTypeList = std::find(algorithmTypeList.begin(), algorithmTypeList.end(), beam->get_dosealgorithmtype());
                if (itAlgorithmTypeList == algorithmTypeList.end())
                {
                    protoBeamOperate->add_beam_modify_typelist((int)RtDbDef::ALGORITHMTYPE);
                }
            }
        }
    }

    return TPS_ER_SUCCESS;
}

int TpsBLController::LoadPlans(const std::vector<std::string>& planUIDList,
    std::vector<RtPlan*>& planList,
    std::vector<RtPoi*>& poiList,
    std::vector<RtDoseline*>& doseLineList,
    std::vector<RtNormgroup*>& normGroupList,
    std::vector<RtBeam*>& beamList)
{
    int iRet = mLoadObjects->LoadPlans(planUIDList);
    if (iRet != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR<<"Failed to load plan list.";
        return iRet;
    }

    planList.clear();
    poiList.clear();
    doseLineList.clear();
    normGroupList.clear();
    beamList.clear();

    auto patMng = mDataRepository->GetPatientManager();
    auto doseMng = mDataRepository->GetDoseManager();
    auto ngMng = mDataRepository->GetNormgroupManager();
    auto beamMng = mDataRepository->GetBeamManager();
    RtPlan* plan = nullptr;
    std::vector<RtDoseline*> tmpDoseLineList;
    std::vector<RtNormgroup*> tmpNgList;
    std::vector<RtBeam*> tmpBeamList;
    for (auto it = planUIDList.begin(); it != planUIDList.end(); ++it){
        //plan
        if (!patMng->GetPlan(*it, &plan))
        {
            TPS_LOG_DEV_ERROR<<"Failed to get plan: "<<*it;
            continue;
        }
        //dose line
        if (!doseMng->GetDoseLineList(*it, tmpDoseLineList))
        {
            TPS_LOG_DEV_ERROR<<"Failed to get dose line list.";
            continue;
        }
        planList.push_back(plan);
        doseLineList.insert(doseLineList.end(), tmpDoseLineList.begin(), tmpDoseLineList.end());
        //normal group
        tmpNgList = std::move(ngMng->GetNormalGroupList(*it));
        normGroupList.insert(normGroupList.end(), tmpNgList.begin(), tmpNgList.end());
        //beam
        for (int i = 0; i < tmpNgList.size(); ++i){
            tmpBeamList = std::move(beamMng->GetBeamsByNormGroup(tmpNgList[i]->get_uid()));
            beamList.insert(beamList.end(), tmpBeamList.begin(), tmpBeamList.end());
        }
    }
    return TPS_ER_SUCCESS;
}

int TpsBLController::LoadDoseGrids(const std::vector<std::string>& doseGridUIDList, 
    std::vector<std::string>& affectedPlanUidList)
{
    int iRet = mLoadObjects->LoadDoseGrids(doseGridUIDList, affectedPlanUidList);
    if (iRet != TPS_ER_SUCCESS){
        TPS_LOG_DEV_ERROR<<"Failed to load dose grids.";
        return iRet;
    }

    bool anySucceeded = false;
    for (int i = 0; i < affectedPlanUidList.size(); ++i){
        anySucceeded |= mRenderProxy->RefreshDose(affectedPlanUidList[i]);
    }
    if (anySucceeded) mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

int TpsBLController::UpdatePlanOrder(const std::string& patientUid,
    std::vector<RtCourse*>& courseList, 
    std::vector<RtPrescription*>& prescriptionList,
    std::vector<RtConstraint*>& constraintList)
{
    int iRet = mLoadObjects->UpdatePlanOrder(patientUid);
    if (iRet != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR<<"Failed to update plan order.";
        return iRet;
    }
    auto patMng = mDataRepository->GetPatientManager();
    courseList = std::move(patMng->GetAllCourse());
    prescriptionList = std::move(patMng->GetAllPrescription());
    constraintList = std::move(patMng->GetAllConstraint());
    return TPS_ER_SUCCESS;
}

int TpsBLController::GetMachineTreeFromManagers(
    std::vector<RtApproverecord*>& vApproverecordList,
    std::vector<RtMachine*>& vMachineList,
    std::vector<RtCommissionedunit*>& vCommissionedunitList,
    std::vector<RtAccessorychunk*>& vRtAccessorychunkList,
    std::vector<RtGoldenstt*>& vRtGoldensttList,
    std::vector<RtAccessory*>& vRtAccessoryList)
{
    if (nullptr == mDataRepository)
    {
        TPS_LOG_DEV_ERROR << "GetMachineTreeFromManagers failed as mDataRepository is null.";
        return TPS_ER_FAILURE;
    }
    mDataRepository->GetMachineManager()->GetAllApproveRecord(vApproverecordList);
    mDataRepository->GetMachineManager()->GetAllMachine(vMachineList);
    mDataRepository->GetMachineManager()->GetAllCommissionedUnit(vCommissionedunitList);
    mDataRepository->GetMachineManager()->GetAllAccessory(vRtAccessoryList);
    mDataRepository->GetMachineManager()->GetAllAccessoryChunk(vRtAccessorychunkList);
    mDataRepository->GetMachineManager()->GetAllGoldenstt(vRtGoldensttList);
    return TPS_ER_SUCCESS;
}

int TpsBLController::GetAllRtTables(std::vector<RtTable*>& vecRtTables)
{
    if (nullptr == mDataRepository)
    {
        TPS_LOG_DEV_ERROR<<"Data repository is null.";
        return TPS_ER_FAILURE;
    }

    vecRtTables = mDataRepository->GetTableManager()->GetAllRtTables();
    return TPS_ER_SUCCESS;
}

int TpsBLController::GetVoiSettingTable(std::vector<RtVoisetting*>& voiSettingTable)
{
    if (nullptr == mDataRepository)
    {
        TPS_LOG_DEV_ERROR<<"Data repository is null.";
        return TPS_ER_FAILURE;
    }

    voiSettingTable = mDataRepository->GetTableManager()->GetVoiManager()->GetVoiSettings();
    return TPS_ER_SUCCESS;
}

int TpsBLController::DoseHotSpots(const std::string& strPlanUid, const std::string& strVoiUid, float fDoseLimit, int iHotFlag)
{
    mRenderProxy->DoseHotSpots(strPlanUid, strVoiUid, fDoseLimit, iHotFlag);
    mRenderProxy->Render();

    return TPS_ER_SUCCESS;
}

int TpsBLController::CompareDoseGrid( CertifiedPlan &primaryPlan, CertifiedPlan &secondaryPlan )
{
    int ret = Identical;
    //1. compare dose grid size
    RtPlan *primPlan = nullptr;
    TpsPatientManager *patientManager = mDataRepository->GetPatientManager();
    patientManager->GetPlan(primaryPlan.mPlanUID, &primPlan);
    RtDosegrid *primDosegrid = primPlan->get_dosegrid();

    RtPlan *secPlan = nullptr;
    patientManager->GetPlan(secondaryPlan.mPlanUID, &secPlan);
    RtDosegrid *secDosegrid = secPlan->get_dosegrid();

    if (primDosegrid==nullptr || secDosegrid==nullptr) {
        TPS_LOG_DEV_ERROR<<"At least one of two merging plan dosegrid is nullptr.";
        return (int)Unknow_Result;
    }

    if (primDosegrid->get_xcount() != secDosegrid->get_xcount()
        || primDosegrid->get_ycount() != secDosegrid->get_ycount()
        || primDosegrid->get_zcount() != secDosegrid->get_zcount()) 
    {
        ret |= DIFF_Dosegrid_Size;
    }

    //2. compare dose gird spacing
    if (primDosegrid->get_xcoordinate3dinc() != secDosegrid->get_xcoordinate3dinc()
        || primDosegrid->get_ycoordinate3dinc() != secDosegrid->get_ycoordinate3dinc()
        || primDosegrid->get_zcoordinate3dinc() != secDosegrid->get_zcoordinate3dinc()) 
    {
        ret |= DIFF_Dosegrid_Spacing;
    }

    return ret;
}

int TpsBLController::RotateShadedSurface(WINDOW_TYPE windowType, const float& preX, const float& preY, const float& curX, const float& curY)
{
    std::string seriesUid;
    mRenderProxy->GetDataSource(windowType, IMAGE, seriesUid);
    Mcsf::Point2f prePt(preX, preY);
    Mcsf::Point2f curPt(curX, curY);
    mRenderProxy->RotateShadedSurface(seriesUid,prePt, curPt);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

int TpsBLController::ResizeViewModule(WINDOW_TYPE window, const std::string& wndUid, DISPLAY_SIZE displaySize)
{
    if (nullptr == mRenderProxy)
    {
        return TPS_ER_FAILURE;
    }

    if (!mRenderProxy->ResizeViewModule(window, wndUid, displaySize)) {
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int TpsBLController::ZoomShadedSurface(WINDOW_TYPE windowType, const float& zoomFactor)
{
    std::string seriesUid;
    mRenderProxy->GetDataSource(windowType, IMAGE, seriesUid);
    mRenderProxy->ShadedSurfaceZoom(seriesUid, zoomFactor);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

bool TpsBLController::GetShadedSurfaceViewDirecrion(WINDOW_TYPE windowType, const std::string& windowUid,
    double& x1, double& y1, double& z1, double& x2, double& y2, double& z2) {
        if (!mRenderProxy->GetShadedSurfaceViewDirecrion(windowType, windowUid, x1, y1, z1, x2, y2, z2)) {
            TPS_LOG_DEV_ERROR << "Failed to GetShadedSurfaceViewDirecrion.";
            return false;
        }
        return true;
}


int TpsBLController::UpdateDrrSetting(std::string seriesUid, std::string normUid, float ctThresholdMin, float ctThresholdMax, std::string ct2densityName, std::string ct2densityUid, float electronEnergy)
{
    bool ret = mRenderProxy->UpdateDrrSetting(seriesUid, normUid, ctThresholdMin, ctThresholdMax, ct2densityUid, electronEnergy);
    if(ret)
    {
        auto normalManager = mDataRepository->GetNormgroupManager();
        RtDrrsetting *pDrrsetting = nullptr;
        normalManager->GetDrrSetting(normUid, &pDrrsetting);
        if(pDrrsetting == nullptr) return TPS_ER_FAILURE;
        pDrrsetting->set_ct2densityuid(ct2densityUid);
        pDrrsetting->set_ctthresholdminvalue(ctThresholdMin);
        pDrrsetting->set_ctthresholdmaxvalue(ctThresholdMax);
        pDrrsetting->set_electronenergy(electronEnergy);
        mDatabaseWrapper->UpdateDrrsetting(*pDrrsetting);
        mRenderProxy->Render();
        return TPS_ER_SUCCESS;
    }
    return TPS_ER_FAILURE;
}

int TpsBLController::CreateDoseLineTemplate(const DoseLineTemplate& doseLineTemplate)
{
    std::ostringstream buf;
    boost::archive::text_oarchive oa(buf);
    oa << doseLineTemplate;

    RtTemplate doseLineTemplateConfig;
    doseLineTemplateConfig.set_uid(doseLineTemplate.GetDoseLineTemplateUID());
    doseLineTemplateConfig.set_name(doseLineTemplate.GetDoseLineTemplateName());
    doseLineTemplateConfig.set_datatype((int)DOSELINE_TEMPLATE);
    doseLineTemplateConfig.set_value(buf.str());

    if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertTemplate(doseLineTemplateConfig)) {
        TPS_LOG_DEV_ERROR << "Failed to save doselinetemplate config in DB with Config UID:"
            << doseLineTemplateConfig.get_uid();
        return TPS_ER_DB_ERROR;
    }

    if (!mDataRepository->GetConfigurationManager()->AddSystemConfig(doseLineTemplateConfig)) {
        TPS_LOG_DEV_WARNING << "Failed to add SystemConfig to manager!";
        return TPS_ER_FAILURE;
    }

    if (!mDataRepository->GetConfigurationManager()->AddDoseLineTemplate(doseLineTemplate)) {
        TPS_LOG_DEV_ERROR << "Failed to add doselinetemplate in TpsConfigurationManager with Config UID:"
            << doseLineTemplate.GetDoseLineTemplateUID();
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int TpsBLController::DeleteDoseLineTemplate(const std::string& doseLineTemplateUID)
{
    if (DB_CODES_SUCCESS != mDatabaseWrapper->EraseObject(doseLineTemplateUID, RtDbDef::TYPE_TEMPLATE)) 
    {
        TPS_LOG_DEV_ERROR << "Failed to save doselinetemplate config in DB with Config UID:"
            << doseLineTemplateUID;
        return TPS_ER_DB_ERROR;
    }

    if (!mDataRepository->GetConfigurationManager()->RemoveSystemConfig(doseLineTemplateUID)) {
        TPS_LOG_DEV_ERROR << "Failed to remove doselinetemplate config in TpsConfigurationManager with Config UID:"
            << doseLineTemplateUID;
        return TPS_ER_FAILURE;
    }

    if (!mDataRepository->GetConfigurationManager()->RemoveDoseLineTemplate(doseLineTemplateUID)) {
        TPS_LOG_DEV_ERROR << "Failed to remove doselinetemplate in TpsConfigurationManager with Config UID:"
            << doseLineTemplateUID;
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int TpsBLController::CreateVoiTemplate(VOITemplate& voiTemplate)
{
    std::ostringstream buf;
    boost::archive::text_oarchive oa(buf);
    oa << voiTemplate;

    RtTemplate voiTemplateConfig;
    voiTemplateConfig.set_uid(voiTemplate.mTemplateUid);
    voiTemplateConfig.set_name(voiTemplate.mTemplateName);
    voiTemplateConfig.set_datatype((int)VOI_TEMPLATE_TYPE);
    voiTemplateConfig.set_value(buf.str());

    if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertTemplate(voiTemplateConfig)) {
        TPS_LOG_DEV_ERROR << "Failed to save voiTemplateConfig config in DB with Config UID:"
            << voiTemplateConfig.get_uid();
        return TPS_ER_DB_ERROR;
    }

    if (!mDataRepository->GetConfigurationManager()->AddSystemConfig(voiTemplateConfig)) {
        TPS_LOG_DEV_WARNING << "Failed to add SystemConfig to manager!";
        return TPS_ER_FAILURE;
    }

    if (!mDataRepository->GetConfigurationManager()->AddVOITemplate(&voiTemplate)) {
        TPS_LOG_DEV_ERROR << "Failed to add voiTemplate in TpsConfigurationManager with Config UID:"
            << voiTemplate.mTemplateUid;
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int TpsBLController::CreateBeamTemplate(BeamTemplate& beamTemplate)
{
    std::ostringstream buf;
    boost::archive::text_oarchive oa(buf);
    oa << beamTemplate;

    RtTemplate beamTemplateConfig;
    beamTemplateConfig.set_uid(beamTemplate.mTemplateUID);
    beamTemplateConfig.set_name(beamTemplate.mTemplateName);
    beamTemplateConfig.set_datatype((int)BEAM_TEMPLATE_TYPE);
    beamTemplateConfig.set_value(buf.str());

    if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertTemplate(beamTemplateConfig)) {
        TPS_LOG_DEV_ERROR << "Failed to save voiTemplateConfig config in DB with Config UID:"
            << beamTemplateConfig.get_uid();
        return TPS_ER_DB_ERROR;
    }

    if (!mDataRepository->GetConfigurationManager()->AddSystemConfig(beamTemplateConfig)) {
        TPS_LOG_DEV_WARNING << "Failed to add SystemConfig to manager!";
        return TPS_ER_FAILURE;
    }

    if (!mDataRepository->GetConfigurationManager()->AddBeamTemplate(&beamTemplate)) {
        TPS_LOG_DEV_ERROR << "Failed to add voiTemplate in TpsConfigurationManager with Config UID:"
            << beamTemplate.mTemplateUID;
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int TpsBLController::CreateOptimizerTemplate(OptimizerTemplate& optimizerTemplate) {
    std::ostringstream buf;
    boost::archive::text_oarchive oa(buf);
    oa << optimizerTemplate;

    RtTemplate optimizerTemplateConfig;
    optimizerTemplateConfig.set_uid(optimizerTemplate.mTemplateUID);
    optimizerTemplateConfig.set_name(optimizerTemplate.mTemplateName);
    optimizerTemplateConfig.set_datatype((int)OPTIMIZER_TEMPLATE_TYPE);
    optimizerTemplateConfig.set_value(buf.str());

    if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertTemplate(optimizerTemplateConfig)) {
        TPS_LOG_DEV_ERROR << "Failed to save optimizerTemplate config in DB with Config UID:"
            << optimizerTemplateConfig.get_uid();
        return TPS_ER_DB_ERROR;
    }

    if (!mDataRepository->GetConfigurationManager()->AddSystemConfig(optimizerTemplateConfig)) {
        TPS_LOG_DEV_WARNING << "Failed to add SystemConfig to manager!";
        return TPS_ER_FAILURE;
    }

    if (!mDataRepository->GetConfigurationManager()->AddOptimizerTemplate(&optimizerTemplate)) {
        TPS_LOG_DEV_ERROR << "Failed to add optimizerTemplate in TpsConfigurationManager with Config UID:"
            << optimizerTemplate.mTemplateUID;
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

//void TpsBLController::ResetMlcShape(const std::string& beamUID) {
//    mRenderProxy->ResetMlcShape(beamUID);
//    mRenderProxy->Render();
//}

int TpsBLController::UpdateLightSetting(std::string seriesUid, int mprSection, float mprTransparency3D, float rx, float ry, float rz, 
    float scale, float specf, float ambf, float difff, float shininess, 
    float opacity, float lightx, float lighty, float lightz,int smoothness, 
    bool useCameraLight, int displayMode, bool displayVertexCube,
    int startStripIndex, int endStripIndex)
{
	TpsDisplayStateManager* displayStateManager = mDataRepository->GetDisplayStateManager();
	displayStateManager->SetMprSection(mprSection);
	displayStateManager->SetMPRTransparency3D(mprTransparency3D);

    mRenderProxy->UpdateLightSetting(seriesUid, mprSection, mprTransparency3D, rx, ry, rz, 
        scale, specf, ambf, difff, shininess, 
        opacity, lightx, lighty, lightz, smoothness, 
        useCameraLight, displayMode, displayVertexCube,
        startStripIndex, endStripIndex);
    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}

int TpsBLController::AddVoiByTemplate(std::string voiTemplateUid,
    std::string seriesUid,
    bool isAutoSegment,
    std::vector<CertifiedVOI>& voiList)
{
    if (voiTemplateUid.empty() || seriesUid.empty()) {
        TPS_LOG_DEV_WARNING << "Failed to call AddVoiByTemplate!";
        return TPS_ER_FAILURE;
    }

    VOITemplate* voiTemplate;
    if (!mDataRepository->GetConfigurationManager()->GetVOITemplate(voiTemplateUid, &voiTemplate)) {
        TPS_LOG_DEV_WARNING << "Failed to add SystemConfig to manager!";
        return TPS_ER_FAILURE;
    }

    //取出mVOICollection中和VOI list不重复的VOI放入mAddVOIInTemplateCollection
    std::vector<SimpleVOI> mAddVOIInTemplateCollection(voiTemplate-> mVOICollection);
    std::vector<ROIEntity*> voiEntitys = mDataRepository->GetVoiManager()->GetROIBySeries(seriesUid);
    bool erased = false;
    for (auto iterT = mAddVOIInTemplateCollection.begin(); iterT != mAddVOIInTemplateCollection.end();)
    {
        erased = false;
        for (auto iter = voiEntitys.begin(); iter != voiEntitys.end(); ++iter)
        {
            if(iterT->mName == (*iter)->GetInnerRoi()->get_name())
            {
                iterT = mAddVOIInTemplateCollection.erase(iterT);
                erased = true;
                break;
            }
        }
        if (!erased)
        {
            iterT++;
        }
    }


    if (isAutoSegment) {

        CertifiedAutoVois autoAddVoiList;
        std::vector<CertifiedVOI> addVoiList;
        std::for_each(mAddVOIInTemplateCollection.begin(), mAddVOIInTemplateCollection.end(),
            [&](SimpleVOI& voi) {

                CertifiedVOI certifiedVOI;
                certifiedVOI.mVoiUid = mDatabaseWrapper->GeneraterUid();
                certifiedVOI.mSeriesUid = seriesUid;
                certifiedVOI.mName = voi.mName;
                certifiedVOI.mColor[0] = voi.mColor[0];
                certifiedVOI.mColor[1] = voi.mColor[1];
                certifiedVOI.mColor[2] = voi.mColor[2];
                certifiedVOI.mColor[3] = 0.0f;
                certifiedVOI.mPlanningRole = voi.mPlanningRole;
                certifiedVOI.mSubrole = voi.mSubrole;

                if (IsAutoSegmentSupported_i(certifiedVOI.mName)) {
                    autoAddVoiList.VOIList.push_back(certifiedVOI);
                }
                else{
                    addVoiList.push_back(certifiedVOI);
                }
        });

        AutoCoutouringCmd cmd(autoAddVoiList);
        int result = ProcessCommand(&cmd);
        if (result != TPS_ER_SUCCESS) {
            return result;
        }

        result = AddVoiList_i(addVoiList);
        if (result != TPS_ER_SUCCESS) {
            return result;
        }

        voiList.resize(autoAddVoiList.VOIList.size() + addVoiList.size());

        std::copy(autoAddVoiList.VOIList.begin(), autoAddVoiList.VOIList.end(), voiList.begin());
        std::copy(addVoiList.begin(), addVoiList.end(), voiList.begin() + autoAddVoiList.VOIList.size());

        return result;
    }
    else {

        std::for_each(mAddVOIInTemplateCollection.begin(), mAddVOIInTemplateCollection.end(),
            [&](SimpleVOI& voi) {

                CertifiedVOI certifiedVOI;
                certifiedVOI.mVoiUid = mDatabaseWrapper->GeneraterUid();
                certifiedVOI.mSeriesUid = seriesUid;
                certifiedVOI.mName = voi.mName;
                certifiedVOI.mColor[0] = voi.mColor[0];
                certifiedVOI.mColor[1] = voi.mColor[1];
                certifiedVOI.mColor[2] = voi.mColor[2];
                certifiedVOI.mColor[3] = 0.0f;
                certifiedVOI.mPlanningRole = voi.mPlanningRole;
                certifiedVOI.mSubrole = voi.mSubrole;

                voiList.push_back(certifiedVOI);
        });

        return AddVoiList_i(voiList);
    }
}

bool TpsBLController::IsAutoSegmentSupported_i(const std::string voiName)
{
    std::string sOrganNameToupper(voiName);
    (void)std::transform(voiName.begin(), voiName.end(), sOrganNameToupper.begin(),
        static_cast<int(*)(int)>(std::toupper));

    for (int index = 0; index < UNKNOWN; ++index){
        if (0 == strcmp(Enum2String_ORGAN_TYPE[index], sOrganNameToupper.c_str())){
            return true;
        }
    }

    return false;
}

int TpsBLController::AddVoiList_i(std::vector<CertifiedVOI>& voiList)
{
    for (auto it = voiList.begin(); it != voiList.end(); it++)
    {
        auto innerRoi = new RtVoi(true);
        ROIEntity* voiEntity = mDataRepository->GetVoiManager()->CreateROIEntity(innerRoi);
        if (voiEntity == nullptr) {
            TPS_LOG_DEV_ERROR << "Failed to CreateVOIEntity";
            return TPS_ER_FAILURE;
        }

        //TODO: get data from certified voi
        const std::string imageUID = (*it).mSeriesUid;

        innerRoi->set_uid((*it).mVoiUid);
        innerRoi->set_structuresetuid((*it).mVoiUid);
        innerRoi->set_seriesuid((*it).mSeriesUid);
        innerRoi->set_isvisible((*it).mIsVisible);
        innerRoi->set_color((*it).mColor);
        innerRoi->set_name((*it).mName);
        voiEntity->SetIsQA(false);
        innerRoi->set_planningrole((*it).mPlanningRole);
        innerRoi->set_subrole((*it).mSubrole);
        innerRoi->set_locktype((*it).mLockType);

        TpsImage3DEntity *tpsImage3DEntity = nullptr;
        if (!mDataRepository->GetImageDataManager()->GetImage3DData(imageUID, &tpsImage3DEntity)
            || tpsImage3DEntity == nullptr) {
                TPS_LOG_DEV_ERROR << "The image 3d does not exist: " << imageUID;
                return TPS_ER_FAILURE;
        }

        RtImage3DHeader *header = nullptr;
        if (!tpsImage3DEntity->GetImage3DHeader(header)
            || header == nullptr) {

                TPS_LOG_DEV_ERROR << "Failed to GetImage3DHeader" << imageUID;
                return TPS_ER_FAILURE;
        }

        Mcsf::Matrix4f matP2V;
        if (!tpsImage3DEntity->GetVolumeToPatientMatrix(matP2V)){
            TPS_LOG_DEV_ERROR << "Failed to get volume to patient matrix of image:" << imageUID;
            return TPS_ER_FAILURE;
        }

        matP2V = matP2V.Inverse();
        float arrP2V[16];
        matP2V.GetElements(arrP2V);
        innerRoi->set_pat2volumematrix(arrP2V);

        if (!mDataRepository->GetVoiManager()->AddROI(voiEntity)) {
            TPS_LOG_DEV_ERROR << "Failed to AddVOI to manager" << imageUID;
            return TPS_ER_FAILURE;
        }

        std::string voiuid = voiEntity->GetUid();
        if(TPS_ER_SUCCESS != mSaveObjects->SaveRoi(*voiEntity)){
            TPS_LOG_DEV_ERROR<<"Failed to save voi: "<<voiuid <<" to database.";
        }

        if (!mRenderProxy->AddVOI(voiuid, imageUID)){
            TPS_LOG_DEV_ERROR<<"Failed to add voi: "<<voiuid<<" to render proxy.";
            return TPS_ER_FAILURE;
        }
        mRenderProxy->Render();
    }

    return TPS_ER_SUCCESS;
}
bool TpsBLController::LoadEasyPlanData( const std::string &patientUid )
{
    bool bRet = true;
    bRet &= mLoadObjects->LoadPatientTreeForEasyplan(patientUid);

    return bRet;
}

//bool TpsBLController::EasyPlanEditMlc(CertifiedMlcEditInfo& certInfo, 
//    /*const std::string& beamuid, 
//    const std::string& segmentuid,*/
//    int apertureType) {
//        // firstly make adjustment to certified info.
//        if (certInfo.mIsEditingStarted){
//            switch(certInfo.mActionType){
//            case MOVE:
//                certInfo.mRealActionType = UPDATE_CANDIDATE_POINT;
//                break;
//            case DRAG:
//            case DOWN:
//                certInfo.mRealActionType = ADD_NEW_POINT;
//                break;
//            case DOUBLE_CLICK_:
//                certInfo.mRealActionType = CLOSE_CONTOUR;
//                break;
//            default:
//                if(certInfo.mActionType == UP && (certInfo.mMlcEditMode== MLC_CIRCLE_MODE ||
//                    certInfo.mMlcEditMode == MLC_RECTANGLE_MODE)) {
//                        certInfo.mRealActionType = CLOSE_CONTOUR;
//                }
//                else {
//                    certInfo.mRealActionType = SMART_CONTOUR_INVALID;
//                }
//            }
//        }
//        else if (certInfo.mActionType == DOWN){
//            certInfo.mRealActionType = CREATE_NEW_CONTOUR;
//        }
//        else {
//            certInfo.mRealActionType = SMART_CONTOUR_INVALID;
//        }
//
//        mRenderProxy->EasyPlanDrawMlcSmartAperture(certInfo);
//
//        if(certInfo.mActionType != CLOSE_CONTOUR) {
//            return true;
//        }
//        UpdateMLCNBlockShape(certInfo.mBeamUid, /*segmentuid,*/ apertureType);
//        return true;
//}

bool TpsBLController::UpdateMLCNBlockShape(
    const std::string& beamuid, 
    //const std::string& segmentuid,
    int apertureType)
{
    RtBeam* beam = nullptr;
    beam = mDataRepository->GetBeamManager()->GetBeam(beamuid);
    if (beam == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get current active beam.";
        return false;
    }
    std::string normGroupuid = beam->get_normgroupuid();
    std::string planuid = mDataRepository->GetNormgroupManager()->GetPlanUIDByNormGroupUID(normGroupuid);

    //Update MLC in local memory and DB
    if (apertureType == (int)RtDbDef::EasyPlan_MLC){

        // get current leaf positions from GOs
        plunc::LEAF_POSITION_STC* leafList = nullptr;
        int leafNum = 0;
        if(!mRenderProxy->EasyPlanGetCurrentMlcData(planuid, leafList, leafNum)){
            TPS_LOG_DEV_ERROR<<"Failed to get current mlc data.";
            return false;
        }
        // update these leaf positions data into beam manager and TMS DB
        std::vector<RtBeamsegment*> beamSegments = beam->get_beamsegments();
        if(beamSegments.size() != 1) {
            TPS_LOG_DEV_ERROR<<"Failed to get current EasyPlan activated beam's segment.";
            return false;
        }
        RtBeamsegment* beamSegment = nullptr;
        RtMlcshape* startMlcShape = nullptr;
        RtMlcshape* endMlcShape = nullptr;
        // EasyPlan beam only has one segment.
        /*for(int i=0; i<beamSegments.size(); i++) {
        if(beamSegments[i]->get_uid() == segmentuid) {
        beamSegment = beamSegments[i];
        break;
        }
        }*/
        beamSegment = beamSegments[0];

        if(beamSegment == nullptr) {
            TPS_LOG_DEV_ERROR<<"Failed to get current active segment.";
            return false;
        }
        startMlcShape = beamSegment->get_startmlcshape();
        endMlcShape = beamSegment->get_endmlcshape();
        if(startMlcShape == nullptr) {
            TPS_LOG_DEV_ERROR<<"The segment has no start mlc shape.";
            return false;
        }
        if(endMlcShape == nullptr) {
            TPS_LOG_DEV_ERROR<<"The segment has no end mlc shape.";
            return false;
        }
        std::vector<db_Point2d> leafPos;
        for (int i = 0; i < leafNum; ++i){
            leafPos.push_back(db_Point2d(leafList[i].pos_1, leafList[i].pos_2));
        }
        // update mlc leaf positions into manager
        startMlcShape->set_leafpositions(leafPos);
        endMlcShape->set_leafpositions(leafPos);
        beam->set_beamsegments(beamSegments);
        // update mlc leaf positions into DB
        if (!mSaveObjects->SaveBeam(beamuid)) {
            TPS_LOG_DEV_INFO<<"Failed to save beam to tms!";
            /*return false;*/
        }
    }
    // Update Block in local memory and DB
    else if (apertureType == (int)RtDbDef::EasyPlan_Aperture_Block){

        // get current block data from GOs
        std::vector<Mcsf::MedViewer3D::Point2D> vecData;
        if (!mRenderProxy->EasyPlanGetCurrentBlockData(planuid, vecData)){
            TPS_LOG_DEV_ERROR<<"Failed to get current block data.";
            return false;
        }
        // update the block data into both beam manager and DB
        RtBlock* block = beam->get_aperture_block();
        if (block == nullptr){
            TPS_LOG_DEV_ERROR<<"The active beam has no block.";
            return false;
        }
        std::vector<db_Point2f> blockData;
        for (auto it = vecData.begin(); it != vecData.end(); ++it) {
            db_Point2f tmsPoint;
            tmsPoint.x = it->x;
            tmsPoint.y = it->y;
            blockData.push_back(tmsPoint);
        }
        // update block data into beam manager
        block->set_points(blockData);
        // update block data into DB
        if (!mSaveObjects->SaveBeam(beamuid)) {
            TPS_LOG_DEV_INFO<<"Failed to save beam to tms!";
            /*return false;*/
        }
    }

    return true;
}

bool TpsBLController::EasyPlanZoomImage( WINDOW_TYPE wndType, const std::string& wndUid, const std::string& beamUid, float scaleFactor )
{
    return mRenderProxy->EasyPlanZoomImage(wndType, wndUid, beamUid, scaleFactor);
}

bool TpsBLController::EasyPlanRotateImage( WINDOW_TYPE wndType, const std::string& wndUid, const std::string& beamid, 
    float startX, float startY, float stopX, float stopY )
{
    return mRenderProxy->EasyPlanRotateImage(wndType, wndUid, beamid, startX, startY, stopX, stopY);
}

bool TpsBLController::EasyPlanSetPanImage( int windowType, std::string &windowUid, std::string &planUid, std::string &beamUid,
    float startX, float startY, float stopX, float stopY, double viewerWidth, double viewerHeight )
{
    return mRenderProxy->EasyPlanSetPanImage(windowType, windowUid, planUid, beamUid, startX, startY, stopX, stopY, 
        viewerWidth, viewerHeight);
}

bool TpsBLController::EasyPlanWindowingImage( int windowType, std::string &windowUid, std::string &beamUid,
    int deltaWindowWidth, int deltaWindowCenter, int& currentWinWidth, int& currentWinCenter)
{
    return mRenderProxy->EasyPlanWindowingImage(windowType, windowUid, beamUid, deltaWindowWidth, deltaWindowCenter, currentWinWidth, currentWinCenter);
}

bool TpsBLController::EasyPlanMesureDistance(WINDOW_TYPE windowType, std::string &windowUid, std::string &beamUID,
    float startX, float startY, float stopX, float stopY, bool actionStarted, bool actionDone, double& distanceInPatient)
{
    actionStarted;

    // send back the calculated distance and display
    if (!actionDone){
        // calculate the distance in patient coordinate of start point and cursor point on screen
        TpsImage2DEntity *image2DData = nullptr;// = new TdaImage2DEntity();
        int iRet = mDataRepository->GetImageDataManager()->Get2DImageDataByBeamUID(beamUID, image2DData);
        if (!iRet)
        {
            distanceInPatient = 0;
            return true;
        }

        int imageWidth, imageHeight;
        std::pair<float, float> imagePlanePixalSpacing;
        image2DData->GetImageSize(imageWidth, imageHeight);
        imagePlanePixalSpacing.first = image2DData->get()->get_firstimageplanepixelspacing();
        imagePlanePixalSpacing.second = image2DData->get()->get_secondimageplanepixelspacing();

        float convertedStartX = 0.0f;
        float convertedStartY = 0.0f;
        float convertedStopX = 0.0f;
        float convertedStopY = 0.0f;
        Mcsf::Point2f ptIn;
        Mcsf::Point3f ptOut;
        ptIn.SetX(startX);
        ptIn.SetY(startY);
        mRenderProxy->TransformPoint(windowType, windowUid, ptIn, ptOut);
        convertedStartX = ptOut.GetX();
        convertedStartY = ptOut.GetY();
        ptIn.SetX(stopX);
        ptIn.SetY(stopY);
        mRenderProxy->TransformPoint(windowType, windowUid, ptIn, ptOut);
        convertedStopX = ptOut.GetX();
        convertedStopY = ptOut.GetY();
        float startX_InImage = convertedStartX * imageWidth;
        float startY_InImage = convertedStartY * imageHeight;
        float cursorX_InImage = convertedStopX * imageWidth;
        float cursorY_InImage = convertedStopY * imageHeight;

        double startX_InPatient = startX_InImage * imagePlanePixalSpacing.first;
        double startY_InPatient = startY_InImage * imagePlanePixalSpacing.second;
        double cursorX_InPatient = cursorX_InImage * imagePlanePixalSpacing.first;
        double cursorY_InPatient = cursorY_InImage * imagePlanePixalSpacing.second;

        double x_dis_InPatient = (cursorX_InPatient - startX_InPatient) * (cursorX_InPatient - startX_InPatient);
        double y_dis_InPatient = (cursorY_InPatient - startY_InPatient) * (cursorY_InPatient - startY_InPatient);

        distanceInPatient = sqrt(x_dis_InPatient + y_dis_InPatient);
    }
    return true;
}

bool TpsBLController::EasyPlanResetImage( int windowType, std::string &windowUid, std::string &beamUid )
{
    return mRenderProxy->EasyPlanResetImage(windowType, windowUid, beamUid);
}

bool TpsBLController::EasyPlanInverseImage( int windowType, const std::string &windowUid, const std::string &beamUid )
{
    return mRenderProxy->EasyPlanSetInverse(windowType, windowUid, beamUid);
}


//bool TpsBLController::EasyPlanChangeApertureType( std::string &strPlanUid, std::string &strBeamUid, int apertureType )
//{
//    return mRenderProxy->EasyPlanChangeApertureType(strPlanUid, strBeamUid, apertureType);
//}

void TpsBLController::EasyPlanChangeSelectedPlan( std::string &strPlanUid )
{
    TpsImageDataManager *imageDataManager = mDataRepository->GetImageDataManager();
    std::string imageUid;
    imageDataManager->GetEasyPlanImageUID(strPlanUid, &imageUid);
    mRenderProxy->SetDataSource(SOURCE_GROUP_EZPLAN, IMAGE, imageUid);
    mRenderProxy->SetDataSource(SOURCE_GROUP_EZPLAN, PLAN, strPlanUid);
    mRenderProxy->Render();
}

int TpsBLController::AddBeamFromTemplate(std::string /*beamTemplateUid*/, std::string /*normGroupUid*/, std::string /*commissionedUnitUid*/, std::string /*isocenterUid*/,
    proto::RtTpsProtoRepeatedBeam& /*outputBeamList*/)
{
    int result = TPS_ER_SUCCESS;
    /*
    BeamTemplate* beamTemplate;
    if (!mDataRepository->GetConfigurationManager()->GetBeamTemplate(beamTemplateUid, &beamTemplate)) {

    result = TPS_ER_FAILURE;
    return result;
    }

    for (auto itBeam = beamTemplate->mBeamCollection.begin();
    itBeam != beamTemplate->mBeamCollection.end(); itBeam++) {

    TpsCertifiedBeam certifiedBeam;
    certifiedBeam.mBeamUID = mDatabaseWrapper->GeneraterUid();
    certifiedBeam.mBeamName = "TemplatizedBeam";
    certifiedBeam.mBeamID = "Templatized";
    certifiedBeam.mIsAutoShapedJaw = true;
    certifiedBeam.mNormGroupUID = normGroupUid;
    certifiedBeam.mCommissionedUnitUID = commissionedUnitUid;
    certifiedBeam.mIsocenterUid = isocenterUid;
    certifiedBeam.mGantryStartAngle = itBeam->mGantryStartAngle;
    certifiedBeam.mGantryEndAngle = itBeam->mGantryEndAngle;
    certifiedBeam.mCollimatorAngle = itBeam->mCollimatorAngle;
    certifiedBeam.mCouchAngle = itBeam->mCouchAngle;

    proto::RtTpsProtoBeam* pProtoBeam = outputBeamList.add_beamlist();
    CreateBeamCmd cmd(certifiedBeam);
    result = ProcessCommand(&cmd);
    const RtBeam* pBeam = nullptr;
    cmd.GetBeamCreated(pBeam);
    if (result != TPS_ER_SUCCESS || nullptr == pBeam) {
    return result;
    }
    //ProtoBeamConverter::ConverterToProto(pBeam, pProtoBeam);
    }
    */
    return result;
}

bool TpsBLController::EasyPlanMesureAngle(int windowType, const std::string& windowUid, const std::string& beamUid,
    const std::list<std::pair<double, double>>& points, float cursorX, float cursorY,
    bool actionStarted, bool firstSideDone, bool secondSideDone)
{
    std::list<Mcsf::MedViewer3D::Point2D> anglePoints;
    for (std::list<pair<double, double>>::const_iterator iter = points.begin(); iter != points.end(); iter++)
    {
        Mcsf::MedViewer3D::Point2D newAnglePoint;
        newAnglePoint.x = iter->first;
        newAnglePoint.y = iter->second;
        anglePoints.push_back(newAnglePoint);
    }
    mRenderProxy->EasyPlanMesureAngle(windowType, windowUid, beamUid, anglePoints, cursorX, cursorY, actionStarted, firstSideDone, secondSideDone);
    return true;
}

int TpsBLController::DeleteBeamTemplate(const std::string& beamTemplateUID) {

    if (DB_CODES_SUCCESS != mDatabaseWrapper->EraseObject(beamTemplateUID, RtDbDef::TYPE_TEMPLATE)) {
        TPS_LOG_DEV_ERROR << "Failed to save BeamTemplate config in DB with Config UID:"
            << beamTemplateUID;
        return TPS_ER_DB_ERROR;
    }

    if (!mDataRepository->GetConfigurationManager()->RemoveSystemConfig(beamTemplateUID)) {
        TPS_LOG_DEV_ERROR << "Failed to remove BeamTemplate config in TpsConfigurationManager with Config UID:"
            << beamTemplateUID;
        return TPS_ER_FAILURE;
    }

    if (!mDataRepository->GetConfigurationManager()->RemoveBeamTemplate(beamTemplateUID)) {
        TPS_LOG_DEV_ERROR << "Failed to remove BeamTemplate in TpsConfigurationManager with Config UID:"
            << beamTemplateUID;
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int TpsBLController::PreviewBeamTemplate(std::string beamTemplateUid, std::string normGroupUid, std::string isocenterUid) {

    mRenderProxy->PreviewBeamTemplate(beamTemplateUid, normGroupUid, isocenterUid);

    return TPS_ER_SUCCESS;
}

int TpsBLController::GetCornerInfoOfMPR(
    int wndType, 
    const std::string& wndUid,
    double normPosX, 
    double normPosY,
    int suvUnitType,
    CornerTextMPR* cornerInfo)
{
    if (cornerInfo==nullptr) 
    {
        TPS_LOG_DEV_ERROR << "cornerInfo==nullptr";
        return TPS_ER_FAILURE;
    }

    bool bRet = mRenderProxy->GetMprCornerInfo(
        (WINDOW_TYPE)wndType, 
        wndUid, 
        normPosX, 
        normPosY, 
        suvUnitType,
        cornerInfo->density,
        cornerInfo->value,
        cornerInfo->dosevalue,
        cornerInfo->windowcenter,
        cornerInfo->windowwidth,
        cornerInfo->slicenumber,
        cornerInfo->startSliceNumber,
        cornerInfo->slicethickness,
        cornerInfo->pixelpositionx,
        cornerInfo->pixelpositiony,
        cornerInfo->pixelpositionz, 
        cornerInfo->isValueValid, 
        cornerInfo->modality);

    if (!bRet)
    {
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

int TpsBLController::GetDynamicCornerInfoOfMPR(
    int windowType, 
    const std::string& windowUid, 
    double normPosX, 
    double normPosY,
    int suvUnitType, 
    double& density, 
    double& value, 
    double& doseValue,
    double& positionx, 
    double& positiony, 
    double& positionz, 
    bool& isValueValid)
{
    if (!mRenderProxy->GetMprDynamicCornerInfo(
        (WINDOW_TYPE)windowType, windowUid, 
        normPosX, normPosY, 
        suvUnitType, density, value, doseValue, 
        positionx, positiony, positionz,
        isValueValid))
    {
        TPS_LOG_DEV_ERROR << "[GetDynamicCornerInfoOfMPR]:Failed to get dynamic corner info";
        return TPS_ER_FAILURE;
    }
    else
    {
        return TPS_ER_SUCCESS;
    }
}

int TpsBLController::GetWindowCenterWidthOfMPR( int windowType, 
    const std::string& windowUid,
    int suvUnitType,
    double& windowCenter, //Top if PT
    double& windowWidth,  //Bottom if PT
    bool& isValueValid)
{
    if (!mRenderProxy->GetMprWindowCenterWidth(
        (WINDOW_TYPE)windowType, windowUid, suvUnitType, windowCenter, windowWidth, isValueValid))
    {
        TPS_LOG_DEV_ERROR << "[GetWindowWidthLevelOfMPR]:Failed to get mpr widnowwidthlevel info";
        return TPS_ER_FAILURE;
    }
    else
    {
        return TPS_ER_SUCCESS;
    }
}

int TpsBLController::GetSliceInfoOfMPR(int windowType, 
    const std::string& windowUid,
    int& sliceIndex,
    int& startSliceNumber,
    int& totalSlices,
    double& sliceThickness,
    float& sliceLocation)
{
    if (!mRenderProxy->GetMprSliceInfo(
        (WINDOW_TYPE)windowType, windowUid, sliceIndex, startSliceNumber, totalSlices,
        sliceThickness, sliceLocation))
    {
        TPS_LOG_DEV_ERROR << "[GetMprSliceInfo]:Failed to get mpr slice info";
        return TPS_ER_FAILURE;
    }
    else
    {
        return TPS_ER_SUCCESS;
    }
}

int TpsBLController::GetCornerInfoOfBEV(const std::string& /*imageUID*/, 
    int /*wndType*/, const std::string& /*wndUid*/, CornerTextBEV* cornerInfo)
{
    if (cornerInfo==nullptr) {
        TPS_LOG_DEV_ERROR<<"cornerInfo==nullptr";
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int TpsBLController::SetRulerVisibility(std::string strSeriesUid, bool visible)
{
    std::shared_ptr<SetRulerVisibilityOperation> spOpSetRulerVisibility(new SetRulerVisibilityOperation(strSeriesUid, visible));
    mRenderProxy->ProcessOperation(spOpSetRulerVisibility);

    mRenderProxy->Render();
    return TPS_ER_SUCCESS;
}
int TpsBLController::GetPixelSpacingIn10MMOnBEV(int windowType, const std::string& windowUid, 
    double& spacingx, double& spacingy)
{
    if (!mRenderProxy->GetPixelSpacingIn10MMOnBEV((WINDOW_TYPE)windowType, windowUid,
        spacingx, spacingy)) {
            return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

bool TpsBLController::GetBevViewDirecrion(WINDOW_TYPE windowType, const std::string& windowUid,
    double& x1, double& y1, double& z1, double& x2, double& y2, double& z2) {
        if (!mRenderProxy->GetBevViewDirecrion(windowType, windowUid, x1, y1, z1, x2, y2, z2)) {
            TPS_LOG_DEV_ERROR << "Failed to GetBevViewDirecrion.";
            return false;
        }
        return true;
}

bool TpsBLController::SaveEasyPlan( std::string planUID, std::string reviewerName )
{
    // Save easy plan should be an atomic operation, any particular failure when saving easyplan 
    //  should cause the whole easyplan to fail to save to DB
    mDatabaseWrapper->BeginTrans();
    //1. save plan first because following savings have foreign keys constraint
    RtPlan* pPlan = nullptr;
    TpsPatientManager *patientManager = mDataRepository->GetPatientManager();
    if (!patientManager->GetPlan(planUID, &pPlan)
        || pPlan == nullptr){
            mDatabaseWrapper->RollBack();
            TPS_LOG_DEV_ERROR<<"The plan with UID: "<<planUID<<" does not exist.";
            return false;
    }

    //Save EasyPlan.delete by qiangqiang.zhou
    //pPlan->SetRTImageUID(currentRTImage);
    //pPlan->SetApprovalStatus(RTFWK::APPROVED);

    if (TPS_ER_SUCCESS != mSaveObjects->SavePlan(*pPlan)) {
        mDatabaseWrapper->RollBack();
        TPS_LOG_DEV_ERROR<<"Failed to save plan in database.";
        return false;
    }

    //2. save points 
    std::vector<RtPoi*> easyPlanPoints;
    TpsPOIManager* poiManager = mDataRepository->GetPoiManager();
    //std::string isocenterUID("");
    std::string seriesUid = mDataRepository->GetPatientManager()->GetSeriesUidByPlanUid(planUID);
    easyPlanPoints = poiManager->GetAllPoiInSeries(seriesUid);

    //std::string normPointUID("");
    //for (auto iter = easyPlanPoints.begin(); iter != easyPlanPoints.end(); ++iter) {
    //    std::string strPoiUid = (*iter)->get_uid();
    //    //if (!RTFWK::RTFWKDLDataAccess::GetInstance()->SaveObjectToDB(*iter) ){
    //    if (TPS_ER_SUCCESS != mSaveObjects->SavePoi(*(*iter))){
    //        mDatabaseWrapper->RollBack();
    //        TPS_LOG_DEV_ERROR<<"Failed to save poi with UID:"
    //            << strPoiUid;
    //        continue;
    //    }
    //    if ((*iter)->get_type() == RTFWK::NormalPoint) {
    //        normPointUID = strPoiUid;
    //    }
    //    else if((*iter)->get_type() == RTFWK::IsoCenter) {
    //        isocenterUID = strPoiUid;
    //    }
    //}

    //3. save normgroup
    TpsNormGroupManager* normgroupManager = mDataRepository->GetNormgroupManager();
    std::vector<std::string> normGroupUIDList = normgroupManager->GetNormGroupUIDListByPlan(planUID);
    if (normGroupUIDList.size() != 1) {
        mDatabaseWrapper->RollBack();
        TPS_LOG_DEV_ERROR<<"Normgroup count is abnormal. ";
        return false;
    }

    RtNormgroup *normGroup = nullptr;
    if (!normgroupManager->GetNormGroup(normGroupUIDList[0], &normGroup)
        || normGroup == nullptr) {
            mDatabaseWrapper->RollBack();
            TPS_LOG_DEV_ERROR<<"get normGroup from TpsNormGroupManager failed with normGroup UID:"
                << normGroupUIDList[0];
            return false;
    }

    // normGroup->SetApprovalStatus(RTFWK::APPROVED);

    //normGroup->SetISOCenterPoiUID(isocenter->GetUID());
    //normGroup->set_normalpointpoiuid(normPointUID);
    //normGroup->SetBeams();

    // this save normal group alse saved beams associated with it.
    //normGroup->set_softwareversionno(mDatabaseWrapper->GetTpsVersion());
    normGroup->set_tpsequipmentuid(mDatabaseWrapper->GetDefaultEquipmentUid());
    if (TPS_ER_SUCCESS != mSaveObjects->SaveNormgroup(*normGroup)) 
    {
        mDatabaseWrapper->RollBack();
        TPS_LOG_DEV_ERROR<<"Failed to save normgroup in database.";
        return false;
    }

    // 4. save rt image
    /*std::string currentRTImage("");
    TpsImageDataManager* imageManager = mDataRepository->GetImageDataManager();
    imageManager->GetEasyPlanImageUID(planUID,&currentRTImage);
    imageManager->GetEasyPlanBeamImageUID()
    if(!currentRTImage.empty()) {
    tps::RtRtimage* rtImage = nullptr;
    tps::TpsImage2DEntity* _2dImageEntity = nullptr;
    imageManager->GetImage2DData(currentRTImage,&_2dImageEntity);
    if(_2dImageEntity != nullptr){
    rtImage = _2dImageEntity->get();
    }
    if(rtImage != nullptr){
    if (TPS_ER_SUCCESS != mSaveObjects->SaveRtImage(*rtImage)) {
    TPS_LOG_DEV_ERROR<<"Failed to save rt image with image UID:"
    << currentRTImage;
    return false;
    }
    }
    }*/

    //5. save beam (include isocenter)
    std::vector<std::string> beamUIDList;
    TpsBeamManager* beamManager = mDataRepository->GetBeamManager();
    beamUIDList = beamManager->GetBeamUIDListByNormgroupUID(normGroupUIDList[0]);
    if(beamUIDList.size() == 0) {
        mDatabaseWrapper->RollBack();
        TPS_LOG_DEV_ERROR<<"Failed to get beam uid list or the list is empty. ";
        return false;
    }

    for (auto iter = beamUIDList.begin(); iter != beamUIDList.end(); ++iter) {
        RtBeam *tmsBeam = nullptr;

        tmsBeam = beamManager->GetBeam(*iter);
        if (tmsBeam == nullptr) {
            //only update current beam
            continue;
        }

        // beam save to DB will be failed without this isoCenterPOIUID.
        //if(isocenterUID.empty()) {
        //    // the default isocenter should be the middle of screen, may not be normal group.
        //    // so here may need to modified.
        //    tmsBeam->set_isocenterpoiuid(normPointUID);
        //}
        //else {
        //    tmsBeam->set_isocenterpoiuid(isocenterUID);
        //}

        // The following save normalgroup will alse save beams.
        if (TPS_ER_SUCCESS != mSaveObjects->SaveBeam(*iter)) {
            mDatabaseWrapper->RollBack();
            TPS_LOG_DEV_ERROR<<"Failed to save beam of beam uid: " << *iter;
            return false;
        }

        // save beam image
        std::string currentRTImage("");
        TpsImageDataManager* imageManager = mDataRepository->GetImageDataManager();
        //imageManager->GetEasyPlanImageUID(planUID,&currentRTImage);
        //imageManager->GetEasyPlanBeamImageUID(tmsBeam->get_uid(), &currentRTImage);
        RtBeam* beam = nullptr;
        beam = beamManager->GetBeam(tmsBeam->get_uid());
        if(beam != nullptr){
            currentRTImage = beam->get_relatedrtimageuid();
        }
        if(!currentRTImage.empty()) {
            tps::RtRtimage* rtImage = nullptr;
            tps::TpsImage2DEntity* _2dImageEntity = nullptr;
            imageManager->GetImage2DData(currentRTImage,&_2dImageEntity);
            if(_2dImageEntity != nullptr){
                rtImage = _2dImageEntity->get();
            }
            if(rtImage != nullptr){
                //rtImage->set_beamuid(tmsBeam->get_uid());
                if (TPS_ER_SUCCESS != mSaveObjects->SaveRtImage(*rtImage)) {
                    mDatabaseWrapper->RollBack();
                    TPS_LOG_DEV_ERROR<<"Failed to save rt image with image UID:"
                        << currentRTImage << "for beam(UID):" << tmsBeam->get_uid();
                    // TODO: Need to check why save RT image failed.
                    //return false;
                }
            }
        }
    }

    // 6. save beam poi dosemap
    for (auto iter = easyPlanPoints.begin(); iter != easyPlanPoints.end(); ++iter) {
        std::string strPoiUid = (*iter)->get_uid();
        // save poi dose map
        if(TPS_ER_SUCCESS != mSaveObjects->UpdateBeamPoiDose(*iter)){
            mDatabaseWrapper->RollBack();
            TPS_LOG_DEV_ERROR<<"Failed to save poi dose map to DB(table tmsbeam_poi) with POI UID:"
                << strPoiUid;
            continue;
        }
    }

    // 6. save dosegrid
    //dataweapper中insertdosegrid已经从insert beam中分离，需要单独保存
    if (TPS_ER_SUCCESS != mSaveObjects->SaveDosegridsForNormgroup(normGroup->get_uid())) 
    {
        mDatabaseWrapper->RollBack();
        TPS_LOG_DEV_ERROR<<"Failed to save SaveDosegridsForNormgroup in database.";
        return false;
    }

    mDatabaseWrapper->Commit();
    return true;
}


int TpsBLController::ApproveUserInfo(RtApproverecord& approverecord)
{
    return mSaveObjects->SaveApproveRecord(approverecord);
}


int TpsBLController::UnApproveUserInfo(const std::string& objectuid)
{
    return mSaveObjects->DeleteApproveRecordViaObjUid(objectuid);
}

int TpsBLController::ApproveEasyPlan(RtApproverecord& approverecord)
{
    std::string planUid = approverecord.get_objectuid();
    if (planUid.empty())
    {
        return TPS_ER_FAILURE;
    }
    std::vector<std::string> normGroupUIDList = mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(planUid);
    if (normGroupUIDList.size() != 1)
    {
        return TPS_ER_FAILURE;
    }

    std::string ngUid = normGroupUIDList[0];

    // save plan approve record to db
    int iRet = mSaveObjects->SaveApproveRecord(approverecord);
    if (iRet != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to save plan approve record in ApproveEasyPlan";
        return iRet;
    }

    approverecord.set_objecttype((int)RtDbDef::Normgroup);
    approverecord.set_objectuid(ngUid);

    // save normalgroup approve record to db
    iRet = mSaveObjects->SaveApproveRecord(approverecord);
    if (iRet != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR << "Failed to save normalgroup approve record in ApproveEasyPlan";
        return iRet;
    }

    return TPS_ER_SUCCESS;
}

int TpsBLController::LoadMachineSetting()
{
    mDataRepository->GetMachineSettingManager()->LoadMachineSetting();
    return true;
}

void TpsBLController::ClearAllPatientLocks()
{
    if (mDatabaseWrapper!=nullptr) {
        mDatabaseWrapper->ClearAllPatientLocks();
    }
}

#pragma region [***************** Gallery ***********************]

bool TpsBLController::LoadSeriesForGallery(std::string seriesUID)
{
    auto ga_img_mgr = mDataRepository->GetGalleryImageManager();
    if (ga_img_mgr->IsSeriesInSeries4D(seriesUID))
    {
        ga_img_mgr->SetIsPlaying(true);
    }

    if (mCurrentImageDataUid == seriesUID){
        return true;
    }

    if (seriesUID == "")
    {
        RenderForInValidSeiresUID();
        return true;
    }

    if(!ga_img_mgr->IsSeriesExists(seriesUID))
    {              
        RenderForInValidSeiresUID();
        TPS_LOG_DEV_ERROR<<"Image3D does not exist.";
        return false;
    }
    ga_img_mgr->SetCurrentImage3D(seriesUID);

    std::string sImage3DUID("");
    TpsImage3DEntity* image3Dptr = nullptr;

    if (!ga_img_mgr->GetCurrentImage3DEntity(sImage3DUID, &image3Dptr))
    {
        return false;
    }

    if (sImage3DUID.empty() || nullptr == image3Dptr){
        TPS_LOG_DEV_WARNING<<"GetDefaultImage3DData failed as mCurrentImageDataUid is empty or image3D is null";
        return false;
    }

    //save current image data UID
    mCurrentImageDataUid = sImage3DUID;

    // TODO Call ControllerBase->SetDataSource; Gallery_mprrendercontroller->ResetDataSource_i; 
    //          Gallery_mprrendercontroller->ResetImage_i;
    //          CheckGraphObject
    mRenderProxy->SetDataSource(SOURCE_GROUP_GALLERY, IMAGE, mCurrentImageDataUid);

    //// TODO : Render Twice here !!!
    //mMPROperator->MPRCornerImageTextVisible(mCurrentImageDataUid, TpsGalleryImageManager::GetInstance()->GetIsVisibleOfImageText());

    mRenderProxy->Render();
    return true;
}

void TpsBLController::RenderForInValidSeiresUID()
{
    //auto group = mRenderProxy->RequestDataSourceGroup(SOURCE_GROUP_COMMON);
    //if (group == nullptr){
    //    TPS_LOG_DEV_ERROR<<"Failed to request common source group.";
    //    return false;
    //}
    //group->SetDataSource(IMAGE, targetUID);


    mRenderProxy->RemoveDataSource(SOURCE_GROUP_GALLERY, IMAGE);  // Set Strategy's GO nullptr(mCurrentImageDataUid);
    mRenderProxy->Render();
    mCurrentImageDataUid = "";
}

int TpsBLController::GetCrosshairMode(std::string seriesUID, int& iCrosshairMode){
    int nResult = mRenderProxy->GetCrosshairMode(seriesUID, iCrosshairMode);
    return nResult;
}

bool TpsBLController::ClearImageDatas()
{
    mCurrentImageDataUid = "";
    if (!mRenderProxy->RemoveGraphicObjects())
    {
        TPS_LOG_DEV_ERROR<<"Failed to leave gallery."; 
        return false;
    }
    else
    {
        mDataRepository->GetGalleryImageManager()->ClearManager();
        //mDataRepository->GetGalleryPatientManager()->ClearManager();
        mDataRepository->GetGalleryPatientManager()->ClearPatientData();
        mRenderProxy->Render();
        return true;
    }
}

bool TpsBLController::ClearImageData(std::string seriesUID)
{
    mCurrentImageDataUid = "";
    if (!mRenderProxy->RemoveGraphicObjects(seriesUID))
    {
        TPS_LOG_DEV_ERROR<<"Failed to clear image data."; 
        return false;
    }
    else
    {
        mDataRepository->GetGalleryImageManager()->ClearImageData(seriesUID);
        mRenderProxy->Render();
        return true;
    }
}

void TpsBLController::GetCurrentSeriesUID(std::string& currentSeriesUID) 
{
    currentSeriesUID = mCurrentImageDataUid;
}

bool TpsBLController::GetCurrentMPRSlice(WINDOW_TYPE strWidowType, const std::string& wndUid,
    int& currentSlice, int& totalSlice)
{
    if (mCurrentImageDataUid.empty()) 
    {
        TPS_LOG_DEV_ERROR<<"[GetCurrentMPRSlice] : mCurrentImageDataUid is empty.";
        return false;
    }
    int startSliceNumber;
    if (!mRenderProxy->GetCurrentMPRSlice(strWidowType, wndUid, currentSlice, startSliceNumber, totalSlice))
    {
        TPS_LOG_DEV_ERROR<<"[GetCurrentMPRSlice] : Failed to GetCurrentMPRSlice.";
        return false;
    }
    return true;
}

bool TpsBLController::DeleteSliceInLocal(const std::string& sSeriesUID, int& currentSlice)
{
    if (mCurrentImageDataUid != sSeriesUID) 
    {
        TPS_LOG_DEV_ERROR<<"[DeleteSliceInLocal] : sSeriesUID not equal to mCurrentImageDataUid .";
        return false;
    }
    if (!mDataRepository->GetGalleryImageManager()->DeleteSlice(sSeriesUID, currentSlice)) 
    {  
        TPS_LOG_DEV_ERROR<<"[DeleteSliceInLocal] : Failed to Delete Slice .";
        return false;
    }
    mRenderProxy->RemoveGraphicObjects(sSeriesUID);
    mCurrentImageDataUid = "";
    LoadSeriesForGallery(sSeriesUID);
    return true;
}

bool TpsBLController::GetDeletedImageUIDs(const std::string& sSeriesUID, std::vector<std::string>& vImageUIDs)
{
    if (&vImageUIDs == nullptr){
        TPS_LOG_DEV_ERROR<<"vImageUIDs vector is null.";
        return false;
    }

    vImageUIDs.clear();

    if (!mDataRepository->GetGalleryImageManager()->GetDeletedImageUIDs(sSeriesUID, vImageUIDs)){
        TPS_LOG_DEV_ERROR<<"[DeleteSlices] : Failed to Get DeletedImageUIDs.";
        return false;
    }
    if (vImageUIDs.size() == 0) {
        TPS_LOG_DEV_ERROR<<"[DeleteSlices] : DeletedImageUIDs is empty.";
        return false;
    }

    return true;
}

bool TpsBLController::BackupOriginalImage3DEntity(const std::string& sSeriesUID)
{
    if(!mDataRepository->GetGalleryImageManager()->BackupOriginalImage3DEntity(sSeriesUID))
    {
        TPS_LOG_DEV_ERROR<<"[BackupOriginalImage3DEntity] : Failed to BackupOriginalImage3DEntity.";
        return false;
    }

    return true;
}

bool TpsBLController::RestoreOriginalImage3DEntity(const std::string& sSeriesUID)
{
    if (!mDataRepository->GetGalleryImageManager()->RestoreOriginalImage3DEntity(sSeriesUID))
    {
        TPS_LOG_DEV_ERROR<<"[RestoreOriginalImage3DEntity] : Failed to RestoreOriginalImage3DEntity.";
        return false;
    }
    mRenderProxy->RemoveGraphicObjects(sSeriesUID);
    mCurrentImageDataUid = "";
    LoadSeriesForGallery(sSeriesUID);
    return true;
}

bool TpsBLController::GalleryMprPageTurn(WINDOW_TYPE wndType, const std::string& wndUid, int iPageturn) {
    if (!mRenderProxy->GalleryMprPageTurn(wndType, wndUid, iPageturn)) {
        TPS_LOG_DEV_ERROR<<"Failed to gallery page turn."; 
        return false;
    } else {
        mRenderProxy->Render();
        return true;
    }
}

bool TpsBLController::GalleryMprPan(WINDOW_TYPE wndType, const std::string& wndUid, 
    float startX, float startY, float stopX, float stopY) {
        if (!mRenderProxy->GalleryMprPan(wndType, wndUid, startX, startY, stopX, stopY)) {
            TPS_LOG_DEV_ERROR<<"Failed to pan gallery mpr."; 
            return false;
        } else {
            mRenderProxy->Render();
            return true;
        }
}

bool TpsBLController::GalleryMprZoom(WINDOW_TYPE wndType, const std::string& wndUid, float scaleFactor){
    if (!mRenderProxy->GalleryMprZoom(wndType, wndUid, scaleFactor)) {
        TPS_LOG_DEV_ERROR<<"Failed to zoom gallery mpr."; 
        return false;
    } else {
        mRenderProxy->Render();
        return true;
    }
}

bool TpsBLController::GalleryMprZoom(WINDOW_TYPE wndType, const std::string& wndUid, 
    float startX, float startY, float stopX, float stopY) {
        if (!mRenderProxy->GalleryMprZoom(wndType, wndUid, startX, startY, stopX, stopY)) {
            TPS_LOG_DEV_ERROR<<"Failed to zoom gallery mpr."; 
            return false;
        } else {
            mRenderProxy->Render();
            return true;
        }
}

bool TpsBLController::GalleryMPRWindowing(WINDOW_TYPE wndType, const std::string& wndUid, float deltaWidth,
    float deltaCenter, bool needUpdateAll){
        if (!mRenderProxy->GalleryMprWindowing(wndType, wndUid, deltaWidth, deltaCenter, needUpdateAll)) {
            TPS_LOG_DEV_ERROR<<"Failed to windowing gallery mpr."; 
            return false;
        } else {
            mRenderProxy->Render();
            return true;
        }
}

bool TpsBLController::GalleryShadedSurfaceRotate(WINDOW_TYPE wndType, const float& prePtX, const float& prePtY,
    const float& curPtX, const float& curPtY)
{
    Mcsf::Point2f prePt(prePtX, prePtY);
    Mcsf::Point2f curPt(curPtX, curPtY);

    std::string seriesUid;
    if (!mRenderProxy->GetDataSource(wndType, IMAGE, seriesUid))
    {
        TPS_LOG_DEV_ERROR<<"Failed to GetDataSource.";
        return false;
    }
    if (!mRenderProxy->GalleryShadedSurfaceRotate(seriesUid, prePt, curPt))
    {
        TPS_LOG_DEV_ERROR<<"Failed to Rotate Gallery Shaded Surface.";
        return false;
    }
    mRenderProxy->Render();
    return true;
}

bool TpsBLController::GetCrosshairProjectionPositionForGallery(WINDOW_TYPE windowType, 
    const std::string& windowUid, float& x2D, float& y2D)
{
    if (!mRenderProxy->GetCrosshairProjectionPositionForGallery(windowType, windowUid,
        x2D, y2D)){
            TPS_LOG_DEV_ERROR<<"Failed to GetCrosshairProjectionPositionForGallery.";
            return false;
    }
    return true;
}

bool TpsBLController::GetCrossXYPointsForGallery(WINDOW_TYPE windowType,
    const std::string& windowUid, double& x1, double& y1, double& x2, double& y2, double& x3, double& y3, double& x4, double& y4)
{
    if (!mRenderProxy->GetCrossXYPointsForGallery(
        (WINDOW_TYPE)windowType, windowUid, x1, y1, x2, y2, x3, y3, x4, y4)){
            TPS_LOG_DEV_ERROR << "Failed to GetCrosshairProjectionPositionForGallery.";
            return false;
    }
    return true;
}

//int TpsBLController::TranslateCrosshairForGallery(WINDOW_TYPE windowType, const std::string& windowUid, 
//    double xStart, double yStart, double xEnd, double yEnd){
//
//        //need to Add
//        //mRenderProxy->UpdateMPRCornerInformation(windowType, windowUid, xEnd, yEnd);
//        mRenderProxy->TranslateCrosshairForGallery(windowType, windowUid, xStart, yStart, xEnd, yEnd);
//        mRenderProxy->Render();
//        return 0;
//}

int TpsBLController::UpdateCrosshairForGallery(
    WINDOW_TYPE windowType, const std::string& windowUid, 
    double xStart, double yStart, double xEnd, double yEnd, int crossMode, int hitMode, double rotateX, double rotateY) {

        //mRenderProxy->UpdateMPRCornerInformationForGallery(windowType, windowUid, xEnd, yEnd);
        mRenderProxy->UpdateCrosshairForGallery(windowType, windowUid, xStart, yStart, xEnd, yEnd, crossMode, hitMode, rotateX, rotateY);
        mRenderProxy->Render();
        return 0;
}

bool TpsBLController::SetCrosshairHittedModeForGallery(int wndType, const std::string& wndUid, int mode){
    bool bRes = mRenderProxy->SetCrosshairHittedModeForGallery(wndType, wndUid, mode);
    if (bRes) mRenderProxy->Render();
    return bRes;
}

bool beamSorter(const RtBeam* first, const RtBeam* second)
{
    return first->get_beamnumber() < second->get_beamnumber();
}

int TpsBLController::SortBeamNumbers(const std::string& normgroupuid, std::string* pBeamUids)
{
    if(nullptr == pBeamUids) return TPS_ER_FAILURE;
    //ORDER BY beamnumber
    std::vector<RtBeam*> beams = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(normgroupuid);
    if (beams.empty()){
        *pBeamUids="";
        return TPS_ER_SUCCESS;
    }

    std::sort(beams.begin(), beams.end(), beamSorter);
    std::vector<std::string> beamuids;
    int index(0);
    mDatabaseWrapper->BeginTrans();
    for (auto itrBeam = beams.begin(); itrBeam!=beams.end(); ++itrBeam){
        (*itrBeam)->set_beamnumber(++index);
        if (1 == index){
            *pBeamUids = (*itrBeam)->get_uid();
        }
        else{
            *pBeamUids += "," + (*itrBeam)->get_uid();
        }

        if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateBeamAttributes(*(*itrBeam))){
            TPS_LOG_DEV_ERROR<<"UpdateBeamAttributes failed for beam:"<< (*itrBeam)->get_uid();
            mDatabaseWrapper->RollBack();
            return TPS_ER_DB_ERROR;
        }
    }
    mDatabaseWrapper->Commit();
    return TPS_ER_SUCCESS;
}

int TpsBLController::GetCornerInfoOfMPRForGallery(const std::string& imageUID, 
    int wndType, const std::string& wndUid, /*const std::string& planUid,*/ 
    double normPosX, double normPosY, bool bUpdateAll, int suvUnitType,
    CornerTextGallery* cornerInfo)
{
    if (cornerInfo==nullptr) {
        TPS_LOG_DEV_ERROR<<"cornerInfo==nullptr";
        return TPS_ER_FAILURE;
    }

    // 以下信息不会随鼠标位置变化而变化
    if (bUpdateAll)
    {
        // 获取图像信息
        TpsGalleryImageManager* imgMgr = mDataRepository->GetGalleryImageManager();
        RtSeries* pSeries = nullptr;
        if (!imgMgr->GetImageSeries(imageUID, &pSeries) || pSeries == nullptr) {
            TPS_LOG_DEV_ERROR<<"Failed to get series obj from manager";
            return TPS_ER_FAILURE;
        }
        RtImage3DHeader& imgHdr = *pSeries->get_header();

        cornerInfo->seriesname = pSeries->get_seriesdescription();
        cornerInfo->patientposition = pSeries->get_patientposition();

        // get seried date and time combined string 
        std::locale fmt(std::locale::classic(), new boost::gregorian::date_facet("%H:%M:%S"));
        std::stringstream ss;
        ss.imbue(fmt);
        ss<< boost::gregorian::to_iso_extended_string(pSeries->get_seriesdate());
        ss<< " ";
        ss<< boost::posix_time::to_simple_string(pSeries->get_seriestime());
        cornerInfo->seriesdatetime = ss.str();  // ss>>seriesDateTime  : Get Unexpected Value

        switch (wndType) {
        case WINDOW_GALLERY_MPR_AXIAL:
            cornerInfo->slicethickness = imgHdr.m_fSliceSpacing;
            break;
        case WINDOW_GALLERY_MPR_CORONAL:
            cornerInfo->slicethickness = imgHdr.m_dYSize;
            break;
        case WINDOW_GALLERY_MPR_SAGITTAL:
            cornerInfo->slicethickness = imgHdr.m_dXSize;
            break;
        }

        cornerInfo->patientname = ""; // 这个字段，应该由前段来填更为合适
    }

    int startSliceNumber;
    //
    mRenderProxy->GetMprCornerInfoForGallery(
        (WINDOW_TYPE)wndType, wndUid, normPosX, normPosY, suvUnitType,
        cornerInfo->density,
        cornerInfo->value,
        cornerInfo->windowcenter,
        cornerInfo->windowwidth,
        cornerInfo->slicenumber,
        startSliceNumber,
        cornerInfo->totalslicenumber,
        cornerInfo->slicethickness,
        cornerInfo->pixelpositionx,
        cornerInfo->pixelpositiony,
        cornerInfo->pixelpositionz, 
        cornerInfo->isValueValid, 
        cornerInfo->modality,
        cornerInfo->zoomfactor,
        cornerInfo->rotate);

    return TPS_ER_SUCCESS;
}

//根据windowType 和windowuid 来重置MPR底图
bool TpsBLController::ResetMPRObjectForGallery(WINDOW_TYPE windowType, const std::string& windowUid)
{
    if (!mRenderProxy->ResetMPRObjectForGallery(windowType, windowUid)) {
        TPS_LOG_DEV_ERROR << "Failed to reset mpr.";
        return false;
    }
    else {
        mRenderProxy->Render();
        return true;
    }
}

bool TpsBLController::GetMprViewDirecrionForGallery(WINDOW_TYPE windowType, const std::string& windowUid,
    double& x1, double& y1, double& z1, double& x2, double& y2, double& z2) {
        if (!mRenderProxy->GetMprViewDirecrionForGallery(windowType, windowUid, x1, y1, z1, x2, y2, z2)) {
            TPS_LOG_DEV_ERROR << "Failed to GetMprViewDirecrion.";
            return false;
        }
        return true; 
}

bool TpsBLController::GetShadedSurfaceViewDirecrionForGallery(WINDOW_TYPE windowType, const std::string& windowUid,
    double& x1, double& y1, double& z1, double& x2, double& y2, double& z2) {
        if (!mRenderProxy->GetShadedSurfaceViewDirecrionForGallery(windowType, windowUid, x1, y1, z1, x2, y2, z2)) {
            TPS_LOG_DEV_ERROR << "Failed to GetShadedSurfaceViewDirecrion.";
            return false;
        }
        return true;
}

//int TpsBLController::MoveTable(std::string imageUid, std::string tableUid, float offSetX, float offSetY, float offsetZ, float angleX, float angleY, float angleZ, float tableDensity)
//{
//    mRenderProxy->MoveTable(imageUid, tableUid,offSetX, offSetY, offsetZ, angleX, angleY, angleZ);
//    //存数据库
//    bool noExistFlag = false;
//    RtSeriestable *seriesTable = mDataRepository->GetTableSettingManager()->GetTableSettingBySeriesUidAndTableUid(imageUid, tableUid);
//    if(seriesTable == nullptr && tableUid != "") 
//	{
//		seriesTable = new RtSeriestable();
//		noExistFlag = true;
//	}
//	if(seriesTable != nullptr)
//	{
//		seriesTable->set_seriesuid(imageUid);
//		seriesTable->set_tableuid(tableUid);
//		seriesTable->set_offsetx(offSetX);
//		seriesTable->set_offsety(offSetY);
//		seriesTable->set_offsetz(offsetZ);
//		seriesTable->set_anglex(angleX);
//		seriesTable->set_angley(angleY);
//		seriesTable->set_anglez(angleZ);
//		seriesTable->set_averagedensity(tableDensity);
//		seriesTable->set_isactive(true);
//	}
//    if(noExistFlag)
//    {
//        if(DB_CODES_SUCCESS != mDatabaseWrapper->InsertSeriestable(*seriesTable))
//        {
//            return TPS_ER_FAILURE;
//        }
//		mDataRepository->GetTableSettingManager()->AddTableSeting(*seriesTable);
//
//    }
//    else
//    {
//		if(tableUid != "")
//		{
//			if(DB_CODES_SUCCESS != mDatabaseWrapper->UpdateSeriestable(*seriesTable))
//			{
//				return TPS_ER_FAILURE;
//			}
//		}
//        std::vector<RtSeriestable*> rtSeriesTables = mDataRepository->GetTableSettingManager()->GetTableSettingsBySeriesUid(imageUid);
//
//        for (auto iter = rtSeriesTables.begin(); iter != rtSeriesTables.end(); ++iter)
//        {
//            if((*iter)->get_tableuid() != tableUid && (*iter)->get_isactive())
//            {
//                (*iter)->set_isactive(false);
//                if(DB_CODES_SUCCESS != mDatabaseWrapper->UpdateSeriestable(**iter))
//                {
//                    return TPS_ER_FAILURE;
//                }
//                break;
//            }
//        }
//    }
//    return TPS_ER_SUCCESS;
//}

#pragma endregion

#pragma region [***************** Plan Report ***********************]

int TpsBLController::InitializeMPRPrinter(const int width, const int height,
    const WindowResponserPtr& responser)
{
    if (nullptr != this->mMPRPrinter)
    {
        delete mMPRPrinter;
    }

    mMPRPrinter = new MPRPrinter(width,height,mDataRepositoryForPlanReport);
    mMPRPrinter->AttachListener(responser);

    return TPS_ER_SUCCESS;
}

int TpsBLController::PrintMPR(std::string planUID, const std::vector<int> sliceNums)
{
    if (nullptr == mMPRPrinter)
    {
        return TPS_ER_FAILURE;
    }

    if (planUID.empty() || sliceNums.empty())
    {
        return TPS_ER_FAILURE;
    }

    mMPRPrinter->SetPlanUID(planUID);

    TpsPatientManager* patientManger = mDataRepositoryForPlanReport->GetPatientManager();
    if (nullptr == patientManger)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get patient manger. ";
        return TPS_ER_FAILURE;
    }

    RtPlan* pPlan = nullptr;
    if (!patientManger->GetPlan(planUID, &pPlan))
    {
        TPS_LOG_DEV_ERROR<<"Failed to get plan of : "<<planUID;
        return TPS_ER_FAILURE;
    }

    mMPRPrinter->SetSeriesUID(pPlan->get_imageseriesuid());

    std::queue<int> seriesSlices;
    for (auto it = sliceNums.cbegin(); it < sliceNums.cend(); ++it)
    {
        seriesSlices.push(*it);
    }

    mMPRPrinter->SetSeriesSlices(seriesSlices);

    mMPRPrinter->Print();

    return TPS_ER_SUCCESS;
}

int TpsBLController::LoadPlanReportData(std::string planUID, const std::string& courseUID)
{
    if (TPS_ER_SUCCESS != mLoadObjectsForPlanReport->LoadMachineTree() ||
        !mLoadObjectsForPlanReport->LoadPlanReportData(planUID, courseUID))
    {
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

int TpsBLController::ComposeDoseForPlanReport(std::string planUID)
{
    if (mLoadObjectsForPlanReport->ComposeDoseForPlanReport(planUID))
    {
        return TPS_ER_SUCCESS;
    }

    return TPS_ER_FAILURE;
}

int TpsBLController::GenerateDVH(std::string planUID)
{
    if (CalculateDVH(planUID))
    {
        TPS_LOG_DEV_ERROR << "UpdateDvhAfterDoseCalculation: Failed to calculate dvh of plan: " << planUID;
        return TPS_ER_FAILURE;
    }
    return TPS_ER_SUCCESS;
}

int TpsBLController::ClearPatientDataForPlanReport()
{
    //Clear render data
    mRenderProxyForPlanReport->RemoveAllDataSources();
    if (nullptr != mMPRPrinter)
    {
        mMPRPrinter->ClearModelWarehouse();
    }

    if (ClearManagerDataForPlanReport_i())
    {
        return TPS_ER_SUCCESS;
    }
    else{
        return TPS_ER_FAILURE;
    }
}

int TpsBLController::ClearPatientData()
{
    if (nullptr == mRenderProxy)
    {
        TPS_LOG_DEV_ERROR<<"mRenderProxy is null.";
        return  TPS_ER_FAILURE;
    }

    //Clear render data

    ADD_WORKFLOW_KEYPOINT << "[ClearPatientData]:" << "RemoveAllDataSource start";
    mRenderProxy->RemoveAllDataSources();
    ADD_WORKFLOW_KEYPOINT << "[ClearPatientData]:" << "RemoveAllDataSource end";

    ADD_WORKFLOW_KEYPOINT << "[ClearPatientData]:" << "ClearAllGrpahicObjects start";
    mRenderProxy->ClearAllGrpahicObjects();
    ADD_WORKFLOW_KEYPOINT << "[ClearPatientData]:" << "ClearAllGrpahicObjects end";

    ADD_WORKFLOW_KEYPOINT << "[ClearPatientData]:" << "Render start";
    mRenderProxy->Render();
    ADD_WORKFLOW_KEYPOINT << "[ClearPatientData]:" << "Render end";

    //clear redo/undo command list
    mDataRepository->GetCommandManager()->Reset();

    if (ClearManagerData_i())
    {
        return TPS_ER_SUCCESS;
    }
    else
    {
        return TPS_ER_FAILURE;
    }
}

#pragma endregion
int TpsBLController::CopyContour(const std::string& seriesUID,const std::string& voiUID)
{
    if (!mRenderProxy->EditContour(seriesUID, voiUID, EDIT_COPY))
    {
        return TPS_ER_FAILURE;
    }

    mRenderProxy->Render();

    return TPS_ER_SUCCESS;
}

int TpsBLController::CutContour(const std::string& seriesUID,const std::string& voiUID)
{
    ROIEntity *voiEntity = mDataRepository->GetVoiManager()->GetROI(voiUID);
    if (nullptr == voiEntity){
        TPS_LOG_DEV_ERROR<<"no voi with the voi UID:"<<voiUID;
        return false;
    }

    if (!mRenderProxy->EditContour(seriesUID, voiUID, EDIT_CUT))
    {
        return TPS_ER_FAILURE;
    }

    mRenderProxy->Render();

    std::string imageUID = voiEntity->GetSeriesUid();
    TpsImage3DEntity *image3D = nullptr;
    TpsImageDataManager* imageMng = mDataRepository->GetImageDataManager();
    if (!imageMng->GetImage3DData(imageUID, &image3D) || image3D == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d data.";
        return false;
    }

    //update voi statistical information
    RtCt2density* pCt2Density = imageMng->GetCT2DensityTableByImageUid(imageUID);
    if (!SubVolumeHelperAPI::UpdateVOIStatisticalInformation(pCt2Density, *image3D, voiEntity))
    {
        TPS_LOG_DEV_ERROR<<"Failed to update statistical information.";
        return false;

    }
  
    if(mSaveObjects->SaveRoi(*voiEntity) != TPS_ER_SUCCESS){
        TPS_LOG_DEV_ERROR<<"SaveVOItoTMS failed VOI UID:"<<voiUID;
        return false;
    }

    //delete all related commands in redo/undo list of CommandManager
    auto mng = mDataRepository->GetCommandManager();
    TpsSafeLock<TpsCommandManager> lock(mng);
    mng->RemoveCommands([&](TpsRedoUndoCommand* cmd)->bool{
        auto voiCmd = dynamic_cast<TpsVOIRedoUndoCmdBase*>(cmd);
        return voiCmd != nullptr && voiCmd->GetVoiUid() == voiUID;
    });
    return TPS_ER_SUCCESS;
}

int TpsBLController::RetrieveVoiDensity(const std::string& voiUID, ROIEntity*& tpsVOIEntity)
{
    tpsVOIEntity = mDataRepository->GetVoiManager()->GetROI(voiUID);
    if (nullptr == tpsVOIEntity){
        TPS_LOG_DEV_ERROR<<"no voi with the voi UID:"<<voiUID;
        return TPS_ER_FAILURE;
    }

    std::string imageUID = tpsVOIEntity->GetSeriesUid();
    TpsImage3DEntity *image3D = nullptr;
    TpsImageDataManager* imageMng = mDataRepository->GetImageDataManager();
    if (!imageMng->GetImage3DData(imageUID, &image3D) || image3D == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d data.";
        return TPS_ER_FAILURE;
    }

    //update voi statistical information
    RtCt2density* pCt2Density = imageMng->GetCT2DensityTableByImageUid(imageUID);
    tpsVOIEntity->GetInnerRoi()->set_isoverride(false);
    if (!SubVolumeHelperAPI::UpdateVOIStatisticalInformation(pCt2Density, *image3D, tpsVOIEntity))
    {
        TPS_LOG_DEV_ERROR<<"Failed to update statistical information.";
        return TPS_ER_FAILURE;

    }

    if(mSaveObjects->SaveRoi(*tpsVOIEntity) != TPS_ER_SUCCESS){
        TPS_LOG_DEV_ERROR<<"SaveVOItoTMS failed VOI UID:"<<voiUID;
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}



int TpsBLController::PasteContour(const std::string seriesUID, const std::string voiUID, EDIT_FUNCTION editFunction)
{
    ROIEntity *voiEntity = mDataRepository->GetVoiManager()->GetROI(voiUID);
    if (nullptr == voiEntity){
        TPS_LOG_DEV_ERROR<<"no voi with the voi UID:"<<voiUID;
        return false;
    }
    if (!mRenderProxy->PasteContour(seriesUID, voiUID, editFunction))
    {
        return TPS_ER_FAILURE;
    }

    mRenderProxy->Render();

    std::string imageUID = voiEntity->GetSeriesUid();
    TpsImage3DEntity *image3D = nullptr;
    TpsImageDataManager* imageMng = mDataRepository->GetImageDataManager();
    if (!imageMng->GetImage3DData(imageUID, &image3D) || image3D == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d data.";
        return false;
    }

    //update voi statistical information
    RtCt2density* pCt2density = imageMng->GetCT2DensityTableByImageUid(imageUID);
    if (!SubVolumeHelperAPI::UpdateVOIStatisticalInformation(pCt2density, *image3D, voiEntity))    
    {
        TPS_LOG_DEV_ERROR<<"Failed to update statistical information.";
        return false;
    }

    if(mSaveObjects->SaveRoi(*voiEntity) != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR<<"SaveVOItoTMS failed VOI UID:"<<voiUID;
        return false;
    }

    //delete all related commands in redo/undo list of CommandManager
    auto mng = mDataRepository->GetCommandManager();
    TpsSafeLock<TpsCommandManager> lock(mng);
    mng->RemoveCommands([&](TpsRedoUndoCommand* cmd)->bool{
        auto voiCmd = dynamic_cast<TpsVOIRedoUndoCmdBase*>(cmd);
        return voiCmd != nullptr && voiCmd->GetVoiUid() == voiUID;
    });
    return TPS_ER_SUCCESS;
}

bool TpsBLController::IsInVOI(WINDOW_TYPE windowType, const std::string& windowUid, 
    const std::string& voiUid, double x, double y)
{
    Mcsf::Point3f p;
    if (!mRenderProxy->TransformPoint(windowType, windowUid, Mcsf::Point2f(x, y), p))
    {
        return false;
    }

    auto roi = mDataRepository->GetVoiManager()->GetROI(voiUid);
    if (roi == nullptr) return false;
    return roi->IsInROI(PT3(p.GetX(), p.GetY(), p.GetZ()));
}

bool TpsBLController::IsInVolume(WINDOW_TYPE windowType, 
    const std::string& windowUid, double x, double y)
{
    return mRenderProxy->IsInVolume(windowType, windowUid, x, y);
}

int TpsBLController::UnApproveVois(std::vector<std::string> voiUidList)
{
    for (int i = 0; i < voiUidList.size(); i++)
    {
        int iRet = mSaveObjects->DeleteApproveRecordViaObjUid(voiUidList[i]);
        if (iRet != TPS_ER_SUCCESS)
        {
            TPS_LOG_DEV_ERROR << "Failed to DeleteApproveRecordViaObjUid";
            return iRet;
        }

        ROIEntity *voiEntity = mDataRepository->GetVoiManager()->GetROI(voiUidList[i]);
        voiEntity->GetInnerRoi()->set_locktype((int)RtDbDef::NORMALLOCK);

        iRet = mSaveObjects->UpdateVoiAttributes(*voiEntity);

        if (iRet != TPS_ER_SUCCESS)
        {
            TPS_LOG_DEV_ERROR << "Failed to UpdateVoiAttributes in UnApproveVois";
            return iRet;
        }
    }

    return TPS_ER_SUCCESS;
}
void TpsBLController::ChangeAllPoiVisibility(const std::string& seriesUid, bool visible)
{
    auto poiList = mDataRepository->GetPoiManager()->GetAllPoiInSeries(seriesUid);
    if (poiList.empty()) return;
    for (int i = 0; i < poiList.size(); ++i){
        if (poiList[i] == nullptr) continue;
        poiList[i]->set_isvisible(visible);
    }

    std::map<std::string, std::string> mapAttributes;
    mapAttributes["isvisible"] = visible ? "1" : "0";
    std::map<std::string, std::vector<std::string>> mapFilters;
    std::vector<std::string> filterValue;
    filterValue.push_back(seriesUid);
    mapFilters["seriesuid"] = filterValue;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateItems(RtDbDef::TYPE_POI, mapAttributes, mapFilters)){
        TPS_LOG_DEV_ERROR<<"Failed to update items.";
        return;
    }

    mRenderProxy->ChangePoiVisibilityOfSeries(seriesUid, visible);
    mRenderProxy->Render();
}

void TpsBLController::UpdateVoiInGridWithPlan( const std::string &planUid )
{
    TpsPatientManager *patientManager = mDataRepository->GetPatientManager();

    RtPlan *plan = nullptr;
    if(!patientManager->GetPlan(planUid, &plan) 
        || plan == nullptr) {
            TPS_LOG_DEV_ERROR<<"Failed to get plan.";
            return;
    }

    std::string imageUid = plan->get_imageseriesuid();
    TpsImageDataManager *imageManager = mDataRepository->GetImageDataManager();
    TpsImage3DEntity *image = nullptr;
    if (!imageManager->GetImage3DData(imageUid, &image)
        || image == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get image.";
        return;
    }

    TpsDoseManager *doseManager = mDataRepository->GetDoseManager();
    RtDosegrid *dosegrid = doseManager->GetDoseGrid(planUid, DOSE_GRID_PLAN);

    TpsVOIManager *voiManager = mDataRepository->GetVoiManager();
    auto voiList = voiManager->GetROIBySeries(imageUid);
    for (int i = 0; i < voiList.size(); ++i)
    {
        if (voiList[i] == nullptr || voiList[i]->IsEmpty())
        {
            continue;
        }
        float inGrid = SubVolumeHelperAPI::CalculateInGridPercentage(*image, voiList[i], dosegrid);
        voiList[i]->SetInGrid(inGrid);
    }
}


bool TpsBLController::CheckPointInGrid(const std::string &planUid, float ptx, float pty, float ptz)
{
    TpsPatientManager *patientManager = mDataRepository->GetPatientManager();

    RtPlan *plan = nullptr;
    if(!patientManager->GetPlan(planUid, &plan) || plan == nullptr) 
    {
        TPS_LOG_DEV_ERROR<<"Failed to get plan.";
        return false;
    }
    TpsDoseManager *doseManager = mDataRepository->GetDoseManager();
    RtDosegrid *dosegrid = doseManager->GetDoseGrid(planUid, DOSE_GRID_PLAN);

    Mcsf::Matrix4f matG2P = TpsArithmeticConverter::ConvertToMatrix4f(dosegrid->get_grid_to_pat_t());
    Mcsf::Matrix4f matP2G = matG2P.Inverse();
    Mcsf::Point3f ptInGrid = matP2G.Transform(Mcsf::Point3f(ptx, pty, ptz));
    if (ptInGrid.GetX() < -0.5f 
        || ptInGrid.GetX() > dosegrid->get_xcount() - 0.5f
        || ptInGrid.GetY() < -0.5f
        || ptInGrid.GetY() > dosegrid->get_ycount() - 0.5f
        || ptInGrid.GetZ() < -0.5f 
        || ptInGrid.GetZ() > dosegrid->get_zcount() - 0.5f)
    {
        return false;
    }
    return true;
}

float TpsBLController::GetVoiInGrid( const std::string &planUid, const std::string &voiUid )
{
    TpsPatientManager *patientManager = mDataRepository->GetPatientManager();

    RtPlan *plan = nullptr;
    if(!patientManager->GetPlan(planUid, &plan) 
        || plan == nullptr) 
    {
            TPS_LOG_DEV_ERROR<<"Failed to get plan.";
            return .0f;
    }

    std::string imageUid = plan->get_imageseriesuid();
    TpsImageDataManager *imageManager = mDataRepository->GetImageDataManager();
    TpsImage3DEntity *image = nullptr;
    if (!imageManager->GetImage3DData(imageUid, &image)
        || image == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get image.";
        return .0f;
    }

    TpsDoseManager *doseManager = mDataRepository->GetDoseManager();
    RtDosegrid *dosegrid = doseManager->GetDoseGrid(planUid, DOSE_GRID_PLAN);

    TpsVOIManager *voiManager = mDataRepository->GetVoiManager();
    ROIEntity *voiEntity = voiManager->GetROI(voiUid);
    if (voiEntity == nullptr || voiEntity->IsEmpty())
    {
        return 0.0f;
    }
    return SubVolumeHelperAPI::CalculateInGridPercentage(*image, voiEntity, dosegrid);
}

TPS_END_NAMESPACE  // end namespace tps
