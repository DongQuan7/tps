//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
///
///  \file tps_rl_shadedsurface_zoom_operation.h
///  \brief class ShadedSurfaceZoomOperation declaration 
///
///  \version 1.0
///  \date    Dec. 23, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SHADEDSURFACE_ZOOM_OPERATION_H_
#define TPS_RL_SHADEDSURFACE_ZOOM_OPERATION_H_

#include "tps_rl_defs.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "RtTpsFramework/tps_fw_operationbase.h"


TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC ShadedSurfaceZoomOperation
    : public OperationBase {
public:
    ShadedSurfaceZoomOperation(const std::string &imageUid, float fZoomFactor);
    ~ShadedSurfaceZoomOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string     mImageUid;
    float           mZoomFactor;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(ShadedSurfaceZoomOperation);

};

TPS_END_NAMESPACE
#endif