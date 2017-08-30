////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Xiaoqing.Shangguan  Xiaoqing.Shangguan@united-imaging.com
/// 
///  \file tps_pqaca_export_slice_dose_cmd.h
/// 
///  \brief 
/// 
///  \version 1.0
/// 
///  \date    2015/06/09
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_EXPORT_SLICE_DOSE_CMD_H_
#define TPS_BL_EXPORT_SLICE_DOSE_CMD_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

TPS_BEGIN_NAMESPACE
class TpsDoseProxy;

class TPS_BL_DECLSPEC ExportSliceDoseCmd : public TpsCommand
{
public:
    ExportSliceDoseCmd(std::string planeType, std::string filePath, void* doseByte, int size);

    virtual ~ExportSliceDoseCmd();

protected:

    virtual int Execute();

private:
    std::string             m_planeType;
    std::string             m_filePath;
    void*                   m_pDoseByte;
    int                     m_size;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(ExportSliceDoseCmd);
};

TPS_END_NAMESPACE
#endif //TPS_PQACA_POI_UPDATE_ATTRIBUTE_CMD_H_