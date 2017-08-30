////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file rt_tps_certified_registation_feedback.cpp
/// 
///  \brief struct CertifiedRegistrationFeedback implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/6/23
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certified_registation_feedback.h"
#include "RtTpsProtoLib/rt_tps_registration_feedback.pb.h"

TPS_BEGIN_NAMESPACE

CertifiedRegistrationFeedback::CertifiedRegistrationFeedback()
: mFeedbackType(0), mMessage(""){}

CertifiedRegistrationFeedback::~CertifiedRegistrationFeedback() {}

void CertifiedRegistrationFeedback::Convert(
    const proto::RT_TPS_REGISTRATION_FEEDBACK& feedback) {
    mFeedbackType = feedback.feedbacktype();
    mMessage = feedback.message();
}

void CertifiedRegistrationFeedback::ConvertBack(
    proto::RT_TPS_REGISTRATION_FEEDBACK* feedback) const{
    feedback->set_feedbacktype(mFeedbackType);
    feedback->set_message(mMessage);
}

TPS_END_NAMESPACE