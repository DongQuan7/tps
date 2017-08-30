//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file poi_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2015/08/11
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "poi_converter.h"

//#include "alg/RtAlgDataLayer/rt_alg_dl_object_base.h"

TPS_BEGIN_NAMESPACE

bool PoiConverter::ConvertToALG(const RtPoi& tpsPoi, std::shared_ptr<RTFWK::RTFWKDLDOPOI>* algPoi)
{
    algPoi->reset(new RTFWK::RTFWKDLDOPOI(false));
    (*algPoi)->SetUID(tpsPoi.get_uid());
    Mcsf::Point3f* poiCoordinate = new Mcsf::Point3f(
        tpsPoi.get_coordinate3dx(), tpsPoi.get_coordinate3dy(), tpsPoi.get_coordinate3dz());
    (*algPoi)->SetCoordinate3D(*poiCoordinate);
    (*algPoi)->SetType((RTFWK::POI_TYPE)(tpsPoi.get_type()));
    (*algPoi)->SetLockType((RTFWK::LOCKTYPE)tpsPoi.get_locktype());
    (*algPoi)->SetName(tpsPoi.get_name());
    (*algPoi)->SetIsVisible(tpsPoi.get_isvisible());
    return true;
}

bool PoiConverter::ConvertToTPS(RTFWK::RTFWKDLDOPOI& algPoi,
    std::map<std::string, std::map<std::string, float> > mBeamDoseContributionPerMuToPOI,
    RtPoi* tpsPoi)
{
    tpsPoi->set_uid(algPoi.GetUID());
    std::map<std::string, std::map<std::string, float> >::iterator itBeamPoi = mBeamDoseContributionPerMuToPOI.begin();
    for (; itBeamPoi != mBeamDoseContributionPerMuToPOI.end(); ++itBeamPoi)
    {
        std::map<std::string, float> mPoiDosePerMU = itBeamPoi->second;
        std::map<std::string, float>::iterator itPoiDose = mPoiDosePerMU.begin();
        for (; itPoiDose != mPoiDosePerMU.end(); ++ itPoiDose)
        {
            if (itPoiDose->first == tpsPoi->get_uid())
            {
                tpsPoi->set_poidose(itBeamPoi->first, itPoiDose->second);
            }
        }
    }
    return true;
}

bool PoiConverter::CreateALGPoiList(
    std::vector<RtPoi*>& tpsPoiList,
    std::vector<std::shared_ptr<RTFWK::RTFWKDLDOPOI>>* algPoiList)
{
    std::vector<RtPoi*>::iterator itTpsPoi = tpsPoiList.begin();
    for (; itTpsPoi != tpsPoiList.end(); ++itTpsPoi)
    {
        RtPoi* tpsPoi = *itTpsPoi;
        std::shared_ptr<RTFWK::RTFWKDLDOPOI> algPoi;
        PoiConverter::ConvertToALG(*tpsPoi, &algPoi);
        algPoiList->push_back(algPoi);
    }
    return true;
}

bool PoiConverter::ConvertToTPSPoiList(std::vector<RTFWK::RTFWKDLDOPOI*>& algPoiList,
    std::map<std::string,std::map<std::string, float> > mBeamDoseContributionPerMuToPOI,
    std::vector<RtPoi*>* tpsPoiList)
{
    std::vector<RtPoi*>::iterator itTpsPoi = tpsPoiList->begin();
    for (; itTpsPoi != tpsPoiList->end(); ++itTpsPoi)
    {
        RtPoi* tpsPoi = *itTpsPoi;
        std::vector<RTFWK::RTFWKDLDOPOI*>::iterator itAlgPoi = algPoiList.begin();
        for (; itAlgPoi != algPoiList.end(); ++itAlgPoi)
        {
            RTFWK::RTFWKDLDOPOI* algPoi = *itAlgPoi;
            if (algPoi->GetUID() == tpsPoi->get_uid())
            {
                PoiConverter::ConvertToTPS(*algPoi, mBeamDoseContributionPerMuToPOI, tpsPoi);
                break;
            }
        }
    }
    return true;
}

TPS_END_NAMESPACE