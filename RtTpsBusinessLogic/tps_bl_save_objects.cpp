////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_cmd_create_plan.cpp
/// 
///  \brief class 
/// 
///  \version 1.0
/// 
///  \date    Dec.16,2015
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

//ZHENGHE
#include "McsfGeometry/mcsf_geometry_sub_volume.h"

//TPS
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_ct2density.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_norm_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_template.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"


#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_table.h"
#include "RtTpsDataAccess/tps_da_tablemanager.h"

#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include <boost/date_time/posix_time/posix_time.hpp>

TPS_BEGIN_NAMESPACE;

TpsSaveObjects::TpsSaveObjects() : mDataRepository(nullptr),mDatabaseWrapper(nullptr)
{

}

TpsSaveObjects::~TpsSaveObjects(void) 
{

}

int TpsSaveObjects::SaveDosegridsForNormgroup(const std::string& sNormgroupUid)
{
    //TEST_PERFORMANCE_INIT;
    //TEST_PERFORMANCE_BEGIN;
    TPS_LOG_DEV_INFO<<"[INFO][TpsSaveObjectsCmd][SaveDosegridsForNormgroup]:"<<sNormgroupUid;
    RtNormgroup *normgroup = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(sNormgroupUid, &normgroup)) 
    {
        TPS_LOG_DEV_ERROR<<"Invalid normgroup["<<sNormgroupUid<<"] instance ";
        return TPS_ER_FAILURE;
    }
    RtDosegrid *dosegrid = normgroup->get_dosegrid();
    if (dosegrid == nullptr) 
    {
        TPS_LOG_DEV_ERROR<<"Invalid normgroup"<<sNormgroupUid<<"dose grid instance in dose manager";
        return TPS_ER_FAILURE;
    }

    //////////////////////////////////////////////////////////////////////////
    //update dose grid for Beams
    std::vector<RtBeam*> vBeams = mDataRepository->GetBeamManager()->GetBeamsByNormGroup(sNormgroupUid);
    RtDosegrid *pDosegridBeam = nullptr;
    for (auto itr=vBeams.begin(); itr!=vBeams.end(); ++itr)
    {
        //safer
        if (*itr == nullptr)
        {
            continue;
        }
        pDosegridBeam = (*itr)->get_dosegrid();
        if (nullptr == pDosegridBeam)
        {
            continue;
        }
        if (nullptr == pDosegridBeam->get_dosegrid_buffer()) 
        {
            continue;
        }

        pDosegridBeam->set_beamuid((*itr)->get_uid());
        //!!! NOTE if you want to save dose grid value to a file please set_isdosevalid!! qqzhou@20170222
        if (mDatabaseWrapper->QueryDBInTable(pDosegridBeam->get_uid(), RtDbDef::TYPE_DOSEGRID))
        {
            //update
            if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateDosegridObject(pDosegridBeam))
            {
                TPS_LOG_DEV_ERROR << "Failed to Update Dosegrid Object :" << pDosegridBeam->get_uid();
                return TPS_ER_DB_ERROR;
            }
        }
        else if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertDosegridObject(pDosegridBeam))
        {
            //insert
            TPS_LOG_DEV_ERROR << "Failed to InsertDoselineObject :" << pDosegridBeam->get_uid();
            return TPS_ER_DB_ERROR;
        }
    }
    //TEST_PERFORMANCE_END("TpsSaveObjects::SaveDosegridsForNormgroup");
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::SaveDosegridsForPlan(const std::string& sPlanUid)
{
    TPS_LOG_DEV_INFO<<"[INFO][TpsSaveObjectsCmd][SaveDosegridsForPlan]: "<<sPlanUid;
    //update dosegrid for NormGroup
    std::vector<std::string> vNormGroupList =
        mDataRepository->GetNormgroupManager()->GetNormGroupUIDListByPlan(sPlanUid);
    int iRet = TPS_ER_FAILURE;
    for (auto itrNg=vNormGroupList.begin(); itrNg != vNormGroupList.end(); ++itrNg){
        iRet = this->SaveDosegridsForNormgroup(*itrNg);
        if (TPS_ER_SUCCESS != iRet){
            return iRet;
        }
    }
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::SavePlan(RtPlan& plan)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;
    if (!mDatabaseWrapper->QueryDBInTable(plan.get_uid(), RtDbDef::TYPE_PLAN)){
        if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertPlanObject(&plan)){
            return TPS_ER_DB_ERROR;
        }
    } 
    else{ 
        if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdatePlanObject(plan)){
            return TPS_ER_DB_ERROR;
        }
    }

    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::SaveNormgroup(RtNormgroup& normgroup)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;
    if (!mDatabaseWrapper->QueryDBInTable(normgroup.get_uid(), RtDbDef::TYPE_NORMGROUP)){
        if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertNormgroupObject(&normgroup)){
            return TPS_ER_DB_ERROR;
        }
    }
    else if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateNormgroupObject(normgroup)){
        return TPS_ER_DB_ERROR;
    }
    return TPS_ER_SUCCESS;
}

// 插入一个新的Beam
int TpsSaveObjects::InsertBeam(RtBeam& beam)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;

    if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertBeamObject(beam))
    {
        return TPS_ER_DB_ERROR;
    }
    return TPS_ER_SUCCESS;
}

// 更新一个已有的Beam
int TpsSaveObjects::UpdateBeam(RtBeam& beam)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;

    //delete old segments to be removed later as that causes memory leak for segmgents
    const std::string sBeamUid = beam.get_uid();
    const std::string sCondition = "beamuid='" + sBeamUid + "'";
    int iRet = mDatabaseWrapper->EraseObjectUnderCondition(sCondition, RtDbDef::TYPE_BEAMSEGMENT);
    if (DB_CODES_SUCCESS != iRet)
    {
        return TPS_ER_DB_ERROR;
    }
    iRet = mDatabaseWrapper->EraseObjectUnderCondition(sCondition, RtDbDef::TYPE_BLOCK);
    if (DB_CODES_SUCCESS != iRet)
    {
        return TPS_ER_DB_ERROR;
    }

    iRet = mDatabaseWrapper->UpdateBeamObject(beam);
    if (DB_CODES_SUCCESS != iRet)
    {
        TPS_LOG_DEV_ERROR << "Failed to UpdateBeam.";
        return TPS_ER_DB_ERROR;
    }

    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::SaveBeamBlock(RtBlock& block)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;

    const std::string sBlockUid = block.get_uid();

    int iRet;
    // update
    if (mDatabaseWrapper->QueryDBInTable(sBlockUid, RtDbDef::TYPE_BLOCK))
    {
        iRet = mDatabaseWrapper->UpdateBlock(block);
        if (DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR << "Failed to save Beam Block.";
            return TPS_ER_DB_ERROR;
        }
    }
    else // insert
    {
        iRet = mDatabaseWrapper->InsertBlock(block);
        if (DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR << "Failed to save Beam Block.";
            return TPS_ER_DB_ERROR;
        }
    }

    return TPS_ER_SUCCESS;
}


// 保存Beam 包括Insert和Update
int TpsSaveObjects::SaveBeam(const std::string& sBeamUid)
{
    if (nullptr == mDatabaseWrapper)
    {
        TPS_LOG_DEV_ERROR << "mDatabaseWrapper is null.";
        return TPS_ER_FAILURE;
    }

    if (nullptr == mDataRepository)
    {
        TPS_LOG_DEV_ERROR << "mDataRepository is null.";
        return TPS_ER_FAILURE;
    }

    TPS_LOG_DEV_INFO << "[INFO][TpsSaveObjectsCmd][SaveBeam]: " << sBeamUid;

    RtBeam* pBeam = mDataRepository->GetBeamManager()->GetBeam(sBeamUid);
    if (nullptr == pBeam)
    {
        return TPS_ER_FAILURE;
    }

    if (!mDatabaseWrapper->QueryDBInTable(sBeamUid, RtDbDef::TYPE_BEAM))
    {
        if (DB_CODES_SUCCESS != this->InsertBeam(*pBeam))
        {
            return TPS_ER_DB_ERROR;
        }
    }
    else
    {
        // update
        if (TPS_ER_SUCCESS != CheckData(*pBeam))
        {
            TPSPRINTF("TpsSaveObjects::SaveBeam::CheckData failed %s\n", sBeamUid.c_str());
            return TPS_ER_FAILURE;
        }

        return this->UpdateBeam(*pBeam);
    }

    return TPS_ER_SUCCESS;
}

// 修改Beam简单属性
int TpsSaveObjects::UpdateBeamAttributes(RtBeam& beam)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateBeamAttributes(beam))
    {
        TPS_LOG_DEV_ERROR << "failed to UpdateBeamAttributes";
        return TPS_ER_DB_ERROR;
    }
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::CheckData(const RtBeam& beam)
{
    std::vector<RtBeamsegment*> segs = beam.get_beamsegments();
    for (auto itr=segs.cbegin(); itr!=segs.cend(); ++itr)
    {
        if ((*itr)->get_gantryrotationdirection() < RtDbDef::NONE_ROTATION_DIRECTION 
            || (*itr)->get_gantryrotationdirection() > RtDbDef::CW )
        {
            TPS_LOG_DEV_ERROR << "gantry rotation direction invalid:"<<(*itr)->get_gantryrotationdirection();

#ifdef _DEBUG
            printf("\n gantry rotation direction invalid:%d\n" ,(*itr)->get_gantryrotationdirection());
#endif
            return TPS_ER_FAILURE;
        }

        if ((*itr)->get_collimatorrotationdirection() < RtDbDef::NONE_ROTATION_DIRECTION 
            || (*itr)->get_collimatorrotationdirection() > RtDbDef::CW )
        {
            TPS_LOG_DEV_ERROR << "collimator rotation direction invalid:"<<(*itr)->get_collimatorrotationdirection();
#ifdef _DEBUG
            printf("\n collimator rotation direction invalid:%d\n" ,(*itr)->get_collimatorrotationdirection());
#endif
            return TPS_ER_FAILURE;
        }
        //TODO added more if need
    }

    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::SavePoi(RtPoi& poi)
{
    if (nullptr != mDatabaseWrapper){
        if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertPoiObject(poi)){
            return TPS_ER_DB_ERROR;
        }
    }
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::SaveCt2Density(RtCt2density& ct2Density)
{
    return mDatabaseWrapper->InsertCt2densityObject(ct2Density);
}

int TpsSaveObjects::SaveVoiObject(RtVoi* voi)
{
	TEST_PERFORMANCE_INIT;
	TEST_PERFORMANCE_BEGIN;

	TPS_LOG_DEV_INFO<<"[INFO][TpsSaveObjects][SaveVoiObject] enter";
	
	voi->set_softwareversionno(mDatabaseWrapper->GetTpsVersion());

	if (mDatabaseWrapper->QueryDBInTable(voi->get_uid(), RtDbDef::TYPE_VOI)) 
	{
		// 如果记录已经存在
		if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateVoiObject(*voi))
		{
			TPS_LOG_DEV_ERROR<<"UpdateVoiObject failed!";
			return TPS_ER_DB_ERROR;
		}
	}
	else 
	{
		// 如果记录尚未存在
		const std::string seriesuid = voi->get_seriesuid();
		std::vector<RtStructureset*> vList;
		std::string sStructuresetUid("");
		mDatabaseWrapper->GetStructuresetListBySeriesUid(seriesuid, vList);
		if (vList.empty()){
			RtStructureset structureset(true);
			structureset.set_seriesuid(seriesuid);
			sStructuresetUid = structureset.get_uid();
			if(DB_CODES_SUCCESS != mDatabaseWrapper->InsertStructureset(structureset)){
				TPS_LOG_DEV_ERROR<<"Insert Structure set failed!";
				return TPS_ER_DB_ERROR;
			}
		}
		else{
			for (auto itrStru=vList.begin(); itrStru!=vList.end(); ++itrStru){
				sStructuresetUid = (*itrStru)->get_uid();
				DEL_PTR(*itrStru);
			}
		}
		voi->set_structuresetuid(sStructuresetUid);
		if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertVoi(*voi)){
			TPS_LOG_DEV_ERROR<<"Insert Voi failed!";
			return TPS_ER_DB_ERROR;
		}
	}

	TEST_PERFORMANCE_END("TpsSaveObjects::SaveVoiObject");
	TPS_LOG_DEV_INFO<<"[INFO][TpsSaveObjects][SaveVoiObject] leave!";
	return TPS_ER_SUCCESS;
}

int TpsSaveObjects::SaveRoi(ROIEntity& roiEntity)
{
    //TODO: 只保存dirty的contours以提升性能-GML
    TEST_PERFORMANCE_INIT;
    TEST_PERFORMANCE_BEGIN;
    TPS_LOG_DEV_INFO<<"[INFO][TpsSaveObjects][SaveRoi] enter";

    auto innerRoi = roiEntity.GetInnerRoi();
    TpsImage3DEntity* pImage3D = nullptr;
    bool bRet = mDataRepository->GetImageDataManager()->GetImage3DData(
        innerRoi->get_seriesuid(), &pImage3D);
    if (!bRet || nullptr == pImage3D)
    {
        TPS_LOG_DEV_ERROR<<"GetImage3DData failed or pImage3D is null.";
        return TPS_ER_FAILURE;
    }

    std::vector<RtContour*> vtContour;
    SubVolumeHelperAPI::UpdateRtContours(pImage3D, roiEntity);
    innerRoi->set_softwareversionno(mDatabaseWrapper->GetTpsVersion());

    if (nullptr != mDatabaseWrapper) 
    {
        if (mDatabaseWrapper->QueryDBInTable(innerRoi->get_uid(), RtDbDef::TYPE_VOI)) 
        {
            // 如果记录已经存在
            if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateVoiObject(*innerRoi))
            {
                TPS_LOG_DEV_ERROR<<"UpdateVoiObject failed!";
                return TPS_ER_DB_ERROR;
            }
        }
        else 
        {
            // 如果记录尚未存在
            const std::string seriesuid = innerRoi->get_seriesuid();
            std::vector<RtStructureset*> vList;
            std::string sStructuresetUid("");
            mDatabaseWrapper->GetStructuresetListBySeriesUid(seriesuid, vList);
            if (vList.empty())
            {
                RtStructureset structureset(true);
                structureset.set_seriesuid(seriesuid);
                sStructuresetUid = structureset.get_uid();
                if(DB_CODES_SUCCESS != mDatabaseWrapper->InsertStructureset(structureset))
                {
                    TPS_LOG_DEV_ERROR<<"Insert Structureset failed!";
                    return TPS_ER_DB_ERROR;
                }
            }
            else
            {
                for (auto itrStru=vList.begin(); itrStru!=vList.end(); ++itrStru)
                {
                    sStructuresetUid = (*itrStru)->get_uid();
                    DEL_PTR(*itrStru);
                }
            }
            innerRoi->set_structuresetuid(sStructuresetUid);
            if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertVoi(*innerRoi))
            {
                TPS_LOG_DEV_ERROR<<"Insert Voi failed!";
                return TPS_ER_DB_ERROR;
            }
        }
    }
    TEST_PERFORMANCE_END("TpsSaveObjects::SaveRoi");
    TPS_LOG_DEV_INFO<<"[INFO][TpsSaveObjects][SaveRoi] leave!";
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::UpdatePatient(RtPatient* pPatient)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdatePatientObject(pPatient)){
        TPS_LOG_DEV_ERROR<<"failed to UpdatePatientObject";
        return TPS_ER_DB_ERROR;
    }
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::UpdateSeries(RtSeries& series)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateSeriesObject(series)){
        TPS_LOG_DEV_ERROR<<"failed to UpdateSeriesObject";
        return TPS_ER_DB_ERROR;
    }
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::UpdatePoi(RtPoi* pPoi)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdatePoiObject(pPoi)){
        TPS_LOG_DEV_ERROR<<"failed to UpdatePoiObject";
        return TPS_ER_DB_ERROR;
    }
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::UpdateBeamPoiDose(RtPoi* pPoi){
    std::string poiUid = pPoi->get_uid();
    std::string beamUid = "";
    float doseContribution = 0.0f;
    std::map<std::string, float> poiDose = pPoi->get_poidosemap();
    for (std::map<std::string, float>::iterator itPoiDose = poiDose.begin(); itPoiDose != poiDose.end(); ++itPoiDose){
        beamUid = itPoiDose->first;
        doseContribution = itPoiDose->second;
        RtBeam_poi beampoi;
        beampoi.set_beamuid(beamUid);
        beampoi.set_dosecontribution(doseContribution);
        beampoi.set_poiuid(poiUid);
        RtBeam_poi* savedBeamPoi = nullptr;
        mDatabaseWrapper->GetPoiDose(poiUid, beamUid, savedBeamPoi);
        if (savedBeamPoi == nullptr){
            if (mDatabaseWrapper->QueryDBInTable(beamUid, RtDbDef::TYPE_BEAM)){// the beam should exist
                if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertPoiDose(beampoi)){
                    TPS_LOG_DEV_ERROR<<"failed to insert poi dose, beam uid:" << beamUid << ", poi uid:" << poiUid;
                    return TPS_ER_DB_ERROR;
                }
            }
        }
        else{
            if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdatePoiDose(beampoi)){
                TPS_LOG_DEV_ERROR<<"failed to update poi dose, beam uid:" << beamUid << ", poi uid:" << poiUid;
                return TPS_ER_DB_ERROR;
            }
        }
    }
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::UpdateVoiAttributes(const ROIEntity& voiEntity)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateVoiAttributes(*voiEntity.GetInnerRoi())){
        TPS_LOG_DEV_ERROR<<"failed to UpdateVoiAttributes";
        return TPS_ER_DB_ERROR;
    }
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::UpdateBeamSegments(std::vector<RtBeamsegment*> beamSegments)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateBeamSegments(beamSegments))
    {
        TPS_LOG_DEV_ERROR << "failed to updateBeamSegments";
        return TPS_ER_DB_ERROR;
    }
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::EraseObject(const std::string& sUid, RtDbDef::OBJECT_TYPE objType) const
{
    return mDatabaseWrapper->EraseObject(sUid, objType);
}

int TpsSaveObjects::SaveRtImage(RtRtimage& rtImage)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;
    return mDatabaseWrapper->InsertRtImageObject(rtImage);
}

int TpsSaveObjects::SavePatient(RtPatient* pPatient)
{
	if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;
	if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertPatient(*pPatient)){
		TPS_LOG_DEV_ERROR<<"failed to Insert Patient";
		return TPS_ER_DB_ERROR;
	}
	return TPS_ER_SUCCESS;
}

int TpsSaveObjects::SaveSeries(RtSeries& series)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertSeriesObject(series)){
        TPS_LOG_DEV_ERROR<<"failed to Insert Series";
        return TPS_ER_DB_ERROR;
    }
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::SaveImage(RtImage& image)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertImageObject(image))
    {
        TPS_LOG_DEV_ERROR<<"failed to InsertImageObject";
        return TPS_ER_DB_ERROR;
    }
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::SaveApproveRecord(RtApproverecord& approveRecord)
{
     if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;

     if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertApproveRecord(approveRecord))
     {
         TPS_LOG_DEV_ERROR << "failed to InsertApproveRecord";
         return TPS_ER_DB_ERROR;
     }

     return TPS_ER_SUCCESS;
}

int TpsSaveObjects::DeleteApproveRecordViaObjUid(const std::string& objectUid)
{
    if (objectUid.empty())
    {
        return TPS_ER_FAILURE;
    }

    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;

    std::string sCondition = "objectuid = '" + objectUid + "'";

    int retCode = mDatabaseWrapper->EraseObjectUnderCondition(sCondition, RtDbDef::TYPE_APPROVERECORD);
    if (TPS_ER_SUCCESS != retCode)
    {
        TPS_LOG_DEV_ERROR << "Failed to delete approve record from DB.";
        return retCode;
    }

    return TPS_ER_SUCCESS;
}

std::string TpsSaveObjects::GetSavePathForImage()
{
    if (nullptr == mDatabaseWrapper) return "";
    std::string sPath = mDatabaseWrapper->GetSavePath() + "/image";
    if (!mDatabaseWrapper->CreatePath(sPath)){
        return "";
    }
    return sPath;
}

bool TpsSaveObjects::CreatePath(const std::string& sPath)
{
    if (nullptr == mDatabaseWrapper) return false;
    return mDatabaseWrapper->CreatePath(sPath);
}

int TpsSaveObjects::SaveDoselines(const std::string& sPlanUid, const std::vector<RtDoseline*>& vdoselines)
{
    std::vector<RtDoseline*> oldDoseLineList;
    mDataRepository->GetDoseManager()->GetDoseLineList(sPlanUid, oldDoseLineList);

    for (auto itDelDoseLine = oldDoseLineList.begin(); itDelDoseLine != oldDoseLineList.end(); ++itDelDoseLine) {
        if(DB_CODES_SUCCESS != mDatabaseWrapper->EraseObject((*itDelDoseLine)->get_uid(), RtDbDef::TYPE_DOSELINE)){
            TPS_LOG_DEV_ERROR<<"Failed to delete dose line to database.";
            return TPS_ER_DB_ERROR;
        }
    }
    for (auto itDoseLine = vdoselines.begin(); itDoseLine != vdoselines.end(); ++itDoseLine) {
        if(DB_CODES_SUCCESS != mDatabaseWrapper->InsertDoselineObject(*itDoseLine)){
            TPS_LOG_DEV_ERROR<<"Fail to save doseLine into db";
            return TPS_ER_DB_ERROR;
        }
    }
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::EraseObject(const std::string& sFirstUid,const std::string& sSecondUid, 
    RtDbDef::OBJECT_TYPE objType) const
{
    if(DB_CODES_SUCCESS != mDatabaseWrapper->EraseObject(sFirstUid, sSecondUid, objType)){
        TPS_LOG_DEV_ERROR<<"EraseObject failed with first uid:"<<sFirstUid
            <<" second uid: "<<sSecondUid<<"OBJECT TYPE: "
            <<RtDbDef::ENUM_TO_STRING_OBJECT_TYPE[objType];
        return TPS_ER_DB_ERROR;
    }
    else{
        return TPS_ER_SUCCESS;
    }
}

int TpsSaveObjects::SaveSeriesTable(const SeriesTable& seriesTable)
{
    auto innerTable = seriesTable.GetInnerTable();
    if (innerTable == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The inner series table is empty.";
        return TPS_ER_FAILURE;
    }
    auto rtTblUid = innerTable->get_tableuid();
    bool hasRtTable = mDataRepository->GetTableManager()->GetRtTable(rtTblUid) != nullptr;

    bool seriesTableExist = mDatabaseWrapper->QueryDBInTableViaSpecifiedKey("seriesuid", 
        innerTable->get_seriesuid(), RtDbDef::TYPE_SERIESTABLE);
    innerTable->setdirty_all_fields(true);

    if (seriesTableExist)
    {
        if (hasRtTable)
        {
            if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateSeriestable(*innerTable))
            {
                TPS_LOG_DEV_ERROR<<"Failed to update series table.";
                return TPS_ER_DB_ERROR;
            }
        }
        else
        {
            return this->EraseObject(innerTable->get_seriesuid(), RtDbDef::TYPE_SERIESTABLE);
        }
    }
    else if (hasRtTable)
    {
        if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertSeriestable(*innerTable)){
            TPS_LOG_DEV_ERROR<<"Failed to insert series table.";
            return TPS_ER_DB_ERROR;
        }
    }
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::SaveNormVoi(RtNorm_voi& normVoi)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;

    if (mDatabaseWrapper->QueryDBInTable(normVoi.get_normgroupuid(), normVoi.get_voiuid(), RtDbDef::TYPE_NORM_VOI))
    {
        int iRet = DeleteNormVoiViaNormalUid(normVoi.get_normgroupuid());
        if (DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR << "Failed to Delete normvoi.";
            return TPS_ER_DB_ERROR;
        }
    }

    if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertNormgroupVoiDose(normVoi))
    {
        TPS_LOG_DEV_ERROR << "failed to InsertNormgroupVoiDose: " << normVoi.get_normgroupuid();
        return TPS_ER_DB_ERROR;
    }

    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::DeleteNormVoiViaNormalUid(const std::string& ngUid)
{
    if (ngUid.empty())
    {
        return TPS_ER_FAILURE;
    }

    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;

    std::string sCondition = "normgroupuid = '" + ngUid + "'";

    int retCode = mDatabaseWrapper->EraseObjectUnderCondition(sCondition, RtDbDef::TYPE_NORM_VOI);
    if (TPS_ER_SUCCESS != retCode)
    {
        TPS_LOG_DEV_ERROR << "Failed to delete normvoi from DB.";
        return retCode;
    }

    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::DeletePlanVoiViaPlanUid(const std::string& planUid)
{
    if (planUid.empty())
    {
        return TPS_ER_FAILURE;
    }

    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;

    std::string sCondition = "planuid = '" + planUid + "'";

    int retCode = mDatabaseWrapper->EraseObjectUnderCondition(sCondition, RtDbDef::TYPE_PLAN_VOI);
    if (TPS_ER_SUCCESS != retCode)
    {
        TPS_LOG_DEV_ERROR << "Failed to delete planvoi from DB.";
        return retCode;
    }

    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::SavePlanVoi(RtPlan_voi& planVoi)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;

    if (mDatabaseWrapper->QueryDBInTable(planVoi.get_planuid(), planVoi.get_voiuid(), RtDbDef::TYPE_PLAN_VOI))
    {
        int iRet = DeletePlanVoiViaPlanUid(planVoi.get_planuid());
        if (DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR << "Failed to Delete planvoi.";
            return TPS_ER_DB_ERROR;
        }
    }


    if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertPlanVoiDose(planVoi))
    {
        TPS_LOG_DEV_ERROR << "failed to InsertPlanVoiDose " << planVoi.get_planuid();
        return TPS_ER_DB_ERROR;
    }

    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::SaveTableContours(const std::vector<RtTablecontour*>& tableContous)
{
    int iRet;
    for (int i = 0; i < tableContous.size(); ++i)
    {
        iRet = mDatabaseWrapper->InsertTablecontour(*tableContous[i]);
        if (DB_CODES_SUCCESS != iRet)
        {
            TPS_LOG_DEV_ERROR << "Failed to InsertTablecontour!";
            return iRet;
        }
    }
    return TPS_ER_SUCCESS;
}


int TpsSaveObjects::SaveTemplate(RtTemplate* sTemplate)
{
    if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertTemplate(*sTemplate))
    {
        TPS_LOG_DEV_ERROR << "failed to InsertWindowingTemplate " << sTemplate->get_uid();
        return TPS_ER_DB_ERROR;
    }

    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::UpdateTemplate(RtTemplate* sTemplate)
{
    if (nullptr == mDatabaseWrapper) return TPS_ER_FAILURE;
    if (DB_CODES_SUCCESS != mDatabaseWrapper->UpdateTemplate(*sTemplate))
    {
        TPS_LOG_DEV_ERROR << "failed to updateTemplate";
        return TPS_ER_DB_ERROR;
    }
    return TPS_ER_SUCCESS;
}

int TpsSaveObjects::RemoveTemplate(std::string& templateUid)
{
    return this->EraseObject(templateUid, RtDbDef::TYPE_TEMPLATE);
}




TPS_END_NAMESPACE
