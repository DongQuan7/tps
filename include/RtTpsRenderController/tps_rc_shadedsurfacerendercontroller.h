////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rc_shadedsurfacerendercontroller.h
/// 
///  \brief class ShadedSurfaceRenderController declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/02/28
////////////////////////////////////////////////////////////////

#include "tps_rc_defs.h"
#include "tps_rc_rendercontroller.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class Vector3D;
    }
}

TPS_BEGIN_NAMESPACE
class TpsShadedVoiStrategy;
class VOI3DRenderStrategy;
class BevBeamRenderStrategy;
class TpsVolumeRenderingStrategy;
class Tps3DWindowRenderStrategy;

class TPS_RC_DECLSPEC ShadedSurfaceRenderController :
    public TpsRenderController
{
public:
    ShadedSurfaceRenderController(WINDOW_TYPE type);
    virtual ~ShadedSurfaceRenderController();

    bool GetShadedSurfaceViewDirecrion(Mcsf::MedViewer3D::Vector3D &vectorView, Mcsf::MedViewer3D::Vector3D &vectotUp); 

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

    /// clear graphic objects from ModelWarehouse
    virtual void ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid);
private:
    //////////////////////////////////////////////////////////////////////////
    // \brief reset image data, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    bool ResetImage_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief reset plan, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    bool ResetPlan_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief clear data source via specified id
    void RemoveDataSource_i(DATA_SOURCE_ID id);
private:

    std::shared_ptr<VOI3DRenderStrategy>            mLinedVoiStrategy;
    std::shared_ptr<Tps3DWindowRenderStrategy>      m3DWindowRenderStrategy;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(ShadedSurfaceRenderController);
};


TPS_END_NAMESPACE
