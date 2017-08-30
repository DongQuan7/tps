////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_dose_mesh_goconverter.h
/// 
///  \brief class DoseMeshGraphicObjectConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/06/08
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_dose_mesh_gocollectionconverter.h"

#include "McsfGeometry/mcsf_geometry_sub_volume.h"
//#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
//#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsAlgorithmProxy/tps_pl_utility.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_mask.h"


#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobject.h"              // for DoseMeshGraphicObject

#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsRenderLibrary/tps_rl_smoothmesh_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobjectcollection.h"
#include <algorithm>

#include "RtTpsDataAccess/tps_da_patientmanager.h"                      // for TpsPatientManager
#include "RtTpsDataAccess/tps_da_dosemanager.h"                         // for TpsDoseManager
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

//#include "RtTpsAlgorithmProxy/tps_ap_voi_helper.h"
#include "RtTpsAlgorithmProxy/tps_ap_dose_mesh_helper.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_doseline.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE
DoseMeshGraphicObjectCollectionConverter::DoseMeshGraphicObjectCollectionConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType), mVolume(nullptr), mPlan(nullptr) {

}

DoseMeshGraphicObjectCollectionConverter::~DoseMeshGraphicObjectCollectionConverter() {

}

GraphicObjectPtr DoseMeshGraphicObjectCollectionConverter::CreateGraphicObject_i(){
    return std::shared_ptr<DoseMeshGraphicObjectCollection>(new DoseMeshGraphicObjectCollection());
}

//uid is plan uid
bool DoseMeshGraphicObjectCollectionConverter::PrepareSourceData_i(const GOConverterParameter& para){

    auto it = para.data_source_map.find(PLAN);
    if (it == para.data_source_map.end()) return false;
    std::string uid = it->second;

    mNormgroupList.clear();
    if (!mDataRepository->GetPatientManager()->GetPlan(uid, &mPlan)){
        TPS_LOG_DEV_ERROR<<"Failed to get plan: "<<uid;
        return false;
    }

    mPlanUID = uid;
    mImageUID = mPlan->get_imageseriesuid();

    auto goVolume = mModelWarehouse->GetModelObject(
        mImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME));
    mVolume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(goVolume);
    if(mVolume == nullptr) {
        TPS_LOG_DEV_ERROR<<"volume go is nullptr!"<<uid;
        return false;
    }

    mNormgroupList = mDataRepository->GetNormgroupManager()->GetNormalGroupList(uid);
    return true;
}


bool DoseMeshGraphicObjectCollectionConverter::Update_i(GraphicObjectPtr spGo){

    std::shared_ptr<DoseMeshGraphicObjectCollection> spDoseMeshGoCollection = std::dynamic_pointer_cast<DoseMeshGraphicObjectCollection>(spGo);
    if (mVolume == nullptr || spDoseMeshGoCollection == nullptr){
        return false;
    }

    auto doseMeshGoCollection = spDoseMeshGoCollection->GetAllDoseMeshGo();

    doseMeshGoCollection.clear();

    TpsImage3DEntity* pImage3D = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(mImageUID, &pImage3D)
        || pImage3D == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get image3dentity from manager with ImageUID:" << mImageUID;
        return false;
    }

    RtPlan* pPlan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(mPlanUID, &pPlan)
        || pPlan == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get plan from manager with ImageUID:" << mPlanUID;
        return false;
    }

    TpsDoseManager* pDoseManager = mDataRepository->GetDoseManager();
    if (pDoseManager == nullptr) {
        TPS_LOG_DEV_ERROR << "DoseMeshGraphicObjectConverter::Update_i(): Failed to find TpsDoseManager.";
        return false;
    }

    float totalGoal = 0.f;
    for (int i = 0; i < mNormgroupList.size(); ++i)
    {
        if (mNormgroupList[i] == nullptr || !mNormgroupList[i]->get_isjoined()) continue;
        totalGoal += mNormgroupList[i]->get_fraction() * 
            mNormgroupList[i]->get_doseperfraction();
    }
    float specfiedDose = mPlan->get_specifieddose();
    //if specified dose is invalid, follow the total goal value
    if (fabs(specfiedDose - INVALID_SPECIFIED_DOSE_VALUE) <= 1e-6)
    {
        specfiedDose = totalGoal;
    }

    float fDose100 = 0.0;
    RtDbDef::HundredPercentDefinition_TYPE nDefinition100 = (RtDbDef::HundredPercentDefinition_TYPE)mPlan->get_hundredpercentdefinition();
    if (nDefinition100 == RtDbDef::TotalGoals)    // TotalGoals = 0, MaxDose = 1, SpecifiedDose = 2
    {
        fDose100 = totalGoal;
    }
    else if (nDefinition100 == RtDbDef::MaxDose)
    {
        float fX = 0.0, fY = 0.0, fZ = 0.0;
        int nResult = pDoseManager->GetMaxDosePointOfPlan(mPlanUID, fX, fY, fZ, fDose100);
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

    std::vector<RtDoseline*> vecDoseLinePtr;
    bool bResult = pDoseManager->GetDoseLineList(mPlanUID, vecDoseLinePtr);
    if (bResult == false)
    {
        TPS_LOG_DEV_ERROR << "DoseMeshGraphicObjectConverter::Update_i(): Failed to find DoseLines.";
        return true;
    }

    if(vecDoseLinePtr.size() == 0)
    {
        TPS_LOG_DEV_ERROR << "DoseMeshGraphicObjectConverter::Update_i(): No DoseLine exists.";
        return true;
    }
    std::string strKey = mImageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(strKey);
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spVolumeGO = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(goVolume);
    if (spVolumeGO == nullptr){
        TPS_LOG_DEV_ERROR<<"[ERROR][DoseUpdateMeshOperation][ModifyGraphicObject]: Failed to find VolumeGO.";
        return false;
    }
    Mcsf::MedViewer3D::Matrix4x4 matPat2World = spVolumeGO->GetWorld2Patient().Inverse();
    RtDoseline* pDoseLine = nullptr;
    for (int i = 0; i < vecDoseLinePtr.size(); i++)
    {
        pDoseLine = vecDoseLinePtr[i];
        bool bVisible = pDoseLine->get_isvisible3d(); //get_isvisible();
        if (!bVisible)
        {
            continue;
        }
        float fDoseRatio = pDoseLine->get_doseratio();
        if (fDoseRatio < 0)
        {
            continue;
        }
        float fDose = fDoseRatio * fDose100;

        STC_STRIPS* pMesh = new STC_STRIPS();
        plunc::ANASTRUCT* pAna = new plunc::ANASTRUCT();
        Mcsf::Matrix4f matPat2World_4f = TpsArithmeticConverter::ConvertToMatrix4f(matPat2World);
        matPat2World_4f.SetIdentity();  // make strips in patient CS.
        int nResult = TpsDoseMeshHelper::GenerateMeshFromDose(pImage3D, pPlan->get_dosegrid(), fDose, matPat2World_4f, pMesh, pAna);
        if (nResult == -1)
        {
            TPS_LOG_DEV_INFO << "DoseUpdateMeshOperation: Failed to GenerateMeshFromDose.";
            return true;
        }

        float fRed = vecDoseLinePtr[i]->get_red();      // r
        float fGreen = vecDoseLinePtr[i]->get_green();    // g
        float fBlue = vecDoseLinePtr[i]->get_blue();     // b
		float trans3D = mDataRepository->GetDisplayStateManager()->GetDoselineTransparency3D();
		float fAlpha = 1.0f - trans3D;
        // Update GO
        auto spDoseMeshGo = std::shared_ptr<DoseMeshGraphicObject>(new DoseMeshGraphicObject());
        spDoseMeshGo->SetDoseMesh(pMesh);
        spDoseMeshGo->SetDoseAnastruct(pAna);
        spDoseMeshGo->SetDoseColor(fRed, fGreen, fBlue, fAlpha);
        spDoseMeshGo->SetDoseRatio(fDoseRatio);

        //spDoseMeshGo->SetDirty(true);
        // Enable dose mesh after correctly update
        spDoseMeshGo->SetValidFlag(true);
        doseMeshGoCollection.push_back(spDoseMeshGo);
    }
    spDoseMeshGoCollection->SetAllDoseMeshGo(doseMeshGoCollection);
    spDoseMeshGoCollection->SetDirty(true);

    return true;
}

 TPS_END_NAMESPACE
