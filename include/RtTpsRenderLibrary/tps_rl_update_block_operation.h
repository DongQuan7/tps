//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_rl_edit_block_nudge_operation.h 
///  \brief   nudge block
///
///  \version 1.0
///  \date  2015/03/31  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_RL_UPDATE_BLOCK_OPERATION_H_
#define TPS_RL_UPDATE_BLOCK_OPERATION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_operationbase.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedmlceditinfo.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC UpdateBlockOperation : public OperationBase {
public:
    UpdateBlockOperation(const CertifiedMlcEditInfo &certNudgeInfo);
    ~UpdateBlockOperation();

    bool ModifyGraphicObject() const;
    void Initialize();

    inline bool NeedSynchronize() { return mNeedSync; }

private:
    bool                                                mNeedSync;
    CertifiedMlcEditInfo                                mNudgeInfo;
    mutable std::vector<Mcsf::MedViewer3D::Point2D>     mNudgeCircle;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(UpdateBlockOperation);
};

TPS_END_NAMESPACE

#endif