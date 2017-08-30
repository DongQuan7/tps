////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_beamgraphicobjectconverter.cpp
/// 
///  \brief class TpsBeamGOConverter implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/25
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_beamgraphicobjectconverter.h"

#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsFramework/tps_fw_mask.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_fluence_map_entity.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_accessory.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_block.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_wedgegraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_wedgegraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_applicator_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_applicator_graphic_object_converter.h"
#include "RtTpsRenderLibrary/tps_rl_block_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_block_graphic_object_converter.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"

#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsBeamGOConverter::TpsBeamGOConverter(
GO_TYPE goType, TpsWedgeGOConverter* wedgeConverter, 
ApplicatorGOConverter* applicatorConverter, 
BlockGOConverter* apertureBlockConverter, 
BlockGOConverter* shieldingBlockConverter)
: TpsGOConverterBase(goType)
, mWedgeConverter(wedgeConverter)
, mApplicatorConverter(applicatorConverter)
, mApertureBlockConverter(apertureBlockConverter)
, mShieldingBlockConverter(shieldingBlockConverter)
, mBeam(nullptr), mMachine(nullptr)
, mNormalGroup(nullptr), mFluenceMapEntity(nullptr)
, mIecCS(nullptr){

}

TpsBeamGOConverter::~TpsBeamGOConverter(){

    if (mWedgeConverter){
        delete mWedgeConverter;
        mWedgeConverter = nullptr;
    }

    if (mApplicatorConverter){
        delete mApplicatorConverter;
        mApplicatorConverter = nullptr;
    }

    if (mApertureBlockConverter){
        delete mApertureBlockConverter;
        mApertureBlockConverter = nullptr;
    }
}

void TpsBeamGOConverter::SetModelWarehouse(ModelWarehouse* modelWarehouse){
    mModelWarehouse = modelWarehouse;

    if (mWedgeConverter){
        mWedgeConverter->SetModelWarehouse(modelWarehouse);
    }

    if (mApplicatorConverter){
        mApplicatorConverter->SetModelWarehouse(modelWarehouse);
    }

    if (mApertureBlockConverter){
        mApertureBlockConverter->SetModelWarehouse(modelWarehouse);
    }
}

void TpsBeamGOConverter::SetDataRepository(ITpsDataRepository* dataRepository)
{
    mDataRepository = dataRepository;
    if (nullptr != mWedgeConverter) 
        mWedgeConverter->SetDataRepository(dataRepository);
    if ( nullptr != mApplicatorConverter) 
        mApplicatorConverter->SetDataRepository(dataRepository);
    if (nullptr != mApertureBlockConverter)
        mApertureBlockConverter->SetDataRepository(dataRepository);
    if (nullptr != mShieldingBlockConverter)
        mShieldingBlockConverter->SetDataRepository(dataRepository);
}

GraphicObjectPtr TpsBeamGOConverter::CreateGraphicObject_i(){
    return std::shared_ptr<TpsBeamGraphicObject>(new TpsBeamGraphicObject());
}

bool TpsBeamGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    mBeam = nullptr; 
    mIecCS = nullptr;
    mMachine = nullptr; 
    mNormalGroup = nullptr; 
    mFluenceMapEntity = nullptr;

    TpsBeamManager* beamManager = mDataRepository->GetBeamManager();
    mBeam = beamManager->GetBeam(para.obj_uid);
    if (mBeam == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get beam: "<<para.obj_uid;
        return false;
    }

    mIecCS = beamManager->GetBeamCS(para.obj_uid);
    if (mIecCS == nullptr){
        // Notice: EasyPlan beam does not have IECCoordinate System.
        //         So, it should not return here.

        //TPS_LOG_DEV_ERROR<<"Failed to get beam coordinate system object: "<<uid;
        //return false;
    }

    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(
        mBeam->get_normgroupuid(), &mNormalGroup) || mNormalGroup == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get normal group.";
        return false;
    }

    if (!mDataRepository->GetMachineManager()->GetMachine(
        mNormalGroup->get_machineuid(), mMachine) || mMachine == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get machine.";
        return false;
    }

    mFluenceMapEntity = beamManager->GetFluenceMap(para.obj_uid);
    return true;
}

bool segmentSorter(const RtBeamsegment* first, const RtBeamsegment* second)
{
    return first->get_sequencenumber() < second->get_sequencenumber();
}

bool TpsBeamGOConverter::Update_i(GraphicObjectPtr go){

    using namespace Mcsf::MedViewer3D;
    std::shared_ptr<TpsBeamGraphicObject> beamGo = 
        std::dynamic_pointer_cast<TpsBeamGraphicObject>(go);
    // Notice: EasyPlan beam does not have IECCoordinate System.
    //         So, the mIecCS null pointer judgement should be removed.
    if (mBeam == nullptr /*|| mIecCS == nullptr*/ || mMachine == nullptr || 
        mNormalGroup == nullptr || beamGo == nullptr){
        return false;
    }

    beamGo->RemoveAllGos();
    beamGo->SetIsJoined(mBeam->get_isjoincalc());//TODO

    float color[4] = {mBeam->get_red(), mBeam->get_green(), mBeam->get_blue(), mBeam->get_alpha()};
    beamGo->SetColorForMprWindow(color);
    beamGo->SetColorForBevWindow(color);
    beamGo->SetVisible(mBeam->get_isvisible());
    beamGo->SetWeight(mBeam->get_beamweight());
    beamGo->SetMU(mBeam->get_beammu());
    beamGo->SetName(mBeam->get_name());
    beamGo->SetUID(mBeam->get_uid());
    beamGo->SetIsoCenterUID(mBeam->get_isocenterpoiuid());
    if (mIecCS != nullptr)
    {
        Mcsf::Matrix4f iecFixToPat;
        mIecCS->GetTransformMatrix(_FIXED, _PATIENT, iecFixToPat);
        beamGo->SetIECFixToPatientMatrix(TpsArithmeticConverter::ConvertToMatrix4x4(iecFixToPat));
    }

    //beamGo->SetEnergy(mBeam->get_machineenergy());why need this?

    RtDbDef::BEAM_TYPE type = (RtDbDef::BEAM_TYPE)mBeam->get_beamtype();
    if(type == RtDbDef::BURST_ARC || RtDbDef::VMAT == type) {
        beamGo->SetBeamType(ARC);
    }
    else {
        beamGo->SetBeamType(IMRT);
    }

    RtDbDef::APERTURE_TYPE apertureType = (RtDbDef::APERTURE_TYPE)mBeam->get_aperturetype();
    beamGo->SetApertureType(apertureType);
    if (apertureType == RtDbDef::Aperture_Block){
        //check aperture block
        RtBlock* block = mBeam->get_aperture_block();
        if (block == nullptr){
            TPS_LOG_DEV_ERROR<<"There is no aperture block when Aperture Type is BLOCK.";
            return false;
        }
    }

    //applicator
    GOConverterParameter para;
    bool isElectronNG = false;//mBeam->get_radiationtype() == RtDbDef::RADIATION_ELETRON;
    std::string applUID = mBeam->get_applicatoruid();
    if (applUID.empty() && isElectronNG) {
        TPS_LOG_DEV_ERROR<<"There is no applicator in electron beam.";
        return false;
    }
    if (!applUID.empty()){
        para.obj_uid = applUID;
        auto go = mApplicatorConverter->Convert(para);
        beamGo->SetApplicator(std::dynamic_pointer_cast<ApplicatorGraphicObject>(go));
    }

    //block, there can be block in some cases: mlc(shielding block), 
    //  applicator(shielding/aperture block), block(aperture block)
    RtBlock* pApertureBlock = mBeam->get_aperture_block();
    if (pApertureBlock != nullptr){
        para.obj_uid = mBeam->get_uid();
        go = mApertureBlockConverter->Convert(para);
        beamGo->SetApertureBlock(std::dynamic_pointer_cast<BlockGraphicObject>(go));
    }
    else{
        beamGo->SetApertureBlock(nullptr);
    }

    RtBlock* pShieldBlock = mBeam->get_shield_block();
    if (pShieldBlock != nullptr){
        para.obj_uid = mBeam->get_uid();
        go = mShieldingBlockConverter->Convert(para);
        beamGo->SetShieldingBlock(std::dynamic_pointer_cast<BlockGraphicObject>(go));
    }
    else{
        beamGo->SetShieldingBlock(nullptr);
    }

    int leafNum = mMachine->get_leafnum();
    const std::vector<double> leafBoundary = mMachine->get_leafboundaries_list();
    if (leafBoundary.size() != leafNum + 1 && !isElectronNG){
#ifdef _DEBUG
        printf("****ERROR***The size of leaf boundary list does not match leaf number!!!");
#endif
        TPS_LOG_DEV_ERROR<<"The size of leaf boundary list does not match leaf number.";
        return false;
    }
    RtDbDef::MLC_LEAF_DIRECTION_TYPE leafDirection = 
        (RtDbDef::MLC_LEAF_DIRECTION_TYPE)mMachine->get_mlcdirectiontype();
    float miniLeafGap = mMachine->get_minleafendgap();

    //add segment graphic object
    std::shared_ptr<TpsSegmentGraphicObject> segmentGO;
    std::vector<RtBeamsegment*> segments = mBeam->get_beamsegments();
    std::sort(segments.begin(), segments.end(), segmentSorter);

    Mcsf::Matrix4f matCollimatorToPatient;
    RtBeamsegment* segment;
    RtMlcshape* shape;

    double jawPos;
    float aJawPos[4];
    std::vector<db_Point2d> leafPositionList;
    std::vector<std::pair<Point3D,Point3D>> leafPosition3DList;
    Point3D pointFst, pointSnd;
    double startGantryAngle, arcLength, collimatorAngle, tableAngle;
    for (unsigned int i=0; i<segments.size(); ++i){
        segment = segments[i];
        startGantryAngle = segment->get_startgantryangle();
        arcLength = segment->get_arclength();
        collimatorAngle = segment->get_startcollimatorangle();
        tableAngle = segment->get_starttablerotationangle();
        // Notice: Since EasyPlan beam does not have IECCoordinate System.
        //         So, the mIecCS should be judged if it is nullptr.
        if(mIecCS != nullptr) {
            mIecCS->RotateCollimator(collimatorAngle);
            mIecCS->RotateTable(tableAngle);
        }
        segmentGO.reset(new TpsSegmentGraphicObject());
        segmentGO->SetStartGantryAngle(startGantryAngle);
        segmentGO->SetGantryArcLength(arcLength);
        segmentGO->SetCollimatorAngle(collimatorAngle);
        segmentGO->SetMlcVisibleFlag(true);
        segmentGO->SetJawVisibleFlag(false);

        bool isCW = RtDbDef::CW == segment->get_gantryrotationdirection();
        //start collimator to patient
        if(mIecCS != nullptr) {
            mIecCS->RotateGantry(startGantryAngle);
            mIecCS->GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matCollimatorToPatient);
        }
        segmentGO->SetStartCollimatorToPatMatrix(
            TpsArithmeticConverter::ConvertToMatrix4x4(matCollimatorToPatient));
        //end collimator to patient
        if(mIecCS != nullptr) {
            double gantryEndAngle = startGantryAngle + (isCW ? 1 : -1) * arcLength;
            mIecCS->RotateGantry(gantryEndAngle);
            mIecCS->GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matCollimatorToPatient);
        }
        segmentGO->SetEndCollimatorToPatMatrix(
            TpsArithmeticConverter::ConvertToMatrix4x4(matCollimatorToPatient));

        segmentGO->SetGantryIsClockWise(isCW);
        segmentGO->SetWeight(segment->get_weight());
        segmentGO->SetMU(segment->get_mu());
        segmentGO->SetUID(segment->get_uid());

        //set jaw position
        shape = segment->get_startmlcshape();
        if (shape == nullptr){
            TPS_LOG_DEV_ERROR<<"The start MLC shape of the segment is empty.";
            return false;
        }
        jawPos = shape->get_xlowerjawpos();
        aJawPos[0] = static_cast<float>(jawPos);
        jawPos = shape->get_xupperjawpos();
        aJawPos[1] = static_cast<float>(jawPos);
        jawPos = shape->get_ylowerjawpos();
        aJawPos[2] = static_cast<float>(jawPos);
        jawPos = shape->get_yupperjawpos();
        aJawPos[3] = static_cast<float>(jawPos);
        segmentGO->SetJawPosition(aJawPos);

        //there are no leaves in electron plan
        if (!isElectronNG){
            leafPositionList = shape->get_leafpositions();
            if (leafPositionList.size() != leafNum){
                TPS_LOG_DEV_ERROR<<"The size of leaf position list does not equal to leaf number, "
                    <<"Leaves number: "<<leafNum<<" Leaf positions pair count: "<<leafPositionList.size();
#ifdef _DEBUG
                printf("\nThe size of leaf position list does not equal to leaf number.\n");
#endif
                return false;
            }

            segmentGO->SetLeafDirection(leafDirection == RtDbDef::MLCY_DIRECTION ? 
                LEAF_DIRECTION_Y : LEAF_DIRECTION_X);
            segmentGO->SetMiniLeafGap(miniLeafGap);
            segmentGO->SetLeafNum(leafPositionList.size());
            for (unsigned int j = 0; j<leafPositionList.size(); ++j){
                leafDirection;
                segmentGO->SetLeafPosition(j, 
                    (float)leafPositionList[j].x,
                    (float)leafPositionList[j].y,
                    (float)leafBoundary[j],
                    (float)leafBoundary[j+1]);
            }
        }
        beamGo->AddSegmentGo(segmentGO);
    }
    if (mStatus == Cvtr_Update) beamGo->CorrectActiveSegment();
    else beamGo->SetActiveSegmentIndex(0);

    if (!beamGo->UpdateCombinationFieldEdges())
    {
        TPS_LOG_DEV_ERROR<<"Failed to update combined fields outline.";
        return false;
    }

    double sad = mMachine->get_sad();
    double jawBdry[4] = 
    {
        mMachine->get_xmin_1(), 
        mMachine->get_xmax_2(),
        mMachine->get_ymin_1(),
        mMachine->get_ymax_2(),
    };
    double extension = 50;//mm
    //segment in 3d window
    auto segmentList = beamGo->GetSegmentGoList();
    for(auto it = segmentList.begin(); it != segmentList.end(); ++it)
    {
        LEAF_DIRECTION leafDirection = (*it)->GetLeafDirection();
        std::vector<Point3D> ptList;
        StcMLC* pMLC = (*it)->GetMLC();
        const int nLeaf = pMLC->m_nLeafNum;
        plunc::LEAF_POSITION_STC* pMlcLeaf = pMLC->m_pLeaf;
        float brdy[4] = 
        { 
            jawBdry[1], jawBdry[0], jawBdry[3], jawBdry[2] 
        };
        for (int i = 0; i < nLeaf; i++)
        {
            if (fabs(pMlcLeaf[i].pos_1 - pMlcLeaf[i].pos_2) <= miniLeafGap + 1e-3)
            {
                continue;
            }
            brdy[0] = TPS_MIN(brdy[0], pMlcLeaf[i].pos_1);
            brdy[1] = TPS_MAX(brdy[1], pMlcLeaf[i].pos_2);
            brdy[2] = TPS_MIN(brdy[2], pMlcLeaf[i].min_w);
            brdy[3] = TPS_MAX(brdy[3], pMlcLeaf[i].max_w);
        }
        brdy[0] = TPS_MAX(brdy[0] - extension, jawBdry[0]); 
        brdy[1] = TPS_MIN(brdy[1] + extension, jawBdry[1]);
        brdy[2] = TPS_MAX(brdy[2] - extension, jawBdry[2]); 
        brdy[3] = TPS_MIN(brdy[3] + extension, jawBdry[3]);

        if(leafDirection == LEAF_DIRECTION_X)
        {
            for (int i = 0; i < nLeaf; i++)
            {
                if (pMlcLeaf[i].min_w < brdy[2] || pMlcLeaf[i].max_w > brdy[3])
                {
                    continue;
                }
                ptList.push_back(Point3D(brdy[0], pMlcLeaf[i].min_w, -1 * sad));
                ptList.push_back(Point3D(brdy[0], pMlcLeaf[i].max_w, -1 * sad));
                ptList.push_back(Point3D(pMlcLeaf[i].pos_1, pMlcLeaf[i].max_w, -1 * sad));
                ptList.push_back(Point3D(pMlcLeaf[i].pos_1, pMlcLeaf[i].min_w, -1 * sad));
                ptList.push_back(Point3D(brdy[1], pMlcLeaf[i].min_w, -1 * sad));
                ptList.push_back(Point3D(brdy[1], pMlcLeaf[i].max_w, -1 * sad));
                ptList.push_back(Point3D(pMlcLeaf[i].pos_2, pMlcLeaf[i].max_w, -1 * sad));
                ptList.push_back(Point3D(pMlcLeaf[i].pos_2, pMlcLeaf[i].min_w, -1 * sad));
            }
        }
        else
        {
            for (int i = 0; i < nLeaf; i++)
            {
                if (pMlcLeaf[i].min_w < brdy[2] || pMlcLeaf[i].max_w > brdy[3])
                {
                    continue;
                }
                ptList.push_back(Point3D(pMlcLeaf[i].min_w, brdy[0], -1 * sad));
                ptList.push_back(Point3D(pMlcLeaf[i].max_w, brdy[0], -1 * sad));
                ptList.push_back(Point3D(pMlcLeaf[i].max_w, pMlcLeaf[i].pos_1, -1 * sad));
                ptList.push_back(Point3D(pMlcLeaf[i].min_w, pMlcLeaf[i].pos_1, -1 * sad));
                ptList.push_back(Point3D(pMlcLeaf[i].min_w, brdy[1], -1 * sad));
                ptList.push_back(Point3D(pMlcLeaf[i].max_w, brdy[1], -1 * sad));
                ptList.push_back(Point3D(pMlcLeaf[i].max_w, pMlcLeaf[i].pos_2, -1 * sad));
                ptList.push_back(Point3D(pMlcLeaf[i].min_w, pMlcLeaf[i].pos_2, -1 * sad));
            }
        }
        (*it)->Set3DSegmentPtList(ptList);
        
        if (leafDirection == LEAF_DIRECTION_Y)
        {
            swap(brdy[0], brdy[2]);
            swap(brdy[1], brdy[3]);
        }
        (*it)->SetCollimatorDisplayRange(brdy);
    }

    //Fluence map
    std::shared_ptr<FluenceMapEntity> fluenceMap(mFluenceMapEntity == nullptr ? 
        new FluenceMapEntity() : new FluenceMapEntity(*mFluenceMapEntity));
    beamGo->SetFluenceMapEntity(fluenceMap);

    //////////////////////////////////////////////////////////////////////////
    //wedge
    //electron plan does not have accessories
    if (isElectronNG) return true;

    //physicalwedge
    RtAccessory* pPhysicalWedge = nullptr;
    const std::string sPhyicalUid = mBeam->get_physicalwedgeuid();
    if (!sPhyicalUid.empty())
    {
        mDataRepository->GetMachineManager()->GetAccessory(sPhyicalUid, pPhysicalWedge);
        if (nullptr != pPhysicalWedge)
        {
            para.obj_uid = sPhyicalUid;
            auto wedge = mWedgeConverter->Convert(para);
            if (wedge == nullptr)
            {
                TPS_LOG_DEV_ERROR << "Failed to convert wedge which uid: "<<sPhyicalUid;
            }
            else
            {
                beamGo->SetPhysicalWedgeGO(std::dynamic_pointer_cast<TpsWedgeGraphicObject>(wedge));
            }
        }
    }

    //virtualwedge
    RtAccessory* pVirtualWedge = nullptr;
    const std::string sVirtualUid = mBeam->get_virtualwedgeuid();
    if (!sVirtualUid.empty())
    {
        mDataRepository->GetMachineManager()->GetAccessory(sVirtualUid, pVirtualWedge);
        if (nullptr != pVirtualWedge)
        {
            para.obj_uid = sVirtualUid;
            auto wedge = mWedgeConverter->Convert(para);
            if (wedge == nullptr)
            {
                TPS_LOG_DEV_ERROR << "Failed to convert wedge which uid: "<<sVirtualUid;
            }
            else
            {
                beamGo->SetVirtualWedgeGO(std::dynamic_pointer_cast<TpsWedgeGraphicObject>(wedge));
            }
        }
    }
    return true;
}
TPS_END_NAMESPACE  // end namespace tps
