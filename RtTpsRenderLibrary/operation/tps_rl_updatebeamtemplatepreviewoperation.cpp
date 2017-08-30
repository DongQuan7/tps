////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \Author SHI Chunhui mailto:chunhui.shi@united-imaging.com
/// 
///  \file tps_rl_updatebeamtemplatepreviewoperation.cpp
/// 
///  \brief class UpdateBeamTemplatePreviewOperation
/// 
///  \version 1.0
/// 
///  \date    2014/10/28
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "boost/algorithm/string.hpp"

// Zhenghe
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

//#include "RtTpsDataAccess/tps_da_arithmeticdefines.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_mask.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_configuremanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_machine.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_plan.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_ieccoordinatesystem.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_simplebeamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamtemplategraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamtemplategraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_updatebeamtemplatepreviewoperation.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"

TPS_BEGIN_NAMESPACE

UpdateBeamTemplatePreviewOperation::UpdateBeamTemplatePreviewOperation(std::string beamTemplateUid, std::string normGroupUid, std::string isocenterUid)
    : mBeamTemplateUid(beamTemplateUid), mNormGroupUid(normGroupUid), mIsocenterUid(isocenterUid) {

}

UpdateBeamTemplatePreviewOperation::~UpdateBeamTemplatePreviewOperation(){

}

bool UpdateBeamTemplatePreviewOperation::ModifyGraphicObject() const{

    RtNormgroup* normGroup = nullptr;
    if (!mDataRepository->GetNormgroupManager()->GetNormGroup(mNormGroupUid, &normGroup)
        || normGroup == nullptr) {

        TPS_LOG_DEV_ERROR << "Failed to get normal group: " << mNormGroupUid;
        return false;
    }

    std::string sPlanUid = normGroup->get_planuid();
    RtPlan* plan = nullptr;
    if (!mDataRepository->GetPatientManager()->GetPlan(sPlanUid, &plan)
        || plan == nullptr) {

        TPS_LOG_DEV_ERROR << "Failed to get plan: " << sPlanUid;
        return false;
    }

    std::string sSeriesUid = plan->get_imageseriesuid();
    TpsImage3DEntity* image = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(sSeriesUid, &image)
        || image == nullptr) {

        TPS_LOG_DEV_ERROR << "Failed to get image 3d data which UID: " << sSeriesUid;
        return false;
    }

    RtMachine* machine = nullptr;
    if (!mDataRepository->GetMachineManager()->GetMachine(normGroup->get_machineuid(), machine)
        || machine == nullptr) {

        TPS_LOG_DEV_ERROR << "Failed to get machine: " << normGroup->get_machineuid();
        return false;
    }

    RtPoi* isoCenter = mDataRepository->GetPoiManager()->GetPOI(mIsocenterUid);
    if (isoCenter == nullptr) {

        TPS_LOG_DEV_ERROR << "Failed to get isocenter: " << mIsocenterUid;
        return false;
    }

    BeamTemplate* beamTemplate = nullptr;
    if (!mDataRepository->GetConfigurationManager()->GetBeamTemplate(mBeamTemplateUid, &beamTemplate)
        || beamTemplate == nullptr) {

        TPS_LOG_DEV_ERROR << "Failed to get beam template: " << mBeamTemplateUid;
        return false;
    }

    Mcsf::Point3f isoCenter3f(isoCenter->get_coordinate3dx(),
        isoCenter->get_coordinate3dy(), isoCenter->get_coordinate3dz());

    Mcsf::Point3f orgPt = *image->GetSetupPoint();
    RtImage3DHeader* header = nullptr;
    if (!image->GetImage3DHeader(header)
        || header == nullptr) {

        TPS_LOG_DEV_ERROR << "Failed to get current image 3d header.";
        return false;
    }

    RtSeries* pSeries = image->GetRtSeries();
    if (nullptr == pSeries){
        TPS_LOG_DEV_ERROR<<"pSeries is null";
        return false;
    }
    PatientPosition pp = (PatientPosition)GetPatientPosition_i(pSeries->get_patientposition());

    double minBoundary = machine->get_negativeleafrangemin();
    double maxBoundary = machine->get_positiveleafrangemax();

    const std::string templatizedNgCollectionGoKey = "TemplatizedNgCollectionGo";
    auto goTemplatizedNgCollection = mModelWarehouse->GetModelObject(templatizedNgCollectionGoKey);
    auto templatizedNgCollectionGo = std::dynamic_pointer_cast<NormalGroupCollection>(goTemplatizedNgCollection);
    if (templatizedNgCollectionGo == nullptr){
        TPS_LOG_DEV_INFO << " Get null go.";
        return false;
    }

    templatizedNgCollectionGo->ClearAll();

    std::shared_ptr<TpsBeamGraphicObjectCollection> templatizedNgGo(new TpsBeamGraphicObjectCollection());

    int beamNum = 0;
    for (auto itTemplatizedBeam = beamTemplate->mBeamCollection.begin();
        itTemplatizedBeam != beamTemplate->mBeamCollection.end(); itTemplatizedBeam++) {

        std::shared_ptr<TpsBeamGraphicObject> beamGo(new TpsBeamGraphicObject());
		if (fabs(itTemplatizedBeam->mArcLength) <= 1e-6)
		{
			beamGo->SetBeamType(IMRT);
		}
		else
		{
			beamGo->SetBeamType(ARC);
		}

        std::shared_ptr<TpsSegmentGraphicObject> segmentGO(new TpsSegmentGraphicObject());

        TpsIECCoordinateSystem iecCS;
        if (!iecCS.Initialize(orgPt, pp, machine->get_sad(),
            itTemplatizedBeam->mGantryStartAngle, itTemplatizedBeam->mCollimatorAngle, itTemplatizedBeam->mCouchAngle)) {
            TPS_LOG_DEV_ERROR << "Failed to initialize IEC coordinate system.";
            continue;
        }

        iecCS.SetISOCenter(isoCenter3f);

        Mcsf::Matrix4f matB2P;
        if (!iecCS.GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matB2P)){
            TPS_LOG_DEV_ERROR << "Failed to get beam to patient matrix.";
            continue;
        }

        float arrayB2P[16];
        matB2P.GetElements(arrayB2P);

        Mcsf::Matrix4f matBeamToPatient = TpsArithmeticConverter::ConvertToMatrix4f(arrayB2P);
        segmentGO->SetStartCollimatorToPatMatrix(
            TpsArithmeticConverter::ConvertToMatrix4x4(matBeamToPatient));
        double gantryEndAngle = itTemplatizedBeam->mGantryStartAngle + 
            (itTemplatizedBeam->mIsClockwise ? 1 : -1) * itTemplatizedBeam->mArcLength;
        iecCS.RotateGantry(gantryEndAngle);
        iecCS.GetTransformMatrix(_BEAM_LIMITING_DEVICE, _PATIENT, matBeamToPatient);
        segmentGO->SetEndCollimatorToPatMatrix(
            TpsArithmeticConverter::ConvertToMatrix4x4(matBeamToPatient));

        segmentGO->SetStartGantryAngle(itTemplatizedBeam->mGantryStartAngle);
        segmentGO->SetGantryArcLength(itTemplatizedBeam->mArcLength);
        segmentGO->SetCollimatorAngle(itTemplatizedBeam->mCollimatorAngle);
        segmentGO->SetGantryIsClockWise(itTemplatizedBeam->mIsClockwise);

        double defaultShape[4] = { -0.5 * 100,
            0.5 * 100,
            -0.5 * 100,
            0.5 * 100 };

        float aJawPos[4];
        aJawPos[0] = static_cast<float>(defaultShape[0]);
        aJawPos[1] = static_cast<float>(defaultShape[1]);
        aJawPos[2] = static_cast<float>(defaultShape[2]);
        aJawPos[3] = static_cast<float>(defaultShape[3]);
        segmentGO->SetJawPosition(aJawPos);

        double leafPos1 = defaultShape[0];
        double leafPos2 = defaultShape[1];

        segmentGO->SetLeafNum(machine->get_leafnum());
        for (unsigned int j = 0; j < machine->get_leafnum(); ++j){
            segmentGO->SetLeafPosition(j,
                leafPos1,
                leafPos2,
                (float)minBoundary,
                (float)maxBoundary);
        }

        beamGo->AddSegmentGo(segmentGO);

        stringstream ss;
        ss << "TemplatizedBeamGo" << beamNum++;
        templatizedNgGo->AddBeam(ss.str(), beamGo);
    }

    templatizedNgCollectionGo->AddNormalGroup("TemplatizedNgGo", templatizedNgGo);
    templatizedNgCollectionGo->SetDirty(true);

    return true;
}

int UpdateBeamTemplatePreviewOperation::GetPatientPosition_i(std::string sPP) const {
    boost::to_upper(sPP);
    PatientPosition pp = UNKNOWN_POSITION;
    if (sPP == "HFS")       { pp = HFS; }
    else if (sPP == "HFP") { pp = HFP; }
    else if (sPP == "HFP") { pp = HFP; }
    else if (sPP == "HFDR") { pp = HFDR; }
    else if (sPP == "HFDL") { pp = HFDL; }
    else if (sPP == "FFS") { pp = FFS; }
    else if (sPP == "FFP") { pp = FFP; }
    else if (sPP == "FFDR") { pp = FFDR; }
    else if (sPP == "FFDL") { pp = FFDL; }
    return pp;
}

TPS_END_NAMESPACE
