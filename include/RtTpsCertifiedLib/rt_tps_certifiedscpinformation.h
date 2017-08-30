//////////////////////////////////////////////////////////////////////////
/// \defgroup MS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  linqi.liu linqi.liu@united-imaging.com
///
///  \file    rt_ms_certifiedscpinformation.h
///  \brief   CertifiedScpInfo
///
///  \version 1.0
///  \date    1. 15, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef RT_MS_CERTIFIEDSCP_H_
#define RT_MS_CERTIFIEDSCP_H_

#include "tps_defs.h"

#include "RtTpsProtoLib/rt_ms_scp.pb.h"

TPS_BEGIN_NAMESPACE

struct CertifiedScpInfo{
    std::string parentUID ;
    std::string uID;
    float fieldSizeX;
    float fieldSizeY;
    float scp;

    void Convert(const ms::proto::RT_MS_SCPInfo &scpInfo) {

        if (scpInfo.has_uid()) {
            uID = scpInfo.uid();
        }
        if (scpInfo.has_parentuid()){
            parentUID = scpInfo.parentuid();
        }
        if (scpInfo.has_fieldsizex()){
            fieldSizeX = scpInfo.fieldsizex();
        }
        if (scpInfo.has_fieldsizey()){
            fieldSizeY = scpInfo.fieldsizey();
        }
        if (scpInfo.has_scp()){
            scp = scpInfo.scp();
        }
    }

    void ConvertBack(ms::proto::RT_MS_SCPInfo* scpInfo) const{
        scpInfo->Clear();
        scpInfo->set_uid(uID);
        scpInfo->set_parentuid(parentUID);
        scpInfo->set_fieldsizex(fieldSizeX);
        scpInfo->set_fieldsizey(fieldSizeY);
        scpInfo->set_scp(scp);
    }

};

TPS_END_NAMESPACE


#endif