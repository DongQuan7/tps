////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_da_hot_cold_spots_manager.h
/// 
///  \brief class HotColdSpotsManager 
/// 
///  \version 1.0
/// 
///  \date    Sep.24, 2014
////////////////////////////////////////////////////////////////

#ifndef TPS_DA_HOT_COLD_SPOTS_MANAGER_H_
#define TPS_DA_HOT_COLD_SPOTS_MANAGER_H_

#include "tps_da_defs.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"    //for Mcsf:Point3f
#include "McsfMedViewer3DArithmetic\point3d.h"      // for Mcsf::MedViewer3D::Point3D
#include "RtTpsDataAccess/tps_da_datamanagerbase.h"

TPS_BEGIN_NAMESPACE

class ROIEntity;
class TPS_DA_DECLSPEC TpsHotColdSpotsManager : public TpsDataManagerBase
{
public:
    struct HotColdSpots
    {
        ROIEntity* pVOI;
        float   fHotLimit;
        float   fColdLimit;
    };

public:
    TpsHotColdSpotsManager(void);
    virtual ~TpsHotColdSpotsManager(void);

    int AddColdSpots(const std::string& strPlanUID, ROIEntity* pVOI, float fColdLimit);

    int AddHotSpots(const std::string& strPlanUID, ROIEntity* pVOI, float fHotLimit);

    int AddHotSpotsForAllVoi(const std::string& strPlanUID, std::vector<ROIEntity*> &vVoiList, float fHotLimit);

    int AddColdSpotsForAllVoi(const std::string& strPlanUID, std::vector<ROIEntity*> &vVoiList, float fColdLimit);

    void ClearPatientData();

    std::shared_ptr<std::vector<Mcsf::MedViewer3D::Point3D>>    m_spVecHotPoints_Axial;
    std::shared_ptr<std::vector<Mcsf::MedViewer3D::Point3D>>    m_spVecHotPoints_Coronal;
    std::shared_ptr<std::vector<Mcsf::MedViewer3D::Point3D>>    m_spVecHotPoints_Sagittal;

    std::shared_ptr<std::vector<Mcsf::MedViewer3D::Point3D>>    m_spVecColdPoints_Axial;
    std::shared_ptr<std::vector<Mcsf::MedViewer3D::Point3D>>    m_spVecColdPoints_Coronal;
    std::shared_ptr<std::vector<Mcsf::MedViewer3D::Point3D>>    m_spVecColdPoints_Sagittal;

private:
    void Dispose();

    int AddPoints(const std::string& strPlanUID, ROIEntity* pVOI, float fLimit, int iHot);

private:
    std::vector<HotColdSpots> m_vecHotColdSpots;
};


TPS_END_NAMESPACE

#endif //TPS_DA_HOT_COLD_SPOTS_MANAGER_H_
