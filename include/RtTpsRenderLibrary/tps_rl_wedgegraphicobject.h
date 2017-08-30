////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_wedgegraphicobject.h
/// 
///  \brief class TpsWedgeGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/04/15
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_WEDGEGRAPHICOBJECT_H_
#define TPS_RL_WEDGEGRAPHICOBJECT_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"

#define WEDGE_GO_VERTEX_COUNT 6
#define WEDGE_GO_VERTEX_INDEX_COUNT 20
#define WEDGE_GO_LINE_COUNT 9

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsWedgeGraphicObject
/// 
/// Wedge coordinate system: Y axis always points to the thinner side.
/// In Zero location, X, Y Z is coincident with the beam limiting device c.s.
///
/// We define the points as following:
///     (4)¡ñ                                                       
///        |\ ¨K                                                   
///        | \  ¨K                                                 
///        |  ¡ñ(5)¨K                                               
///        |  | ¨K   ¨K                                            
///        |  |   ¨K   ¨K              wedge c.s.                  
///        |  |     ¨K   ¨K            (z)                         
///     (0)¡ñ--|-------¨K---¡ñ(3)        ¡ü                           
///         \ |         ¨K  \          ¡ñ¡ú(y)                       
///          \|           ¨K \          ¨K                         
///        (1)¡ñ---------------¡ñ(2)       (x)                       
///                                                                
/// And the 9 lines' definition:
/// Lines: L0(P0-P1), L1(P1-P2), L2(P2-P3), 
///        L3(P0-P3), L4(P3-P4), L5(P4-P5),
///        L6(P2-P5), L7(P0-P4), L8(P1-P5),
//////////////////////////////////////////////////////////////////////////
class TPS_RL_DECLSPEC TpsWedgeGraphicObject : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    TpsWedgeGraphicObject();
    virtual ~TpsWedgeGraphicObject();

    void SetWedgeToBeam(const Mcsf::MedViewer3D::Matrix4x4& wedge2Beam);
    Mcsf::MedViewer3D::Matrix4x4 GetWedgeToBeam() const;

    void SetVertices(Mcsf::MedViewer3D::Point3D* vertices);
    void GetVertices(Mcsf::MedViewer3D::Point3D* vertices) const;

    void GetVertexIndex(unsigned int* indexBuffer) const;
    void GetVertexColor(double* color) const;

    void SetColor(float* color);
    void GetColor(float* color) const;

    void CalcVisibleLineIndexBuffer(
        const Mcsf::MedViewer3D::Vector3D& vecLookAtToEye, 
        unsigned int* indexBuffer, unsigned int& count);
private:

    float mColor[4];
    Mcsf::MedViewer3D::Matrix4x4    mWedgeToBeam;

    Mcsf::MedViewer3D::Point3D      mVertices[WEDGE_GO_VERTEX_COUNT];
    static unsigned int             mIndexBuffer[WEDGE_GO_VERTEX_INDEX_COUNT];
    static double                   mVerticeColor[WEDGE_GO_VERTEX_COUNT * 3];
    static unsigned int             mLineDefinition[WEDGE_GO_LINE_COUNT*2];
    static unsigned int             mLineNumsOnPlane[WEDGE_GO_VERTEX_INDEX_COUNT];
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsWedgeGraphicObject);
};

TPS_END_NAMESPACE  // end namespace tps

#endif