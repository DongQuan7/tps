//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  SHI CHUNHUI chunhui.shi@united-imaging.com
///
///  \file    tps_rl_mprreset_operation.h
///  \brief   
///
///  \version 1.0
///  \date    May 12, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

namespace Mcsf{
    namespace MedViewer3D{
        class ClsGraphicObjectMPR;
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE
class TpsTextOverlayGraphicObject;

/// \class MPRZoomOperation
/// 
/// \brief this operation perform the zoom operation on MPR
class TPS_RL_DECLSPEC MPRResetOperation
    : public OperationBase{
public:

    MPRResetOperation(const std::string& imageUID);
    MPRResetOperation(const std::string& imageUID, int actionType);

    ~MPRResetOperation();

    /// \brief Set GraphicObject keys
    /// \param[in] sVolumeKey       volume
    /// \param[in] sMPRKey          MPR
    /// \param[in] sMPRCacheKey         Cache
    /// \return void 
    void SetGraphicObjectKeys(const std::string& sVolumeKey, 
                              const std::string& mprKey,
                              const std::string& crossHairKey,
                              const std::string &mprRulerTextKey);

    void SetTwoOtherKeys(const std::vector<std::string>& twoMprKeys);

    virtual bool ModifyGraphicObject() const;
    //virtual bool ModifyGraphicObject_i(
    //    const std::string& imageUID,
    //    FIRST_POSTFIX_COMPONENT sectionPage, 
    //    SECOND_POSTFIX_COMPONENT location = EMPTY_COMPONENT) const;

private:
    int             mActionType;
    std::string     mVolumeKey;
    std::string     mMprKey;
    std::string     mCrossHairKey;
    std::string     mMprRulerTextKey;
    std::string     mImageUID;
    
    std::vector<std::string> mTwoOtherMprKeys;

    std::string     mSection;
    std::string     mLocation;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(MPRResetOperation);
};

TPS_END_NAMESPACE
