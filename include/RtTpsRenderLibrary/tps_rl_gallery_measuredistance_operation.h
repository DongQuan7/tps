//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  yuxuan.duan yuxuan.duan@united-imaging.com
///
///  \file    tps_rl_gallery_measuredistance_operation.h
///  \brief   
///
///  \version 1.0
///  \date    Apr. 22, 2015
///  
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_MEASUREDISTANCE_OPERATION_H_
#define TPS_RL_GALLERY_MEASUREDISTANCE_OPERATION_H_

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

    /// \class TpsGalleryMeasureDistanceOperation
    /// 
    /// \brief this operation perform the measure distance operation
class TPS_RL_DECLSPEC TpsGalleryMeasureDistanceOperation : public OperationBase
{
public:
    //the constructor and destructor
    /// \brief constructor
    /// \param[in] startx   start x position in display cache cs
    /// \param[in] starty   start y position in display cache cs
    /// \param[in] stopx    stop x position in display cache cs
    /// \param[in] stopy    stop y position in display cache cs
    TpsGalleryMeasureDistanceOperation(const tps::DISPLAY_SIZE &displaySize, const LAYOUT_UNIT &unit,
        const float startX, const float startY, const float stopX, const float stopY,
        const bool actionStarted, const bool actionDone);
    ~TpsGalleryMeasureDistanceOperation();

    /// \brief Set GraphicObject keys
    /// \param[in] sMeasureKey       measure
    /// \return void 
    void SetGraphicObjectKeys(
        const std::string& sVolumeKey,
        const std::string& sMprKey,
        const std::string& sMeasureKey, 
        const std::string& sMeasureValKey);

    virtual bool ModifyGraphicObject() const;

private:
    LAYOUT_UNIT                         mUnit;
    float                               mStartX;
    float                               mStartY;
    float                               mStopX;
    float                               mStopY;
    bool                                mActionStarted;
    bool                                mActionDone;
    std::string                         mVolumeKey;
    std::string                         mMprKey;
    std::string                         mMeasureKey;
    std::string                         mMeasureValKey;
    int                                 mWidth;
    int                                 mHeight;

private:
    float CalculateDistance(Mcsf::MedViewer3D::Point3D startPoint, Mcsf::MedViewer3D::Point3D stopPoint) const;

    //Mcsf::MedViewer3D::Point2D ScreenCoordinateToImageCoordinate(Mcsf::MedViewer3D::Point2D anglePoint, int iWidth, int iHeight) const;

    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryMeasureDistanceOperation);
};
TPS_END_NAMESPACE

#endif