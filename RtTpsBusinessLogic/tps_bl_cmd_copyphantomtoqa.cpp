#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_copyphantomtoqa.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_datarepositoryfactory.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsFramework/tps_fw_table.h"
#include "RtTpsDataAccess/tps_da_tablemanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_contour.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_study.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_patient.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image.h"

#include "McsfGeometry/mcsf_geometry_sub_volume.h"

#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_common_datastructure.h"

#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsRenderController/tps_rc_defs.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "McsfDicomConvertor/mcsf_dicom_convertor.h"
#include "McsfDicomConvertor/mcsf_dicom_convertor_factory.h"

#include "Common/McsfSystemEnvironmentConfig/mcsf_systemenvironment_factory.h"

TPS_BEGIN_NAMESPACE
    TpsCopyPhantomToQACmd::TpsCopyPhantomToQACmd(const std::string& seriesUid) :
mPhantomSeriesUid(seriesUid), mQASeries(nullptr)
{

}
TpsCopyPhantomToQACmd::~TpsCopyPhantomToQACmd()
{
    Dispose();
}

void TpsCopyPhantomToQACmd::Dispose()
{
    DEL_PTR(mQASeries);
}

void TpsCopyPhantomToQACmd::GetQASeries(const RtSeries*& pSeries)
{
    pSeries = mQASeries;
}

int TpsCopyPhantomToQACmd::Execute()
{
    //获取current patient
    RtPatient* currentPatient = nullptr;
    currentPatient = mDataRepository->GetPatientManager()->GetPatient();
    if(nullptr == currentPatient) {
        TPS_LOG_DEV_ERROR<<"TpsCopyPhantomToQACmd: Failed to get current patient from patient manager.";
        return TPS_ER_FAILURE;
    }

    if(DB_CODES_SUCCESS != mDatabaseWrapper->GetSeriesObjectByUid(mPhantomSeriesUid, mQASeries)){
        TPS_LOG_DEV_ERROR<<"TpsCopyPhantomToQACmd: Failed to load GetSeriesObjectByUid which uid: "<<mPhantomSeriesUid;
        return TPS_ER_FAILURE;
    }

    // 首先创建一条study对象，保存至数据库
    //不使用堆对象，内存泄漏
    RtStudy newStudy(true); 
    newStudy.set_patientuid(currentPatient->get_uid());

    if(DB_CODES_SUCCESS != mDatabaseWrapper->InsertStudy(newStudy)) {
        TPS_LOG_DEV_ERROR<<"TpsCopyPhantomToQACmd: Failed to save study("<< newStudy.get_uid() <<") to DB.";
        return TPS_ER_FAILURE;
    }

    //sopinstanceuid + studyuid必须唯一
    mQASeries->setdirty_all_fields(true);
    mQASeries->set_uid(mDatabaseWrapper->GeneraterUid());
    mQASeries->set_sopinstanceuid(mDatabaseWrapper->GeneraterUid());
    //设置studyuid，planQA导出需要用到
    mQASeries->set_studyuid(newStudy.get_uid());
    //newSeries->set_seriespixeldatauid(newSeries->get_uid());
    mQASeries->set_isqa(true);
    mQASeries->set_isphantom(false);
    if (mQASeries->get_seriesdescription().empty())
    {
        mQASeries->set_seriesdescription(mQASeries->get_patientname());
    }
    else
    {
        mQASeries->set_seriesdescription(mQASeries->get_patientname() + " - " + mQASeries->get_seriesdescription());
    }

    //tms要求newSeries PatientName为当前当前病人
    // In DICOM the patient name format is: "LastName^FirstName^MiddleName^NamePrefix^NamePostfix"
    mQASeries->set_patientname(currentPatient->get_lastname() + "^" + currentPatient->get_firstname() + "^" 
        + currentPatient->get_middlename() + "^" + currentPatient->get_nameprefix() + "^" + currentPatient->get_namesuffix());
    mQASeries->set_patientid(currentPatient->get_patientid());
    mQASeries->set_patientbirthdate(ConvertDateTime_i(currentPatient->get_patientbirthdate()));
    mQASeries->set_patientsex(std::to_string((long long)currentPatient->get_patientsex()));
    mQASeries->set_patientuid(currentPatient->get_uid());

    if(mSaveObjects->SaveSeries(*mQASeries) != DB_CODES_SUCCESS) {
        TPS_LOG_DEV_ERROR<<"TpsCopyPhantomToQACmd: Failed to save QA Series which uid is : "<<mQASeries->get_uid();
        return TPS_ER_FAILURE;
    }

    std::map<std::string, std::string> imageUidMap;
    if (!CopyImagesBySeries_i(mPhantomSeriesUid, *mQASeries, imageUidMap))
    {
        TPS_LOG_DEV_ERROR<<"TpsCopyPhantomToQACmd: Failed to CopyImagesBySeries which uid is : "<<mQASeries->get_uid();
        return TPS_ER_FAILURE;
    }

    if (!CopyVoisBySeries_i(mPhantomSeriesUid, mQASeries->get_uid(), imageUidMap))
    {
        TPS_LOG_DEV_ERROR<<"TpsCopyPhantomToQACmd: Failed to CopyVoisBySeries uid is : "<<mQASeries->get_uid();
        return TPS_ER_FAILURE;
    }

    if (!CopyPoisBySeries_i(mPhantomSeriesUid, mQASeries->get_uid()))
    {
        TPS_LOG_DEV_ERROR<<"TpsCopyPhantomToQACmd: Failed to CopyPoisBySeries uid is : "<<mQASeries->get_uid();
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

bool TpsCopyPhantomToQACmd::CopyImagesBySeries_i(const std::string& phantomSeriesUid, const RtSeries& qaSeries, std::map<std::string, std::string>& imageUidMap)
{
    Mcsf::ISystemEnvironmentConfig* pSystemEnvironmentConfig = Mcsf::ConfigSystemEnvironmentFactory::Instance()->GetSystemEnvironment();
    if ( nullptr == pSystemEnvironmentConfig )
    {
        TPS_LOG_DEV_ERROR<<"GetSystemEnvironment pSystemEnvironmentConfig is null!";
        return false;
    }

    //get Root Path of TPS
    //D:/UIH
    const std::string sUihRootPath = pSystemEnvironmentConfig->GetRootPath();
    const std::string sTempImageFolder= sUihRootPath + "/tempimage";
    if(!this->CreateMultiFolder_i(sTempImageFolder))
    {
        TPS_LOG_DEV_ERROR << "TpsCopyPhantomToQACmd::CopyImagesBySeries CreateMultiFolder failed with phantom series uid:" << phantomSeriesUid;
        return false;
    }

    std::vector<RtImage*> imageList;
    if(mDatabaseWrapper->GetImageListBySeriesUid(phantomSeriesUid, imageList, true) != DB_CODES_SUCCESS){
        TPS_LOG_DEV_ERROR<<"TpsCopyPhantomToQACmd: Failed to GetImageListBySeriesUid with phantom series uid: "<<phantomSeriesUid;
        return false;
    }

    bool bSuccess = true;
    std::string sBufferString("");
    for (auto itrImg=imageList.begin(); itrImg!=imageList.end(); ++itrImg)
    {
        //overwrite patient info in image dicom 
        //move to out
        Mcsf::ISimpleDICOMConvertorPtr pConvertor = Mcsf::DICOMConvertorFactory::Instance()->CreateSimpleConvertor();
        Mcsf::IDataHeaderElementMap* dataHeaderPtr = Mcsf::IDataHeaderElementMap::CreateDataHeader();
        if (dataHeaderPtr == nullptr)
        {
            TPS_LOG_DEV_ERROR << "TpsCopyPhantomToQACmd::CopyImagesBySeries: Create Mcsf::IDataHeaderElementMap failed with uid:" << phantomSeriesUid;
            bSuccess = false;
            break;
        }
        unsigned long uloldSize(0);
        char* pData = (*itrImg)->get_dcmfiledata(&uloldSize);
        if (!pConvertor->LoadFile(pData, uloldSize, dataHeaderPtr))
        {
            TPS_LOG_DEV_ERROR << "TpsCopyPhantomToQACmd::CopyImagesBySeries: pConvertor->LoadFile failed with uid:" << phantomSeriesUid;
            DEL_PTR(dataHeaderPtr);
            bSuccess = false;
            break;
        }

        const std::string sImgUid = mDatabaseWrapper->GeneraterUid();
        imageUidMap[(*itrImg)->get_uid()] = sImgUid;
        (*itrImg)->set_uid(sImgUid);
        (*itrImg)->set_sopinstanceuid(mDatabaseWrapper->GeneraterUid());
        (*itrImg)->set_seriesuid(qaSeries.get_uid());
        (*itrImg)->setdirty_all_fields(true);

        sBufferString = qaSeries.get_patientname();
        if (!ResetDicomData_i(dataHeaderPtr, Mcsf::kTagDcm_PatientName, sBufferString))
        {
            TPS_LOG_DEV_ERROR << "TpsCopyPhantomToQACmd::CopyImagesBySeries Reset kTagDcm_PatientName failed with uid:" << phantomSeriesUid;
            bSuccess = false;
            break;
        }

        sBufferString = qaSeries.get_patientid();
        if (!ResetDicomData_i(dataHeaderPtr, Mcsf::kTagDcm_PatientID, sBufferString))
        {
            TPS_LOG_DEV_ERROR << "TpsCopyPhantomToQACmd::CopyImagesBySeries Reset kTagDcm_PatientID failed with uid:" << phantomSeriesUid;
            bSuccess = false;
            break;
        }

        sBufferString = qaSeries.get_patientbirthdate();
        if (!ResetDicomData_i(dataHeaderPtr, Mcsf::kTagDcm_PatientBirthDate, sBufferString))
        {
            TPS_LOG_DEV_ERROR << "TpsCopyPhantomToQACmd::CopyImagesBySeries Reset kTagDcm_PatientBirthDate failed with uid:" << phantomSeriesUid;
            bSuccess = false;
            break;
        }

        if (qaSeries.get_patientsex().compare("1") == 0)
        {
            sBufferString = "M";
        }
        else if (qaSeries.get_patientsex().compare("2") == 0)
        {
            sBufferString = "F";
        }
        else if (qaSeries.get_patientsex().compare("4") == 0)
        {
            sBufferString = "O";
        }
        else
        {
            sBufferString = "";
        }
        if (!ResetDicomData_i(dataHeaderPtr, Mcsf::kTagDcm_PatientSex, sBufferString))
        {
            TPS_LOG_DEV_ERROR << "TpsCopyPhantomToQACmd::CopyImagesBySeries Reset kTagDcm_PatientSex failed with uid:" << phantomSeriesUid;
            bSuccess = false;
            break;
        }

        sBufferString = qaSeries.get_sopinstanceuid();                                   // Series SopInstanceUID
        if (!ResetDicomData_i(dataHeaderPtr, Mcsf::kTagDcm_SeriesInstanceUID, sBufferString))
        {
            TPS_LOG_DEV_ERROR << "TpsCopyPhantomToQACmd::CopyImagesBySeries Reset kTagDcm_SeriesInstanceUID failed with uid:" << phantomSeriesUid;
            bSuccess = false;
            break;
        }

        // SOPInstanceUID actually is image UID in this case
        sBufferString = (*itrImg)->get_sopinstanceuid();
        if (!ResetDicomData_i(dataHeaderPtr, Mcsf::kTagDcm_SOPInstanceUID, sBufferString))
        {
            TPS_LOG_DEV_ERROR << "TpsCopyPhantomToQACmd::CopyImagesBySeries Reset kTagDcm_SOPInstanceUID failed with uid:" << phantomSeriesUid;
            bSuccess = false;
            break;
        }

        std::string sImageFilePath = sTempImageFolder + "/" + sImgUid + ".DCM";
        // save file
        if (!pConvertor->SaveFile(dataHeaderPtr, sImageFilePath))
        {
            TPS_LOG_DEV_ERROR << "TpsCopyPhantomToQACmd::CopyImagesBySeries: SaveFile failed with uid:" << phantomSeriesUid;
            DEL_PTR(dataHeaderPtr);
            bSuccess = false;
            break;
        }
        DEL_PTR(dataHeaderPtr);

        char* pBuffer = nullptr;
        unsigned long ulSize(0);
        bool bRet = this->ConvertDcmFile2Cache_i(sImageFilePath, pBuffer, &ulSize);
        if (!bRet || ulSize < 1 || nullptr == pBuffer)
        {
            TPS_LOG_DEV_ERROR << "TpsCopyPhantomToQACmd::CopyImagesBySeries: Failed to ConvertDcmFile2Cache with uid:" << phantomSeriesUid;
            bSuccess = false;
            break;
        }
        else 
        {
            (*itrImg)->set_dcmfiledata(pBuffer, ulSize);
            DEL_ARRAY(pBuffer);
        }

        if (TPS_ER_SUCCESS != mSaveObjects->SaveImage(**itrImg))
        {
            TPS_LOG_DEV_ERROR<<"TpsCopyPhantomToQACmd::CopyImagesBySeries: Failed to save image:"<<sImgUid;
            bSuccess = false;
            break;
        }
    }
    for (auto itrImg=imageList.begin(); itrImg != imageList.end(); ++itrImg)
    {
        DEL_PTR(*itrImg);
    }
    if(!this->DeleteMultiFolder_i(sTempImageFolder))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd:: DeleteMultiFolder failed with series uid:" << phantomSeriesUid;
        return false;
    }
    return bSuccess;
}

bool TpsCopyPhantomToQACmd::ResetDicomData_i(Mcsf::IDataHeaderElementMap* dataHeaderPtr, const Mcsf::Tag tag, const std::string& sBufferString)
{
    Mcsf::IDataHeaderElementMap::ConstElementPointer elementPointer = nullptr;
    int iCount = static_cast<int>(sBufferString.length());
    if (!dataHeaderPtr->FindTag(tag, &elementPointer))
    {
        if (!dataHeaderPtr->InsertStringByTag(tag, sBufferString.c_str(), iCount))
        {
            DEL_PTR(dataHeaderPtr);
            return false;
        }
    }
    else
    {
        if (!dataHeaderPtr->SetStringByTag(tag, sBufferString.c_str(), iCount))
        {
            DEL_PTR(dataHeaderPtr);
            return false;
        }
    }
    return true;
}

// Copy VOI list from Phantom to PlanQA, and save to DB.
bool TpsCopyPhantomToQACmd::CopyVoisBySeries_i(const std::string& phantomSeriesUid, const std::string& qaSeriesUid, std::map<std::string, std::string>& imageUidMap)
{
    // Copy VOI from Phantom series to PlanQA series and save to DB.
    std::vector<RtVoi*> rtVois;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->GetVoiListBySeriesUid(phantomSeriesUid, rtVois))
    {
        TPS_LOG_DEV_ERROR<<"TpsCopyPhantomToQACmd: Failed to GetVoiListBySeriesUid which uid: "<<phantomSeriesUid;
        return false;
    }
    bool bSuccess = true;
    for (auto rtVoi = rtVois.begin(); rtVoi != rtVois.end(); ++rtVoi)
    {
        (*rtVoi)->setdirty_all_fields(true);
        (*rtVoi)->set_uid(mDatabaseWrapper->GeneraterUid());
        (*rtVoi)->set_seriesuid(qaSeriesUid);
        (*rtVoi)->set_locktype(RtDbDef::UNLOCK);
        std::vector<RtContour*> voiContours = (*rtVoi)->get_contours();
        for(auto itr = voiContours.begin(); itr != voiContours.end(); ++itr)
        {
            (*itr)->setdirty_all_fields(true);
            (*itr)->set_uid(mDatabaseWrapper->GeneraterUid());
            (*itr)->set_voiuid((*rtVoi)->get_uid());
            (*itr)->set_imageuid(imageUidMap[(*itr)->get_imageuid()]);
        }
        // structure set uid will be automatically referenced in database wrapper

        // Save VOI to DB.
        if(TPS_ER_SUCCESS != mSaveObjects->SaveVoiObject(*rtVoi))
        {
            TPS_LOG_DEV_ERROR<<"Failed to save voi: "<<(*rtVoi)->get_uid() <<" to database.";
            bSuccess = false;
            break;
        }
    }
    for (auto it=rtVois.begin(); it != rtVois.end(); ++it)
    {
        DEL_PTR(*it);
    }
    return bSuccess;
}

// Copy POI list from Phantom to PlanQA, and save to DB.
bool TpsCopyPhantomToQACmd::CopyPoisBySeries_i(const std::string& phantomSeriesUid, const std::string& qaSeriesUid) {
    // Copy POI from Phantom series to PlanQA series and save to DB.
    std::vector<RtPoi*> rtPois;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->GetPoiListBySeriesUid(phantomSeriesUid, rtPois))
    {
        TPS_LOG_DEV_ERROR<<"TpsCopyPhantomToQACmd: Failed to GetPoiListBySeriesUid which uid: "<<phantomSeriesUid;
        return false;
    }
    bool bSuccess = true;
    for (auto rtPoi = rtPois.begin(); rtPoi != rtPois.end(); ++rtPoi)
    {
        (*rtPoi)->setdirty_all_fields(true);
        (*rtPoi)->set_uid(mDatabaseWrapper->GeneraterUid());
        (*rtPoi)->set_seriesuid(qaSeriesUid);
        (*rtPoi)->set_locktype(RtDbDef::UNLOCK);

        if(TPS_ER_SUCCESS != mSaveObjects->SavePoi((**rtPoi)))
        {
            TPS_LOG_DEV_ERROR<<"Failed to save poi: "<< (*rtPoi)->get_uid() <<" to database.";
            bSuccess = false;
            break;
        }
    }
    for (auto it=rtPois.begin(); it != rtPois.end(); ++it)
    {
        DEL_PTR(*it);
    }
    return bSuccess;
}

std::string TpsCopyPhantomToQACmd::ConvertDateTime_i(const DATE_BOOST& date)
{
    std::string sDate("");
    if (date.is_not_a_date()){
        return sDate;
    }
    //YYYYMMDDTHHMMSS
    sDate = boost::gregorian::to_iso_extended_string(date);
    //         (void)sDate.insert(4,"-");
    //         (void)sDate.insert(7,"-");
    //sDate = "'" + sDate + "'";//for database
    return sDate;
}

bool TpsCopyPhantomToQACmd::CreateMultiFolder_i(const std::string& sFolderName) const
{
    namespace bfs = boost::filesystem;
    try{
        // create a directory, then check it for consistency
        //   take extra care to report problems, since if this fails
        //   many subsequent tests will fail
        if (bfs::is_directory(sFolderName)){
            return true;// already exists 
        }else if (bfs::exists(sFolderName)){
            return true; //is other
        }
        return bfs::create_directories(sFolderName);
    }
    catch ( const bfs::filesystem_error & x ){
        TPS_LOG_DEV_ERROR<<("***** Creating directory: " + sFolderName + " failed.          *****\n" + std::string(x.what()));
        return false;
    }
}

bool TpsCopyPhantomToQACmd::DeleteMultiFolder_i(const std::string& sFolderName) const
{
    try{
        boost::filesystem::path b_dir_path(sFolderName);
        if(boost::filesystem::exists(b_dir_path))
        {
            boost::filesystem::remove_all(b_dir_path);
        }
    }catch(boost::filesystem::filesystem_error & e)
    {
        TPS_LOG_DEV_ERROR<<("***** Deleting directory: " + sFolderName + " failed.          *****\n" + std::string(e.what()));
        return false;
    }
    return true;
}

bool TpsCopyPhantomToQACmd::ConvertDcmFile2Cache_i(const std::string& sImageFilePath, char*& pBuffer, unsigned long* ulSize)
{
    if (nullptr == ulSize){
        return false;
    }
    const int iIosBinary = static_cast<int>(std::ios::binary);
    const int iIosIn = static_cast<int>(std::ios::in);
    const int iIosNoCreate = static_cast<int>(std::ios::_Nocreate);
    // open file
    std::ifstream myFileStream(sImageFilePath.c_str(), iIosBinary|iIosIn|iIosNoCreate);
    if(!myFileStream.good()) return false;

    // move to the end
    (void)myFileStream.seekg(0 , std::ios::end);
    // get file stream size
    const unsigned long ulFileStreamSize =  static_cast<unsigned long>(myFileStream.tellg());
    // move to the begin
    (void)myFileStream.seekg(0 , std::ios::beg);

    // create file buffer
    pBuffer = new char[ulFileStreamSize];
    *ulSize = ulFileStreamSize;
    // get file stream
    (void)myFileStream.read(pBuffer, static_cast<std::streamsize>(ulFileStreamSize));
    // close file
    myFileStream.close();

    return true;
}
TPS_END_NAMESPACE
