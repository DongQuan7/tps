////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_wedgegraphicobject.cpp
/// 
///  \brief class TpsWedgeGraphicObject implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/04/15
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_wedgegraphicobject.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

unsigned int TpsWedgeGraphicObject::mIndexBuffer[WEDGE_GO_VERTEX_INDEX_COUNT] = 
{
    0, 3, 2, 1, /*|*/ 2, 3, 4, 5, /*|*/ 0, 1, 5, 4, /*|*/ 0, 4, 3, 0, /*|*/ 1, 2, 5, 1 
};

unsigned int TpsWedgeGraphicObject::mLineNumsOnPlane[WEDGE_GO_VERTEX_INDEX_COUNT] = 
{
    0, 1, 2, 3, /*|*/ 2, 4, 5, 6, /*|*/ 0, 8, 5, 7, /*|*/ 7, 4, 3, 3, /*|*/ 1, 6, 8, 8 
};

unsigned int TpsWedgeGraphicObject::mLineDefinition[WEDGE_GO_LINE_COUNT * 2] = 
{
    /*L0*/ 0, 1, /*L1*/ 1, 2, /*L2*/ 2, 3, /*L3*/ 0, 3, /*L4*/ 3, 4, 
    /*L5*/ 4, 5, /*L6*/ 2, 5, /*L7*/ 0, 4, /*L8*/ 1, 5
};

double TpsWedgeGraphicObject::mVerticeColor[WEDGE_GO_VERTEX_COUNT * 3] = 
{
    0.17, 0.48, 0.57, 
    0.17, 0.48, 0.57, 
    0.85, 0.95, 1.00, 
    0.85, 0.95, 1.00, 
    0.17, 0.48, 0.57, 
    0.17, 0.48, 0.57
};

TpsWedgeGraphicObject::TpsWedgeGraphicObject()
{

}

TpsWedgeGraphicObject::~TpsWedgeGraphicObject()
{

}

void TpsWedgeGraphicObject::SetWedgeToBeam(const Mcsf::MedViewer3D::Matrix4x4& wedge2Beam)
{
    mWedgeToBeam = wedge2Beam;
}

Mcsf::MedViewer3D::Matrix4x4 TpsWedgeGraphicObject::GetWedgeToBeam() const
{
    return mWedgeToBeam;
}

void TpsWedgeGraphicObject::SetVertices(Mcsf::MedViewer3D::Point3D* vertices)
{
    memcpy(mVertices, vertices, WEDGE_GO_VERTEX_COUNT * sizeof(Mcsf::MedViewer3D::Point3D));
}

void TpsWedgeGraphicObject::GetVertices(Mcsf::MedViewer3D::Point3D* vertices) const
{
    memcpy(vertices, mVertices, WEDGE_GO_VERTEX_COUNT * sizeof(Mcsf::MedViewer3D::Point3D));
}

void TpsWedgeGraphicObject::GetVertexIndex(unsigned int* indexBuffer) const
{
    memcpy(indexBuffer, mIndexBuffer, WEDGE_GO_VERTEX_INDEX_COUNT * sizeof(unsigned int));
}

void TpsWedgeGraphicObject::GetVertexColor(double* color) const
{
    memcpy(color, mVerticeColor, WEDGE_GO_VERTEX_COUNT * 3 * sizeof(double));
}

void TpsWedgeGraphicObject::SetColor(float* color)
{
    memcpy(mColor, color, 4 * sizeof(float));
}

void TpsWedgeGraphicObject::GetColor(float* color) const
{
    memcpy(color, mColor, 4 * sizeof(float));
}

void TpsWedgeGraphicObject::CalcVisibleLineIndexBuffer(
    const Mcsf::MedViewer3D::Vector3D& vecLookAtToEye, 
    unsigned int* indexBuffer, unsigned int& count)
{
    if (indexBuffer == nullptr) 
    {
        count = 0;
        return;
    }

    //there are 9 lines, so use a 9 elements' array
    bool visibleFlag[WEDGE_GO_LINE_COUNT];
    memset(visibleFlag, 0, sizeof(visibleFlag));

    Mcsf::MedViewer3D::Point3D p1, p2, p3;
    Mcsf::MedViewer3D::Vector3D v1, v2;
    double angle;
    for (int i = 0; i < WEDGE_GO_VERTEX_INDEX_COUNT / 4; ++i){
        p1 = mVertices[mIndexBuffer[4 * i]];
        p2 = mVertices[mIndexBuffer[4 * i + 1]];
        p3 = mVertices[mIndexBuffer[4 * i + 2]];
        v1 = p2 - p1;
        v2 = p3 - p2;
        angle = fabs(v1.CrossProduct(v2).AngleBetween(vecLookAtToEye));
        if (angle > PI/2) continue;
        visibleFlag[mLineNumsOnPlane[4*i]] = true;
        visibleFlag[mLineNumsOnPlane[4*i + 1]] = true;
        visibleFlag[mLineNumsOnPlane[4*i + 2]] = true;
        visibleFlag[mLineNumsOnPlane[4*i + 3]] = true;
    }

    count = 0;
    for (int i = 0; i < WEDGE_GO_LINE_COUNT; ++i){
        if (!visibleFlag[i]) continue;
        indexBuffer[count] = mLineDefinition[2* i];
        indexBuffer[count + 1] = mLineDefinition[2* i + 1];
        count += 2;
    }
}
TPS_END_NAMESPACE  // end namespace tps