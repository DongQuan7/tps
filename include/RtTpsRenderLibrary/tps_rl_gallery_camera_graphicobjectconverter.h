//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    
///  \brief   
///
///  \version 1.0
///  \date    Apr. 17, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_CAMERA_GRAPHIC_OBJECT_CONVERTER_H_
#define TPS_RL_GALLERY_CAMERA_GRAPHIC_OBJECT_CONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

namespace Mcsf{
    namespace MedViewer3D{
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsSegmentGraphicObject;

/// \class TpsGalleryCameraGoConverter
/// 
/// \brief *****
class TPS_RL_DECLSPEC TpsGalleryCameraGoConverter : public TpsGOConverterBase {
public:
    //the constructor and destructor
    TpsGalleryCameraGoConverter(GO_TYPE goType);
    virtual ~TpsGalleryCameraGoConverter();

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
    //std::shared_ptr<TpsSegmentGraphicObject> mActiveSegment;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> mVolume;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryCameraGoConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif