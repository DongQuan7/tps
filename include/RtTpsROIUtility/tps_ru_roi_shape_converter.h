#ifndef TPS_RU_ROI_SHAPE_CONVERTER_H_
#define TPS_RU_ROI_SHAPE_CONVERTER_H_

#include "tps_ru_defs.h"
#include <vector>
#include "RtTpsFramework/tps_fw_arithmetic.h"

namespace McsfGeometry
{
    class SubVolume;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class ContourSet;
class Contour;
class SignedDistanceField;

class TPS_RU_DECLSPEC ROIShapeConverter
{
public:
    static bool ContourSetToSDF(
        const ContourSet& cs, 
        SignedDistanceField& sdf, 
        bool isOnlyDirtySlices = true);

    static bool ContourSetToSDF_CPU(
        const ContourSet& cs, 
        SignedDistanceField& sdf);

    static void SDFToContourSet(
        const SignedDistanceField& sdf, 
        const PLANE& plane, 
        ContourSet& cs);

    static void SliceSDFToContours(
        const SignedDistanceField& sdf, 
        const PLANE& rect, 
        std::vector<Contour>& contours);

    static void SliceSDFToContours_CPU(
        const SignedDistanceField& sdf, 
        const PLANE& rect, 
        std::vector<Contour>& contours);

    ///axis: 0->x, 1->y, 2->z
    static void SDFToContourSet(
        const SignedDistanceField& sdf, 
        int axis, ContourSet& cs);

    static void SDFToContourSet_CPU(
        const SignedDistanceField& sdf, 
        int axis, ContourSet& cs);

    static bool SubVolumeToSDF(
        McsfGeometry::SubVolume& sv, 
        const MAT4& matP2V,
        SignedDistanceField& sdf);

    static bool SDFToSubVolume(
        const SignedDistanceField& sdf, 
        const MAT4& matP2V,
        const size_t* volDim,
        McsfGeometry::SubVolume& sv);

    static void InterpolateROI(char* interFlags, SignedDistanceField& sdf, ContourSet& cs);
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_RU_ROI_SHAPE_CONVERTER_H_