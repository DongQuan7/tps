////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_dosegraphicobjectconverter.cpp
/// 
///  \brief class TpsDoseGOConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/01/03
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_dosegraphicobjectconverter.h"

#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"

//#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsDataAccess/tps_da_configuremanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsRenderLibrary/tps_rl_mprdosegraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsDoseGOConverter::TpsDoseGOConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType), mPlan(nullptr), mDoseGrid(nullptr)
    , mMpr(nullptr), mVolume(nullptr){

}

TpsDoseGOConverter::~TpsDoseGOConverter(){

}

GraphicObjectPtr TpsDoseGOConverter::CreateGraphicObject_i(){

    return std::shared_ptr<TpsMPRDoseGraphicObject>(
        new TpsMPRDoseGraphicObject());
}

bool TpsDoseGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    using namespace Mcsf::MedViewer3D;

    mPlan = nullptr; 
    mDoseGrid = nullptr; 
    mMpr = nullptr; 
    mVolume = nullptr;

    auto it = para.data_source_map.find(PLAN);
    if (it == para.data_source_map.end()) return false;
    if (!mDataRepository->GetPatientManager()->GetPlan(it->second, &mPlan) || mPlan == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get try with uid: "<<it->second;
        return false;
    }
    //RTFWK::PLANSOURCE_TYPE type = mPlan->GetPlanSourceType();
    std::string imageUID = mPlan->get_imageseriesuid();
    auto go = mModelWarehouse->GetModelObject(
        imageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME));
    mVolume = std::dynamic_pointer_cast<GraphicObjVolume>(go);
    if (mVolume == nullptr){
        TPS_LOG_DEV_ERROR<<"The volume GO of image 3d: "<<imageUID<<" does not exist.";
        return false;
    }

    //get section type and location
    auto section = GOTypeHelper::GetFirstPostfixComponent(mGOType);
    auto location = GOTypeHelper::GetSecondPostfixComponent(mGOType);
    GO_TYPE mprType = GOTypeHelper::ComposeAsGOType(MPR, section, location);
    go = mModelWarehouse->GetModelObject(imageUID + "|" + 
        GOTypeHelper::ToString(mprType));
    mMpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mMpr == nullptr){
        TPS_LOG_DEV_ERROR<<"The MPR GO which type is: "<<mprType<<" does not exist.";
        return false;
    }

    //get dose grid
    mDoseGrid = mPlan->get_dosegrid();
    if (mDoseGrid == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get dose grid of plan: "<<it->second;
        return false;
    }

    //old requirement
    //DOSE_CALCULATION_LEVEL doseCalcLevel = DisplayStateManager::GetInstance()->GetDoseCalcLevel();
    //if(type == RTFWK::QAPLAN) {
    //    doseCalcLevel = CALCULATION_LEVEL_PLAN;
    //}
    //if (CALCULATION_LEVEL_NORMGROUP == doseCalcLevel){
    //    std::string normgroupUid = DisplayStateManager::GetInstance()->GetDoseCalcUid();
    //    mDoseGrid = mDataRepository->GetDoseManager()->GetDoseGrid(normgroupUid, DOSE_GRID_NORMGROUP);
    //}
    //else if (CALCULATION_LEVEL_BEAM == doseCalcLevel){
    //    std::string beamUid = DisplayStateManager::GetInstance()->GetDoseCalcUid();
    //    mDoseGrid = mDataRepository->GetDoseManager()->GetDoseGrid(beamUid, DOSE_GRID_BEAM);
    //}
    //else{
    //    //get dose grid
    //    mDoseGrid = &(mPlan->GetDoseGrid());
    //    if (mDoseGrid == nullptr){
    //        TPS_LOG_DEV_ERROR<<"Failed to get dose grid of plan: "<<uid;
    //        return false;
    //    }
    //}

    return true;
}

bool TpsDoseGOConverter::Update_i(GraphicObjectPtr go){
    using namespace Mcsf::MedViewer3D;
    auto doseGo = std::dynamic_pointer_cast<TpsMPRDoseGraphicObject>(go);
    if (mMpr == nullptr || mVolume == nullptr || mPlan == nullptr
        || mDoseGrid == nullptr || doseGo == nullptr){
        return false;
    }

    Matrix4x4 matW2P = mVolume->GetModel2PatientMat();
    matW2P.Append(mVolume->GetModel2World().Inverse());

    Mcsf::Matrix4f matG2P = TpsArithmeticConverter::ConvertToMatrix4f(mDoseGrid->get_grid_to_pat_t());
    Mcsf::Matrix4f matW2G = matG2P.Inverse();
    matW2G.Append(TpsArithmeticConverter::ConvertToMatrix4f(matW2P));
    unsigned int dim[3] = { mDoseGrid->get_xcount(), mDoseGrid->get_ycount(),
        mDoseGrid->get_zcount() };

    //get plane, transform to dose grid c.s.
    Point3D aVertices[4];
    Mcsf::Point3f aVerticesG[4];
    mMpr->GetVertices(aVertices);
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
    SubVolumeHelperAPI::GetBoundingRectangle(dim1, aVerticesG, miniVerticesG,matDoseGridToNormalPlane);
    int width, height;
    width = static_cast<int>(Mcsf::Vector3f(miniVerticesG[1] - miniVerticesG[0]).Magnitude() + 1.5);
    height = static_cast<int>(Mcsf::Vector3f(miniVerticesG[3] - miniVerticesG[0]).Magnitude() + 1.5);
    if (width<0 || height<0) {
        TPS_LOG_DEV_ERROR<<"Invalid width["<<width<<"],height["<<height<<"] for dose plane.";
        return false;
    }

    Matrix4x4 worldToNormalPlane = TpsArithmeticConverter::ConvertToMatrix4x4(matDoseGridToNormalPlane);
    worldToNormalPlane.Append(TpsArithmeticConverter::ConvertToMatrix4x4(matW2G));

    //cut dose grid according plane information
    float* dosePlaneBuffer = new float[width * height];
    memset(dosePlaneBuffer, 0, sizeof(float) * width * height);
    if (mDoseGrid->get_isdosevalid() && 
        TPS_ER_SUCCESS != SubVolumeHelperAPI::GetSectionOfVolume(
        mDoseGrid->get_dosegrid_buffer(), dim, miniVerticesG, 
        width, height, dosePlaneBuffer))
    {
        TPS_LOG_DEV_ERROR<<"Failed to get section of volume.";
        delete[] dosePlaneBuffer;
        dosePlaneBuffer = nullptr;
        return false;
    }

    //create dose graphic object, over!
    doseGo->SetDoseGrid(dosePlaneBuffer, height, width, worldToNormalPlane);
    doseGo->SetDirty(true);
    delete[] dosePlaneBuffer;
    dosePlaneBuffer = nullptr;
    return true;
}

TPS_END_NAMESPACE  // end namespace tps
