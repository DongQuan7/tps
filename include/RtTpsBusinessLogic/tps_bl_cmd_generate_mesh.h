//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_bl_cmd_generate_mesh.h
///  \brief   
///
///  \version 1.0
///  \date  2015/10/28  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_GENERATE_MESH_H_
#define TPS_BL_CMD_GENERATE_MESH_H_

#undef min
#undef max

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC GenerateMeshCmd : public TpsCommand
{
public:
	GenerateMeshCmd(std::string &imageUID, int organType);
	~GenerateMeshCmd();

protected:
	virtual int Execute();

private:
	std::string				mImageUID;
	int						mOrganType;

private:
	TPS_DISALLOW_COPY_AND_ASSIGN(GenerateMeshCmd);
};

TPS_END_NAMESPACE

#endif