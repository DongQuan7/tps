////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rc_mpr_render_controller.h
/// 
///  \brief class TpsMPRRenderController declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/13
////////////////////////////////////////////////////////////////

#ifndef TPS_RC_MPR_RENDERCONTROLLER_H_
#define TPS_RC_MPR_RENDERCONTROLLER_H_

#include "tps_rc_defs.h"
#include "tps_rc_rendercontroller.h"
#include <map>
#include <vector>

namespace Mcsf{
    class Point3f;
    class Vector2f;
}
namespace Mcsf {
    namespace MedViewer3D {
        class ClsGraphicObjectMPR;
        class GraphicObjVolume;
        class Point2D;
        class Matrix4x4;
        class Vector3D;
    }
}

TPS_BEGIN_NAMESPACE

class Volume4dMprRenderStrategy;
class TpsScaleRulerStrategy;
class ScaleRulerTextStrategy;
class CrosshairRenderStrategy;
class MPRPageTurnOperation;
class MPRGraphicObject;
class RtDosegrid;
struct HitTestResult;

/// \class TpsMPRRenderController
/// 
/// \brief *****
class TPS_RC_DECLSPEC TpsMPRRenderController : public TpsRenderController{
public:
    //the constructor and destructor
    TpsMPRRenderController(WINDOW_TYPE type);
    virtual ~TpsMPRRenderController();

    //////////////////////////////////////////////////////////////////////////
    // \brief  transform point form 2d screen coordinate to \
    //          specified c.s. related to the render controller
    // \param[in] ptIn  the view 2d normalized point
    // \param[out] ptOut  result point of render controller's coordinate interested
    // \return bool  true when succeeded, false otherwise
    // \note in mpr render controller, the target coordinate system is patient c.s.
    virtual bool TransformPoint(const Mcsf::MedViewer3D::Point2D& ptIn, 
        Mcsf::MedViewer3D::Point3D& ptOut);

    virtual bool TransformPoint(const Mcsf::MedViewer3D::Point3D& ptIn, 
        Mcsf::MedViewer3D::Point2D& ptOut);

    virtual bool TransformPoints(
        const std::vector<Mcsf::MedViewer3D::Point2D>& ptListIn, 
        std::vector<Mcsf::MedViewer3D::Point3D>& ptListOut,
        bool isSquaredPoints = true);

    virtual bool SetCrosshairHittedMode(int mode);

    //根据当前nudge的半径刷新前端图元的半径
    virtual bool MapMilimeter2Pixel(double lengthInmm, 
        double* pixelinXNP, double* pixelinYNP);

    //Hit Test绘制内容，取代之前Hit GO的实现。MprRenderController需要拆分，
    //以明确不同绘制内容窗口对应唯一RenderController类，HitTest的实现也将变得更单一、清晰
    virtual HitTestResult HitTest(double screenX, double screenY);

    //判断点的距离，前端拿该值用于判断poi点是否在当前层附近 true则绘制poi name false不绘制
    double GetDistanceToPlane(std::string &imageUID, float x3D, float y3D, float z3D);

    //UBoy placement Indicator
    bool GetMprViewDirecrion(Mcsf::MedViewer3D::Vector3D &vectorView, Mcsf::MedViewer3D::Vector3D &vectotUp);

    //给重前端传十字线位置用来刷新
    bool ResetMPRObject();

    //////////////////////////////////////////////////////////////////////////
    // \brief  
    // \return section type
    FIRST_POSTFIX_COMPONENT GetSectionType();

    bool GetCrosshairProjectionPosition(float& x2D, float& y2D);

    bool GetCrossXYPoints(double& x1, double& y1, double& x2, double& y2, double& x3, double& y3, double& x4, double& y4);

    void HideNonImageOverlay(bool isToHide);

    bool GetNormal(Mcsf::MedViewer3D::Vector3D& normal);

    bool IsInVolume(float x, float y) const;
protected:
    //////////////////////////////////////////////////////////////////////////
    // \brief  setup a rendering scene according TPS data(such as a CT image)
    // \return  bool  true if succeeded
    // false if no data source has been set or other error occurs
    virtual bool SetupScene_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief reset data source, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \param[in] id  the identity of data source
    // \return bool  true if succeeded, false if any error happens.
    bool ResetDataSource_i(DATA_SOURCE_ID id);

    //////////////////////////////////////////////////////////////////////////
    // \brief reset image data, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    virtual bool ResetImage_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief reset float image data, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    virtual bool ResetFloatImage_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief reset plan, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    virtual bool ResetPlan_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief clear data source via specified id
    virtual void RemoveDataSource_i(DATA_SOURCE_ID id);

    /// clear graphic objects from ModelWarehouse
    virtual void ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid);

private:
    // \brief set section type according to WINDOW_TYPE when initialize
    void SetSectionType_i();

protected:
    //Axial, Sagittal, Coronal, Oblique
    FIRST_POSTFIX_COMPONENT             mSectionType;
    //PRE, POST
    SECOND_POSTFIX_COMPONENT            mLocation;

    std::shared_ptr<Volume4dMprRenderStrategy>          mMprStrategy;
    std::shared_ptr<TpsScaleRulerStrategy>              mScaleRulerStrategy;
    std::shared_ptr<CrosshairRenderStrategy>            m_spCrosshairRenderStrategy;

    bool                                                mIsDirty;
    DATA_SOURCE_GROUP_ID                                mWindowGroupType;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsMPRRenderController);
};

TPS_END_NAMESPACE

#endif
