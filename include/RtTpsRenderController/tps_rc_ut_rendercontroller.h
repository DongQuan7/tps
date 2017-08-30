////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rc_ut_rendercontroller.h
/// 
///  \brief class TpsUtRenderController declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/04/28
////////////////////////////////////////////////////////////////

#ifndef TPS_RC_UT_RENDERCONTROLLER_H_
#define TPS_RC_UT_RENDERCONTROLLER_H_

#include "tps_rc_defs.h"
#include "tps_rc_rendercontroller.h"
#include "RtTpsRenderController/tps_rc_mpr_render_controller.h"

TPS_BEGIN_NAMESPACE


class TPS_RC_DECLSPEC TpsUtRenderController : public TpsMPRRenderController //public TpsRenderController 
{
public:
    TpsUtRenderController(WINDOW_TYPE type);
    virtual ~TpsUtRenderController();

    FIRST_POSTFIX_COMPONENT GetSectionType() ;

    //virtual bool Initialize(DISPLAY_SIZE displaySize, DATA_SOURCE_GROUP_ID group = -1);

    virtual void Resize(DISPLAY_SIZE displaySize);

    virtual bool GetDisplaySize(DISPLAY_SIZE& displaySize);

    virtual bool TransformPoint(const Mcsf::MedViewer3D::Point2D& ptIn, 
                                Mcsf::MedViewer3D::Point3D& ptOut);

protected:

    virtual bool SetupScene_i();

    virtual bool ResetDataSource_i(DATA_SOURCE_ID id);
private:

    virtual bool ResetImage_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief clear data source via specified id
    void RemoveDataSource_i(DATA_SOURCE_ID id);
private:

    FIRST_POSTFIX_COMPONENT                     m_nSectionType;// = EMPTY_COMPONENT;
    int m_iWidth;
    int m_iHeight;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsUtRenderController);
};

TPS_END_NAMESPACE

#endif
