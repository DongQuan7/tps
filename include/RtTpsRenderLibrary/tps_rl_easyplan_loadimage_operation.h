////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Liang Xiang xiang.liang@united-imaging.com
/// 
///  \file tps_rl_easyplan_loadimage_operation.h
/// 
///  \brief class TpsEasyPlanLoadImageOperation
/// 
///  \version 1.0
/// 
///  \date    2017/7/13
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANLOADIMAGEOPERATION_H_
#define TPS_RL_EASYPLANLOADIMAGEOPERATION_H_


#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

class EasyPlanDrrGraphicObejct;

class TPS_RL_DECLSPEC TpsEasyPlanLoadImageOperation
    : public OperationBase {
public:
    TpsEasyPlanLoadImageOperation(const std::string &planUID,const int& viewerWidth, const int& viewerHeight);
    ~TpsEasyPlanLoadImageOperation();
    virtual bool ModifyGraphicObject() const;

private:
    std::string                     mPlanUID;
    int                             mViewerWidth;
    int                             mViewerHeight;

    //private:
    //    std::shared_ptr<EasyPlanDrrGraphicObejct> GenerateDrrGO(std::string imageUID);

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanLoadImageOperation);
};
TPS_END_NAMESPACE

#endif