/////////////////////////////////////////////////////////////////////////
/// \defgroup MS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  linqi.liu linqi.liu@united-imaging.com
///
///  \file    rt_ms_certifiedpencilbeam.h
///  \brief   CertifiedPencilBeam
///
///  \version 1.0
///  \date    1 16, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef RT_MS_CERTIFIEDPENCILBEAM_H_
#define RT_MS_CERTIFIEDPENCILBEAM_H_

#include "tps_defs.h"

#include "RtTpsCertifiedLib/rt_tps_certifiedwedgefield.h"
#include "RtTpsProtoLib/rt_ms_pencilbeam.pb.h"

TPS_BEGIN_NAMESPACE

typedef struct PencilPPDTMR
{
    std::string uid;
    float depth;
    float value;
}Tmr;

typedef struct PencilSMRInfo
{
    std::string uid;
    float depth;
    std::vector<float> radiusList;
}SMRInfo;

typedef struct PencilOffAxisVector
{
    std::string uid;
	std::vector<float> rowValues;
}PencialVector;

struct CertifiedPencilBeam
{
    float diamOne;
    float diamTwo;
    float diamThree;
    float srcRatio;
    float srcRatio3;
    float eeqdiam;
    float transmission;
    float psd;
    float sddXJaw;
    float sddYJaw;

    float xk0;
    float xk1;
    float xk2;
    float yk0;
    float yk1;
    float yk2;


    int scXcount;
    int scYcount;
    std::string scXPositions;
    std::string scYPositions;
    std::string scBlocb;
    float referencefluence;

    //std::string wedgeType;
    std::string wedgeId;
    //float  mu;
    //float dmudx;
    //float dmudr;
    //float dmuda;
    //float hvlSlope;
    //float carriermu;
    //float carrierdmudx;
    //float carrierdmudr;
    //float carrierdmuda;
    //float carrierhvlSlope;

    //float wedgeThick;
    //float carrierThick;
    //std::string virtualUid;

    std::string commissionUID;
    std::string accName;
    std::string accessoryUID;

    std::string depthvec;
    std::string radiusvec;
    std::string sar;

    std::string flatnessradiusvec;
    std::string flatnessdepthvec;
    std::string flatness;

    std::vector< std::tr1::shared_ptr<PencilPPDTMR> > tMRInfoList;

    std::vector< std::tr1::shared_ptr<CommonPhysicalWedgeField> > physicalWedgeList;
    std::vector< std::tr1::shared_ptr<CertifiedVirtualWedgeInfo> > virtualWedgeList;

    void Convert(const ms::proto::RT_MS_PencilBeam &pencilBeam) {

        if ( pencilBeam.has_diamone()){
            diamOne = pencilBeam.diamone();
        }
        if (pencilBeam.has_diamtwo()){
            diamTwo = pencilBeam.diamtwo();
        }
        if (pencilBeam.has_diamthree()){
            diamThree = pencilBeam.diamthree();
        }
        if ( pencilBeam.has_srcratio()){
            srcRatio = pencilBeam.srcratio();
        }
        if (pencilBeam.has_srcratio3()){
            srcRatio3 = pencilBeam.srcratio3();
        }
        if (pencilBeam.has_eeqdiam()){
            eeqdiam = pencilBeam.eeqdiam();
        }
        if ( pencilBeam.has_transmission()){
            transmission = pencilBeam.transmission();
        }
        if ( pencilBeam.has_psd()){
            psd = pencilBeam.psd();
        }
        if ( pencilBeam.has_sddxjaw()){
            sddXJaw = pencilBeam.sddxjaw();
        }
        if ( pencilBeam.has_sddyjaw()){
            sddYJaw = pencilBeam.sddyjaw();
        }

        if ( pencilBeam.has_xk0()){
            xk0 = pencilBeam.xk0();
        }
        if ( pencilBeam.has_xk1()){
            xk1 = pencilBeam.xk1();
        }
        if ( pencilBeam.has_xk2()){
            xk2 = pencilBeam.xk2();
        }
        if ( pencilBeam.has_yk0()){
            yk0 = pencilBeam.yk0();
        }
        if ( pencilBeam.has_yk1()){
            yk1 = pencilBeam.yk1();
        }
        if ( pencilBeam.has_yk2()){
            yk2 = pencilBeam.yk2();
        }
        if (pencilBeam.has_commissionuid()){
            commissionUID = pencilBeam.commissionuid();
        }
        if (pencilBeam.has_accname()){
           accName = pencilBeam.accname();
        }
        if (pencilBeam.has_accessoryuid()){
            accessoryUID = pencilBeam.accessoryuid();
        }
        if (pencilBeam.has_wedgeid()){
            wedgeId = pencilBeam.wedgeid();
        }

        //PencilBeamScCorrection
        if (pencilBeam.has_scxpositionsproto())
        {
            scXPositions = pencilBeam.scxpositionsproto();
        }
        if (pencilBeam.has_scypositionsproto())
        {
            scYPositions = pencilBeam.scypositionsproto();
        }
        if (pencilBeam.has_scblocbproto())
        {
            scBlocb = pencilBeam.scblocbproto();
        }
        if (pencilBeam.has_referencefluence())
        {
            referencefluence = pencilBeam.referencefluence();
        }

        //PencilPDDTMR
        ms::proto::RT_MS_PencilPDDTMR tmr;
        for (int i = 0; i < pencilBeam.tmrlistlist_size();++i){
             std::tr1::shared_ptr<Tmr> pTMRInfo(new Tmr());
            tmr = pencilBeam.tmrlistlist(i);
            pTMRInfo->uid = tmr.uid();
            pTMRInfo->depth = tmr.depth();
            pTMRInfo->value = tmr.value();
            tMRInfoList.push_back(pTMRInfo);
        }

        //PencilPDDSMRInfo
        if (pencilBeam.has_depthvec()){
            depthvec = pencilBeam.depthvec();
        }
        if (pencilBeam.has_radiusvec()){
            radiusvec = pencilBeam.radiusvec();
        }
        if (pencilBeam.has_sar()){
            sar = pencilBeam.sar();
        }

        //PencilOffAxisVector
        if (pencilBeam.has_flatnessradiusvec()){
            flatnessradiusvec = pencilBeam.flatnessradiusvec();
        }
        if (pencilBeam.has_flatnessdepthvec()){
            flatnessdepthvec = pencilBeam.flatnessdepthvec();
        }
        if (pencilBeam.has_flatness()){
            flatness = pencilBeam.flatness();
        }

        // RT_MS_PhysicalWedgeField
        ms::proto::RT_MS_PencilBeamPhysicalWedgeField physicalWedgeField;
        for (int i = 0; i < pencilBeam.physicalwedgefieldlist_size();++i){

            std::tr1::shared_ptr<PhysicalWedgeField> pPhysicalWedgeField(new PhysicalWedgeField());
            physicalWedgeField = pencilBeam.physicalwedgefieldlist(i);

            pPhysicalWedgeField->wedgeUID = pencilBeam.physicalwedgefieldlist(i).wedgeuid();
            pPhysicalWedgeField->wedgeId = pencilBeam.physicalwedgefieldlist(i).wedgeid();
            pPhysicalWedgeField->wedgeType = pencilBeam.physicalwedgefieldlist(i).wedgetype();
            pPhysicalWedgeField->mu = pencilBeam.physicalwedgefieldlist(i).mu();
            pPhysicalWedgeField->dmudx = pencilBeam.physicalwedgefieldlist(i).dmudx();
            pPhysicalWedgeField->dmudr = pencilBeam.physicalwedgefieldlist(i).dmudr();
            pPhysicalWedgeField->dmuda = pencilBeam.physicalwedgefieldlist(i).dmuda();
            pPhysicalWedgeField->hvlSlope = pencilBeam.physicalwedgefieldlist(i).hvlslope();
            pPhysicalWedgeField->wedgeThick = pencilBeam.physicalwedgefieldlist(i).wedgethick();
            pPhysicalWedgeField->density = pencilBeam.physicalwedgefieldlist(i).density();

            pPhysicalWedgeField->carrierUID = pencilBeam.physicalwedgefieldlist(i).carrieruid();
            pPhysicalWedgeField->carriermu = pencilBeam.physicalwedgefieldlist(i).carriermu();
            pPhysicalWedgeField->carrierdmudx = pencilBeam.physicalwedgefieldlist(i).carrierdmudx();
            pPhysicalWedgeField->carrierdmudr = pencilBeam.physicalwedgefieldlist(i).carrierdmudr();
            pPhysicalWedgeField->carrierdmuda = pencilBeam.physicalwedgefieldlist(i).carrierdmuda();
            pPhysicalWedgeField->carrierhvlSlope = pencilBeam.physicalwedgefieldlist(i).carrierhvlslope();
            pPhysicalWedgeField->carrierThick = pencilBeam.physicalwedgefieldlist(i).carrierthick();
            pPhysicalWedgeField->contourUID1 = pencilBeam.physicalwedgefieldlist(i).contouruid1();
            pPhysicalWedgeField->contourUID2 = pencilBeam.physicalwedgefieldlist(i).contouruid2();

            ms::proto::RT_MS_PencilContourVerter contourVerter;
            for (int j = 0; j < pencilBeam.physicalwedgefieldlist(i).weightcontourverterlist_size();++j ){

                std::tr1::shared_ptr<CommonContourVerter> pPhysicalContourVerter(new CommonContourVerter());
                contourVerter = pencilBeam.physicalwedgefieldlist(i).weightcontourverterlist(j);
                pPhysicalContourVerter->uid = contourVerter.uid();
                pPhysicalContourVerter->contourVerterx = contourVerter.contourverterx();
                pPhysicalContourVerter->contourVertery = contourVerter.contourvertery();
                pPhysicalWedgeField->wedgeContourVerterList.push_back(pPhysicalContourVerter);
            }

            for (int j = 0; j < pencilBeam.physicalwedgefieldlist(i).carriercontourverterlist_size();++j ){
                std::tr1::shared_ptr<CommonContourVerter> pCarrierContourVerter(new CommonContourVerter());
                contourVerter = pencilBeam.physicalwedgefieldlist(i).carriercontourverterlist(j);
                pCarrierContourVerter->uid = contourVerter.uid();
                pCarrierContourVerter->contourVerterx = contourVerter.contourverterx();
                pCarrierContourVerter->contourVertery = contourVerter.contourvertery();
                pPhysicalWedgeField->carrierContourVerterList.push_back(pCarrierContourVerter);
            }

            physicalWedgeList.push_back(pPhysicalWedgeField);

        }

        ms::proto::RT_MS_PencilBeamVirtualWedgeInfo virtualWedgeInfo;
        for (int k = 0; k < pencilBeam.virtualwedgeinfolist_size(); ++k ){
            std::tr1::shared_ptr<VirtualWedgeInfo> pVirtualWedgeInfo(new VirtualWedgeInfo());
            virtualWedgeInfo = pencilBeam.virtualwedgeinfolist(k);
            pVirtualWedgeInfo->virtualUID = pencilBeam.virtualwedgeinfolist(k).virtualuid();
            pVirtualWedgeInfo->goldenUID = pencilBeam.virtualwedgeinfolist(k).goldenuid();

            ms::proto::RT_MS_PencilVirtualWedgeField virtualWedgeField;
            for (int m = 0; m < pencilBeam.virtualwedgeinfolist(k).virtualwedgefieldlist_size(); ++m){
                std::tr1::shared_ptr<VirtualWedgeField> pVirtualWedgeField(new VirtualWedgeField());
                virtualWedgeField = pencilBeam.virtualwedgeinfolist(k).virtualwedgefieldlist(m);
                pVirtualWedgeField->uid = virtualWedgeField.uid();
                pVirtualWedgeField->collimatorX1Position = virtualWedgeField.collimatorx1position();
                pVirtualWedgeField->collimatorX2Position = virtualWedgeField.collimatorx2position();
                pVirtualWedgeField->relativeMU = virtualWedgeField.relativemu();
                pVirtualWedgeField->correctionFactor = virtualWedgeField.correctionfactor();
                pVirtualWedgeInfo->virtualWedgeList.push_back(pVirtualWedgeField);
            }
            virtualWedgeList.push_back(pVirtualWedgeInfo);
        }

    }

    void ConvertBack(ms::proto::RT_MS_PencilBeam* pencilBeamInfo) const {

        //OutField
        pencilBeamInfo->set_diamone(diamOne);
        pencilBeamInfo->set_diamtwo(diamTwo);
        pencilBeamInfo->set_diamthree(diamThree);
        pencilBeamInfo->set_srcratio(srcRatio);
        pencilBeamInfo->set_srcratio3(srcRatio3);
        pencilBeamInfo->set_eeqdiam(eeqdiam);
        pencilBeamInfo->set_transmission(transmission);
        pencilBeamInfo->set_psd(psd);
        pencilBeamInfo->set_sddxjaw(sddXJaw);
        pencilBeamInfo->set_sddyjaw(sddYJaw);

        //Jaw
        pencilBeamInfo->set_xk0(xk0);
        pencilBeamInfo->set_xk1(xk1);
        pencilBeamInfo->set_xk2(xk2);
        pencilBeamInfo->set_yk0(yk0);
        pencilBeamInfo->set_yk1(yk1);
        pencilBeamInfo->set_yk2(yk2);
        pencilBeamInfo->set_wedgeid(wedgeId);

        //PencilBeamScCorrection
        pencilBeamInfo->set_scxpositionsproto(scXPositions);
        pencilBeamInfo->set_scypositionsproto(scYPositions);
        pencilBeamInfo->set_scblocbproto(scBlocb);
        pencilBeamInfo->set_referencefluence(referencefluence);

        //PencilPDDTMR
        ms::proto::RT_MS_PencilPDDTMR* pTMR = nullptr;
        for (auto tmr = tMRInfoList.begin(); tmr <tMRInfoList.end(); ++tmr){
            pTMR = pencilBeamInfo->add_tmrlistlist();
            pTMR->set_uid((*tmr)->uid);
            pTMR->set_depth((*tmr)->depth);
            pTMR->set_value((*tmr)->value);
        }

        //PencilPDDSMRInfo
        pencilBeamInfo->set_depthvec(depthvec);
        pencilBeamInfo->set_radiusvec(radiusvec);
        pencilBeamInfo->set_sar(sar);


        //PencilOffAxisVector
        pencilBeamInfo->set_flatnessradiusvec(flatnessradiusvec);
        pencilBeamInfo->set_flatnessdepthvec(flatnessdepthvec);
        pencilBeamInfo->set_flatness(flatness);


        //PhysicalWedge
        ms::proto::RT_MS_PencilBeamPhysicalWedgeField* pPhysicalWedge = nullptr;
        for (int i = 0; i < physicalWedgeList.size(); ++i){
            pPhysicalWedge = pencilBeamInfo->add_physicalwedgefieldlist();

            pPhysicalWedge->set_wedgeuid(physicalWedgeList[i]->wedgeUID);
            pPhysicalWedge->set_wedgeid(physicalWedgeList[i]->wedgeId);
            pPhysicalWedge->set_wedgetype(physicalWedgeList[i]->wedgeType);
            pPhysicalWedge->set_mu(physicalWedgeList[i]->mu);
            pPhysicalWedge->set_dmudx(physicalWedgeList[i]->dmudx);
            pPhysicalWedge->set_dmudr(physicalWedgeList[i]->dmudr);
            pPhysicalWedge->set_dmuda(physicalWedgeList[i]->dmuda);
            pPhysicalWedge->set_hvlslope(physicalWedgeList[i]->hvlSlope);
            pPhysicalWedge->set_wedgethick(physicalWedgeList[i]->wedgeThick);
            pPhysicalWedge->set_density(physicalWedgeList[i]->density);

            pPhysicalWedge->set_carrieruid(physicalWedgeList[i]->carrierUID);
            pPhysicalWedge->set_carriermu(physicalWedgeList[i]->carriermu);
            pPhysicalWedge->set_carrierdmudx(physicalWedgeList[i]->carrierdmudx);
            pPhysicalWedge->set_carrierdmudr(physicalWedgeList[i]->carrierdmudr);
            pPhysicalWedge->set_carrierdmuda(physicalWedgeList[i]->carrierdmuda);
            pPhysicalWedge->set_carrierhvlslope(physicalWedgeList[i]->carrierhvlSlope);
            pPhysicalWedge->set_carrierthick(physicalWedgeList[i]->carrierThick);
            pPhysicalWedge->set_contouruid1(physicalWedgeList[i]->contourUID1);
            pPhysicalWedge->set_contouruid2(physicalWedgeList[i]->contourUID2);

            ms::proto::RT_MS_PencilContourVerter* conContour = nullptr;
            for (int j = 0; j < physicalWedgeList[i]->wedgeContourVerterList.size(); ++j){
                conContour = pPhysicalWedge->add_weightcontourverterlist();
                conContour->set_uid(physicalWedgeList[i]->wedgeContourVerterList[j]->uid);
                conContour->set_contourverterx(physicalWedgeList[i]->wedgeContourVerterList[j]->contourVerterx);
                conContour->set_contourvertery(physicalWedgeList[i]->wedgeContourVerterList[j]->contourVertery);
            }

            for (int k = 0; k < physicalWedgeList[i]->carrierContourVerterList.size(); ++k){
                conContour = pPhysicalWedge->add_carriercontourverterlist();
                conContour->set_uid(physicalWedgeList[i]->carrierContourVerterList[k]->uid);
                conContour->set_contourverterx(physicalWedgeList[i]->carrierContourVerterList[k]->contourVerterx);
                conContour->set_contourvertery(physicalWedgeList[i]->carrierContourVerterList[k]->contourVertery);
            }
        }

        ms::proto::RT_MS_PencilBeamVirtualWedgeInfo* pVirtualWedgeInfo = nullptr;
        for (int i = 0; i < virtualWedgeList.size(); ++i){

            pVirtualWedgeInfo = pencilBeamInfo->add_virtualwedgeinfolist();
            pVirtualWedgeInfo->set_virtualuid(virtualWedgeList[i]->virtualUID);
            pVirtualWedgeInfo->set_goldenuid(virtualWedgeList[i]->goldenUID);

            for (int j = 0; j < virtualWedgeList[i]->virtualWedgeList.size(); ++j){
                ms::proto::RT_MS_PencilVirtualWedgeField* pVirtualWedgeField = nullptr;
                pVirtualWedgeField = pVirtualWedgeInfo->add_virtualwedgefieldlist();
                pVirtualWedgeField->set_uid(virtualWedgeList[i]->virtualWedgeList[j]->uid);
                pVirtualWedgeField->set_collimatorx1position(virtualWedgeList[i]->virtualWedgeList[j]->collimatorX1Position);
                pVirtualWedgeField->set_collimatorx2position(virtualWedgeList[i]->virtualWedgeList[j]->collimatorX2Position);
                pVirtualWedgeField->set_relativemu(virtualWedgeList[i]->virtualWedgeList[j]->relativeMU);
                pVirtualWedgeField->set_correctionfactor(virtualWedgeList[i]->virtualWedgeList[j]->correctionFactor);
            }
        }

    }
};

TPS_END_NAMESPACE


#endif
