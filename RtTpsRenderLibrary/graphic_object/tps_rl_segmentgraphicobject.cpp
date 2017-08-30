//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Miao Chenfeng Chenfeng.Miao@united-imaging.com
//
/// \file    tps_rl_segmentgraphicobejct.cpp
///
/// \brief   TpsSegmentGraphicObject
/// 
/// \version 1.0
/// 
/// \date    2013/12/28
/////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"

TPS_BEGIN_NAMESPACE

TpsSegmentGraphicObject::TpsSegmentGraphicObject() : 
    mIsMlcVisible(true), mIsJawVisible(false), mIsActive(false), 
    mMlcOrientation(true), mNeedReCalculate(false), 
    mIsClockWise(true)
{
    mStartCollimator2PatMatrix.SetIdintity();
    mEndCollimator2PatMatrix.SetIdintity();
    memset(&m_stcMLC, 0, sizeof(m_stcMLC));
    for (int i = 0; i < StcMLC::MAX_LEAF_NUM; i++){
        m_stcMLC.m_pLeaf[i].independent = 2;
    }
    m_stcMLC.m_eLeafDirection = LEAF_DIRECTION_X;
    m_stcMLC.m_fMiniGap = 0.f;
    // some dummy data for m_stcMLC
    //m_stcMLC.m_nSelectionMode = 0;
    //m_stcMLC.m_nSelectedLeaf = -1;
    //m_stcMLC.m_nLeaf = 40;
    //LEAF_POSITION_STC* pJaw = m_stcMLC.m_pLeaf;
    //pJaw[0].independent = 2;
    //pJaw[0].pos_1 = 0.0;
    //pJaw[0].pos_2 = 0.0;
    //pJaw[0].min_w = (float)(-200.0);
    //pJaw[0].max_w = (float)(-195.0);
    //for (int i = 1; i < 39; i++)
    //{
    //    pJaw[i].independent = 2;
    //    pJaw[i].pos_1 = -i* 5.0;
    //    pJaw[i].pos_2 = 2.0;
    //    pJaw[i].min_w = pJaw[i - 1].max_w;
    //    pJaw[i].max_w = pJaw[i].min_w + 10;
    //}
    //pJaw[39].independent = 2;
    //pJaw[39].pos_1 = 0.0;
    //pJaw[39].pos_2 = 0.0;
    //pJaw[39].min_w = 195;
    //pJaw[39].max_w = 200;

//     m_stcMLC.m_nSelectionMode = MLC_IDLE;
//     m_stcMLC.m_nSelectedLeaf = -1;
    //this->m_nIsEditingOutline = false;
}

bool TpsSegmentGraphicObject::Initialize() {
    return true;
}

bool TpsSegmentGraphicObject::Finalize() {
    return true;
}

TpsSegmentGraphicObject::~TpsSegmentGraphicObject() {
}

void TpsSegmentGraphicObject::SetActiveFlag(bool flag) {
    mIsActive = flag;
}

void TpsSegmentGraphicObject::SetJawVisibleFlag(bool flag) {
    mIsJawVisible = flag;
}

void TpsSegmentGraphicObject::SetStartCollimatorToPatMatrix(const Viewer3D::Matrix4x4& matrix)
{
    mStartCollimator2PatMatrix = matrix;
    mNeedReCalculate = true;
}

void TpsSegmentGraphicObject::GetStartCollimatorToPatMatrix(Viewer3D::Matrix4x4* matrix)
{
    *matrix = mStartCollimator2PatMatrix;
}

void TpsSegmentGraphicObject::SetEndCollimatorToPatMatrix(const Viewer3D::Matrix4x4& matrix)
{
    mEndCollimator2PatMatrix = matrix;
}

void TpsSegmentGraphicObject::GetEndCollimatorToPatMatrix(Viewer3D::Matrix4x4* matrix)
{
    *matrix = mEndCollimator2PatMatrix;
}

bool TpsSegmentGraphicObject::GetActiveFlag() {
    return mIsActive;
}

void TpsSegmentGraphicObject::SetMlcVisibleFlag(bool flag) {
    mIsMlcVisible = flag;
}

bool TpsSegmentGraphicObject::GetMlcVisibleFlag() {
    return mIsMlcVisible;
}

void TpsSegmentGraphicObject::SetJawPosition(float *jawPosition) {
    for(int i=0; i<4; i++) {
        m_stcMLC.m_aJawPosition[i] = jawPosition[i];
    }
    mNeedReCalculate = true;
}

bool TpsSegmentGraphicObject::GetOutLine(std::vector<Viewer3D::Point3D> *ptList) {
    if(nullptr == ptList) return false;

    float minX = m_stcMLC.m_aJawPosition[0];
    float maxX = m_stcMLC.m_aJawPosition[1];
    float minY = m_stcMLC.m_aJawPosition[2];
    float maxY = m_stcMLC.m_aJawPosition[3];

    ptList->clear();
    if (m_stcMLC.m_nLeafNum == 0){

        ptList->push_back(Viewer3D::Point3D(minX, minY, -1000));
        ptList->push_back(Viewer3D::Point3D(maxX, minY, -1000));
        ptList->push_back(Viewer3D::Point3D(maxX, maxY, -1000));
        ptList->push_back(Viewer3D::Point3D(minX, maxY, -1000));
        return true;
    }

    std::vector<Viewer3D::Point3D> vecPtsTemp;
    //get the first/last leaf number that not closed
    int start, end;
    for (start = 0; start < m_stcMLC.m_nLeafNum && 
        fabs(m_stcMLC.m_pLeaf[start].pos_1 - m_stcMLC.m_pLeaf[start].pos_2) <= m_stcMLC.m_fMiniGap + 1e-3; start++);
    for (end = m_stcMLC.m_nLeafNum - 1; end >= 0&& 
        fabs(m_stcMLC.m_pLeaf[end].pos_1 - m_stcMLC.m_pLeaf[end].pos_2) <= m_stcMLC.m_fMiniGap + 1e-3; end--);

    Viewer3D::Point3D point(0.0, 0.0, -1000.0);
    if (m_stcMLC.m_eLeafDirection == LEAF_DIRECTION_X){
        for (int i = start; i <= end; i++)
        {
            if (m_stcMLC.m_pLeaf[i].max_w - minY <= 1e-6)   // Below the jaw bottom
                continue;
            if (m_stcMLC.m_pLeaf[i].min_w - maxY >= -1e-6)   // Above the jaw top
                break;

            point.x = TPS_CLAMP(m_stcMLC.m_pLeaf[i].pos_1, minX, maxX);
            point.y = TPS_CLAMP(m_stcMLC.m_pLeaf[i].min_w, minY, maxY);
            vecPtsTemp.push_back(point);
            point.y = TPS_CLAMP(m_stcMLC.m_pLeaf[i].max_w, minY, maxY);
            vecPtsTemp.push_back(point);
        }

        for (int i = end; i >= start; i--)
        {
            if (m_stcMLC.m_pLeaf[i].min_w - maxY >= -1e-6)   // Above the jaw top
                continue;
            if (m_stcMLC.m_pLeaf[i].max_w - minY <= 1e-6)   // Below the jaw bottom
                break;

            point.x = TPS_CLAMP(m_stcMLC.m_pLeaf[i].pos_2, minX, maxX);
            point.y = TPS_CLAMP(m_stcMLC.m_pLeaf[i].max_w, minY, maxY);
            vecPtsTemp.push_back(point);
            point.y = TPS_CLAMP(m_stcMLC.m_pLeaf[i].min_w, minY, maxY);
            vecPtsTemp.push_back(point);
        }
    }
    else {
        for (int i = start; i <= end; i++)
        {
            if (m_stcMLC.m_pLeaf[i].max_w - minX <= 1e-6)   // Below the jaw bottom
                continue;
            if (m_stcMLC.m_pLeaf[i].min_w - maxX >= -1e-6)   // Above the jaw top
                break;

            point.x = TPS_CLAMP(m_stcMLC.m_pLeaf[i].min_w, minX, maxX);
            point.y = TPS_CLAMP(m_stcMLC.m_pLeaf[i].pos_1, minY, maxY);
            vecPtsTemp.push_back(point);
            point.x = TPS_CLAMP(m_stcMLC.m_pLeaf[i].max_w, minX, maxX);
            vecPtsTemp.push_back(point);
        }

        for (int i = end; i >= start; i--)
        {
            if (m_stcMLC.m_pLeaf[i].min_w - maxX >= -1e-6)   // Above the jaw top
                continue;
            if (m_stcMLC.m_pLeaf[i].max_w - minX <= 1e-6)   // Below the jaw bottom
                break;

            point.x = TPS_CLAMP(m_stcMLC.m_pLeaf[i].max_w, minX, maxX);
            point.y = TPS_CLAMP(m_stcMLC.m_pLeaf[i].pos_2, minY, maxY);
            vecPtsTemp.push_back(point);
            point.x = TPS_CLAMP(m_stcMLC.m_pLeaf[i].min_w, minX, maxX);
            vecPtsTemp.push_back(point);
        }
    }

    //exclude points on same line
    Viewer3D::Point3D point0, point1, point2;
    Viewer3D::Vector3D vec0, vec1;
    for (int i = 0; i < vecPtsTemp.size(); ++i)
    {
        point0 = i == 0 ? vecPtsTemp.back() : vecPtsTemp[i - 1];
        point1 = vecPtsTemp[i];
        point2 = i == vecPtsTemp.size() - 1 ? vecPtsTemp.front() : vecPtsTemp[i + 1];
        vec0 = point1 - point0;
        vec1 = point2 - point1;
        if (vec0.Magnitude() <= 1e-6 || vec1.Magnitude() <= 1e-6 || 
            fabs(vec0.AngleBetween(vec1)) <= 1e-6)
        {
            continue;
        }
        ptList->push_back(point1);
    }
    return true;
}

// void TpsSegmentGraphicObject::SetBeamOutline(std::vector<Viewer3D::Point3D>& vecPoint3D) {
//     this->mOutLine = vecPoint3D;
// }


void TpsSegmentGraphicObject::GetJawPosition(float *jaw) 
{
    memcpy(jaw, m_stcMLC.m_aJawPosition, 4 * sizeof(float));
}

StcMLC* TpsSegmentGraphicObject::GetMLC()
{ 
    return &m_stcMLC;
}

void TpsSegmentGraphicObject::SetLeafNum(int nLeafNum)
{
    m_stcMLC.m_nLeafNum = nLeafNum;
}

void TpsSegmentGraphicObject::SetLeafPosition(int nLeafIndex, 
                                           float fPos1, float fPos2, 
                                           float fWidthMin, float fWidthMax)
{
    if (nLeafIndex < StcMLC::MAX_LEAF_NUM)
    {
        m_stcMLC.m_pLeaf[nLeafIndex].pos_1 = fPos1;
        m_stcMLC.m_pLeaf[nLeafIndex].pos_2 = fPos2;
        m_stcMLC.m_pLeaf[nLeafIndex].min_w = fWidthMin;
        m_stcMLC.m_pLeaf[nLeafIndex].max_w = fWidthMax;
        if (m_stcMLC.m_nLeafNum < nLeafIndex + 1)
        {
            m_stcMLC.m_nLeafNum = nLeafIndex + 1;  // because zero based array
        }
    }
}


//bool TpsSegmentGraphicObject::GetEditingOutline(std::vector<Viewer3D::Point3D> *pEditingOutline) {
//    if(nullptr == pEditingOutline)
//        return false;
//
//    pEditingOutline->clear();
//    for(auto itr = m_vecEditingOutline.begin(); itr != m_vecEditingOutline.end(); ++itr) {
//        pEditingOutline->push_back(*itr);
//    }
//    return true;
//}
//
//void TpsSegmentGraphicObject::SetEditingOutline(std::vector<Viewer3D::Point3D>& vecEditingOutline) {
//    this->m_vecEditingOutline = vecEditingOutline;
//    if (vecEditingOutline.size() > 0)
//        this->m_nIsEditingOutline = false;
//}
//
//void TpsSegmentGraphicObject::SetAddedMlcContour(std::vector<Viewer3D::Point3D>& vAddedPoints) {
//    mVec_AddedMlcContour = vAddedPoints;
//    if (vAddedPoints.size() > 0)
//        m_nIsEditingOutline = true;
//}
//
//void TpsSegmentGraphicObject::FinishEditingOutline()
//{
//    this->m_nIsEditingOutline = false;
//}

//void TpsSegmentGraphicObject::SetPickCoordinate(double x, double y)
//{
//    m_stcMLC.m_dPickX = x;
//    m_stcMLC.m_dPickY = y;
//    m_stcMLC.m_nSelectionMode = MLC_SELECTING;  // selecting
//}
//bool TpsSegmentGraphicObject::GetPickCoordinate(double* x, double* y)
//{
//    if (x)
//        *x = m_stcMLC.m_dPickX;
//    if (y)
//        *y = m_stcMLC.m_dPickY;
//    return true;
//}
//
//void TpsSegmentGraphicObject::MovingMLC(double targetX, double targetY)
//{
//    if(m_stcMLC.m_nSelectedLeaf < 0 || m_stcMLC.m_nSelectedLeaf >= m_stcMLC.m_nLeaf)
//        return;
//    m_stcMLC.m_dTargetX = targetX;
//    m_stcMLC.m_dTargetY = targetY;
//
//    m_stcMLC.m_nSelectionMode = MLC_DRAGGING;  // dragging
//}
//
//void TpsSegmentGraphicObject::FinishMovingMLC()
//{
//    m_stcMLC.m_nSelectionMode = MLC_FINISHED; // finished
//}

float TpsSegmentGraphicObject::GetCollimatorAngle() const {
    return mCollimatorAngle;
}

void TpsSegmentGraphicObject::SetCollimatorAngle(const float &angle) {
    mCollimatorAngle = angle;
}

float TpsSegmentGraphicObject::GetStartGantryAngle() const {
    return mStartGantryAngle;
}

void TpsSegmentGraphicObject::SetStartGantryAngle(const float &angle) {
    mStartGantryAngle = angle;
}

void TpsSegmentGraphicObject::SetGantryArcLength(const float & length)
{
    mGantryArcLength = length;
}

float TpsSegmentGraphicObject::GetGantryArcLength() const
{
    return mGantryArcLength;
}

void TpsSegmentGraphicObject::SetGantryIsClockWise(bool bIsClockWise){
    mIsClockWise = bIsClockWise;
}
bool TpsSegmentGraphicObject::GetGantryIsClockWise() const{
    return mIsClockWise;
}

//void TpsSegmentGraphicObject::SetMlcApertureCache(const std::vector<TpsMlcAperture>& vAperture){
//    mMlcApertureCache = vAperture;
//}
//
//void TpsSegmentGraphicObject::GetMlcApertureCache(std::vector<TpsMlcAperture>& aperture) const{
//    aperture = mMlcApertureCache;
//}
//
//void TpsSegmentGraphicObject::ClearMlcApertureCache(){
//    for(int i = 0; i < mMlcApertureCache.size(); ++i){
//        mMlcApertureCache[i].mPoint3DList.clear();
//    }
//    mMlcApertureCache.clear();
//}
//
//void TpsSegmentGraphicObject::FinishEditingMlcAperture(){
//    m_vecEditingOutline.clear();
//    for(int i = 0; i < mMlcApertureCache.size(); ++i){
//        for(int j = 0; j < mMlcApertureCache[i].mPoint3DList.size(); ++j)
//        m_vecEditingOutline.push_back(mMlcApertureCache[i].mPoint3DList[j]);
//    }
//}
//
//void TpsSegmentGraphicObject::SetMaskBuffer(unsigned char* mask, int width, int height){
//    if (width * height != mWidth * mHeight){
//        delete[] mMaskBuffer;
//        mMaskBuffer = new unsigned char[width * height];
//    }
//
//    mWidth = width;
//    mHeight = height;
//    memcpy(mMaskBuffer, mask, width * height * sizeof(unsigned char));
//
//    mActiveBuffType = BUFF_TYPE_MASK;
//
//}

//void TpsSegmentGraphicObject::SetPrevPoint2D(double x, double y){
//    mPrevPoint2D.x = x;
//    mPrevPoint2D.y = y;
//}
//
//void TpsSegmentGraphicObject::GetPrevPoint2D(Viewer3D::Point2D& prevPoint2D) const{
//    prevPoint2D = mPrevPoint2D;
//}
//
//void TpsSegmentGraphicObject::SetNudgeCircle(const std::vector<Viewer3D::Point2D>& circle){
//    mv_NudgeCircle = circle;
//}
//
//void TpsSegmentGraphicObject::GetNudgeCircle(std::vector<Viewer3D::Point2D>& circle) const{
//    circle = mv_NudgeCircle;
//}
//
//void TpsSegmentGraphicObject::ClearNudgeCircle(){
//    mv_NudgeCircle.clear();
//}

//void TpsSegmentGraphicObject::GetJawContour(std::vector<Viewer3D::Point3D>& jawContour){
//    for(int i= 0; i< m_stcMLC.m_nLeaf; ++i){
//        Viewer3D::Point3D p1;
//        p1.x = m_stcMLC.m_pLeaf[i].pos_1;
//        p1.y = m_stcMLC.m_pLeaf[i].pos_2;
//        p1.z = -1000;
//        jawContour.push_back(p1);
//    }
//    for(int i= m_stcMLC.m_nLeaf - 1; i >= 0 ; --i){
//        Viewer3D::Point3D p2;
//        p2.x = m_stcMLC.m_pLeaf[i].pos_1;
//        p2.y = m_stcMLC.m_pLeaf[i].pos_2;
//        p2.z = -1000;
//        jawContour.push_back(p2);
//    }
//}

//void TpsSegmentGraphicObject::SetInOutNudgeAction(float centerX, int lowCenter){
//    if(centerX < m_stcMLC.m_pLeaf[(int)(0.1*(lowCenter + 200))].pos_1){ // push inward
//        mIsNudgeInwardFromRight = false;
//        mIsNudgeInwardFromLeft = true;
//    }else if(centerX > m_stcMLC.m_pLeaf[(int)(0.1 * (lowCenter + 200))].pos_2){
//        mIsNudgeInwardFromLeft = false;
//        mIsNudgeInwardFromRight = true;
//    }else{
//        mIsNudgeInwardFromLeft = false;
//        mIsNudgeInwardFromRight = false;
//    }
//    return;
//}

//void TpsSegmentGraphicObject::CollisionDetection(const std::vector<std::pair<int, float>>& leftLeaves, 
//    const std::vector<std::pair<int, float>>& rightLeaves){
//
//    if(mIsNudgeInwardFromLeft){
//        for(int i = 0; i < rightLeaves.size(); ++i){
//            if(m_stcMLC.m_pLeaf[rightLeaves[i].first].pos_1 < rightLeaves[i].second){
//                if(rightLeaves[i].second >= 200.f){
//                    m_stcMLC.m_pLeaf[rightLeaves[i].first].pos_1 = 199.f;
//                }else{
//                    m_stcMLC.m_pLeaf[rightLeaves[i].first].pos_1 = rightLeaves[i].second;
//                }
//            }
//            if(m_stcMLC.m_pLeaf[rightLeaves[i].first].pos_2 < rightLeaves[i].second){
//                if(rightLeaves[i].second >= 200){
//                    m_stcMLC.m_pLeaf[rightLeaves[i].first].pos_2 = 200.f;
//                }else{
//                    m_stcMLC.m_pLeaf[rightLeaves[i].first].pos_2 = rightLeaves[i].second+1;
//                }
//            }
//        }
//    }else if(mIsNudgeInwardFromRight){
//        for(int i = 0; i < leftLeaves.size(); ++i){
//            if(m_stcMLC.m_pLeaf[leftLeaves[i].first].pos_2 > leftLeaves[i].second){
//                if(leftLeaves[i].second <= -200){
//                    m_stcMLC.m_pLeaf[leftLeaves[i].first].pos_2 = -199.f;
//                }else{
//                    m_stcMLC.m_pLeaf[leftLeaves[i].first].pos_2 = leftLeaves[i].second;
//                }
//            }
//            if(m_stcMLC.m_pLeaf[leftLeaves[i].first].pos_1 > leftLeaves[i].second){
//                if(leftLeaves[i].second <= -200){
//                    m_stcMLC.m_pLeaf[leftLeaves[i].first].pos_1 = -200.f;
//                }else{
//                    m_stcMLC.m_pLeaf[leftLeaves[i].first].pos_1 = leftLeaves[i].second-1;
//                }
//            }
//        }
//    }else{
//        for(int i = 0; i < leftLeaves.size(); ++i){
//            if(m_stcMLC.m_pLeaf[leftLeaves[i].first].pos_1 > leftLeaves[i].second){
//                if(leftLeaves[i].second <= -200){
//                    m_stcMLC.m_pLeaf[leftLeaves[i].first].pos_1 = -200.f;
//                }else{
//                    m_stcMLC.m_pLeaf[leftLeaves[i].first].pos_1 = leftLeaves[i].second;
//                }
//            }
//            if(m_stcMLC.m_pLeaf[rightLeaves[i].first].pos_2 < rightLeaves[i].second){
//                if(rightLeaves[i].second >= 200){
//                    m_stcMLC.m_pLeaf[rightLeaves[i].first].pos_2 = 199.f;
//                }else{
//                    m_stcMLC.m_pLeaf[rightLeaves[i].first].pos_2 = rightLeaves[i].second;
//                }
//            }
//        }
//    }
//}
void TpsSegmentGraphicObject::SetWeight(float weight){
    mWeight = weight;
}

float TpsSegmentGraphicObject::GetWeight() const{
    return mWeight;
}

void TpsSegmentGraphicObject::SetMU(float mu){
    mMU = mu;
}

float TpsSegmentGraphicObject::GetMU() const{
    return mMU;
}

//void TpsSegmentGraphicObject::SetSeletedLeaft(int index, float pos){
//    mSelectedLeaf = index;
//	m_stcMLC.m_nSelectedLeaf = index;
//
//
//    if(m_stcMLC.m_pLeaf[mSelectedLeaf].pos_1 > pos){
//        mSelectLeftOrRightLeaf = 1;
//    }
//    else if(m_stcMLC.m_pLeaf[mSelectedLeaf].pos_2 < pos){
//        mSelectLeftOrRightLeaf = 2;
//    }else{
//        mSelectLeftOrRightLeaf = 0;
//    }
//
//    if(mSelectLeftOrRightLeaf == 1){
//        if(mOldPos<2){
//            if(pos > 2 && pos <= 10){
//                m_stcMLC.m_pLeaf[mSelectedLeaf].pos_1 = pos;
//            }
//        }
//    }
//    else if(mSelectLeftOrRightLeaf == 2){
//        if(mOldPos>198){
//            if(pos > 190 && pos <= 198){
//                m_stcMLC.m_pLeaf[mSelectedLeaf].pos_2 = pos;
//            }
//        }
//    }
//    mOldPos = pos;
//
//    //printf("%d is set\n", mSelectedLeaf);
//
//}
//void TpsSegmentGraphicObject::DragSeletedLeaft(float pos){
//    if(mSelectLeftOrRightLeaf == 1){
//        m_stcMLC.m_pLeaf[mSelectedLeaf].pos_1 += pos - mOldPos;
//        if(m_stcMLC.m_pLeaf[mSelectedLeaf].pos_2<m_stcMLC.m_pLeaf[mSelectedLeaf].pos_1){
//            m_stcMLC.m_pLeaf[mSelectedLeaf].pos_2 = m_stcMLC.m_pLeaf[mSelectedLeaf].pos_1;
//            if(m_stcMLC.m_pLeaf[mSelectedLeaf].pos_1 > 200){
//                m_stcMLC.m_pLeaf[mSelectedLeaf].pos_2 = 200;
//                m_stcMLC.m_pLeaf[mSelectedLeaf].pos_1 = 200;
//            }
//        }
//    }else if(mSelectLeftOrRightLeaf == 2){
//        m_stcMLC.m_pLeaf[mSelectedLeaf].pos_2 += pos - mOldPos;
//        if(m_stcMLC.m_pLeaf[mSelectedLeaf].pos_1>m_stcMLC.m_pLeaf[mSelectedLeaf].pos_2){
//            m_stcMLC.m_pLeaf[mSelectedLeaf].pos_1 = m_stcMLC.m_pLeaf[mSelectedLeaf].pos_2;
//            if(m_stcMLC.m_pLeaf[mSelectedLeaf].pos_2 < -200){
//                m_stcMLC.m_pLeaf[mSelectedLeaf].pos_1 = -200;
//                m_stcMLC.m_pLeaf[mSelectedLeaf].pos_2 = -200;
//            }
//        }
//    }else{
//        return;
//    }
//    mOldPos = pos;
//}
//
//void TpsSegmentGraphicObject::Flip(int flipXY){
//    switch(flipXY){
//    case 0:
//        for(int i = 0; i < m_stcMLC.m_nLeaf; ++i){
//            int tmp = m_stcMLC.m_pLeaf[i].pos_1;
//            m_stcMLC.m_pLeaf[i].pos_1 = -m_stcMLC.m_pLeaf[i].pos_2;
//            m_stcMLC.m_pLeaf[i].pos_2 = -tmp;
//        }
//        break;
//    case 1:
//        for(int i = 0; i <= m_stcMLC.m_nLeaf/2; ++i){
//            int tmp = m_stcMLC.m_pLeaf[i].pos_1;
//            m_stcMLC.m_pLeaf[i].pos_1 = m_stcMLC.m_pLeaf[m_stcMLC.m_nLeaf-1-i].pos_1;
//            m_stcMLC.m_pLeaf[m_stcMLC.m_nLeaf-1-i].pos_1 = tmp;
//        }
//        for(int i = 0; i <= m_stcMLC.m_nLeaf/2; ++i){
//            int tmp = m_stcMLC.m_pLeaf[i].pos_2;
//            m_stcMLC.m_pLeaf[i].pos_2 = m_stcMLC.m_pLeaf[m_stcMLC.m_nLeaf-1-i].pos_2;
//            m_stcMLC.m_pLeaf[m_stcMLC.m_nLeaf-1-i].pos_2 = tmp;
//        }
//        break;
//    }
//}

std::string TpsSegmentGraphicObject::GetUID() const{
    return mUID;
}

void TpsSegmentGraphicObject::SetUID(const std::string& uid){
    mUID = uid;
}

void TpsSegmentGraphicObject::Clear() {
	
}

void TpsSegmentGraphicObject::SetLeafDirection(LEAF_DIRECTION dir){
    m_stcMLC.m_eLeafDirection = dir;
}

LEAF_DIRECTION TpsSegmentGraphicObject::GetLeafDirection() const{
    return m_stcMLC.m_eLeafDirection;
}

void TpsSegmentGraphicObject::Set3DSegmentPtList(const std::vector<Viewer3D::Point3D>& ptList)
{
    m3DSegmentPtList = ptList;
}

std::vector<Viewer3D::Point3D> TpsSegmentGraphicObject::Get3DSegmentPtList()
{
    return m3DSegmentPtList;
}

void TpsSegmentGraphicObject::SetMiniLeafGap(float miniGap)
{
    m_stcMLC.m_fMiniGap = miniGap;
}

float TpsSegmentGraphicObject::GetMiniLeafGap() const
{
    return m_stcMLC.m_fMiniGap;
}

void TpsSegmentGraphicObject::SetCollimatorDisplayRange(const float* range)
{
    if (range == nullptr) return;
    memcpy(mCollimatorDisplayRange, range, 4 * sizeof(float));
}

void TpsSegmentGraphicObject::GetCollimatorDisplayRange(float* range) const
{
    if (range == nullptr) return;
    memcpy(range, mCollimatorDisplayRange, 4 * sizeof(float));
}
TPS_END_NAMESPACE
