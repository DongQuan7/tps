//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_rc_easyplan_image_rendercontroller.h 
///  \brief   easy plan image window
///
///  \version 1.0
///  \date  2014/10/16  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_RC_EASYPLAN_IMAGE_RENDERCONTROLLER_H_
#define TPS_RC_EASYPLAN_IMAGE_RENDERCONTROLLER_H_

#include "tps_rc_defs.h"
#include "tps_rc_rendercontroller.h"

namespace Mcsf{
    class Point3f;
    class Vector2f;
}
namespace Mcsf {
    namespace MedViewer3D {
        class Point2D;
    }
}

TPS_BEGIN_NAMESPACE

class TpsEasyPlanDrrRenderStrategy;
class EasyPlanMeasureDistanceStrategy;
class EasyPlanMeasureAngleStrategy;
class TpsEasyPlanSetPoiRenderStrategy;
class TpsImage2DEntity;
class EasyPlanDrrGraphicObejct;

class TPS_RC_DECLSPEC TpsEasyPlanImageRenderController : public TpsRenderController {
public:
    /////////////////////////////////////////////////////////////////
    ///  \brief Constructor
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    TpsEasyPlanImageRenderController(WINDOW_TYPE type);
    /////////////////////////////////////////////////////////////////
    ///  \brief Destructor
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    ~TpsEasyPlanImageRenderController();
    //////////////////////////////////////////////////////////////////////////
    // \brief  transform point form 2d screen coordinate to \
    //          specified c.s. related to the render controller
    // \param[in] ptIn  the view 2d normalized point
    // \param[out] ptOut  result point of render controller's coordinate interested
    // \return bool  true when succeeded, false otherwise
    // \note in drr render controller, the target coordinate system is image c.s.
    virtual bool TransformPoint(const Mcsf::MedViewer3D::Point2D& ptIn, 
        Mcsf::MedViewer3D::Point3D& ptOut);

    bool GetDrrGoZoomFactor(std::string imageUid, float& zoomFactor);

    //////////////////////////////////////////////////////////////////////////
    // \brief setPixelSpaceData for join Poi info in image
    // \ if no image exist,use default.
    // \param[in] width  the identity of data source
    // \param[in] height  the UID of data source in database
    // \param[in] mImagePlanPixelSpacingX  image PixelSpaceX
    // \param[in] mImagePlanPixelSpacingY   image PixelSpaceY
    void SetPixelSpaceData( float mImagePlanPixelSpacingX, float mImagePlanPixelSpacingY);

    virtual bool GetCurrentPixelSpacingIn10MM(double& spacingX, double& spacingY);

protected:
    //////////////////////////////////////////////////////////////////////////
    // \brief  setup a rendering scene according TPS data(such as a CT image)
    // \return  bool  true if succeeded
    //                if no data source has been set or other error occurs
    bool SetupScene_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief reset data source, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \param[in] id  the identity of data source
    // \param[in] uid  the UID of data source in database
    // \return bool  true if succeeded, false if any error happens.
    bool ResetDataSource_i(DATA_SOURCE_ID id);

    /// clear graphic objects from ModelWarehouse
    virtual void ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid);
private:
    //////////////////////////////////////////////////////////////////////////
    // \brief reset plan, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    bool ResetPlan_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief clear data source via specified id
    void RemoveDataSource_i(DATA_SOURCE_ID id);

    bool GetEasyPlanActiveBeamUid(std::string& beamUid);
    std::shared_ptr<EasyPlanDrrGraphicObejct> GetEasyPlanDrrGoByImageUid(const std::string& imageUid);
private:
    //////////////////////////////////////////////////////////////////////////
    // \ drr renderstrategy
    std::shared_ptr<TpsEasyPlanDrrRenderStrategy>            mDRRStrategy;
    //////////////////////////////////////////////////////////////////////////
    // \ easyplan measure distance render strategy
    std::shared_ptr<EasyPlanMeasureDistanceStrategy>         mEasyPlanMeasureDistanceStrategy;
    //////////////////////////////////////////////////////////////////////////
    // \ easyplan measure angle render strategy
    std::shared_ptr<EasyPlanMeasureAngleStrategy>            mEasyPlanMeasureAngleStrategy;
    //////////////////////////////////////////////////////////////////////////
    // \ render set isocenter in easyplan
    std::shared_ptr<TpsEasyPlanSetPoiRenderStrategy>        mEasyPlanSetIsoCenterStrategy;

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanImageRenderController);

};

TPS_END_NAMESPACE

#endif
