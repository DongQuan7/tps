//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rc_gallery_mprrendercontroller.h
///  \brief   
///
///  \version 1.0
///  \date    Nov. 24, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RC_GALLERY_MPR_RENDERCONTROLLER_H_
#define TPS_RC_GALLERY_MPR_RENDERCONTROLLER_H_

#include "tps_rc_defs.h"
#include "tps_rc_rendercontroller.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_patientmanager.h"
#include "RtTpsRenderLibrary/tps_rl_cornertextinforenderstrategy.h"

namespace Mcsf{
    class Point3f;
    class Vector2f;
}
namespace Mcsf {
    namespace MedViewer3D {
        class ClsGraphicObjectMPR;
        class GraphicObjVolume;
        class Point2D;
        class Vector3D;
    }
}

TPS_BEGIN_NAMESPACE

class TpsRenderStrategyBase;
//class MPRRenderStrategy;
class GalleryVolume4dMprRenderStrategy;
class TpsScaleRulerStrategy;
class ScaleRulerTextStrategy;
class CrosshairRenderStrategy;
class MprHotSpotsRenderStrategy;
class DummyRenderStrategy;
class GalleryMeasureAngleStrategy;
class GalleryMeasureDistanceStrategy;
class GalleryMeasureAngleValStrategy;
class GalleryTextStrategy; //

typedef std::shared_ptr<TpsRenderStrategyBase> TpsRSPtr;

/// \class TpsMPRRenderController
/// 
/// \brief *****
class TPS_RC_DECLSPEC TpsGalleryMPRRenderController : public TpsRenderController{
public:
    //the constructor and destructor
    TpsGalleryMPRRenderController(WINDOW_TYPE type);
    virtual ~TpsGalleryMPRRenderController();

    //////////////////////////////////////////////////////////////////////////
    // \brief  transform point form 2d screen coordinate to \
    //          specified c.s. related to the render controller
    // \param[in] ptIn  the view 2d normalized point
    // \param[out] ptOut  result point of render controller's coordinate interested
    // \return bool  true when succeeded, false otherwise
    // \note in mpr render controller, the target coordinate system is patient c.s.
    virtual bool TransformPoint(const Mcsf::MedViewer3D::Point2D& ptIn, 
        Mcsf::MedViewer3D::Point3D& ptOut);

    virtual bool TransformPoint(const Mcsf::MedViewer3D::Point3D& ptIn,
        Mcsf::MedViewer3D::Point2D& ptOut);

    virtual bool SetCrosshairHittedModeForGallery(int mode);
    //////////////////////////////////////////////////////////////////////////
    // \brief  
    // \param[out] current slice number
    // \param[out] totalSlice count
    // \return bool [true] if  [false] if
    virtual bool GetCurrentMprSlice(int& slice, int& startSliceNumber, int& totalSlice);

    //////////////////////////////////////////////////////////////////////////
    // \brief  
    // \return section type
    FIRST_POSTFIX_COMPONENT GetSectionType();

    //virtual bool ResetSeries4D(const std::string&);

    void RemoveGraphicObjects(const std::string& seriesUID);

    bool GetCrosshairProjectionPosition(float& x2D, float& y2D);

    bool GetCrossXYPoints(double& x1, double& y1, double& x2, double& y2, double& x3, double& y3, double& x4, double& y4);

    void HideNonImageOverlay(bool isToHide);

    bool GetMprViewDirecrion(Mcsf::MedViewer3D::Vector3D &vectorView, Mcsf::MedViewer3D::Vector3D &vectotUp);

    bool ResetMPRObject();

protected:
    //////////////////////////////////////////////////////////////////////////
    // \brief  setup a rendering scene according TPS data(such as a CT image)
    // \return  bool  true if succeeded
    // false if no data source has been set or other error occurs
    virtual bool SetupScene_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief reset data source, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \param[in] id  the identity of data source
    // \return bool  true if succeeded, false if any error happens.
    virtual bool ResetDataSource_i(DATA_SOURCE_ID id);

    //////////////////////////////////////////////////////////////////////////
    // \brief reset image data, replace graphic objects needed.
    //        if not exist, convert a new one.
    // \return bool  true if succeeded, false if any error happens.
    virtual bool ResetImage_i();

    //////////////////////////////////////////////////////////////////////////
    // \brief clear data source via specified id
    virtual void RemoveDataSource_i(DATA_SOURCE_ID id);

    ///// clear graphic objects from ModelWarehouse
    //virtual void ClearGraphicObjects_i(DATA_SOURCE_ID id, const DATA_SOURCE_UID& uid);

private:
    // \brief set section type according to WINDOW_TYPE when initialize
    void SetSectionType_i();

    //virtual bool ResetSeries4D_i(const std::string&);

    //void RemoveGraphicObject_i(const std::string& seriesUID, GO_TYPE goType);


protected:
    //Axial, Sagittal, Coronal, Oblique
    FIRST_POSTFIX_COMPONENT             mSectionType;
    //PRE, POST
    SECOND_POSTFIX_COMPONENT            mLocation;

private:
    //std::shared_ptr<MPRRenderStrategy>                  m_spMPRStrategy;
    std::shared_ptr<GalleryVolume4dMprRenderStrategy>   m_spMprStrategy;
    std::shared_ptr<TpsScaleRulerStrategy>              m_spScaleRulerStrategy;
    //std::shared_ptr<DummyRenderStrategy>                m_spDummyStrategy;
    std::shared_ptr<CrosshairRenderStrategy>            m_spCrosshairRenderStrategy;
    //std::shared_ptr<MprHotSpotsRenderStrategy>          m_spMprHotSpotsRenderStrategy;
    std::shared_ptr<GalleryMeasureAngleStrategy>        m_spMeasureAngleStrategy;
    std::shared_ptr<GalleryMeasureDistanceStrategy>     m_spMeasureDistanceStrategy;

    std::shared_ptr<GalleryMeasureAngleValStrategy>     m_spMeasureAngleValStrategy;
    std::shared_ptr<GalleryTextStrategy>                m_spTextStrategy;
    bool                                                mIsDirty;

    std::map<int, std::vector<TpsRSPtr>> mStrategyList;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryMPRRenderController);
};

TPS_END_NAMESPACE

#endif
