////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rc_rendercontrollerfactory.h
/// 
///  \brief class TpsRenderControllerFactory declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/02
////////////////////////////////////////////////////////////////

#ifndef TPS_RC_RENDERCONTROLLERFACTORY_H_
#define TPS_RC_RENDERCONTROLLERFACTORY_H_

#include "tps_rc_defs.h"
#include "tps_singleton.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsRenderControllerBase;
class TpsDisplayDelegate;

/// \class TpsRenderControllerFactory
/// 
/// \brief simple factory of render controller, this class will create different
///        render controller instances according to specified window type
class TPS_RC_DECLSPEC TpsRenderControllerFactory
{
    DECLARE_SINGLETON(TpsRenderControllerFactory);

public:

    //////////////////////////////////////////////////////////////////////////
    // \brief create render controller instance according to specified 
    //        window type & data source(optional)
    // \param[in] type  the window type, such as BEV, MPR_AXIAL, MPR_AXIAL_SOURCE_GROUP_FUSION_FIXED, .etc
    //            in our case, it corresponds to a view control of front end, 
    // \return TpsRenderControllerBase instance pointer
    TpsRenderControllerBase* CreateRenderController(WINDOW_TYPE type);

private:
    //the constructor and destructor
    TpsRenderControllerFactory();
};


TPS_END_NAMESPACE  // end namespace tps

#endif