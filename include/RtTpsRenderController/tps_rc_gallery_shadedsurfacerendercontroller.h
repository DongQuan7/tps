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

#include "tps_rc_defs.h"
#include "tps_rc_rendercontroller.h"

namespace Mcsf {
    namespace MedViewer3D {
        class Vector3D;
    }
}

TPS_BEGIN_NAMESPACE
//class MascotVrRenderStrategy;
class GalleryVRRenderStrategy;

class TPS_RC_DECLSPEC TpsGalleryShadedSurfaceRenderController :
    public TpsRenderController {
public:
    TpsGalleryShadedSurfaceRenderController(WINDOW_TYPE type);
    virtual ~TpsGalleryShadedSurfaceRenderController();

    bool GetShadedSurfaceViewDirecrion(Mcsf::MedViewer3D::Vector3D &vectorView, Mcsf::MedViewer3D::Vector3D &vectotUp);

    void RemoveGraphicObjects(const std::string& seriesUID);

protected:
    //////////////////////////////////////////////////////////////////////////
    // \brief  setup a rendering scene according TPS data(such as a CT image)
    // \return  bool  true if succeeded
    //                if no data source has been set or other error occurs
    virtual bool SetupScene_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief reset data source, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \param[in] id  the identity of data source
    // \return bool  true if succeeded, false if any error happens.
    virtual bool ResetDataSource_i(DATA_SOURCE_ID id);

private:
    //////////////////////////////////////////////////////////////////////////
    // \brief reset image data, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    bool ResetImage_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief clear data source via specified id
    void RemoveDataSource_i(DATA_SOURCE_ID id);

    //////////////////////////////////////////////////////////////////////////
    // \brief Remove graphic object of seriesuid
    void RemoveGraphicObject_i(const std::string& seriesUID, GO_TYPE goType);

private:
    // Delete by wzd 2016-10-10 小人前端WPF绘制
    //std::shared_ptr<MascotVrRenderStrategy>         m_spMascotVrRenderStrategy
    std::shared_ptr<GalleryVRRenderStrategy>        m_spVRRenderStrategy;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryShadedSurfaceRenderController); 
};


TPS_END_NAMESPACE
