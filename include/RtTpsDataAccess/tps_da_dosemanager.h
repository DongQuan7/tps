////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file tps_da_tpsdosegridmanager.h
/// 
///  \brief class TPS dose grid manager declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/09
////////////////////////////////////////////////////////////////

#ifndef TPS_DA_TPSDOSEGRIDMANAGER_H_
#define TPS_DA_TPSDOSEGRIDMANAGER_H_

#include "tps_da_defs.h"


#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsDataAccess/tps_da_datamanagerbase.h"

#define DEFAULT_DOSE_GRID_SPACING 3 //mm

namespace RTFWK
{
    class RTFWKDLDODoseGrid;
    class RTFWKDLDOImage3DHeader;
    //class RTFWKDLDODoseline;
};

namespace Mcsf{
    namespace MedViewer3D{
        class Vector3D;
        class MedViewer3DImageData;
        class Point2D;
    }
    class Vector2f;
    class Vector3f;
    class Point2f;
    class Point3f;
    class Matrix4f;
}

namespace boost
{
    class recursive_mutex;
    class mutex;
}

TPS_BEGIN_NAMESPACE 

enum DoseGridType {
    DOSE_GRID_PLAN = 0,
    DOSE_GRID_NORMGROUP,
    DOSE_GRID_BEAM,
};

class TpsImage3DEntity;
class RtImage3DHeader;
class RtDosegrid;
class RtDoseline;
class ROIEntity;

struct DosegridInfo {
    float spacingx;
    float spacingy;
    float spacingz;
    float startx;
    float starty;
    float startz;
};

class TPS_DA_DECLSPEC TpsDoseManager : public TpsDataManagerBase
{
public:
    TpsDoseManager();
    ~TpsDoseManager();

    //////////////////////////////////////////////////////////////////////////
    // \brief add a dose grid of specified data, this method will create a new 
    //          dose grid object and insert it into the dose grid map
    // \param[in] start  the original point of the dose grid in patient c.s.
    // \param[in] spacing  the spacing array of three axis in patient c.s.
    // \param[in] axis  the unit vector of three axis in patient c.s.
    // \param[in] dim  the dimension of the dose grid
    // \param[out] dosegrid created dosegrid object pointer
    // \return TPS_ER_SUCCESS when success, return TPS_ER_FAILURE when failed
    int CreateDoseGrid(const Mcsf::Point3f& start, 
        ::Mcsf::Vector3f* axis, float* dosegridSpacing, int* dosegridDim,
        RtDosegrid*& dosegrid);

    //////////////////////////////////////////////////////////////////////////
    // \brief get dose grid value
    //        
    // \param[in] uid
    // \param[in&out] plan's dosegrid
    // \return TPS_ER_SUCCESS when success, return TPS_ER_FAILURE when failed
    RtDosegrid* GetDoseGrid(const std::string& uid, DoseGridType type);

    bool UpdateDoseGridBuffer(RtDosegrid* target, RtDosegrid* source);

    int UpdateDoseGrid(RtDosegrid* grid, const Mcsf::Point3f& start, 
        ::Mcsf::Vector3f* axis, float* spacing, int* dim, bool bCreatBuffer = true);

    void SetPickCoordinate(double x, double y);
    bool GetPickCoordinate(double& x, double& y);
    void SetTargetCoordinate(double x, double y);
    bool GetTargetCoordinate(double& x, double& y);
    void MovingBoundary(double targetX, double targetY);
    void FinishMovingBoundary();
    int GetSelectionMode();
    bool IsHitTestSuccess();
    void SetHitTestSuccess(bool flag);
    void SetMovingPlanes(int* plane);
    bool GetMovingPlanes(int* plane);
    void SetDoseGridSpacing(float* spacing);
    void GetDoseGridSpacing(float* spacing);

    bool IsTwoDoseGridConsistent(const RtDosegrid* grid1, 
        const RtDosegrid* grid2);

    /// \brief Get a pair of dose quantity ratio and corresponding Color
    ///  use for color spectra display
    /// \param[in] pair<float, float[4]>
    /// \return void 
    void GetDoseQuantityRatioAndColor(std::vector<std::pair<float, float*>>&);

    void ClearDosegridForPlan(const std::string& planUID);
    void ClearDosegridForNormgroup(const std::string& ngUID);
    void ClearDosegridForNormgroup_i(const std::string& ngUID);

    void ClearDosegridForBeam_i(const std::string& beamUID);

    // return -1 if invalid
    // return 0  if dose returned is valid
    int GetPointDoseValueofPlan(const std::string& strPlanUID, 
        double xInPatient, double yInPatient, double zInPatient, 
        float& doseValue);
    int GetPointDoseValueofNormgroup(const std::string& strNgUID, 
        double xInPatient, double yInPatient, double zInPatient, 
        float& doseValue);
    int GetPointDoseValueofBeam(const std::string& strBeamUID, 
        double xInPatient, double yInPatient, double zInPatient, 
        float& doseValue);

    int GetMaxDosePointOfPlan(const std::string& strPlanUID,
        float& pXInPat, float& pYInPat, float& pZInPat, float& doseValue);
    int GetMaxDosePointOfNormgroup(const std::string& strNgUid, 
        float& pXInPat, float& pYInPat, float& pZInPat, float& doseValue);
    int GetMaxDosePointOfBeam(const std::string& strBeamUid, 
        float& pXInPat, float& pYInPat, float& pZInPat, float& doseValue);

    bool InitializeDosegrid(const DosegridInfo& dosegridInfo, 
        const TpsImage3DEntity* image3DEntity,
        const ROIEntity* skinVoi, 
        RtDosegrid& aDosegrid,
        bool bCreatBuffer = true);

    Mcsf::Matrix4f SetupGridToPatient(const RtImage3DHeader& imgHdr,
        const RtDosegrid& dosegrid);

    // DoseLine Relative
    bool AddDoseLineList(std::string planUID, const std::vector<RtDoseline*>& doseLineList);

    bool RemoveDoseLineList(std::string planUID);

    bool UpdateDoseLineList(std::string planUID, std::vector<RtDoseline*>& doseLineList);

    bool GetDoseLineList(std::string planUID, std::vector<RtDoseline*>& doseLineList);

    void ClearPatientData();

    static int GetPointDoseValue(RtDosegrid& dosegrid, 
        double xInPatient, double yInPatient, double zInPatient, 
        float& pDoseValue);
private:
    void Dispose();

    //////////////////////////////////////////////////////////////////////////
    // \brief setup the dose grid to patient transform matrix
    // \param[in] start  the original point of the dose grid in patient c.s.
    // \param[in] spacing  the spacing array of three axis 
    // \param[in] axis  the unit vector of three axis
    // \return Mcsf::Matrix4f  the result transform matrix
    Mcsf::Matrix4f SetupGridToPatient_i(const Mcsf::Point3f& start, 
        ::Mcsf::Vector3f* axis, float* spacing);

    int UpdateDoseGrid_i(RtDosegrid* grid, const Mcsf::Point3f& start, 
        ::Mcsf::Vector3f* axis, float* spacing, int* dim, bool bCreatBuffer = true);

    int GetMaxDose_i(RtDosegrid& dosegrid,
        float& pXInPat, float& pYInPat, float& pZInPat, float& doseValue);
private:
    double                                           mPickCoordinateX;
    double                                           mPickCoordinateY;
    double                                           mTargetX;
    double                                           mTargetY;
    int                                              mSelectionMode;
    int                                              mSelectedBoundary;
    bool                                             mIsHitTestSuccess;
    int                                              mPlanes[3];
    float                                            mSpacing[3];
    boost::recursive_mutex                           *mReMutex;
    boost::mutex                                     *mMutex;
    int                                              mDoseInfoType;

    std::vector<std::pair<float, float*>>            mQuantityRatioColor;

    std::map<std::string, std::vector<RtDoseline*>>  mDoselineCollection;
};

TPS_END_NAMESPACE

#endif
