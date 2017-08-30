////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_bl_cmd_manset_voi_density.cpp
/// 
///  \brief class TpsMansetVoiDensityCmd 
/// 
///  \version 1.0
/// 
///  \date    2015/10/26
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsBusinessLogic/tps_bl_cmd_phantom_create.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"                 // for TpsSaveObjects
#include "RtTpsDataAccess/tps_da_idatarepository.h"                 // for ITpsDataRepository
#include "RtTpsRenderController/tps_rc_renderproxy.h"               // for TpsRenderProxy
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"          // for GOTypeHelper
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_study.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_patient.h"

#include "RtTpsFramework/tps_fw_table.h"
#include "RtTpsDataAccess/tps_da_tablemanager.h"

#include "McsfDicomConvertor/mcsf_dicom_convertor.h"                // for header declaration
#include "McsfDicomConvertor/mcsf_dicom_convertor_factory.h"
#include "McsfDatabase/mcsf_database_dicom_uid_manager_factory_i.h"
#include "McsfDatabase/mcsf_database_dicom_uid_manager_i.h"
#include "tps_logger.h"

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/filesystem.hpp"                                     //delete file
#include <fstream>                                                  // for ofstream file write

#include "Common/McsfSystemEnvironmentConfig/mcsf_systemenvironment_factory.h"

#define DEFAULT_CT_DENSITY "ut_ct2density_uid_9527"
#define DEFAULT_CT_DENSITY_SIZE 6025

#include "tps_performance.h"

TPS_BEGIN_NAMESPACE;

PhantomCreateCmd::PhantomCreateCmd(PhantomSeries &phantomSeries) 
    : m_PhantomSeries(phantomSeries)
{
    m_strNewPhantomUid = "";
    m_pImageDataBuffer = nullptr;
}

PhantomCreateCmd::~PhantomCreateCmd() 
{
    DEL_ARRAY(m_pImageDataBuffer);
}

int PhantomCreateCmd::PreExecute() {
    return TPS_ER_SUCCESS;
}

int PhantomCreateCmd::Execute() 
{
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    if (mSaveObjects == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"mSaveObject is NULL.";
        return TPS_ER_FAILURE;
    }

    //check default ct2density table
    RtCt2density* ct2densityTable = nullptr;
    int iRet = mDatabaseWrapper->GetCt2densityByUid(DEFAULT_CT_DENSITY, ct2densityTable);
    if (iRet != DB_CODES_SUCCESS || ct2densityTable == nullptr){
        TPS_LOG_DEV_ERROR << "ct2densityTable is null. Failed to GetCT2DensityTableByUid with uid"
            << DEFAULT_CT_DENSITY;
        return false;
    }
    //const float ctValue = Density2CT(m_fDensity,ct2densityTable->get_ct2densitybuffer());

    //seires and images to save
    RtSeries* pSeries = new RtSeries(true);
    const std::string sSeriesUid = pSeries->get_uid();

    pSeries->set_isphantom(true);
    pSeries->set_ct2densityuid(DEFAULT_CT_DENSITY);
    pSeries->set_manufacturermodelname("uRT-TPS/uRT-OIS");
    pSeries->set_convolutionkernal("STANDARD");

    char buffer[256];
    std::string sBufferString("");
    int iCount;
    std::string strFrameOfReferenceUID; // now it is the study UID

    // Create DataHeaderElementMap
    std::shared_ptr<Mcsf::IDataHeaderElementMap> spDataHeader;
    spDataHeader.reset(Mcsf::IDataHeaderElementMap::CreateDataHeader());
    // UID Manager
    Mcsf::IDatabaseDicomUIDManager* pUidManager = Mcsf::IDatabaseDicomUIDManagerFactory::Instance()->CreateDicomUIDManager();
    // DICOM Converter
    boost::shared_ptr<Mcsf::ISimpleDICOMConvertor> spConvert = Mcsf::DICOMConvertorFactory::Instance()->CreateSimpleConvertor();

    // ****************************************************************************
    // *                   General Equipment                                      *
    // ****************************************************************************

    sBufferString = "UIH";                                      // Manufacturer
    iCount = static_cast<int>(sBufferString.length());
    if (!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_Manufacturer, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_Manufacturer failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    pSeries->set_manufacturer(sBufferString);

    // ****************************************************************************
    // *                   Patient                                                *
    // ****************************************************************************

	//获取current patient
	RtPatient* currentPatient = nullptr;
	bool isPatientManaged = true;
	currentPatient = mDataRepository->GetPatientManager()->GetPatient();
	if(nullptr == currentPatient) {
		TPS_LOG_DEV_WARNING<<"TpsCopyPhantomToQACmd: Failed to get current patient from patient manager. Internal Patient";
		if (TPS_ER_SUCCESS != mDatabaseWrapper->GetPatientByUid(m_PhantomSeries.patientUID, currentPatient))
		{
			TPS_LOG_DEV_ERROR<<"TpsCopyPhantomToQACmd: Failed to get internal patient:" << m_PhantomSeries.patientUID;
			return TPS_ER_FAILURE;
		}
		isPatientManaged = false;
	}

    // In DICOM the patient name format is: "LastName^FirstName^MiddleName^NamePrefix^NamePostfix"
    sBufferString = m_PhantomSeries.phantomName.c_str();
    iCount = static_cast<int>(sBufferString.length());
    if (!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_PatientName, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_PatientName failed!";
		DEL_PTR(pSeries);
		if (!isPatientManaged) DEL_PTR(currentPatient);
        return TPS_ER_FAILURE;
    }
    pSeries->set_patientname(sBufferString);

    // This is commented out because when exporting series the information of patient name, id, birth-date and gender 
    //      should be the same with VOI, otherwise they can not be seen as belonging to same patient.
    //      Now VOI exporting takes TMS API and they didn't fill out the information of ID, birth-date and gender.
	sBufferString = currentPatient->get_patientid();          // Patient ID
	iCount = static_cast<int>(sBufferString.length());
    if (!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_PatientID, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_PatientID failed!";
		DEL_PTR(pSeries);
		if (!isPatientManaged) DEL_PTR(currentPatient);
        return TPS_ER_FAILURE;
    }

	sBufferString = ConvertDateTime(currentPatient->get_patientbirthdate());   // Patient BirthDate
	iCount = static_cast<int>(sBufferString.length());
    if (!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_PatientBirthDate, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_PatientBirthDate failed!";
		DEL_PTR(pSeries);
		if (!isPatientManaged) DEL_PTR(currentPatient);
        return TPS_ER_FAILURE;
	}

	// Patient Sex
	if (currentPatient->get_patientsex() == 1)
	{
		sBufferString = "M";
	}
	else if (currentPatient->get_patientsex() == 2)
	{
		sBufferString = "F";
	}
	else if (currentPatient->get_patientsex() == 4)
	{
		sBufferString = "O";
	}
	else
	{
		sBufferString = "";
	}
	iCount = static_cast<int>(sBufferString.length());
    if (!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_PatientSex, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_PatientSex failed!";
		DEL_PTR(pSeries);
		if (!isPatientManaged) DEL_PTR(currentPatient);
        return TPS_ER_FAILURE;
    }

	if (!isPatientManaged) DEL_PTR(currentPatient);

    // ****************************************************************************
    // *                   General Study                                          *
    // ****************************************************************************
    // 创建一条study对象，保存至数据库
    //RtStudy *newStudy = new RtStudy(true);
    RtStudy newStudy(true); 
    newStudy.set_patientuid(m_PhantomSeries.patientUID);
    int ret = mDatabaseWrapper->InsertStudy(newStudy);
    if(ret != DB_CODES_SUCCESS) {
        TPS_LOG_DEV_ERROR<<"Failed to save study("<< newStudy.get_uid() <<") to DB.";
        return ret;
    }
    pSeries->set_studyuid(newStudy.get_uid());
    sBufferString = newStudy.get_uid();

    strFrameOfReferenceUID = sBufferString;
    iCount = static_cast<int>(sBufferString.length());
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_StudyInstanceUID, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_StudyInstanceUID failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    sBufferString = "1";
    iCount = static_cast<int>(sBufferString.length());      // Study ID
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_StudyID, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_StudyID failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    pSeries->set_studyid(sBufferString);

    sBufferString = "Physician^NA";                         // Referring Physician Name
    iCount = static_cast<int>(sBufferString.length());
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_ReferringPhysicianName, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_ReferringPhysicianName failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    sBufferString = "1";                                    // Accession Number
    iCount = static_cast<int>(sBufferString.length());
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_AccessionNumber, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_AccessionNumber failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    // ****************************************************************************
    // *                   General Series                                         *
    // ****************************************************************************
    const std::string sModality = "CT";                                   // Modality
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_Modality, sModality.c_str(), sModality.length()))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_Modality failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    pSeries->set_modality(sModality);

    sBufferString = sSeriesUid;//pUidManager->CreateSeriesUID("");       // SeriesInstanceUID
    iCount = static_cast<int>(sBufferString.length());
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_SeriesInstanceUID, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_SeriesInstanceUID failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    pSeries->set_sopinstanceuid(sSeriesUid);

    sBufferString = "1";                                    // Series Number
    iCount = static_cast<int>(sBufferString.length());
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_SeriesNumber, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_SeriesNumber failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    pSeries->set_seriesnumber(1);

    sBufferString = "HFS";                                  // Patient Position
    iCount = static_cast<int>(sBufferString.length());
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_PatientPosition, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_PatientPosition failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    pSeries->set_patientposition(sBufferString);

    sBufferString = "R";                                    // Lateriality ???
    iCount = static_cast<int>(sBufferString.length());
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_Laterality, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_Laterality failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    pSeries->set_laterality(sBufferString);

    const boost::posix_time::ptime ptimeUTCTime = boost::posix_time::microsec_clock::local_time();
    const boost::gregorian::date dtDateInstance = ptimeUTCTime.date();
    const boost::posix_time::time_duration dtTimeInstance = ptimeUTCTime.time_of_day();
    std::string sDate = boost::gregorian::to_iso_string(dtDateInstance);    // DA(Date) format: YYYYMMDD
    std::string sTime = boost::posix_time::to_iso_string(dtTimeInstance);   // TM(Time) format: HHMMSS.FFFFFF

    // SeriesDescription
    sBufferString = m_PhantomSeries.description;
    iCount = static_cast<int>(sBufferString.length());
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_SeriesDescription, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_SeriesDescription failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    pSeries->set_seriesdescription(sBufferString);

    // Study Date and Series Date
    sBufferString = sDate;
    iCount = static_cast<int>(sBufferString.length());
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_StudyDate, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_StudyDate failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_SeriesDate, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_SeriesDate failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    pSeries->set_seriesdate(dtDateInstance);

    //set_acquisitiontime
    pSeries->set_acquisitiontime(ptimeUTCTime);

    // Study Time and Series Time
    sBufferString = sTime;
    iCount = static_cast<int>(sBufferString.length());
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_StudyTime, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_StudyTime failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_SeriesTime, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_SeriesTime failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    pSeries->set_seriestime(dtTimeInstance);

    // ****************************************************************************
    // *                   Image Plane module                                     *
    // ****************************************************************************

    sprintf_s(buffer, "%.2f\\%.2f", m_PhantomSeries.xSpacing, m_PhantomSeries.ySpacing);
    const std::string cPixelSpacing(buffer);
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_PixelSpacing, cPixelSpacing.c_str(), cPixelSpacing.length()))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_PixelSpacing failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    sprintf_s(buffer, "%.5f", m_PhantomSeries.sliceSpacing);
    const std::string cSliceThickness(buffer);
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_SliceThickness, cSliceThickness.c_str(), cSliceThickness.length()))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_SliceThickness failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    const std::string cImageOrientationPatient = "1.0\\0.0\\0.0\\0.0\\1.0\\0.0";
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_ImageOrientationPatient, cImageOrientationPatient.c_str(), cImageOrientationPatient.length()))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_ImageOrientationPatient failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    float xStartPos = -256.0 * m_PhantomSeries.xSpacing;
    float yStartPos = -256.0 * m_PhantomSeries.ySpacing;
    sprintf_s(buffer, "%.3f\\%.3f\\%.3f", xStartPos, yStartPos, 0);
    const std::string cImagePositionPatient(buffer); //"-256\\-256\\0";
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_ImagePositionPatient, cImagePositionPatient.c_str(), cImagePositionPatient.length()))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_ImagePositionPatient failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    // ****************************************************************************
    // *                   CT Image                                     *
    // ****************************************************************************

    const std::string cImageType = "ORIGINAL\\PRIMARY\\AXIAL";
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_ImageType, cImageType.c_str(), cImageType.length()))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_ImageType failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    int iImageType = 2; // ??? 
    pSeries->set_imagetype(iImageType);

    const std::string cPhtotmetricInterpretation = "MONOCHROME2";//PhotometricInterpretation = 0x00280004;
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_PhotometricInterpretation, cPhtotmetricInterpretation.c_str(), cPhtotmetricInterpretation.length()))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_PhotometricInterpretation failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    const std::string cRescaleSlope = "1";
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_RescaleSlope, cRescaleSlope.c_str(), cRescaleSlope.length()))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_RescaleSlope failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    const int iRescaleIntercept = -1024;
    const std::string cRescaleIntercept = "-1024";
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_RescaleIntercept, cRescaleIntercept.c_str(), cRescaleIntercept.length()))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_RescaleIntercept failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    Mcsf::ConstString cAcquisitionNumber = "1";
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_AcquisitionNumber, cAcquisitionNumber, static_cast<int>(std::strlen(cAcquisitionNumber))))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_AcquisitionNumber failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    int nAcquisitionNumber = 1;

    Mcsf::ConstString cKVP = "120";
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_KVP, cKVP, static_cast<int>(std::strlen(cKVP))))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_KVP failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    unsigned short uSamplesPerPixel = 1;
    unsigned short uRows = m_PhantomSeries.yDim;
    unsigned short uCols = m_PhantomSeries.xDim;
    unsigned short uBitsAlloc = 16;
    unsigned short uBitsStored = 16;
    unsigned short uHighBit = 15;
    unsigned short uPixelRepresentation = 0;

    if(!spDataHeader->InsertUInt16ValueByTag(Mcsf::kTagDcm_SamplesPerPixel, uSamplesPerPixel))   // 0X00280002 Samples Per Pixel
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_SamplesPerPixel failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    if(!spDataHeader->InsertUInt16ValueByTag(Mcsf::kTagDcm_Rows, uRows))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_Rows failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    if(!spDataHeader->InsertUInt16ValueByTag(Mcsf::kTagDcm_Columns, uCols))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_Columns failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    if(!spDataHeader->InsertUInt16ValueByTag(Mcsf::kTagDcm_BitsAllocated, uBitsAlloc))       // 0X00280100 Bits Allocated
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_BitsAllocated failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    if(!spDataHeader->InsertUInt16ValueByTag(Mcsf::kTagDcm_BitsStored, uBitsStored))         // 0X00280101 Bits Stored
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_BitsStored failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    if(!spDataHeader->InsertUInt16ValueByTag(Mcsf::kTagDcm_HighBit, uHighBit))               // 0X00280102 High Bit
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_HighBit failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    if(!spDataHeader->InsertUInt16ValueByTag(Mcsf::kTagDcm_PixelRepresentation, uPixelRepresentation))   //0X00280103 Pixel Representation
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_PixelRepresentation failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    //set default window center and window width
    std::string strWindowCenter = "0";
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_WindowCenter, strWindowCenter.c_str(), (int)strWindowCenter.size()))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_WindowCenter failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    std::string strWindowWidth = "2048";
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_WindowWidth, strWindowWidth.c_str(), (int)strWindowWidth.size()))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_WindowWidth failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    const unsigned long lImageSize = uRows * uCols;
    unsigned short* pImageData = new unsigned short[lImageSize];
    //one way
    //memset(pImageData, 0, sizeof(unsigned short) * lImageSize);

    //another way
    const int iAirCtValue = -1000;
    unsigned short usAir = (iAirCtValue - iRescaleIntercept) > 0 ? (iAirCtValue - iRescaleIntercept) : 0; 
    for (int i=0; i<lImageSize; ++i) *(pImageData +i) = usAir;
    //std::fill_n(pImageData, usAir, (unsigned int)(lImageSize * sizeof(unsigned short))); not work

    int nHalfVolumeX = m_PhantomSeries.phantomWidth / m_PhantomSeries.xSpacing / 2.0;
    int nHalfVolumeY = m_PhantomSeries.phantomHeight / m_PhantomSeries.ySpacing / 2.0;
    //for (i = -phantomHeight / 2; i < phantomHeight / 2; i++) {
    //    for (j = -phantomWidth / 2; j < phantomWidth / 2; j++) {
    //        pImageData[(uRows / 2 + i) * uCols + uCols / 2 + j] = 1024;
    //    }
    //}

    for (int j = -nHalfVolumeY; j <= nHalfVolumeY; j++) 
    {
        for (int i = -nHalfVolumeX; i <= nHalfVolumeX; i++) 
        {
            pImageData[(uRows / 2 + j) * uCols + uCols / 2 + i] = (unsigned short)(m_PhantomSeries.ctvalue - iRescaleIntercept);//1024;
        }
    }

    if (!spDataHeader->InsertUInt16ArrayByTag(Mcsf::kTagDcm_PixelData, pImageData, static_cast<int>(lImageSize)))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertUInt16ArrayByTag kTagDcm_PixelData failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    // ****************************************************************************
    // *                   Frame of Reference                                     *
    // ****************************************************************************

    sBufferString = strFrameOfReferenceUID;                     // Frame of reference UID
    iCount = static_cast<int>(sBufferString.length());
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_FrameOfReferenceUID, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_FrameOfReferenceUID failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }
    pSeries->set_frameofreferenceuid(sBufferString);

    sBufferString = "CN";                                       // Position Reference
    iCount = static_cast<int>(sBufferString.length());
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_PositionReferenceIndicator, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_PositionReferenceIndicator failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    // ****************************************************************************
    // *                   SOP Common                                             *
    // ****************************************************************************

    // SOP class UID
    const std::string sSopClassUid = "1.2.840.10008.5.1.4.1.1.2";       // this is CT image
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_SOPClassUID, sSopClassUid.c_str(), sSopClassUid.length()))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_SOPClassUID failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    // Media Storage SOP Class UID
    if (!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_MediaStorageSOPClassUID, sSopClassUid.c_str(), sSopClassUid.length()))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_MediaStorageSOPClassUID failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    // SOPInstanceUID actually is image UID in this case
    sBufferString = pUidManager->CreateImageUID("");
    iCount = static_cast<int>(sBufferString.length());
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_SOPInstanceUID, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_SOPInstanceUID failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    Mcsf::ConstString cInstanceID = "1";                    // 0x00200013 Instance Number
    if(!spDataHeader->InsertStringByTag(Mcsf::kTagDcm_InstanceNumber, cInstanceID, static_cast<int>(std::strlen(cInstanceID))))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_InstanceNumber failed!";
        DEL_PTR(pSeries);
        return TPS_ER_FAILURE;
    }

    const unsigned long lTotalSize = m_PhantomSeries.xDim * m_PhantomSeries.yDim * m_PhantomSeries.zDim * (uBitsAlloc >> 3);
    char* pImage3dData = new char[lTotalSize];
    memset(pImage3dData, 0, lTotalSize * sizeof(char));
    for (int i = 0; i < m_PhantomSeries.zDim; ++i)
    {
        memcpy(pImage3dData + lImageSize * 2 * i, (char*)pImageData, lImageSize * 2);
    }

    //Save series to database
    pSeries->set_imagedata(pImage3dData, lTotalSize);
    //series.set_seriespixeldatauid(series.get_uid());

    if (TPS_ER_SUCCESS != mSaveObjects->SaveSeries(*pSeries))
    {
        TPS_LOG_DEV_ERROR<<"Failed to save series:"<<sSeriesUid;
        DEL_ARRAY(pImageData);
        return TPS_ER_FAILURE;
    }
    //////////////////////////////////////////////////////////////////////////
    Mcsf::ISystemEnvironmentConfig* pSystemEnvironmentConfig = Mcsf::ConfigSystemEnvironmentFactory::Instance()->GetSystemEnvironment();
    if ( nullptr == pSystemEnvironmentConfig )
    {
        TPS_LOG_DEV_ERROR<<"GetSystemEnvironment pSystemEnvironmentConfig is null!";
        return TPS_ER_FAILURE;
    }
    //get version of TPS
    //D:/UIH
    const std::string sUihRootPath = pSystemEnvironmentConfig->GetRootPath();
    const std::string sImage= sUihRootPath + "/temp";
    if(!this->CreateMultiFolder(sImage))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd:: CreateMultiFolder failed with series uid:" << sSeriesUid;
        return TPS_ER_FAILURE;
    }
    std::vector<RtImage*> vImageList;
    for (int i = 0; i < m_PhantomSeries.zDim; i++)
    {
        RtImage* pImage = new RtImage();
        pImage->set_seriesuid(sSeriesUid);
        pImage->set_instancecreationdate(dtDateInstance);
        pImage->set_instancecreationtime(dtTimeInstance);
        pImage->set_pixelspacing(cPixelSpacing);
        pImage->set_slicethickness(cSliceThickness);
        pImage->set_imageorientationpatient(cImageOrientationPatient);
        pImage->set_imagepositionpatient(cImagePositionPatient);
        pImage->set_imagetype(cImageType);
        pImage->set_photometricinterpretation(cPhtotmetricInterpretation);
        pImage->set_rescaleslope(cRescaleSlope);
        pImage->set_rescaleintercept(cRescaleIntercept);
        pImage->set_acquisitionnumber(nAcquisitionNumber);
        pImage->set_acquisitionnumber(nAcquisitionNumber);
        pImage->set_samplesperpixel(uSamplesPerPixel);
        pImage->set_rows(uRows);
        pImage->set_columns(uCols);
        pImage->set_bitsallocated(uBitsAlloc);
        pImage->set_bitsstored(uBitsStored);
        pImage->set_highbit(uHighBit);
        pImage->set_pixelrepresentation(uPixelRepresentation);
        pImage->set_windowcenter(strWindowCenter);
        pImage->set_windowwidth(strWindowWidth);
        pImage->set_sopclassuid(sSopClassUid);

        Mcsf::IDataHeaderElementMap* pDataHeader = nullptr;//Mcsf::IDataHeaderElementMap::CreateDataHeader();
        spDataHeader->Clone(&pDataHeader);

        int iRet = this->SaveImage(pImage, pDataHeader, i, xStartPos, yStartPos);

        vImageList.push_back(pImage);
        DEL_PTR(pDataHeader);

        if(TPS_ER_SUCCESS != iRet)
        {
            DEL_ARRAY(pImageData);
            //delete series
            if (TPS_ER_SUCCESS != mSaveObjects->EraseObject(sSeriesUid, RtDbDef::TYPE_SERIES)){
                TPS_LOG_DEV_ERROR<<"Failed to delete the series:"<<sSeriesUid;
                return TPS_ER_DB_ERROR;
            }
            return TPS_ER_FAILURE;
        }
    }
    if(!this->DeleteMultiFolder(sImage))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd:: DeleteMultiFolder failed with series uid:" << sSeriesUid;
        return TPS_ER_FAILURE;
    }

    //////////////////////////////////////////////////////////////////////////
    // Convert buffer from unsigned short* to unsigned char* 
    //    based on DICOM TAG: window center, window width, rescale slope, intercept
    DEL_ARRAY(m_pImageDataBuffer);
    m_pImageDataBuffer = new unsigned char[uCols*uRows];
    double winWidth = atof(strWindowWidth.c_str());
    double winCenter = atof(strWindowCenter.c_str());
    double rescaleSlope = atof(cRescaleSlope.c_str());
    double fIntercept = atof(cRescaleIntercept.c_str());

    if( winWidth> 1)
    {
        for(int i = 0; i<uCols*uRows; ++i)
        {
            float temp = ((float)pImageData[i]*(float)rescaleSlope+fIntercept-winCenter)/winWidth +0.5f;
            if(temp >= 1) 
            {
                m_pImageDataBuffer[i] = 255;
            } else if(temp <= 0) 
            {
                m_pImageDataBuffer[i] = 0;
            }  else {
                m_pImageDataBuffer[i] = (unsigned char )(temp*255);
            }
        } 
    }else if(1==winWidth) 
    {
        for(int i = 0; i<uCols*uRows; ++i) 
        {
            float temp = ((float)pImageData[i]*(float)rescaleSlope+fIntercept-winCenter)/winWidth +0.5f;
            if(temp >= 1) {
                m_pImageDataBuffer[i] = 255;
            } else if(temp <= 0) {
                m_pImageDataBuffer[i] = 0;
            } 
        } 
    }
    else
    {
        TPS_LOG_DEV_ERROR<<"Illegal window width["<<winWidth<<"] in created phantom series.";   
        DEL_ARRAY(pImageData);
        return TPS_ER_FAILURE;
    }

    // assign phantom buffer info values
    m_iImageDataBufferWidth = uCols;
    m_iImageDataBufferHeight = uRows;
    m_strNewPhantomUid = sSeriesUid;

    //////////////////////////////////////////////////////////////////////////
    //BuildImage3DFromSeries
    if(!mDatabaseWrapper->BuildImage3DFromSeries(pSeries, vImageList, lTotalSize))
    {
        for (auto itr=vImageList.begin(); itr!=vImageList.end(); ++itr)
        {
            DEL_PTR(*itr);
        }
        // free the unmanaged memory.
        DEL_ARRAY(pImageData);
        TPS_LOG_DEV_ERROR << "Failed to BuildImage3DFromSeries";
        return TPS_ER_FAILURE;
    }

    for (auto itr=vImageList.begin(); itr!=vImageList.end(); ++itr)
    {
        DEL_PTR(*itr);
    }
    // free the unmanaged memory.
    DEL_ARRAY(pImageData);

    //////////////////////////////////////////////////////////////////////////
    //added to manager
    TpsImageDataManager* pTmp = mDataRepository->GetImageDataManager();
    if (nullptr == pTmp)
    {
        TPS_LOG_DEV_ERROR << "Failed to get image data manager;";
        return TPS_ER_FAILURE;
    }

    TpsImage3DEntity* pTpsImage3D = pTmp->CreateImage3D();
    if (pTpsImage3D == nullptr)
    {
        TPS_LOG_DEV_ERROR << "pTpsImage3D is nullptr";
        return TPS_ER_FAILURE;
    }
    if (!pTpsImage3D->SetupImage3D(pSeries))
    { // get Image3D from series
        DEL_PTR(pTpsImage3D);
        TPS_LOG_DEV_ERROR << "Failed to set up image 3d object.";
        return TPS_ER_FAILURE;
    }
    // set up a default slice buffer for each Image3d Entity
    if (!pTpsImage3D->InitSliceBuffer())
    {
        TPS_LOG_DEV_ERROR << "Failed to Initialize slice buffer.";
        DEL_PTR(pTpsImage3D);
        return TPS_ER_FAILURE;
    }
    if (!mDataRepository->GetImageDataManager()->AddImage3D(sSeriesUid, pTpsImage3D))
    {
        TPS_LOG_DEV_ERROR << "Failed to add image 3d object(uid: " << sSeriesUid << ") into ImageDataManager.";
        DEL_PTR(pTpsImage3D);
        return TPS_ER_FAILURE;
    }

    SeriesTable* srsTbl = new SeriesTable();
    srsTbl->SetSeriesUid(sSeriesUid);
    mDataRepository->GetTableManager()->AddSeriesTable(sSeriesUid, srsTbl);

    TEST_PERFORMANCE_END("PhantomCreateCmd");
    return TPS_ER_SUCCESS;
}

int PhantomCreateCmd::SaveImage(RtImage* pImage, Mcsf::IDataHeaderElementMap* pDataHeader, int index, float xStartPos, float yStartPos)
{
    if (nullptr == pImage || nullptr == pDataHeader)
    {
        return TPS_ER_FAILURE;
    }

    pImage->set_uid(mDatabaseWrapper->GeneraterUid());

    //image to save
    const std::string sImgUid = pImage->get_uid();

    char buffer[256];
    std::string sBufferString("");
    int iCount;
    bool bRet;

    // SOPInstanceUID actually is image UID in this case
    sBufferString = sImgUid;
    iCount = static_cast<int>(sBufferString.length());
    if(!pDataHeader->SetStringByTag(Mcsf::kTagDcm_SOPInstanceUID, sBufferString.c_str(), iCount))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: SetStringByTag kTagDcm_SOPInstanceUID failed!";
        return TPS_ER_FAILURE;
    }
    pImage->set_sopinstanceuid(sBufferString);

    // 0x00200013 Instance Number
    sprintf_s(buffer, "%d", index + 1);
    Mcsf::ConstString cInstanceID = buffer;
    if(!pDataHeader->SetStringByTag(Mcsf::kTagDcm_InstanceNumber, cInstanceID, static_cast<int>(std::strlen(cInstanceID))))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: InsertStringByTag kTagDcm_InstanceNumber failed!";
        return TPS_ER_FAILURE;
    }
    pImage->set_instancenumber(index + 1);

    // 0x00200032 kTagDcm_ImagePositionPatient
    //sprintf_s(buffer, "-256\\-256\\%.3f", -sliceSpacing * i);
    sprintf_s(buffer, "%.3f\\%.3f\\%.3f", xStartPos, yStartPos, -m_PhantomSeries.sliceSpacing * index);
    Mcsf::ConstString cImagePositionPatient = buffer;
    if(!pDataHeader->SetStringByTag(Mcsf::kTagDcm_ImagePositionPatient, cImagePositionPatient, static_cast<int>(std::strlen(cImagePositionPatient))))
    {
        TPS_LOG_DEV_ERROR << "PhantomCreateCmd: SetStringByTag kTagDcm_ImagePositionPatient failed!";
        return TPS_ER_FAILURE;
    }
    pImage->set_imagepositionpatient(cImagePositionPatient);

    // ???
    sprintf_s(buffer, "%.3f", -m_PhantomSeries.sliceSpacing * index);
    Mcsf::ConstString cSliceLocation = buffer;
    pImage->set_slicelocation(cSliceLocation);

    //save dcm file
    Mcsf::ISystemEnvironmentConfig* pSystemEnvironmentConfig = Mcsf::ConfigSystemEnvironmentFactory::Instance()->GetSystemEnvironment();
    if ( nullptr == pSystemEnvironmentConfig )
    {
        TPS_LOG_DEV_ERROR<<"GetSystemEnvironment pSystemEnvironmentConfig is null!";
        return TPS_ER_FAILURE;
    }
    //get version of TPS
    //D:/UIH
    const std::string sUihRootPath = pSystemEnvironmentConfig->GetRootPath();
    const std::string sImage= sUihRootPath + "/temp";
    std::string sImageFilePath = sImage + "/" + sImgUid + ".DCM";
    if (!this->SaveDcmFile(sImageFilePath, pDataHeader))
    {
        TPS_LOG_DEV_ERROR<<"Failed to save dcm file.";
        DeleteMultiFolder(sImage);
        return TPS_ER_FAILURE;
    }

    char* pBuffer = nullptr;
    unsigned long ulSize(0);
    bRet = this->ConvertDcmFile2Cache(sImageFilePath, pBuffer, &ulSize);
    if (!bRet || ulSize < 1 || nullptr == pBuffer)
    {
        TPS_LOG_DEV_ERROR << "Failed to ConvertDcmFile2Cache";
        DeleteMultiFolder(sImage);
        return TPS_ER_FAILURE;
    }
    else 
    {
        pImage->set_dcmfiledata(pBuffer, ulSize);
        DEL_ARRAY(pBuffer);
    }

    if (TPS_ER_SUCCESS != mSaveObjects->SaveImage(*pImage))
    {
        TPS_LOG_DEV_ERROR<<"Failed to save image:"<<sImgUid;
        DeleteMultiFolder(sImage);
        return TPS_ER_FAILURE;
    }

    return TPS_ER_SUCCESS;
}

bool PhantomCreateCmd::SaveDcmFile(const std::string& sImageFilePath, Mcsf::IDataHeaderElementMap* const pDataHeaderImage)
{
    Mcsf::ISimpleDICOMConvertorPtr pConvertor = Mcsf::DICOMConvertorFactory::Instance()->CreateSimpleConvertor();
    if(!pConvertor->SaveFile(pDataHeaderImage,sImageFilePath))
    {
        TPS_LOG_DEV_ERROR<<"Failed to save dcm file.";
        return false;
    }
    return true;
}

bool PhantomCreateCmd::ConvertDcmFile2Cache(const std::string& sImageFilePath, char*& pBuffer, unsigned long* ulSize)
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


int PhantomCreateCmd::PostExecute()
{
    return TPS_ER_SUCCESS;
}

float PhantomCreateCmd::Density2CT(float fDensity, const float* Ct2densityArray)
{
    float mMaxCTValue = 5000;
    float mMinCTValue = -1024;

    int ctValue = -2000;
    float currentDensityValue = -1;
    float nextDensityValue = -1;
    float minDensityValue = 6;
    float maxDensityValue = -1;

    // reference SSFS key 8251
    if (fDensity < 0.0f){
        TPS_LOG_DEV_WARNING<<"Density value: "<< fDensity << " is out of scope";
        return mMinCTValue;
    }
    if (fDensity > 5.0f){
        TPS_LOG_DEV_WARNING<<"Density value: "<< fDensity << " is out of scope";
        return mMaxCTValue;
    }

    for (int i = 0; i < mMaxCTValue - mMinCTValue; ++i){
        currentDensityValue = Ct2densityArray[i];
        nextDensityValue = Ct2densityArray[i+1];

        if ((currentDensityValue <= fDensity && nextDensityValue > fDensity)
            || (currentDensityValue >= fDensity && nextDensityValue < fDensity))
        {
            ctValue = (i + mMinCTValue) + (fDensity - currentDensityValue) / (nextDensityValue - currentDensityValue);
            break;//accepted the first one and ignored others
        }
        if (minDensityValue > currentDensityValue){
            minDensityValue = currentDensityValue;
        }
        if (maxDensityValue < currentDensityValue){
            maxDensityValue = currentDensityValue;
        }
    }

    if (-2000 == ctValue){
        if (fDensity > maxDensityValue){
            ctValue = mMaxCTValue;
        }
        else if (fDensity < minDensityValue){
            ctValue = mMinCTValue;
        }
    }

    return ctValue;
}

bool PhantomCreateCmd::CreateMultiFolder(const std::string& sFolderName) const
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

bool PhantomCreateCmd::DeleteMultiFolder(const std::string& sFolderName) const
{
    try{
        boost::filesystem::path b_dir_path(sFolderName);
        if(boost::filesystem::exists(b_dir_path))
        {
            boost::filesystem::remove_all(b_dir_path);
        }
    }catch(boost::filesystem::filesystem_error & x)
    {
        TPS_LOG_DEV_ERROR<<("***** Deleting directory: " + sFolderName + " failed.          *****\n" + std::string(x.what()));
        return false;
    }
    return true;
}

std::string PhantomCreateCmd::GetPhantomSeriesUID() 
{
    return m_strNewPhantomUid;
}

std::string PhantomCreateCmd::ConvertDateTime(const DATE_BOOST& date)
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

TPS_END_NAMESPACE