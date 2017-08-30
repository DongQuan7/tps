////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_rendercontrollerbase.h
/// 
///  \brief class TpsRenderController declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/11/29
////////////////////////////////////////////////////////////////

#ifndef TPS_RC_RENDERCONTROLLER_H_
#define TPS_RC_RENDERCONTROLLER_H_

#include "tps_rc_defs.h"
#include "RtTpsFramework/tps_fw_rendercontrollerbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsGOConverterBase;
class TpsRenderControllerBase;

/// \class TpsRenderController
/// 
/// \brief Render controller is used to setup a rendering scene using 3D 
///        render engine according to the data from TPS data layer, 
///        and take charge of it.
///        This class is an abstract base class
class TPS_RC_DECLSPEC TpsRenderController : public TpsRenderControllerBase {
public:
    //the constructor and destructor
    TpsRenderController(WINDOW_TYPE type);
    virtual ~TpsRenderController();

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsRenderController);

    //////////////////////////////////////////////////////////////////////////
    // \brief  get a unique converter instance via graphic object type
    // \param[in] goType the graphic object type defined in "tps_fw_renderdefines.h"
    // \return TpsGOConverterBase*  the converter pointer to be checked
    //         NULL will be returned if any error happens
    virtual TpsGOConverterBase* GetConverter_i(GO_TYPE goType);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
