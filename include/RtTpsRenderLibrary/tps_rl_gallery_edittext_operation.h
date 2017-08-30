//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
///
///  \file    tps_rl_gallery_edittext_operation.h
///  \brief   
///
///  \version 1.01
///  \date    Jul. 01, 2015
///  
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_EDITTEXT_OPERATION_H_
#define TPS_RL_GALLERY_EDITTEXT_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE

    /// \class TpsGalleryEditTextOperation
    /// 
    /// \brief this operation perform the edit text operation
class TPS_RL_DECLSPEC TpsGalleryEditTextOperation : public OperationBase
{
public:
    //the constructor and destructor
    /// \brief constructor
    /// \param[in] startx   start x position in display cache cs
    /// \param[in] starty   start y position in display cache cs
    /// \param[in] stopx    stop x position in display cache cs
    /// \param[in] stopy    stop y position in display cache cs
    TpsGalleryEditTextOperation(const std::string textKey, const LAYOUT_UNIT &unit,
        const float startX, const float startY, const float stopX, const float stopY,
        const bool isDelete);
    ~TpsGalleryEditTextOperation();

    virtual bool ModifyGraphicObject() const;

private:
    LAYOUT_UNIT                         mUnit;
    float                               mStartX;
    float                               mStartY;
    float                               mStopX;
    float                               mStopY;
    bool                                mIsDelete;
    std::string                         mTextKey;
    int                                 mWidth;
    int                                 mHeight;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryEditTextOperation);
};
TPS_END_NAMESPACE

#endif