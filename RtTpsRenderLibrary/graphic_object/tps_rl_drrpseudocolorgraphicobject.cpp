//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_rl_drrpseudocolorgraphicobject.cpp 
///  \brief   pseudo color go for tda drr go base
///
///  \version 1.0
///  \date  2014/07/04  
///  \
//////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include "RtTpsRenderLibrary/tps_rl_drrpseudocolorgraphicobject.h"

TPS_BEGIN_NAMESPACE
DrrPseudoColorGOBase::DrrPseudoColorGOBase() {

}

DrrPseudoColorGOBase::~DrrPseudoColorGOBase() {

}

void DrrPseudoColorGOBase::SetFixPseudoColorTable( unsigned char* pData,
    unsigned int uiSize ) {
    mFixPseudoColorTable.reset(new unsigned char[uiSize * 3]);
    memcpy(mFixPseudoColorTable.get(), pData, uiSize * 3);
}

void DrrPseudoColorGOBase::SetFloatPseudoColorTable( unsigned char* pData,
    unsigned int uiSize )  {
    mFloatPseudoColorTable.reset(new unsigned char[uiSize * 3]);
    memcpy(mFloatPseudoColorTable.get(), pData, uiSize * 3);
}

TPS_END_NAMESPACE
