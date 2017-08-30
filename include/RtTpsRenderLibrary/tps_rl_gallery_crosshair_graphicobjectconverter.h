//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rl_gallery_crosshair_graphicobjectconverter.h
///  \brief   
///
///  \version 1.0
///  \date    Dec. 03, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_CROSSHAIR_GRAPHICOBJECTCONVERTER_H_
#define TPS_RL_GALLERY_CROSSHAIR_GRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class MPRGraphicObject;

/// \class TpsGalleryCrosshairGOConverter
/// 
/// \brief this class cut image 3d data to get a MPR graphic object.
class TPS_RL_DECLSPEC TpsGalleryCrosshairGOConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsGalleryCrosshairGOConverter(GO_TYPE goType);
    virtual ~TpsGalleryCrosshairGOConverter();

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