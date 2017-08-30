//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rl_gallery_mprgraphicobjectconverter.h
///  \brief   
///
///  \version 1.0
///  \date    Nov. 24, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_MPRGRAPHICOBJECTCONVERTER_H_
#define TPS_RL_GALLERY_MPRGRAPHICOBJECTCONVERTER_H_

#include "tps_rl_defs.h"

#include "RtTpsRenderLibrary/tps_rl_mprgraphicobjectconverter.h"

namespace Mcsf{
    namespace MedViewer3D{
        class GraphicObjVolume;
        class ClsGraphicObjectMPR;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

    /// \class TpsMPRGOConverter
    /// 
    /// \brief this class cut image 3d data to get a MPR graphic object.
class TPS_RL_DECLSPEC TpsGalleryMPRGOConverter : public TpsMPRGOConverter{
public:
    //the constructor and destructor
    TpsGalleryMPRGOConverter(GO_TYPE goType);
    ~TpsGalleryMPRGOConverter();

protected:
    //////////////////////////////////////////////////////////////////////////
    /// \brief get source data to be converted
    /// \param[in] uid  the UID of the TMS data
    /// \return bool  true if succeeded, false otherwise
    virtual bool PrepareSourceData_i(const GOConverterParameter& para);

    virtual bool Update_i(GraphicObjectPtr go);

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryMPRGOConverter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif