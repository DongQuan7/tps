////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song   song.li@united-imaging.com
/// 
///  \file tps_rc_voi_helper.h
/// 
///  \brief class TpsMprHelper declaration 
/// 
///  \version 1.0
/// 
///  \date    Sep.21, 2015
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"
#include <string.h>     // for memset, memcpy etc
#include <stdio.h>      // for printf etc

//ZHENGHE
#include <math.h>
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"   
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"

#include "RtTpsFramework/tps_fw_safelock.h"             // for TpsSafeLock
#include "RtTpsAlgorithmProxy/tps_ap_utility.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

//#define DEBUG_VOI_FROM_DOSE

bool TpsMprHelper::GetMinThickness(
    const VolumeGOPtr& spVolumeGO, 
    const MprGOPtr& spMPRGO,
    double& dThicknessInWorld)
{
    if (spMPRGO == nullptr || spVolumeGO == nullptr)
    {
        TPS_LOG_TRACE_ERROR<<"Invalid parameters.";
        return false;
    }

    // 1. Model坐标系下的法线
    Vector3D vNormalInWorld;
    spMPRGO->GetNormal(&vNormalInWorld);

    size_t aVolumeDim[3];
    spVolumeGO->GetDimension(aVolumeDim);

    double aSpacing[3];
    spVolumeGO->GetSpacing(aSpacing);

    double dSliceThickness = spVolumeGO->GetSliceThickness();

    Matrix4x4 matM2W = spVolumeGO->GetModel2World();
    Matrix4x4 matW2M = matM2W.Inverse();


    Vector3D vNormalInModel = matW2M.Transform(vNormalInWorld);
    vNormalInModel.Normalize();

    // 1. Model坐标系下的立方体
    double dVoxelInModel_X = 1.0 / (double)aVolumeDim[0];
    double dVoxelInModel_Y = 1.0 / (double)aVolumeDim[1];
    double dVoxelInModel_Z = dSliceThickness / (double)aVolumeDim[2] / aSpacing[2];

    // 2. 法线和立方体的相交线
    // Calculate the min spacing along normal direction,
    // which is the min of the 3 intersection lines with 3 planes

    double dMinSpacingM = 2.0;                                  // Model的最大边长为1，最大斜线为1.414
    if (abs(vNormalInModel.x) > 1e-10)
    {
        dMinSpacingM = abs(dVoxelInModel_X / vNormalInModel.x);
    }
    if (abs(vNormalInModel.y) > 1e-10)
    {
        dMinSpacingM = min(dMinSpacingM, abs(dVoxelInModel_Y / vNormalInModel.y));
    }
    if (abs(vNormalInModel.z) > 1e-10)
    {
        dMinSpacingM = min(dMinSpacingM, abs(dVoxelInModel_Z / vNormalInModel.z));
    }

    Vector3D vIntersectInModel = vNormalInModel * dMinSpacingM;

    // 3. 变换到World坐标系下
    Vector3D vIntersectInWorld = matM2W.Transform(vIntersectInModel);
    dThicknessInWorld = vIntersectInWorld.Magnitude();

    return true;
}

bool TpsMprHelper::GetSliceNumber(
    const VolumeGOPtr& spVolumeGO, 
    const MprGOPtr& spMPRGO, 
    int& iCurrentSliceNumber, 
    int& iTotalSliceNumber, 
    PlaneTypeInVol& ePlaneType)
{
    if (spMPRGO == nullptr || spVolumeGO == nullptr)
    {
        TPS_LOG_TRACE_ERROR<<"Invalid parameters.";
        return false;
    }
    Matrix4x4 matWorld2Model = spVolumeGO->GetModel2World().Inverse();

    Vector3D normalW;
    spMPRGO->GetNormal(&normalW);
    normalW.Normalize();
    Point3D center = spMPRGO->GetCenter();
    Vector3D normalM = matWorld2Model.Transform(normalW);
    normalM.Normalize();
    center = matWorld2Model.Transform(center);

    size_t dim[3];
    spVolumeGO->GetDimension(dim);
    if (fabs(fabs(normalM.z) - 1) <= 1e-6)
    {
        iCurrentSliceNumber = int(center.z * dim[2] + 1);
        iTotalSliceNumber = dim[2];
        ePlaneType = TpsMprHelper::Z;
        return true;
    }
    if (fabs(fabs(normalM.y) - 1) <= 1e-6)
    {
        iCurrentSliceNumber = int(center.y * dim[1] + 1);
        iTotalSliceNumber = dim[1];
        ePlaneType = TpsMprHelper::Y;
        return true;
    }
    if (fabs(fabs(normalM.x) - 1) <= 1e-6)
    {
        iCurrentSliceNumber = int(center.x * dim[0] + 1);
        iTotalSliceNumber = dim[0];
        ePlaneType = TpsMprHelper::X;
        return true;
    }
    //oblique case
    ePlaneType = TpsMprHelper::Oblique;
    return GetSliceNumberInViewOrder(spVolumeGO, spMPRGO, iCurrentSliceNumber, iTotalSliceNumber);
}

bool TpsMprHelper::GetSliceNumberInViewOrder(
    const VolumeGOPtr& spVolumeGO, 
    const MprGOPtr& spMPRGO, 
    int& iCurrentSliceNumber, 
    int& iTotalSliceNumber)
{
    if (spMPRGO == nullptr || spVolumeGO == nullptr)
    {
        TPS_LOG_TRACE_ERROR<<"Invalid parameters.";
        return false;
    }
    Matrix4x4 matModel2World = spVolumeGO->GetModel2World();

    Vector3D normalW;
    spMPRGO->GetNormal(&normalW);
    normalW.Normalize();
    //in view order
    normalW *= -1;
    Point3D center = spMPRGO->GetCenter();

    double dSpacing = 0.0;
    TpsMprHelper::CalcSpacingInWorld(spVolumeGO, spMPRGO, dSpacing);

    Point3D vertices[8] = {
        Point3D(0, 0, 0), Point3D(1, 0, 0), Point3D(0, 1, 0), Point3D(1, 1, 0),
        Point3D(0, 0, 1), Point3D(1, 0, 1), Point3D(0, 1, 1), Point3D(1, 1, 1)
    };
    double dotProd, min(100), max(-100);
    for (int i = 0; i < 8; ++i)
    {
        vertices[i] = matModel2World.Transform(vertices[i]);
        dotProd = normalW.DotProduct(vertices[i] - Point3D(0, 0, 0));
        min = TPS_MIN(min, dotProd);
        max = TPS_MAX(max, dotProd);
    }
    double curr = normalW.DotProduct(Vector3D(center.x, center.y, center.z));
    iCurrentSliceNumber = TpsUtility::Floor((curr - min) / dSpacing + 1);
    iTotalSliceNumber = iCurrentSliceNumber + TpsUtility::Floor((max - curr) / dSpacing);
    return true;
}

bool TpsMprHelper::CalcSpacingInWorld(
    const VolumeGOPtr& spVolumeGO, 
    const MprGOPtr& spMPRGO,
    double& dSpacing)
{
    if (spMPRGO == nullptr || spVolumeGO == nullptr)
    {
        TPS_LOG_TRACE_ERROR<<"Invalid parameters.";
        return false;
    }

    Vector3D normal;
    spMPRGO->GetNormal(&normal);
    size_t dim[3];
    spVolumeGO->GetDimension(dim);
    Matrix4x4 matModelToVolume(dim[0], 0, 0, 0, 
                            0, dim[1], 0, 0, 
                            0, 0, dim[2], 0, 
                            -0.5, -0.5, -0.5, 1);
    Matrix4x4 matWorld2Volume = spVolumeGO->GetModel2World().Inverse();
    matWorld2Volume.Prepend(matModelToVolume);

    normal = matWorld2Volume.Transform(normal);
    normal.Normalize();

    double posInf = 1000000.0;
    double lamda = posInf;
    if (abs(normal.x) > 1e-6)
    {
        lamda = TPS_MIN(lamda, 1.0 / abs(normal.x));
    }
    if (abs(normal.y) > 1e-6)
    {
        lamda = TPS_MIN(lamda, 1.0 / abs(normal.y));
    }
    if (abs(normal.z) > 1e-6)
    {
        lamda = TPS_MIN(lamda, 1.0 / abs(normal.z));
    }

    Vector3D step = matWorld2Volume.Inverse().Transform(normal * lamda);
    dSpacing = step.Magnitude();
    return true;
}
TPS_END_NAMESPACE
