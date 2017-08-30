//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright,(c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ChenXi xi.chen@united-imaging.com
///
///  \file    tps_bl_load_objects.h
///  \brief   load data from DB
///
///  \version 1.0
///  \date    Oct. 21, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_LOAD_OBJECTS_H_
#define TPS_BL_LOAD_OBJECTS_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include <vector>
#include "boost/thread/thread.hpp"
#include "boost/thread/detail/thread_group.hpp"

namespace Mcsf{
    class Point3f;
    class Matrix4f;
}

TPS_BEGIN_NAMESPACE

struct CertifiedUnit;
class ROIEntity;
class TpsImage3DEntity;
class FTStringBufferGenerator;
class TPSVOIMesh;
class ITpsDataRepository;
class IRtDatabaseWrapper;
class DoseCalculationImpl;
class TpsDoseProxy;
class VOITemplate;
class DoseLineTemplate;
class BeamTemplate;
class OptimizerTemplate;

class RtPatient;
class RtCourse;
class RtSeries;
class RtVoi;
class RtPlan;
class RtNormgroup;
class RtBeam;
class RtDosegrid;
class RtMachine;
class RtCommissionedunit;
class RtAccessorychunk;
class RtAccessory;
class RtTable;
class RtTablecontour;
class SeriesTable;
class RtTemplate;

class TPS_BL_DECLSPEC TpsLoadObjects {

public:
	TpsLoadObjects(void);
	virtual ~TpsLoadObjects(void);

    virtual bool Initialize();
    virtual bool Finalize();

    void SetDataRepository(ITpsDataRepository* repos);
    void SetDatabaseWrapper(IRtDatabaseWrapper* dbWrapper) {mDatabaseWrapper=dbWrapper;}
    void SetDoseCalculation(DoseCalculationImpl* doseCalc) { mDoseCalculation = doseCalc; }
    void SetDoseProxy(TpsDoseProxy* doseProxy) { mDoseProxy = doseProxy; }
    
    bool ReloadPatient(const std::string&);

    bool LoadTcpNTcpParameter();

    bool LoadSeries(const std::string&, std::vector<std::string>&);

    int LoadSeries(const std::vector<std::string>& seriesUIDList);

    int LoadROIs(const std::vector<std::string>& structureSetUIDList);

    int LoadPlans(const std::vector<std::string>& planUIDList);

    int LoadDoseGrids(const std::vector<std::string>& doseGridUidList, 
        std::vector<std::string>& affectedPlanUidList);

    int UpdatePlanOrder(const std::string& patientUid);
   // bool LoadVOIs(const std::vector<std::string>&);

    bool LoadPOIs(const std::vector<std::string>&);

    //bool LoadAllCoursesOfPatient(const std::string&,float& totalGoal);

    /// \brief  Load image 3d from TMS
    /// \param[in] sImage3DUID  the image 3d UID in database
    /// \return bool  [true] if succeeded, [false] otherwise
    bool LoadImage3D(const std::string& sImage3DUID);

    /// \brief load machine list from TMS
    ///  
    /// \return bool  [true] if succeeded, [false] otherwise
    int LoadMachineTree();

    int LoadKernelData();

    bool LoadSystemCT2DensityTables();

    /// \brief load prescription list from TMS
    ///  
    /// \return bool  [true] if succeeded, [false] otherwise
    virtual bool LoadPrescription();

    /// \brief load voi template from DataManager
    ///  
    /// \param[out] 
    /// \return bool  [true] if succeeded, [false] otherwise
    int LoadVOITemplate(std::vector<VOITemplate*>& voiTemplateList);
    /// \brief load beam template list from DataManager
    ///  
    /// \param[out] 
    /// \return bool  [true] if succeeded, [false] otherwise
    int LoadBeamTemplate(std::vector<BeamTemplate*>& beamTemplateList);
    /// \brief load beam template list from DataManager
    ///  
    /// \param[out] 
    /// \return bool  [true] if succeeded, [false] otherwise
    int LoadOptimizerTemplateList(std::vector<OptimizerTemplate*>& optimizerTemplateList);

    int LoadDoseLineTemplate(std::vector<DoseLineTemplate*>& doseLineTemplateList);

    int LoadWindowingTemplateList(std::vector<RtTemplate*>& windowingTemplateList);

    bool LoadPatientTree(const std::string&);

    bool LoadPatientTreeForEasyplan(const std::string&);

    bool LoadPatientForGallery(const std::string&);

    bool LoadPhantomSeries(std::vector<std::string>& phantomUidList);

    bool LoadPlanReportData(const std::string& planUID, const std::string& courseUID);

    bool ComposeDoseForPlanReport(const std::string& planUID);

    bool LoadMachineSetting();

    int LoadTemplate();

    bool ValidateCT2DensityOfSeries(RtSeries* series);

    static bool InitializeTableLocation(const TpsImage3DEntity* image3d, 
        const RtTable* rtTable, SeriesTable* seriesTable);

    TPS_ERROR_CODES GetLastError() const;

private:

    int LoadMachines_i(const std::vector<std::string>& approvedMachineUidList);
    /// \brief Load VOI list from TMS
    /// \param[in] pSeries      The Series of TMS
    /// \return bool 
    bool LoadVOIListFromSeries_i(const RtSeries& tmsSeries);
    void _stdcall LoadVOIFromDatabase_i(RtVoi* pVoi, TpsImage3DEntity* image);

    /// \brief Load POI list from TMS
    /// \param[in] pSeries      The Series of TMS
    /// \return bool 
    bool LoadPOIListFromSeries_i(const std::string& sSeriesUid);

    /// \brief Load DoseLine Params from TMS
    /// \param[in] pPlan      The Plan of TMS
    /// \return bool 
    bool LoadDoseLineParamsFromPlan_i(const RtPlan* plan);



    /// \brief  Load image 3d via a RTFWKDLDOSeries object
    /// \param[in] pSeries  the RTFWKDLDOSeries pointer
    /// \return bool  [true] if succeeded, [false] otherwise
    bool LoadImage3D_i(RtSeries* const pSeries);

    bool LoadImage3DForGallery_i(RtSeries* const pSeries);

    bool LoadROI_i(const std::string& imgUID, ROIEntity* roi);

    /// \brief Load plans from TMS
    /// \param[in] vCourseUID          the list of course
    /// \return bool 
    //bool LoadPlans_i(std::vector<std::string>& vCourseUID);

    bool LoadPlansByCourse_i(RtCourse& aCourse, std::vector<std::string>& loadedPlanUID);
    bool LoadPlansByCourseForEasyplan_i(RtCourse& aCourse, std::vector<std::string>& loadedPlanUID);
    bool LoadPrescriptionByCourse_i(RtCourse&);
    bool LoadSiteByCourese_i(const std::string& courseUid);
    bool LoadPlanData_i(RtPlan& aPlan);
    bool LoadEasyPlanData_i(RtPlan &easyPlan);
    bool LoadEasyNormgroupsByPlan_i(RtPlan &easyPlan);

    //set totaldose for Plan
    bool LoadNormgroupsByPlan_i(RtPlan& aPlan);
    bool LoadNormgroupData_i(RtNormgroup& aNormgroup, TpsImage3DEntity* image3D);
    bool AddNormgroupToNGManager_i(RtNormgroup* aNormGroup, RtPlan* plan);

    bool LoadCourseByPatient_i(RtPatient&, std::vector<std::string>& loadedCourseUID);
    bool LoadCourseByPatientForEasyplan_i(RtPatient&, std::vector<std::string>& loadedCourseUID);
    bool LoadCourseByPatientForGallery_i(RtPatient&, std::vector<std::string>& loadedCourseUID);
    bool LoadBeamGroupUIDs(RtCourse&, std::vector<std::string>& vBeamGroupUIDs);

    bool LoadSeriesByCourse_i(RtCourse&, std::vector<std::string>& loadedSeriesUID);
    bool LoadSeriesByCourseForGallery_i(RtCourse&, std::vector<std::string>& loadedSeriesUID);

    /// \brief Prepare one image text string for display
    /// \param[in]  bufGen          buffer generator
    /// \param[in]  strText         text string for display
    /// \param[in]  strKey          key for storing this text in data manager
    bool PrepareOneImageTextString_i(FTStringBufferGenerator& bufGen,
        const std::string& strText, const std::string& strKey);

    /// \brief Add doseLine to DoseListManager
    /// \param[in] plan
    /// \return bool 
    bool AddDoseLine(const RtNormgroup& normGroup);

    bool AddBeamIECCS_i(const Mcsf::Point3f &orgPt,
        std::string patientPosition,
        float sad,
        RtBeam &beam);

    /// \brief Load float images registered with certain series
    /// \param[in] fixSeriesUID fix image uid
    /// \return bool 
    bool LoadImageTransformation_i(const std::string &fixSeriesUID);

    //to be removed
    bool ClearPatient(const std::string&);

    bool LoadImage2DData(const std::string &imageUID);
    bool LoadImage2DDataByBeamUID(const std::string& beamUID);
    bool GeneratePlanDoseGrid_i(RtPlan& aPlan);
    bool GenerateDoseGrid_i(const std::string& seriesUid, 
        RtDosegrid*& dosegridOut);
    void ClearBeamList_i(std::vector<RtBeam*>& beamList);
    bool HandleEmptyDosegridForBeam_i(RtBeam* pBeam, 
        const std::string& seriesUid);

    bool LoadRtTables_i();
    bool LoadSeriesTable_i(const TpsImage3DEntity& image3D);
    void ConvertToIECTableSpace_i(std::vector<RtTablecontour*>& contours);
    bool SetupTableVolume_i(RtTable* table);

    static Mcsf::Matrix4f CalcTableToPatientViaPatientPosition_i(
        int patientPosition, const float* srsBdry, const float* tblBdry);

    bool LoadSeriesByPatient_i(RtPatient&);
    bool LoadSeriesByPatientForGallery_i(RtPatient&);
    bool ReadMachineLeafGapOffset_i();

private:
    std::string mconfigUid;
    ITpsDataRepository          *mDataRepository;
    IRtDatabaseWrapper          *mDatabaseWrapper;
    DoseCalculationImpl         *mDoseCalculation;
    TpsDoseProxy                *mDoseProxy;
    boost::thread_group         mBoostThread;
    TPS_ERROR_CODES             mLastError;

private:
	TPS_DISALLOW_COPY_AND_ASSIGN(TpsLoadObjects);
};

TPS_END_NAMESPACE

#endif  // TPS_BL_LOAD_OBJECTS_H_
