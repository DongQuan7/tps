////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wang Jie  jie.wang@united-imaging.com
/// 
///  \file tps_rc_common_fusion_mpr_render_controller.h
/// 
///  \brief class TpsCommonFusionMPRRenderController declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/11/03
////////////////////////////////////////////////////////////////

#ifndef TPS_RC_COMMON_FUSION_MPRRENDERCONTROLLER_H_
#define TPS_RC_COMMON_FUSION_MPRRENDERCONTROLLER_H_

#include "tps_rc_defs.h"
#include "tps_rc_mpr_render_controller.h"
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
class TpsRenderStrategyBase;
class FusionRenderStrategy;
class LandmarkRenderStrategy;
class LandmarkTextRenderStrategy;
class DoseGridBoundaryRenderStrategy;
class ITpsDataRepository;
class MprRoiRenderStrategy;


typedef std::shared_ptr<TpsRenderStrategyBase> TpsRSPtr;

class TPS_RC_DECLSPEC TpsCommonFusionMPRRenderController : public TpsMPRRenderController 
{
public:
    TpsCommonFusionMPRRenderController(WINDOW_TYPE type);
    virtual ~TpsCommonFusionMPRRenderController();

    //FIRST_POSTFIX_COMPONENT  GetSectionType() ;
protected:
    //////////////////////////////////////////////////////////////////////////
    // \brief  setup a rendering scene according TPS data(such as a CT image)
    // \return  bool  true if succeeded
    //                if no data source has been set or other error occurs
    virtual bool SetupScene_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief reset plan, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    virtual bool ResetPlan_i();

    /// clear graphic objects from ModelWarehouse
    virtual void ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid);

    //////////////////////////////////////////////////////////////////////////
    // \brief reset image data, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    virtual bool ResetImage_i();

    virtual HitTestResult HitTest(double screenX, double screenY);

private:
    //////////////////////////////////////////////////////////////////////////
    // \brief clear data source via specified id
    void RemoveDataSource_i(DATA_SOURCE_ID id);

    // \brief set section type according to WINDOW_TYPE when initialize
    void SetSectionType_i();

    //void HitLandmark_i(double x, double y, HitTestResult& hitRes);

    void HitAlignmentBox_i(double x, double y, HitTestResult& hitRes);

private:
    DATA_SOURCE_GROUP_ID                                mWindowGroupType;
    std::shared_ptr<DoseGridBoundaryRenderStrategy>     mFusionRegionStrategy;
    //std::shared_ptr<LandmarkRenderStrategy>             mLandmarkStrategy;
    //std::shared_ptr<LandmarkTextRenderStrategy>         mLandmarkTextStrategy;
    std::shared_ptr<MprRoiRenderStrategy>               mROIStrategy;


    bool                                                mIsDirty;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsCommonFusionMPRRenderController);
};

TPS_END_NAMESPACE

#endif
