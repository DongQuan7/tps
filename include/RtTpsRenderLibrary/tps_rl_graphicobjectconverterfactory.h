////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_graphicobjectconverterfactory.h
/// 
///  \brief class TpsGOConverterFactory declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/18
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GRAPHICOBJECTCONVERTERFACTORY_H_
#define TPS_RL_GRAPHICOBJECTCONVERTERFACTORY_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphic_object_converter_factory_base.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsGOConverterBase;

/// \class TpsGOConverterFactory
/// 
/// \brief this factory take charge of the graphic object converters.
///        one can get a unique converter instance by graphic object type defined
///        in "tps_rl_graphicobjecttypedefines.h". this class check the converter map,
///        if not exist, it will create new one.
class TPS_RL_DECLSPEC TpsGOConverterFactory : public GOConverterFactoryBase{

public:
    TpsGOConverterFactory();
    virtual ~TpsGOConverterFactory();

private:
    //////////////////////////////////////////////////////////////////////////
    // \brief  create a new converter instance via graphic object type
    // \param[in] goType the graphic object type defined in "tps_fw_renderdefines.h"
    // \return TpsGOConverterBase*  the new converter pointer
    //         NULL will be returned if any error happens
    TpsGOConverterBase* CreateConverter_i(GO_TYPE goType);
};

TPS_END_NAMESPACE  // end namespace tps

#endif