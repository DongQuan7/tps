//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rl_gallery_mprzoompanwindowing_operation.h
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_MPRZOOMPAN_OPERATION_H_
#define TPS_RL_GALLERY_MPRZOOMPAN_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"


namespace Mcsf{
    namespace MedViewer3D{
        class ClsGraphicObjectMPR;
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE

    /// \class TpsGalleryMPRZoomOperation
    /// 
    /// \brief this operation perform the zoom operation on MPR
class TPS_RL_DECLSPEC TpsGalleryMPRZoomOperation
    : public OperationBase{
public:

    enum ZOOMPARAMODE {
        BY_FACTOR,
        BY_VECTOR
    };

    //the constructor and destructor
    // MODE: BY_FACTOR
    TpsGalleryMPRZoomOperation(float fZoomFactor);
    // MODE: BY_VECTOR
    TpsGalleryMPRZoomOperation(int width, int height, 
        float startX, float startY, float stopX, float stopY);

    ~TpsGalleryMPRZoomOperation();

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
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryMPRZoomOperation);
};

/// \class TpsGalleryMPRPanOperation
/// 
/// \brief this operation perform the pan operation on MPR
class TPS_RL_DECLSPEC TpsGalleryMPRPanOperation
    : public OperationBase{
public:
    //the constructor and destructor
    /// \brief constructor
    /// \param[in] startx   start x position in display cache cs
    /// \param[in] starty   start y position in display cache cs
    /// \param[in] stopx    stop x position in display cache cs
    /// \param[in] stopy    stop y position in display cache cs
    TpsGalleryMPRPanOperation(int width, int height, float startx, float starty, float stopx, float stopy);
    ~TpsGalleryMPRPanOperation();

    /// \brief Set GraphicObject keys
    /// \param[in] sVolumeKey       volume
    /// \param[in] sMPRKey          MPR
    /// \return void 
    void SetGraphicObjectKeys(const std::string& sVolumeKey, 
        const std::string& sMPRKey);

    virtual bool ModifyGraphicObject() const;

private:

private:
    float           mStartX;
    float           mStartY;
    float           mStopX;
    float           mStopY;
    std::string     mVolumeKey;
    std::string     mMPRKey;
    int             mWidth;
    int             mHeight;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryMPRPanOperation);
};

/// \class TpsGalleryMPRWindowingOperation
/// 
/// \brief this operation perform the windowing operation on MPR
class TPS_RL_DECLSPEC TpsGalleryMPRWindowingOperation : public OperationBase{
public:
    //the constructor and destructor
    /// \brief constructor
    /// \param[in] deltaWidth   delta width of windowing
    /// \param[in] deltaCenter  delta center of windowing
    TpsGalleryMPRWindowingOperation(float deltaWidth, float deltaCenter);
    ~TpsGalleryMPRWindowingOperation();

    /// \brief Set GraphicObject keys
    /// \param[in] sVolumeKey       volume
    /// \param[in] sMPRKey          MPR
    /// \return void 
    void SetGraphicObjectKeys(const std::string& sVolumeKey, 
        const std::string& sMPRKey,
        const std::string& mprCornerInfoKey, 
        const std::vector<std::string>& vecLinkedMprKey, 
        const std::vector<std::string>& vecLinkedMprCornerInfoKey);

    virtual bool ModifyGraphicObject() const;
    virtual bool Windowing_i(
        const std::string& mprKey, 
        const std::string& mprCornerInfoKey) const;

private:
    mutable float   mDeltaWidth;
    mutable float   mDeltaCenter;
    mutable double  mWindowWidth;
    mutable double  mWindowCenter;
    std::string     mVolumeKey;
    std::string     mMPRKey;
    std::string     mMprCornerInfoKey;
    std::vector<std::string>    mVecLinkedMprKey;
    std::vector<std::string>    mVecLinkedMprCornerInfoKey;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryMPRWindowingOperation);
};

TPS_END_NAMESPACE

#endif
