//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  ZHOU Qiangqiang qiangqiang.zhou@united-imaging.com
///
///  \file    tps_rl_mprgraphicobject.h
///  \brief   MPR GraphicObject
///
///  \version 1.0
///  \date    Nov. 21, 2013
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_RL_MPRGRAPHICOBJECT_H_
#define TPS_RL_MPRGRAPHICOBJECT_H_

#include "tps_rl_defs.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"

namespace Viewer3D = Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC MPRGraphicObject : 
    public Viewer3D::ClsGraphicObjectMPR {
public:
    MPRGraphicObject(void);
    virtual ~MPRGraphicObject(void);

    virtual bool Initialize();
    virtual bool Finalize();

    /// \brief SetMPRMode
    /// \param[in] bMPRMode  
    ///             TRUE:in fusion mode
    ///             FALSE: nomal MPR
    /// \return void 
    void SetMPRMode(bool bMPRMode);

    /// \brief GetMPRMode
    /// \return bool
    ///         TRUE:in fusion mode
    ///         FALSE: nomal MPR
    bool GetMPRMode();

    /// \brief Set fixed volume
    /// \param[in] sVolumeUID
    /// \param[in] fFusionFactor
    /// \return void 
    void SetFixedVolumeUID(const std::string &sVolumeUID, const float fFusionFactor);

    /// \brief Set display(float) volume
    /// \param[in] sVolumeUID
    /// \param[in] matRegistrationMatrix
    /// \param[in] fFusionFactor
    /// \return void 
    void SetFloatVolume(const std::string &sVolumeUID, const Viewer3D::Matrix4x4 &matRegistrationMatrix, const float fFusionFactor);

    /// \brief Set window width and window center
    /// \param[in] sVolumeUID
    /// \param[in] dWindowWidth
    /// \param[in] dWindowCenter
    /// \return void 
    void SetWindowLevel( 
        const std::string& sVolumeUID, const double dWindowWidth, const double dWindowCenter );

    /// \brief Get window width and window center
    /// \param[in] sVolumeUID
    /// \param[out] dWindowWidth
    /// \param[out] dWindowCenter
    /// \return void 
    void GetWindowLevel( 
        const std::string& sVolumeUID, double& dWindowWidth, double& dWindowCenter ) const;

    /// \brief get fusion factor of fusion VR 
    /// \param[in] sVolumeUID     UID of  volume 
    /// \return fusion factor of volume(UID)
    float GetFusionFactor(const std::string& sVolumeUID) const;

    void SetSectionType(FIRST_POSTFIX_COMPONENT section_Type);
    
    FIRST_POSTFIX_COMPONENT GetSectionType() const;

    void SetLocation(SECOND_POSTFIX_COMPONENT location);
    SECOND_POSTFIX_COMPONENT GetLocation() const;

    Viewer3D::Matrix4x4 GetWorldToViewMatrix();
    void SetRulerIsVisible(bool isVisible);
    bool RulerVisible() const;

private:
    void GetDisplaySize(int* /*pWidht*/, int * /*pHeight*/) const { return; }
private:
    bool                        mMPRMode;
    FIRST_POSTFIX_COMPONENT     mSectionType;
    SECOND_POSTFIX_COMPONENT    mLocation;
    bool                        mIsRulerVisible;
};

TPS_END_NAMESPACE

#endif  //TPS_RL_MPRGRAPHICOBJECT_H_
