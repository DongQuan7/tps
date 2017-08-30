////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wang Jie  jie.wang@united-imaging.com
/// 
///  \file tps_rc_common_rendercontroller.h
/// 
///  \brief class TpsCommonMPRRenderController declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/11/03
////////////////////////////////////////////////////////////////

#ifndef TPS_RC_COMMON_MPRRENDERCONTROLLER_H_
#define TPS_RC_COMMON_MPRRENDERCONTROLLER_H_

#include "tps_rc_defs.h"
#include "tps_rc_mpr_render_controller.h"
#include <vector>



TPS_BEGIN_NAMESPACE
class TpsRenderStrategyBase;
class TpsBeamRenderStrategy;
class TpsMPRDoseRenderStrategy;
//class TpsVoiRenderStrategy;
class TpsPoiRenderStrategy;
class DoseGridBoundaryRenderStrategy;
class CrosshairRenderStrategy;
class MprHotSpotsRenderStrategy;
class SeriesTableRenderStrategy;
class MprRoiRenderStrategy;
typedef std::shared_ptr<TpsRenderStrategyBase> TpsRSPtr;

class TPS_RC_DECLSPEC TpsCommonMPRRenderController : public TpsMPRRenderController 
{
public:
    TpsCommonMPRRenderController(WINDOW_TYPE type);
    virtual ~TpsCommonMPRRenderController();

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

    void HitChess_i(double x, double y, HitTestResult& hitRes);
    void HitDoseGrid_i(double x, double y, HitTestResult& hitRes);
    void HitBeam_i(double x, double y, HitTestResult& hitRes);

private:
    DATA_SOURCE_GROUP_ID                                mWindowGroupType;
    std::shared_ptr<TpsBeamRenderStrategy>              mBeamStrategy;
    //std::shared_ptr<TpsVoiRenderStrategy>               mVOIStrategy;
    std::shared_ptr<MprRoiRenderStrategy>               mRoiStrategy;
    std::shared_ptr<TpsMPRDoseRenderStrategy>           mDoseStrategy;
    std::shared_ptr<DoseGridBoundaryRenderStrategy>     mDoseGridBoundaryStrategy;
    std::shared_ptr<DoseGridBoundaryRenderStrategy>     mLimitRegionStrategy;
    std::shared_ptr<TpsPoiRenderStrategy>               m_spPoiStrategy;
    std::shared_ptr<TpsPoiRenderStrategy>               m_spMaxPointStrategy;
    std::shared_ptr<CrosshairRenderStrategy>            m_spCrosshairRenderStrategy;
    std::shared_ptr<MprHotSpotsRenderStrategy>          m_spMprHotSpotsRenderStrategy;
    std::shared_ptr<SeriesTableRenderStrategy>          mSeriesTableRenserStrategy;
    bool                                                mIsDirty;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsCommonMPRRenderController);
};

TPS_END_NAMESPACE

#endif
