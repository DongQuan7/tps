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

#ifndef TPS_RC_ADMIN_MPRRENDERCONTROLLER_H_
#define TPS_RC_ADMIN_MPRRENDERCONTROLLER_H_

#include "tps_rc_defs.h"
#include "tps_rc_mpr_render_controller.h"
#include <vector>


TPS_BEGIN_NAMESPACE
class MprRoiRenderStrategy;
class TpsPoiRenderStrategy;
class SeriesTableRenderStrategy;

typedef std::shared_ptr<TpsRenderStrategyBase> TpsRSPtr;

class TPS_RC_DECLSPEC TpsAdminMPRRenderController : public TpsMPRRenderController 
{
public:
    TpsAdminMPRRenderController(WINDOW_TYPE type);
    virtual ~TpsAdminMPRRenderController();

    //FIRST_POSTFIX_COMPONENT  GetSectionType() ;
protected:
    //////////////////////////////////////////////////////////////////////////
    // \brief  setup a rendering scene according TPS data(such as a CT image)
    // \return  bool  true if succeeded
    //                if no data source has been set or other error occurs
    virtual bool SetupScene_i();

    /// clear graphic objects from ModelWarehouse
    virtual void ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid);

    //////////////////////////////////////////////////////////////////////////
    // \brief reset image data, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    virtual bool ResetImage_i();

private:


    //////////////////////////////////////////////////////////////////////////
    // \brief clear data source via specified id
    void RemoveDataSource_i(DATA_SOURCE_ID id);

    // \brief set section type according to WINDOW_TYPE when initialize
    void SetSectionType_i();

private:
    DATA_SOURCE_GROUP_ID                                mWindowGroupType;
    std::shared_ptr<MprRoiRenderStrategy>               mVOIStrategy;
    std::shared_ptr<TpsPoiRenderStrategy>               m_spPoiStrategy;
    std::shared_ptr<SeriesTableRenderStrategy>          mSeriesTableRenserStrategy;

    bool                                                mIsDirty;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsAdminMPRRenderController);
};

TPS_END_NAMESPACE

#endif
