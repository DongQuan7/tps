//////////////////////////////////////////////////////////////////////////
/// \defgroup TDA of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  wenlong.zhao mailto:wenlong.zhao@united-imaging.com
///
///  \file    tps_rl_easyplan_measuregraphicobject.h
///  \brief   tps easyplan measure graphic object class
///
///  \version 1.0
///  \date    2014-11-08
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANMEASUREGRAPHICOBJECT_H_
#define TPS_RL_EASYPLANMEASUREGRAPHICOBJECT_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
// Mcsf
#include "McsfMedViewer3DArithmetic/point2d.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC EasyPlanMeasureGraphicObject : public TpsGraphicObjectBase  {
public:
    EasyPlanMeasureGraphicObject(void);
    virtual ~EasyPlanMeasureGraphicObject(void);
    virtual bool Initialize();
    virtual bool Finalize();
    //virtual bool IsToUpdate() const ;
    
    // for measure distance operation
    void SetMeasureDistanceStartPoint(float startX, float startY);
    void GetMeasureDistanceStartPoint(float *startX, float *startY);
    void SetMeasureDistanceStopPoint(float stopX, float stopY);
    void GetMeasureDistanceStopPoint(float *stopX, float *stopY);
    void SetMeasureDistanceActionStatus(bool isActionStarted, bool isActionDone);
    void GetMeasureDistanceActionStatus(bool *isActionStarted, bool *isActionDone);

    // for measure angle operation
    void SetMeasureAnglePoints(std::list<Mcsf::MedViewer3D::Point2D> anglePoints);
    void GetMeasureAnglePoints(std::list<Mcsf::MedViewer3D::Point2D>& anglePoints) const;
    void SetMeasureAngleCursorPoint(float cursorX, float cursorY);
    void GetMeasureAngleCursorPoint(float *cursorX, float *cursorY);
    void SetMeasureAngleActionStatus(bool isActionStarted, bool isFirstSideDone, bool isSecondSideDone);
    void GetMeasureAngleActionStatus(bool *isActionStarted, bool *isFirstSideDone, bool *isSecondSideDone);

private:
    // for measure distance
    float                                   mRulerStartX;
    float                                   mRulerStartY;
    float                                   mRulerStopX;
    float                                   mRulerStopY;
    bool                                    mRulerIsActionStarted;
    bool                                    mRulerIsActionDone;

    // for measure angle
    float                                   mProtractorCursorX;
    float                                   mProtractorCursorY;
    std::list<Mcsf::MedViewer3D::Point2D>        mProtractorAnglePoints;
    bool                                    mProtractorActionStarted;
    bool                                    mProtractorFirstSideDone;
    bool                                    mProtractorSecondSideDone;

};
TPS_END_NAMESPACE
#endif