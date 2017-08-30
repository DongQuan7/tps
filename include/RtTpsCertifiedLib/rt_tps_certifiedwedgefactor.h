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
///  \date    4. 10, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef RT_MS_CERTIFIEDWEDGEFACTOR_H_
#define RT_MS_CERTIFIEDWEDGEFACTOR_H_

#include "tps_defs.h"
#include "RtTpsProtoLib/rt_ms_wedgefactorSetting.pb.h"

TPS_BEGIN_NAMESPACE

struct WedgeInfoFactor
{
    std::string accessoryUID;
    int fieldSizeX;
    int fieldSizeY;
    float fValue;
};

struct CertifiedWedgeFactorInfoList{

    std::string uid;
    std::string parentUid;

    std::vector<std::tr1::shared_ptr<WedgeInfoFactor> > wedgeInfoList;

    void Convert(const ms::proto::RT_MS_WedgeFactorSettingInfoList &wedgeFactorInfoList) {

        if (wedgeFactorInfoList.has_uid()) {
            uid = wedgeFactorInfoList.uid();
        }
        if (wedgeFactorInfoList.has_parentuid()){
            parentUid = wedgeFactorInfoList.parentuid();
        }

        ms::proto::RT_MS_WedgeFactorSettingInfo wedgeFactor;
        for (int i = 0; i <wedgeFactorInfoList.wedgefactorlist_size(); ++i ){
                std::tr1::shared_ptr<WedgeInfoFactor> wedgeInfo(new WedgeInfoFactor());
                wedgeFactor = wedgeFactorInfoList.wedgefactorlist(i);
                wedgeInfo->accessoryUID = wedgeFactor.accessoryuid();
                wedgeInfo->fieldSizeX = wedgeFactor.fieldsizex();
                wedgeInfo->fieldSizeY = wedgeFactor.fieldsizey();
                wedgeInfo->fValue = wedgeFactor.value();
                wedgeInfoList.push_back(wedgeInfo);
            }
    }

    void ConvertBack(ms::proto::RT_MS_WedgeFactorSettingInfoList* wedgeFactorInfoList) const{
        wedgeFactorInfoList->Clear();
        wedgeFactorInfoList->set_uid(uid);
        wedgeFactorInfoList->set_parentuid(parentUid);

        ms::proto::RT_MS_WedgeFactorSettingInfo * wedgeFactorInfo = nullptr;
        for (auto it = wedgeInfoList.begin(); it != wedgeInfoList.end(); ++it){
            wedgeFactorInfo=  wedgeFactorInfoList->add_wedgefactorlist();
            wedgeFactorInfo->set_accessoryuid((*it)->accessoryUID);
            wedgeFactorInfo->set_fieldsizex((*it)->fieldSizeX);
            wedgeFactorInfo->set_fieldsizey((*it)->fieldSizeY);
            wedgeFactorInfo->set_value((*it)->fValue);
        }
    }

};

TPS_END_NAMESPACE


#endif
