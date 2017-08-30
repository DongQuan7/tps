//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  kai.zuo kai.zuo@united-imaging.com
///
///  \file    tps_pca_updategridboundarycmd.h
///  \brief   update gridboundary command
///
///  \version 1.0
///  \date    Apr. 10, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_UPDATEGRIDBOUNDARYCMD_H
#define TPS_BL_UPDATEGRIDBOUNDARYCMD_H

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedgridboundary.h"

namespace Mcsf
{
    class Point3f;
    class Vector3f;
    class Matrix4f;
}

TPS_BEGIN_NAMESPACE

class RtDosegrid;


class TPS_BL_DECLSPEC TpsUpdateGridBoundaryCMD : public TpsCommand{
public:
    TpsUpdateGridBoundaryCMD(CertifiedGridBoundary *pocoGridBoundary);
    virtual ~TpsUpdateGridBoundaryCMD(void);
protected:
    virtual int Execute();
private:
    bool HitTest(int *plane, RtDosegrid* grid);
    bool GetHitTestPlane(Mcsf::Point3f& point, double radius, int *plane);
    bool GetMovingPlane(int *plane, Mcsf::Point3f& point1, Mcsf::Point3f& point2,
        Mcsf::Point3f& point3);
    bool UpdateDoseGridBoundary();
    bool UpdateDoseGridBoundaryByMovingOnePlane(int plane, Mcsf::Point3f lbPt,
        Mcsf::Point3f rtPt, Mcsf::Point3f targetPt);
    bool UpdateDoseGrid(RtDosegrid* grid, Mcsf::Vector3f* axis, bool bCreatBuffer = true);
    bool GetAxis(const Mcsf::Matrix4f& matrix, Mcsf::Vector3f* axis);
    void MovePoint(Mcsf::Point3f& point, float offset);

    // 更新plan图像相关voi的in grid信息
    void UpdateRelatedVOIInGrid(const std::string &planUid, RtDosegrid* dosegrid);
private:
    CertifiedGridBoundary  *mPocoGridBoundary;
    int                    mPlane[3];
};

TPS_END_NAMESPACE

#endif //TPS_PCA_UPDATEGRIDBOUNDARYCMD_H