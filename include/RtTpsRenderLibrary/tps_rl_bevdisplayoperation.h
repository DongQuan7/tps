////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao Chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_bevdisplayoperation.h
/// 
///  \brief class BEVWindowOperation
/// 
///  \version 1.0
/// 
///  \date    2014/5/28
////////////////////////////////////////////////////////////////


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

#include "Mcsf3DArithmetic/mcsf_3d_point2f.h"



TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC BEVPanOperation :
    public OperationBase {
public:
    BEVPanOperation(const Mcsf::Point2f &preCoord ,
        const Mcsf::Point2f &curCoord,
        const std::string& planUID,
        DISPLAY_SIZE displaySize);
    ~BEVPanOperation();

    virtual bool ModifyGraphicObject() const;

private:
    Mcsf::Point2f           mPreCoord;
    Mcsf::Point2f           mCurCoord;
    std::string             mPlanUID;
    DISPLAY_SIZE            mDisplaySize;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(BEVPanOperation);
};

class TPS_RL_DECLSPEC BEVZoomOperation :
    public OperationBase {
public:
    BEVZoomOperation(float zoomFactor, const std::string& imageUID, 
        const std::string& planUID);
    ~BEVZoomOperation();

    virtual bool ModifyGraphicObject() const;

private:
    float       mZoomFactor;
    std::string mImageUID;
    std::string mPlanUID;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(BEVZoomOperation);
};

class TPS_RL_DECLSPEC SwitchBEVCEVOperation :
    public OperationBase {
public:
    SwitchBEVCEVOperation(const std::string& planUID);
    ~SwitchBEVCEVOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string     mPlanUID;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(SwitchBEVCEVOperation);
};



TPS_END_NAMESPACE
