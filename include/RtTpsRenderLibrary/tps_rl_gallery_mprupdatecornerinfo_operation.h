//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  shaosen.huang shaosen.huang@united-imaging.com
//
/// \file    tps_rl_gallery_mprupdatecornerinfo_operation.h
///
/// \brief   
/// 
/// \version 1.0
/// 
/// \date    2015/04/10
/////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_UPDATECORNERTEXTINFO_OPERATION_H_
#define TPS_RL_GALLERY_UPDATECORNERTEXTINFO_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

namespace Mcsf{
    namespace MedViewer3D{
        class ClsGraphicObjectMPR;
        class GraphicObjVolume;
        class Point3D;
    }
}

TPS_BEGIN_NAMESPACE

class TpsTextOverlayGraphicObject;
class TPS_RL_DECLSPEC TpsGalleryMPRUpdateCornerTextInfoOperation : public OperationBase{
public:
    /// \brief constructor
    /// \param[in] 
    /// \param[in] 
    TpsGalleryMPRUpdateCornerTextInfoOperation(double x, double y/*double width, double height, double mousex, double mousey*/);
    ~TpsGalleryMPRUpdateCornerTextInfoOperation();

    /// \brief Set GraphicObject keys
    /// \param[in] sVolumeKey       volume
    /// \param[in] sMPRKey          MPR
    /// \return void 
    void SetGraphicObjectKeys(const std::string& sVolumeKey, 
        const std::string& sMPRKey,
        const std::string& mprCornerInfoKey,
        const std::string& sDummyGokey);
    void SetSuvType(int suvUnitType);

    virtual bool ModifyGraphicObject() const;

    void CalculatePointInVolumeAndWorld(Mcsf::MedViewer3D::Point3D* ptInPatient, 
                                        Mcsf::MedViewer3D::Point3D* ptInWorld, 
                                        Mcsf::MedViewer3D::Point3D* ptInVolume) const;

    void SetSeriesUid(const std::string& seriesUid) {mSeriesUID = seriesUid;}

    //void GetDoseValue(const Mcsf::MedViewer3D::Point3D& ptInPatient, float *dose) const;
    void GetCTAndDensityValue(double* ct, double* density) const;
    void GetPositionInPatientCS(float *position) const; 

    bool ConvertCTAndDensityValueToImageText(double ct, double density, std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo) const;
    //bool ConvertDoseValueToImageText(float dose, std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo) const;
    bool ConvertPtInPatientToImageText(Mcsf::MedViewer3D::Point3D* ptInPatient, std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo) const;

public:
    mutable double mValue;
    mutable bool   mIsValueValid;
    mutable double mDensity;
    mutable double mPositionX;
    mutable double mPositionY;
    mutable double mPositionZ;
    mutable double mWindowWidth;
    mutable double mWindowCenter;
    mutable int    mSliceIndex;
	mutable int	   mStartSliceNumber;
    mutable int    mTotalSliceNum;
    mutable double mSliceThickness;
    mutable std::string mModality;
    mutable double mZoomFactor;
    mutable double mRotate;

private:
    //double          mWidth, mHeight;
    //double          mMouseX, mMouseY;
    double          m_dX;
    double          m_dY;
    std::string     m_strVolumeKey;
    std::string     m_strMPRKey;
    std::string     m_strCornerInfoKey;
    std::string     m_strDummyGoKey;
    int             m_iSUVUnitType; //FOR PT
    std::string     mSeriesUID;

    int GetVoiOverrideDensity_i(const std::string& seriesUid,
        float fXInPat, float fYInPat, float fZInPat, float* fDensity) const;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryMPRUpdateCornerTextInfoOperation);
};

TPS_END_NAMESPACE

#endif
