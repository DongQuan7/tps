////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_crosshair_graphicobjectconverter.h
/// 
///  \brief class TpsCrosshairGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/07/08
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_CROSSHAIR_GRAPHICOBJECTCONVERTER_H_
#define TPS_RL_CROSSHAIR_GRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class MPRGraphicObject;

/// \class TpsCrosshairGOConverter
/// 
/// \brief this class cut image 3d data to get a MPR graphic object.
class TPS_RL_DECLSPEC TpsCrosshairGOConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsCrosshairGOConverter(GO_TYPE goType);
    virtual ~TpsCrosshairGOConverter();

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

    std::string                             m_strImageUID;
};

TPS_END_NAMESPACE  // end namespace tps

#endif