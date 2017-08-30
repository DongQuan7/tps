////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_mprfusiongraphicobject.h
/// 
///  \brief class TpsMprFusionGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/05/07
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_MPRFUSIONGRAPHICOBJECT_H_
#define TPS_RL_MPRFUSIONGRAPHICOBJECT_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"

namespace Mcsf{
    namespace MedViewer3D{
        class Texture3D;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class MPRGraphicObject;
typedef std::shared_ptr<Mcsf::MedViewer3D::Texture3D> Tex3DPtr;

enum RegAlg{
    Rigid, 
    NonRigid,
};

/// \class TpsMprFusionGraphicObject
/// 
/// \brief *****
class TPS_RL_DECLSPEC FusionGraphicObject : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    FusionGraphicObject();
    virtual ~FusionGraphicObject();

    inline void SetFixedImageUID(const std::string& imageUID){
        mFixedImageUID = imageUID;
    }
    inline std::string GetFixedImageUID() const{ return mFixedImageUID; }

    inline void SetFloatImageUID(const std::string& imageUID){
        mFloatImageUID = imageUID;
    }
    inline std::string GetFloatImageUID() const { return mFloatImageUID; }

    void SetFixedAlpha(float alpha);
    float GetFixedAlpha() const;

    void SetFloatAlpha(float alpha);
    float GetFloatAlpha() const;

    ///the registration matrix is from fixed patient c.s. to float patient c.s.
    void SetRegistrationMatrix(const Mcsf::MedViewer3D::Matrix4x4& matrix);
    Mcsf::MedViewer3D::Matrix4x4 GetRegistrationMatrix() const;

    void SetIsFusionEnabled(bool bIsFusion);
    bool IsFusionEnabled() const;

    inline void SetFusionType(FUSION_TYPE futionType) {
        mFusionType = futionType;
    }

    inline FUSION_TYPE GetFusionType() const {
        return mFusionType;
    }

    inline void SetCanEditFlag(bool flag) { mCanEdit = flag; }
    inline bool CanEdit() { return mCanEdit; }

    inline void SetRegAlg(RegAlg alg) { mRegAlg = alg; }
    inline RegAlg GetRegAlg() const { return mRegAlg; }

    inline void SetFusionVolumeTexture(Tex3DPtr tex) { mTexFusionVolume = tex;}
    inline Tex3DPtr GetFusionVolumeTexture(){ return mTexFusionVolume; }
private:
    std::string                         mFixedImageUID;
    std::string                         mFloatImageUID;
    Mcsf::MedViewer3D::Matrix4x4        mRegistrationMatrix;

    float                               mFixedAlpha;
    float                               mFloatAlpha;

    bool                                mIsFusionEnabled;
    FUSION_TYPE                         mFusionType;
    bool                                mCanEdit;

    RegAlg                              mRegAlg;
    Tex3DPtr                            mTexFusionVolume;//deformable volume texture
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(FusionGraphicObject);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
