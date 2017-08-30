////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2017
///  All rights reserved.
/// 
///  \author  Liang Xiang  xiang.liang@united-imaging.com
/// 
///  \file tps_rl_drrcollection_graphic_object_converter.h
/// 
///  \brief class TpsEasyPlanDrrCollectionGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2017/7/12
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EASYPLANDRRCOLLECTIONGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_EASYPLANDRRCOLLECTIONGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include <vector>

TPS_BEGIN_NAMESPACE

class TpsEasyPlanDrrGOConverter;
class RtBeam;
class TpsImage2DEntity;

class TPS_RL_DECLSPEC TpsEasyPlanDrrCollectionGOConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsEasyPlanDrrCollectionGOConverter(GO_TYPE goType, TpsEasyPlanDrrGOConverter* easyPlanDrrConverter);
    ~TpsEasyPlanDrrCollectionGOConverter();

    //////////////////////////////////////////////////////////////////////////
    // \brief  set model warehouse 
    virtual void SetModelWarehouse(ModelWarehouse* modelWarehouse);

    //////////////////////////////////////////////////////////////////////////
    // \brief set data repository
    virtual void SetDataRepository(ITpsDataRepository* dataRepository);

protected:
    //////////////////////////////////////////////////////////////////////////
    /// \brief create new graphic object instance
    /// \return GraphicObjectPtr the new graphic object
    virtual GraphicObjectPtr CreateGraphicObject_i();

    //////////////////////////////////////////////////////////////////////////
    /// \brief get source data to be converted
    /// \param[in] uid  the UID of the TMS data
    /// \return bool  true if succeeded, false otherwise
    virtual bool PrepareSourceData_i(const DataSourceMap& dataSourceMap);

    //////////////////////////////////////////////////////////////////////////
    /// \brief update graphic object according to source data
    /// \brief go  the graphic object pointer to be updated
    /// \return bool  true if succeeded, false otherwise
    virtual bool Update_i(GraphicObjectPtr go);

protected:
    TpsEasyPlanDrrGOConverter*                     mEasyPlanDrrConverter;
    std::vector<RtBeam*>                           mBeamList;
    //key = beamUID, value = imageData
    std::map<std::string, TpsImage2DEntity*>       mRtImageMap;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanDrrCollectionGOConverter);
};

TPS_END_NAMESPACE

#endif
