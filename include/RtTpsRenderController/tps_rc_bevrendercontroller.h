////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rc_bevrendercontroller.h
/// 
///  \brief class TpsBEVRenderController declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/01/20
////////////////////////////////////////////////////////////////


#ifndef TPS_RC_BEVRENDERCONTROLLER_H_
#define TPS_RC_BEVRENDERCONTROLLER_H_

#include "tps_rc_defs.h"
#include "tps_rc_rendercontroller.h"


namespace Mcsf {
    namespace MedViewer3D {
        class ClsGraphicObjectMPR;
        class GraphicObjVolume;
        class Point2D;
        class Matrix4x4;
        class Vector3D;
    }
}

TPS_BEGIN_NAMESPACE


class TpsBEVBeamRenderStrategy;
class FluenceMapRenderStrategy;
class TpsShadedVoiStrategy;
class VOI3DRenderStrategy;
class ApplicatorRenderStrategy;
class DrrRenderStrategy;
//class TpsBevDoseShadedStrategy;
class BevWindowTransparencyRenderStrategy;

class TPS_RC_DECLSPEC TpsBEVRenderController :
    public TpsRenderController {

public:
    TpsBEVRenderController(WINDOW_TYPE type);
    virtual ~TpsBEVRenderController();

    //////////////////////////////////////////////////////////////////////////
    // \brief  transform point form 2d screen coordinate to \
    //          specified c.s. related to the render controller
    // \param[in] ptIn  the view 2d normalized point
    // \param[out] ptOut  result point of render controller's coordinate interested
    // \return bool  true when succeeded, false otherwise
    // \note in BEV render controller, the target coordinate system is collimator c.s. on iso plane
    virtual bool TransformPoint(const Mcsf::MedViewer3D::Point2D& ptIn, 
        Mcsf::MedViewer3D::Point3D& ptOut);

    virtual bool GetCurrentPixelSpacingIn10MM(double& spacingX, double& spacingY);
    bool GetBevViewDirecrion(Mcsf::MedViewer3D::Vector3D &vectorView, Mcsf::MedViewer3D::Vector3D &vectotUp);

    void DownSampleImage(float rate);

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
    // \param[in] uid  the UID of data source in database
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
    //////////////////////////////////////////////////////////////////////////
    //std::shared_ptr<TpsShadedVoiStrategy>           mShadedVoiStrategy;
    std::shared_ptr<BevWindowTransparencyRenderStrategy> mTransparencyRenderStrategy;
    std::shared_ptr<VOI3DRenderStrategy>            mVOIRenderStrategy;
    std::shared_ptr<ApplicatorRenderStrategy>       mApplicatorRenderStrategy;
    std::shared_ptr<DrrRenderStrategy>              mDrrRenderStrategy;
    //std::shared_ptr<TpsBevDoseShadedStrategy>       mBevDoseShadedRenderStrategy;
    std::shared_ptr<FluenceMapRenderStrategy>       mFluenceMapRenderStrategy;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsBEVRenderController);

};

TPS_END_NAMESPACE

#endif
