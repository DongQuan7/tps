//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Miao Chenfeng Chenfeng.Miao@united-imaging.com
//
/// \file    tps_rl_beamgraphicobject.cpp
///
/// \brief   TpsBeamGraphicObject
/// 
/// \version 1.0
/// 
/// \date    2013/12/28
/////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_wedgegraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_applicator_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_block_graphic_object.h"
#include "RtTpsFramework/tps_fw_fluence_map_entity.h"

using namespace Mcsf::MedViewer3D;

#define SAD 1000

TPS_BEGIN_NAMESPACE

TpsBeamGraphicObject::TpsBeamGraphicObject()
: mIsVisible(true), mActiveSegmentGoIndex(0), mPhysicalWedgeGo(nullptr)
, /*mCombinedSegmentGo(nullptr),*/ mIsJoined(true), mVirtualWedgeGo(nullptr)
, mWeight(0.0f), mMU(0.0f), mApplicator(nullptr), mApertureBlock(nullptr)
, mEnergy(0.f), mArcEditModeEnabled(false)
{

}

bool TpsBeamGraphicObject::Initialize() 
{
    mSegmentGoList.clear();
    return true;
}

bool TpsBeamGraphicObject::Finalize() 
{
    mSegmentGoList.clear();
    return true;
}

TpsBeamGraphicObject::~TpsBeamGraphicObject() 
{

}

std::vector<std::shared_ptr<TpsSegmentGraphicObject>> TpsBeamGraphicObject::GetSegmentGoList() const
{
    return mSegmentGoList;
}

void TpsBeamGraphicObject::AddSegmentGo(std::shared_ptr<TpsSegmentGraphicObject> segmentGo) 
{
    mSegmentGoList.push_back(segmentGo);
}

void TpsBeamGraphicObject::SetSegmentGos(const std::vector<std::shared_ptr<TpsSegmentGraphicObject>> &segmentGoList) 
{
    mSegmentGoList = segmentGoList;
}

void TpsBeamGraphicObject::RemoveAllGos() 
{
    mSegmentGoList.clear();
    mPhysicalWedgeGo = nullptr;
    mApplicator = nullptr;
    mVirtualWedgeGo = nullptr;
    mApertureBlock = nullptr;
}

void TpsBeamGraphicObject::SetColorForMprWindow(const float color[4]) 
{
    memcpy(mColorForMprWindow,color,4*sizeof(float));
}

void TpsBeamGraphicObject::SetColorForBevWindow(const float color[4]) 
{
    memcpy(mColorForBevWindow,color,4*sizeof(float));
}

void TpsBeamGraphicObject::GetColorForMprWindow(float color[4]) 
{
    memcpy(color,mColorForMprWindow,4*sizeof(float));
}

void TpsBeamGraphicObject::GetColorForBevWindow(float color[4]) 
{
    memcpy(color,mColorForBevWindow,4*sizeof(float));

}

bool TpsBeamGraphicObject::GetVisible() const 
{
    return mIsVisible;
}

void TpsBeamGraphicObject::SetVisible(bool flag) 
{
    mIsVisible = flag;
}

void TpsBeamGraphicObject::SetPhysicalWedgeGO(std::shared_ptr<TpsWedgeGraphicObject> wedge)
{
    mPhysicalWedgeGo = wedge;
}

std::shared_ptr<TpsWedgeGraphicObject> TpsBeamGraphicObject::GetPhysicalWedgeGO() const
{
    return mPhysicalWedgeGo;
}
std::shared_ptr<TpsSegmentGraphicObject> TpsBeamGraphicObject::GetActiveSegmentGo()
{
    if(mActiveSegmentGoIndex < 0 || mActiveSegmentGoIndex >= mSegmentGoList.size()) {
        TPS_LOG_DEV_ERROR<<"segmentGo index out of range!"<<mActiveSegmentGoIndex;
        return nullptr;
    }
    return mSegmentGoList[mActiveSegmentGoIndex];
}

int TpsBeamGraphicObject::GetActiveSegmentIndex() const 
{
    return mActiveSegmentGoIndex;
}

void TpsBeamGraphicObject::SetActiveSegmentIndex(int index) 
{
    mActiveSegmentGoIndex = index;
}

void TpsBeamGraphicObject::CorrectActiveSegment()
{
    mActiveSegmentGoIndex = TPS_CLAMP(mActiveSegmentGoIndex, 0, mSegmentGoList.size() - 1);
}

//std::shared_ptr<TpsSegmentGraphicObject> TpsBeamGraphicObject::GetCombinedSegmentGo() const {
//    return mCombinedSegmentGo;
//}
//
//void TpsBeamGraphicObject::SetCombinedSegmentGo(std::shared_ptr<TpsSegmentGraphicObject> segmentGo) {
//    mCombinedSegmentGo = segmentGo;
//}

int TpsBeamGraphicObject::GetSegmentGoCount() const 
{
    return mSegmentGoList.size();
}

void TpsBeamGraphicObject::SetBeamType(BEAM_TYPE type) 
{
    mType = type;
}

BEAM_TYPE TpsBeamGraphicObject::GetBeamType() const 
{
    return mType;
}

void TpsBeamGraphicObject::SetVirtualWedgeGO( std::shared_ptr<TpsWedgeGraphicObject> wedge ) 
{
    mVirtualWedgeGo = wedge;
}

std::shared_ptr<TpsWedgeGraphicObject> TpsBeamGraphicObject::GetVirtualWedgeGO() const 
{
    return mVirtualWedgeGo;
}

void TpsBeamGraphicObject::SetApplicator(std::shared_ptr<ApplicatorGraphicObject> applicator)
{
    mApplicator = applicator;
}

std::shared_ptr<ApplicatorGraphicObject> TpsBeamGraphicObject::GetApplicator() const
{
    return mApplicator;
}

void TpsBeamGraphicObject::SetApertureBlock(std::shared_ptr<BlockGraphicObject> block)
{
    mApertureBlock = block;
}

std::shared_ptr<BlockGraphicObject> TpsBeamGraphicObject::GetApertureBlock() const
{
    return mApertureBlock;
}

std::shared_ptr<BlockGraphicObject> TpsBeamGraphicObject::GetShieldingBlock() const
{
    return mShieldingBlock;
}

void TpsBeamGraphicObject::SetShieldingBlock(std::shared_ptr<BlockGraphicObject> block)
{
    mShieldingBlock = block;
}

void TpsBeamGraphicObject::SetWeight(float weight)
{
    mWeight = weight;
}

float TpsBeamGraphicObject::GetWeight() const
{
    return mWeight;
}

void TpsBeamGraphicObject::SetMU(float mu)
{
    mMU = mu;
}

float TpsBeamGraphicObject::GetMU() const
{
    return mMU;
}

void TpsBeamGraphicObject::SetName(const std::string& name)
{
    mName = name;
}

std::string TpsBeamGraphicObject::GetName() const
{
    return mName;
}

RtDbDef::APERTURE_TYPE TpsBeamGraphicObject::GetApertureType() const
{
    return mApertureType;
}

void TpsBeamGraphicObject::SetApertureType(RtDbDef::APERTURE_TYPE type)
{
    mApertureType = type;
}

std::string TpsBeamGraphicObject::GetUID() const
{
    return mUID;
}

void TpsBeamGraphicObject::SetUID(const std::string& uid)
{
    mUID = uid;
}

void TpsBeamGraphicObject::SetEnergy(float energy)
{
    mEnergy = energy;
}

float TpsBeamGraphicObject::GetEnergy() const
{
    return mEnergy;
}

void TpsBeamGraphicObject::SetFluenceMapEntity(std::shared_ptr<FluenceMapEntity> fluenceMap)
{
    mFluenceMapEntity = fluenceMap;
}

std::shared_ptr<FluenceMapEntity> TpsBeamGraphicObject::GetFluenceMapEntity() const
{
    return mFluenceMapEntity;
}

bool TpsBeamGraphicObject::IsArcEditModeEnabled() const
{
    return mArcEditModeEnabled;
}

void TpsBeamGraphicObject::SetArcEditModeEnabled(bool isEnabled)
{
    mArcEditModeEnabled = isEnabled;
}

ArcBeamEditCache& TpsBeamGraphicObject::GetArcEditCache()
{
    return mArcBeamEditCache;
}

std::string TpsBeamGraphicObject::GetIsoCenterUID() const
{
    return mIsoCenterUID;
}

void TpsBeamGraphicObject::SetIsoCenterUID(const std::string& isoCenterUID)
{
    mIsoCenterUID = isoCenterUID;
}

CombinationEdges TpsBeamGraphicObject::GetCombinationFieldEdges() const
{
    return mCombinationEdges;
}

std::vector<Rectangle> TpsBeamGraphicObject::GetCombinationFieldRects() const
{
    return mCombinationFieldsRects;
}

//需要注意的是：对于IMRT beam， CombinationEdges是所有子野形状的并集
//            而对于ARC beam, 只是当前子野的形状（因为每个子野Gantry角度不同）
bool TpsBeamGraphicObject::UpdateCombinationFieldEdges()
{
    //for arc beam
    if (mType == ARC)
    {
        return UpdateARCCombinationFieldEdges_i();
    }
    //for IMRT beam
    else
    {
        return UpdateIMRTCombinationFieldEdges_i();
    }
}

bool TpsBeamGraphicObject::UpdateARCCombinationFieldEdges_i()
{
    mCombinationEdges.negEdges.clear();
    mCombinationEdges.posEdges.clear();
    mCombinationFieldsRects.clear();
    auto segment = this->GetActiveSegmentGo();
    if (segment == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Active segment is null.";
        return false;
    }

    StcMLC* mlc = segment->GetMLC();
    if (mlc == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The mlc shape of the segment is null.";
        return false;
    }

    LEAF_DIRECTION dir = mlc->m_eLeafDirection;
    float minX, maxX, minY, maxY;
    Edge tmpEdge;
    mCombinationEdges.negEdges.reserve(mlc->m_nLeafNum);
    mCombinationEdges.posEdges.reserve(mlc->m_nLeafNum);
    for (int i = 0; i < mlc->m_nLeafNum; ++i)
    {
        plunc::LEAF_POSITION_STC& leafPos = mlc->m_pLeaf[i];
        if (fabs(leafPos.pos_1 - leafPos.pos_2) <= mlc->m_fMiniGap + 1e-3)
            continue;

        minX = mlc->m_aJawPosition[0];
        maxX = mlc->m_aJawPosition[1];
        minY = mlc->m_aJawPosition[2];
        maxY = mlc->m_aJawPosition[3];

        if (dir == LEAF_DIRECTION_X)
        {
            if (leafPos.max_w - minY <= 1e-6 || 
                leafPos.min_w - maxY >= -1e-6)
                continue;

            tmpEdge.location = TPS_CLAMP(leafPos.pos_1, minX, maxX);
            tmpEdge.start = TPS_CLAMP(leafPos.min_w, minY, maxY);
            tmpEdge.end = TPS_CLAMP(leafPos.max_w, minY, maxY);
            mCombinationEdges.negEdges.push_back(tmpEdge);
            tmpEdge.location = TPS_CLAMP(leafPos.pos_2, minX, maxX);
            mCombinationEdges.posEdges.push_back(tmpEdge);
        }
        else
        {
            if (leafPos.max_w - minX <= 1e-6 || 
                leafPos.min_w - maxX >= -1e-6)
                continue;

            tmpEdge.location = TPS_CLAMP(leafPos.pos_1, minY, maxY);
            tmpEdge.start = TPS_CLAMP(leafPos.min_w, minX, maxX);
            tmpEdge.end = TPS_CLAMP(leafPos.max_w, minX, maxX);
            mCombinationEdges.negEdges.push_back(tmpEdge);
            tmpEdge.location = TPS_CLAMP(leafPos.pos_2, minY, maxY);
            mCombinationEdges.posEdges.push_back(tmpEdge);
        }
    }
    DevideCombinationFieldsToRectangles_i(dir);
    return true;
}

bool TpsBeamGraphicObject::UpdateIMRTCombinationFieldEdges_i()
{
    mCombinationEdges.negEdges.clear();
    mCombinationEdges.posEdges.clear();
    mCombinationFieldsRects.clear();

    std::vector<StcMLC*> mlcShapes;
    mlcShapes.reserve(mSegmentGoList.size());
    StcMLC* mlc;
    for (int i = 0; i < mSegmentGoList.size(); ++i)
    {
        if (mSegmentGoList[i] == nullptr) continue;
        mlc = mSegmentGoList[i]->GetMLC();
        if (mlc == nullptr) continue;
        mlcShapes.push_back(mlc);
    }
    if (mlcShapes.empty()) return true;

    //validate if all mlc shapes have same leaf number, same leaf direction
    int leafNum = mlcShapes[0]->m_nLeafNum;
    LEAF_DIRECTION leafDirection = mlcShapes[0]->m_eLeafDirection;
    for (int i = 1; i < mlcShapes.size(); ++i)
    {
        if (mlcShapes[i]->m_nLeafNum != leafNum)
        {
            TPS_LOG_DEV_ERROR<<"The mlc shapes in the beam have different leaf numbers.";
            return false;
        }
        if (mlcShapes[i]->m_eLeafDirection != leafDirection)
        {
            TPS_LOG_DEV_ERROR<<"The mlc shapes in the beam have different leaf direction.";
            return false;
        }
    }

    std::vector<Edge>& negEdges = mCombinationEdges.negEdges;
    std::vector<Edge>& posEdges = mCombinationEdges.posEdges;
    std::vector<Edge> negEdgesLeaf, posEdgesLeaf;
    for (int i = 0; i < leafNum; ++i)
    {
        CombineSingleLeaf_i(mlcShapes, i, negEdgesLeaf, posEdgesLeaf);
        negEdges.insert(negEdges.end(), negEdgesLeaf.begin(), negEdgesLeaf.end());
        posEdges.insert(posEdges.end(), posEdgesLeaf.begin(), posEdgesLeaf.end());
    }
    DevideCombinationFieldsToRectangles_i(leafDirection);
    return true;
}

void TpsBeamGraphicObject::CombineSingleLeaf_i(
    const std::vector<StcMLC*>& mlcShapes, 
    int leafNum, 
    std::vector<Edge>& negEdges, 
    std::vector<Edge>& posEdges) const
{
    negEdges.clear();
    posEdges.clear();
    negEdges.reserve(mlcShapes.size());
    posEdges.reserve(mlcShapes.size());
    float leafWidth;
    ComputeLeafEdges_i(mlcShapes, leafNum, negEdges, posEdges, leafWidth);

    CombineEdges_i(Negative, leafWidth, negEdges);
    CombineEdges_i(Positive, leafWidth, posEdges);
}

void TpsBeamGraphicObject::ComputeLeafEdges_i(
    const std::vector<StcMLC*>& mlcShapes, 
    int leafNum, 
    std::vector<Edge>& negEdges, 
    std::vector<Edge>& posEdges, 
    float& leafWidth) const
{
    negEdges.clear();
    posEdges.clear();
    leafWidth = 0.f;
    if (mlcShapes.empty()) return;

    leafWidth = mlcShapes[0]->m_pLeaf[leafNum].max_w - 
                mlcShapes[0]->m_pLeaf[leafNum].min_w;
    LEAF_DIRECTION dir = mlcShapes[0]->m_eLeafDirection;
    StcMLC* mlc;
    float minX, maxX, minY, maxY;
    Edge tmpEdge;
    for (int i = 0; i < mlcShapes.size(); ++i)
    {
        mlc = mlcShapes[i];
        plunc::LEAF_POSITION_STC& leafPos = mlc->m_pLeaf[leafNum];
        if (fabs(leafPos.pos_1 - leafPos.pos_2) <= mlc->m_fMiniGap + 1e-3)
            continue;

        minX = mlc->m_aJawPosition[0];
        maxX = mlc->m_aJawPosition[1];
        minY = mlc->m_aJawPosition[2];
        maxY = mlc->m_aJawPosition[3];

        if (dir == LEAF_DIRECTION_X)
        {
            if (leafPos.max_w - minY <= 1e-6 || 
                leafPos.min_w - maxY >= -1e-6)
                continue;

            tmpEdge.location = TPS_CLAMP(leafPos.pos_1, minX, maxX);
            tmpEdge.start = TPS_CLAMP(leafPos.min_w, minY, maxY);
            tmpEdge.end = TPS_CLAMP(leafPos.max_w, minY, maxY);
            negEdges.push_back(tmpEdge);
            tmpEdge.location = TPS_CLAMP(leafPos.pos_2, minX, maxX);
            posEdges.push_back(tmpEdge);
        }
        else
        {
            if (leafPos.max_w - minX <= 1e-6 || 
                leafPos.min_w - maxX >= -1e-6)
                continue;

            tmpEdge.location = TPS_CLAMP(leafPos.pos_1, minY, maxY);
            tmpEdge.start = TPS_CLAMP(leafPos.min_w, minX, maxX);
            tmpEdge.end = TPS_CLAMP(leafPos.max_w, minX, maxX);
            negEdges.push_back(tmpEdge);
            tmpEdge.location = TPS_CLAMP(leafPos.pos_2, minY, maxY);
            posEdges.push_back(tmpEdge);
        }
    }
}

void TpsBeamGraphicObject::CombineEdges_i(
    TpsBeamGraphicObject::CombineDirection direction, 
    float leafWidth, std::vector<Edge>& edges) const
{
    if (edges.empty()) return;

    std::vector<Edge> combinedEdges;
    //排序
    if (direction == TpsBeamGraphicObject::Negative)
    {
        sort(edges.begin(), edges.end(), [](const Edge& edge1, const Edge& edge2)->bool
        {
            return edge1.location < edge2.location; 
        });
    }
    else
    {
        sort(edges.begin(), edges.end(), [](const Edge& edge1, const Edge& edge2)->bool
        {
            return edge1.location > edge2.location;
        });
    }

    float currentWidth = 0.f, backup_end;
    std::list<Edge> handlingEdges;
    std::list<Edge>::iterator it;
    for (int i = 0; i < edges.size(); ++i)
    {
        handlingEdges.push_back(edges[i]);
        for (int j = 0; j < combinedEdges.size(); ++j)
        {
            it = handlingEdges.begin();
            while (it != handlingEdges.end())
            {
                //剔除不合理的edge
                if (it->start - it->end >= -1e-6)
                {
                    handlingEdges.erase(it++);
                    continue;
                }
                //检查已被占据的width范围，切割edge
                if (it->start - combinedEdges[j].start < -1e-6)
                {
                    backup_end = it->end;
                    it->end = TPS_MIN(it->end, combinedEdges[j].start);
                    if (backup_end - combinedEdges[j].end > 1e-6)
                    {
                        Edge newEdge;
                        newEdge.start = combinedEdges[j].end;
                        newEdge.end = backup_end;
                        newEdge.location = it->location;
                        handlingEdges.push_front(newEdge);
                    }
                    it++;
                }
                else if (it->end - combinedEdges[j].end > 1e-6)
                {
                    it->start = TPS_MAX(it->start, combinedEdges[j].end);
                    it++;
                }
                else
                {
                    handlingEdges.erase(it++);
                }
            }
        }
        for (it = handlingEdges.begin(); it != handlingEdges.end(); ++it)
        {
            combinedEdges.push_back(*it);
            currentWidth += it->end - it->start;
        }
        handlingEdges.clear();
        if (currentWidth - leafWidth >= -1e-6) break;
    }

    sort(combinedEdges.begin(), combinedEdges.end(),
        [](const Edge& edge1, const Edge& edge2)->bool
    {
        return fabs(edge1.start - edge2.start) <= 1e-6;
    });
    edges = std::move(combinedEdges);
}

//没有考虑多孔情况，如果需要这个方法，需要添加相应代码
std::vector<Point3D> TpsBeamGraphicObject::ConnectEdgesToLine_i(
    const std::vector<Edge>& negEdges, 
    const std::vector<Edge>& posEdges, 
    int leafDirection)
{
    Point3D point;
    std::vector<Point3D> outLine;
    outLine.reserve(negEdges.size() * 2 + posEdges.size() * 2);
    point.z = -1 * SAD;
    if (leafDirection == LEAF_DIRECTION_X)
    {
        for (int i = 0; i < negEdges.size(); ++i)
        {
            point.x = negEdges[i].location;
            point.y = negEdges[i].start;
            outLine.push_back(point);
            point.y = negEdges[i].end;
            outLine.push_back(point);
        }
        for (int i = posEdges.size() - 1; i >= 0; --i)
        {
            point.x = negEdges[i].location;
            point.y = negEdges[i].end;
            outLine.push_back(point);
            point.y = negEdges[i].start;
            outLine.push_back(point);
        }
    }
    else
    {
        for (int i = 0; i < negEdges.size(); ++i)
        {
            point.y = negEdges[i].location;
            point.x = negEdges[i].start;
            outLine.push_back(point);
            point.x = negEdges[i].end;
            outLine.push_back(point);
        }
        for (int i = posEdges.size() - 1; i >= 0; --i)
        {
            point.y = negEdges[i].location;
            point.x = negEdges[i].end;
            outLine.push_back(point);
            point.x = negEdges[i].start;
            outLine.push_back(point);
        }
    }
    return outLine;
}

void TpsBeamGraphicObject::DevideCombinationFieldsToRectangles_i(int leafDirection)
{
    mCombinationFieldsRects.clear();

    Rectangle tempRect;
    int negIdx = 0, posIdx = 0;
    double start(0.0), end(0.0);
    while(negIdx < mCombinationEdges.negEdges.size() && 
          posIdx < mCombinationEdges.posEdges.size())
    {
        const Edge& negEdge = mCombinationEdges.negEdges[negIdx];
        const Edge& posEdge = mCombinationEdges.posEdges[posIdx];
        if (negEdge.end - posEdge.start <= 1e-6)
        {
            negIdx++;
            continue;
        }
        if (negEdge.start - posEdge.end >= -1e-6)
        {
            posIdx++;
            continue;
        }

        start = TPS_MAX(negEdge.start, posEdge.start);
        if (negEdge.end - posEdge.end >= -1e-6)
        {
            end = posEdge.end;
            posIdx++;
        }
        if (negEdge.end - posEdge.end <= 1e-6)
        {
            end = negEdge.end;
            negIdx++;
        }

        if (leafDirection == LEAF_DIRECTION_X)
        {
            tempRect.boundings[0] = negEdge.location;
            tempRect.boundings[1] = posEdge.location;
            tempRect.boundings[2] = start;
            tempRect.boundings[3] = end;
        }
        else
        {
            tempRect.boundings[0] = start;
            tempRect.boundings[1] = end;
            tempRect.boundings[2] = negEdge.location;
            tempRect.boundings[3] = posEdge.location;
        }
        mCombinationFieldsRects.push_back(tempRect);
    }
}

void TpsBeamGraphicObject::SetIECFixToPatientMatrix(const Viewer3D::Matrix4x4& matrix)
{
    mIECFixToPatientMat = matrix;
}

Viewer3D::Matrix4x4 TpsBeamGraphicObject::GetIECFixToPatientMatrix() const
{
    return mIECFixToPatientMat;
}
TPS_END_NAMESPACE
