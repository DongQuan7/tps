//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_rl_simplebeamgraphicobjectconverter.h 
///  \brief  
///  \
///  class  TpsSimpleBeamGraphicObjectConverter
///  \version 1.0
///  \date  2014/11/24
///  \
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SIMPLEBEAM_GRAPHICOBJECTCONVERTER_H_
#define TPS_RL_SIMPLEBEAM_GRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"


TPS_BEGIN_NAMESPACE

class SimpleBeam;
/// \class TpsSimpleBeamGraphicObjectConverter
/// 
/// \brief this class convert beam in data layer to beam graphic object
class TPS_RL_DECLSPEC TpsSimpleBeamGraphicObjectConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsSimpleBeamGraphicObjectConverter(GO_TYPE goType);
    ~TpsSimpleBeamGraphicObjectConverter();

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
    SimpleBeam*       mSimpleBeam;

    TPS_DISALLOW_COPY_AND_ASSIGN(TpsSimpleBeamGraphicObjectConverter);
};

TPS_END_NAMESPACE

#endif