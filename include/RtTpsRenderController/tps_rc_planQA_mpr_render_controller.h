////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wang Jie  jie.wang@united-imaging.com
/// 
///  \file tps_rc_planQA_mpr_render_controller.h
/// 
///  \brief class TpsPlanQAMPRRenderController declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/11/03
////////////////////////////////////////////////////////////////

#ifndef TPS_RC_PLANQA_MPRRENDERCONTROLLER_H_
#define TPS_RC_PLANQA_MPRRENDERCONTROLLER_H_

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
class TpsBeamRenderStrategy;
class TpsMPRDoseRenderStrategy;
class TpsRoiRenderStrategy;
class MprRoiRenderStrategy;
class TpsPoiRenderStrategy;
class TpsScaleRulerStrategy;
class DoseGridBoundaryRenderStrategy;
class BackgroundRenderStrategy;
class TpsColorSpectraRenderStrategy;
class CrosshairRenderStrategy;
class TableBoxRenderStrategy;

typedef std::shared_ptr<TpsRenderStrategyBase> TpsRSPtr;

class TPS_RC_DECLSPEC TpsPlanQAMPRRenderController : public TpsMPRRenderController 
{
public:
    TpsPlanQAMPRRenderController(WINDOW_TYPE type);
    virtual ~TpsPlanQAMPRRenderController();

    HitTestResult HitTest(double screenX, double screenY);

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

    //////////////////////////////////////////////////////////////////////////
    // \brief clear data source via specified id
    void RemoveDataSource_i(DATA_SOURCE_ID id);

private:
    // \brief set section type according to WINDOW_TYPE when initialize
    void SetSectionType_i();
    void HitDoseGrid_i(double x, double y, HitTestResult& hitRes);
    void HitBeam_i(double x, double y, HitTestResult& hitRes);

private:
    DATA_SOURCE_GROUP_ID                                mWindowGroupType;
    std::shared_ptr<TpsBeamRenderStrategy>              mBeamStrategy;
    std::shared_ptr<MprRoiRenderStrategy>               mVOIStrategy;
    std::shared_ptr<TpsMPRDoseRenderStrategy>           mDoseStrategy;
    std::shared_ptr<DoseGridBoundaryRenderStrategy>     mDoseGridBoundaryStrategy;
    std::shared_ptr<DoseGridBoundaryRenderStrategy>     mLimitRegionStrategy;
    std::shared_ptr<TpsPoiRenderStrategy>               m_spPoiStrategy;
    std::shared_ptr<TpsPoiRenderStrategy>               m_spMaxPointStrategy;

    bool                                                mIsDirty;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsPlanQAMPRRenderController);
};

TPS_END_NAMESPACE

#endif
