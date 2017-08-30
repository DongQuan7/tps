////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_update_table.h
/// 
///  \brief class UpdateTableCommand declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/07/21
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_UPDATE_TABLE_H_
#define TPS_BL_CMD_UPDATE_TABLE_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC UpdateTableCommand : public TpsCommand
{
public:
    UpdateTableCommand(const std::string& seriesUid, 
        const std::string& tableUid, double density, double referenceEDensity,
        double offsetX, double offsetY, double offsetZ, 
        double angleX, double angleY, double angleZ);
    ~UpdateTableCommand(void);

protected:
    virtual int Execute();

    std::string mSeriesUid;
    std::string mTableUid;
    double mDensity;
    double mReferenceEDensity;
    double mOffsets[3];
    double mRotationAngles[3];

    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(UpdateTableCommand);
};

TPS_END_NAMESPACE

#endif