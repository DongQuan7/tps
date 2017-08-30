////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  wenlong.zhao  wenlong.zhao@united-imaging.com
/// 
///  \file tps_rl_easyplan_setisocenter_operation.h 
/// 
///  \brief class TpsEasyPlanSetIsocenterOperation
/// 
///  \version 1.0
/// 
///  \date    2015/07/18
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANSETISOCENTER_H_
#define TPS_RL_EASYPLANSETISOCENTER_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC TpsEasyPlanSetIsocenterOperation
    : public OperationBase {
public:
    TpsEasyPlanSetIsocenterOperation(const std::string &planUid,const std::string &poiUid,
        const std::string& currentImageUID, const float& currentIsoInPlanImgX, const float& currentIsoInPlanImgY, 
        const float& previousIsoInPlanImgCoorX, const float& previousIsoInPlanImgCoorY,
        const float& viewerWidth, const float& viewerHeight, const int& imageWidth, const int& imageHeight,
        const float& firstImagePlanePixelSpacing, const float& secondImagePlanePixelSpacing);
    ~TpsEasyPlanSetIsocenterOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string                  mPlanUid;
    std::string                  mPoiUid;
    std::string                  mCurrentImageUID;
    float                        mCurrentIsoInPlanImgX;
    float                        mCurrentIsoInPlanImgY;
    float                        mPreviousIsoInPlanImgCoorX;
    float                        mPreviousIsoInPlanImgCoorY;
    float                        mViewerWidth;
    float                        mViewerHeight;
    int                          mImageWidth;
    int                          mImageHeight;
    float                        mFirstImagePlanePixelSpacing;
    float                        mSecondImagePlanePixelSpacing;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanSetIsocenterOperation);
};
TPS_END_NAMESPACE

#endif