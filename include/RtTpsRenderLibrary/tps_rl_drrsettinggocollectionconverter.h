////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  zuo kai  kai.zuo@united-imaging.com
/// 
///  \file tps_rl_drrsettinggocollectionconverter.h
/// 
///  \brief class TpsDrrSettingGOCollectionConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/20
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_DRRSETTINGGOCOLLECTIONCONVERTER_H_
#define TPS_RL_DRRSETTINGGOCOLLECTIONCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class TpsContoursGOCollectionConverter
/// 
class TPS_RL_DECLSPEC TpsDrrSettingGOCollectionConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsDrrSettingGOCollectionConverter(GO_TYPE goType);
    ~TpsDrrSettingGOCollectionConverter();

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
    virtual bool PrepareSourceData_i(const GOConverterParameter& para);

    //////////////////////////////////////////////////////////////////////////
    /// \brief update graphic object according to source data
    /// \brief go  the graphic object pointer to be updated
    /// \return bool  true if succeeded, false otherwise
    virtual bool Update_i(GraphicObjectPtr go);

private:
    // the default copy and assign method are not allowed.
    std::string                           mPlanUid;
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsDrrSettingGOCollectionConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif