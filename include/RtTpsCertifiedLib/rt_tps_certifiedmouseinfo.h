////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file rt_tps_certifiedmouseinfo.h
/// 
///  \brief class CertifiedMouseInfo declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/05/29
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIEDMOUSEINFO_H_
#define RT_TPS_CERTIFIEDMOUSEINFO_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

namespace proto{
    class RT_TPS_MOUSE_INFO;
}

struct CertifiedMouseInfo{
    double mX;
    double mY;
    int mActionType;
    int mViewerControlID;
    int mCellID;

    void Convert(const proto::RT_TPS_MOUSE_INFO& proto);

    void ConvertBack(proto::RT_TPS_MOUSE_INFO* proto) const;
};

TPS_END_NAMESPACE  // end namespace tps

#endif
