////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_remove_registration_relationship.h
/// 
///  \brief class RemoveRegistrationRelationshipCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/10/20
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_REMOVE_REGISTRATION_RELATIONSHIP_H_
#define TPS_BL_CMD_REMOVE_REGISTRATION_RELATIONSHIP_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"              // for TpsCommand

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TPS_BL_DECLSPEC RemoveRegistrationRelationshipCmd : public TpsCommand {
public:
    //the constructor and destructor
    RemoveRegistrationRelationshipCmd(const std::string& strFixedSeriesUid, 
        const std::string& strFloatSeriesUid);
    virtual ~RemoveRegistrationRelationshipCmd();

protected:
    virtual int Execute();

private:
    std::string mFixedSeriesUid;
    std::string mFloatSeriesUid;
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_BL_CMD_REMOVE_REGISTRATION_RELATIONSHIP_H_