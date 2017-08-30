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
///  \date    Dec. 03, 2014
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_TEXTOVERLAYGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_GALLERY_TEXTOVERLAYGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE

class RtSeries;
class RtImage3DHeader;
class MPRGraphicObject;
/// \class TpsTextOverlayGOConverter
/// 
/// \brief this class convert text to text overlay graphic object
class TPS_RL_DECLSPEC TpsGalleryCornerInfoGOConverter : public TpsGOConverterBase{
public:
    //the constructor and destructor
    TpsGalleryCornerInfoGOConverter(GO_TYPE goType);
    ~TpsGalleryCornerInfoGOConverter();

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
    void GetSlicePosAndThickness(
        std::shared_ptr<MPRGraphicObject> mpr,
        std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volume, 
        double* slicepos, size_t* slicetotal, double* thickness);

    void GetMousePosition(std::shared_ptr<MPRGraphicObject> mpr, 
        std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volume, 
        double *pos, std::string& posKey);

protected:
    std::shared_ptr<MPRGraphicObject>                       mMpr;
    std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume>    mVolume;
    RtSeries*                                 mSeries;
    RtImage3DHeader*                          mHeader;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryCornerInfoGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_RL_TEXTOVERLAYGRAPHICOBJECTCONVERTER_H_
