////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file rt_tps_certified_registation_feedback.h
/// 
///  \brief struct CertifiedRegistrationFeedback declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/6/23
////////////////////////////////////////////////////////////////

#ifndef RT_TPS_CERTIFIED_REGISTRATION_FEEDBACK_H_
#define RT_TPS_CERTIFIED_REGISTRATION_FEEDBACK_H_

#include "tps_defs.h"

TPS_BEGIN_NAMESPACE

namespace proto{
    class RT_TPS_REGISTRATION_FEEDBACK;
}

struct CertifiedRegistrationFeedback {
    int mFeedbackType;
    std::string mMessage;

    CertifiedRegistrationFeedback();
    ~CertifiedRegistrationFeedback();

    void Convert(const proto::RT_TPS_REGISTRATION_FEEDBACK& feedback);

    void ConvertBack(proto::RT_TPS_REGISTRATION_FEEDBACK* feedback) const;
};

TPS_END_NAMESPACE
#endif