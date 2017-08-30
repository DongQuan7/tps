////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file rt_tps_certifiedmouseinfo.cpp
/// 
///  \brief class CertifiedMouseInfo implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/05/29
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifiedmouseinfo.h"
#include "RtTpsProtoLib/rt_tps_mouse_info.pb.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

void CertifiedMouseInfo::Convert(const proto::RT_TPS_MOUSE_INFO& proto) {
    mX = proto.x();
    mY = proto.y();
    mActionType = proto.mouseactiontype();
    mViewerControlID = proto.viewercontrolid();
    mCellID = proto.cellid();
}

void CertifiedMouseInfo::ConvertBack(proto::RT_TPS_MOUSE_INFO* proto) const{
    proto->set_x(mX);
    proto->set_y(mY);
    proto->set_mouseactiontype(mActionType);
    proto->set_viewercontrolid(mViewerControlID);
    proto->set_cellid(mCellID);
}

TPS_END_NAMESPACE  // end namespace tps
