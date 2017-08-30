//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Zuo Kai kai.zuo@united-imaging.com
//
/// \file    tps_rl_dosegridboundaryrenderstrategy.h
///
/// \brief   DoseGridBoundaryRenderStrategy
/// 
/// \version 1.0
/// 
/// \date    2014/04/10
/////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DOSEGRIDBOUNDARYRENDERSTRATEGY_H_
#define TPS_RL_DOSEGRIDBOUNDARYRENDERSTRATEGY_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsRenderLibrary/tps_rl_dosegridboundarygraphicobject.h"
#include "McsfMedViewer3DArithmetic/point3d.h"
#include "McsfMedViewer3DArithmetic/vector3d.h"
#include <vector>

namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE

class MPRGraphicObject;
struct HitTestResult;


class TPS_RL_DECLSPEC DoseGridBoundaryRenderStrategy : public TpsRenderStrategyBase
{
public:
    DoseGridBoundaryRenderStrategy();
    ~DoseGridBoundaryRenderStrategy();
    virtual void Initialize();
    virtual void Finalize();
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);

    void SetDoseGridBoundaryGO(std::shared_ptr<TpsDoseGridBoundaryGraphicObject> go);
    void SetMprGraphicObject(std::shared_ptr<MPRGraphicObject>);
    void SetVolumeGraphicObject(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>);
    void HitTest(double x, double y, HitTestResult& hitRes);
    bool HitLine_i(const Mcsf::MedViewer3D::Point2D& point,
        Mcsf::MedViewer3D::Point2D& startPt, Mcsf::MedViewer3D::Point2D& endPt);
    bool HitPoint_i(const Mcsf::MedViewer3D::Point2D& point,
        Mcsf::MedViewer3D::Point2D& targetPt);

protected:
    virtual void Render_i();

private:
    bool GetIntersectionsOfDoseGridWithMpr(double* dim,
        const Mcsf::MedViewer3D::Point3D* pVertices,
        std::vector<Mcsf::MedViewer3D::Point3D>& intersections);
    void CalcScreenToWorld_i();
    bool MakeVertexOrdered(std::vector<Mcsf::MedViewer3D::Point3D>& pVertices,
        int vertexCount);
    void RenderForTest();
    bool GetIntersectionOfBoundaryWithPlane(std::vector<Mcsf::MedViewer3D::Point3D>& pVertices,
        std::vector<Mcsf::MedViewer3D::Point3D>& intersections, double A, double B, double C, double D);

private:
    Mcsf::MedViewer3D::Matrix4x4                         mScreenToWorldMat;
    std::shared_ptr<TpsDoseGridBoundaryGraphicObject>    mDoseGridBoundaryGO;
    std::shared_ptr<MPRGraphicObject>                    mMprGraphicObject;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> mVolumeGraphicObject;
    std::vector<Mcsf::MedViewer3D::Point2D>              mHitTestData;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(DoseGridBoundaryRenderStrategy);
};
TPS_END_NAMESPACE

#endif