////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Xi.Chen xi.chen@united-imaging.com
/// 
///  \file tps_rl_cornerinfographicobjectconverter.h
/// 
///  \brief class TpsRulerTextGOConverter defination 
/// 
///  \version 1.0
/// 
///  \date    2014/06/23
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_RULERTEXTGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_RULERTEXTGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class MPRGraphicObject;
/// \class TpsRulerTextGOConverter
/// 
/// \brief this class convert text to textoverlay graphic object
class TPS_RL_DECLSPEC TpsRulerTextGOConverter : 
    public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsRulerTextGOConverter(GO_TYPE goType);
    ~TpsRulerTextGOConverter();

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
    void CalculatePixelSpacing(const std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> &spVolumeGO,
        const std::shared_ptr<MPRGraphicObject> &spMPRGO);

    double mRulerLength;
    double mXOrigin;
    double mYOrigin;

    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsRulerTextGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_RL_RULERTEXTGRAPHICOBJECTCONVERTER_H_
