//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_rl_surfacecamera_graphicobject_converter.h
///
/// \brief   SurfaceCameraGraphicObjectConverter
/// 
/// \version 1.0
/// 
/// \date    2015/06/09
/////////////////////////////////////////////////////////////////////////
#ifndef TPS_RL_SURFACECAMERA_GRAPHICOBJECT_CONVERTER_H_
#define TPS_RL_SURFACECAMERA_GRAPHICOBJECT_CONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class SurfaceCameraGraphicObjectConverter
/// 
/// \brief *****
class TPS_RL_DECLSPEC SurfaceCameraGraphicObjectConverter : public TpsGOConverterBase {
public:
    //the constructor and destructor
    SurfaceCameraGraphicObjectConverter(GO_TYPE goType);
    virtual ~SurfaceCameraGraphicObjectConverter();

protected:
    //////////////////////////////////////////////////////////////////////////
    /// \brief create new graphic object instance
    /// \return GraphicObjectPtr the new graphic object
    virtual GraphicObjectPtr CreateGraphicObject_i();

    //////////////////////////////////////////////////////////////////////////
    /// \brief update graphic object according to source data
    /// \brief go  the graphic object pointer to be updated
    /// \return bool  true if succeeded, false otherwise
    virtual bool Update_i(GraphicObjectPtr go);

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(SurfaceCameraGraphicObjectConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_RL_SURFACECAMERA_GRAPHICOBJECT_CONVERTER_H_