//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_rc_mlcrendercontroller.h 
///  \brief   render mlc in simulator of evaluation
///
///  \version 1.0
///  \date  2015/02/04  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_RC_MLCRENDERCONTROLLER_H_
#define TPS_RC_MLCRENDERCONTROLLER_H_

#include "tps_rc_defs.h"
#include "tps_rc_rendercontroller.h"
TPS_BEGIN_NAMESPACE
class TpsSimulatorMLCRenderStategy;

class TPS_RC_DECLSPEC TpsMLCRenderController :
    public TpsRenderController {

public:
    TpsMLCRenderController(WINDOW_TYPE type);
    virtual ~TpsMLCRenderController();

    virtual bool GetCurrentPixelSpacingIn10MM(double& spacingX, double& spacingY);

protected:
    //////////////////////////////////////////////////////////////////////////
    // \brief  setup a rendering scene according TPS data(such as a CT image)
    // \return  bool  true if succeeded
    //                if no data source has been set or other error occurs
    virtual bool SetupScene_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief reset data source, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \param[in] id  the identity of data source
    // \param[in] uid  the UID of data source in database
    // \return bool  true if succeeded, false if any error happens.
    virtual bool ResetDataSource_i(DATA_SOURCE_ID id);

    /// clear graphic objects from ModelWarehouse
    virtual void ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid);

private:
    //////////////////////////////////////////////////////////////////////////
    // \brief reset image data, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    //bool ResetImage_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief reset plan, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    bool ResetPlan_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief clear data source via specified id
    void RemoveDataSource_i(DATA_SOURCE_ID id);
private:
    //////////////////////////////////////////////////////////////////////////
    // \ mlc render strategy
    std::shared_ptr<TpsSimulatorMLCRenderStategy>              mMLCRenderStrategy;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsMLCRenderController);

};

TPS_END_NAMESPACE

#endif