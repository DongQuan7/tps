////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_ap_voi_helper.h
/// 
///  \brief class VOI helper declaration 
/// 
///  \version 1.0
/// 
///  \date    Dec.10,2013
////////////////////////////////////////////////////////////////

#ifndef TPS_AP_VOI_HELPER_H_
#define TPS_AP_VOI_HELPER_H_

#include "tps_ap_defs.h"

namespace McsfGeometry
{
    class SubVolume;
}
namespace Mcsf
{
    class Point3f;
}

TPS_BEGIN_NAMESPACE  // begin namespace TPS

class ROIEntity;
class TpsImage3DEntity;
class RtDosegrid;
class RtCt2density;
class SeriesTable;
class RtTable;

class TPS_AP_DECLSPEC TpsVoiHelper
{
public:
    static int GenerateVoiViaMirror(
        TpsImage3DEntity* image3D,
        McsfGeometry::SubVolume *srcSV, 
        int mirrorDirection,
        McsfGeometry::SubVolume* tgtSV);

    static int GenerateVoiViaExpansion(
        McsfGeometry::SubVolume *srcSV, 
        bool is2D,
        bool isExpansion,
        int delta[6],
        size_t volumeDim[3],
        McsfGeometry::SubVolume* tgtSV);

    static int GenerateVoiViaCombination(
        McsfGeometry::SubVolume* srcSV, 
        McsfGeometry::SubVolume* combineSV, 
        int combinationMode,
        McsfGeometry::SubVolume* tgtSV);

    static int GenerateVoiFromDose(
        TpsImage3DEntity* pImage3D, 
        RtDosegrid* pDoseGrid, 
        float fDoseMin, 
        float fDoseMax, 
        McsfGeometry::SubVolume* tgtSV);

    static float TrilinearInterpolate(float* buffer, int dim[3], const Mcsf::Point3f& point);

    static int ChopSubvolumeToFitImage(McsfGeometry::SubVolume *pSubvolume, TpsImage3DEntity* pImage3D);

    static int PopulateVoiContourGivenSubvolume(ROIEntity* pVoi, const TpsImage3DEntity* pImage3D, RtCt2density* ct2density);

    static bool IsSubvolumeEmpty(McsfGeometry::SubVolume *pVolume);

	static bool ConvertSeriesTableToVoi(const RtTable& rtTable, const TpsImage3DEntity& image3D, const SeriesTable& seriesTable, ROIEntity* & voiEntity);
};

TPS_END_NAMESPACE  // end namespace TPS

#endif  //TPS_AP_VOI_HELPER_H_
