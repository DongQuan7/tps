//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
///
///  \file    tps_rl_gallery_measureangle_operation.h
///  \brief   
///
///  \version 1.0
///  \date    Mar. 06, 2015
///  
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_MEASUREANGLE_OPERATION_H_
#define TPS_RL_GALLERY_MEASUREANGLE_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

//MCSF
#include "McsfMedViewer3DArithmetic/point2d.h"
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

/// \class TpsGalleryMeasureAngleOperation
/// 
/// \brief this operation perform the measure angle operation
class TPS_RL_DECLSPEC TpsGalleryMeasureAngleOperation : public OperationBase
{
public:
    //the constructor and destructor
    /// \brief constructor
    /// \param[in] startx   start x position in display cache cs
    /// \param[in] starty   start y position in display cache cs
    /// \param[in] stopx    stop x position in display cache cs
    /// \param[in] stopy    stop y position in display cache cs
    TpsGalleryMeasureAngleOperation(const tps::DISPLAY_SIZE &displaySize, const LAYOUT_UNIT &unit,
        const std::list<Mcsf::MedViewer3D::Point2D>& anglePoints, const float cursorX, const float cursorY,
        const bool actionStarted, const bool firstSideDone, const bool secondSideDone);
    ~TpsGalleryMeasureAngleOperation();

    /// \brief Set GraphicObject keys
    /// \param[in] sMeasureKey       measure
    /// \return void 
    void SetGraphicObjectKeys(const std::string& sMprKey,
        const std::string& sMeasureKey, 
        const std::string& sMeasureValKey);

    virtual bool ModifyGraphicObject() const;

private:
    LAYOUT_UNIT                        mUnit;
    std::list<Mcsf::MedViewer3D::Point2D>   mAnglePoints;
    float                                   mCursorX;
    float                                   mCursorY;
    bool                                    mActionStarted;
    bool                                    mFirstSideDone;
    bool                                    mSecondSideDone;
    std::string                             mMprKey;
    std::string                             mMeasureKey;
    std::string                             mMeasureValKey;
    int                                     mWidth;
    int                                     mHeight;

private:
    float CalculateAngle(std::list<Mcsf::MedViewer3D::Point2D> anglePoints, int iWidth, int iHeight) const;

    //Mcsf::MedViewer3D::Point2D ScreenCoordinateToImageCoordinate(Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight) const;

    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryMeasureAngleOperation);
};
TPS_END_NAMESPACE

#endif