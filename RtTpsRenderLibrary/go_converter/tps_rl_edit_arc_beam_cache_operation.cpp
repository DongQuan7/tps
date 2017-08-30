////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_edit_arc_beam_cache_operation.cpp
/// 
///  \brief class EditArcBeamCache implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/12/11
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_edit_arc_beam_cache_operation.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "RtTpsAlgorithmProxy/tps_ap_utility.h"

#include "tps_logger.h"

//2 degrees
#define INTERVAL_DEGREE 2
#define MIN_PRECISION 0.1
#define FLT_EPSILON_LOC 1e-3 
#define MIN_VALID_ANGLE_FROM_EDGE 45

using namespace Mcsf::MedViewer3D;
TPS_BEGIN_NAMESPACE   // begin namespace tps

EditArcBeamCacheOperation::EditArcBeamCacheOperation(
    const std::string& imageUid, 
    const std::string& planUid, 
    const std::string ngUid, 
    const std::string& beamUid, 
    int sectionType, 
    double preX, double preY, 
    double curX, double curY, 
    DISPLAY_SIZE displaySize)
    : mImageUid(imageUid), mPlanUid(planUid)
    , mNormGroupUid(ngUid), mBeamUid(beamUid)
    , mSectionType(sectionType), mPreX(preX), mPreY(preY)
    , mCurX(curX), mCurY(curY), mDisplaySize(displaySize)
{

}

EditArcBeamCacheOperation::~EditArcBeamCacheOperation()
{

}

bool EditArcBeamCacheOperation::ModifyGraphicObject() const
{
    std::string key = mPlanUid + "|" + GOTypeHelper::ToString(
        GO_TYPE_NORMALGROUP_COLLECTION);
    auto go = mModelWarehouse->GetModelObject(key);
    auto planGo = std::dynamic_pointer_cast<NormalGroupCollection>(go);
    if (planGo == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get plan graphic object.";
        return false;
    }
    auto ngGo = planGo->GetNormalGroup(mNormGroupUid);
    if (ngGo == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get normal group graphic object.";
        return false;
    }
    auto beamGo = ngGo->SelectBeam(mBeamUid);
    if (beamGo == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get beam graphic object.";
        return false;
    }
    if (!beamGo->IsArcEditModeEnabled()){
        TPS_LOG_DEV_ERROR<<"The arc edit mode is disabled.";
        return false;
    }
    ArcBeamEditCache& arcCache = beamGo->GetArcEditCache();

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType);
    key = mImageUid + "|" + GOTypeHelper::ToString(goType);
    go = mModelWarehouse->GetModelObject(key);
    auto mprGo = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mprGo == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get mpr graphic object.";
        return false;
    }

    key = mImageUid + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    go = mModelWarehouse->GetModelObject(key);
    auto volumeGo = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(go);
    if (volumeGo == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get volume graphic object.";
        return false;
    }
    auto matW2P = volumeGo->GetWorld2Patient();

    //transform to gantry coordinate
    int iWidth = mDisplaySize.width;
    int iHeight = mDisplaySize.height;
    double dRatioX = 1.0, dRatioY = 1.0;
    if (iWidth >= iHeight){
        dRatioX = static_cast<double>(iHeight) / iWidth;
    }
    else{
        dRatioY = static_cast<double>(iWidth) / iHeight;
    }

    Matrix4x4 matW2G = matW2P;
    matW2G.Prepend(arcCache._matPatient2OrgGantry);
    Vector3D normal;
    mprGo->GetNormal(&normal);
    normal = matW2G.Transform(normal);
    normal.Normalize();
    double angleNorm2GantryY = normal.AngleBetween(Vector3D(0.0, 1.0, 0.0));
    //tolerance is 20 degree, if current plane is not parallel enough with the 
    //gantry rotation plane, do nothing
    if (fabs(angleNorm2GantryY - PI/2) <= 20.0 * PI / 180){
        return true;
    }

    Point2D ptMprPlane;
    ptMprPlane.x = (mCurX - (1 - dRatioX) / 2.0) / dRatioX;
    ptMprPlane.y = (mCurY - (1 - dRatioY) / 2.0) / dRatioY;
    auto curPoint = mprGo->GetWorldCoordinate(ptMprPlane);
    curPoint = matW2G.Transform(curPoint);
    ptMprPlane.x = (mPreX - (1 - dRatioX) / 2.0) / dRatioX;
    ptMprPlane.y = (mPreY - (1 - dRatioY) / 2.0) / dRatioY;
    auto prePoint = mprGo->GetWorldCoordinate(ptMprPlane);
    prePoint = matW2G.Transform(prePoint);

    int sign = arcCache._isClockwise ? 1 : -1;
    double startAngle = arcCache._startGantry;
    double arcLength = arcCache._arcLength;
    double endAngle = TpsUtility::ToClockPosition(startAngle + sign * arcLength);
    bool editingStart = arcCache._isToRotateStart;
    double& targetAngle = editingStart ? startAngle : endAngle;

    Vector3D vecRotationAxis(0.0, 1.0, 0.0);
    Vector3D vecGantry(0.0, 0.0, 1.0);
    Vector3D vecCur(curPoint.x, curPoint.y, curPoint.z);
    //in-project to gantry rotation plane
    if (fabs(vecCur.y) > FLT_EPSILON_LOC){
        double lamda = (0.f - vecCur.y) / normal.y;
        vecCur += lamda * normal;
    }
    Vector3D vecPre(sin(targetAngle * PI / 180), 0.0, cos(targetAngle * PI / 180));
    //利用当前鼠标所在点与Gantry原点组成的向量与当前编辑轴的夹角，来判断该次编辑对弧长的增减。
    //这种方法存在一个问题，如果从无效区域进入有效区，而此时上述的夹角小于180的话，则出现弧的突变。
    //我们希望从哪里出界，就从哪里回来；所以我们设置一个阈值（10度），如果之前的编辑鼠标处于无效区内，
    //并且当前的角度符合要求，那么该次编辑的弧长变化不应大于这个阈值，如果大于，则放弃。
    bool isPositiveMovement = vecRotationAxis.AngleBetween(vecPre.CrossProduct(vecCur)) < PI/2;
    double currentAngle = TPS_FLOAT_SIGN(vecCur.x) * vecGantry.AngleBetween(vecCur);
    currentAngle *= 180.0 / PI;
    currentAngle = TpsUtility::ToClockPosition(currentAngle);

    double delta = fabs(targetAngle - currentAngle);
    delta = TPS_MIN(delta, TpsUtility::ToClockPosition(-delta));
    sign = 1;
    if (arcCache._isClockwise &&  isPositiveMovement &&  editingStart || 
        arcCache._isClockwise && !isPositiveMovement && !editingStart || 
       !arcCache._isClockwise &&  isPositiveMovement && !editingStart || 
       !arcCache._isClockwise && !isPositiveMovement &&  editingStart)
    {
        sign = -1;
    }

    //编辑开始时，计算这次旋转角度的有效范围
    if (!arcCache._isEditingInfoInit)
    {
        CalcValidRotationRange_i(startAngle, arcLength, arcCache);
        beamGo->SetDirty(true);
    }
    bool isOnEdge = false;
    arcLength += sign * delta;
    if (arcLength <= INTERVAL_DEGREE)
    {
        arcLength = INTERVAL_DEGREE;
        isOnEdge = true;
    }
    else if (arcLength >= arcCache._maxArcLength)
    {
        arcLength = arcCache._maxArcLength;
        isOnEdge = true;
    }
    
    double deltaArcLength = fabs(arcCache._arcLength - arcLength);
    if (deltaArcLength <= 1e-6)
    {
        return true;
    }
    if (arcCache._isOnEdge && deltaArcLength > MIN_VALID_ANGLE_FROM_EDGE)
    {
        return true;
    }
    arcCache._isOnEdge = isOnEdge;
    arcCache._arcLength = arcLength;
    
    double orgAngle = editingStart ? arcCache._startGantry : endAngle;
    if (editingStart)
    {
        arcCache._startGantry = arcCache._isClockwise ? 
            endAngle - arcCache._arcLength : endAngle + arcCache._arcLength;
        arcCache._startGantry = TpsUtility::ToClockPosition(arcCache._startGantry);

        //printf("\n start: %f, arc length: %f", arcCache._startGantry, arcCache._arcLength);
    }
    sign = arcCache._isClockwise ? 1 : -1;
    double deltaAngle = (editingStart ? arcCache._startGantry : 
        TpsUtility::ToClockPosition(arcCache._startGantry + sign * arcLength))- orgAngle;
    Quaternion quat(deltaAngle * PI / 180.0, Vector3D(0.0, 1.0, 0.0));
    Matrix4x4& targetMatrix = editingStart ? arcCache._matStartGantry2Patient
        : arcCache._matEndGantry2Patient;
    targetMatrix.Append(quat.ToMatrix());

    beamGo->SetDirty(true);
    return true;
}

void EditArcBeamCacheOperation::CalcValidRotationRange_i(
    double start, double arcLength, ArcBeamEditCache& cache) const
{
    bool isFullArc = fabs(cache._maxAngle - cache._minAngle) - 
        (360.0 - MIN_PRECISION) >= -FLT_EPSILON_LOC;
    bool isCW = cache._isClockwise;
    int sign = isCW ? 1 : -1;
    bool isEditingStart = cache._isToRotateStart;
    double end = TpsUtility::ToClockPosition(start + sign * arcLength);
    double remainingAngle = isEditingStart ? end : start;
    if (!isFullArc)
    {
        double min = cache._minAngle;
        double max = cache._maxAngle;
        double cwMax = min + cache._cwRange;
        double ccwMin = max - cache._ccwRange;

        double minAngle = isCW ? min : TPS_MAX(min, ccwMin);
        double maxAngle = isCW ? TPS_MIN(max, cwMax) : max;
        if ((isCW && isEditingStart) || 
            (!isCW && !isEditingStart))
        {
            cache._maxArcLength = remainingAngle - minAngle;
        }
        else
        {
            cache._maxArcLength = maxAngle - remainingAngle;
        }
    }
    else
    {
        double min = -cache._ccwRange;
        double max = cache._cwRange;
        if ((isCW && isEditingStart) || 
            (!isCW && !isEditingStart))
        {
            double close2Max = remainingAngle + floor((max - remainingAngle) / 360) * 360;
            cache._maxArcLength = close2Max - min;
        }
        else
        {
            double endClose2Min = remainingAngle + ceil((min - remainingAngle) / 360) * 360;
            cache._maxArcLength = max - endClose2Min;
        }
    }
    cache._isOnEdge = fabs(cache._arcLength - INTERVAL_DEGREE) <= 1e-6 || 
        fabs(cache._arcLength - cache._maxArcLength) <= 1e-6;
    cache._isEditingInfoInit = true;
}

bool EditArcBeamCacheOperation::BelongsToCircleSection_i(
    double start, double end, double value) const
{
    //if start equals to end, we consider the case as one circle range
    if (fabs(start - end) <= FLT_EPSILON_LOC) return true;
    if (end - start >= FLT_EPSILON_LOC)
    {
        return value - start >= -FLT_EPSILON_LOC && value - end <= FLT_EPSILON_LOC;
    }
    else
    {
        return (value - start >= -FLT_EPSILON_LOC && value - 360.0 <= FLT_EPSILON_LOC) || 
            (value >= -FLT_EPSILON_LOC && value - end <= FLT_EPSILON_LOC);
    }
}

bool EditArcBeamCacheOperation::IsFullReachablePosition_i(
    double start, double end, double value) const
{
    //value + x * 360 = y; y 属于[start, end], x的解是否大于1个
    if (start > end) return false;
    double lamda = (value - start) / 360;
    lamda -= floor(lamda);

    double y0 = start + lamda * 360;
    if (y0 < start || y0 > end)
    {
        return false;
    }
    double y1 = y0 + 360;
    if (y1 < start || y1 > end)
    {
        return false;
    }
    return true;
}

TPS_END_NAMESPACE  // end namespace tps