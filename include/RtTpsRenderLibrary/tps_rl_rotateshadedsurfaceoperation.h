////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_rotateshadedsurfaceoperation.h
/// 
///  \brief RotateShadedSurfaceOperation 
/// 
///  \version 1.0
/// 
///  \date    2014/4/16
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_ROTATESHADEDSURFACEOPERATION_H_
#define TPS_RL_ROTATESHADEDSURFACEOPERATION_H_

#include "tps_rl_defs.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "RtTpsFramework/tps_fw_operationbase.h"


TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC RotateShadedSurfaceOperation
    : public OperationBase {
public:
    RotateShadedSurfaceOperation(const std::string &imageUid,const Mcsf::Point2f &preCoord, const Mcsf::Point2f &currentCoord);
    ~RotateShadedSurfaceOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string     mImageUid;
    Mcsf::Point2f   mPreCoord;
    Mcsf::Point2f   mCurrentCoord;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(RotateShadedSurfaceOperation);

};

TPS_END_NAMESPACE
#endif