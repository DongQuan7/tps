////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_volumegraphicobjectconverter.h
/// 
///  \brief class TpsVolumeGOConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/18
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_VOLUMEGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_VOLUMEGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

namespace Mcsf{
    namespace MedViewer3D{
        class MedViewer3DImageData;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsImage3DEntity;
typedef std::shared_ptr<Mcsf::MedViewer3D::MedViewer3DImageData> VolumeDataPtr;

/// \class TpsVolumeGOConverter
/// 
/// \brief this class convert image 3d data in data layer of TMS to 
///        volume graphic object of 3d rendering engine
class TPS_RL_DECLSPEC TpsVolumeGOConverter : 
    public TpsGOConverterBase {
public:
    //the constructor and destructor
    TpsVolumeGOConverter(GO_TYPE goType);
    ~TpsVolumeGOConverter();

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
    //////////////////////////////////////////////////////////////////////////
    // \brief convert rt database image data (RTFWKDLDOImage3D) to
    //        3D render engine image data (MedViewer3DImageData)
    // \param[in] imageEntity  image 3d data entity pointer
    // \return std::shared_ptr<Mcsf::MedViewer3D::MedViewer3DImageData>  the 3D volume data
    //         NULL will be returned when failed to convert
    VolumeDataPtr ConvertRTImageToVolumeDataIn3D_i(
        const TpsImage3DEntity* const imageEntity);

protected:
    TpsImage3DEntity*       mImage;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsVolumeGOConverter);
};

class TPS_RL_DECLSPEC TpsMaskGOConverter : 
    public TpsGOConverterBase {
public:
    //the constructor and destructor
    TpsMaskGOConverter(GO_TYPE goType);
    ~TpsMaskGOConverter();

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

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsMaskGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif