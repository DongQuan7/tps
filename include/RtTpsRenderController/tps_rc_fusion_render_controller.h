////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rc_fusion_render_controller.h
/// 
///  \brief class TpsFusionRenderController declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/05/12
////////////////////////////////////////////////////////////////

#ifndef TPS_RC_FUSIONRENDERCONTROLLER_H_
#define TPS_RC_FUSIONRENDERCONTROLLER_H_

#include "tps_rc_defs.h"
#include "tps_rc_rendercontroller.h"
#include "RtTpsRenderController/tps_rc_mpr_render_controller.h"
#include "McsfMedViewer3DArithmetic/matrix_4x4.h"

TPS_BEGIN_NAMESPACE
class MprRoiRenderStrategy;


class TPS_RC_DECLSPEC TpsFusionRenderController :
    public TpsMPRRenderController {
public:
    TpsFusionRenderController(WINDOW_TYPE type);
    virtual ~TpsFusionRenderController();

protected:
    //////////////////////////////////////////////////////////////////////////
    // \brief  setup a rendering scene according TPS data(such as a CT image)
    // \return  bool  true if succeeded
    //                if no data source has been set or other error occurs
    virtual bool SetupScene_i();

    /// clear graphic objects from ModelWarehouse
    virtual void ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid);

    HitTestResult HitTest(double screenX, double screenY);
private:
    //////////////////////////////////////////////////////////////////////////
    // \brief reset image data, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    bool ResetImage_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief reset float image data, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    bool ResetFloatImage_i();

    // \brief set section type according to WINDOW_TYPE when initialize
    void SetSectionType_i();

    void HitChess_i(double x, double y, HitTestResult& hitRes);

    //////////////////////////////////////////////////////////////////////////
    // \brief clear data source via specified id
    void RemoveDataSource_i(DATA_SOURCE_ID id);

private:
    std::shared_ptr<MprRoiRenderStrategy> mVOIStrategy;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsFusionRenderController);
};

TPS_END_NAMESPACE

#endif
