////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_updatevoioperation.cpp
/// 
///  \brief TpsUpdateVoiOperation  
/// 
///  \version 1.0
/// 
///  \date    2014/6/11
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATEVOIOPERATION_H_
#define TPS_RL_UPDATEVOIOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

    /// \class TpsUpdateVoiDisplayOperation
    /// 
    /// \brief this operation update a existing according to beam in beam manager.
class TPS_RL_DECLSPEC TpsUpdateVoiOperation
    : public OperationBase{
public:
    //the constructor and destructor
    TpsUpdateVoiOperation(const std::string& seriesUid, const std::string& voiUid);
    ~TpsUpdateVoiOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string         mSeriesUid;
    std::string         mVoiUid;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsUpdateVoiOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
