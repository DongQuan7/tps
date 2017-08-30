////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_get_density_overlapped_voi_list.h
/// 
///  \brief class GetDensityOverlappedVoiListCommand declaration 
/// 
///  \version 1.0
/// 
///  \date    2017/02/14
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_GET_DENSITY_OVERLAPPED_VOI_LIST_H_
#define TPS_BL_CMD_GET_DENSITY_OVERLAPPED_VOI_LIST_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsFramework/tps_fw_arithmetic.h"

TPS_BEGIN_NAMESPACE
class ROIEntity;
    
class TPS_BL_DECLSPEC GetDensityOverlappedVoiListCommand : public TpsCommand
{
public:
    GetDensityOverlappedVoiListCommand(const std::string& seriesUid);
    ~GetDensityOverlappedVoiListCommand(void);

    std::vector<std::string> GetOverlappedVoiList() const;

protected:
    virtual int Execute();

private:
    bool IsOverlapped(const MAT4& matP2V, const ROIEntity* source, const ROIEntity* target);

    std::string mSeriesUid;

    std::vector<std::string> mOverlappedVoiList;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(GetDensityOverlappedVoiListCommand);
};

TPS_END_NAMESPACE

#endif