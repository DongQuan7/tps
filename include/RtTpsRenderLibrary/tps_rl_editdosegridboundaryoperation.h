////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  ZUO Kai  kai.zuo@united-imaging.com
/// 
///  \file tps_rl_editdosegridboundaryoperation.h
/// 
///  \brief class TpsEditDoseGridBoundaryOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/04/10
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EDITDOSEGRIDBOUNDARYOPERATION_H_
#define TPS_RL_EDITDOSEGRIDBOUNDARYOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"
#include "McsfMedViewer3DArithmetic/point3d.h"
#include "McsfMedViewer3DArithmetic/vector3d.h"
#include "McsfMedViewer3DArithmetic/matrix_4x4.h"
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsEditDoseGridBoundaryOperation
/// 
/// \brief edit dose grid boundary operation
class TPS_RL_DECLSPEC TpsEditDoseGridBoundaryOperation
    : public OperationBase{
public:
    //the constructor and destructor
    TpsEditDoseGridBoundaryOperation(const std::string& planUID, 
                                     Mcsf::Point3f startPoint, 
                                     Mcsf::Point3f endPoint, Mcsf::Point3f spacing);

    ~TpsEditDoseGridBoundaryOperation();

    virtual bool ModifyGraphicObject() const;

private :
    bool HitTestAndUpdateDoseGrid() const;
    bool GetIntersectionsOfDoseGridWithMpr(double* dim, 
                                    const Mcsf::MedViewer3D::Point3D* pVertices,
                        std::vector<Mcsf::MedViewer3D::Point3D>& intersections)const;
    void CalcScreenToWorld_i() const;
    bool MakeVertexOrdered(std::vector<Mcsf::MedViewer3D::Point3D>& pVertices, 
                           int vertexCount) const;
    bool GetMovingVector(const Mcsf::MedViewer3D::Point3D& point1, 
                         const Mcsf::MedViewer3D::Point3D& point2, 
                         Mcsf::MedViewer3D::Vector3D& movingVector) const;
    bool GetMovingPlanes(const Mcsf::MedViewer3D::Point3D& point1, 
                        const Mcsf::MedViewer3D::Point3D& point2, 
                        double *dim, int* movingPlanes) const;
    bool GetAngelsBetweenMprAndMovingPlanes(const Mcsf::MedViewer3D::Point3D* 
                                pVertices, const int* movingPlanes, double* angels) const;
    bool UpdateDoseGridBoundaryGo(const int* movingPlane, const double* angels, 
        const Mcsf::MedViewer3D::Vector3D& movingVector) const;

private:
    mutable std::string mPlanUID;
    mutable Mcsf::Point3f mStartPoint;
    mutable Mcsf::Point3f mEndPoint;
    mutable Mcsf::Point3f mSpacing;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEditDoseGridBoundaryOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif