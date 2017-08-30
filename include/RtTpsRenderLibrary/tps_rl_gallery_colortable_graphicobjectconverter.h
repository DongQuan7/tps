////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rl_gallery_cornerinfographicobjectconverter.h
///  \brief   
///
///  \version 1.0
///  \date    Jul. 10, 2015
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_COLORTABLE_GRAPHICOBJECTCONVERTER_H_
#define TPS_RL_GALLERY_COLORTABLE_GRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjectVR_ColorTable;
    }
}

TPS_BEGIN_NAMESPACE

/// \class TpsTextOverlayGOConverter
/// 
/// \brief this class convert text to text overlay graphic object
class TPS_RL_DECLSPEC TpsGalleryColorTableGOConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsGalleryColorTableGOConverter(GO_TYPE goType);
    ~TpsGalleryColorTableGOConverter();

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
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjectVR_ColorTable> m_spColorTable;

    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryColorTableGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_RL_TEXTOVERLAYGRAPHICOBJECTCONVERTER_H_
