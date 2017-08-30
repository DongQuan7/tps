//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  LI Song song.li@united-imaging.com
///
///  \file    tps_rl_dose_update_mesh_operation.h
///  \brief   DoseUpdateMeshOperation implementation
///
///  \version 1.0
///  \date    Jun. 10, 2015
///  
////////////////////////////////////////////////////////////////////////// 

#include "StdAfx.h"

// TPS
#include "RtTpsRenderLibrary/tps_rl_dose_update_mesh_operation.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobject.h"          // for DoseMeshGraphicObject
#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobjectcollection.h"

#include "RtTpsDataAccess/tps_da_patientmanager.h"                      // for TpsPatientManager
#include "RtTpsDataAccess/tps_da_dosemanager.h"                         // for TpsDoseManager
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"

#include "McsfGeometry/mcsf_geometry_sub_volume.h"
//#include "RtTpsAlgorithmProxy/tps_ap_voi_helper.h"
#include "RtTpsAlgorithmProxy/tps_ap_dose_mesh_helper.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_doseline.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsFramework/tps_fw_image3dentity.h"
//#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

#include "tps_logger.h"
// ZHENGHE

#include "McsfViewer3DModel/mcsf_viewer3d_model.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

DoseUpdateMeshOperation::DoseUpdateMeshOperation(const std::string& strPlanUid, const bool& doseGridChanged)
    :m_strPlanUid(strPlanUid), m_doseGridChanged(doseGridChanged){
}

DoseUpdateMeshOperation::~DoseUpdateMeshOperation(){

}

bool DoseUpdateMeshOperation::ModifyGraphicObject() const 
{
    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(m_strPlanUid, &plan) ||
        plan==nullptr) {
        TPS_LOG_DEV_ERROR<<"Failed to get plan from patient manager";
        return false;
    }

    TpsImage3DEntity* pImage3D = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(plan->get_imageseriesuid(), &pImage3D)
        || pImage3D == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get image3dentity from manager with ImageUID:" << plan->get_imageseriesuid();
        return false;
    }

    std::string strKey = m_strPlanUid + "|" + GOTypeHelper::ToString(GO_TYPE_DOSE_MESH_COLLECTION);
    auto go = mModelWarehouse->GetModelObject(strKey);
    std::shared_ptr<DoseMeshGraphicObjectCollection> spDoseMeshGoCollection = std::dynamic_pointer_cast<DoseMeshGraphicObjectCollection>(go);
    if (spDoseMeshGoCollection == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][DoseUpdateMeshOperation][ModifyGraphicObject]: Failed to find DoseMeshGO.";
        return false;
    }

    if(m_doseGridChanged)
    {
        spDoseMeshGoCollection->ClearAllDoseMeshGo();
    }

    TpsDoseManager* pDoseManager = mDataRepository->GetDoseManager();
    if (pDoseManager == nullptr) {
        TPS_LOG_DEV_ERROR << "DoseMeshGraphicObjectConverter::Update_i(): Failed to find TpsDoseManager.";
        return false;
    }

    std::string imgaeUid = plan->get_imageseriesuid();

    std::vector<RtDoseline*> vecDoseLinePtr;
    bool bResult = pDoseManager->GetDoseLineList(m_strPlanUid, vecDoseLinePtr);
    if (bResult == false)
    {
        TPS_LOG_DEV_ERROR << "DoseMeshGraphicObjectConverter::Update_i(): Failed to find DoseLines.";
        return true;
    }

    if(vecDoseLinePtr.size() == 0)
    {
		spDoseMeshGoCollection->ClearAllDoseMeshGo();
		spDoseMeshGoCollection->SetDirty(true);
        TPS_LOG_DEV_WARNING << "DoseMeshGraphicObjectConverter::Update_i(): No DoseLine exists.";
        return true;
    }

    strKey = imgaeUid + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(strKey);
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spVolumeGO = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(goVolume);
    if (spVolumeGO == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][DoseUpdateMeshOperation][ModifyGraphicObject]: Failed to find VolumeGO.";
        return false;
    }

    auto ngList = mDataRepository->GetNormgroupManager()->GetNormalGroupList(m_strPlanUid);
    float totalGoal = 0.f;
    for (int i = 0; i < ngList.size(); ++i)
    {
        if (ngList[i] == nullptr || !ngList[i]->get_isjoined()) continue;
        totalGoal += ngList[i]->get_fraction() * ngList[i]->get_doseperfraction();
    }
    float specfiedDose = plan->get_specifieddose();
    //if specified dose is invalid, follow the total goal value
    if (fabs(specfiedDose - INVALID_SPECIFIED_DOSE_VALUE) <= 1e-6)
    {
        specfiedDose = totalGoal;
    }

	float fDose100 = 0.0;
	RtDbDef::HundredPercentDefinition_TYPE nDefinition100 = (RtDbDef::HundredPercentDefinition_TYPE)plan->get_hundredpercentdefinition();
	if (nDefinition100 == RtDbDef::TotalGoals)    // TotalGoals = 0, MaxDose = 1, SpecifiedDose = 2
	{
		fDose100 = totalGoal;
	}
	else if (nDefinition100 == RtDbDef::MaxDose)
	{
		float fX = 0.0, fY = 0.0, fZ = 0.0;
		int nResult = pDoseManager->GetMaxDosePointOfPlan(m_strPlanUid, fX, fY, fZ, fDose100);
		if (nResult == -1)
		{
			TPS_LOG_DEV_ERROR << "DoseMeshGraphicObjectConverter::Update_i(): Failed to find MaxDose.";
			return true;
		}
	}
	else if(nDefinition100 == RtDbDef::SpecifiedDose)
	{
		fDose100 = specfiedDose;
	}
    Mcsf::MedViewer3D::Matrix4x4 matPat2World = spVolumeGO->GetWorld2Patient().Inverse();
    RtDoseline* pDoseLine = nullptr;
    float fDose = 0.0;
    auto doseMeshGoCollection = spDoseMeshGoCollection->GetAllDoseMeshGo();

	//先将doselinecollection中没有而doseMeshGoCollection中有的go删除
	float dosemeshRatio = 0.0;
	for (auto iter = doseMeshGoCollection.begin(); iter != doseMeshGoCollection.end();)
	{
		dosemeshRatio = (*iter)->GetDoseRatio();
		bool inDoseLine = false;
		for (int j = 0; j < vecDoseLinePtr.size(); j++)
		{
			if(abs(vecDoseLinePtr[j]->get_doseratio() - dosemeshRatio) < 1e-6)
			{
				inDoseLine = true;
				break;
			}
		}
		if(!inDoseLine)
		{
			iter = doseMeshGoCollection.erase(iter);
		}
		else
		{
			++iter;
		}
	}

    for (int i = 0; i < vecDoseLinePtr.size(); i++)
    {
        pDoseLine = vecDoseLinePtr[i];

        float fDoseRatio = pDoseLine->get_doseratio();
        if (fDoseRatio < 0)
        {
            continue;
        }
		fDose = fDose100 * fDoseRatio;
        bool bVisible = pDoseLine->get_isvisible3d();

        auto doseMeshGo = GetDoseMeshGoByRatio(fDoseRatio, spDoseMeshGoCollection);

        STC_STRIPS* pMesh = new STC_STRIPS();
        plunc::ANASTRUCT* pAna = new plunc::ANASTRUCT();
        Mcsf::Matrix4f matPat2World_4f = TpsArithmeticConverter::ConvertToMatrix4f(matPat2World);
        matPat2World_4f.SetIdentity();  // make strips in patient CS.

        if(doseMeshGo == nullptr && !bVisible) continue;

        if(doseMeshGo == nullptr && bVisible)
        {
            int nResult = TpsDoseMeshHelper::GenerateMeshFromDose(pImage3D, plan->get_dosegrid(), fDose, matPat2World_4f, pMesh, pAna);
            if (nResult == -1)
            {
                TPS_LOG_DEV_INFO << "DoseUpdateMeshOperation: Failed to GenerateMeshFromDose.";
                continue;
            }
            // Update GO
			doseMeshGo = std::shared_ptr<DoseMeshGraphicObject>(new DoseMeshGraphicObject());
			doseMeshGo->SetValidFlag(true);
			doseMeshGo->SetDoseMesh(pMesh);
			doseMeshGo->SetDoseAnastruct(pAna);
			doseMeshGo->SetDoseRatio(fDoseRatio);
            doseMeshGoCollection.push_back(doseMeshGo);
        }

        if(doseMeshGo != nullptr)
        {
            doseMeshGo->SetValidFlag(bVisible);
        }

        float fRed = vecDoseLinePtr[i]->get_red();      // r
        float fGreen = vecDoseLinePtr[i]->get_green();    // g
        float fBlue = vecDoseLinePtr[i]->get_blue();     // b
        float fAlpha = 1.0f - mDataRepository->GetDisplayStateManager()->GetDoselineTransparency3D();
		
		doseMeshGo->SetDoseColor(fRed, fGreen, fBlue, fAlpha);

    }
    spDoseMeshGoCollection->SetAllDoseMeshGo(doseMeshGoCollection);
    spDoseMeshGoCollection->SetDirty(true);
    return true;
}

TpsDoseMeshGraphicObjectPtr DoseUpdateMeshOperation::GetDoseMeshGoByRatio(const float& ratio, TpsDoseMeshGraphicObjectCollectionPtr doseMeshCollectionPtr) const
{
    auto doseMeshGoCollection = doseMeshCollectionPtr->GetAllDoseMeshGo();
    for (int i = 0; i < doseMeshGoCollection.size(); ++i)
    {
        auto doseMeshGo = doseMeshGoCollection[i];
        if(abs(ratio - doseMeshGo->GetDoseRatio()) < 1e-6)
        {
            return doseMeshGo;
        }
    }
    return nullptr;
}

TPS_END_NAMESPACE  // end namespace tps
