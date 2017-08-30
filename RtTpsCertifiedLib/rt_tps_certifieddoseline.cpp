////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file rt_tps_certifieddoseline.cpp
/// 
///  \brief class CertifiedDoseLine implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/09/29
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifieddoseline.h"
#include "RtTpsProtoLib/rt_tps_proto_doseline.pb.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

void CertifiedDoseLine::Convert(const proto::RtTpsProtoDoseLine &doseLine){
    mUid = doseLine.doselineuid();
    mDoseRatio = doseLine.doseratio();
    mColor[0] = doseLine.doselinecolor(0);
    mColor[1] = doseLine.doselinecolor(1);
    mColor[2] = doseLine.doselinecolor(2);
    mColor[3] = doseLine.doselinecolor(3);
    mIsVisible = doseLine.isvisible();
    mPlanUid = doseLine.planuid();
}

void CertifiedDoseLine::ConvertBack(proto::RtTpsProtoDoseLine *protoLine) const{
    protoLine->set_doselineuid(mUid);
    protoLine->set_doseratio(mDoseRatio);
    protoLine->clear_doselinecolor();
    protoLine->add_doselinecolor(mColor[0]);
    protoLine->add_doselinecolor(mColor[1]);
    protoLine->add_doselinecolor(mColor[2]);
    protoLine->add_doselinecolor(mColor[3]);
    protoLine->set_isvisible(mIsVisible);
    protoLine->set_planuid(mPlanUid);
}

TPS_END_NAMESPACE  // end namespace tps
