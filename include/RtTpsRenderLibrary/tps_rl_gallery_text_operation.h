//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
///
///  \file    tps_rl_gallery_text_operation.h
///  \brief   
///
///  \version 1.01
///  \date    2015/06/12
///  
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_TEXT_OPERATION_H_
#define TPS_RL_GALLERY_TEXT_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

//MCSF
#include "McsfMedViewer3DArithmetic/point3d.h"
#include <list>

namespace Mcsf
{
    namespace MedViewer3D
    {
        class ClsGraphicObjectMPR;
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE

    /// \class TpsGalleryTextOperation
    /// 
    /// \brief this operation perform the text operation
class TPS_RL_DECLSPEC TpsGalleryTextOperation : public OperationBase
{
public:
    //the constructor and destructor
    /// \brief constructor
    /// \param[in] startx   start x position in display cache cs
    /// \param[in] starty   start y position in display cache cs
    /// \param[in] stopx    stop x position in display cache cs
    /// \param[in] stopy    stop y position in display cache cs
    TpsGalleryTextOperation(const tps::DISPLAY_SIZE &displaySize, const LAYOUT_UNIT &unit,
        const float startX, const float startY, const float textWidth, const float textHeight, const std::string text);
    ~TpsGalleryTextOperation();

    /// \brief Set GraphicObject keys
    /// \param[in] sMeasureKey       measure
    /// \return void 
    void SetGraphicObjectKeys(
        const std::string& sVolumeKey,
        const std::string& sMprKey,
        const std::string& sTextKey);

    virtual bool ModifyGraphicObject() const;

private:
    LAYOUT_UNIT                         mUnit;
    float                               mStartX;
    float                               mStartY;
    float                               mTextWidth;
    float                               mTextHeight;
    std::string                         mText;
    //bool                                mActionStarted;
    //bool                                mActionDone;
    std::string                         mVolumeKey;
    std::string                         mMprKey;
    std::string                         mTextKey;
    int                                 mWidth;
    int                                 mHeight;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryTextOperation);
};
TPS_END_NAMESPACE

#endif