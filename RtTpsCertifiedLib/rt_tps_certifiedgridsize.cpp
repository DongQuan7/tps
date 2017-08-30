////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  ZUO Kai  kai.zuo@united-imaging.com
/// 
///  \file rt_tps_certifiedgridsize.cpp
/// 
///  \brief struct Certifiedgridsize implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/04/09
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifiedgridsize.h"
#include "RtTpsProtoLib/rt_tps_gridsize.pb.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

void CertifiedGridSize::Convert(const proto::RT_TPS_GridSize& protoGridSize) {
    mx = protoGridSize.x();
    my = protoGridSize.y();
    mz = protoGridSize.z();
    mstartx = protoGridSize.startx();
    mstarty = protoGridSize.starty();
    mstartz = protoGridSize.startz();
    msizex = protoGridSize.sizex();
    msizey = protoGridSize.sizey();
    msizez = protoGridSize.sizez();
    mPlanUid = protoGridSize.plan_uid();
    mNormGroup = protoGridSize.normgroup_uid();
    if (protoGridSize.has_is_show_gridboundary()){
        mIsShowBoundary = protoGridSize.is_show_gridboundary();
    }
}

void CertifiedGridSize::ConvertBack(proto::RT_TPS_GridSize* protoGridSize) {
    protoGridSize->set_x(mx);
    protoGridSize->set_y(my);
    protoGridSize->set_z(mz);
    protoGridSize->set_sizex(msizex);
    protoGridSize->set_sizey(msizey);
    protoGridSize->set_sizez(msizez);
    protoGridSize->set_startx(mstartx);
    protoGridSize->set_starty(mstarty);
    protoGridSize->set_startz(mstartz);
    protoGridSize->set_plan_uid(mPlanUid);
    protoGridSize->set_is_show_gridboundary(mIsShowBoundary);
    protoGridSize->set_normgroup_uid(mNormGroup);
}

TPS_END_NAMESPACE  // end namespace tps