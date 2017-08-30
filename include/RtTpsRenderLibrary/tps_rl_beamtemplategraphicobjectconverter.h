//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_rl_beamtemplategraphicobjectconverter.h 
///  \brief  
///  \
///  class  TpsBeamTemplateGraphicObjectConverter
///  \version 1.0
///  \date  2014/11/21
///  \
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_BEAMGRAPHICOBJECT_CONVERTER_H_
#define TPS_RL_BEAMGRAPHICOBJECT_CONVERTER_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"


TPS_BEGIN_NAMESPACE

class BeamTemplate;
class TpsSimpleBeamGraphicObjectConverter;
class TpsImage3DEntity;

/// \class TpsBeamTemplateGraphicObjectConverter
/// 
/// \brief this class convert beamtemplate in data layer to beamtemplate graphic object
class TPS_RL_DECLSPEC TpsBeamTemplateGraphicObjectConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsBeamTemplateGraphicObjectConverter(GO_TYPE goType);
    ~TpsBeamTemplateGraphicObjectConverter();

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
    TpsSimpleBeamGraphicObjectConverter         *mSimpleBeamGoConverter;
    BeamTemplate                                   *mBeamTemplate;
    TpsImage3DEntity                               *mImage;
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsBeamTemplateGraphicObjectConverter);
};

TPS_END_NAMESPACE

#endif