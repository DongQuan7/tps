//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file dvh_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2015/08/20
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvh_converter.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"


TPS_BEGIN_NAMESPACE
//bool DVHConverter::ConvertToALG(const RtDvh& tpsDVH, std::shared_ptr<RTFWK::RTFWKDLDODVH>* algDVH)
//{
//    algDVH->reset(new RTFWK::RTFWKDLDODVH());
//    (*algDVH)->mBinWidth = tpsDVH.get_binwidth();
//    (*algDVH)->mVoiUID = tpsDVH.get_uid();
//    return true;
//}

bool DVHConverter::ConvertToTPS(const RTFWK::RTFWKDLDODVH& algDVH, ITpsDataRepository *dataRepository, RtDvh* tpsDVH)
{
    tpsDVH->set_uid(algDVH.mVoiUID);
    tpsDVH->set_deviation(algDVH.m_fDeviation);
    tpsDVH->set_ingrid(algDVH.m_fInGrid);
    tpsDVH->set_maxdose(algDVH.mMaxDose);
    tpsDVH->set_meandose(algDVH.mMeanDose);
    tpsDVH->set_mindose(algDVH.mMinDose);
    tpsDVH->set_standarddeviation(algDVH.m_fStandardDeviation);
    tpsDVH->set_binwidth(algDVH.mBinWidth);

    float voiVolume = algDVH.m_fVolume;
    std::map<int, float> cumulativeDVHData = algDVH.mCumulativeDVHData;
    std::map<int, float> updatedCumulativeDVHData;
    std::map<int, float>::iterator itCumData = cumulativeDVHData.begin();
    for (; itCumData != cumulativeDVHData.end(); ++itCumData)
    {
        updatedCumulativeDVHData.insert(
            std::make_pair(itCumData->first, itCumData->second / voiVolume));
    }
    tpsDVH->set_cumulative_dvhdata(updatedCumulativeDVHData);

    std::map<int, float> differentialDVHData = algDVH.mDifferentialDVHData;
    std::map<int, float> updatedDifferentialDVHData;
    std::map<int, float>::iterator itDiffData = differentialDVHData.begin();
    for (; itDiffData != differentialDVHData.end(); ++itDiffData)
    {
        updatedDifferentialDVHData.insert(
            std::make_pair(itDiffData->first, itDiffData->second / voiVolume));
    }
    tpsDVH->set_differential_dvhdata(updatedDifferentialDVHData);

    std::string voiUid = tpsDVH->get_uid();
    ROIEntity* roiEntity = nullptr;
    if (dataRepository != nullptr)
    {
        roiEntity = dataRepository->GetVoiManager()->GetROI(voiUid);
        if (roiEntity != nullptr)
        {
            roiEntity->GetInnerRoi()->set_standarddeviation(tpsDVH->get_standarddeviation());
        }
    }

    return true;
}

//bool DVHConverter::CreateALGDVHList(
//    std::vector<std::shared_ptr<RTFWK::RTFWKDLDODVH>>* algDVHList,
//    std::vector<RtDvh*>& tpsDVHList)
//{
//    (void)algDVHList, (void)tpsDVHList;
//    return true;
//}

bool DVHConverter::ConvertToTPSDvhList(
    std::vector<RTFWK::RTFWKDLDODVH*>& algDVHList,
    ITpsDataRepository *dataRepository,
    std::vector<RtDvh*>* tpsDVHList)
{
    std::vector<RtDvh*>::iterator itTpsDvh = tpsDVHList->begin();
    for (; itTpsDvh != tpsDVHList->end(); ++itTpsDvh)
    {
        RtDvh* tpsDvh = *itTpsDvh;
        std::vector<RTFWK::RTFWKDLDODVH*>::iterator itAlgDvh = algDVHList.begin();
        for (; itAlgDvh != algDVHList.end(); ++itAlgDvh)
        {
            RTFWK::RTFWKDLDODVH* algDvh = *itAlgDvh;
            if (algDvh->mVoiUID == tpsDvh->get_uid())
            {
                if (algDvh->mCumulativeDVHData.size() > 10000)
                {
                    //Wrong dvh data 
                    printf("\nWrong dvh[voi %s] data size %d \n",
                        algDvh->mVoiUID.c_str(), algDvh->mCumulativeDVHData.size());

                    continue;
                } 
                DVHConverter::ConvertToTPS(*algDvh, dataRepository, tpsDvh);
                break;
            }
        }
    }
    return true;
}
TPS_END_NAMESPACE