////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  WANG Jie  jie.wang@united-imaging.com
/// 
///  \file tps_fw_prepare_notifier.h
/// 
///  \brief class TpsMCPrepareNotifier declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/10/09
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_PREPARE_NOTIFIER_H_
#define TPS_FW_PREPARE_NOTIFIER_H_

#include "tps_fw_defs.h"
#include "alg/alg_commission_prenotifier.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class PrepareActiveResponser;

/// \class TpsMCPrepareNotifier
/// 
/// \brief 
class TPS_FW_DECLSPEC TpsMCPrepareNotifier : public CommiPreEneNotifier{
public:
    //the constructor and destructor
    TpsMCPrepareNotifier();
    virtual ~TpsMCPrepareNotifier();

    virtual void NotifyProgress(int i, int errortype);

    virtual int GetProgress(void) const;

    // \brief set progress responser
    void SetPrepareResponser(const std::shared_ptr<PrepareActiveResponser>& responser);

private:
    //int              mProgess;
    int              completeFilesNumber;

    std::shared_ptr<PrepareActiveResponser> mPrepareResponser;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsMCPrepareNotifier);
};

TPS_END_NAMESPACE  // end namespace tps

#endif