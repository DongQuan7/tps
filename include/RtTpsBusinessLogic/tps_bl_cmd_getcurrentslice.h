////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_cmd_create_plan.h
/// 
///  \brief class 
/// 
///  \version 1.0
/// 
///  \date    Oct.15,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_GETCURRENTSLICE_H_
#define TPS_BL_CMD_GETCURRENTSLICE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC GetCurrentSliceCmd : public TpsCommand{
public:
    GetCurrentSliceCmd(const int& windType, const std::string& windUid);
    virtual ~GetCurrentSliceCmd(void);
	int GetCurrentSlice();
	int GetTotalSlice();
	int GetStartSliceNumber();
protected:
    virtual int Execute();

private:
    int                         mCurrentSlice;
	int                         mWindType;
	std::string                 mWindUid;
	int                         mTotalSlice;
	int							mStartSliceNumber;
};

TPS_END_NAMESPACE

#endif //TPS_PCA_ADDTRYCMD_H
