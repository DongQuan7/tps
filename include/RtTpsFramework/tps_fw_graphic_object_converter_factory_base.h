////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_graphic_object_converter_factory_base.h
/// 
///  \brief class GOConverterFactoryBase declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/07/13
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_GRAPHIC_OBJECT_CONVERTER_FACTORY_BASE_H_
#define TPS_FW_GRAPHIC_OBJECT_CONVERTER_FACTORY_BASE_H_

#include "RtTpsFramework/tps_fw_defs.h"


#include <map>

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsGOConverterBase;
class ModelWarehouse;
class ITpsDataRepository;
/// \class GOConverterFactoryBase
/// 
/// \brief graphic object converter factory base class
class TPS_FW_DECLSPEC GOConverterFactoryBase{

public:
    GOConverterFactoryBase();
    virtual ~GOConverterFactoryBase();

    //////////////////////////////////////////////////////////////////////////
    // \brief  get a unique converter instance via graphic object type
    // \param[in] goType the graphic object type
    // \return TpsGOConverterBase*  the converter pointer to be checked
    //         NULL will be returned if any error happens
    TpsGOConverterBase* GetConverter(GO_TYPE goType);

    //////////////////////////////////////////////////////////////////////////
    // \brief  set model warehouse 
    virtual void SetModelWarehouse(ModelWarehouse* modelWarehouse);

    //////////////////////////////////////////////////////////////////////////
    // \brief set data repository
    virtual void SetDataRepository(ITpsDataRepository* dataRepository);

    void ClearAll();
private:
    //////////////////////////////////////////////////////////////////////////
    // \brief  create a new converter instance via graphic object type
    // \param[in] goType the graphic object type defined in "tps_fw_renderdefines.h"
    // \return TpsGOConverterBase*  the new converter pointer
    //         NULL will be returned if any error happens
    virtual TpsGOConverterBase* CreateConverter_i(GO_TYPE goType) = 0;

protected:
    ITpsDataRepository*                     mDataRepository;
    ModelWarehouse*                         mModelWarehouse;
    std::map<GO_TYPE, TpsGOConverterBase*>  mConverterMap;
};

TPS_END_NAMESPACE  // end namespace tps

#endif