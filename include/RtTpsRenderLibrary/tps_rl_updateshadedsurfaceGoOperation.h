////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  wenqiao.jiang  wenqiao.jiang@united-imaging.com
/// 
///  \file tps_rl_updateshadedsurfaceGoOperation.cpp
/// 
///  \brief Update ShadedSurface Go 
/// 
///  \version 1.0
/// 
///  \date    2016/03/23
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATESHADEDSURFACEGOPERATION_H_
#define TPS_RL_UPDATESHADEDSURFACEGOPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsUpdateVoiDisplayOperation
/// 
/// \brief this operation update a existing according to beam in beam manager.
class TPS_RL_DECLSPEC TpsUpdateShadeSurfaceOperation
    : public OperationBase{
public:
    //the constructor and destructor
    TpsUpdateShadeSurfaceOperation(const std::string& seriesUid);
    ~TpsUpdateShadeSurfaceOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string         mSeriesUid;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsUpdateShadeSurfaceOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
