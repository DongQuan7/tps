////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_updatedoseoperation.h
/// 
///  \brief class TpsRefreshDoseOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/08
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATEDOSEOPERATION_H_
#define TPS_RL_UPDATEDOSEOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"


TPS_BEGIN_NAMESPACE  // begin namespace tps

struct DoseLineAttribute;
/// \class TpsRefreshDoseOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC TpsRefreshDoseOperation
        : public OperationBase{
public:
    //the constructor and destructor
    TpsRefreshDoseOperation(const std::string& planUid, FIRST_POSTFIX_COMPONENT sectionType = EMPTY_COMPONENT);
    ~TpsRefreshDoseOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string     mPlanUID;
    FIRST_POSTFIX_COMPONENT mSectionType;


private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsRefreshDoseOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
