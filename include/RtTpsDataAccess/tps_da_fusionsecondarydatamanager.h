////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_da_fusionsecondarydatamanager.h
/// 
///  \brief class FusionSecondaryDataManager declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/06/24
////////////////////////////////////////////////////////////////

#ifndef TPS_DA_FUSIONSECONDARYDATAMANAGER_H_
#define TPS_DA_FUSIONSECONDARYDATAMANAGER_H_

#include "tps_da_defs.h"
#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"
#include "Mcsf3DArithmetic/mcsf_3d_matrix4x4.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class FusionSecondaryDataManager
/// 
/// \brief *****
class TPS_DA_DECLSPEC TpsFusionSecondaryDataManager : public TpsImageDataManager
{
public: 
    enum EditingPlane{
        PLANE_SMALL_X = 0,
        PLANE_BIG_X,
        PLANE_SMALL_Y,
        PLANE_BIG_Y,
        PLANE_SMALL_Z,
        PLANE_BIG_Z,
    };
public:
    TpsFusionSecondaryDataManager();
    ~TpsFusionSecondaryDataManager();

    void SetFixedImageUID(const std::string& imageUID);
    void SetFloatImageUID(const std::string& imageUID);

    std::string GetFixedImageUID() const;
    std::string GetFloatImageUID() const;
    int ImagesStatus() const;

    void SetRigidResult(const Mcsf::Matrix4f& matrix);
    Mcsf::Matrix4f GetRigidResult() const;

    //alignment secondary data, image registration region
    inline bool IsRegionVisible(FUSION_ROLE role){ return mIsRegionVisible[role]; } 
    inline void SetRegionVisible(FUSION_ROLE role, bool isVisible) { 
        mIsRegionVisible[role] = isVisible; }
    inline bool IsRegionReady() const{ return mIsRegionReady; }
    inline void SetRegionReady(bool isReady) { mIsRegionReady = isReady; }
    inline void GetRegion(const int*& region) const { region = mLimitedRegion; }
    void SetRegion(const int* region);
    inline void UpdateRegion(EditingPlane plane, int value, FUSION_ROLE role) {
        if (role == FUSION_ROLE_FLOAT) plane = EditingPlane((int)plane + 6);
        mLimitedRegion[plane] = value;
    }
    inline char GetEditingPlaneFlag(FUSION_ROLE role) const {
        return mEditingPlaneFlag[role];
    }
    inline void SetEditingPlaneFlag(char flag, FUSION_ROLE role){
        mEditingPlaneFlag[role] = flag;
    }
    void ClearEditingRegionFlag() {
        memset(mEditingPlaneFlag, 0, 2 * sizeof(char));
    }
    void ClearRegion();

    //Registration Algorithm: Landmark, Alignment, Deformable
    inline REIGISTRATION_ALGORITHM CurrentRegistrationAlgorithm() const {
        return mCurrentRegistrationAlgorithm;
    }
    inline void SetCurrentRegistrationAlgorithm(REIGISTRATION_ALGORITHM algorithm) {
        mCurrentRegistrationAlgorithm = algorithm;
    }


private:
    std::string   mFixedImageUID;
    std::string   mFloatImageUID;

    //alignment secondary data, image registration region
    bool          mIsRegionVisible[2];
    bool          mIsRegionReady;
    int           mLimitedRegion[12];
    char          mEditingPlaneFlag[2];

    //fusion type
    //FUSION_TYPE   mCurrentFusionType;
    //registration algorithm
    REIGISTRATION_ALGORITHM mCurrentRegistrationAlgorithm;
    //the constructor and destructor

    Mcsf::Matrix4f mRigidResult;
};

TPS_END_NAMESPACE  // end namespace tps

#endif