//////////////////////////////////////////////////////////////////////////
/// \defgroup MS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  linqi.liu linqi.liu@united-imaging.com
///
///  \file    rt_ms_certifiedmachinesetting.h
///  \brief   CertifiedMachineSetting
///
///  \version 1.0
///  \date    Dec. 4, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef RT_MS_CERTIFIEDMACHINESETTING_H_
#define RT_MS_CERTIFIEDMACHINESETTING_H_

#include "tps_defs.h"

#include "RtTpsProtoLib/rt_ms_machinesettingconfig.pb.h"
#include "RtTpsProtoLib/rt_ms_photonsmontecarlo.pb.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_data.h"

TPS_BEGIN_NAMESPACE

typedef struct CertifiedPairInfo{
        std::string UID;
        int  pair ;
        double leafPairs;
}PairInfo;

typedef struct CertifiedDoseRate{
    std::string doseRateUID;
    float doseRate;
    bool isDoseRateDefault;
}DoseRate;

typedef struct CertifiedElectronApplicator{
    std::string applicatorACCID;
    float sizeX;
    float sizeY;
    std::string applicatorUID;
}ElectronApplicator;

typedef struct CertifiedWedgeInfo{
    std::string wedgeACCID;
    std::string wedgeACCcode;
    float degree;

    float leftmin;
    float leftmax;
    float rightmin;
    float rightmax;
    float topmin;
    float topmax;
    float bottommin;
    float bottommax;

    std::string  orientationType;
    std::string wedgeUID;
    int wedgeType;
}WedgeInfo;

typedef struct TrayInfo
{
    std::string trayUID;
    std::string trayID;
    std::string trayCode;
}Tray;

typedef struct FieldSizeAndAbsoluteDoseInfo
{
    float fieldSize;
    float absoluteDose;
}FieldSizeAndAbsoluteDose;

typedef struct SecondarySourceParameterInfo
{
    float flatFilterA;
    float flatFilterB;
}SecondarySourceParameter;

typedef struct EnergyAndWeightInfo
{
    float energy;
    float weight;
}EnergyAndWeight;

typedef struct FieldSizeAndCorrectionFactorInfo
{
    float fieldSize;
    float correctionFactor;
}FieldSizeAndCorrectionFactor;

typedef struct PhysicalWedgeParameterInfo
{
    std::string wedgeUID;
    float wedgeThickness;
    std::vector<db_Point3f> PointCollection;
    float carrierThickness;
    std::vector<db_Point3f> CarrierPointCollection;
}PhysicalWedgeParameter;


typedef struct CertifiedCommissionInfo{
    std::string parentUID;
    std::string commissionUID;
    std::string commissionName;
    int radiationType;
    float energy;
    int flatteningFilterType; 
    bool isConvolution;
    bool  isPencilBeam;
    bool  isMonteCarlo;
    bool  isDoseRateVarSupport;
    bool isPlan2D;                                        //这里修改2014.12.12
    bool isPlan3D;
    bool isPlanStepShoot;
    bool isPlanSlidingWindow;
    bool isPlanSimpleArc;
    bool isPlanConformalArc;
    bool isPlanVMATArc;
    bool isPlanBurstArc;
    int normGroupTechType;
    int electornType;
    bool isApprove;

    bool isCheckedHalfProfile;
    float calibrationFieldSizeX;
    float calibrationFieldSizeY;
    float calibrationDmax;
    float normalizeDose;
    float sSD;
    float lateral;
    float depth;
    std::string date;
    std::string time;
    std::string who;
    float penumbra;
    float blockAttenFactor;
    std::string phoCommissionUID;

    float xJawFactor;
    float yJawFactor;
    float MLCFactor;

    std::vector<float> trayFactorList;
    // std::vector<CertifiedDoseRate*>doseRateList;
    std::vector< std::tr1::shared_ptr<CertifiedDoseRate> > doseRateList;


    //PMC Part
      //1.PDD
    float primaryEnergyStart;
    float primaryEnergyEnd;
    int primaryEnergyBin;
    float primaryEnergyRestart;
    float secondaryEnergyStart;
    float secondaryEnergyEnd;
    int secondaryEnergyBin;
    float secondaryEnergyRestart;
    float electronEnergy;
    int particleNumber;
    std::vector<EnergyAndWeight> primarySpectrumCollection;
    std::vector<EnergyAndWeight> secondarySpectrumCollection;
    std::vector<EnergyAndWeight> electronSpectrumCollection;
    int completeFilesNumber;

      //2.In Field
    bool primarySoften;
    float softenParameter;
    int selectedSecondarySourceType;
    bool scatterSoften;
    std::vector<SecondarySourceParameter> secondarySourceCollection;
      //3.Out Field
    float flatDistance;
    float jawDistance;
    float sigmaP;
    float sigmaS;
    float tailCorrection;
    float rectangleScatterCorrection;
    float mlcLeakage;
    bool gaussianBlurSwitch;
    float gaussianSigmaX;
    float gaussianSigmaY;
      //4.Electron Contamination     
    float standardRadius;
    float sigma;
    float offset;
    float powerFactor;
      //5.Absoulate dose
    float norm10Dose;
	int  mindoserate_mupermin;
    std::vector<FieldSizeAndAbsoluteDose> absoluteDoseCollection;
      //6.Wedge Field
    std::vector<PhysicalWedgeParameter> physicalWedgeCollection;




}CommissionInfo;


struct CertifiedMachineSetting {
    //General
    std::string uid ;
    std::string machineName;
    std::string manufactory;
    std::string DeviceID ;
    std::string coordinationSystem ;
    std::string date ;
    std::string time ;
    std::string byWho ;
    bool isApprove;
    std::string comments;
    bool isActive;
    //Geometry
    float gantryAngleMin;
    float gantryAngleMax;
    float maxSpeedLimit;
    float sad;
    bool   isCW;
    bool   isCCW;
    float  cwLimit;
    float  ccwLimit;
    float collimatorLow;
    float collimatorHigh;
    float lateralMotionMax;
    float lateralMotionMin;
    float longitudeMotionMin;
    float longitudeMotionMax;
    float verticalMotionMax;
    float verticalMotionMin;
    float rTNMax;
    float rTNMIN;
    //Jaw
    bool isForceJawFollow;
    bool  isXjaw;
    bool isYjaw ;
    bool isXasym;
    bool isYasym;
    std::string leftJawLabel;
    std::string rightJawLabel;
    float  maxLeftJawPosition;
    float  minLeftJawPosition;
    float maxRightJawPosition;
    float minRightJawPosition;
    float lateralJawOutsideEdge;
    float lateralInsideJawEdge;
    float sourceLateralJawDistance;
    float jawGap;
    float MaxJawSpeed;
    std::string topJawLabel;
    std::string bottomJawLabel;
    float maxTopJawPosition;

    float minTopJawPosition;
    float maxBottomJawPosition;

    float minBottomJawPosition ;
    float longitudeJawOutsideEdge;

    float longitudeJawInsideEdge;
    float sourceLongitudeJawDistance;
    bool lateralTracking;
    bool longitudinalTranking;
    //MLC
    int pair;
    float sourceToMLCtopDistance;
    float minLeafJawOverlap;
    float minGap ;
    float maxLeafSpeed;
    float minTipPosition;
    float  maxTipPosition;
    bool  leafInterdigitation;
    float maxLeafoutofCarriageDistance;
    int   mlcDirection;
    //std::vector<CertifiedPairInfo*> pairInfoList;
    std::vector< std::tr1::shared_ptr<CertifiedPairInfo> > pairInfoList;

    //BeamMode
    std::vector<std::tr1::shared_ptr<CertifiedCommissionInfo> >commissionInfoList;
    //std::vector<CertifiedCommissionInfo*>commissionInfoList;                         //这里估计有问题。

    //Accessory
    std::string accessoryUID;
    bool  isApertureBlock;
    bool  isShieldBlock;
    float sourceToBlockTryDistance;
    float  height;
    float sourceToApplicatorLowDistance;

    std::vector<std::tr1::shared_ptr<CertifiedElectronApplicator> >electronApplicatorList;
    //std::vector<CertifiedElectronApplicator*> electronApplicatorList;
    //wedge
    float sourceToWedgeDistance;
    std::vector<std::tr1::shared_ptr<CertifiedWedgeInfo> >wedgeInfoList;
    // std::vector<CertifiedWedgeInfo*> wedgeInfoList;
    std::vector<std::string>trayUIDList;
    std::vector<std::string>trayAccCode;

     std::vector<std::tr1::shared_ptr<TrayInfo> >trayInfoList;
    //motorized
    int motorizedWedgeOrientation;
    float motorizedFieldSizeLimitX;
    float motorizedfieldSizeLimitY;

    void Convert(const ms::proto::RT_MS_MachineConfigInfo &machineconfigInfo) {
        //General
        if (machineconfigInfo.has_machinegeneralinfo()) {
            uid = machineconfigInfo.machinegeneralinfo().uid();
            machineName = machineconfigInfo.machinegeneralinfo().machinename();
            manufactory = machineconfigInfo.machinegeneralinfo().manufactory();
            DeviceID = machineconfigInfo.machinegeneralinfo().deviceid();
            coordinationSystem = machineconfigInfo.machinegeneralinfo().coordinationsystem();
            date = machineconfigInfo.machinegeneralinfo().date();
            time = machineconfigInfo.machinegeneralinfo().time();
            byWho = machineconfigInfo.machinegeneralinfo().bywho();
            isApprove = machineconfigInfo.isapprove();
            comments = machineconfigInfo.machinegeneralinfo().comments();
            isActive = machineconfigInfo.isactive();
        }

        //Geometry
        if (machineconfigInfo.has_machinegeometry()) {
            gantryAngleMin = machineconfigInfo.machinegeometry().gantryanglemin();
            gantryAngleMax = machineconfigInfo.machinegeometry().gantryanglemax();
            maxSpeedLimit = machineconfigInfo.machinegeometry().maxspeedlimit();
            sad = machineconfigInfo.machinegeometry().sad();
            cwLimit = machineconfigInfo.machinegeometry().cwlimit();
            ccwLimit = machineconfigInfo.machinegeometry().ccwlimit();
            collimatorLow = machineconfigInfo.machinegeometry().collimatorlow();
            collimatorHigh = machineconfigInfo.machinegeometry().collimatorhigh();
            lateralMotionMax = machineconfigInfo.machinegeometry().lateralmotionmax();
            lateralMotionMin = machineconfigInfo.machinegeometry().lateralmotionmin();
            longitudeMotionMin = machineconfigInfo.machinegeometry().longitudemotionmin();
            longitudeMotionMax = machineconfigInfo.machinegeometry().longitudemotionmax();
            verticalMotionMax = machineconfigInfo.machinegeometry().verticalmotionmax();
            verticalMotionMin = machineconfigInfo.machinegeometry().verticalmotionmin();
            rTNMax = machineconfigInfo.machinegeometry().rtnmax();
            rTNMIN = machineconfigInfo.machinegeometry().rtnmin();
        }

        //Jaw
        if (machineconfigInfo.has_machinejaw()) {
            isForceJawFollow = machineconfigInfo.machinejaw().isforcejawfollow();
            isXjaw = machineconfigInfo.machinejaw().isxjaw();
            isYjaw = machineconfigInfo.machinejaw().isyjaw();
            isXasym = machineconfigInfo.machinejaw().isxasym();
            isYasym = machineconfigInfo.machinejaw().isyasym();
            leftJawLabel = machineconfigInfo.machinejaw().leftjawlabel();
            rightJawLabel = machineconfigInfo.machinejaw().rightjawlabel();
            maxLeftJawPosition = machineconfigInfo.machinejaw().maxleftjawposition();
            minLeftJawPosition = machineconfigInfo.machinejaw().minleftjawposition();
            maxRightJawPosition = machineconfigInfo.machinejaw().maxrightjawposition();
            minRightJawPosition = machineconfigInfo.machinejaw().minrightjawposition();
            lateralJawOutsideEdge = machineconfigInfo.machinejaw().lateraljawoutsideedge();
            lateralInsideJawEdge = machineconfigInfo.machinejaw().lateralinsidejawedge();
            sourceLateralJawDistance = machineconfigInfo.machinejaw().sourcelateraljawdistance();
            jawGap = machineconfigInfo.machinejaw().jawgap();
            MaxJawSpeed = machineconfigInfo.machinejaw().maxjawspeed();
            topJawLabel = machineconfigInfo.machinejaw().topjawlabel();
            bottomJawLabel = machineconfigInfo.machinejaw().bottomjawlabel();
            maxTopJawPosition = machineconfigInfo.machinejaw().maxtopjawposition();
            minTopJawPosition = machineconfigInfo.machinejaw().mintopjawposition();
            maxBottomJawPosition = machineconfigInfo.machinejaw().maxbottomjawposition();
            minBottomJawPosition = machineconfigInfo.machinejaw().minbottomjawposition();
            longitudeJawOutsideEdge = machineconfigInfo.machinejaw().longitudejawoutsideedge();
            longitudeJawInsideEdge = machineconfigInfo.machinejaw().longitudejawinsideedge();
            sourceLongitudeJawDistance = machineconfigInfo.machinejaw().sourcelongitudejawdistance();
            lateralTracking = machineconfigInfo.machinejaw().lateraltracking();
            longitudinalTranking = machineconfigInfo.machinejaw().longitudinaltranking();
        }

        //MLC
        if (machineconfigInfo.has_machinemlc()) {
            sourceToMLCtopDistance = machineconfigInfo.machinemlc().sourcetomlctopdistance();
            minLeafJawOverlap = machineconfigInfo.machinemlc().minleafjawoverlap();
            minGap = machineconfigInfo.machinemlc().mingap();
            maxLeafSpeed = machineconfigInfo.machinemlc().maxleafspeed();
            minTipPosition = machineconfigInfo.machinemlc().mintipposition();
            maxTipPosition = machineconfigInfo.machinemlc().maxtipposition();
            leafInterdigitation = machineconfigInfo.machinemlc().leafinterdigitation();
            maxLeafoutofCarriageDistance = machineconfigInfo.machinemlc().maxleafoutofcarriagedistance();
            mlcDirection = machineconfigInfo.machinemlc().mlcdirection();
            pair = machineconfigInfo.machinemlc().pairs();

            ms::proto::RT_MS_PairInfo pairList;
           
            if (machineconfigInfo.has_machinemlc()){
                for (int i = 0; i <machineconfigInfo.machinemlc().pairinfolist_size(); ++i ){
                     std::tr1::shared_ptr<PairInfo> pairInfo(new PairInfo());
                    pairList = machineconfigInfo.machinemlc().pairinfolist(i);
                    pairInfo->UID = pairList.pairuid();
                    pairInfo->pair = pairList.pair();
                    pairInfo->leafPairs = pairList.leafpairs();
                    pairInfoList.push_back(pairInfo);
                }
            }
        }

        //BeamMode
        ms::proto::RT_MS_CommissionInfo commisInfo;
        std::tr1::shared_ptr<CommissionInfo> pcommInfo(new CommissionInfo());
        //commissionInfoList.clear();
        for (int i = 0; i < machineconfigInfo.machinecommissioninfo_size(); ++i){
            commisInfo = machineconfigInfo.machinecommissioninfo(i);
            pcommInfo->parentUID = commisInfo.parentuid();
            pcommInfo->commissionUID = commisInfo.commissionuid();
            pcommInfo->phoCommissionUID = commisInfo.phocommissionuid();
            pcommInfo->commissionName = commisInfo.commissionname();
            pcommInfo->radiationType = commisInfo.radiationtype();
            pcommInfo->energy        = commisInfo.energy();
            pcommInfo->flatteningFilterType = commisInfo.flatteningfiltertype();
            pcommInfo->isConvolution = commisInfo.isconvolution();
            pcommInfo->isPencilBeam = commisInfo.ispencilbeam();
            pcommInfo->isMonteCarlo = commisInfo.ismontecarlo();
            pcommInfo->isDoseRateVarSupport = commisInfo.isdoseratevarsupport();
            pcommInfo->isPlan2D    = commisInfo.isplan2d();
            pcommInfo->isPlan3D =     commisInfo.isplan3d();
            pcommInfo->isPlanStepShoot = commisInfo.isplanstepshoot();
            pcommInfo->isPlanSlidingWindow = commisInfo.isplanslidingwindow();
            pcommInfo->isPlanSimpleArc = commisInfo.isplansimplearc();
            pcommInfo->isPlanConformalArc = commisInfo.isplanconformalarc();
            pcommInfo->isPlanVMATArc     = commisInfo.isplanvmatarc();
            pcommInfo->isPlanBurstArc   = commisInfo.isplanburstarc();
            pcommInfo->electornType  =   commisInfo.electorntype();
            pcommInfo->isApprove = commisInfo.isapprove();
			pcommInfo->mindoserate_mupermin = commisInfo.mindoserate_mupermin();

            pcommInfo->isCheckedHalfProfile = commisInfo.ischeckedhalfprofile();
            pcommInfo->calibrationFieldSizeX = commisInfo.calibrationfieldsizex();
            pcommInfo->calibrationFieldSizeY = commisInfo.calibrationfieldsizey();
            pcommInfo->calibrationDmax = commisInfo.calibrationdmax();
            pcommInfo->normalizeDose = commisInfo.nordepth();
            pcommInfo->sSD = commisInfo.ssd();
            pcommInfo->lateral = commisInfo.lateral();
            pcommInfo->depth = commisInfo.depth();
            pcommInfo->date = commisInfo.date();
            pcommInfo->time = commisInfo.time();
            pcommInfo->who = commisInfo.who();
            pcommInfo->penumbra = commisInfo.penumbra();
            pcommInfo->blockAttenFactor = commisInfo.blockattenuationfactor();

            pcommInfo->MLCFactor = commisInfo.mlcfactor();
            pcommInfo->xJawFactor = commisInfo.xjawfactor();
            pcommInfo->yJawFactor = commisInfo.yjawfactor();

            ms::proto::RT_MS_MachineBeamModeDoseRate doseRate;
            for (int it = 0; it <commisInfo.doseratelistinfo_size(); ++it ){
                std::tr1::shared_ptr<DoseRate> pdoseRate(new DoseRate());
                doseRate = commisInfo.doseratelistinfo(it);
                pdoseRate->doseRateUID = doseRate.doserateuid();
                pdoseRate->doseRate     = doseRate.doserate();
                pdoseRate->isDoseRateDefault = doseRate.isdoseratedefault();
                pcommInfo->doseRateList.push_back(pdoseRate);
            }

            for(int it = 0; it < commisInfo.trayfactorlist_size(); ++it){
                 pcommInfo->trayFactorList.push_back(commisInfo.trayfactorlist(it));
            }

            commissionInfoList.push_back(pcommInfo);
        }

        //Accessory
        if (machineconfigInfo.has_machineaccessories()) {
            isApertureBlock = machineconfigInfo.machineaccessories().isapertureblock();
            isShieldBlock = machineconfigInfo.machineaccessories().isshieldblock();
            sourceToBlockTryDistance = machineconfigInfo.machineaccessories().sourcetoblocktrydistance();
            height = machineconfigInfo.machineaccessories().height();
            sourceToApplicatorLowDistance = machineconfigInfo.machineaccessories().sourcetoapplicatorlowdistance();

            ms::proto::RT_MS_MachineElectronApplicator electronApp;
            std::tr1::shared_ptr<ElectronApplicator> pElectronApp(new ElectronApplicator());
            for (int i  = 0; i < machineconfigInfo.machineaccessories().electronapplicatorlistinfo_size(); ++i){
                electronApp = machineconfigInfo.machineaccessories().electronapplicatorlistinfo(i);
                pElectronApp->applicatorUID = electronApp.applicatoruid();
                pElectronApp->applicatorACCID = electronApp.applicatoraccid();
                pElectronApp->sizeX = electronApp.sizex();
                pElectronApp->sizeY = electronApp.sizey();
                electronApplicatorList.push_back(pElectronApp);
            }

            for (int j = 0; j < machineconfigInfo.machineaccessories().trayuidlist_size(); ++j){
                trayUIDList.push_back( machineconfigInfo.machineaccessories().trayuidlist(j));
            }

            for (int k = 0; k < machineconfigInfo.machineaccessories().trayacccode_size(); ++k){
                trayAccCode.push_back( machineconfigInfo.machineaccessories().trayacccode(k));
            }

            ms::proto::RT_MS_MachineTray trayInfo;
            for (int g = 0; g < machineconfigInfo.machineaccessories().traylist_size(); ++g){

            std::tr1::shared_ptr<Tray> pTray(new Tray());
            trayInfo = machineconfigInfo.machineaccessories().traylist(g);
            pTray->trayUID = trayInfo.trayuid();
            pTray->trayID = trayInfo.trayid();
            pTray->trayCode = trayInfo.traycode();
            trayInfoList.push_back(pTray);
            }
        }

        //wedge
        if (machineconfigInfo.has_machineaccessories()){
            sourceToWedgeDistance = machineconfigInfo.machineaccessories().sourcetowedgedistance();

            ms::proto::RT_MS_MachineWedge wedgeInfo;
            std::tr1::shared_ptr<WedgeInfo> pwedgeInfo(new WedgeInfo());
            for (int i = 0; i < machineconfigInfo.machineaccessories().machinewedgelistinfo_size() ; ++i){
                wedgeInfo = machineconfigInfo.machineaccessories().machinewedgelistinfo(i);
                pwedgeInfo->wedgeUID = wedgeInfo.wedgeuid();
                pwedgeInfo->wedgeType = wedgeInfo.wedgetype();
                pwedgeInfo->wedgeACCID = wedgeInfo.wedgeaccid();
                pwedgeInfo->wedgeACCcode = wedgeInfo.acccode();
                pwedgeInfo->degree = wedgeInfo.degree();

                pwedgeInfo->leftmin = wedgeInfo.leftmin();
                pwedgeInfo->leftmax = wedgeInfo.leftmax();
                pwedgeInfo->rightmin = wedgeInfo.rightmin();
                pwedgeInfo->rightmax = wedgeInfo.rightmax();
                pwedgeInfo->topmin = wedgeInfo.topmin();
                pwedgeInfo->topmax = wedgeInfo.topmax();
                pwedgeInfo->bottommin = wedgeInfo.bottommin();
                pwedgeInfo->bottommax = wedgeInfo.bottommax();

                pwedgeInfo->orientationType = wedgeInfo.orientationtype();
                wedgeInfoList.push_back(pwedgeInfo);
            }
        }
    }

    void ConvertBack(ms::proto::RT_MS_MachineConfigInfo* machineconfigInfo) const{
        machineconfigInfo->Clear();
        machineconfigInfo->set_isactive(isActive);
        //General
        machineconfigInfo->mutable_machinegeneralinfo()->set_uid(uid);
        machineconfigInfo->mutable_machinegeneralinfo()->set_machinename(machineName);
        machineconfigInfo->mutable_machinegeneralinfo()->set_manufactory(manufactory);
        machineconfigInfo->mutable_machinegeneralinfo()->set_deviceid(DeviceID);
        machineconfigInfo->mutable_machinegeneralinfo()->set_coordinationsystem(coordinationSystem);
        machineconfigInfo->mutable_machinegeneralinfo()->set_date(date);
        machineconfigInfo->mutable_machinegeneralinfo()->set_time(time);
        machineconfigInfo->mutable_machinegeneralinfo()->set_bywho(byWho);
		machineconfigInfo->mutable_machinegeneralinfo()->set_comments(comments);

        //Geometry
        machineconfigInfo->mutable_machinegeometry()->set_gantryanglemin(gantryAngleMin);
        machineconfigInfo->mutable_machinegeometry()->set_gantryanglemax(gantryAngleMax);
        machineconfigInfo->mutable_machinegeometry()->set_maxspeedlimit(maxSpeedLimit);
        machineconfigInfo->mutable_machinegeometry()->set_cwlimit(cwLimit);
        machineconfigInfo->mutable_machinegeometry()->set_ccwlimit(ccwLimit);
        machineconfigInfo->mutable_machinegeometry()->set_sad(sad);
        machineconfigInfo->mutable_machinegeometry()->set_collimatorlow(collimatorLow);
        machineconfigInfo->mutable_machinegeometry()->set_collimatorhigh(collimatorHigh);
        machineconfigInfo->mutable_machinegeometry()->set_lateralmotionmax(lateralMotionMax);
        machineconfigInfo->mutable_machinegeometry()->set_lateralmotionmin(lateralMotionMin);
        machineconfigInfo->mutable_machinegeometry()->set_longitudemotionmax(longitudeMotionMax);
        machineconfigInfo->mutable_machinegeometry()->set_longitudemotionmin(longitudeMotionMin);
        machineconfigInfo->mutable_machinegeometry()->set_verticalmotionmax(verticalMotionMax);
        machineconfigInfo->mutable_machinegeometry()->set_verticalmotionmin(verticalMotionMin);
        machineconfigInfo->mutable_machinegeometry()->set_rtnmax(rTNMax);
        machineconfigInfo->mutable_machinegeometry()->set_rtnmin(rTNMIN);
        //Jaw
        machineconfigInfo->mutable_machinejaw()->set_isforcejawfollow(isForceJawFollow);
        machineconfigInfo->mutable_machinejaw()->set_isxjaw(isXjaw);
        machineconfigInfo->mutable_machinejaw()->set_isyjaw(isYjaw);
        machineconfigInfo->mutable_machinejaw()->set_isxasym(isXasym);
        machineconfigInfo->mutable_machinejaw()->set_isyasym(isYasym);
        machineconfigInfo->mutable_machinejaw()->set_leftjawlabel(leftJawLabel);
        machineconfigInfo->mutable_machinejaw()->set_rightjawlabel(rightJawLabel);
        machineconfigInfo->mutable_machinejaw()->set_maxleftjawposition(maxLeftJawPosition);
        machineconfigInfo->mutable_machinejaw()->set_minleftjawposition(minLeftJawPosition);
        machineconfigInfo->mutable_machinejaw()->set_maxrightjawposition(maxRightJawPosition);
        machineconfigInfo->mutable_machinejaw()->set_minrightjawposition(minRightJawPosition);
        machineconfigInfo->mutable_machinejaw()->set_lateraljawoutsideedge(lateralJawOutsideEdge);
        machineconfigInfo->mutable_machinejaw()->set_lateralinsidejawedge(lateralInsideJawEdge);
        machineconfigInfo->mutable_machinejaw()->set_sourcelateraljawdistance(sourceLateralJawDistance);
        machineconfigInfo->mutable_machinejaw()->set_jawgap(jawGap);
        machineconfigInfo->mutable_machinejaw()->set_maxjawspeed(MaxJawSpeed);
        machineconfigInfo->mutable_machinejaw()->set_topjawlabel(topJawLabel);
        machineconfigInfo->mutable_machinejaw()->set_bottomjawlabel(bottomJawLabel);
        machineconfigInfo->mutable_machinejaw()->set_maxtopjawposition(maxTopJawPosition);
        machineconfigInfo->mutable_machinejaw()->set_mintopjawposition(minTopJawPosition);
        machineconfigInfo->mutable_machinejaw()->set_maxbottomjawposition(maxBottomJawPosition);
        machineconfigInfo->mutable_machinejaw()->set_minbottomjawposition(minBottomJawPosition);
        machineconfigInfo->mutable_machinejaw()->set_longitudejawoutsideedge(longitudeJawOutsideEdge);
        machineconfigInfo->mutable_machinejaw()->set_longitudejawinsideedge(longitudeJawInsideEdge);
        machineconfigInfo->mutable_machinejaw()->set_sourcelongitudejawdistance(sourceLongitudeJawDistance);
        machineconfigInfo->mutable_machinejaw()->set_lateraltracking(lateralTracking);
        machineconfigInfo->mutable_machinejaw()->set_longitudinaltranking(longitudinalTranking);

        //MLC
        machineconfigInfo->mutable_machinemlc()->set_sourcetomlctopdistance(sourceToMLCtopDistance);
        machineconfigInfo->mutable_machinemlc()->set_mingap(minGap);
        machineconfigInfo->mutable_machinemlc()->set_minleafjawoverlap(minLeafJawOverlap);
        machineconfigInfo->mutable_machinemlc()->set_maxleafspeed(maxLeafSpeed);
        machineconfigInfo->mutable_machinemlc()->set_mintipposition(minTipPosition);
        machineconfigInfo->mutable_machinemlc()->set_maxtipposition(maxTipPosition);
        machineconfigInfo->mutable_machinemlc()->set_leafinterdigitation(leafInterdigitation);
        machineconfigInfo->mutable_machinemlc()->set_maxleafoutofcarriagedistance(maxLeafoutofCarriageDistance);
        machineconfigInfo->mutable_machinemlc()->set_mlcdirection(mlcDirection);

        ms::proto::RT_MS_PairInfo * pairInfo = nullptr;
        for (auto it = pairInfoList.begin(); it != pairInfoList.end(); ++it){
            pairInfo=  machineconfigInfo->mutable_machinemlc()->add_pairinfolist();
            pairInfo->set_pairuid((*it)->UID);
            pairInfo->set_leafpairs((*it)->leafPairs);
            pairInfo->set_pair((*it)->pair);
        }

        //BeamMode
        ms::proto::RT_MS_CommissionInfo* pcomInfo =nullptr;
        for (auto itt =commissionInfoList.begin(); itt != commissionInfoList.end(); ++itt ){
            pcomInfo = machineconfigInfo->add_machinecommissioninfo();
            pcomInfo->set_commissionuid((*itt)->commissionUID);
            pcomInfo->set_phocommissionuid((*itt)->phoCommissionUID);
            pcomInfo->set_commissionname((*itt)->commissionName);
            pcomInfo->set_radiationtype((*itt)->radiationType);
            pcomInfo->set_energy((*itt)->energy);
            pcomInfo->set_flatteningfiltertype((*itt)->flatteningFilterType);
            pcomInfo->set_isconvolution((*itt)->isConvolution);
            pcomInfo->set_ispencilbeam((*itt)->isPencilBeam);
            pcomInfo->set_ismontecarlo((*itt)->isMonteCarlo);
            pcomInfo->set_isdoseratevarsupport((*itt)->isDoseRateVarSupport);
            pcomInfo->set_isplan2d((*itt)->isPlan2D);
            pcomInfo->set_isplan3d((*itt)->isPlan3D);
            pcomInfo->set_isplanstepshoot((*itt)->isPlanStepShoot);
            pcomInfo->set_isplanslidingwindow((*itt)->isPlanSlidingWindow);
            pcomInfo->set_isplansimplearc((*itt)->isPlanSimpleArc);
            pcomInfo->set_isplanconformalarc((*itt)->isPlanConformalArc);
            pcomInfo->set_isplanvmatarc((*itt)->isPlanVMATArc);
            pcomInfo->set_isplanburstarc((*itt)->isPlanBurstArc);
            pcomInfo->set_electorntype((*itt)->electornType == 1);
            pcomInfo->set_isapprove((*itt)->isApprove);

            pcomInfo->set_ischeckedhalfprofile((*itt)->isCheckedHalfProfile);
            pcomInfo->set_calibrationfieldsizex((*itt)->calibrationFieldSizeX);
            pcomInfo->set_calibrationfieldsizey((*itt)->calibrationFieldSizeY);
            pcomInfo->set_calibrationdmax((*itt)->calibrationDmax);
            pcomInfo->set_nordepth((*itt)->normalizeDose);

            pcomInfo->set_ssd((*itt)->sSD);
            pcomInfo->set_lateral((*itt)->lateral);
            pcomInfo->set_depth((*itt)->depth);
            pcomInfo->set_date((*itt)->date);
            pcomInfo->set_time((*itt)->time);
            pcomInfo->set_who((*itt)->who);
            pcomInfo->set_penumbra((*itt)->penumbra);
            pcomInfo->set_blockattenuationfactor((*itt)->blockAttenFactor);
            pcomInfo->set_mlcfactor((*itt)->MLCFactor);
            pcomInfo->set_xjawfactor((*itt)->xJawFactor);
            pcomInfo->set_yjawfactor((*itt)->yJawFactor);

            ms::proto::RT_MS_MachineBeamModeDoseRate* doseRate = nullptr;
            for (auto it = (*itt)->doseRateList.begin(); it != (*itt)->doseRateList.end(); ++it ){
                doseRate =  pcomInfo->add_doseratelistinfo();
                doseRate->set_doserate((*it)->doseRate);
                doseRate->set_doserateuid((*it)->doseRateUID);
                doseRate->set_isdoseratedefault((*it)->isDoseRateDefault);
            }

            for (auto it = (*itt)->trayFactorList.begin(); it != (*itt)->trayFactorList.end(); ++it){
                pcomInfo->add_trayfactorlist(*it);
            }

            //PMC part
               //这里UID可能不对，还可能为commissionUID
            pcomInfo->mutable_photonmontecarloinfo()->set_commissionuid((*itt)->phoCommissionUID);
               //PDD
            auto pDDParameter = pcomInfo->mutable_photonmontecarloinfo()->mutable_pddparameter();
            pDDParameter->set_primaryenergystart((*itt)->primaryEnergyStart);
            pDDParameter->set_primaryenergyend((*itt)->primaryEnergyEnd);
            pDDParameter->set_primaryenergybin((*itt)->primaryEnergyBin);
            pDDParameter->set_primaryenergyrestart((*itt)->primaryEnergyRestart);
            pDDParameter->set_secondaryenergystart((*itt)->secondaryEnergyStart);
            pDDParameter->set_secondaryenergyend((*itt)->secondaryEnergyEnd);
            pDDParameter->set_secondaryenergybin((*itt)->secondaryEnergyBin);
            pDDParameter->set_secondaryenergyrestart((*itt)->secondaryEnergyRestart);
            pDDParameter->set_electronenergy((*itt)->electronEnergy);
            pDDParameter->set_particlenumber((*itt)->particleNumber);
            pDDParameter->set_completefilesnumber((*itt)->completeFilesNumber);

            ms::proto::RT_MS_PMCEnergyAndSpectrum* primarySpectrum = nullptr;
            for (auto it = (*itt)->primarySpectrumCollection.begin(); it != (*itt)->primarySpectrumCollection.end(); ++it ){
                primarySpectrum = pDDParameter->add_primaryspectrumcollection();
                primarySpectrum->set_energy(it->energy);
                primarySpectrum->set_weight(it->weight);
            }

            ms::proto::RT_MS_PMCEnergyAndSpectrum* secondarySpectrum = nullptr;
            for (auto it = (*itt)->secondarySpectrumCollection.begin(); it != (*itt)->secondarySpectrumCollection.end(); ++it ){
                secondarySpectrum = pDDParameter->add_secondaryspectrumcollection();
                secondarySpectrum->set_energy(it->energy);
                secondarySpectrum->set_weight(it->weight);
            }

            ms::proto::RT_MS_PMCEnergyAndSpectrum* electronSpectrum = nullptr;
            for (auto it = (*itt)->electronSpectrumCollection.begin(); it != (*itt)->electronSpectrumCollection.end(); ++it ){
                electronSpectrum = pDDParameter->add_electronspectrumcollection();
                electronSpectrum->set_energy(it->energy);
                electronSpectrum->set_weight(it->weight);
            }

               //In Field
            auto inFieldParameter = pcomInfo->mutable_photonmontecarloinfo()->mutable_infieldparameter();
            inFieldParameter->set_primarysoften((*itt)->primarySoften);
            inFieldParameter->set_softenparameter((*itt)->softenParameter);
            inFieldParameter->set_selectedsecondarysourcetype((*itt)->selectedSecondarySourceType);
            inFieldParameter->set_scattersoften((*itt)->scatterSoften);

            ms::proto::RT_MS_PMCSecondarySourceParameter* secondarySourceParameter = nullptr;
            for (auto it = (*itt)->secondarySourceCollection.begin(); it != (*itt)->secondarySourceCollection.end(); ++it ){
                secondarySourceParameter = inFieldParameter->add_secondarysourceparametercollection();
                secondarySourceParameter->set_flatfiltera(it->flatFilterA);
                secondarySourceParameter->set_flatfilterb(it->flatFilterB);
            }

                //Out Field
            auto outFieldParameter = pcomInfo->mutable_photonmontecarloinfo()->mutable_outfieldparameter();
            outFieldParameter->set_flatdistance((*itt)->flatDistance);
            outFieldParameter->set_jawdistance((*itt)->jawDistance);
            outFieldParameter->set_sigmap((*itt)->sigmaP);
            outFieldParameter->set_sigmas((*itt)->sigmaS);
            outFieldParameter->set_tailcorrection((*itt)->tailCorrection);
            outFieldParameter->set_rectanglescattercorrection((*itt)->rectangleScatterCorrection);
            outFieldParameter->set_mlcleakage((*itt)->mlcLeakage);
            outFieldParameter->set_gaussianblurswitch((*itt)->gaussianBlurSwitch);
            outFieldParameter->set_gaussiansigmax((*itt)->gaussianSigmaX);
            outFieldParameter->set_gaussiansigmay((*itt)->gaussianSigmaY);

               //Electron Contamination
            auto electronContaminationParameter = pcomInfo->mutable_photonmontecarloinfo()->mutable_electroncontaminationparameter();
            electronContaminationParameter->set_standardradius((*itt)->standardRadius);
            electronContaminationParameter->set_sigma((*itt)->sigma);
            electronContaminationParameter->set_offset((*itt)->offset);
            electronContaminationParameter->set_powerfactor((*itt)->powerFactor);


               //Absolute Dose
            //auto absoluteDoseParameter = pcomInfo->mutable_photonmontecarloinfo()->mutable_absolutedoseparameter();
            //absoluteDoseParameter->set_norm10dose((*itt)->norm10Dose);
            //ms::proto::RT_MS_PMCFieldSizeAndAbsoluteDose* pmcabsoluteDose = nullptr;
            //for (auto it = (*itt)->absoluteDoseCollection.begin(); it != (*itt)->absoluteDoseCollection.end(); ++it ){
            //    pmcabsoluteDose = absoluteDoseParameter->add_absolutedosecollection();
            //    pmcabsoluteDose->set_fieldsize(it->fieldSize);
            //    pmcabsoluteDose->set_absolutedose(it->absoluteDose);
            //}

               //PMCWedgeField
                   //PhysicalWedge
           auto physicalWedgeCollection = pcomInfo->mutable_photonmontecarloinfo()->mutable_wedgefieldparameter();
           ms::proto::RT_MS_PMCPhysicalWedge* physicalWedge = nullptr;
           for (auto it = (*itt)->physicalWedgeCollection.begin(); it != (*itt)->physicalWedgeCollection.end(); ++it ){
                physicalWedge = physicalWedgeCollection->add_physicalwedgecollection();
                physicalWedge->set_wedgeuid(it->wedgeUID);
                physicalWedge->set_wedgethickness(it->wedgeThickness);
                ms::proto::RT_MS_Point2D* pointSingle = nullptr;
                for(auto onePoint = (it->PointCollection).begin(); onePoint != (it->PointCollection).end(); ++onePoint)
                {
                    pointSingle = physicalWedge->add_contourvertexpointcollection();
                    pointSingle->set_x(onePoint->x);
                    pointSingle->set_y(onePoint->y);
                }
                physicalWedge->set_carrierthickness(it->carrierThickness);
                ms::proto::RT_MS_Point2D* pointSingle2 = nullptr;
                for(auto onePoint = (it->CarrierPointCollection).begin(); onePoint != (it->CarrierPointCollection).end(); ++onePoint)
                {
                    pointSingle2 = physicalWedge->add_carriercontourvertexpointcollection();
                    pointSingle2->set_x(onePoint->x);
                    pointSingle2->set_y(onePoint->y);
                }
            }
        }

        //Accessory
        machineconfigInfo->mutable_machineaccessories()->set_isapertureblock(isApertureBlock);
        machineconfigInfo->mutable_machineaccessories()->set_isshieldblock(isShieldBlock);
        machineconfigInfo->mutable_machineaccessories()->set_sourcetoapplicatorlowdistance(sourceToApplicatorLowDistance);
        machineconfigInfo->mutable_machineaccessories()->set_sourcetoblocktrydistance(sourceToBlockTryDistance);
        machineconfigInfo->mutable_machineaccessories()->set_height(height);

        ms::proto::RT_MS_MachineElectronApplicator* electronApplicator = nullptr;
        for (auto it = electronApplicatorList.begin(); it != electronApplicatorList.end(); ++it){
            electronApplicator = machineconfigInfo->mutable_machineaccessories()->add_electronapplicatorlistinfo();
            electronApplicator->set_applicatoruid((*it)->applicatorUID);
            electronApplicator->set_applicatoraccid((*it)->applicatorACCID);
            electronApplicator->set_sizex((*it)->sizeX);
            electronApplicator->set_sizey((*it)->sizeY);
        }

        for (auto it = trayUIDList.begin(); it != trayUIDList.end(); ++it){
             machineconfigInfo->mutable_machineaccessories()->add_trayuidlist(*it);
        }
        for (auto it = trayAccCode.begin(); it != trayAccCode.end(); ++it){
            machineconfigInfo->mutable_machineaccessories()->add_trayacccode(*it);
        }

            ms::proto::RT_MS_MachineTray* trayInfo = nullptr;
           for (auto it = trayInfoList.begin(); it != trayInfoList.end(); ++it ){
             trayInfo = machineconfigInfo->mutable_machineaccessories()->add_traylist();
             trayInfo->set_trayuid((*it)->trayUID);
             trayInfo->set_trayid((*it)->trayID);
             trayInfo->set_traycode((*it)->trayCode);
           }

        //wedge
        machineconfigInfo->mutable_machineaccessories()->set_sourcetowedgedistance(sourceToWedgeDistance);

        ms::proto::RT_MS_MachineWedge* wedge = nullptr;
        for (auto it = wedgeInfoList.begin(); it != wedgeInfoList.end(); ++it){
            wedge = machineconfigInfo->mutable_machineaccessories()->add_machinewedgelistinfo();
            wedge->set_degree((*it)->degree);

            wedge->set_leftmin((*it)->leftmin);
            wedge->set_leftmax((*it)->leftmax);
            wedge->set_rightmin((*it)->rightmin);
            wedge->set_rightmax((*it)->rightmax);
            wedge->set_topmin((*it)->topmin);
            wedge->set_topmax((*it)->topmax);
            wedge->set_bottommin((*it)->bottommin);
            wedge->set_bottommax((*it)->bottommax);

            wedge->set_orientationtype((*it)->orientationType);
            wedge->set_wedgeaccid((*it)->wedgeACCID);
            wedge->set_wedgeuid((*it)->wedgeUID);
            wedge->set_wedgetype((*it)->wedgeType);
            wedge->set_acccode((*it)->wedgeACCcode);
        }
    }
    CertifiedMachineSetting(){

    }

};

TPS_END_NAMESPACE


#endif
