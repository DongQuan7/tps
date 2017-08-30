////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Xiaoqing.Shangguan Xiaoqing.Shangguan@united-imaging.com
/// 
///  \file rt_tps_certifiedoptoutput.cpp
/// 
///  \brief struct CertifiedOptOutput implementation 
/// 
///  \version 1.0
/// 
///  \date    Apr 21, 2014
////////////////////////////////////////////////////////////////
#include "RtTpsCertifiedLib/rt_tps_certifiedoptoutput.h"
#include "RtTpsProtoLib/rt_tps_optoutput.pb.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

void CertifiedOptOutput::ConvertBack(proto::RT_TPS_OptOutput* protoOptOutput)
{
    protoOptOutput->set_nguid(mNormalGroupUid);
    protoOptOutput->set_iterationnumber(mIterationNumber);
    protoOptOutput->set_iterationresult(mIterationResult);
    protoOptOutput->set_progressvalue(mProgressValue);
    protoOptOutput->set_ngmu(mPlanMU);
    protoOptOutput->set_ngsegmentcount(mPlanSegmentCount);
    protoOptOutput->set_timedelived(mTimeDelived);
    protoOptOutput->set_timeleft(mTimeLeft);
    protoOptOutput->set_optreturnvalue(mOptReturnValue);
    protoOptOutput->set_algstatus(mAlgStatus);
    for (std::map<std::string, float>::iterator itEud = mEudResult.begin();
        itEud != mEudResult.end(); ++itEud)
    {
        proto::RT_TPS_OptResultValue* eudValue = protoOptOutput->mutable_eudvalue()->add_eudvaluelist();
        eudValue->set_optuid(itEud->first);
        eudValue->set_resultvalue(itEud->second);
    }
    for (std::map<std::string, float>::iterator itObject = mObjectValue.begin();
        itObject != mObjectValue.end(); ++itObject)
    {
        proto::RT_TPS_OptResultValue* objectValue = protoOptOutput->mutable_objectvalue()->add_objectvaluelist();
        objectValue->set_optuid(itObject->first);
        objectValue->set_resultvalue(itObject->second);
    }
}

TPS_END_NAMESPACE  // end namespace tps
