//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xiaoqing.Shaangguan Xiaoqing.Shaangguan@united-imaging.com
//
/// \file    tps_clear_data_manager_cmd.h
///
/// \brief   
/// 
/// \version 1.0
/// 
/// \date    2015/07/08
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_CLEAR_DATA_MANAGER_H_
#define TPS_BL_CMD_CLEAR_DATA_MANAGER_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE // begin namespace tps

class ITpsDataRepository;

class TPS_BL_DECLSPEC ClearDataManagerCmd : public TpsCommand
{
public:
	ClearDataManagerCmd();
	virtual ~ClearDataManagerCmd();
protected:
	virtual int Execute();
};

TPS_END_NAMESPACE

#endif  
