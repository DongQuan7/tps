//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
///
///  \file    tps_rl_mprgraphicobject.cpp
///  \brief   MPRGraphicObject
///
///  \version 1.0
///  \date    Nov. 11, 2013
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"

TPS_BEGIN_NAMESPACE

MPRGraphicObject::MPRGraphicObject(void): mMPRMode(0), mIsRulerVisible(true)
{
}


MPRGraphicObject::~MPRGraphicObject(void)
{
}

bool MPRGraphicObject::Initialize() {
    return Viewer3D::ClsGraphicObjectMPR::Initialize();
}

bool MPRGraphicObject::Finalize() {
    return Viewer3D::ClsGraphicObjectMPR::Finalize();
}

Viewer3D::Matrix4x4 MPRGraphicObject::GetWorldToViewMatrix(){
    //view c.s. is a left hand c.s. .
    using namespace Viewer3D;

    Point3D aVertices[4];
    m_plane3D->GetVertices(aVertices);
    Vector3D vecY = aVertices[3] - aVertices[0];
    vecY.Normalize();
    Vector3D vecZ = -1 * m_plane3D->GetNormal();
    Vector3D vecX = vecZ.CrossProduct(vecY);
    vecY = vecZ.CrossProduct(vecX);

    Point3D ptOrigin = aVertices[0];
    Vector3D temp(ptOrigin.x, ptOrigin.y, ptOrigin.z);
    return Matrix4x4(vecX.x, vecY.x, vecZ.x, 0.0, 
                     vecX.y, vecY.y, vecZ.y, 0.0, 
                     vecX.z, vecY.z, vecZ.z, 0.0, 
                     -1 * temp.DotProduct(vecX), 
                     -1 * temp.DotProduct(vecY), 
                     -1 * temp.DotProduct(vecZ), 1.0);
}
void MPRGraphicObject::SetSectionType(FIRST_POSTFIX_COMPONENT sectionType) {
    mSectionType = sectionType;
}

FIRST_POSTFIX_COMPONENT MPRGraphicObject::GetSectionType() const{
    return mSectionType;
}

void MPRGraphicObject::SetLocation(SECOND_POSTFIX_COMPONENT location){
    mLocation = location;
}

SECOND_POSTFIX_COMPONENT MPRGraphicObject::GetLocation() const{
    return mLocation;
}

void MPRGraphicObject::SetRulerIsVisible(bool isVisible){
    mIsRulerVisible = isVisible;
}
bool MPRGraphicObject::RulerVisible() const{
    return mIsRulerVisible;
}

TPS_END_NAMESPACE


