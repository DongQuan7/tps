////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Xi.Chen xi.chen@united-imaging.com
/// 
///  \file tps_rl_cornerinfographicobjectconverter.h
/// 
///  \brief class TpsCornerInfoGOConverter definition 
/// 
///  \version 1.0
/// 
///  \date    2014/06/23
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_TEXTOVERLAYGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_TEXTOVERLAYGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
    }
}

namespace RTFWK{
    class RTFWKDLDOSeries;
    class RTFWKDLDOImage3DHeader;
}

TPS_BEGIN_NAMESPACE

class MPRGraphicObject;
class RtSeries;
class RtImage3DHeader;

/// \class TpsTextOverlayGOConverter
/// 
/// \brief this class convert text to text overlay graphic object
class TPS_RL_DECLSPEC TpsCornerInfoGOConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsCornerInfoGOConverter(GO_TYPE goType);
    ~TpsCornerInfoGOConverter();

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

protected:
    std::shared_ptr<MPRGraphicObject>                         mMpr;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>    mVolume;
    std::pair<int, RtSeries*>                   mIndexSeriesPair;
    RtImage3DHeader*                             mHeader;
    RtSeries*                                                 mSeries;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsCornerInfoGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_RL_TEXTOVERLAYGRAPHICOBJECTCONVERTER_H_
