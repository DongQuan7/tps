////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file rt_tps_certifiedmovevoi.h
/// 
///  \brief struct CertifiedMoveVOI declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/6/19
////////////////////////////////////////////////////////////////


#ifndef RT_TPS_CERTIFIEDMOVEVOI_H_
#define RT_TPS_CERTIFIEDMOVEVOI_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE

namespace proto{
    class RT_TPS_MOVE_VOI;
}

struct CertifiedMoveVOI {
    int mWindowType;
    std::string mWindowUid;
    float  mPreCoordX;
    float  mPreCoordY;
    float  mCurCoordX;
    float  mCurCoordY;
    std::string mVoiUID;
    float mOffsetX;
    float mOffsetY;
    float mOffsetZ;
    bool mIsEditByOffset;
    bool mIsEditCurrentSlice;
    int mMouseActionType;
    bool mIsEditEnd;
    bool mIsShowWarning;

    void Convert(const proto::RT_TPS_MOVE_VOI &protoMoveVOI);

    void ConvertBack(proto::RT_TPS_MOVE_VOI *protoMoveVOI) const;
};

TPS_END_NAMESPACE

#endif