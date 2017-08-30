////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_segmentgraphicobject.h
/// 
///  \brief class TpsSegmentGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/26
////////////////////////////////////////////////////////////////


#ifndef TPS_RL_SEGMENTGRAPHICOBJECT_H
#define TPS_RL_SEGMENTGRAPHICOBJECT_H

#include "tps_rl_defs.h"

#include "McsfMedViewer3DArithmetic/arithmetic.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsAlgorithmProxy/tps_pl_collimator.h"
#include "RtTpsAlgorithmProxy/tps_pl_datastructure.h"
#include <vector>

namespace Viewer3D = Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

struct TpsMlcAperture{
    std::vector<Mcsf::MedViewer3D::Point3D> mPoint3DList;
    Mcsf::MedViewer3D::Point3D mCandidatePoint;
    bool hasCandidate;
};

enum LEAF_DIRECTION{
    LEAF_DIRECTION_X,
    LEAF_DIRECTION_Y
};

typedef enum{
    BUFF_TYPE_APERTURE,
    BUFF_TYPE_MASK,
}BUFF_TYPE;

typedef enum{
    MLC_IDLE,
    MLC_SELECTING,
    MLC_DRAGGING,
    MLC_FINISHED
}SELECTION_MODE;

struct StcMLC
{
    const static int                MAX_LEAF_NUM = 200;//当前叶片最多为200对，与ZYG确认 2016-7-5
    plunc::LEAF_POSITION_STC        m_pLeaf[MAX_LEAF_NUM];
    int                             m_nLeafNum;
    LEAF_DIRECTION                  m_eLeafDirection;
    float                           m_fMiniGap;
    //x1, x2, y1, y2
    float                           m_aJawPosition[4];
//     int                             m_nSelectedLeaf;
//     int                             m_nSelectedRightEdge;   // 0: left edge is selected, 1: right edge
// 
//     SELECTION_MODE                  m_nSelectionMode;       // 0: idle 1: selecting 2: dragging 3: finished
//     double                          m_dPickX;               // mouse position to pick the MLC (screen cs)
//     double                          m_dPickY;
//     double                          m_dTargetX;             // the target mouse position when dragging to edit MLC (screen cs)
//     double                          m_dTargetY;
// 
//     float                           m_fColorR;
//     float                           m_fColorG;
//     float                           m_fColorB;
//     float                           m_fSelectedColorR;
//     float                           m_fSelectedColorG;
//     float                           m_fSelectedColorB;
};

class TPS_RL_DECLSPEC TpsSegmentGraphicObject : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    TpsSegmentGraphicObject();
    virtual ~TpsSegmentGraphicObject();

    //////////////////////////////////////////////////////////////////////////
    //\brief virtual methods derived from GraphicObiBase;
    virtual bool Initialize();
    virtual bool Finalize();

    void SetStartCollimatorToPatMatrix(const Viewer3D::Matrix4x4& matrix);

    void GetStartCollimatorToPatMatrix(Viewer3D::Matrix4x4* matrix);

    void SetEndCollimatorToPatMatrix(const Viewer3D::Matrix4x4& matrix);

    void GetEndCollimatorToPatMatrix(Viewer3D::Matrix4x4* matrix);

    //////////////////////////////////////////////////////////////////////////
    // \brief set mlc visible flag;
    // \param[in] mlc visible flag, true mlc is visible;
    void SetMlcVisibleFlag(bool);

    bool GetMlcVisibleFlag();
    //////////////////////////////////////////////////////////////////////////
    // \brief set the jaw visible flag;
    // \param[in] jaw visible flag, true jaw is visible;
    void SetJawVisibleFlag(bool);

    bool GetJawVisibleFlag();
    //////////////////////////////////////////////////////////////////////////
    // brief set the active flag;
    // \param[in] active flag,true the beam is selected and would not be displayed in the bev window
    void SetActiveFlag(bool);

    bool GetActiveFlag();

    bool GetOutLine(std::vector<Viewer3D::Point3D> *);

    //void SetBeamOutline(std::vector<Viewer3D::Point3D>& vecPoint3D);

    void SetJawPosition(float *);

    void GetJawPosition(float *);

    void SetCollimatorAngle(const float &);

    float GetCollimatorAngle() const;

    void SetStartGantryAngle(const float &);

    float GetStartGantryAngle() const;

    void SetGantryArcLength(const float &);

    float GetGantryArcLength() const;

    void SetGantryIsClockWise(bool bIsClockWise);
    bool GetGantryIsClockWise() const;

    // MLC accessor and manipulator

    StcMLC* GetMLC();

    void SetLeafNum(int nLeafNum);

    void SetLeafPosition(int nLeafIndex, float fPos1, float fPos2, float fWidthMin, float fWidthMax);


    // MLC selecting and editing
//     void SetEditingOutline(std::vector<Viewer3D::Point3D>& vecPoint3D);
//     bool GetEditingOutline(std::vector<Viewer3D::Point3D>* pEditingOutline);

//     inline bool  IsEditingOutline() { return m_nIsEditingOutline; }
//     inline void SetIsEditingStarted(bool isStarted){ m_nIsEditingOutline = isStarted;}

    //void FinishEditingOutline();

    /*void SetPickCoordinate(double x, double y);
    bool GetPickCoordinate(double* x, double* y);
    void MovingMLC(double targetX, double targetY);
    void FinishMovingMLC();*/

    //void SetAddedMlcContour(std::vector<Viewer3D::Point3D>& vAddedPoints);

   /* void SetMlcApertureCache(const std::vector<TpsMlcAperture>& vAperture);
    void GetMlcApertureCache(std::vector<TpsMlcAperture>& vAperture) const;
    void ClearMlcApertureCache();
    void FinishEditingMlcAperture();*/

    /*void SetMaskBuffer(unsigned char* mask, int widht, int height);
    void GetMaskBuffer(unsigned char*& mask, int& width, int& height) const{
        mask = mMaskBuffer, width = mWidth, height = mHeight;
    }*/

//     void SetPrevPoint2D(double x, double y);
//     void GetPrevPoint2D(Viewer3D::Point2D& prevPt2D) const;
    
//     inline void SetIsContourShow(bool isShow){ mIsContourShow = isShow;}
//     inline bool GetIsContourShow(){ return mIsContourShow;}

//     void SetNudgeCircle(const std::vector<Viewer3D::Point2D>& circle);
//     void GetNudgeCircle(std::vector<Viewer3D::Point2D>& circle) const;
//     void ClearNudgeCircle();
//     void GetJawContour(std::vector<Viewer3D::Point3D>& contour);
//     void SetInOutNudgeAction(float centerX, int LeafBottomLineCoord);
//     void CollisionDetection(const std::vector<std::pair<int, float>>& leftLeaves, 
//                             const std::vector<std::pair<int, float>>& rightLeaves);

    void SetWeight(float weight);
    float GetWeight() const;

    void SetMU(float mu);
    float GetMU() const;

//     void SetSeletedLeaft(int, float);
//     void DragSeletedLeaft(float);

    //void Flip(int flipXY);

    std::string GetUID() const;
    void SetUID(const std::string& uid);
    void Clear();

    void SetLeafDirection(LEAF_DIRECTION dir);
    LEAF_DIRECTION GetLeafDirection() const;

    void Set3DSegmentPtList(const std::vector<Viewer3D::Point3D>& ptList);
    std::vector<Viewer3D::Point3D> Get3DSegmentPtList();

    void SetCollimatorDisplayRange(const float* range);
    void GetCollimatorDisplayRange(float* range) const;

    void SetMiniLeafGap(float miniGap);
    float GetMiniLeafGap() const;
private:
    void ComputeOutLine_i();

private:
    std::string                                 mUID;
    Viewer3D::Matrix4x4                         mStartCollimator2PatMatrix;
    Viewer3D::Matrix4x4                         mEndCollimator2PatMatrix;
    float                                       mColor[4];
    bool                                        mIsMlcVisible;
    bool                                        mIsJawVisible;
    bool                                        mIsActive;
    std::vector<Viewer3D::Point3D>              mOutLine;
    bool                                        mMlcOrientation;
    bool                                        mNeedReCalculate;
    float                                       mCollimatorAngle;
    float                                       mStartGantryAngle;
    float                                       mGantryArcLength;
    bool                                        mIsClockWise;
    //BUFF_TYPE                                   mActiveBuffType;
    //int                                         mWidth, mHeight;
    //unsigned char*                              mMaskBuffer;
    //bool                                        mIsContourShow;
    //std::vector<Viewer3D::Point2D>              mv_NudgeCircle;
    //int                                         mNudgeCircleOct[8];
    std::vector<Viewer3D::Point3D>              m3DSegmentPtList;
    float                                       mCollimatorDisplayRange[4];
private:
    StcMLC m_stcMLC;

    //bool                                           m_nIsEditingOutline;
    //std::vector<Viewer3D::Point3D>              m_vecEditingOutline;    // screen coordinates
    //std::vector<Viewer3D::Point3D>              mVec_AddedMlcContour;

    //Viewer3D::Point2D                            mPrevPoint2D;

    //std::vector<TpsMlcAperture>                 mMlcApertureCache;
    //std::vector<Viewer3D::Point3D>              mJawContour;

    //bool                                           mIsNudgeInwardFromLeft;
    //bool                                           mIsNudgeInwardFromRight;
    //int                                            mSelectLeftOrRightLeaf;
    //int                                            mSelectedLeaf;
    //float                                          mOldPos;
    float                       mWeight;
    float                       mMU;
// public:
//     // for debug only
//     std::vector<Viewer3D::Point3D>              m_vecTestPoints;
};
TPS_END_NAMESPACE

#endif
