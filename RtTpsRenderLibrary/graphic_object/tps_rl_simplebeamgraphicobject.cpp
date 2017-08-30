//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_rl_simplebeamgraphicobject.cpp
///  \brief  
///  \
///  class  TpsSimpleBeamGraphicObject
///  \version 1.0
///  \date  2014/11/24
///  \
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_simplebeamgraphicobject.h"
//#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"

TPS_BEGIN_NAMESPACE

TpsSimpleBeamGraphicObject::TpsSimpleBeamGraphicObject(){

    Mcsf::Matrix4f tempB2P;

    Mcsf::Vector4f vector0(1.0f, 0.0f, 0.0f, 0.0f);
    Mcsf::Vector4f vector1(0.0f, -0.0f, 1.0f, 0.0f);
    Mcsf::Vector4f vector2(0.0f, -1.0f, -0.0f, 0.0f);
    Mcsf::Vector4f vector3(33.022098541f, -905.87548828f, 232.49995422f, 1.0f);
    tempB2P.SetCol0(vector0);
    tempB2P.SetCol1(vector1);
    tempB2P.SetCol2(vector2);
    tempB2P.SetCol3(vector3);

    mBeam2PatMatrix = TpsArithmeticConverter::ConvertToMatrix4x4(tempB2P);

}
TpsSimpleBeamGraphicObject::~TpsSimpleBeamGraphicObject() {

}
bool TpsSimpleBeamGraphicObject::Initialize() {
    return true;
}

bool TpsSimpleBeamGraphicObject::Finalize() {
    return true;
}

/////////////////////////////////////////////////////////////////////////
void TpsSimpleBeamGraphicObject::CreateSimpleBeam(){
    RtDbDef::BEAM_TYPE btype = RtDbDef::STATICIMRT;

    switch(btype){
    case RtDbDef::VMAT:

        break;
    case RtDbDef::BURST_ARC:
        //TODO
        break;

    case RtDbDef::DYNAMICIMRT:
    case RtDbDef::STATICIMRT:
    default:
        break;
    }
}

void TpsSimpleBeamGraphicObject::AddSegmentGO(std::shared_ptr<TpsSegmentGraphicObject> segment) {
    mSegmentList.push_back(segment);
}

void TpsSimpleBeamGraphicObject::SetActiveSegment(int index) {
    mActiveSegmentGoIndex = index;
}
int TpsSimpleBeamGraphicObject::GetActiveSegmentIndex() const {
    return mActiveSegmentGoIndex;
}
void TpsSimpleBeamGraphicObject::SetBeamType(BEAM_TYPE btype){
    mType = btype;
}

void TpsSimpleBeamGraphicObject::SetGantryStartAngle(const float &gsa){
    mGantryStartAngle = gsa;
}

void TpsSimpleBeamGraphicObject::SetGantryArcLength(const float &gea){
    mGantryArcLength = gea;
}

void TpsSimpleBeamGraphicObject::SetCouchAngle(const float &ca){
    mCouchAngle = ca;
}

void TpsSimpleBeamGraphicObject::SetCollimatorAngle(const float &ca){
    mCollimatorAngle = ca;
}


TpsSimpleBeamGraphicObject::BEAM_TYPE TpsSimpleBeamGraphicObject::GetBeamType() const{
    return mType;
}

float TpsSimpleBeamGraphicObject::GetGantryStartAngle() const{
    return mGantryStartAngle;
}

float TpsSimpleBeamGraphicObject::GetGantryArcLength() const{
    return mGantryArcLength;
}

float TpsSimpleBeamGraphicObject::GetCollimatorAngle() const{
    return mCollimatorAngle;
}

float TpsSimpleBeamGraphicObject::GetCouchAngle() const{
    return mCouchAngle;
}

void TpsSimpleBeamGraphicObject::SetGantryIsClockWise(bool isClockWise){
    mIsClockWise = isClockWise;
}
bool TpsSimpleBeamGraphicObject::GetGantryIsClockWise() const{
    return mIsClockWise;
}

std::vector<std::shared_ptr<TpsSegmentGraphicObject>> TpsSimpleBeamGraphicObject::GetSegmentGoList() const{
    return mSegmentList;
}

void TpsSimpleBeamGraphicObject::SetBeamToPatMatrix(const Viewer3D::Matrix4x4 &mat){
    mBeam2PatMatrix = mat;
}

void TpsSimpleBeamGraphicObject::GetBeamToPatMatrix(Viewer3D::Matrix4x4 *mat) const{
    //memcpy(mat, &mBeam2PatMatrix, 16);
    *mat = mBeam2PatMatrix;
}

TPS_END_NAMESPACE
