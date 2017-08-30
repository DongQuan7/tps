//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rl_gallery_volumegraphicobjectconverter.h
///  \brief   
///
///  \version 1.0
///
///  \date    Dec. 29, 2014
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_VOLUMEGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_GALLERY_VOLUMEGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsRenderLibrary/tps_rl_volumegraphicobjectconverter.h"

//namespace Mcsf{
//    namespace MedViewer3D{
//        class MedViewer3DImageData;
//    }
//}

TPS_BEGIN_NAMESPACE  // begin namespace tps

//class TpsImage3DEntity;
//typedef std::shared_ptr<Mcsf::MedViewer3D::MedViewer3DImageData> VolumeDataPtr;

/// \class TpsGalleryVolumeGOConverter
/// 
/// \brief this class convert image 3d data in data layer of TMS to 
///        volume graphic object of 3d rendering engine
class TPS_RL_DECLSPEC TpsGalleryVolumeGOConverter : 
    public TpsVolumeGOConverter {
public:
    //the constructor and destructor
    TpsGalleryVolumeGOConverter(GO_TYPE goType);
    ~TpsGalleryVolumeGOConverter();

protected:
    //////////////////////////////////////////////////////////////////////////
    /// \brief create new graphic object instance
    /// \return GraphicObjectPtr the new graphic object
    //virtual GraphicObjectPtr CreateGraphicObject_i();

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
    //////////////////////////////////////////////////////////////////////////
    // \brief convert rt database image data (RTFWKDLDOImage3D) to
    //        3D render engine image data (MedViewer3DImageData)
    // \param[in] imageEntity  image 3d data entity pointer
    // \return std::shared_ptr<Mcsf::MedViewer3D::MedViewer3DImageData>  the 3D volume data
    //         NULL will be returned when failed to convert
    //VolumeDataPtr ConvertRTImageToVolumeDataIn3D_i(
    //    const TpsImage3DEntity* const imageEntity);

protected:
    //TpsImage3DEntity*       mImage;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryVolumeGOConverter);
};

class TPS_RL_DECLSPEC TpsGalleryMaskGOConverter : 
    public TpsMaskGOConverter {
public:
    //the constructor and destructor
    TpsGalleryMaskGOConverter(GO_TYPE goType);
    ~TpsGalleryMaskGOConverter();

protected:
    //////////////////////////////////////////////////////////////////////////
    /// \brief create new graphic object instance
    /// \return GraphicObjectPtr the new graphic object
    //virtual GraphicObjectPtr CreateGraphicObject_i();

    //////////////////////////////////////////////////////////////////////////
    /// \brief get source data to be converted
    /// \param[in] uid  the UID of the TMS data
    /// \return bool  true if succeeded, false otherwise
    virtual bool PrepareSourceData_i(const GOConverterParameter& para);

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryMaskGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif