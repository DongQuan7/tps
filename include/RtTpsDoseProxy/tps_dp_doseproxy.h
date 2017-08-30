//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file tps_dp_dosecalculation.h
/// 
///  \version 1.0
/// 
///  \date    2013/12/14
//////////////////////////////////////////////////////////////////////////

#ifndef	TPS_DP_DOSECALCULATION_H_
#define TPS_DP_DOSECALCULATION_H_

#include "tps_dp_defs.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include <vector>
#include <map>

namespace RTFWK
{
    class RTFWKDLDOVOI;
    class RTFWKDLDOPOI;
    class RTFWKDLDONormgroup;
    class RTFWKDLDODVH;
    class RTFWKDLDOBeam;
}

namespace Mcsf
{
    class Matrix4f;
}

class DoseCalculationManager;
namespace RTALG
{
    class DoseCalculationManager;
}

TPS_BEGIN_NAMESPACE

class TpsImage3DEntity;
class ITpsDataRepository;
class RtVoi;
class RtBeam;
class RtDosegrid;
class FluenceMapEntity;
class RtPlan;
class RtNormgroup;
class RtDvh;
class ROIEntity;
class RtPoi;

typedef std::map<std::string, std::map<std::string, float>> BeamDose2PoiMap;

class TPS_DP_DECLSPEC TpsDoseProxy
{
public:
    TpsDoseProxy();
    ~TpsDoseProxy();
    bool Initialize();
    void SetDataRepository(ITpsDataRepository* repos) {mDataRepository = repos;}
    bool CalculateFluenceMap(const RtBeam* beam, FluenceMapEntity* fluenceMap);
    bool UpdateFluenceMap(const RtBeam* beam);
    int CalculateDoseForNormgroup(const std::string& ngUid);
    bool CalculateDVH(const std::string& planUid);

    bool CalculateNormGroupDVH(
        const std::string& normGroupUid,
        const std::vector<std::string>& voiUidList, 
        std::vector<RtDvh*>& tpsDVHList);

    bool ComputeTcpNTcpByNormalGroup(
        const std::string& normalGroupUid, 
        const std::vector<std::string>& voiUidList, 
        std::vector<ROIEntity*>& vVois);

    bool ComposeDoseForPlan(const std::string& planUid);
    bool ComposeDoseForNormgroup(const std::string& ngUid);
    bool CheckValidDosegrid(RtDosegrid& dosegrid) const;
    bool ExportSliceDose(RtDosegrid* doseGrid, const std::string& filePath, void* pDoseByte, int size);

    /// \param RTFWKDLDOBeam* beam : input beam, should be STATICIMRT with only one segment. otherwise return error.
    /// \param std::vector<RTFWKDLDOPOI*> &pointList : input/output, list of points to calculate, 
    ///  one of them should be normalization point type.
    ///  point coordinate(input): x and y in IEC collimator system, z as depth. all in mm.
    ///  point dose @current beam(output): m_fDose, in cGy.
    /// \param float prescribeDose : input, prescribe dose in cGy
    /// \param float SSD : input, use same SSD for all points, in mm.
    bool EasyPlanCalculateDose(RtBeam *beam, std::vector<RtPoi*>& vPOIList,
        float prescribeDose, int fractionCount, std::map<std::string, float>& poiUID2TotalDose);

    bool MergeDose(RtPlan *primaryPlan,
        RtPlan *secondPlan, int mergeType,
        const Mcsf::Matrix4f& secondaryToPrimary, RtPlan *resultPlan);

    int RescaleDosegrid(std::vector<RtDosegrid*>& vSourceGrids,
        std::vector<float>& vRescaleFactor,
        RtDosegrid* pResultGrid);

    int CalculateBeamMU(
        const TpsImage3DEntity& image3D, 
        const RtNormgroup* normalgroup, 
        ROIEntity* voi, 
        std::vector<RtBeam*>* vBeamList);

    int CalculateBeamMU(
        const RtNormgroup* normalgroup, 
        const BeamDose2PoiMap& beamDose2PoiMap, 
        std::vector<RtBeam*>* vBeamList);

    int ComputeBeamWeight(
        RtNormgroup* normalgroup, 
        const BeamDose2PoiMap& beamDose2PoiMap, 
        std::vector<RtBeam*>* vBeamList);

    int ComputeBeamWeight(
        const TpsImage3DEntity& image3D, 
        RtNormgroup* normalgroup, 
        ROIEntity* pVoiEntity, 
        std::vector<RtBeam*>* vBeamList);




private:
     RTALG::DoseCalculationManager      *mDoseCalculationManager;
     ITpsDataRepository                 *mDataRepository;

private:
    void DumpBeamInfo_i(RtBeam* pBeam);
    bool ComposeDoseGrids_i(const std::vector<RtDosegrid*>& sources,
        const std::vector<float>& rescaleSlopes, RtDosegrid* target);
    bool RetrieveVoiList_i(const TpsImage3DEntity& image3D, std::vector<RtVoi*>* voiList);
    int CheckSeriesSkin_i(const TpsImage3DEntity& image3D);

    bool CheckInputParameters(const std::vector<RtBeam*> vBeams, bool isLockMU);
    void ReleaseDVHListMemory_i(std::vector<RTFWK::RTFWKDLDODVH*>* dvhList);
    bool CanCompose_i(RtDosegrid* target, RtDosegrid* source);
    bool ComposePoiDoseForPlan_i(const std::string& planUID);
    bool ComposePoiDoseForNG_i(const std::string& ngUID);
    std::string GetSiteTemplateUidByNormalGroupUid_i(const std::string& ngUid);
    bool GetPixelVolumeByNormalgroupUid_i(const std::string& normalGroupUid, float& pixelVolume);
};

TPS_END_NAMESPACE

#endif
