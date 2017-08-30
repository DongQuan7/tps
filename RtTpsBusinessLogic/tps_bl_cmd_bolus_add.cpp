//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  Wang Zhendong zhendong.wang@united-imaging.com
///
///  \file    tps_bl_cmd_bolus_add.cpp
///  \brief   addbolus
///
///  \version 1.0
///  \date    February. 9, 2017
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_bolus_add.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"

#include "RtTpsProtoLib/rt_tps_bolus.pb.h"

#include "boost/date_time/posix_time/posix_time.hpp"

//ZHENGHE
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_normgroup.h"
#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_beam.h"
#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_mlcshape.h"
#include "RtTpsDatabaseWrapper\rt_tps_database_interface_object_machine.h"

#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"

#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_entity_converter.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_normgroupmanager.h"
#include "RtTpsDataAccess/tps_da_machinemanager.h"
#include "RtTpsDataAccess/tps_da_beammanager.h"

#include "RtTpsAlgorithmProxy/tps_pl_make_bolus.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

#include "RtTpsDoseProxy/tps_dp_doseproxy.h"

#include "RtTpsRenderController/tps_rc_renderproxy.h"


TPS_BEGIN_NAMESPACE


AddBolusCmd::AddBolusCmd(proto::RtTpsBolus& protoBolus)
 : mProtoBolus(protoBolus)
{

}


AddBolusCmd::~AddBolusCmd(void)
{
   
}

bool AddBolusCmd::CreateBolus_i(const std::string& imageUID, RtBeam* beam)
{
    if (beam == nullptr)
    {
        TPS_LOG_DEV_ERROR << "beam is nullptr";
        return false;
    }

    std::vector<RtBeamsegment*> segments = beam->get_beamsegments();
    if (mProtoBolus.bolus_thickness() > 0 && !segments.empty())
    {
        // draw bolus
        float margin[4];
        margin[0] = mProtoBolus.margin_x1();
        margin[1] = mProtoBolus.margin_x2();
        margin[2] = mProtoBolus.margin_y1();
        margin[3] = mProtoBolus.margin_y2();

        float bolusThickness = mProtoBolus.bolus_thickness();
        float bolusMassDensity = mProtoBolus.mass_density();
        float bolusRerenceEDensity = mProtoBolus.reference_electron_density();
        std::string bolusVOIUID = MakeBolus(imageUID, beam->get_normgroupuid(),
            segments, bolusThickness, margin, bolusMassDensity, bolusRerenceEDensity);
        ROIEntity* pBolusVoi = mDataRepository->GetVoiManager()->GetROI(bolusVOIUID);
        if (bolusVOIUID.empty() || nullptr == pBolusVoi)
        {
            TPS_LOG_DEV_ERROR << "MakeBolus fail";
            return false;
        }

        if (mSaveObjects->SaveRoi(*pBolusVoi) != (int)TPS_ER_SUCCESS)
        {
            TPS_LOG_DEV_ERROR << "Save Bolus voi fail";
            return false;
        }

        beam->set_bolusvoiuid(bolusVOIUID);

        if (!mDoseProxy->UpdateFluenceMap(beam))
        {
            TPS_LOG_DEV_ERROR << "Failed to Update Fluence Map.";
            return false;
        }

        if (mSaveObjects->UpdateBeamAttributes(*beam) != (int)TPS_ER_SUCCESS)
        {
            TPS_LOG_DEV_ERROR << "Save beam fail";
            return false;
        }

        mProtoBolus.set_bolusuid(bolusVOIUID);
    }

    return true;
}

std::string AddBolusCmd::MakeBolus(const std::string& imageUID,
    const std::string& normalgroupuid,
    const std::vector<RtBeamsegment*>& segments,
    float bolusThickness,
    float margin[4],
    float bolusMassDensity,
    float bolusRerenceEDensity)
{
    std::string voiuid("");
    if (segments.empty()) return voiuid;

    tps::ROIEntity* pVOISkin = mDataRepository->GetVoiManager()->GetExternal(imageUID);

    Mcsf::Matrix4f matBeam2Pat = TpsArithmeticConverter::ConvertToMatrix4f(segments[0]->get_t_beam_to_pat());
    if (!matBeam2Pat.HasInverse())
    {
        return voiuid;
    }
    Mcsf::Matrix4f matPat2Beam = matBeam2Pat.Inverse();
    float fPat2Beam[4][4];
    matPat2Beam.GetElements(&fPat2Beam[0][0]);
    float fBeam2Pat[4][4];
    matBeam2Pat.GetElements(&fBeam2Pat[0][0]);

    std::vector<RtMlcshape*> shapes;
    shapes.reserve(segments.size());
    for (int i = 0; i < segments.size(); ++i)
    {
        shapes.push_back(segments[i]->get_startmlcshape());
    }

    RtNormgroup* normGroup = nullptr;
    mDataRepository->GetNormgroupManager()->GetNormGroup(normalgroupuid, &normGroup);
    if (nullptr == normGroup)
    {
        TPS_LOG_DEV_ERROR << "Failed to get normgroup.";
        return voiuid;
    }

    //get machine
    RtMachine* machine = nullptr;
    if (!mDataRepository->GetMachineManager()->GetMachine(normGroup->get_machineuid(), machine))
    {
        TPS_LOG_DEV_ERROR << "Failed to get machine.";
        return voiuid;
    }


    RtMlcshape* combinedShape = new RtMlcshape();
    if (!CombineMlcShapes_i(shapes, combinedShape, machine->get_minleafendgap()))
    {
        TPS_LOG_DEV_ERROR << "Failed to combine mlc shapes.";
        delete combinedShape;
        return voiuid;
    }
    double boundingBox[4] =
    {
        combinedShape->get_xlowerjawpos(),
        combinedShape->get_xupperjawpos(),
        combinedShape->get_ylowerjawpos(),
        combinedShape->get_yupperjawpos()};
        auto vecLeaves = combinedShape->get_leafpositions();
        for (int i = 0; i < vecLeaves.size(); ++i)
        {
            boundingBox[0] = TPS_MIN(vecLeaves[i].x, boundingBox[0]);
            boundingBox[1] = TPS_MAX(vecLeaves[i].x, boundingBox[1]);
            boundingBox[2] = TPS_MIN(vecLeaves[i].y, boundingBox[2]);
            boundingBox[3] = TPS_MAX(vecLeaves[i].y, boundingBox[3]);
        }
        delete combinedShape;

        plunc::CONTOUR_STC contour;
        memset(&contour, 0, sizeof(contour));
        contour.z = -1000;
        contour.x = (float*)malloc(4*sizeof(float));
        contour.y = (float*)malloc(4*sizeof(float));
        contour.vertex_count = 4;
        contour.x[0] = boundingBox[0];
        contour.y[0] = boundingBox[3];
        contour.x[1] = boundingBox[1];
        contour.y[1] = boundingBox[3];
        contour.x[2] = boundingBox[1];
        contour.y[2] = boundingBox[2];
        contour.x[3] = boundingBox[0];
        contour.y[3] = boundingBox[2];
        contour.max.x = TPS_MAX(boundingBox[0], boundingBox[1]);
        contour.max.y = TPS_MAX(boundingBox[2], boundingBox[3]);
        contour.max.z = -1000.0f;
        contour.min.x = TPS_MIN(boundingBox[0], boundingBox[1]);
        contour.min.y = TPS_MIN(boundingBox[2], boundingBox[3]);
        contour.min.z = -1000.0f;

        if (pVOISkin)
        {
            auto innerSkin = pVOISkin->GetInnerRoi();
            auto innerRoi = new RtVoi(true);
            tps::ROIEntity* pVOIBolus = mDataRepository->GetVoiManager()->CreateROIEntity(innerRoi);

            innerRoi->set_seriesuid(imageUID);
            innerRoi->set_isvisible(true);

            innerRoi->set_pat2volumematrix(innerSkin->get_pat2volumematrix());
            float color[4] = 
            {
                0.33f, 0.87f, 0.23f, 0.30f
            };
            innerRoi->set_color(color);
            innerRoi->set_name(mProtoBolus.bolusname());
            innerRoi->set_planningrole((int)RtDbDef::PLANNING_ROLE_BOLUS);
            innerRoi->set_averagedensity(bolusMassDensity);
            innerRoi->set_mindensity(bolusMassDensity);
            innerRoi->set_maxdensity(bolusMassDensity);
            innerRoi->set_isoverride(true);
            innerRoi->set_overridedenstiy(bolusMassDensity);
            innerRoi->set_density(bolusRerenceEDensity);
            innerRoi->set_isbolus(true);

            //get image3d
            tps::TpsImage3DEntity* pImage3D = nullptr;
            bool bRet = mDataRepository->GetImageDataManager()->GetImage3DData(imageUID, &pImage3D);
            if (!bRet || nullptr == pImage3D)
            {
                TPS_LOG_DEV_ERROR << "GetImage3DData failed or pImage3D is null.";
                DEL_PTR(pVOIBolus);
                free(contour.x);
                free(contour.y);
                return voiuid;
            }
            //get matrix
            Mcsf::Matrix4f mat4fVol2Pat;
            pImage3D->GetVolumeToPatientMatrix(mat4fVol2Pat);

            make_bolus(*pImage3D, pVOISkin, &contour, fPat2Beam, fBeam2Pat, bolusThickness, margin, pVOIBolus);

            TpsImageDataManager* imgDataMgr = mDataRepository->GetImageDataManager();
            RtCt2density* pCt2Density = imgDataMgr->GetCT2DensityTableByImageUid(imageUID);
            if (!SubVolumeHelperAPI::UpdateVOIStatisticalInformation(pCt2Density, *pImage3D, pVOIBolus))
            {
                TPS_LOG_DEV_ERROR << "Failed to update statistical information.";
                DEL_PTR(pVOIBolus);
                free(contour.x);
                free(contour.y);
                return voiuid;
            }

            RtImage3DHeader *header = nullptr;
            pImage3D->GetImage3DHeader(header);
            int sliceCount = header->m_iSliceCount;

            std::vector<bool> interpolates;
            for(int index = 0; index < sliceCount; ++index)
            {
                interpolates.push_back(false);
            }
            innerRoi->set_interpolate(interpolates);

            mDataRepository->GetVoiManager()->AddROI(pVOIBolus);

            voiuid = pVOIBolus->GetUid();
            if (!mRenderProxy->AddVOI(voiuid, imageUID))
            {
                TPS_LOG_DEV_ERROR << "Failed to add voi: " << voiuid <<" to render proxy.";
            }
        }

        free(contour.x);
        free(contour.y);

        return voiuid;
}

bool AddBolusCmd::CombineMlcShapes_i(
    const std::vector<RtMlcshape*>& vecMlcShape, RtMlcshape* shape, float miniLeafGap)
{
    if (vecMlcShape.empty())
    {
        TPS_LOG_DEV_ERROR << "The mlc shape list to be combined is empty.";
        return false;
    }
    if (shape == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The target mlc shape is null.";
        return false;
    }

    double jawPos[4] =
    {
        vecMlcShape[0]->get_xlowerjawpos(),
        vecMlcShape[0]->get_xupperjawpos(),
        vecMlcShape[0]->get_ylowerjawpos(),
        vecMlcShape[0]->get_yupperjawpos()};
        int direction = vecMlcShape[0]->get_mlcdirectiontype();
        auto leafPos = vecMlcShape[0]->get_leafpositions();

        RtMlcshape* tmpShape;
        std::vector<db_Point2d> tmpLeafPos;
        for (int i = 1; i < vecMlcShape.size(); ++i)
        {
            tmpShape = vecMlcShape[i];
            jawPos[0] = TPS_MIN(jawPos[0], tmpShape->get_xlowerjawpos());
            jawPos[1] = TPS_MAX(jawPos[1], tmpShape->get_xupperjawpos());
            jawPos[2] = TPS_MIN(jawPos[2], tmpShape->get_ylowerjawpos());
            jawPos[3] = TPS_MAX(jawPos[3], tmpShape->get_yupperjawpos());
            tmpLeafPos = tmpShape->get_leafpositions();
            if (leafPos.size() != tmpLeafPos.size())
            {
                TPS_LOG_DEV_ERROR << "The leaf position size is not coincident.";
                return false;
            }
            for (int j = 0; j < leafPos.size(); ++j)
            {
                if (abs(tmpLeafPos[j].y - tmpLeafPos[j].x - miniLeafGap) < 1e-4)
                {
                    continue;
                }
                leafPos[j].x = TPS_MIN(leafPos[j].x, tmpLeafPos[j].x);
                leafPos[j].y = TPS_MAX(leafPos[j].y, tmpLeafPos[j].y);
            }
        }

        shape->set_xlowerjawpos(jawPos[0]);
        shape->set_xupperjawpos(jawPos[1]);
        shape->set_ylowerjawpos(jawPos[2]);
        shape->set_yupperjawpos(jawPos[3]);
        shape->set_mlcdirectiontype(direction);
        shape->set_leafpositions(leafPos);
        return true;
}


int AddBolusCmd::Execute()
{
    std::string imageUID = mProtoBolus.seriesuid();
    RtBeam* beam = mDataRepository->GetBeamManager()->GetBeam(mProtoBolus.beam_uid());
    if (nullptr == beam)
    {
        TPS_LOG_DEV_ERROR << "Failed to get beam.";
        return TPS_ER_FAILURE;
    }

    if (!CreateBolus_i(imageUID, beam))
    {
        TPS_LOG_DEV_ERROR << "Failed to create bolus for beam[" << mProtoBolus.beam_uid() << "].";
        return TPS_ER_FAILURE;
    }

    mRenderProxy->Render();

    return TPS_ER_SUCCESS;
}


void AddBolusCmd::GetCreatedBolus(const ROIEntity*& pBlous)
{
    pBlous = mDataRepository->GetVoiManager()->GetROI(mProtoBolus.bolusuid());
}

TPS_END_NAMESPACE
