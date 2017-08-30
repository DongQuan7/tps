////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_da_hot_cold_spots_manager.cpp
/// 
///  \brief class TpsHotColdSpotsManager 
/// 
///  \version 1.0
/// 
///  \date    Sep.24, 2014
////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RtTpsDataAccess/tps_da_hot_cold_spots_manager.h"

#include "RtTpsFramework/tps_fw_signed_distance_field.h"
#include "McsfMedViewer3DArithmetic/point3d.h"          // for Mcsf::MedViewer3D::Point3D

//TPS
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_dosegrid.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDataAccess/tps_da_dosemanager.h"         // for TpsDoseManager
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"    // for TpsImageManager
#include "tps_logger.h"


TPS_BEGIN_NAMESPACE
    TpsHotColdSpotsManager::TpsHotColdSpotsManager(void)
{
    m_spVecHotPoints_Axial = nullptr;
    m_spVecHotPoints_Coronal = nullptr;
    m_spVecHotPoints_Sagittal = nullptr;
    m_spVecColdPoints_Axial = nullptr;
    m_spVecColdPoints_Coronal = nullptr;
    m_spVecColdPoints_Sagittal = nullptr;
}

TpsHotColdSpotsManager::~TpsHotColdSpotsManager(void)
{
    Dispose();
}

void TpsHotColdSpotsManager::Dispose()
{
    m_spVecHotPoints_Axial = nullptr;
    m_spVecHotPoints_Coronal = nullptr;
    m_spVecHotPoints_Sagittal = nullptr;
    m_spVecColdPoints_Axial = nullptr;
    m_spVecColdPoints_Coronal = nullptr;
    m_spVecColdPoints_Sagittal = nullptr;
}

void TpsHotColdSpotsManager::ClearPatientData()
{
    Dispose();
}

int TpsHotColdSpotsManager::AddColdSpotsForAllVoi(const std::string& strPlanUID, std::vector<ROIEntity*> &vVoiList, float fColdLimit)
{
    m_spVecColdPoints_Axial.reset(new std::vector<Mcsf::MedViewer3D::Point3D>());
    m_spVecColdPoints_Coronal.reset(new std::vector<Mcsf::MedViewer3D::Point3D>());
    m_spVecColdPoints_Sagittal.reset(new std::vector<Mcsf::MedViewer3D::Point3D>());

    RtDosegrid* pDoseGrid = mDataRepository->GetDoseManager()->GetDoseGrid(strPlanUID, DOSE_GRID_PLAN);
    if (pDoseGrid == nullptr)
    {
        TPS_LOG_DEV_ERROR << "TpsHotColdSpotsManager::AddHotSpots(): Failed to get dose grid of plan: " << strPlanUID;
        return -1;
    }

    if (fColdLimit >= pDoseGrid->get_min())
    {
        for (std::vector<ROIEntity*>::iterator itVoi = vVoiList.begin(); itVoi != vVoiList.end(); ++itVoi)
        {
            ROIEntity* ROIEntity = *itVoi;
            AddPoints(strPlanUID, ROIEntity, fColdLimit, 0);
        }
    }

    return 0;
}

int TpsHotColdSpotsManager::AddColdSpots(const std::string& strPlanUID, ROIEntity* pVOI, float fColdLimit) 
{
    m_spVecColdPoints_Axial.reset(new std::vector<Mcsf::MedViewer3D::Point3D>());
    m_spVecColdPoints_Coronal.reset(new std::vector<Mcsf::MedViewer3D::Point3D>());
    m_spVecColdPoints_Sagittal.reset(new std::vector<Mcsf::MedViewer3D::Point3D>());

    RtDosegrid* pDoseGrid = mDataRepository->GetDoseManager()->GetDoseGrid(strPlanUID, DOSE_GRID_PLAN);
    if (pDoseGrid == nullptr){
        TPS_LOG_DEV_ERROR<<"TpsHotColdSpotsManager::AddHotSpots(): Failed to get dose grid of plan: "<<strPlanUID;
        return -1;
    }
    if (fColdLimit >= pDoseGrid->get_min())
    {
        AddPoints(strPlanUID, pVOI, fColdLimit, 0);
    }

    return 0;
}

int TpsHotColdSpotsManager::AddHotSpotsForAllVoi(const std::string& strPlanUID, std::vector<ROIEntity*> &vVoiList, float fHotLimit)
{
    m_spVecHotPoints_Axial.reset(new std::vector<Mcsf::MedViewer3D::Point3D>());
    m_spVecHotPoints_Coronal.reset(new std::vector<Mcsf::MedViewer3D::Point3D>());
    m_spVecHotPoints_Sagittal.reset(new std::vector<Mcsf::MedViewer3D::Point3D>());

    RtDosegrid* pDoseGrid = mDataRepository->GetDoseManager()->GetDoseGrid(strPlanUID, DOSE_GRID_PLAN);
    if (pDoseGrid == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"TpsHotColdSpotsManager::AddHotSpots(): Failed to get dose grid of plan: "<<strPlanUID;
        return -1;
    }
    if (fHotLimit <= pDoseGrid->get_max())
    {
        for (std::vector<ROIEntity*>::iterator itVoi = vVoiList.begin(); itVoi != vVoiList.end(); ++itVoi)
        {
            ROIEntity* ROIEntity = *itVoi;
            AddPoints(strPlanUID, ROIEntity, fHotLimit, 1);
        }
    }

    return 0;
}

int TpsHotColdSpotsManager::AddHotSpots(const std::string& strPlanUID, ROIEntity* pVOI, float fHotLimit)
{
    m_spVecHotPoints_Axial.reset(new std::vector<Mcsf::MedViewer3D::Point3D>());
    m_spVecHotPoints_Coronal.reset(new std::vector<Mcsf::MedViewer3D::Point3D>());
    m_spVecHotPoints_Sagittal.reset(new std::vector<Mcsf::MedViewer3D::Point3D>());

    RtDosegrid* pDoseGrid = mDataRepository->GetDoseManager()->GetDoseGrid(strPlanUID, DOSE_GRID_PLAN);
    if (pDoseGrid == nullptr)
    {
        TPS_LOG_DEV_ERROR << "TpsHotColdSpotsManager::AddHotSpots(): Failed to get dose grid of plan: " << strPlanUID;
        return -1;
    }
    if (fHotLimit <= pDoseGrid->get_max())
    {
        AddPoints(strPlanUID, pVOI, fHotLimit, 1);
    }
    return 0;
}

int TpsHotColdSpotsManager::AddPoints(const std::string& strPlanUID, ROIEntity* pVOI, float fLimit, int iHot)
{
    //TEST_PERFORMANCE_INIT;
    //TEST_PERFORMANCE_BEGIN;
    // Get plan dose grid
    RtDosegrid* pDoseGrid = mDataRepository->GetDoseManager()->GetDoseGrid(strPlanUID, DOSE_GRID_PLAN);
    if (pDoseGrid == nullptr){
        TPS_LOG_DEV_ERROR<<"TpsHotColdSpotsManager::AddHotSpots(): Failed to get dose grid of plan: "<<strPlanUID;
        return -1;
    }


    PT3 point;

    int i, j, k, index = 0;
    int xDim = pDoseGrid->get_xcount();
    int yDim = pDoseGrid->get_ycount();
    int zDim = pDoseGrid->get_zcount();

    const float* pDoseBuffer = pDoseGrid->get_dosegrid_buffer();
    Mcsf::Matrix4f matGrid2Pat = TpsArithmeticConverter::ConvertToMatrix4f(pDoseGrid->get_grid_to_pat_t());
    Mcsf::Matrix4f matPat2Grid = matGrid2Pat.Inverse();
    Mcsf::Point3f ptInPat;
    Mcsf::MedViewer3D::Point3D pt3dInPat;


    TpsImageDataManager* pImageDataManager = mDataRepository->GetImageDataManager();
    if (pImageDataManager == nullptr) {
        TPS_LOG_DEV_ERROR << "TpsHotColdSpotsManager::AddPoints(): Failed to find TpsImageDataManager.";
        return -1;
    }
    auto innerRoi = pVOI->GetInnerRoi();
    std::string strImageUID = innerRoi->get_seriesuid();
    TpsImage3DEntity* pImage3D = nullptr;
    pImageDataManager->GetImage3DData(strImageUID, &pImage3D);
    if (nullptr == pImage3D){
        TPS_LOG_DEV_ERROR<<"TpsHotColdSpotsManager::AddPoints(): Failed to get image for "<<strImageUID;
        return -1;
    }

    Mcsf::Matrix4f matVolume2Pat;
    pImage3D->GetVolumeToPatientMatrix(matVolume2Pat);
    MAT4 matP2V = TpsArithmeticConverter::ConvertToMatrix4x4(matVolume2Pat.Inverse());

    auto sdf = pVOI->GetSDF();
    int size[3];
    int offset[3];
    sdf->GetParasInVol(matP2V, offset, size);

    Mcsf::Matrix4f matVolume2Grid = matPat2Grid * matVolume2Pat;

    int nInterDoseData = xDim * yDim;
    if (xDim * zDim > nInterDoseData) nInterDoseData = xDim * zDim;
    if (yDim * zDim > nInterDoseData) nInterDoseData = yDim * zDim;
    float *pInterDoseData = (float*)malloc(nInterDoseData * sizeof(float));

    // axial
    for (int zloop = offset[2]; zloop < offset[2] + size[2]; zloop++)
    {
        Mcsf::Point3f ptStartInVolume(0, 0, zloop);
        Mcsf::Point3f ptStartInGrid = matVolume2Grid.Transform(ptStartInVolume);
        float fK = ptStartInGrid.GetZ();
        int nK = fK;
        float dk = fK - nK;

        k = nK;
        if (k < 0 || k >= zDim) continue;
        int k1 = nK + 1;
        if (k1 >= zDim)
        {
            k1 = zDim - 1;
        }
        for (j = 0; j < yDim; j++)
        {
            for (i = 0; i < xDim; i++)
            {
                pInterDoseData[j * xDim + i] = (1 - dk) * pDoseBuffer[k * xDim * yDim + j * xDim + i] +
                    dk * pDoseBuffer[k1 * xDim * yDim + j * xDim + i];
            }
        }
        index = 0;
        for (j = 0; j < yDim; j++)
        {
            for (i = 0; i < xDim; i++, index++)
            {
                if (iHot && pInterDoseData[index] > fLimit)
                {
                    ptInPat = matGrid2Pat.Transform(Mcsf::Point3f(i, j, fK));
                    point.x = ptInPat.GetX();
                    point.y = ptInPat.GetY();
                    point.z = ptInPat.GetZ();
                    pt3dInPat.x = ptInPat.GetX();
                    pt3dInPat.y = ptInPat.GetY();
                    pt3dInPat.z = ptInPat.GetZ(); 
                    if (pVOI->IsInROI(point))
                    {
                        m_spVecHotPoints_Axial->push_back(pt3dInPat);
                    }
                }
                else if (!iHot && pInterDoseData[index] < fLimit)
                {
                    ptInPat = matGrid2Pat.Transform(Mcsf::Point3f(i, j, fK));
                    point.x = ptInPat.GetX();
                    point.y = ptInPat.GetY();
                    point.z = ptInPat.GetZ();
                    pt3dInPat.x = ptInPat.GetX();
                    pt3dInPat.y = ptInPat.GetY();
                    pt3dInPat.z = ptInPat.GetZ(); 
                    if (pVOI->IsInROI(point))
                    {
                        m_spVecColdPoints_Axial->push_back(pt3dInPat);
                    }
                }
            }
        }
    }

    //this->m_nHotPointsInAxial = m_vecHotPoints.size();
    //this->m_nColdPointsInAxial = m_vecColdPoints.size();

    // sagittal
    for (int xloop = offset[0]; xloop < offset[0] + size[0]; xloop++)
    {
        Mcsf::Point3f ptStartInVolume(xloop, 0, 0);
        Mcsf::Point3f ptStartInGrid = matVolume2Grid.Transform(ptStartInVolume);
        float fI = ptStartInGrid.GetX();
        int nI = fI;
        float dI = fI - nI;

        i = nI;
        if (i < 0 || i >= xDim) continue;
        int i1 = nI + 1;
        if (i1 >= xDim)
        {
            i1 = xDim - 1;
        }
        for (k = 0; k < zDim; k++)
        {
            for (j = 0; j < yDim; j++)
            {
                pInterDoseData[k * yDim + j] = (1 - dI) * pDoseBuffer[k * xDim * yDim + j * xDim + i] +
                    dI * pDoseBuffer[k * xDim * yDim + j * xDim + i1];
            }
        }
        index = 0;
        for (k = 0; k < zDim; k++)
        {
            for (j = 0; j < yDim; j++, index++)
            {
                if (iHot && pInterDoseData[index] > fLimit)
                {
                    ptInPat = matGrid2Pat.Transform(Mcsf::Point3f(fI, j, k));
                    point.x = ptInPat.GetX();
                    point.y = ptInPat.GetY();
                    point.z = ptInPat.GetZ();
                    pt3dInPat.x = ptInPat.GetX();
                    pt3dInPat.y = ptInPat.GetY();
                    pt3dInPat.z = ptInPat.GetZ(); 

                    if (pVOI->IsInROI(point))
                    {
                        m_spVecHotPoints_Sagittal->push_back(pt3dInPat);
                    }
                }
                else if (!iHot && pInterDoseData[index] < fLimit)
                {
                    ptInPat = matGrid2Pat.Transform(Mcsf::Point3f(fI, j, k));
                    point.x = ptInPat.GetX();
                    point.y = ptInPat.GetY();
                    point.z = ptInPat.GetZ();
                    pt3dInPat.x = ptInPat.GetX();
                    pt3dInPat.y = ptInPat.GetY();
                    pt3dInPat.z = ptInPat.GetZ(); 

                    if (pVOI->IsInROI(point))
                    {
                        m_spVecColdPoints_Sagittal->push_back(pt3dInPat);
                    }
                }
            }
        }
    }

    //this->m_nHotPointsInSagittal = m_vecHotPoints.size();
    //this->m_nColdPointsInSagittal = m_vecColdPoints.size();
    // coronal
    for (int yloop = offset[1]; yloop < offset[1] + size[1]; yloop++)
    {
        Mcsf::Point3f ptStartInVolume(0, yloop, 0);
        Mcsf::Point3f ptStartInGrid = matVolume2Grid.Transform(ptStartInVolume);
        float fJ = ptStartInGrid.GetY();
        int nJ = fJ;
        float dJ = fJ - nJ;

        j = nJ;
        if (j < 0 || j >= yDim) continue;
        int j1 = nJ + 1;
        if (j1 >= yDim)
        {
            j1 = yDim - 1;
        }
        for (k = 0; k < zDim; k++)
        {
            for (i = 0; i < xDim; i++)
            {
                pInterDoseData[k * xDim + i] = (1 - dJ) * pDoseBuffer[k * xDim * yDim + j * xDim + i] +
                    dJ * pDoseBuffer[k * xDim * yDim + j1 * xDim + i];
            }
        }
        index = 0;
        for (k = 0; k < zDim; k++)
        {
            for (i = 0; i < xDim; i++, index++)
            {
                if (iHot && pInterDoseData[index] > fLimit)
                {
                    ptInPat = matGrid2Pat.Transform(Mcsf::Point3f(i, fJ, k));
                    point.x = ptInPat.GetX();
                    point.y = ptInPat.GetY();
                    point.z = ptInPat.GetZ();
                    pt3dInPat.x = ptInPat.GetX();
                    pt3dInPat.y = ptInPat.GetY();
                    pt3dInPat.z = ptInPat.GetZ(); 

                    if (pVOI->IsInROI(point))
                    {
                        m_spVecHotPoints_Coronal->push_back(pt3dInPat);
                    }
                }
                else if (!iHot && pInterDoseData[index] < fLimit)
                {
                    ptInPat = matGrid2Pat.Transform(Mcsf::Point3f(i, fJ, k));
                    point.x = ptInPat.GetX();
                    point.y = ptInPat.GetY();
                    point.z = ptInPat.GetZ();
                    pt3dInPat.x = ptInPat.GetX();
                    pt3dInPat.y = ptInPat.GetY();
                    pt3dInPat.z = ptInPat.GetZ(); 

                    if (pVOI->IsInROI(point))
                    {
                        m_spVecColdPoints_Coronal->push_back(pt3dInPat);
                    }
                }
            }
        }
    }

    //this->m_nHotPointsInCoronal = m_vecHotPoints.size();
    //this->m_nColdPointsInCoronal = m_vecColdPoints.size();
    free(pInterDoseData);
    //TEST_PERFORMANCE_END("TpsHotColdSpotsManager::AddPoints");
    return 0;
}

TPS_END_NAMESPACE
