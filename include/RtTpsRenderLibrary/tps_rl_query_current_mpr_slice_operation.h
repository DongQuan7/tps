////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_query_current_mpr_slice_operation.h
/// 
///  \brief class QueryMprSliceInfoOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2017/02/13
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_QUERY_CURRENT_MPR_SLICE_OPERATION_H_
#define TPS_RL_QUERY_CURRENT_MPR_SLICE_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class QueryMprSliceInfoOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC QueryMprSliceInfoOperation
 : public OperationBase{
public:
    //the constructor and destructor
    QueryMprSliceInfoOperation(const std::string& seriesUID, 
        FIRST_POSTFIX_COMPONENT section, bool isViewOrder = false);
    virtual ~QueryMprSliceInfoOperation();

    virtual bool ModifyGraphicObject() const;

    void GetSliceInfo(int& current, int& total, int& start) const;
protected:
    std::string mSeriesUID;
    FIRST_POSTFIX_COMPONENT mSectionType;
    bool        mIsViewOrder;
    mutable int mCurrentSliceNumber;
    mutable int mTotalSliceNumber;
    mutable int mStartSliceNumber;

    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(QueryMprSliceInfoOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
