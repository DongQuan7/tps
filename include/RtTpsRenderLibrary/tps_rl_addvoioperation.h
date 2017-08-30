////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_addvoioperation.h
/// 
///  \brief class TpsAddVOIOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/20
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_ADDVOIOPERATION_H_
#define TPS_RL_ADDVOIOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsAddVOIOperation
/// 
/// \brief add a VOI graphic object into ModelWareHouse
class TPS_RL_DECLSPEC TpsAddVOIOperation
    : public OperationBase{
public:
    //the constructor and destructor
    //param[in] the UID of image which the VOI belongs to
    //param[in] the UID of the VOI to be added in database
    TpsAddVOIOperation(const std::string& imageUID, const std::string& voiUID);
    ~TpsAddVOIOperation();

    virtual bool ModifyGraphicObject() const;

private:
    mutable std::string mImageUID;
    mutable std::string mVOIUID;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsAddVOIOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif