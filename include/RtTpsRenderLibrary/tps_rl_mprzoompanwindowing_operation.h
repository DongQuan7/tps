//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  RONG Chengcheng chengcheng.rong@united-imaging.com
///
///  \file    tps_rl_mprzoompanwindowing_operation.h
///  \brief   MPR zoom and pan operation classes
///
///  \version 1.0
///  \date    2014-02-20
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_MPRZOOMPAN_OPERATION_H_
#define TPS_RL_MPRZOOMPAN_OPERATION_H_

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
class TPS_RL_DECLSPEC MPRZoomOperation
    : public OperationBase{
public:

    enum ZOOMPARAMODE {
        BY_FACTOR,
        BY_VECTOR
    };

    //the constructor and destructor
    // MODE: BY_FACTOR
    MPRZoomOperation(float fZoomFactor);
    // MODE: BY_VECTOR
    MPRZoomOperation(int width, int height, 
        float startX, float startY, float stopX, float stopY);

    ~MPRZoomOperation();

    /// \brief Set GraphicObject keys
    /// \param[in] sVolumeKey       volume
    /// \param[in] sMPRKey          MPR
    /// \param[in] sMPRCacheKey         Cache
    /// \return void 
    void SetGraphicObjectKeys(const std::string& sVolumeKey, 
                              const std::string& sMPRKey,
                              const std::string &mprRulerTextKey);

    virtual bool ModifyGraphicObject() const;
    virtual bool ModifyGraphicObject_i(
        const std::string& imageUID,
        FIRST_POSTFIX_COMPONENT sectionPage, 
        SECOND_POSTFIX_COMPONENT location = EMPTY_COMPONENT) const;

private:
    float   mZoomFactor;
    std::string     mVolumeKey;
    std::string     mMPRKey;
    std::string     mMprRulerTextKey;
    int             mWidth;
    int             mHeight;
    float           mStartX;
    float           mStartY;
    float           mStopX;
    float           mStopY;
    ZOOMPARAMODE    mZoomInputMode;

    std::string     mSection;
    std::string     mLocation;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(MPRZoomOperation);
};

/// \class MPRPanOperation
/// 
/// \brief this operation perform the pan operation on MPR
class TPS_RL_DECLSPEC MPRPanOperation
    : public OperationBase{
public:
    //the constructor and destructor
    /// \brief constructor
    /// \param[in] startx   start x position in display cache cs
    /// \param[in] starty   start y position in display cache cs
    /// \param[in] stopx    stop x position in display cache cs
    /// \param[in] stopy    stop y position in display cache cs
    MPRPanOperation(int width, int height, 
        float startx, float starty, float stopx, float stopy);
    ~MPRPanOperation();

    /// \brief Set GraphicObject keys
    /// \param[in] sVolumeKey       volume
    /// \param[in] sMPRKey          MPR
    /// \return void 
    void SetGraphicObjectKeys(const std::string& sVolumeKey, 
        const std::string& sMPRKey);

    virtual bool ModifyGraphicObject() const;

private:

private:
    int mWidth;
    int mHeight;
    float           mStartX;
    float           mStartY;
    float           mStopX;
    float           mStopY;
    std::string     mVolumeKey;
    std::string     mMPRKey;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(MPRPanOperation);
};

/// \class MPRWindowingOperation
/// 
/// \brief this operation perform the windowing operation on MPR
class TPS_RL_DECLSPEC MPRWindowingOperation : public OperationBase{
public:
    //the constructor and destructor
    /// \brief constructor
    /// \param[in] deltaWidth   delta width of windowing
    /// \param[in] deltaCenter  delta center of windowing
    MPRWindowingOperation(float deltaWidth, float deltaCenter);
    ~MPRWindowingOperation();

    /// \brief Set GraphicObject keys
    /// \param[in] sVolumeKey       volume
    /// \param[in] sMPRKey          MPR
    /// \return void 
    void SetGraphicObjectKeys(const std::string& sVolumeKey, 
        const std::string& sMPRKey,
        const std::vector<std::string>& vecLinkedMprKey);

    virtual bool ModifyGraphicObject() const;
private:
    mutable float   mDeltaWidth;
    mutable float   mDeltaCenter;
    mutable double  mWindowWidth;
    mutable double  mWindowCenter;
    std::string     mVolumeKey;
    std::string     mMPRKey;
    std::string     mCornerInfoKey;
    std::vector<std::string>    mVecLinkedMprKey;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(MPRWindowingOperation);
};

TPS_END_NAMESPACE

#endif
