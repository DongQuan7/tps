//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  LI Song  song.li@united-imaging.com
//
/// \file    tps_rl_update_corner_info_operation.cpp
///
/// \brief   UpdateCornerInfoOperation
/// 
/// \version 1.0
/// 
/// \date    2014/11/24
/////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_update_corner_info_operation.h"

//ZHENGHE
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"   // for GraphicObjVolume
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_suv_calculator.h"

//TPS
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_table.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"

#include "RtTpsDataAccess/tps_da_dosemanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_table.h"
#include "RtTpsDataAccess/tps_da_tablemanager.h"

#include "RtTpsRenderLibrary/tps_rl_series_table_cache_graphic_object.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_poi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
//#include "RtTpsRenderLibrary/tps_rl_mprdosegraphicobject.h"

#include "RtTpsRenderLibrary/tps_rl_mprdosegraphicobject.h"

TPS_BEGIN_NAMESPACE

    using namespace Mcsf::MedViewer3D;

UpdateCornerInfoOperation::UpdateCornerInfoOperation(
    double x, double y, const std::string& strPlanUID, MPR_CORNER_TYPE mprCornertype): 
m_dX(x), m_dY(y), m_strPlanUID(strPlanUID), m_mprCornerType(mprCornertype)
{

}

UpdateCornerInfoOperation::~UpdateCornerInfoOperation()
{

}

void UpdateCornerInfoOperation::SetSuvType(int suvUnitType)
{
    m_iSUVUnitType = suvUnitType;
}

void UpdateCornerInfoOperation::SetSeriesUid(const std::string& seriesUid)
{
    m_SeriesUID = seriesUid;
}

void UpdateCornerInfoOperation::SetSectionType(FIRST_POSTFIX_COMPONENT section)
{
    m_iSectionType = section;
}

bool UpdateCornerInfoOperation::ModifyGraphicObject() const 
{
    std::string key = m_SeriesUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME);
    auto go = mModelWarehouse->GetGraphicObject(key);
    auto spVolumeGO = std::dynamic_pointer_cast<GraphicObjVolume>(go);
    if (spVolumeGO == nullptr) 
    {
        TPS_LOG_DEV_ERROR << "[ERROR][UpdateCornerInfoOperation][ModifyGraphicObject]: "
            << "Failed to get volume GO.";
        return false;
    }

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, m_iSectionType);
    key = m_SeriesUID + "|" + GOTypeHelper::ToString(goType);
    go = mModelWarehouse->GetGraphicObject(key);
    auto spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (spMprGO == nullptr)
    {
        TPS_LOG_DEV_ERROR << "[ERROR][UpdateCornerInfoOperation][ModifyGraphicObject]: "
            <<"Failed to get MPR GO.";
        return false;
    }

    TpsImage3DEntity* image = nullptr;
    if (!mDataRepository->GetImageDataManager()->GetImage3DData(m_SeriesUID, &image) || image == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get image 3d entity.";
        return false;
    }

    // mouseposition
    if (m_mprCornerType & CORNER_TYPE_MOUSEPOSITION)
    {
        Mcsf::MedViewer3D::Point3D ptInPatient = GetMousePositionInPatient_i(
            spMprGO.get(), spVolumeGO.get(), m_dX, m_dY);
        mPositionX = ptInPatient.x;
        mPositionY = ptInPatient.y;
        mPositionZ = ptInPatient.z;
    }


    // modality
    mModality = spVolumeGO->GetModality();

    // isvaluevalid
    if (m_mprCornerType & CORNER_TYPE_VALUEVALID)
    {
        mIsValueValid = CheckIsValueValid_i(spVolumeGO.get(), mModality, m_iSUVUnitType);
    }

    // windowcenter windowWidth
    if (m_mprCornerType & CORNER_TYPE_WINDOWCENTERWIDTH)
    {
        std::unique_ptr<float[]> windowWidthCenter = GetWindowCenterLevel_i(
            spMprGO.get(), spVolumeGO.get(), mModality, mIsValueValid, m_iSUVUnitType);
        mWindowCenter = windowWidthCenter[0];
        mWindowWidth = windowWidthCenter[1];
    }

    // value density mousedose
    if (m_mprCornerType & CORNER_TYPE_VALUE_DENSITY_DOSE)
    {
        if (m_SeriesUID.empty())
        {
            TPS_LOG_DEV_ERROR << "[ERROR][UpdateCornerInfoOperation][ModifyGraphicObject]: "
                << "m_SeriesUID is empty or m_strPlanUID is empty";
            return false;
        }

        std::unique_ptr<double[]> valueDensityDose = GetValueDensityAndDose_i(
            spMprGO.get(), spVolumeGO.get(), m_dX, m_dY, 
            mModality, mIsValueValid, m_iSUVUnitType, m_SeriesUID, m_strPlanUID);
        mValue = valueDensityDose[0];
        mDensity = valueDensityDose[1];
        mDose = valueDensityDose[2];

        //切割的dose grid plane buffer测试代码
//         auto doseGOKey = m_strPlanUID + "|" + GOTypeHelper::ToString(
//             GOTypeHelper::ComposeAsGOType(DOSE, m_iSectionType));
//         go = mModelWarehouse->GetModelObject(doseGOKey);
//         auto doseGO = std::dynamic_pointer_cast<TpsMPRDoseGraphicObject>(go);
//         if (doseGO != nullptr)
//         {
//             float* buffer;
//             int width, height;
//             Mcsf::MedViewer3D::Matrix4x4 mat;
//             doseGO->GetDoseGrid(buffer, height, width, mat);
//             Mcsf::MedViewer3D::Point2D pt2(m_dX, m_dY);
//             Mcsf::MedViewer3D::Point3D pt3 = spMprGO->GetWorldCoordinate(pt2);
//             pt3 = mat.Transform(pt3);
// 
//             float x = pt3.x * width - 0.5;
//             float y = pt3.y * height - 0.5;
//             if (x < -0.5 || x > width - 0.5 || 
//                 y < -0.5 || y > height - 0.5)
//             {
//                 printf("\ndose on plane: --");
//             }
//             else
//             {
//                 int x0 = floor(x);
//                 int y0 = floor(y);
//                 if (x0 < 0) x0 = 0;
//                 if (y0 < 0) y0 = 0;
//                 int x1 = x0 + 1;
//                 int y1 = y0 + 1;
//                 if (x1 >= width) x1 = width - 1;
//                 if (y1 >= height) y1 = height - 1;
//                 float dx = x - x0;
//                 float dy = y - y0;
//                 float v00 = buffer[y0 * width + x0];
//                 float v10 = buffer[y0 * width + x1];
//                 float v11 = buffer[y1 * width + x1];
//                 float v01 = buffer[y1 * width + x0];
//                 float v = v00 * (1 - dx) * (1 - dy) + v10 * dx * (1 - dy) + 
//                     v11 * dx * dy + v01 * (1 - dx) * dy;
//                 printf("\ndose on plane: %f", v);
//             }
//         }
    }

    // sliceindex, totalslice, slicethickness
    if (m_mprCornerType & CORNER_TYPE_SLICEINDEX_THICKNESS)
    {
        mSliceThickness = spMprGO->GetThickness();
        Mcsf::MedViewer3D::Vector3D thickness3D(0.0, 0.0, mSliceThickness);
        mSliceThickness = spVolumeGO->GetWorld2Patient().Transform(thickness3D).z;
        TpsMprHelper::PlaneTypeInVol planeType;
        TpsMprHelper::GetSliceNumber(spVolumeGO, spMprGO, 
            mSliceIndex, mTotalSlice, planeType);
        mStartSliceNumber = planeType == TpsMprHelper::Z ? image->GetStartSliceNumber() : 1;
    }

    return true;
}

Mcsf::MedViewer3D::Point3D UpdateCornerInfoOperation::GetMousePositionInPatient_i(
    MPRGraphicObject* spMprGO, 
    Mcsf::MedViewer3D::GraphicObjVolume* spVolumeGO, 
    double dx, double dy) const
{
    Mcsf::MedViewer3D::Point2D ptInPlane(dx, dy);
    Mcsf::MedViewer3D::Point3D ptInWorld = spMprGO->GetWorldCoordinate(ptInPlane);
    Mcsf::MedViewer3D::Point3D ptInPatient = spVolumeGO->GetWorld2Patient().Transform(ptInWorld);
    return ptInPatient;
}

bool UpdateCornerInfoOperation::CheckIsValueValid_i(
    Mcsf::MedViewer3D::GraphicObjVolume* spVolumeGO,
    std::string& modality, 
    int suvUnitType) const
{
    bool bRet = true;

    if (modality != "PT")
    {
        return bRet;
    }

    auto dataHeader = mDataRepository->GetImageDataManager()->GetDataHeader(spVolumeGO->GetSeriesUID());
    if (dataHeader == nullptr) 
    {
        TPS_LOG_DEV_ERROR << "Failed to get data header.";
        return false;
    }
    GraphicObjectSUVCalculator suv_calc;
    suv_calc.GetSUVParamFromImageHeader(dataHeader);
    auto suvType = (Viewer3DSUVCalculateType)suvUnitType;
    suv_calc.SetSUVCalculateType(suvType);
    switch (suvType)
    {
    case CONCENTRATION:
        if (!suv_calc.IsConcentrationExist())
        {
            bRet = false;
        }
        break;
    case BODYWEIGHT:
        if (!suv_calc.IsSUVBodyWeightExist())
        {
            bRet = false;
        }
        break;
    case LEANBODYMASS:
        if (!suv_calc.IsSUVLeanBodyMassExist())
        {
            bRet = false;
        }
        break;
    case BODYSURFACEAREA:
        if (!suv_calc.IsSUVBodySurfaceAreaExist())
        {
            bRet = false;
        }
        break;
    default:
        bRet = false;
    }

    return bRet;
}

std::unique_ptr<float[]> UpdateCornerInfoOperation::GetWindowCenterLevel_i(
    MPRGraphicObject* spMprGO, 
    Mcsf::MedViewer3D::GraphicObjVolume* spVolumeGO, 
    std::string& modality, 
    bool isValueValid, 
    int suvUnitType) const
{
    std::unique_ptr<float[]> petSuv(new float[2]);
    double windowCenter = 0.0;
    double windowWidth = 0.0;
    spMprGO->GetWindowing(windowWidth, windowCenter);
    if (modality != "PT")
    {
        petSuv[0] = static_cast<float>(windowCenter);
        petSuv[1] = static_cast<float>(windowWidth);
        return petSuv;
    }

    if (isValueValid)
    {
        auto dataHeader = mDataRepository->GetImageDataManager()->GetDataHeader(spVolumeGO->GetSeriesUID());
        GraphicObjectSUVCalculator suv_calc;
        suv_calc.GetSUVParamFromImageHeader(dataHeader);
        auto suvType = (Viewer3DSUVCalculateType)suvUnitType;
        suv_calc.SetSUVCalculateType(suvType);
        std::unique_ptr<float[]> uptr_rawpixel(new float[2]);
        uptr_rawpixel[0] = static_cast<float>(windowCenter - 0.5 * windowWidth);//B
        uptr_rawpixel[1] = static_cast<float>(windowCenter + 0.5 * windowWidth);//T
        petSuv = suv_calc.GetPETSUV(uptr_rawpixel, 2);
    }

    return petSuv;
}

std::unique_ptr<double[]> UpdateCornerInfoOperation::GetValueDensityAndDose_i(
    MPRGraphicObject* spMprGO, 
    Mcsf::MedViewer3D::GraphicObjVolume* spVolumeGO, 
    double dx, 
    double dy,
    std::string& modality, 
    bool isValueValid, 
    int suvUnitType, 
    const std::string& seriesUid,
    const std::string& planUid) const
{
    std::unique_ptr<double[]> valueDensityDose(new double[3]);
    double value = 0.0;
    double density = 0.0;

    Mcsf::MedViewer3D::Point2D ptInPlane(dx, dy);
    Mcsf::MedViewer3D::Point3D ptInWorld = spMprGO->GetWorldCoordinate(ptInPlane);
    Mcsf::MedViewer3D::Point3D ptInPatient = spVolumeGO->GetWorld2Patient().Transform(ptInWorld);
    Mcsf::MedViewer3D::Point3D ptInVolume;
    Mcsf::MedViewer3D::Point3D ptInVolumeModel = spVolumeGO->GetModel2World().Inverse().Transform(ptInWorld);
    bool isInVolume = 
        ptInVolumeModel.x > -1e-6 && ptInVolumeModel.x < 1 + 1e-6 && 
        ptInVolumeModel.y > -1e-6 && ptInVolumeModel.y < 1 + 1e-6 && 
        ptInVolumeModel.z > -1e-6 && ptInVolumeModel.z < 1 + 1e-6;
    size_t volumeDim[3];
    spVolumeGO->GetDimension(volumeDim);
    ptInVolume.x = ptInVolumeModel.x * (volumeDim[0]);
    ptInVolume.y = ptInVolumeModel.y * (volumeDim[1]);
    ptInVolume.z = ptInVolumeModel.z * (volumeDim[2]);
    if (isInVolume)
    {
        spVolumeGO->GetPixelValue((int)ptInVolume.x, (int)ptInVolume.y, (int)ptInVolume.z, &value);
    }
    else
    {
        value = TPS_NEGATIVE_INF;
    }

    if (modality == "CT")
    {
        density = GetDensity_i(seriesUid, value, isInVolume, ptInWorld, ptInPatient);
    }
    else if (modality == "PT")
    {
        if (isValueValid)
        {
            auto dataHeader = mDataRepository->GetImageDataManager()->GetDataHeader(spVolumeGO->GetSeriesUID());
            GraphicObjectSUVCalculator suv_calc;
            suv_calc.GetSUVParamFromImageHeader(dataHeader);
            auto suvType = (Viewer3DSUVCalculateType)suvUnitType;
            suv_calc.SetSUVCalculateType(suvType);

            std::unique_ptr<float[]> uptr_rawpixel(new float[1]);
            float slope = spVolumeGO->GetSlope();
            float intercept = spVolumeGO->GetIntercept();
            float rawPixel = 0.f;
            if (slope == 0.f)
            {
                value = TPS_NEGATIVE_INF;
            }
            else
            {
                rawPixel = (value - intercept) / slope;
                uptr_rawpixel[0] = static_cast<float>(rawPixel);
                std::unique_ptr<float[]> petSuv;
                petSuv = suv_calc.GetPETSUV(uptr_rawpixel, 1);
                if (isInVolume) value = petSuv[0];
            }
        }
    }

    float dose = INVALID_DOSE_VALUE;
    if (isInVolume && !seriesUid.empty())
    {
        // 先判断是否击中POI 是则直接取POIDose
        bool isHitPoi = false;
        std::vector<RtPoi*> poiList = mDataRepository->GetPoiManager()->GetAllPoiInSeries(seriesUid);
        std::vector<RtPoi*>::iterator itPoi = poiList.begin();
        for (; itPoi != poiList.end(); ++itPoi)
        {
            // 偏差1mm范围内击中 POI
            RtPoi* poi = *itPoi;
            if (abs(ptInPatient.x - poi->get_coordinate3dx()) < 1
                && abs(ptInPatient.y - poi->get_coordinate3dy()) < 1
                && abs(ptInPatient.z - poi->get_coordinate3dz()) < 1)
            {
                dose = poi->get_plandose(planUid);
                isHitPoi = true;
                break;
            }
        }

        if (!isHitPoi)
        {
            mDataRepository->GetDoseManager()->GetPointDoseValueofPlan(
                planUid, ptInPatient.x, ptInPatient.y, ptInPatient.z, dose);
        }
    }

    valueDensityDose[0] = static_cast<double>(value);
    valueDensityDose[1] = static_cast<double>(density);
    valueDensityDose[2] = static_cast<double>(dose);
    return valueDensityDose;
}

double  UpdateCornerInfoOperation::GetDensity_i(
    const std::string& seriesUid,
    double ctValue, bool isInVolume,
    const Mcsf::MedViewer3D::Point3D& pointWorld, 
    const Mcsf::MedViewer3D::Point3D& pointPatient) const
{
    double density = TPS_NEGATIVE_INF;
    DensityOverrideType overrideType = NoOverride;

    //get voi override density
    ROIEntity* voi;
    RtVoi* innerRoi;
    auto voiMng = mDataRepository->GetVoiManager();
    auto voiList = voiMng->GetROIBySeries(seriesUid);
    for (int i = 0; i < voiList.size(); ++i)
    {
        voi = voiList[i];
        if (voi == nullptr) continue;
        innerRoi = voi->GetInnerRoi();
        if (innerRoi == nullptr) continue;

        if (!innerRoi->get_isoverride() || !voi->IsInROI(pointPatient))
        {
            continue;
        }
        OverrideDensity_i(voi->IsSkin(), innerRoi->get_overridedenstiy(), overrideType, density);
        if (overrideType == Overlap) break;
    }

    //table override density
    auto seriesTable = mDataRepository->GetTableManager()->GetSeriesTable(seriesUid);
    if (seriesTable != nullptr && IsInTable_i(pointWorld))
    {
        OverrideDensity_i(false, seriesTable->GetDensity(), overrideType, density);
    }

    if (overrideType == NoOverride && isInVolume)
    {
        density = mDataRepository->GetImageDataManager()->CT2DensityByImageUid(ctValue, seriesUid);
    }
    else if (overrideType == Overlap)
    {
        //-1 means overlap
        density = -1;
    }
    return density;
}

void UpdateCornerInfoOperation::OverrideDensity_i(bool isExternal, 
    double overrideDensity, DensityOverrideType& overrideType, double& density) const
{
    if(overrideType == Overlap) return;
    if(overrideType == NoOverride)
    {
        density = overrideDensity;
        overrideType = isExternal ? ExternalOverride : NormalOverride;
    }
    else if(overrideType == ExternalOverride)
    {
        density = overrideDensity;
        overrideType = OverlapExternal;
    }
    else if(!isExternal && fabs(density - overrideDensity) > 1e-6)
    {
        overrideType = Overlap;
    }
}

bool UpdateCornerInfoOperation::IsInTable_i(const Mcsf::MedViewer3D::Point3D& pointWorld) const
{
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(SERIES_TABLE_CACHE, m_iSectionType);
    auto key = m_SeriesUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetGraphicObject(key);
    auto seriesTableCacheGO = std::dynamic_pointer_cast<SeriesTableCacheGraphicObject>(go);
    if (seriesTableCacheGO == nullptr) return false;
    if (seriesTableCacheGO->IsTableEmpty()) return false;

    unsigned char* mask;
    int width, height;
    Mcsf::MedViewer3D::Matrix4x4 matWorld2NP;
    seriesTableCacheGO->GetCache(mask, width, height, matWorld2NP);
    auto pointNP = matWorld2NP.Transform(pointWorld);
    if (pointNP.x < 0 || pointNP.x > 1 || pointNP.y < 0 || pointNP.y > 1)
        return false;

    int x = TPS_CLAMP(TPS_ROUND(pointNP.x * width), 0, width - 1);
    int y = TPS_CLAMP(TPS_ROUND(pointNP.y * height), 0, height - 1);
    return mask[y * width + x] != 0;
}
TPS_END_NAMESPACE
