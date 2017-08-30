/////////////////////////////////////////////////////////////////////////
/// \defgroup MS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  linqi.liu linqi.liu@united-imaging.com
///
///  \file    rt_ms_certifiedconvolution.h
///  \brief   CertifiedConvolution
///
///  \version 1.0
///  \date    1 15, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef RT_MS_CERTIFIEDCONVOLUTION_H_
#define RT_MS_CERTIFIEDCONVOLUTION_H_

#include "tps_defs.h"
#include <memory>

#include "RtTpsCertifiedLib/rt_tps_certifiedwedgefield.h"
#include "RtTpsProtoLib/rt_ms_convolution.pb.h"

TPS_BEGIN_NAMESPACE

typedef struct CertifiedPDDSpectrum
{
    std::string uid0;
    std::string uid40;
    int energy0;
    int energy40;
    int fieldSize;
    float kernelWeight0;
    float kernelWeight40;
}Spectrum;


typedef struct CertifiedInFieldOffaxisVector
{
    std::string uid;
    float radius;
    float offaxisRatio;
}InFieldVector;

struct CertifiedConvolution
{
    std::string parentUID;
    //OutField
    float diamOne;
    float diamTwo;
    float srcRatio;
    float transmission;
    float psd;
    float srcDepth;
    float srcAtten;
    float sddXJaw;
    float sddYJaw;
    float normTenMultiTen;

    //Jaw
    float xk0;
    float xk1;
    float xk2;
    float yk0;
    float yk1;
    float yk2;

    //Electron
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

    std::string commissionUID;
    std::string accName;
    std::string accessoryUID;
    std::string wedgeId;
    int algType;

    //Pdd
    float pddMuBase;
    float pddDmumx;
    float dmudr;

    std::string mudatax;
    std::string ddmudr;
    std::string mudatay;
    std::string dmudatay;
    std::string outputmuy;

    std::vector< std::tr1::shared_ptr<CertifiedPDDSpectrum> > spectrumList;
    std::vector< std::tr1::shared_ptr<CertifiedInFieldOffaxisVector> > inFieldVectorList;

    std::vector< std::tr1::shared_ptr<CommonPhysicalWedgeField> >physicalWedgeList;
    std::vector< std::tr1::shared_ptr<CertifiedVirtualWedgeInfo> >virtualWedgeList;

    void Convert(const  ms::proto::RT_MS_Convolution &convolutionInfo)
    {
            algType = convolutionInfo.algtype();
            diamOne = convolutionInfo.diamone();
            diamTwo = convolutionInfo.diamtwo();
            srcRatio = convolutionInfo.srcratio();
            transmission = convolutionInfo.transmission();
            psd = convolutionInfo.psd();
            srcDepth = convolutionInfo.srcdepth();
            srcAtten = convolutionInfo.srcatten();
            sddXJaw = convolutionInfo.sddxjaw();
            sddYJaw = convolutionInfo.sddyjaw();
            normTenMultiTen = convolutionInfo.normtenmultiten();
            xk0 = convolutionInfo.xk0();
            xk1 = convolutionInfo.xk1();
            xk2 = convolutionInfo.xk2();
            yk0 = convolutionInfo.yk0();
            yk1 = convolutionInfo.yk1();
            yk2 = convolutionInfo.yk2();
            wedgeId = convolutionInfo.wedgeid();
            electronContaminationFlag = convolutionInfo.electroncontaminationflag();
            electronMaxDepth = convolutionInfo.electronmaxdepth();
            surfaceDose = convolutionInfo.surfacedose();
            depthCoefficientK = convolutionInfo.depthcoefficientk();
            offaxisCoefficientA = convolutionInfo.offaxiscoefficienta();
            df = convolutionInfo.df();
            sf = convolutionInfo.sf();
            cOne = convolutionInfo.cone();
            cTwo = convolutionInfo.ctwo();
            cThree = convolutionInfo.cthree();
            commissionUID = convolutionInfo.commissionuid();
            accName = convolutionInfo.accname();
            accessoryUID = convolutionInfo.accessoryuid();

            pddMuBase = convolutionInfo.pddmubase();
            pddDmumx = convolutionInfo.pdddmumx();

            if (convolutionInfo.has_dmudr())
            {
                dmudr = convolutionInfo.dmudr();
            }
            if (convolutionInfo.has_mudatax())
            {
                mudatax = convolutionInfo.mudatax();
            }
            if (convolutionInfo.has_ddmudr())
            {
                ddmudr = convolutionInfo.ddmudr();
            }
            if (convolutionInfo.has_mudatay())
            {
                mudatay = convolutionInfo.mudatay();
            }
            if (convolutionInfo.has_dmudatay())
            {
                dmudatay = convolutionInfo.dmudatay();
            }
            if (convolutionInfo.has_outputmuy())
            {
                outputmuy = convolutionInfo.outputmuy();
            }



        //Spectrum
        ms::proto::RT_MS_PDDSpectrum spectrum;
        for (int i = 0; i < convolutionInfo.spectrumlist_size();++i){
            std::tr1::shared_ptr<Spectrum> pSpectrum(new Spectrum());
            spectrum = convolutionInfo.spectrumlist(i);
            pSpectrum->uid0 = spectrum.uid0();
            pSpectrum->uid40 = spectrum.uid40();
            pSpectrum->energy0 = spectrum.energy0();
            pSpectrum->energy40 = spectrum.energy40();
            pSpectrum->kernelWeight0 = spectrum.kernelweight0();
            pSpectrum->kernelWeight40 = spectrum.kernelweight40();
            spectrumList.push_back(pSpectrum);
        }

        //InFieldOffaxisVector
        ms::proto::RT_MS_InFieldOffaxisVector inFieldVector;
        for (int i = 0; i < convolutionInfo.infieldoffaxisvector_size();++i){
            std::tr1::shared_ptr<InFieldVector> pInFieldVector(new InFieldVector());
            inFieldVector = convolutionInfo.infieldoffaxisvector(i);
            pInFieldVector->uid = inFieldVector.uid();
            pInFieldVector->radius = inFieldVector.radius();
            pInFieldVector->offaxisRatio = inFieldVector.offaxisratio();
            inFieldVectorList.push_back(pInFieldVector);
        }

       // RT_MS_PhysicalWedgeField
         ms::proto::RT_MS_PhysicalWedgeField physicalWedgeField;
         for (int i = 0; i < convolutionInfo.physicalwedgefieldlist_size();++i){
             std::tr1::shared_ptr<PhysicalWedgeField> pPhysicalWedgeField(new PhysicalWedgeField());
             physicalWedgeField = convolutionInfo.physicalwedgefieldlist(i);

             pPhysicalWedgeField->wedgeUID = convolutionInfo.physicalwedgefieldlist(i).wedgeuid();
             pPhysicalWedgeField->wedgeId = convolutionInfo.physicalwedgefieldlist(i).wedgeid();
             pPhysicalWedgeField->wedgeType = convolutionInfo.physicalwedgefieldlist(i).wedgetype();
             pPhysicalWedgeField->mu = convolutionInfo.physicalwedgefieldlist(i).mu();
             pPhysicalWedgeField->dmudx = convolutionInfo.physicalwedgefieldlist(i).dmudx();
             pPhysicalWedgeField->dmudr = convolutionInfo.physicalwedgefieldlist(i).dmudr();
             pPhysicalWedgeField->dmuda = convolutionInfo.physicalwedgefieldlist(i).dmuda();
             pPhysicalWedgeField->hvlSlope = convolutionInfo.physicalwedgefieldlist(i).hvlslope();
             pPhysicalWedgeField->wedgeThick = convolutionInfo.physicalwedgefieldlist(i).wedgethick();
             pPhysicalWedgeField->density = convolutionInfo.physicalwedgefieldlist(i).density();

             pPhysicalWedgeField->carrierUID = convolutionInfo.physicalwedgefieldlist(i).carrierudi();
             pPhysicalWedgeField->carriermu = convolutionInfo.physicalwedgefieldlist(i).carriermu();
             pPhysicalWedgeField->carrierdmudx = convolutionInfo.physicalwedgefieldlist(i).carrierdmudx();
             pPhysicalWedgeField->carrierdmudr = convolutionInfo.physicalwedgefieldlist(i).carrierdmudr();
             pPhysicalWedgeField->carrierdmuda = convolutionInfo.physicalwedgefieldlist(i).carrierdmuda();
             pPhysicalWedgeField->carrierhvlSlope = convolutionInfo.physicalwedgefieldlist(i).carrierhvlslope();
             pPhysicalWedgeField->carrierThick = convolutionInfo.physicalwedgefieldlist(i).carrierthick();
             pPhysicalWedgeField->contourUID1 = convolutionInfo.physicalwedgefieldlist(i).contouruid1();
             pPhysicalWedgeField->contourUID2 = convolutionInfo.physicalwedgefieldlist(i).contouruid2();

             ms::proto::RT_MS_ConvolutionContourVerter contourVerter;
             
             for (int j = 0; j < convolutionInfo.physicalwedgefieldlist(i).weightcontourverterlist_size();++j ){
                 std::tr1::shared_ptr<CommonContourVerter> pPhysicalContourVerter(new CommonContourVerter());
                 contourVerter = convolutionInfo.physicalwedgefieldlist(i).weightcontourverterlist(j);
                 pPhysicalContourVerter->uid = contourVerter.uid();
                 pPhysicalContourVerter->contourVerterx = contourVerter.contourverterx();
                 pPhysicalContourVerter->contourVertery = contourVerter.contourvertery();
                 pPhysicalWedgeField->wedgeContourVerterList.push_back(pPhysicalContourVerter);
             }

             for (int j = 0; j < convolutionInfo.physicalwedgefieldlist(i).carriercontourverterlist_size();++j ){
                 std::tr1::shared_ptr<CommonContourVerter> pCarrierContourVerter(new CommonContourVerter());
                 contourVerter = convolutionInfo.physicalwedgefieldlist(i).carriercontourverterlist(j);
                 pCarrierContourVerter->uid = contourVerter.uid();
                 pCarrierContourVerter->contourVerterx = contourVerter.contourverterx();
                 pCarrierContourVerter->contourVertery = contourVerter.contourvertery();
                 pPhysicalWedgeField->carrierContourVerterList.push_back(pCarrierContourVerter);
             }

             physicalWedgeList.push_back(pPhysicalWedgeField);

         }

         ms::proto::RT_MS_VirtualWedgeInfo virtualWedgeInfo;
         for (int k = 0; k < convolutionInfo.virtualwedgeinfolist_size(); ++k ){
             std::tr1::shared_ptr<VirtualWedgeInfo> pVirtualWedgeInfo(new VirtualWedgeInfo());
             virtualWedgeInfo = convolutionInfo.virtualwedgeinfolist(k);
             pVirtualWedgeInfo->virtualUID = convolutionInfo.virtualwedgeinfolist(k).virtualuid();
             pVirtualWedgeInfo->goldenUID = convolutionInfo.virtualwedgeinfolist(k).goldenuid();
             ms::proto::RT_MS_VirtualWedgeField virtualWedgeField;
             for (int m = 0; m < convolutionInfo.virtualwedgeinfolist(k).virtualwedgefieldlist_size(); ++m){
                 std::tr1::shared_ptr<VirtualWedgeField> pVirtualWedgeField(new VirtualWedgeField());
                 virtualWedgeField = convolutionInfo.virtualwedgeinfolist(k).virtualwedgefieldlist(m);
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

    void ConvertBack( ms::proto::RT_MS_Convolution* convolutionInfo) const{

        //OutField
        convolutionInfo->set_algtype(algType);
        convolutionInfo->set_diamone(diamOne);
        convolutionInfo->set_diamtwo(diamTwo);
        convolutionInfo->set_srcratio(srcRatio);
        convolutionInfo->set_transmission(transmission);
        convolutionInfo->set_psd(psd);
        convolutionInfo->set_srcdepth(srcDepth);
        convolutionInfo->set_srcatten(srcAtten);
        convolutionInfo->set_sddxjaw(sddXJaw);
        convolutionInfo->set_sddyjaw(sddYJaw);
        convolutionInfo->set_normtenmultiten(normTenMultiTen);

        //Jaw
        convolutionInfo->set_xk0(xk0);
        convolutionInfo->set_xk1(xk1);
        convolutionInfo->set_xk2(xk2);
        convolutionInfo->set_yk0(yk0);
        convolutionInfo->set_yk1(yk1);
        convolutionInfo->set_yk2(yk2);

        //Electron
        convolutionInfo->set_electroncontaminationflag(electronContaminationFlag);
        convolutionInfo->set_electronmaxdepth(electronMaxDepth);
        convolutionInfo->set_surfacedose(surfaceDose);
        convolutionInfo->set_depthcoefficientk(depthCoefficientK);
        convolutionInfo->set_offaxiscoefficienta(offaxisCoefficientA);
        convolutionInfo->set_df(df);
        convolutionInfo->set_sf (sf);
        convolutionInfo->set_cone(cOne);
        convolutionInfo->set_ctwo(cTwo);
        convolutionInfo->set_cthree(cThree);
        convolutionInfo->set_wedgeid(wedgeId);

        //Pdd
        convolutionInfo->set_pddmubase(pddMuBase);
        convolutionInfo->set_pdddmumx(pddDmumx);

        convolutionInfo->set_dmudr(dmudr);
        convolutionInfo->set_mudatax(mudatax);
        convolutionInfo->set_ddmudr(ddmudr);
        convolutionInfo->set_mudatay(mudatay);
        convolutionInfo->set_dmudatay(dmudatay);
        convolutionInfo->set_outputmuy(outputmuy);

        //Spectrum
        ms::proto::RT_MS_PDDSpectrum* pSpectrum = nullptr;
        for (auto spectrum = spectrumList.begin(); spectrum != spectrumList.end(); ++spectrum){
            pSpectrum = convolutionInfo->add_spectrumlist();
            pSpectrum->set_uid0((*spectrum)->uid0);
            pSpectrum->set_uid40((*spectrum)->uid40);
            pSpectrum->set_fieldsize((*spectrum)->fieldSize);
            pSpectrum->set_energy0((*spectrum)->energy0);
            pSpectrum->set_energy40((*spectrum)->energy40);
            pSpectrum->set_kernelweight0((*spectrum)->kernelWeight0);
            pSpectrum->set_kernelweight40((*spectrum)->kernelWeight40);
        }

        ////PDDAttenuationFactors
        //ms::proto::RT_MS_PDDAttenuationFactors* pAttenuationFactors = nullptr;
        //for (auto Factor = attenuationFactorsList.begin(); Factor != attenuationFactorsList.end(); ++Factor){
        //    pAttenuationFactors = convolutionInfo->add_attenuationfactorslist();
        //    pAttenuationFactors->set_uid((*Factor)->uid);
        //    pAttenuationFactors->set_fieldsize((*Factor)->fieldSize);
        //    //pAttenuationFactors->set_mubase((*Factor)->muBase);
        //    //pAttenuationFactors->set_dmudx((*Factor)->dmudx);
        //    pAttenuationFactors->set_dmubase((*Factor)->dmuBase);
        //    pAttenuationFactors->set_ddmudx((*Factor)->ddmudx);
        //    pAttenuationFactors->set_ddmudr((*Factor)->ddmudr);
        //    pAttenuationFactors->set_absolutedosecorrectionfactor((*Factor)->absoluteDoseCorrectionFactor);
        //}

        ////InFieldAttenuationFactor
        //ms::proto::RT_MS_InFieldAttenuationFactor* pInFieldFactor = nullptr;
        //for (auto InField = inFieldAttenuationFactorList.begin(); InField != inFieldAttenuationFactorList.end(); ++InField){
        //    pInFieldFactor = convolutionInfo->add_infieldattenuationfactorslist();
        //    pInFieldFactor->set_uid((*InField)->uid);
        //    pInFieldFactor->set_fieldsize((*InField)->fieldSize);
        //    pInFieldFactor->set_dmudr((*InField)->dmudr);
        //    pInFieldFactor->set_ddmudr((*InField)->ddmudr);
        //}

        //InFieldOffaxisVector
        ms::proto::RT_MS_InFieldOffaxisVector* pInFieldVector = nullptr;
        for (auto inFieldVector = inFieldVectorList.begin(); inFieldVector != inFieldVectorList.end(); ++inFieldVector){
            pInFieldVector = convolutionInfo->add_infieldoffaxisvector();
            pInFieldVector->set_uid((*inFieldVector)->uid);
            pInFieldVector->set_radius((*inFieldVector)->radius);
            pInFieldVector->set_offaxisratio((*inFieldVector)->offaxisRatio);
        }

        //PhysicalWedge
        ms::proto::RT_MS_PhysicalWedgeField* pPhysicalWedge = nullptr;
        for (int i = 0; i < physicalWedgeList.size(); ++i){
            pPhysicalWedge = convolutionInfo->add_physicalwedgefieldlist();

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

            pPhysicalWedge->set_carrierudi(physicalWedgeList[i]->carrierUID);
            pPhysicalWedge->set_carriermu(physicalWedgeList[i]->carriermu);
            pPhysicalWedge->set_carrierdmudx(physicalWedgeList[i]->carrierdmudx);
            pPhysicalWedge->set_carrierdmudr(physicalWedgeList[i]->carrierdmudr);
            pPhysicalWedge->set_carrierdmuda(physicalWedgeList[i]->carrierdmuda);
            pPhysicalWedge->set_carrierhvlslope(physicalWedgeList[i]->carrierhvlSlope);
            pPhysicalWedge->set_carrierthick(physicalWedgeList[i]->carrierThick);

            pPhysicalWedge->set_contouruid1(physicalWedgeList[i]->contourUID1);
            pPhysicalWedge->set_contouruid2(physicalWedgeList[i]->contourUID2);

             ms::proto::RT_MS_ConvolutionContourVerter* conContour = nullptr;
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

         ms::proto::RT_MS_VirtualWedgeInfo* pVirtualWedgeInfo = nullptr;
         for (int i = 0; i < virtualWedgeList.size(); ++i){

             pVirtualWedgeInfo = convolutionInfo->add_virtualwedgeinfolist();
             pVirtualWedgeInfo->set_virtualuid(virtualWedgeList[i]->virtualUID);
             pVirtualWedgeInfo->set_goldenuid(virtualWedgeList[i]->goldenUID);

             for (int j = 0; j < virtualWedgeList[i]->virtualWedgeList.size(); ++j){
                 ms::proto::RT_MS_VirtualWedgeField* pVirtualWedgeField = nullptr;
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
