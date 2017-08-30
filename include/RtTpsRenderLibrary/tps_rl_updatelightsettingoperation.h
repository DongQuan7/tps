////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  RONG Chengcheng chengcheng.rong@united-imaging.com
/// 
///  \file tps_rl_updatelightsettingoperation.cpp
/// 
///  \brief class ActiveChessEdit implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/05/05
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATELIGHTSETTING_OP_H_
#define TPS_RL_UPDATELIGHTSETTING_OP_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsAddVOIOperation
/// 
/// \brief add a VOI graphic object into ModelWareHouse
class TPS_RL_DECLSPEC TpsUpdateLightSettingOperation
    : public OperationBase{
public:
    //the constructor and destructor
    //param[in] the UID of image which the VOI belongs to
    //param[in] the UID of the VOI to be added in database
    TpsUpdateLightSettingOperation(
        const std::string& imageUID, int mprSection, float mprTransparency3D, float rx, float ry, float rz, 
        float scale, float specf, float ambf, float difff, float shininess, 
        float opacity, float lightx, float lighty, float lightz, int smoothness, 
        bool useCameraLight, int displayMode, bool displayVertexCube,
        int startStripIndex, int endStripIndex);
    ~TpsUpdateLightSettingOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string mImageUID;
    float mMprTransparency3D;
    float mrx;
    float mry;
    float mrz;
    float mscale;
    float mspecf;
    float mambf;
    float mdifff;
    float mshininess;
    float mopacity;
    float mlightx;
    float mlighty;
    float mlightz;
    int   msmoothness;
    bool  museCameraLight;
    int   mDisplayMode;
    bool  mdisplayVertexCube;
    int   mStartStripIndex;
    int   mEndStripIndex;
    int   mMprSection;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsUpdateLightSettingOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif