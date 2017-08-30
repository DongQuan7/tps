////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_save_objects.h
/// 
///  \brief class 
/// 
///  \version 1.0
/// 
///  \date    Oct.16,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_SAVE_OBJECTS_H_
#define TPS_BL_SAVE_OBJECTS_H_

#include "tps_bl_defs.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"

TPS_BEGIN_NAMESPACE;

class ITpsDataRepository;
class IRtDatabaseWrapper;
class RtPoi;
class RtVoi;
class ROIEntity;
class RtRtimage;
class RtDoseline;
class RtCt2density;
class RtSeries;
class RtImage;
class RtPlan;
class RtNormgroup;
class RtBeam;
class SeriesTable;
class RtNorm_voi;
class RtPlan_voi;
class RtTablecontour;
class RtBeamsegment;
class RtBlock;
class RtTemplate;
class RtApproverecord;
class RtPatient;

class TPS_BL_DECLSPEC TpsSaveObjects
{
public:
    TpsSaveObjects();
    virtual ~TpsSaveObjects(void);

    void SetDataRepository(ITpsDataRepository* dataRepos) {mDataRepository=dataRepos;}
    void SetDatabaseWrapper(IRtDatabaseWrapper* dataWrapper) {mDatabaseWrapper=dataWrapper;}

    int SavePlan(RtPlan& plan);
    int SaveNormgroup(RtNormgroup& normgroup);

    int InsertBeam(RtBeam& beam);
    // include bolus
    int UpdateBeam(RtBeam& beam);
    int SaveBeam(const std::string& sBeamUid);
    int UpdateBeamAttributes(RtBeam& beam);
    int UpdateBeamSegments(std::vector<RtBeamsegment*> beamSegments);
    int SaveBeamBlock(RtBlock& block);
    //check data include:
    // gantryrotationdirection collimatorrotationdirection
    int CheckData(const RtBeam& beam);

    int SaveDosegridsForNormgroup(const std::string& sNormgroupUid);
    int SaveDosegridsForPlan(const std::string& sPlanUid);
    int SavePoi(RtPoi& poi);
	int SaveVoiObject(RtVoi* voi);
    int SaveRoi(ROIEntity& roiEntity);
    int SaveRtImage(RtRtimage& rtImage);
    int SaveDoselines(const std::string& sPlanUid, const std::vector<RtDoseline*>& vdoselines);
    int SaveCt2Density(RtCt2density& ct2Density);

    int SaveSeriesTable(const SeriesTable& seriesTable);

    int SaveSeries(RtSeries& series);
    int SaveImage(RtImage& image);

    int UpdateSeries(RtSeries& series);
    int UpdatePoi(RtPoi* pPoi);
    int UpdateVoiAttributes(const ROIEntity& voiEntity);

    std::string GetSavePathForImage();
    bool CreatePath(const std::string& sPath);

    /// \brief  Erase object on database
    ///  
    /// \param[in] const std::string& sUid          UID of the object
    /// \param[in] RtDbDef::OBJECT_TYPE objType    the type of the object
    /// \return int TPS return codes
    int EraseObject(const std::string& sUid, RtDbDef::OBJECT_TYPE objType) const;

    /// \brief  Erase object on database
    ///  
    /// \param[in] const std::string& sFirstUid    first UID of the object
    /// \param[in] const std::string& sSecondUid   second UID of the object
    /// \param[in] RtDbDef::OBJECT_TYPE objType    the type of the object
    /// \return int
    int EraseObject(const std::string& sFirstUid,const std::string& sSecondUid, 
        RtDbDef::OBJECT_TYPE objType) const;
    int UpdateBeamPoiDose(RtPoi* pPoi);

    int SaveNormVoi(RtNorm_voi& normVoi);

    int SavePlanVoi(RtPlan_voi& planVoi);

    int DeleteNormVoiViaNormalUid(const std::string& ngUid);

    int DeletePlanVoiViaPlanUid(const std::string& planUid);

    int SaveTableContours(const std::vector<RtTablecontour*>& tableContous);


    int SaveApproveRecord(RtApproverecord& approveRecord);
    int DeleteApproveRecordViaObjUid(const std::string& objectUid);

    int SaveTemplate(RtTemplate* sTemplate);
    int UpdateTemplate(RtTemplate* sTemplate);
    int RemoveTemplate(std::string& templateUid);

	int SavePatient(RtPatient* pPatient);
    int UpdatePatient(RtPatient* pPatient);

private:
    ITpsDataRepository*         mDataRepository;
    IRtDatabaseWrapper*         mDatabaseWrapper;
};

TPS_END_NAMESPACE

#endif //TPS_PCA_ADDTRYCMD_H
