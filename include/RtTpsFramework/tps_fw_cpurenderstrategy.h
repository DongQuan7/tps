////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  RONG Chengcheng chengcheng.rong@united-imaging.com
/// 
///  \file tps_fw_cpurenderstrategy.h
/// 
///  \brief class TpsDisplayCashe declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/03
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_CPURENDERSTRATEGY_H_
#define TPS_FW_CPURENDERSTRATEGY_H_

#include "RtTpsFramework/tps_fw_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class CPURenderStrategy {
public:
    virtual bool Render(unsigned char*& buffer, int bufWidth, int bufHeight) = 0;
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_RC_CPURENDERSTRATEGY_H_