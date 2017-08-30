//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  Xiaoqing.Shangguan Xiaoqing.Shangguan@united-imaging.com
///
///  \file    tps_rl_updatevoioperation.h
///  \brief 
///
///  \version 1.0
///  \date    Mar. 08, 2013
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATEVOIDISPLAYOPERATION_H_
#define TPS_RL_UPDATEVOIDISPLAYOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsUpdateVoiDisplayOperation
/// 
/// \brief this operation update a existing according to beam in beam manager.
class TPS_RL_DECLSPEC TpsUpdateVoiDisplayOperation
    : public OperationBase{
public:
    //the constructor and destructor
    TpsUpdateVoiDisplayOperation(const std::string& seriesUid, const std::string& voiUid);
    ~TpsUpdateVoiDisplayOperation();
    virtual bool ModifyGraphicObject() const;
private:
    std::string         mImageUid;
    std::string         mVoiUid;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsUpdateVoiDisplayOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif