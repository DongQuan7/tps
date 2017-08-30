////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_switch_table.h
/// 
///  \brief class SwitchTableCommand declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/07/16
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_SWITCH_TABLE_H_
#define TPS_BL_CMD_SWITCH_TABLE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

namespace Mcsf
{
    class Matrix4f;
}

TPS_BEGIN_NAMESPACE

class TpsImage3DEntity;
class RtTable;
class SeriesTable;

class TPS_BL_DECLSPEC SwitchTableCommand : public TpsCommand
{
public:
    SwitchTableCommand(const std::string& seriesUid, const std::string& tableUid);
    ~SwitchTableCommand(void);

protected:
    virtual int Execute();

private:
    void ExtractEulerAnglesFromMatrix_i(const Mcsf::Matrix4f& matrix, double* angles, 
        Mcsf::Matrix4f& rotationTM) const;

    std::string mSeriesUid;
    std::string mTableUid;

    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(SwitchTableCommand);
};

TPS_END_NAMESPACE

#endif