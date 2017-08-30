//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  wenqiao.jiang (mailto:wenqiao.jiang@united-imaging.com)
///
///  \brief   delete poi in easy plan
///
///  \version 1.0
///  \date  2015/10/30  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_EASYPLAN_DELETE_POI_H_
#define TPS_BL_CMD_EASYPLAN_DELETE_POI_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC TpsDeletePoiEasyPlanCmd : public TpsCommand {
public:
    TpsDeletePoiEasyPlanCmd(const std::string& sEzPlanUID, const std::string& sEzPoiUID, const RtDbDef::POI_TYPE & sEZPoiType);
    virtual ~TpsDeletePoiEasyPlanCmd();

protected:
    virtual int Execute();

private:
    std::string mPlanUid;
    std::string mPoiUid;
    RtDbDef::POI_TYPE mPoiType;
};

TPS_END_NAMESPACE

#endif