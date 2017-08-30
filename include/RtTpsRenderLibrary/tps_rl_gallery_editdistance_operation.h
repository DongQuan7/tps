//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
///
///  \file    tps_rl_gallery_editdistance_operation.h
///  \brief   
///
///  \version 1.01
///  \date    Jun. 04, 2015
///  
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_EDITDISTANCE_OPERATION_H_
#define TPS_RL_GALLERY_EDITDISTANCE_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

////MCSF
//#include "McsfMedViewer3DArithmetic/point3d.h"
//#include <list>
//
//namespace Mcsf
//{
//    namespace MedViewer3D
//    {
//        class ClsGraphicObjectMPR;
//        class GraphicObjVolume;
//    }
//}

TPS_BEGIN_NAMESPACE

    /// \class TpsGalleryEditDistanceOperation
    /// 
    /// \brief this operation perform the edit distance operation
class TPS_RL_DECLSPEC TpsGalleryEditDistanceOperation : public OperationBase
{
public:
    //the constructor and destructor
    /// \brief constructor
    /// \param[in] startx   start x position in display cache cs
    /// \param[in] starty   start y position in display cache cs
    /// \param[in] stopx    stop x position in display cache cs
    /// \param[in] stopy    stop y position in display cache cs
    TpsGalleryEditDistanceOperation(/*const tps::DISPLAY_SIZE &displaySize, */const std::string measureKey, const std::string measureValKey,
        const LAYOUT_UNIT &unit, const float startX, const float startY, const float stopX, const float stopY,
        const bool isDelete);
    ~TpsGalleryEditDistanceOperation();

    ///// \brief Set GraphicObject keys
    ///// \param[in] sMeasureKey       measure
    ///// \return void 
    //void SetGraphicObjectKeys(
    //    const std::string& sVolumeKey,
    //    const std::string& sMprKey,
    //    const std::string& sMeasureKey, 
    //    const std::string& sMeasureValKey);

    virtual bool ModifyGraphicObject() const;

private:
    LAYOUT_UNIT                         mUnit;
    float                               mStartX;
    float                               mStartY;
    float                               mStopX;
    float                               mStopY;
    bool                                mIsDelete;
    std::string                         mMeasureKey;
    std::string                         mMeasureValKey;
    int                                 mWidth;
    int                                 mHeight;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryEditDistanceOperation);
};
TPS_END_NAMESPACE

#endif