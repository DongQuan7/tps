////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_wedgegraphicobjectconverter.cpp
/// 
///  \brief class TpsWedgeGOConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/04/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_wedgegraphicobjectconverter.h"

#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsRenderLibrary/tps_rl_wedgegraphicobject.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_accessory.h"


#define SWD_PERCENTAGE 0.8

TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsWedgeGOConverter::TpsWedgeGOConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType), mAccessory(nullptr), mMachine(nullptr){

}

TpsWedgeGOConverter::~TpsWedgeGOConverter(){

}

GraphicObjectPtr TpsWedgeGOConverter::CreateGraphicObject_i(){
    return std::shared_ptr<TpsWedgeGraphicObject>(new TpsWedgeGraphicObject());
}

bool TpsWedgeGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    if (!mDataRepository->GetMachineManager()->GetAccessory(para.obj_uid, mAccessory) || mAccessory == nullptr){
        TPS_LOG_DEV_ERROR<<"The mAccessory does not exist which UID: "<<para.obj_uid;
        return false;
    }

    const std::string machineUID = mAccessory->get_machineuid();
    if (machineUID.empty()) {
        TPS_LOG_DEV_ERROR<<"The related machine of mAccessory : "<<para.obj_uid << " is empty";
        return false;
    }
    
    mDataRepository->GetMachineManager()->GetMachine(machineUID, mMachine);
    return true;
}

bool TpsWedgeGOConverter::Update_i(GraphicObjectPtr go)
{
    auto wedge = std::dynamic_pointer_cast<TpsWedgeGraphicObject>(go);
    if (nullptr == mAccessory || nullptr == wedge || nullptr == mMachine) 
        return false;

    //check accessory is wedge, if not return null
    auto accessoryType = mAccessory->get_accessorytype();
    if (RtDbDef::ACCESSORY_PHYSICALWEDGE != accessoryType
        && RtDbDef::ACCESSORY_VIRTUALWEDGE != accessoryType) {
        TPS_LOG_DEV_INFO<<"The mAccessory is not a wedge: "<<mAccessory->get_uid();
        return false;
    }

    //check if the angle and orientation of wedge is valid
    auto wedgeOrientation = mAccessory->get_wedgeorientation();
    double angle = mAccessory->get_wedgeangle();
    if (wedgeOrientation < 1 || angle <= 0) {
        TPS_LOG_DEV_INFO<<"The angle and orientation of wedge is invalid:"<<mAccessory->get_uid();
        return false;
    }
    angle = ConvertToDisplayAngle_i(angle);

    //set color
    float color[4] = {1.0f, 1.0f, 0.0f, 1.0f};
    wedge->SetColor(color);

    double sad = mMachine->get_sad();
    //double sad = 1000;
    //put wedge on the 0.8 * sad point
    double swd = sad * SWD_PERCENTAGE;

    double orientation = 0;
    switch(wedgeOrientation) {
        case RtDbDef::TOE_IN:
            orientation = 0;
            break;
        case RtDbDef::TOE_OUT:
            orientation = 180;
            break;
        case RtDbDef::TOE_RIGHT:
            orientation = 270;
            break;
        case RtDbDef::TOE_LEFT:
            orientation = 90;
            break;
        default:
            break;
    }

    /*Mcsf::MedViewer3D::Matrix4x4 w2B = TpsArithmeticConverter::ConvertToMatrix4x4(
         mAccessory->GetALGT_filter_to_beam());*/
    Mcsf::MedViewer3D::Matrix4x4 w2B;
    w2B.SetIdintity();
    float radians = -1 * orientation * float(PI) / 180.0f;
    w2B.Append(MakeQuaternion(Mcsf::MedViewer3D::Quaternion(
        radians, Mcsf::MedViewer3D::Vector3D(0, 0, 1))));
    w2B.m[3][2] = -1 * swd;
    wedge->SetWedgeToBeam(w2B);

    double limits[4] = {-50, 50, -50, 50};
    /*double limits[4] = {
        mAccessory->get_min1_jaw1(), mAccessory->get_max2_jaw1(), 
        mAccessory->get_min1_jaw2(), mAccessory->get_max2_jaw2()};*/
    double width, height;
    if (orientation == 0 || orientation == 180){
        width = limits[1] - limits[0];
        height = limits[3] - limits[2];
    }
    else{
        width = limits[3] - limits[2];
        height = limits[1] - limits[0];
    }
    width *= SWD_PERCENTAGE;
    height *= SWD_PERCENTAGE;
    float thickness = abs(tan(angle * PI / 180)) * height;
    Mcsf::MedViewer3D::Point3D vertices[WEDGE_GO_VERTEX_COUNT] = {
        Mcsf::MedViewer3D::Point3D(-0.5 * width, -0.5 * height, 0), 
        Mcsf::MedViewer3D::Point3D(0.5 * width, -0.5 * height, 0), 
        Mcsf::MedViewer3D::Point3D(0.5 * width, 0.5 * height, 0), 
        Mcsf::MedViewer3D::Point3D(-0.5 * width, 0.5 * height, 0), 
        Mcsf::MedViewer3D::Point3D(-0.5 * width, -0.5 * height, thickness), 
        Mcsf::MedViewer3D::Point3D(0.5 * width, -0.5 * height, thickness)
    };
    wedge->SetVertices(vertices);
    return true;
}

double TpsWedgeGOConverter::ConvertToDisplayAngle_i(double angle) const
{
    double org_angles[5] = {15.0, 30.0, 45.0, 60.0, 75.0};
    double display_angles[5] = {15.0, 22.5, 30.0, 37.5, 45.0};

    int i = 0;
    while(i <5 && angle > org_angles[i])
    {
        i++;
    }
    if (i == 0)
    {
        return display_angles[0];
    }
    else if (i > 4)
    {
        return display_angles[4];
    }
    else
    {
        double w = (angle - org_angles[i - 1]) / (org_angles[i] - org_angles[i - 1]);
        return display_angles[i - 1] + w * (display_angles[i] - display_angles[i - 1]);
    }
}
TPS_END_NAMESPACE  // end namespace tps