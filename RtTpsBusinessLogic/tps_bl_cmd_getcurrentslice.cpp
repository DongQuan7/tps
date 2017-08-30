////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_bl_cmd_delete_voi.cpp
/// 
///  \brief class  delete voi  
/// 
///  \version 1.0
/// 
///  \date    Oct.21,2015
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_getcurrentslice.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"


#include "tps_logger.h"

TPS_BEGIN_NAMESPACE;

GetCurrentSliceCmd::GetCurrentSliceCmd(const int& windType, const std::string& windUid):
mCurrentSlice(-1), mWindType(windType), mWindUid(windUid), mTotalSlice(-1)
{
}

GetCurrentSliceCmd::~GetCurrentSliceCmd() {

}

int GetCurrentSliceCmd::Execute() 
{
	int slice = -1;
	int totalSlice = -1;
	int startSliceNumber = -1;
	mRenderProxy->GetCurrentMprSlice((WINDOW_TYPE)mWindType, mWindUid, slice, startSliceNumber, totalSlice);
	mCurrentSlice = slice;
	mTotalSlice = totalSlice;
	mStartSliceNumber = startSliceNumber;
    return TPS_ER_SUCCESS;
}

int GetCurrentSliceCmd::GetCurrentSlice()
{
	return mCurrentSlice;
}

int GetCurrentSliceCmd::GetTotalSlice()
{
	return mTotalSlice;
}

int GetCurrentSliceCmd::GetStartSliceNumber()
{
	return mStartSliceNumber;
}
TPS_END_NAMESPACE

