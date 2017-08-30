////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZUO Kai  kai.zuo@united-imaging.com
/// 
///  \file rt_tps_certifiedgridboundary.cpp
/// 
///  \brief struct Certifiedgridboundary implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/04/10
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifiedgridboundary.h"
#include "RtTpsProtoLib/rt_tps_gridboundary.pb.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

void CertifiedGridBoundary::Convert(const proto::RT_TPS_GridBoundary& protoGridBoundary) {
    mPlanUid = protoGridBoundary.plan_uid();
    mNormGroup = protoGridBoundary.normalgroup_uid();
    mActionType = protoGridBoundary.action();
    mWindowType = protoGridBoundary.windowtype();
    mWindowUid = protoGridBoundary.windowuid();
    mY = protoGridBoundary.y();
    mX = protoGridBoundary.x();
}

void CertifiedGridBoundary::ConvertBack(proto::RT_TPS_GridBoundary* protoGridBoundary) {
    protoGridBoundary->set_plan_uid(mPlanUid);
    protoGridBoundary->set_normalgroup_uid(mNormGroup);
    protoGridBoundary->set_action(mActionType);
    protoGridBoundary->set_windowtype(mWindowType);
    protoGridBoundary->set_windowuid(mWindowUid);
    protoGridBoundary->set_y(mY);
    protoGridBoundary->set_x(mX);
}

TPS_END_NAMESPACE  // end namespace tps