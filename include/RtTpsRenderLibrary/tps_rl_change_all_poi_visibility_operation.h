////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_change_all_poi_visibility_operation.h
/// 
///  \brief class ChangeAllPoiVisibility declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/03/30
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_CHANGE_ALL_POI_VISIBILITY_OPERATION_H_
#define TPS_RL_CHANGE_ALL_POI_VISIBILITY_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class ChangeAllPoiVisibilityOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC ChangeAllPoiVisibilityOperation
 : public OperationBase{
public:
    //the constructor and destructor
    ChangeAllPoiVisibilityOperation(const std::string& seriesUid, bool visible);
    virtual ~ChangeAllPoiVisibilityOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string mSeriesUid;
    bool        mVisbile;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(ChangeAllPoiVisibilityOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif