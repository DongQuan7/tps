//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
///
///  \file    tps_rl_gallery_mprrotate_operation.h
///  \brief   
///
///  \version 1.0
///  \date    Feb. 05, 2015
///  
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_MPRROTATE_OPERATION_H_
#define TPS_RL_GALLERY_MPRROTATE_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class ClsGraphicObjectMPR;
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE

/// \class TpsGalleryMPRPanOperation
/// 
/// \brief this operation perform the pan operation on MPR
class TPS_RL_DECLSPEC TpsGalleryMPRRotateOperation : public OperationBase
{
public:
    //the constructor and destructor
    /// \brief constructor
    /// \param[in] startx   start x position in display cache cs
    /// \param[in] starty   start y position in display cache cs
    /// \param[in] stopx    stop x position in display cache cs
    /// \param[in] stopy    stop y position in display cache cs
    TpsGalleryMPRRotateOperation(int width, int height, float startx, float starty, float stopx, float stopy);
    ~TpsGalleryMPRRotateOperation();

    /// \brief Set GraphicObject keys
    /// \param[in] sVolumeKey       volume
    /// \param[in] sMPRKey          MPR
    /// \return void 
    void SetGraphicObjectKeys(const std::string& sVolumeKey, 
        const std::string& sMPRKey);

    virtual bool ModifyGraphicObject() const;

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
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryMPRRotateOperation);
};
TPS_END_NAMESPACE

#endif