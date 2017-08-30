/////////////////////////////////////////////////////////////////////////
/// \defgroup MS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  linqi.liu linqi.liu@united-imaging.com
///
///  \file    rt_ms_certifiedmontecarlo.h
///  \brief   CertifiedMonteCarlo
///
///  \version 1.0
///  \date    3 06, 2015
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef RT_MS_CERTIFIEDMONTECARLO_H_
#define RT_MS_CERTIFIEDMONTECARLO_H_

#include "tps_defs.h"
#include <memory>

#include "RtTpsProtoLib/rt_ms_montecarlo.pb.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedwedgefield.h"

TPS_BEGIN_NAMESPACE

typedef struct CertifiedSpectrum
{
    std::string uid;
    float energy;
    float weight;

}MonteCarloSpectrum;

typedef struct CertifiedFieldSize
{
    std::string uid;
    float fieldSize;
    float absoluteDoseCorrectionFactor;
}MonteCarloFieldSize;

struct CertifiedMonteCarlo
{
    std::string parentUID;
    std::string uid;
    std::string accessory;
    float xJawTopPosition;
    float yJawTopPosition;
    float xJawThickness;
    float yJawThickness;

    float mlcCenterPosition;
    float mlcThickness;
    float mlcLeafendRadius;
    float airGapWidth;
    float tongueGrooveWidth;
    float muCorrectionFactor;
    float minElectronEnergy;
    float maxElectronEnergy;

    std::string phoFilePath;
    float normTen ;

    std::string oarDepth1;
    std::string oarDepth2;
    std::string energyValue;

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

    std::string wedgeId;

    std::vector< std::tr1::shared_ptr<CertifiedSpectrum> > spectrumList;
    std::vector< std::tr1::shared_ptr<CertifiedFieldSize> > fieldSizeList;

    std::vector< std::tr1::shared_ptr<CommonPhysicalWedgeField> > physicalWedgeList;
    std::vector< std::tr1::shared_ptr<VirtualWedgeInfo> > virtualWedgeList;

    void Convert(const ms::proto::RT_MS_MonteCarlo &monteCarloInfo) {
        if (monteCarloInfo.has_uid()){
            uid = monteCarloInfo.uid();
        }
        if (monteCarloInfo.has_parentuid()){
            parentUID = monteCarloInfo.parentuid();
        }
        if (monteCarloInfo.has_accessoryuid()){
            accessory = monteCarloInfo.accessoryuid();
        }
        if (monteCarloInfo.has_filepath()){
            phoFilePath = monteCarloInfo.filepath();
        }
        if (monteCarloInfo.has_xjawtopposition()){
            xJawTopPosition = monteCarloInfo.xjawtopposition();
        }
        if (monteCarloInfo.has_yjawtopposition()){
            monteCarloInfo.yjawtopposition();
        }
        if (monteCarloInfo.has_xjawthickness()){
            xJawThickness = monteCarloInfo.xjawthickness();
        }
        if (monteCarloInfo.has_yjawthickness()){
            yJawThickness = monteCarloInfo.yjawthickness();
        }
        if (monteCarloInfo.has_mlccenterposition()){
            mlcCenterPosition = monteCarloInfo.mlccenterposition();
        }
        if (monteCarloInfo.has_mlcthickness()){
            mlcThickness = monteCarloInfo.mlcthickness();
        }
        if (monteCarloInfo.has_mlcleafendradius()){
            mlcLeafendRadius = monteCarloInfo.mlcleafendradius();
        }
        if (monteCarloInfo.has_airgapwidth()){
            airGapWidth = monteCarloInfo.airgapwidth();
        }
        if (monteCarloInfo.has_tonguegroovewidth()){
            tongueGrooveWidth = monteCarloInfo.tonguegroovewidth();
        }
        if (monteCarloInfo.has_mucorrectionfactor()){
            muCorrectionFactor = monteCarloInfo.mucorrectionfactor();
        }
        if (monteCarloInfo.has_minelectronenergy()){
            minElectronEnergy = monteCarloInfo.minelectronenergy();
        }
        if (monteCarloInfo.has_maxelectronenergy()){
            maxElectronEnergy = monteCarloInfo.maxelectronenergy();
        }
        if (monteCarloInfo.has_normten()){
            normTen = monteCarloInfo.normten();
        }
        if (monteCarloInfo.has_oardepth1()){
            oarDepth1 = monteCarloInfo.oardepth1();
        }
        if (monteCarloInfo.has_oardepth2()){
            oarDepth2 = monteCarloInfo.oardepth2();
        }
        if (monteCarloInfo.has_energyvalue()){
            energyValue = monteCarloInfo.energyvalue();
        }
        if (monteCarloInfo.has_electroncontaminationflag()){
            electronContaminationFlag = monteCarloInfo.electroncontaminationflag();
        }
        if (monteCarloInfo.has_electronmaxdepth()){
            electronMaxDepth = monteCarloInfo.electronmaxdepth();
        }
        if (monteCarloInfo.has_surfacedose()){
            surfaceDose = monteCarloInfo.surfacedose();
        }
        if (monteCarloInfo.has_depthcoefficientk()){
            depthCoefficientK = monteCarloInfo.depthcoefficientk();
        }
        if (monteCarloInfo.has_offaxiscoefficienta()){
            offaxisCoefficientA = monteCarloInfo.offaxiscoefficienta();
        }
        if (monteCarloInfo.has_df()){
            df = monteCarloInfo.df();
        }
        if (monteCarloInfo.has_sf()){
            sf = monteCarloInfo.sf();
        }
        if (monteCarloInfo.has_cone()){
            cOne = monteCarloInfo.cone();
        }
        if (monteCarloInfo.has_ctwo()){
            cTwo = monteCarloInfo.ctwo();
        }
        if (monteCarloInfo.has_cthree()){
            cThree = monteCarloInfo.cthree();
        }
        if (monteCarloInfo.has_wedgeid()){
            wedgeId = monteCarloInfo.wedgeid();
        }

        //Spectrum
        ms::proto::RT_MS_Spectrum spectrum;
        for (int i = 0; i < monteCarloInfo.spectrumlist_size();++i){
            std::tr1::shared_ptr<MonteCarloSpectrum> pMonteCarloSp(new MonteCarloSpectrum());
            spectrum = monteCarloInfo.spectrumlist(i);
            pMonteCarloSp->uid = spectrum.uid();
            pMonteCarloSp->energy = spectrum.energy();
            pMonteCarloSp->weight = spectrum.weight();
            spectrumList.push_back(pMonteCarloSp);
        }

        //FieldSize
        ms::proto::RT_MS_FieldSize fieldSize;
        for (int i = 0; i < monteCarloInfo.fieldsizelist_size();++i){
            std::tr1::shared_ptr<MonteCarloFieldSize> pfieldSize(new MonteCarloFieldSize());
            fieldSize = monteCarloInfo.fieldsizelist(i);
            pfieldSize->uid = fieldSize.uid();
            pfieldSize->fieldSize = fieldSize.fieldsize();
            pfieldSize->absoluteDoseCorrectionFactor = fieldSize.absolutefactor();
            fieldSizeList.push_back(pfieldSize);
        }

        // RT_MS_PhysicalWedgeField
        ms::proto::RT_MS_MonteCarloPhysicalWedgeField physicalWedgeField;
        for (int i = 0; i < monteCarloInfo.physicalwedgefieldlist_size();++i){
            std::tr1::shared_ptr<PhysicalWedgeField> pPhysicalWedgeField(new PhysicalWedgeField());
            physicalWedgeField = monteCarloInfo.physicalwedgefieldlist(i);

            pPhysicalWedgeField->wedgeUID = monteCarloInfo.physicalwedgefieldlist(i).wedgeuid();
            pPhysicalWedgeField->wedgeId = monteCarloInfo.physicalwedgefieldlist(i).wedgeid();
            pPhysicalWedgeField->wedgeType = monteCarloInfo.physicalwedgefieldlist(i).wedgetype();
            pPhysicalWedgeField->mu = monteCarloInfo.physicalwedgefieldlist(i).mu();
            pPhysicalWedgeField->dmudx = monteCarloInfo.physicalwedgefieldlist(i).dmudx();
            pPhysicalWedgeField->dmudr = monteCarloInfo.physicalwedgefieldlist(i).dmudr();
            pPhysicalWedgeField->dmuda = monteCarloInfo.physicalwedgefieldlist(i).dmuda();
            pPhysicalWedgeField->hvlSlope = monteCarloInfo.physicalwedgefieldlist(i).hvlslope();
            pPhysicalWedgeField->wedgeThick = monteCarloInfo.physicalwedgefieldlist(i).wedgethick();
            //pPhysicalWedgeField->density = monteCarloInfo.physicalwedgefieldlist(i)

            pPhysicalWedgeField->carrierUID = monteCarloInfo.physicalwedgefieldlist(i).carrieruid();
            pPhysicalWedgeField->carriermu = monteCarloInfo.physicalwedgefieldlist(i).carriermu();
            pPhysicalWedgeField->carrierdmudx = monteCarloInfo.physicalwedgefieldlist(i).carrierdmudx();
            pPhysicalWedgeField->carrierdmudr = monteCarloInfo.physicalwedgefieldlist(i).carrierdmudr();
            pPhysicalWedgeField->carrierdmuda = monteCarloInfo.physicalwedgefieldlist(i).carrierdmuda();
            pPhysicalWedgeField->carrierhvlSlope = monteCarloInfo.physicalwedgefieldlist(i).carrierhvlslope();
            pPhysicalWedgeField->carrierThick = monteCarloInfo.physicalwedgefieldlist(i).carrierthick();

            pPhysicalWedgeField->contourUID1 = monteCarloInfo.physicalwedgefieldlist(i).contouruid1();
            pPhysicalWedgeField->contourUID2 = monteCarloInfo.physicalwedgefieldlist(i).contouruid2();

            ms::proto::RT_MS_ContourVerter contourVerter;
            for (int j = 0; j < monteCarloInfo.physicalwedgefieldlist(i).weightcontourverterlist_size();++j ){
                std::tr1::shared_ptr<CommonContourVerter> pPhysicalContourVerter(new CommonContourVerter());
                contourVerter = monteCarloInfo.physicalwedgefieldlist(i).weightcontourverterlist(j);
                pPhysicalContourVerter->uid = contourVerter.uid();
                pPhysicalContourVerter->contourVerterx = contourVerter.contourverterx();
                pPhysicalContourVerter->contourVertery = contourVerter.contourvertery();
                pPhysicalWedgeField->wedgeContourVerterList.push_back(pPhysicalContourVerter);
            }

            for (int j = 0; j < monteCarloInfo.physicalwedgefieldlist(i).carriercontourverterlist_size();++j ){
                std::tr1::shared_ptr<CommonContourVerter> pCarrierContourVerter(new CommonContourVerter());
                contourVerter = monteCarloInfo.physicalwedgefieldlist(i).carriercontourverterlist(j);
                pCarrierContourVerter->uid = contourVerter.uid();
                pCarrierContourVerter->contourVerterx = contourVerter.contourverterx();
                pCarrierContourVerter->contourVertery = contourVerter.contourvertery();
                pPhysicalWedgeField->carrierContourVerterList.push_back(pCarrierContourVerter);
            }

            physicalWedgeList.push_back(pPhysicalWedgeField);

        }

        ms::proto::RT_MS_MonteCarloVirtualWedgeInfo virtualWedgeInfo;
        for (int k = 0; k < monteCarloInfo.virtualweidgefieldlist_size(); ++k ){
            std::tr1::shared_ptr<VirtualWedgeInfo> pVirtualWedgeInfo(new VirtualWedgeInfo());
            virtualWedgeInfo = monteCarloInfo.virtualweidgefieldlist(k);
            pVirtualWedgeInfo->virtualUID = monteCarloInfo.virtualweidgefieldlist(k).virtualuid();
            pVirtualWedgeInfo->goldenUID =  monteCarloInfo.virtualweidgefieldlist(k).goldenuid();

            ms::proto::RT_MS_MonteCarloVirtualWedgeField virtualWedgeField;
            for (int m = 0; m < monteCarloInfo.virtualweidgefieldlist(k).virtualwedgefieldlist_size(); ++m){
                std::tr1::shared_ptr<VirtualWedgeField> pVirtualWedgeField(new VirtualWedgeField());
                virtualWedgeField = monteCarloInfo.virtualweidgefieldlist(k).virtualwedgefieldlist(m);
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

    void ConvertBack(ms::proto::RT_MS_MonteCarlo* monteCarloInfo) const{

        monteCarloInfo->set_uid(uid);
        monteCarloInfo->set_parentuid(parentUID);
        monteCarloInfo->set_accessoryuid(accessory);
        monteCarloInfo->set_filepath(phoFilePath);
        monteCarloInfo->set_xjawtopposition(xJawTopPosition);
        monteCarloInfo->set_yjawtopposition(yJawTopPosition);
        monteCarloInfo->set_xjawthickness(xJawThickness);
        monteCarloInfo->set_yjawthickness(yJawThickness);
        monteCarloInfo->set_mlccenterposition(mlcCenterPosition);
        monteCarloInfo->set_mlcleafendradius(mlcLeafendRadius);
        monteCarloInfo->set_mlcthickness(mlcThickness);
        monteCarloInfo->set_airgapwidth(airGapWidth);
        monteCarloInfo->set_tonguegroovewidth(tongueGrooveWidth);
        monteCarloInfo->set_mucorrectionfactor(muCorrectionFactor);
        monteCarloInfo->set_minelectronenergy(minElectronEnergy);
        monteCarloInfo->set_maxelectronenergy(maxElectronEnergy);
        monteCarloInfo->set_normten(normTen);
        monteCarloInfo->set_oardepth1(oarDepth1);
        monteCarloInfo->set_oardepth2(oarDepth2);
        monteCarloInfo->set_energyvalue(energyValue);
        monteCarloInfo->set_electroncontaminationflag(electronContaminationFlag);
        monteCarloInfo->set_electronmaxdepth(electronMaxDepth);
        monteCarloInfo->set_surfacedose(surfaceDose);
        monteCarloInfo->set_depthcoefficientk(depthCoefficientK);
        monteCarloInfo->set_offaxiscoefficienta(offaxisCoefficientA);
        monteCarloInfo->set_df(df);
        monteCarloInfo->set_sf(sf);
        monteCarloInfo->set_cone(cOne);
        monteCarloInfo->set_ctwo(cTwo);
        monteCarloInfo->set_cthree(cThree);
        monteCarloInfo->set_wedgeid(wedgeId);

        //Spectrum
        ms::proto::RT_MS_Spectrum* pSpectrum = nullptr;
        for (auto spectrum = spectrumList.begin(); spectrum <spectrumList.end(); ++spectrum){
            pSpectrum = monteCarloInfo->add_spectrumlist();
            pSpectrum->set_uid((*spectrum)->uid);
            pSpectrum->set_energy((*spectrum)->energy);
            pSpectrum->set_weight((*spectrum)->weight);
        }

        //FieldSize
        ms::proto::RT_MS_FieldSize* pFieldSize = nullptr;
        for (auto fieldSize = fieldSizeList.begin(); fieldSize <fieldSizeList.end(); ++fieldSize){
            pFieldSize = monteCarloInfo->add_fieldsizelist();
            pFieldSize->set_uid((*fieldSize)->uid);
            pFieldSize->set_fieldsize((*fieldSize)->fieldSize);
            pFieldSize->set_absolutefactor((*fieldSize)->absoluteDoseCorrectionFactor);
        }

        //PhysicalWedge
        ms::proto::RT_MS_MonteCarloPhysicalWedgeField* pPhysicalWedge = nullptr;
        for (int i = 0; i < physicalWedgeList.size(); ++i){
            pPhysicalWedge = monteCarloInfo->add_physicalwedgefieldlist();

            pPhysicalWedge->set_wedgeuid(physicalWedgeList[i]->wedgeUID);
            pPhysicalWedge->set_wedgeid(physicalWedgeList[i]->wedgeId);
            pPhysicalWedge->set_wedgetype(physicalWedgeList[i]->wedgeType);
            pPhysicalWedge->set_mu(physicalWedgeList[i]->mu);
            pPhysicalWedge->set_dmudx(physicalWedgeList[i]->dmudx);
            pPhysicalWedge->set_dmudr(physicalWedgeList[i]->dmudr);
            pPhysicalWedge->set_dmuda(physicalWedgeList[i]->dmuda);
            pPhysicalWedge->set_hvlslope(physicalWedgeList[i]->hvlSlope);
            pPhysicalWedge->set_wedgethick(physicalWedgeList[i]->wedgeThick);

            pPhysicalWedge->set_carrieruid(physicalWedgeList[i]->carrierUID);
            pPhysicalWedge->set_carriermu(physicalWedgeList[i]->carriermu);
            pPhysicalWedge->set_carrierdmudx(physicalWedgeList[i]->carrierdmudx);
            pPhysicalWedge->set_carrierdmudr(physicalWedgeList[i]->carrierdmudr);
            pPhysicalWedge->set_carrierdmuda(physicalWedgeList[i]->carrierdmuda);
            pPhysicalWedge->set_carrierhvlslope(physicalWedgeList[i]->carrierhvlSlope);
            pPhysicalWedge->set_carrierthick(physicalWedgeList[i]->carrierThick);

            pPhysicalWedge->set_contouruid1(physicalWedgeList[i]->contourUID1);
            pPhysicalWedge->set_contouruid2(physicalWedgeList[i]->contourUID2);

            ms::proto::RT_MS_ContourVerter* conContour = nullptr;
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

        ms::proto::RT_MS_MonteCarloVirtualWedgeInfo* pVirtualWedgeInfo = nullptr;
        for (int i = 0; i < virtualWedgeList.size(); ++i){

            pVirtualWedgeInfo = monteCarloInfo->add_virtualweidgefieldlist();
            pVirtualWedgeInfo->set_virtualuid(virtualWedgeList[i]->virtualUID);
            pVirtualWedgeInfo->set_goldenuid(virtualWedgeList[i]->goldenUID);

            for (int j = 0; j < virtualWedgeList[i]->virtualWedgeList.size(); ++j){
                ms::proto::RT_MS_MonteCarloVirtualWedgeField* pVirtualWedgeField = nullptr;
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
