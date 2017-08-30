////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file rt_tps_certifiedmovevoi.cpp
/// 
///  \brief struct CertifiedMoveVOI implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/6/19
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifiedmovevoi.h"
#include "RtTpsProtoLib/rt_tps_move_voi.pb.h"

TPS_BEGIN_NAMESPACE

void CertifiedMoveVOI::Convert(const proto::RT_TPS_MOVE_VOI &protoMoveVOI) {
    mVoiUID = protoMoveVOI.voiuid();
    mWindowType = protoMoveVOI.windowtype();
    mWindowUid = protoMoveVOI.windowuid();
    mPreCoordX = protoMoveVOI.precoordx();
    mPreCoordY = protoMoveVOI.precoordy();
    mCurCoordX = protoMoveVOI.curcoordx();
    mCurCoordY = protoMoveVOI.curcoordy();
    mOffsetX = protoMoveVOI.offsetx();
    mOffsetY = protoMoveVOI.offsety();
    mOffsetZ = protoMoveVOI.offsetz();
    mIsEditByOffset = protoMoveVOI.movedbyoff();
    mIsEditCurrentSlice = protoMoveVOI.ispancurrentslice();
    mMouseActionType = protoMoveVOI.mouseactiontype();
    mIsShowWarning = protoMoveVOI.isshowwarning();
    mIsEditEnd = false;
};

void CertifiedMoveVOI::ConvertBack(proto::RT_TPS_MOVE_VOI *protoMoveVOI) const {
    protoMoveVOI->set_windowtype(mWindowType);
    protoMoveVOI->set_windowuid(mWindowUid);
    protoMoveVOI->set_precoordx(mPreCoordX);
    protoMoveVOI->set_precoordy(mPreCoordY);
    protoMoveVOI->set_curcoordx(mCurCoordX);
    protoMoveVOI->set_curcoordy(mCurCoordY);
    protoMoveVOI->set_voiuid(mVoiUID);
    protoMoveVOI->set_offsetx(mOffsetX);
    protoMoveVOI->set_offsety(mOffsetY);
    protoMoveVOI->set_offsetz(mOffsetZ);
    protoMoveVOI->set_movedbyoff(mIsEditByOffset);
    protoMoveVOI->set_ispancurrentslice(mIsEditCurrentSlice);
    protoMoveVOI->set_mouseactiontype(mMouseActionType);
    protoMoveVOI->set_isshowwarning(mIsShowWarning);
}

TPS_END_NAMESPACE