////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
/// 
///  \author  GONG Maoliang mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_get_density_overlapped_voi_list.cpp
/// 
///  \brief class GetDensityOverlappedVoiListCommand implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/02/14
////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_get_density_overlapped_voi_list.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_tablemanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_table.h"
#include "RtTpsFramework/tps_fw_signed_distance_field.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"
//#include "RtTpsDoseProxy/table_converter.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"

#include <list>

#include "tps_logger.h"
TPS_BEGIN_NAMESPACE

GetDensityOverlappedVoiListCommand::GetDensityOverlappedVoiListCommand(
    const std::string& seriesUid) : mSeriesUid(seriesUid)
{

}

GetDensityOverlappedVoiListCommand::~GetDensityOverlappedVoiListCommand()
{

}

int GetDensityOverlappedVoiListCommand::Execute()
{
    //TEST_PERFORMANCE_INIT;
    //TEST_PERFORMANCE_BEGIN;

    mOverlappedVoiList.clear();

    auto voiList = mDataRepository->GetVoiManager()->GetROIBySeries(mSeriesUid);
    TpsImage3DEntity* image = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(mSeriesUid, &image))
    {
        return TPS_ER_FAILURE;
    }
    MAT4 matP2V = image->GetVolumeToPatient().Inverse();

    RtVoi *voi1, *voi2;
    for (int i = 0; i < voiList.size() - 1; ++i)
    {
        voi1 = voiList[i]->GetInnerRoi();
        if (!voi1->get_isoverride())
        {
            continue;
        }

        for (int j = i + 1; j < voiList.size(); ++j)
        {
            voi2 = voiList[j]->GetInnerRoi();
            if (!voi2->get_isoverride() || 
                fabs(voi1->get_overridedenstiy() - voi2->get_overridedenstiy()) <= 1e-6)
            {
                continue;
            }

            if (IsOverlapped(matP2V, voiList[i], voiList[j]))
            {
                mOverlappedVoiList.push_back(voi1->get_uid());
                mOverlappedVoiList.push_back(voi2->get_uid());
            }
        }
    }

    //TEST_PERFORMANCE_END("Calc overlapped density-overrided voi.");
    return TPS_ER_SUCCESS;
}

std::vector<std::string> GetDensityOverlappedVoiListCommand::GetOverlappedVoiList() const
{
    return mOverlappedVoiList;
}

bool GetDensityOverlappedVoiListCommand::IsOverlapped(
    const MAT4& matP2V, const ROIEntity* source, const ROIEntity* target)
{
    auto srcSDF = source->GetSDF();
    auto tgtSDF = target->GetSDF();
    if (srcSDF == nullptr || tgtSDF == nullptr)
    {
        return false;
    }

    int srcOffset[3], srcSize[3];
    srcSDF->GetParasInVol(matP2V, srcOffset, srcSize);
    int tgtOffset[3], tgtSize[3];
    tgtSDF->GetParasInVol(matP2V, tgtOffset, tgtSize);

    int srcBox[6] = 
    {
        srcOffset[0], srcOffset[0] + srcSize[0] - 1, 
        srcOffset[1], srcOffset[1] + srcSize[1] - 1, 
        srcOffset[2], srcOffset[2] + srcSize[2] - 1
    };
    int trgtBox[6] = 
    {
        tgtOffset[0], tgtOffset[0] + tgtSize[0] - 1, 
        tgtOffset[1], tgtOffset[1] + tgtSize[1] - 1, 
        tgtOffset[2], tgtOffset[2] + tgtSize[2] - 1
    };
    if (trgtBox[0] > srcBox[1] || srcBox[0] > trgtBox[1] || 
        trgtBox[2] > srcBox[3] || srcBox[2] > trgtBox[3] || 
        trgtBox[4] > srcBox[5] || srcBox[4] > trgtBox[5] )
    {
        return false;
    }

    int overlapBox[6] = 
    {
        TPS_MAX(srcBox[0], trgtBox[0]), TPS_MIN(srcBox[1], trgtBox[1]), 
        TPS_MAX(srcBox[2], trgtBox[2]), TPS_MIN(srcBox[3], trgtBox[3]),
        TPS_MAX(srcBox[4], trgtBox[4]), TPS_MIN(srcBox[5], trgtBox[5])
    };
    for (int z = overlapBox[4]; z <= overlapBox[5]; ++z)
    {
        for (int y = overlapBox[2]; y <= overlapBox[3]; ++y)
        {
            for (int x = overlapBox[0]; x <= overlapBox[1]; ++x)
            {
                if (srcSDF->GetValueInVol(matP2V, x, y, z) >= -1e-6 && 
                    tgtSDF->GetValueInVol(matP2V, x, y, z) >= -1e-6)
                {
                    return true;
                }
            }
        }
    }
    return false;
}
TPS_END_NAMESPACE