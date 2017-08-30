////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_pca_create_normgroupcmd.h
/// 
///  \brief class Create a new Norm Group declaration 
/// 
///  \version 1.0
/// 
///  \date    Oct.15,2015
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_CREATE_NORMGROUP_H_
#define TPS_BL_CMD_CREATE_NORMGROUP_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"


TPS_BEGIN_NAMESPACE

class RtNormgroup;
class RtDrrsetting;

class TPS_BL_DECLSPEC CreateNormGroupCmd : public TpsCommand
{
public:
    CreateNormGroupCmd(RtNormgroup* pNormgroup, RtDrrsetting* pDrrsetting = nullptr, int dataSourceId = SOURCE_GROUP_COMMON);
    virtual ~CreateNormGroupCmd(void);

    std::string GetNormGroupUID() const;

protected:
    virtual int Execute();
private:

   // bool AddPOIByVOI(const std::string& voiUID, std::string& poiUID);
    bool CheckInputData();

private:
    RtNormgroup*                mNormgroup;
    RtDrrsetting*               mDrrsetting;
    std::string                 mNormGroupUID;
    int                         mDataSourceId;
};

TPS_END_NAMESPACE

#endif //TPS_PCA_CREATE_NORMGROUPCMD_H_
