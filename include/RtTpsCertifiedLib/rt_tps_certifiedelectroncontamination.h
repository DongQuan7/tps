//////////////////////////////////////////////////////////////////////////
/// \defgroup MS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  linqi.liu linqi.liu@united-imaging.com
///
///  \file    rt_ms_certifiedelectroncontamination.h
///  \brief   CertifiedElectronContInfo
///
///  \version 1.0
///  \date    1. 15, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef RT_MS_CERTIFIEDELECTRONCONTAMINATION_H_
#define RT_MS_CERTIFIEDELECTRONCONTAMINATION_H_

#include "tps_defs.h"

#include "RtTpsProtoLib/rt_ms_electroncontamination.pb.h"

TPS_BEGIN_NAMESPACE

struct CertifiedElectronContInfo{

     bool  electronContaminationFlag;
     float electronMaxDepth;
     float surfaceDose;
     float depthCoefficientK;
     float offaxisCoefficientA;
     float df;
     float sf;
     float cOne;
     float cTwo;
     float cThree;

    void Convert(const  ms::proto::RT_MS_ElectronContamination &elctronConInfo) {

         if (elctronConInfo.has_electroncontaminationflag()){
             electronContaminationFlag = elctronConInfo.electroncontaminationflag();
         }

        if (elctronConInfo.has_electronmaxdepth()) {
            electronMaxDepth = elctronConInfo.electronmaxdepth();
        }
        if (elctronConInfo.has_surfacedose()){
            surfaceDose = elctronConInfo.surfacedose();
        }
        if (elctronConInfo.has_depthcoefficientk()){
            depthCoefficientK = elctronConInfo.depthcoefficientk();
        }
        if (elctronConInfo.has_offaxiscoefficienta()){
            offaxisCoefficientA = elctronConInfo.offaxiscoefficienta();
        }
        if (elctronConInfo.has_df()){
            df = elctronConInfo.df();
        }
        if (elctronConInfo.has_sf()){
            sf = elctronConInfo.sf();
        }
        if (elctronConInfo.has_cone()){
            cOne = elctronConInfo.cone();
        }
        if (elctronConInfo.has_ctwo()){
            cTwo = elctronConInfo.ctwo();
        }
        if (elctronConInfo.has_cone()){
            cThree = elctronConInfo.cthree();
        }
    }

    void ConvertBack( ms::proto::RT_MS_ElectronContamination* pElctronConInfo) const{

        pElctronConInfo->Clear();
        pElctronConInfo->set_electroncontaminationflag(electronContaminationFlag);
        pElctronConInfo->set_electronmaxdepth(electronMaxDepth);
        pElctronConInfo->set_surfacedose(surfaceDose);
        pElctronConInfo->set_depthcoefficientk(depthCoefficientK);
        pElctronConInfo->set_offaxiscoefficienta(offaxisCoefficientA);
        pElctronConInfo->set_df(df);
        pElctronConInfo->set_sf(sf);
        pElctronConInfo->set_cone(cOne);
        pElctronConInfo->set_ctwo(cTwo);
        pElctronConInfo->set_cthree(cThree);
    }

};

TPS_END_NAMESPACE


#endif