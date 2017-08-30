//////////////////////////////////////////////////////////////////////////
/// \defgroup MS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  linqi.liu linqi.liu@united-imaging.com
///
///  \file    rt_ms_certifiedimportgsttfromfile.h
///  \brief   CertifiedScpInfo
///
///  \version 1.0
///  \date    03. 19, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef RT_MS_CERTIFIEGSTTFILE_H_
#define RT_MS_CERTIFIEGSTTFILE_H_

#include "tps_defs.h"

#include "RtTpsProtoLib/rt_ms_importfromgsstfile.pb.h"

TPS_BEGIN_NAMESPACE


typedef struct CertifiedVirtualFactor{

     float  collimatorX1Position;
     float collimatorX2Position;
     float relativeMU;
     float factor;
     float convolutionFactor;
     float monteCarloFactor;

}VirtualFactor;

struct CertifiedGSTTInfo{

     std::string sAccessoryUID;
     std::string sCommissionedunitUID;
     std::string goldenUID;
     std::string sGSTTFileName;
     float wedgefactor;
     int orientationType;
     std::vector< std::tr1::shared_ptr<CertifiedVirtualFactor> > virtualWedgeFactorList;

    void Convert(const  ms::proto::RT_MS_ImportGSTTFromFile &gsttInfo) {

        if (gsttInfo.has_saccessoryuid()) {
            sAccessoryUID = gsttInfo.saccessoryuid();
        }
        if (gsttInfo.has_scommissionedunituid()){
            sCommissionedunitUID = gsttInfo.scommissionedunituid();
        }
        if (gsttInfo.has_sgsttfilename()){
            sGSTTFileName = gsttInfo.sgsttfilename();
        }
        if (gsttInfo.has_wedgefactor()){
            wedgefactor = gsttInfo.wedgefactor();
        }
        if (gsttInfo.has_orientationtype())
        {
            orientationType = gsttInfo.orientationtype();
        }
        if (gsttInfo.has_goldenuid())
        {
            goldenUID = gsttInfo.goldenuid();
        }
        ms::proto::RT_MS_VirtualWedgeFactorField virtualWedgeFactor;
        std::tr1::shared_ptr<VirtualFactor> pVirtualFactorInfo(new VirtualFactor());
        for (int i = 0; i < gsttInfo.virtualwedgefactorlist_size();++i){
            virtualWedgeFactor = gsttInfo.virtualwedgefactorlist(i);
            pVirtualFactorInfo->collimatorX1Position = virtualWedgeFactor.collimatorx1position();
            pVirtualFactorInfo->collimatorX2Position = virtualWedgeFactor.collimatorx2position();
            pVirtualFactorInfo->relativeMU = virtualWedgeFactor.relativemu();
            pVirtualFactorInfo->factor = virtualWedgeFactor.correctionfactor();
            pVirtualFactorInfo->convolutionFactor = virtualWedgeFactor.convolutionfactor();
            pVirtualFactorInfo->monteCarloFactor = virtualWedgeFactor.montecarlofactor();
            virtualWedgeFactorList.push_back(pVirtualFactorInfo);
        }
    }

    void ConvertBack( ms::proto::RT_MS_ImportGSTTFromFile* pGSTTInfo) const{

        pGSTTInfo->Clear();
        pGSTTInfo->set_saccessoryuid(sAccessoryUID);
        pGSTTInfo->set_scommissionedunituid(sCommissionedunitUID);
        pGSTTInfo->set_sgsttfilename(sGSTTFileName);
        pGSTTInfo->set_wedgefactor(wedgefactor);
        pGSTTInfo->set_orientationtype(orientationType);
        pGSTTInfo->set_goldenuid(goldenUID);

        ms::proto::RT_MS_VirtualWedgeFactorField* pVirtualWedgeFactor = nullptr;
        for (auto virtualWedgeFactor = virtualWedgeFactorList.begin(); virtualWedgeFactor <virtualWedgeFactorList.end(); ++virtualWedgeFactor){
            pVirtualWedgeFactor = pGSTTInfo->add_virtualwedgefactorlist();
            pVirtualWedgeFactor->set_collimatorx1position((*virtualWedgeFactor)->collimatorX1Position);
            pVirtualWedgeFactor->set_collimatorx2position((*virtualWedgeFactor)->collimatorX2Position);
            pVirtualWedgeFactor->set_relativemu((*virtualWedgeFactor)->relativeMU);
            pVirtualWedgeFactor->set_correctionfactor((*virtualWedgeFactor)->factor);
            pVirtualWedgeFactor->set_convolutionfactor((*virtualWedgeFactor)->convolutionFactor);
            pVirtualWedgeFactor->set_montecarlofactor((*virtualWedgeFactor)->monteCarloFactor);
        }
    }

};

TPS_END_NAMESPACE


#endif
