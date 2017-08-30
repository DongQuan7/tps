////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  
/// 
///  \file tps_rl_TpsGalleryVRRotateOperation.h
/// 
///  \brief TpsGalleryVRRotateOperation 
/// 
///  \version 1.0
/// 
///  \date    2014/4/16
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_VRROTATE_OPERATION_H_
#define TPS_RL_GALLERY_VRROTATE_OPERATION_H_

#include "tps_rl_defs.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "RtTpsFramework/tps_fw_operationbase.h"


TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsGalleryVRRotateOperation
    : public OperationBase {
public:
    TpsGalleryVRRotateOperation(const std::string seriesUID, const Mcsf::Point2f &preCoord, const Mcsf::Point2f &currentCoord);
    ~TpsGalleryVRRotateOperation();
    virtual bool ModifyGraphicObject() const;

private:
    Mcsf::Point2f   mPreCoord;
    Mcsf::Point2f   mCurrentCoord;
    std::string     mSeriesUID;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryVRRotateOperation);

};

TPS_END_NAMESPACE
#endif