//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  LI Song  song.li@united-imaging.com
//
/// \file    tps_rl_update_corner_info_operation.h
///
/// \brief   UpdateCornerInfoOperation
/// 
/// \version 1.0
/// 
/// \date    2014/11/24
/////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_UPDATE_CORNER_INFO_OPERATION_H_
#define TPS_RL_UPDATE_CORNER_INFO_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"
#include "tps_rl_graphicobjecttypedefines.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class GraphicObjVolume;
        class Point3D;
    }
}

TPS_BEGIN_NAMESPACE
class TpsImage3DEntity;
class MPRGraphicObject;

class TPS_RL_DECLSPEC UpdateCornerInfoOperation : public OperationBase
{
public:
    /// \brief constructor
    /// \param[in] 
    /// \param[in] 
    UpdateCornerInfoOperation(double x, double y, const std::string& strPlanUID, 
        MPR_CORNER_TYPE mprCornertype = CORNER_TYPE_ALL);
    virtual ~UpdateCornerInfoOperation();

    void SetSeriesUid(const std::string& seriesUid);
    void SetSuvType(int suvUnitType);
    void SetSectionType(FIRST_POSTFIX_COMPONENT section);

    virtual bool ModifyGraphicObject() const;
public:
    mutable double mValue;
    mutable bool   mIsValueValid;
    mutable double mDensity;
    mutable float  mDose;
    mutable double mPositionX;
    mutable double mPositionY;
    mutable double mPositionZ;
    mutable double mWindowWidth;
    mutable double mWindowCenter;
    mutable int    mSliceIndex;
    mutable int	   mStartSliceNumber;
    mutable int    mTotalSlice;
    mutable double mSliceThickness;
    mutable std::string mModality;
private:
    double          m_dX;
    double          m_dY;
    FIRST_POSTFIX_COMPONENT m_iSectionType;
    std::string     m_strPlanUID;
    int             m_iSUVUnitType; //FOR PT
    mutable std::string     m_SeriesUID;
    MPR_CORNER_TYPE m_mprCornerType;

    bool CheckIsValueValid_i(
        Mcsf::MedViewer3D::GraphicObjVolume* spVolumeGO, 
        std::string& modality, 
        int suvUnitType) const;

    std::unique_ptr<float[]> GetWindowCenterLevel_i(
        MPRGraphicObject* spMprGO, 
        Mcsf::MedViewer3D::GraphicObjVolume* spVolumeGO, 
        std::string& modality, 
        bool isValueValid, 
        int suvUnitType) const;

    Mcsf::MedViewer3D::Point3D GetMousePositionInPatient_i(
        MPRGraphicObject* spMprGO, 
        Mcsf::MedViewer3D::GraphicObjVolume* spVolumeGO, 
        double dx, 
        double dy) const;

    std::unique_ptr<double[]> GetValueDensityAndDose_i(
        MPRGraphicObject* spMprGO, 
        Mcsf::MedViewer3D::GraphicObjVolume* spVolumeGO, 
        double dx, 
        double dy,
        std::string& modality, 
        bool isValueValid, 
        int suvUnitType, 
        const std::string& seriesUid,
        const std::string& planUid) const;

    bool IsInTable_i(const Mcsf::MedViewer3D::Point3D& pointWorld) const;

    double GetDensity_i(
        const std::string& seriesUid,
        double ctValue, bool isInVolume,
        const Mcsf::MedViewer3D::Point3D& pointWorld, 
        const Mcsf::MedViewer3D::Point3D& pointPatient) const;

    void OverrideDensity_i(bool isExternal, double overrideDensity, 
        DensityOverrideType& overrideType, double& density) const;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(UpdateCornerInfoOperation);
};

TPS_END_NAMESPACE

#endif //TPS_RL_UPDATE_CORNER_INFO_OPERATION_H_
