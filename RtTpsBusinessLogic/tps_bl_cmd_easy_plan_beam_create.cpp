////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  Song LI  song.li@united-imaging.com
/// 
///  \file tps_bl_cmd_easy_plan_beam_create.cpp
/// 
///  \brief class  EasyPlanBeamCreateCmd 
/// 
///  \version 1.0
/// 
///  \date    Oct.15,2015
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_easy_plan_beam_create.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"                 // for ITpsDataRepository
#include "RtTpsRenderController/tps_rc_renderproxy.h"               // for TpsRenderProxy
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"          // for GOTypeHelper

#include "RtTpsDataAccess/tps_da_normgroupmanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"    // for RtNormgroup
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"         // for RtPlan
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"      // for RtMachine
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"         // for RtBeam
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_block.h"        // for RtBlock
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"  // for RtBeamsegment

//TPS
//#include "RtTpsFramework/tps_fw_commproxy.h"                            // for TpsCommProxy
#include "RtTpsFramework/tps_fw_commandids.h"
#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"
#include "McsfAlgorithms/McsfAppLib/McsfAlgoAutoContour.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"

#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_datasourcegroup.h"
#include "RtTpsFramework/tps_fw_fire_operation.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"
#include "RtTpsFramework/tps_fw_common_datastructure.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"


#include "RtTpsProtoLib/rt_tps_proto.pb.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedbeam.h"
#include "RtTpsProtoLib/rt_tps_poigroup.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_beam.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_segment.pb.h"
#include "RtTpsProtoLib/rt_tps_proto_beam.pb.h"

#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_image2dentity.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_objects.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_uid_generater.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_data.h"

#include "RtTpsAlgorithmProxy/tps_pl_collimator.h"
#include "RtTpsAlgorithmProxy/tps_pl_datastructure.h"
#include "RtTpsAlgorithmProxy/tps_pl_utility.h"
#include "RtTpsAlgorithmProxy/tps_pl_make_bolus.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "tps_logger.h"

#define DEFAULT_FIELD_SIZE 100//mm

TPS_BEGIN_NAMESPACE;

EasyPlanBeamCreateCmd::EasyPlanBeamCreateCmd(TpsCertifiedBeam& certifiedBeam, /*proto::RtTpsProtoBeam** protoBeam,*/ int dataSourceID)
:mCertifiedBeam(certifiedBeam),/*mProtoBeam(protoBeam),*/ mDataSourceId(dataSourceID)
{
}

EasyPlanBeamCreateCmd::~EasyPlanBeamCreateCmd(void) 
{

}

int EasyPlanBeamCreateCmd::Execute() 
{
    TPS_LOG_DEV_INFO<<"[INFO][CreateBeamCmd][Execute]: "<<"CreateBeamCmd execute!";

    std::string strNormGroupUid = mCertifiedBeam.mNormGroupUID;
    std::string strCommissionedUnitUid = mCertifiedBeam.mCommissionedUnitUID;
    int BeamNumber = mCertifiedBeam.mBeamNumber;
    //float gantryStartAngle = mCertifiedBeam.mGantryStartAngle;
    //float gantryEndAngle = mCertifiedBeam.mGantryEndAngle;
    //float couchAngle = mCertifiedBeam.mCouchAngle;
    //float collimatorAngle = mCertifiedBeam.mCollimatorAngle;

    //(1) Check norm group
    TpsNormGroupManager* pNormGroupManager = mDataRepository->GetNormgroupManager();
    RtNormgroup* pNormGroup = nullptr;
    if (pNormGroupManager == nullptr)
    {
        return TPS_ER_FAILURE;
    }
    if (pNormGroupManager->GetNormGroup(strNormGroupUid, &pNormGroup) != true)
    {
        TPS_LOG_DEV_ERROR<<"Failed to find normgroup uid :"<< mCertifiedBeam.mNormGroupUID;
        return TPS_ER_FAILURE;
    }
    if (pNormGroup == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get normgroup:"<< strNormGroupUid;
        return TPS_ER_FAILURE;
    }

    // (2) Check plan
    std::string strPlanUid = pNormGroup->get_planuid();
    TpsPatientManager* pPatientManager = mDataRepository->GetPatientManager();
    RtPlan* pPlan = nullptr;
    if (pPatientManager == nullptr)
    {
        return TPS_ER_FAILURE;
    }
    if (pPatientManager->GetPlan(strPlanUid, &pPlan) != true)
    {
        TPS_LOG_DEV_ERROR<<"Failed to find plan uid :"<< strPlanUid;
        return TPS_ER_FAILURE;
    }
    if (pPlan == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get plan:" <<  strPlanUid;
        return TPS_ER_FAILURE;
    }

    // (3) Check commission unit
    TpsMachineManager* pMachineManager = mDataRepository->GetMachineManager();
    RtCommissionedunit* pUnit = nullptr;
    if (pMachineManager == nullptr)
    {
        return -1;//TPS_ER_FAILURE;
    }
    if (pMachineManager->GetCommissionedUnit(strCommissionedUnitUid, pUnit) != true)
    {
        TPS_LOG_DEV_ERROR<<"Failed to find unit uid :"<< strCommissionedUnitUid;
        return TPS_ER_FAILURE;
    }
    if (pUnit == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get unit :" <<  strCommissionedUnitUid;
        return TPS_ER_FAILURE;
    }

    // (4) Check machine
    std::string strMachineUid = pNormGroup->get_machineuid();
    RtMachine* pMachine = nullptr;
    if (pMachineManager->GetMachine(strMachineUid, pMachine) != true)
    {
        TPS_LOG_DEV_ERROR<<"Failed to find machine uid :"<< strMachineUid;
        return TPS_ER_FAILURE;
    }
    if (pMachine == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get machine :" <<  strMachineUid;
        return TPS_ER_FAILURE;
    }
    

    //std::string strImageUid = pPlan->get_imageseriesuid();
    //RtDbDef::RADIATION_TYPE radType = (RtDbDef::RADIATION_TYPE)pNormGroup->get_radiationtype();
    //RtDbDef::PLAN_TYPE planType = (RtDbDef::PLAN_TYPE)pNormGroup->get_plantype();
    //if (!ValidateData(planType, radType)){
    //    TPS_LOG_DEV_ERROR<<"Failed to validate beam parameters.";
    //    return TPS_ER_FAILURE;
    //}
     //todo now energy stored in RTFWKDLDOCommissionedunit is string, need modify to float
    //float fEnergy = pUnit->get_energy();

    // (6) Check beam manager
    TpsBeamManager* pBeamManager = mDataRepository->GetBeamManager();
    if (pBeamManager == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to find beam manager";
        return TPS_ER_FAILURE;
    }

    //RtBeam beam;
    RtBeam* pBeam = new RtBeam(true);                                   // will generate unique beam uid
    std::string strBeamUid = pBeam->get_uid();                      // unique UID in database
    mCertifiedBeam.mBeamUID = strBeamUid;
    pBeam->set_beamnumber(mCertifiedBeam.mBeamNumber);
    pBeam->set_name(mCertifiedBeam.mBeamName);
    pBeam->set_description(mCertifiedBeam.mBeamDescription);
    pBeam->set_isvisible(mCertifiedBeam.mIsVisible);

    pBeam->set_ssd(mCertifiedBeam.mSsd);

    pBeam->set_normgroupuid(mCertifiedBeam.mNormGroupUID);
    pBeam->set_commissionedunituid(mCertifiedBeam.mCommissionedUnitUID);
    
    // 冗余属性，radiationType已经包含在NormGroup， machineEnergy已经包含在CommissionedUnit里
    // alg may use these two fields
    //pBeam->set_radiationtype((RtDbDef::RADIATION_TYPE)pNormGroup->get_radiationtype()); //same as RadiationType of normgroup
    //pBeam->set_machineenergy(fEnergy);

    //300A,00C4 Beam Type 
    //
    //Motion characteristic of Beam.
    //    Enumerated Values:
    //     STATIC  = all beam parameters remain unchanged during delivery
    //     DYNAMIC = one or more beam parameters changes during delivery

    //     RTFWK::BEAM_TYPE beamType
    //      STATICIMRT = 1,
    //      DYNAMICIMRT,
    //      BURST_ARC,
    //      CONTINOUS_ARC,
    RtDbDef::BEAM_TYPE beamType = RtDbDef::STATICIMRT;
    pBeam->set_beamtype(beamType);

    // Check Isocenter
    //std::string strIsocenterUid = mCertifiedBeam.mIsocenterUid;
    //RtPoi* pIsocenter = mDataRepository->GetPoiManager()->GetPOI(strIsocenterUid);
    //if (pIsocenter == nullptr) {
    //    TPS_LOG_DEV_ERROR<<"The iso center does not exist of uid: "<< strIsocenterUid;
    //    return TPS_ER_FAILURE;
    //}
    pBeam->set_isocenterpoiuid(mCertifiedBeam.mIsocenterUid);
    if (mCertifiedBeam.mHasBeamWeight)
    {
        pBeam->set_beamweight(mCertifiedBeam.mBeamWeight);
    }

    pBeam->set_beamdeliverytype(RtDbDef::TREATMENT_BEAM);   //now set treatment beam as default delivery type, in the future may change
    pBeam->set_doserate(mCertifiedBeam.mDoseRate);

    pBeam->set_isjoincalc(mCertifiedBeam.mIsJoined);
    pBeam->set_isjoinopt(mCertifiedBeam.mIsJoined);
    pBeam->set_aperturetype(RtDbDef::APERTURE_TYPE(mCertifiedBeam.mApertureType));          //0:MLC 1:JawOnly 2:Cone 3:Aperture Block
    //pBbeam->SetIsClockWise(mCertifiedBeam.mIsClockWise);

    pBeam->create_aperture_block(); 
    RtBlock* pBlock = pBeam->get_aperture_block();

    std::vector<RtBeamsegment*> vecSegmentPtr; //= pBeam->get_beamsegments();
    RtBeamsegment* pSegment = new RtBeamsegment(true);
    vecSegmentPtr.push_back(pSegment);

    pSegment->set_startgantryangle(mCertifiedBeam.mGantryStartAngle);
    pSegment->set_arclength(0.0f);

    pSegment->set_starttablerotationangle(0);
    pSegment->set_endtablerotationangle(mCertifiedBeam.mCouchAngle);

    pSegment->set_startcollimatorangle(mCertifiedBeam.mCollimatorAngle);
    pSegment->set_endcollimatorangle(mCertifiedBeam.mCollimatorAngle);

    pSegment->set_weight(1.0f);

    pSegment->set_sequencenumber(0);
    //pSegment->set_t_beam_to_pat(matB2P);
    ////delete this later
    ////segment->Set_pat_to_beam(matB2P.Inverse());
    pSegment->set_gantryrotationdirection(RtDbDef::NONE_ROTATION_DIRECTION);
    pSegment->set_collimatorrotationdirection(RtDbDef::NONE_ROTATION_DIRECTION);//default direction
    pSegment->set_beamuid(strBeamUid);

    if (!CreateDefaultSingleSegment(pSegment, pMachine, pBlock))
    {
        TPS_LOG_DEV_ERROR<<"Failed to create single segment.";
        return false;
    }

    //shielding block or blocks for electron beam
	if (mCertifiedBeam.mUseShieldingBlock || mCertifiedBeam.mUseEBeamBlock)
	{
		pBeam->create_shield_block();
		CreateEmptyBlock_i(pSegment, pBeam->get_shield_block());
	}
    pBeam->set_beamsegments(vecSegmentPtr);
	for (auto itr=vecSegmentPtr.begin(); itr!=vecSegmentPtr.end(); ++itr) DEL_PTR(*itr);

    //create empty dose grid
    if (!CreateDosegridForBeam(pPlan, pNormGroup, pBeam)) {
        TPS_LOG_DEV_ERROR<<"Failed to create empty dose grid for beam["<<BeamNumber<<"].";
        delete pBeam; pBeam = nullptr;
        return TPS_ER_FAILURE;
    }

    //if (nullptr == mDatabaseWrapper)
    //{
    //    return TPS_ER_DB_ERROR;
    //}
    //if (DB_CODES_SUCCESS != mDatabaseWrapper->InsertBeamObject(*pBeam))
    //{
    //    TPS_LOG_DEV_ERROR<<"InsertNormgroupObject failed! uid:"<< strBeamId;
    //    return TPS_ER_DB_ERROR;
    //}

    if(!pBeamManager->AddBeam2Map(pBeam))
    {
        TPS_LOG_DEV_ERROR<<"AddBeam2Map Failed with beam UID:"<<BeamNumber;
        delete pBeam;
        pBeam = nullptr;
        return TPS_ER_FAILURE;
    }

    tps::TpsImageDataManager *pImageDataManager = mDataRepository->GetImageDataManager();
    std::string imageUID = "";
    // use the EasyPlan image to this beam.
    if (mCertifiedBeam.mIsOppositeBeam) {
        pImageDataManager->GetEasyPlanOpposedImageUID(strPlanUid, &imageUID);
    }
    // if it is opposed beam, use the EasyPlanOpposed image to the opposed beam.
    else {
        pImageDataManager->GetEasyPlanImageUID(strPlanUid, &imageUID);
    }
    tps::TpsImage2DEntity* image2DData = nullptr;
    pImageDataManager->GetImage2DData(imageUID, &image2DData);
    if(image2DData != nullptr && image2DData->get() != nullptr){
        image2DData->get()->set_beamuid(mCertifiedBeam.mBeamUID);
    }

    if(!imageUID.empty()) {
        // set the relation between beam uid and image uid.
        //pImageDataManager->SetEasyPlanBeamImageUID(mCertifiedBeam.mBeamUID, imageUID);
        RtBeam* beam = pBeamManager->GetBeam(mCertifiedBeam.mBeamUID);
        if(beam != nullptr){
            beam->set_relatedrtimageuid(imageUID);
        }
    }

    DATA_SOURCE_GROUP_ID id = (DATA_SOURCE_GROUP_ID)mDataSourceId;
    //mRenderProxy->SetDataSource(id, IMAGE, pPlan->get_imageseriesuid());
    mRenderProxy->SetDataSource(id, IMAGE, imageUID);
    mRenderProxy->SetDataSource(id, PLAN, pNormGroup->get_planuid());

    if (!mRenderProxy->AddBeam(strNormGroupUid, strBeamUid))
    {
        TPS_LOG_DEV_ERROR<<"Failed to add beam in render proxy.";
        return TPS_ER_FAILURE;
    }
    mRenderProxy->ResetBeamViewCameras(strPlanUid);
    mRenderProxy->Render();

    //ConvertBeamToProto(pBeam, *mProtoBeam);
    TPS_LOG_DEV_INFO<<"[INFO][PlanningCreateBeamHandler][HandleCommand]: "
        <<"Add beam command leave!";

    return TPS_ER_SUCCESS;
}

void EasyPlanBeamCreateCmd::ConvertBeamToProto(const RtBeam* beam, proto::RtTpsProtoBeam* protoBeam) 
{
    protoBeam->set_beam_uid(beam->get_uid());
    protoBeam->set_beam_number(beam->get_beamnumber());
    protoBeam->set_normal_group_uid(beam->get_normgroupuid());
    protoBeam->set_beam_name(beam->get_name());
    protoBeam->set_commissioned_unit_uid(beam->get_commissionedunituid());
    protoBeam->set_isocenter_uid(beam->get_isocenterpoiuid());
    protoBeam->set_is_visible(beam->get_isvisible());
    protoBeam->set_is_joined(beam->get_isjoincalc());//TODO
    //protoBeam->set_bolus_thickness(beam->get_bolusthickness());
    protoBeam->set_bolus_voi_uid(beam->get_bolusvoiuid());
    protoBeam->set_beam_mu(beam->get_beammu());
    protoBeam->set_beam_weight(beam->get_beamweight());
    protoBeam->set_beam_description(beam->get_description());
    protoBeam->set_is_auto_created(beam->get_isautocreated());
    protoBeam->set_dose_rate(beam->get_doserate());
    protoBeam->set_ssd(beam->get_ssd());

    // Geometry
    if (beam->get_beamsegments().size()>0) {
        RtBeamsegment* segment0 = beam->get_beamsegments()[0];
        protoBeam->set_gantry_start_angle(segment0->get_startgantryangle());
        protoBeam->set_collimator_angle(segment0->get_startcollimatorangle());
        protoBeam->set_couch_angle(segment0->get_starttablerotationangle());
        protoBeam->set_gantry_arc_length(0.0f);
    }

    //Aperture Type
    //0:MLC 1:JawOnly 2:Applicator 3:Aperture Block
    protoBeam->set_aperture_type(beam->get_aperturetype());

    //////////////////////////////////////////////////////////////////////////
    //Accessory
    //physical wedge
    //rcc: 暂时注掉，等加载过程补上accessory和applicator后打开注释
    //std::vector<std::string> vAccessoriesUidList;
    //vAccessoriesUidList = beam->GetAccessoryUids();
    //protoBeam->set_use_physical_wedge(false);
    //protoBeam->set_use_virtual_wedge(false);
    //for (auto itAccessory = vAccessories.begin(); itAccessory != vAccessories.end(); ++itAccessory) {
    //    mDataRepository->GetMachineManager()->GetAccessory(*itAccessory, )
    //    if(RtDbDef::ACCESSORY_PHYSICALWEDGE == (*itAccessory)->get_accessorytype()){
    //        const std::string PhysicalWedgeUid = protoBeam->physical_wedge_uid();
    //        protoBeam->set_physical_wedge_uid(PhysicalWedgeUid);
    //        protoBeam->set_use_physical_wedge(true);
    //    }
    //    else if (RtDbDef::ACCESSORY_VIRTUALWEDGE == (*itAccessory)->get_accessorytype()){
    //        const std::string VirtualWedgeUid = protoBeam->virtual_wedge_uid();
    //        protoBeam->set_virtual_wedge_uid(VirtualWedgeUid);
    //        protoBeam->set_use_virtual_wedge(true);
    //    }
    //}

    //bolus
    //protoBeam->set_use_bolus(false);

    //applicator
    protoBeam->set_applicator_uid(beam->get_applicatoruid());

    std::vector<RtBeamsegment*> beamsegmentList;
    beamsegmentList = beam->get_beamsegments();
    proto::RtTpsProtoSegment* protoSegment = nullptr;
    bool bClockwise = true;
    bool isFirstSegment = true;
    for (auto itBeamsegment = beamsegmentList.begin(); itBeamsegment != beamsegmentList.end(); ++itBeamsegment) {
        protoSegment = protoBeam->add_segment_list();
        bClockwise = (*itBeamsegment)->get_gantryrotationdirection()==RtDbDef::CW;
        protoSegment->set_segmentuid((*itBeamsegment)->get_uid());
        protoSegment->set_beamuid((*itBeamsegment)->get_beamuid());
        protoSegment->set_segmentmu((*itBeamsegment)->get_mu());
        protoSegment->set_segmentweight((*itBeamsegment)->get_weight());
        protoSegment->set_sequencenumber((*itBeamsegment)->get_sequencenumber());

        RtMlcshape* pMlcShape = (*itBeamsegment)->get_startmlcshape();
        if(nullptr != pMlcShape) {
            auto jaw = protoSegment->mutable_startjaw();
            jaw->set_x1(pMlcShape->get_xlowerjawpos());
            jaw->set_x2(pMlcShape->get_xupperjawpos());
            jaw->set_y1(pMlcShape->get_ylowerjawpos());
            jaw->set_y2(pMlcShape->get_yupperjawpos());

            if (isFirstSegment){
                isFirstSegment = false;
                protoBeam->set_is_auto_tracking_shaped(beam->get_isautoshape());
                protoBeam->set_target_voi_uid(beam->get_targetvoiuid());
                protoBeam->set_target_margin_x(beam->get_targetmarginx());
                protoBeam->set_target_margin_y(beam->get_targetmarginy());
                protoBeam->set_avoid_voi_uid(beam->get_avoidvoiuid());
                protoBeam->set_avoid_margin_x(beam->get_avoidmarginx());
                protoBeam->set_avoid_margin_y(beam->get_avoidmarginy());

                //Jaw
                protoBeam->set_is_jaw_follow(RtDbDef::JAWTYPE_Follow == beam->get_jawtype());
                protoBeam->set_is_x_symmetric(pMlcShape->get_jawxissymmetry());
                protoBeam->set_is_y_symmetric(pMlcShape->get_jawyissymmetry());
                protoBeam->set_jaw_follow_margin_x(beam->get_jawmarginx());
                protoBeam->set_jaw_follow_margin_y(beam->get_jawmarginy());
            }
        }
    }
    protoBeam->set_is_clockwise(bClockwise);

    //Block
    RtBlock* pBlock = beam->get_aperture_block();
    if (nullptr != pBlock){
        protoBeam->set_use_shielding_block(RtDbDef::SHIELDING == pBlock->get_blocktype());

        //commented by GML !!!!!!!!!!
        ////protoBeam->set_use_block_for_e_beam();TODO
        //protoBeam->set_block_type(pBlock->get_blocktype());
        //protoBeam->set_block_uid(pBlock->get_uid());
        //protoBeam->set_is_auto_shaped_block(pBlock->get_isautoshape());
        //protoBeam->set_block_name(pBlock->get_blockname());
        ////protoBeam->set_block_material(pBlock->GetMaterialID());
        //protoBeam->set_block_thickness(pBlock->get_blockthickness());
        //protoBeam->set_target_voi_uid_block(pBlock->get_targetvoiuid());
        //protoBeam->set_target_margin_x_block(pBlock->get_targetmarginx());
        //protoBeam->set_target_margin_y_block(pBlock->get_targetmarginy());
        //protoBeam->set_avoid_voi_uid_block(pBlock->get_avoidvoiuid());
        //protoBeam->set_avoid_margin_x_block(pBlock->get_avoidmarginx());
        //protoBeam->set_avoid_margin_y_block(pBlock->get_avoidmarginy());
        const std::string sTrayUid = beam->get_trayuid();
        protoBeam->set_tray_uid(sTrayUid);
    }
}

void EasyPlanBeamCreateCmd::GetBeamCreated(const RtBeam*& pBeam)
{
    RtBeam* newBeam = mDataRepository->GetBeamManager()->GetBeam(mCertifiedBeam.mBeamUID);
    pBeam = newBeam;
}

bool EasyPlanBeamCreateCmd::CreateRectShape_i(RtBeamsegment* pSegment, 
    RtMachine* const pMachine, double* rectSize/* = nullptr*/, int mode/* = 0*/)
{
    //create default MLC shape
    bool isAllOpen = rectSize == nullptr;
    double minBoundary = pMachine->get_negativeleafrangemin();
    double maxBoundary = pMachine->get_positiveleafrangemax();
    float fLeafMinGap = pMachine->get_minleafendgap();
    std::vector<double> leafBounds = pMachine->get_leafboundaries_list();

    int leafNum = pMachine->get_leafnum();
    RtDbDef::MLC_LEAF_DIRECTION_TYPE leafDirt = (RtDbDef::MLC_LEAF_DIRECTION_TYPE)pMachine->get_mlcdirectiontype();

    double leafPos1 = (!isAllOpen && (mode & Leaf_Followed)) ? (leafDirt == RtDbDef::MLCX_DIRECTION ?
        rectSize[0] : rectSize[2]) : minBoundary;
    double leafPos2 = (!isAllOpen && (mode & Leaf_Followed)) ? (leafDirt == RtDbDef::MLCX_DIRECTION ?
        rectSize[1] : rectSize[3]) : maxBoundary;
    std::vector<db_Point2d> vecLeafPos;
    vecLeafPos.reserve(leafNum);
    db_Point2d leafPos;
    for (int i = 0; i < leafNum; i++){
        if (!isAllOpen && (mode & Leaf_Followed))
        {
            if (leafBounds[i] >= rectSize[2] && leafBounds[i] < rectSize[3])
            {
                leafPos.x = leafPos1;
                leafPos.y = leafPos2;
            }
            else
            {
                leafPos.x = -0.5 * fLeafMinGap;
                leafPos.y = 0.5 * fLeafMinGap; 
            }
        }
        else
        {
            leafPos.x = leafPos1;
            leafPos.y = leafPos2;
        }
        vecLeafPos.push_back(leafPos);
    }

    double jawBdry[4] = 
    {
        pMachine->get_xmin_1(), 
        pMachine->get_xmax_2(), 
        pMachine->get_ymin_1(),
        pMachine->get_ymax_2(),
    };
    double jawPos[4];
    memcpy(jawPos, jawBdry, 4 * sizeof(double));
    if (!isAllOpen && (mode & Jaw_Followed)){
        jawPos[0] = TPS_CLAMP(rectSize[0], jawBdry[0], jawBdry[1]);
        jawPos[1] = TPS_CLAMP(rectSize[1], jawBdry[0], jawBdry[1]);
        jawPos[2] = TPS_CLAMP(rectSize[2], jawBdry[2], jawBdry[3]);
        jawPos[3] = TPS_CLAMP(rectSize[3], jawBdry[2], jawBdry[3]);
    }

    CreateMlcShapes(pSegment, vecLeafPos, jawPos, pMachine);
    return true;
}

bool EasyPlanBeamCreateCmd::CreateEmptyBlock_i(RtBeamsegment* segment, 
    RtBlock* block)
{
    if(segment == nullptr || block==nullptr) {
        TPS_LOG_DEV_ERROR<<"segment is null.";
        return false;
    }

    //commented by GML !!!!!!!!!!
    if(RtDbDef::APERTURE == block->get_blocktype()) {
        block->set_uid(mCertifiedBeam.mApertureBlock.mBlockUid);
        block->set_isautoshape(mCertifiedBeam.mIsAutoTrackingShape);
        block->set_targetvoiuid(mCertifiedBeam.mTargetVoiUid);
        block->set_targetmarginx(mCertifiedBeam.mTargetMarginX);
        block->set_targetmarginy(mCertifiedBeam.mTargetMarginY);
        block->set_avoidvoiuid(mCertifiedBeam.mAvoidVoiUid);
        block->set_avoidmarginx(mCertifiedBeam.mAvoidMarginX);
        block->set_avoidmarginy(mCertifiedBeam.mAvoidMarginY);
    }
    else {
        block->set_uid(mCertifiedBeam.mShieldingBlock.mBlockUid);
        block->set_blockname(mCertifiedBeam.mShieldingBlock.mBlockName);
        block->set_isautoshape(mCertifiedBeam.mIsShieldingBlockAutoTrackingShape);
        //block->set_blocktype((RtDbDef::BLOCK_TYPE)mCertifiedBeam.mBlockType);
        block->set_targetvoiuid(mCertifiedBeam.mShieldingBlockTargetVoiUid);
        block->set_targetmarginx(mCertifiedBeam.mShieldingBlockTargetMarginX);
        block->set_targetmarginy(mCertifiedBeam.mShieldingBlockTargetMarginY);
        block->set_avoidvoiuid(mCertifiedBeam.mShieldingBlockAvoidVoiUid);
        block->set_avoidmarginx(mCertifiedBeam.mShieldingBlockAvoidMarginX);
        block->set_avoidmarginy(mCertifiedBeam.mShieldingBlockAvoidMarginY);
    }
    //block->set_uid(mCertifiedBeam.mBlockUid);
    //block->set_blockname(mCertifiedBeam.mBlockName);
    //block->set_isautoshape(mCertifiedBeam.mIsAutoShapedBlock);
    //block->set_blocktype(mCertifiedBeam.mBlockType);
    return true;
}

bool EasyPlanBeamCreateCmd::CreateDefaultSingleSegment(RtBeamsegment* pSegment,
    RtMachine* const pMachine, 
    RtBlock* pBlock)
{
    if (pSegment == nullptr){
        TPS_LOG_DEV_ERROR<<"segment is null.";
        return false;
    }
    if (mCertifiedBeam.mSegmentList.empty()){
        return false;
    }
    TpsCertifiedSegment certSegment = mCertifiedBeam.mSegmentList[0];
    //默认10*10(cm)方野
    double defaultShape[4] = { -0.5 * DEFAULT_FIELD_SIZE,
                                0.5 * DEFAULT_FIELD_SIZE,
                                -0.5 * DEFAULT_FIELD_SIZE,
                                0.5 * DEFAULT_FIELD_SIZE};
    

    bool res = false;
    switch (mCertifiedBeam.mApertureType) {
    case RtDbDef::Mlc:
        //如果手动设定了Jaw位置，使用设定位置产生矩形野
        if (!mCertifiedBeam.mIsJawFollow) {
            defaultShape[0] = certSegment.startJaw.x1;
            defaultShape[1] = certSegment.startJaw.x2;
            defaultShape[2] = certSegment.startJaw.y1;
            defaultShape[3] = certSegment.startJaw.y2;
        }
        res = CreateRectShape_i(pSegment, pMachine, defaultShape, Jaw_Followed | Leaf_Followed);
        break;
    case RtDbDef::JawOnly:
        //如果手动设定了Jaw位置，使用设定位置产生矩形野
        if (!mCertifiedBeam.mIsJawFollow) {
            defaultShape[0] = certSegment.startJaw.x1;
            defaultShape[1] = certSegment.startJaw.x2;
            defaultShape[2] = certSegment.startJaw.y1;
            defaultShape[3] = certSegment.startJaw.y2;
        }
        res = CreateRectShape_i(pSegment, pMachine, defaultShape, Jaw_Followed);
        break;
    case RtDbDef::Aperture_Block:
        //包括两种情况：1.只有Avoid voi没有Target 2.没有选择任何voi做适形
        res = CreateEmptyBlock_i(pSegment, pBlock);
        //创建Mlc shape，Leaf保持全开，Jaw如果已设定，采用设定值，否则也全开
        if (!mCertifiedBeam.mIsJawFollow) {
            double defaultShape[4] = { 
                certSegment.startJaw.x1,
                certSegment.startJaw.x2,
                certSegment.startJaw.y1,
                certSegment.startJaw.y2};
            res &= CreateRectShape_i(pSegment, pMachine, defaultShape, Jaw_Followed);
        }
        else res &= CreateRectShape_i(pSegment, pMachine);
        break;
    case RtDbDef::Applicator:
        //adjust jaw to cone
        //res = CreateApplicatorShape_i(segment, machine);
        break;
    default:
        TPS_LOG_DEV_ERROR<<"Unrecognized aperture type!";
        break;
    }
    return res;
}


bool EasyPlanBeamCreateCmd::CreateDosegridForBeam(RtPlan* pPlan, RtNormgroup* pNormGroup, RtBeam* pBeam)
{
    if (pBeam == nullptr || pPlan == nullptr || pNormGroup == nullptr) 
    {
        TPS_LOG_DEV_ERROR<<"beam/normgroup/plan is nullptr";
        return false;
    }

    // The beam dosegrid will have the same dimension as that of normgroup
    RtDosegrid* pNormGroupDosegrid = pNormGroup->get_dosegrid();
    if (pNormGroupDosegrid == nullptr) {
        TPS_LOG_DEV_ERROR<<"Invalid dose grid buffer under norm-group ["<<pNormGroup->get_uid()<<"].";
        return false;
    }

    RtDosegrid* pPlanDosegrid = pPlan->get_dosegrid();
    if (pPlanDosegrid == nullptr) {
        TPS_LOG_DEV_ERROR<<"Invalid dose grid buffer under plan ["<<pPlan->get_uid()<<"].";
        return false;
    }

    RtDosegrid* pDosegrid = pBeam->get_dosegrid();
    pDosegrid->set_uid(pBeam->get_uid());
    pDosegrid->set_beamuid(pBeam->get_uid());
    pDosegrid->set_xcoordinate3dstart(pNormGroupDosegrid->get_xcoordinate3dstart());
    pDosegrid->set_ycoordinate3dstart(pNormGroupDosegrid->get_ycoordinate3dstart());
    pDosegrid->set_zcoordinate3dstart(pNormGroupDosegrid->get_zcoordinate3dstart());
    pDosegrid->set_xcoordinate3dinc(pNormGroupDosegrid->get_xcoordinate3dinc());
    pDosegrid->set_ycoordinate3dinc(pNormGroupDosegrid->get_ycoordinate3dinc());
    pDosegrid->set_zcoordinate3dinc(pNormGroupDosegrid->get_zcoordinate3dinc());
    pDosegrid->set_grid_to_pat_t(pNormGroupDosegrid->get_grid_to_pat_t());
    pDosegrid->set_xcount(pNormGroupDosegrid->get_xcount());
    pDosegrid->set_ycount(pNormGroupDosegrid->get_ycount());
    pDosegrid->set_zcount(pNormGroupDosegrid->get_zcount());
    pDosegrid->set_accuracy(pNormGroupDosegrid->get_accuracy());
    pDosegrid->create_dosegrid_buffer();
    float* buffer = pDosegrid->get_dosegrid_buffer();
    size_t bufferSize = pDosegrid->get_xcount() * pDosegrid->get_ycount() * pDosegrid->get_zcount();
    memset(buffer, 0, bufferSize * sizeof(float));
    pDosegrid->set_isdosevalid(false);
    pDosegrid->set_bitsallocated(TPS_BITSALLOCATED);
    pDosegrid->set_bitsstored(TPS_BITSALLOCATED);
    pDosegrid->set_highbits(TPS_BITSALLOCATED -1);
    //set normal group & plan dose invalid
    pNormGroupDosegrid->set_isdosevalid(false);
    pPlanDosegrid->set_isdosevalid(false);

    return true;
}

void EasyPlanBeamCreateCmd::CreateMlcShapes(RtBeamsegment* pSegment, 
                                            const std::vector<db_Point2d>& leafPositions,
                                            double jawPos[4],
                                            RtMachine* pMachine)
{
    // add shapes
    RtMlcshape* mlcStartShape = pSegment->get_startmlcshape();
    mlcStartShape->set_segmentuid(pSegment->get_uid());
//    mlcStartShape->set_jawtype(mCertifiedBeam.mIsJawFollow ? RtDbDef::JAWTYPE_Follow: RtDbDef::JAWTYPE_Manual);
    mlcStartShape->set_xlowerjawpos(jawPos[0]);
    mlcStartShape->set_xupperjawpos(jawPos[1]);
    mlcStartShape->set_jawxissymmetry(mCertifiedBeam.mIsXSymmetric);
    mlcStartShape->set_ylowerjawpos(jawPos[2]);
    mlcStartShape->set_yupperjawpos(jawPos[3]);
    mlcStartShape->set_jawyissymmetry(mCertifiedBeam.mIsYSymmetric);
    mlcStartShape->set_mlcdirectiontype(pMachine->get_mlcdirectiontype()); //rcc: 这个还需要确定一下，枚举是否对得上
//    mlcStartShape->set_isautoshape(mCertifiedBeam.mIsAutoTrackingShape);
    //mlcStartShape->set_targetvoiuid(mCertifiedBeam.mTargetVoiUid);
    //mlcStartShape->set_targetmarginx(mCertifiedBeam.mTargetMarginX);
    //mlcStartShape->set_targetmarginy(mCertifiedBeam.mTargetMarginY);
    //mlcStartShape->set_avoidvoiuid(mCertifiedBeam.mAvoidVoiUid);
    //mlcStartShape->set_avoidmarginx(mCertifiedBeam.mAvoidMarginX);
    //mlcStartShape->set_avoidmarginy(mCertifiedBeam.mAvoidMarginY);
    //mlcStartShape->set_jawmarginx(mCertifiedBeam.mJawFollowMarginX);
    //mlcStartShape->set_jawmarginy(mCertifiedBeam.mJawFollowMarginY);
    std::vector<db_Point2d> leafPos; //= mlcStartShape->get_leafpositions();
    for (auto itPt = leafPositions.begin(); itPt!=leafPositions.end(); ++itPt) {
        leafPos.push_back(*itPt);
    }
    mlcStartShape->set_leafpositions(leafPos);
    //TODO
    //mlcStartShape.SetJawType(RTFWK::JAWTYPE(RTFWK::JAWTYPE_Follow));

    RtMlcshape* mlcEndShape = pSegment->get_endmlcshape();
    const std::string mlcEndShapeUid = mlcEndShape->get_uid();
    *mlcEndShape = *mlcStartShape;
    mlcEndShape->set_uid(mlcEndShapeUid);
    mlcEndShape->set_isstartmlcshape(false);
}

TPS_END_NAMESPACE
