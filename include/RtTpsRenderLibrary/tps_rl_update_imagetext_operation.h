//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
/// All rights reserved. 
/// 
/// \author  ZHOU Qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
//
/// \file    tps_rl_update_imagetext_operation.h
///
/// \brief   UpdateImageTextOperation
/// 
/// \version 1.0
/// 
/// \date    June 26, 2015
/////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATE_IMAGETEXT_OPERATION_H_
#define TPS_RL_UPDATE_IMAGETEXT_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

namespace Mcsf
{
    namespace MedViewer3D{
        class ClsGraphicObjectMPR;
    }
}

TPS_BEGIN_NAMESPACE

class TpsTextOverlayGraphicObject;
class TPS_RL_DECLSPEC UpdateImageTextOperation : public OperationBase
{
public:
    /// \brief constructor
    /// \param[in] 
    /// \param[in] 
    UpdateImageTextOperation();
    virtual ~UpdateImageTextOperation();

    /// \brief Set GraphicObject keys
    /// \param[in] sMPRKey          MPR
    /// \param[in] sImageTextKey    key
    /// \param[in] sPlanName        value
    /// \return void 
    void SetGraphicObjectKeys(const std::string& sMPRKey,
        const std::string& sImageTextKey,
        const std::string& sPlanName);

    virtual bool ModifyGraphicObject() const;

private:
    bool ConvertPlanNameToImageText(const std::string sPlanName, 
        std::shared_ptr<TpsTextOverlayGraphicObject> spImageTextGo) const;

private:
    std::string     mMprKey;
    std::string     mImageTextKey;
    std::string     mPlanName;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(UpdateImageTextOperation);
};

TPS_END_NAMESPACE

#endif //TPS_RL_UPDATE_CORNER_INFO_OPERATION_H_
