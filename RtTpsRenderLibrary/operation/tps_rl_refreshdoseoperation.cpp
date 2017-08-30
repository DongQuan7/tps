////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_refreshdoseoperation.cpp
/// 
///  \brief class TpsRefreshDoseOperation implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/01/08
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_refreshdoseoperation.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
//#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsRenderLibrary/tps_rl_mprdosegraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_doselineobject.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_configuremanager.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

const GO_TYPE aDoseMPRKey[5][2] = {
    { GO_TYPE_DOSE_CORONAL,      GO_TYPE_MPR_CORONAL },
    { GO_TYPE_DOSE_SAGITTAL,     GO_TYPE_MPR_SAGITTAL},
    { GO_TYPE_DOSE_AXIAL,        GO_TYPE_MPR_AXIAL},
    { GO_TYPE_DOSE_AXIAL_PREV,   GO_TYPE_MPR_AXIAL_PREV},
    { GO_TYPE_DOSE_AXIAL_POST,   GO_TYPE_MPR_AXIAL_POST},
};

TpsRefreshDoseOperation::TpsRefreshDoseOperation(
    const std::string& planUID, FIRST_POSTFIX_COMPONENT sectionType) : 
    mPlanUID(planUID), mSectionType(sectionType){
}

TpsRefreshDoseOperation::~TpsRefreshDoseOperation(){

}

bool TpsRefreshDoseOperation::ModifyGraphicObject() const{
    using namespace Mcsf::MedViewer3D;

    std::string doseGOKey, mprGOKey;
    //get image 3d data uid first, get volume graphic object
    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(mPlanUID, &plan) || plan == nullptr){
        TPS_LOG_DEV_ERROR<<"The Plan does not exist, uid: "<<mPlanUID;
        return false;
    }
    //RTFWK::PLANSOURCE_TYPE type = plan->GetPlanSourceType();
    std::string goType = GOTypeHelper::ToString(GO_TYPE_VOLUME);
    std::string imageUID = plan->get_imageseriesuid();
    auto go = mModelWarehouse->GetModelObject(imageUID + "|" + goType);
    auto volumeGO = std::dynamic_pointer_cast<GraphicObjVolume>(go);
    if (go == nullptr || volumeGO == nullptr){
        TPS_LOG_DEV_ERROR<<"Volume graphic object of image 3d: "<<imageUID<<" does not exist.";
        return false;
    }
    Matrix4x4 matW2P = volumeGO->GetModel2PatientMat();
    matW2P.Append(volumeGO->GetModel2World().Inverse());
    
    RtDosegrid* doseGrid = nullptr;
    //old requirement
    //DOSE_CALCULATION_LEVEL doseCalcLevel = DisplayStateManager::GetInstance()->GetDoseCalcLevel();
    //if(type == RTFWK::QAPLAN) {
    //    doseCalcLevel = CALCULATION_LEVEL_PLAN;
    //}
    //if (CALCULATION_LEVEL_NORMGROUP == doseCalcLevel){
    //    std::string normgroupUid = DisplayStateManager::GetInstance()->GetDoseCalcUid();
    //    //if(type == RTFWK::QAPLAN) {
    //    //    std::vector<std::string> normGroupList = mDataRepository->GetNormgroupManager()->GetNormGroupListByPlan(mPlanUID);
    //    //    if(normGroupList.size() >= 1) {
    //    //        normgroupUid = normGroupList[0];
    //    //    }
    //    //    else {
    //    //        normgroupUid = "";
    //    //    }
    //    //}
    //    doseGrid = mDataRepository->GetDoseManager()->GetDoseGrid(normgroupUid, DOSE_GRID_NORMGROUP);
    //}
    //else if (CALCULATION_LEVEL_BEAM == doseCalcLevel){
    //    std::string beamUid = DisplayStateManager::GetInstance()->GetDoseCalcUid();
    //    doseGrid = mDataRepository->GetDoseManager()->GetDoseGrid(beamUid, DOSE_GRID_BEAM);
    //}
    //else{
    //    //get dose grid
    //    doseGrid = &(plan->GetDoseGrid());
    //    if (doseGrid == nullptr){
    //        TPS_LOG_DEV_ERROR<<"Failed to get dose grid of plan: "<<mPlanUID;
    //        return false;
    //    }
    //}
    // rcc: 暂时注掉，等替换dosegrid数据结构的时候再打开
    doseGrid = plan->get_dosegrid();
    if (doseGrid == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get dose grid of plan: "<<mPlanUID;
        return false;
    }

    Mcsf::Matrix4f matG2P = TpsArithmeticConverter::ConvertToMatrix4f(doseGrid->get_grid_to_pat_t());
    Mcsf::Matrix4f matW2G = matG2P.Inverse();
    matW2G.Append(TpsArithmeticConverter::ConvertToMatrix4f(matW2P));
    unsigned int dim[3] = { doseGrid->get_xcount(), doseGrid->get_ycount(),
        doseGrid->get_zcount() };

    std::shared_ptr<TpsMPRDoseGraphicObject> doseGO = nullptr;
    std::shared_ptr<MPRGraphicObject> mprGO = nullptr;
    Point3D aVertices[4];
    Mcsf::Point3f aVerticesG[4];

    int start = 0, end = 5;
    switch(mSectionType)
    {
    case CORONAL:
        start = 0;
        end = 1;
        break;
    case SAGITTAL:
        start = 1;
        end = 2;
        break;
    case AXIAL:
        start = 2;
        end = 5;
    }
    for (int i = start; i < end; ++i){
        doseGOKey = mPlanUID + "|" + GOTypeHelper::ToString(aDoseMPRKey[i][0]);
        mprGOKey = imageUID + "|" + GOTypeHelper::ToString(aDoseMPRKey[i][1]);

        go = mModelWarehouse->GetModelObject(doseGOKey);
        doseGO = std::dynamic_pointer_cast<TpsMPRDoseGraphicObject>(go);
        if (go == nullptr || doseGO == nullptr){
            TPS_LOG_DEV_WARNING<<"Dose graphic object with key: "<<doseGOKey<<" does not exist.";
            continue;
        }

        go = mModelWarehouse->GetModelObject(mprGOKey);
        mprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
        if (go == nullptr || mprGO == nullptr){
            TPS_LOG_DEV_WARNING<<"MPR graphic object with key: "<<mprGOKey<<" does not exist.";
            continue;
        }

        //get plane, transform to dose grid c.s.
        mprGO->GetVertices(aVertices);
        for (int j = 0; j < 4; ++j){
            aVerticesG[j] = matW2G.Transform(
                TpsArithmeticConverter::ConvertToPoint3f(aVertices[j]));
        }
        Mcsf::Point3f miniVerticesG[4];
        Mcsf::Matrix4f matDoseGridToNormalPlane;
        size_t dim1[3];
        dim1[0] = dim[0];
        dim1[1] = dim[1];
        dim1[2] = dim[2];
        SubVolumeHelperAPI::GetBoundingRectangle(dim1, aVerticesG, miniVerticesG, matDoseGridToNormalPlane);
        int width, height;
        width = static_cast<int>(Mcsf::Vector3f(miniVerticesG[1] - miniVerticesG[0]).Magnitude() + 1.5);
        height = static_cast<int>(Mcsf::Vector3f(miniVerticesG[3] - miniVerticesG[0]).Magnitude() + 1.5);
        Matrix4x4 worldToNormalPlane = TpsArithmeticConverter::ConvertToMatrix4x4(matDoseGridToNormalPlane);
        worldToNormalPlane.Append(TpsArithmeticConverter::ConvertToMatrix4x4(matW2G));

        //call AlgorithmProxy API to cut dose grid
        //cut dose grid according plane information
        float* dosePlaneBuffer = new float[width * height];
        memset(dosePlaneBuffer, 0, sizeof(float) * width * height);
        //TEST_PERFORMANCE_BEGIN

        float* doseGridBuffer = doseGrid->get_dosegrid_buffer();
        if (doseGrid->get_isdosevalid() && 
            TPS_ER_SUCCESS != SubVolumeHelperAPI::GetSectionOfVolume(
            doseGridBuffer, dim, miniVerticesG, 
            width, height, dosePlaneBuffer))
        {
            TPS_LOG_DEV_ERROR<<"Failed to get section of volume.";
            delete[] dosePlaneBuffer;
            return nullptr;
        }

        //TEST_PERFORMANCE_END(GetSectionOfVolume)
//         #ifdef _DEBUG
//         char fileName[256];
//         sprintf_s(fileName, "d:\\temp\\slice_dose_result_%s.bmp", 
//             (mPlanUID + "_" + GOTypeHelper::ToString(aDoseMPRKey[i][0])).c_str());
//         SubVolumeHelperAPI::SaveToTrueColorBitmapFloat(fileName, dosePlaneBuffer, height, width);
//         #endif

        //set dose grid buffer to dose go
        doseGO->SetDoseGrid(dosePlaneBuffer, height, width, worldToNormalPlane);
        doseGO->SetDirty(true);

//         float* tmp = new float[width];
//         for (int i = 0; i <= height / 2; ++i)
//         {
//             memcpy(tmp, dosePlaneBuffer + i * width, width * sizeof(float));
//             memcpy(dosePlaneBuffer + i * width, dosePlaneBuffer + (height - 1 - i) * width, width * sizeof(float));
//             memcpy(dosePlaneBuffer + (height - 1 - i) * width, tmp, width * sizeof(float));
//         }
//         delete[]tmp;
        delete[] dosePlaneBuffer;
    }
    return true;
}

TPS_END_NAMESPACE  // end namespace tps
