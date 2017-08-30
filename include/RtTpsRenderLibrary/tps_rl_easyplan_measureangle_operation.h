////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wenlong.Zhao wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_measureangle_operation.h
/// 
///  \brief class TpsEasyPlanMeasureAngleOperation
/// 
///  \version 1.0
/// 
///  \date    2014/11/11
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANMEASUREANGLEOPERATION_H_
#define TPS_RL_EASYPLANMEASUREANGLEOPERATION_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

//MCSF
#include "McsfMedViewer3DArithmetic/point2d.h"
#include <list>

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsEasyPlanMeasureAngleOperation
    : public OperationBase {
public:
    TpsEasyPlanMeasureAngleOperation(const std::string &planUID, const std::string &drrImageUID, 
        const std::list<Mcsf::MedViewer3D::Point2D>& anglePoints, const float cursorX, const float cursorY,
        const bool actionStarted, const bool firstSideDone, const bool secondSideDone);
    ~TpsEasyPlanMeasureAngleOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string                         mPlanUID;
    std::string                         mDrrImageUID;
    std::list<Mcsf::MedViewer3D::Point2D>    mAnglePoints;
    float                               mCursorX;
    float                               mCursorY;
    bool                                mActionStarted;
    bool                                mFirstSideDone;
    bool                                mSecondSideDone;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanMeasureAngleOperation);
};
TPS_END_NAMESPACE

#endif