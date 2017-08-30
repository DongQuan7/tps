//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rc_gallery_mprrendercontroller.cpp
///  \brief   
///
///  \version 1.0
///  \date    Nov. 24, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderController/tps_rc_gallery_mprrendercontroller.h"

#include "boost/algorithm/string/split.hpp"
#include "boost/lexical_cast.hpp"

//ZHENGHE
#include "McsfViewer3DModel/mcsf_viewer3d_model.h"
#include "McsfMedViewer3DScene/mcsf_viewer3d_view.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_mpr.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DArithmetic/point3d.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_raii_utility.h"

// Render Strategies
//#include "RtTpsRenderLibrary/tps_rl_mprrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_volume4d_mpr_renderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_scalerulerstrategy.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_renderstrategy.h"         // for CrosshairRenderStrategy
#include "RtTpsRenderLibrary/tps_rl_mpr_hot_spots_renderstrategy.h"     // for MprHotSpotsRenderStrategy
#include "RtTpsRenderLibrary/tps_rl_dummyrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measureanglestrategy.h"     // for MeasureAngleStrategy
#include "RtTpsRenderLibrary/tps_rl_gallery_measuredistancestrategy.h"  // for MeasureDistanceStrategy
#include "RtTpsRenderLibrary/tps_rl_gallery_measureanglevaluestrategy.h"// for MeasureValueStrategy
#include "RtTpsRenderLibrary/tps_rl_gallery_textstrategy.h"             // for TextStrategy

// Graphic Objects
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_textoverlaygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobject.h"          // for CrosshairGraphicObject
#include "RtTpsRenderLibrary/tps_rl_hot_spots_graphicobject.h"          // for HotSpotsGraphicObject
#include "RtTpsRenderLibrary/tps_rl_volume4d_graphicobject.h"           // for Volume4dGraphicObject
#include "RtTpsRenderLibrary/tps_rl_dummygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measuregraphicobject.h"     // for MeasureAngleGraphicObject
#include "RtTpsRenderLibrary/tps_rl_gallery_measurevaluegraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_textgraphicobject.h"        // for TextGraphicObject


// frameworks
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_displaycache.h"
#include "RtTpsFramework/tps_fw_safelock.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_rendercontrollerbase.h"
#include "RtTpsFramework/tps_fw_graphic_object_converter_factory_base.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsFramework/tps_fw_series_group4d.h"

#include "RtTpsDataAccess/tps_da_gallery_imagemanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "tps_logger.h"
#include "RtTpsRenderLibrary/tps_rl_mpr_helper.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE

TpsGalleryMPRRenderController::TpsGalleryMPRRenderController(WINDOW_TYPE type)
    : TpsRenderController(type),
      m_spMprStrategy(nullptr),
      m_spScaleRulerStrategy(nullptr),
      //m_spDummyStrategy(nullptr),
      m_spCrosshairRenderStrategy(nullptr),
      //m_spMprHotSpotsRenderStrategy(nullptr),
      m_spMeasureAngleStrategy(nullptr),
      m_spMeasureDistanceStrategy(nullptr),
      m_spMeasureAngleValStrategy(nullptr),
      m_spTextStrategy(nullptr)
{
    SetSectionType_i();
}

TpsGalleryMPRRenderController::~TpsGalleryMPRRenderController()
{

}

bool TpsGalleryMPRRenderController::SetupScene_i()
{
    try
    {
        ///////////////////////////IMAGE Overlay////////////////////////////
        m_spMprStrategy.reset(new GalleryVolume4dMprRenderStrategy());
        RegisterRenderStrategy_i(ClsView::IMAGE, m_spMprStrategy);

        /////////////////////////GRAPHIC Overlay////////////////////////////
        m_spScaleRulerStrategy.reset(new TpsScaleRulerStrategy());
        RegisterRenderStrategy_i(ClsView::GRAPHIC, m_spScaleRulerStrategy);

        m_spMeasureAngleStrategy.reset(new GalleryMeasureAngleStrategy());

        m_spMeasureDistanceStrategy.reset(new GalleryMeasureDistanceStrategy());

        m_spMeasureAngleValStrategy.reset(new GalleryMeasureAngleValStrategy());
        if (!mDisplayCache->RegisterCPURenderStrategy("GalleryMeasureAngleVal", m_spMeasureAngleValStrategy.get()))
        {
            TPS_LOG_DEV_WARNING << "Failed to RegisterCPURenderStrategy GalleryMeasureAngleVal";
        }

        m_spTextStrategy.reset(new GalleryTextStrategy());
        if (!mDisplayCache->RegisterCPURenderStrategy("GalleryText", m_spTextStrategy.get()))
        {
            TPS_LOG_DEV_WARNING << "Failed to RegisterCPURenderStrategy GalleryText";
        }

        return true;
    }
    catch (std::exception& ex)
    {
        TPS_LOG_DEV_ERROR << "Failed to setup scene: " << ex.what();
        return false;
    }
}

bool TpsGalleryMPRRenderController::ResetDataSource_i(DATA_SOURCE_ID id)
{
    bool bRes = false;
    switch (id)
    {
    case IMAGE:
        bRes = ResetImage_i();
        break;
    default:
        TPS_LOG_DEV_ERROR << "Unrecognized data source id: " << id;
        break;
    }
    return bRes;
}

bool TpsGalleryMPRRenderController::ResetImage_i()
{
    std::string imageUID = mDataSourceMap[IMAGE];

    auto spSeriesGroup4d = mDataRepository->GetGalleryImageManager()->GetSeriesGroup4d(imageUID);
    bool is4DSeries = spSeriesGroup4d != nullptr;

    //check volume graphic object
    IGraphicObjectPtr goVolume = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_GALLERY_VOLUME, goVolume))
    {
        TPS_LOG_DEV_ERROR << "Failed to check volume graphic object of image: " << imageUID;
        return false;
    }

    //check volume4d graphic object
    std::shared_ptr<Volume4dGraphicObject> spVolume4dGO = nullptr;
    if (is4DSeries)
    {
        IGraphicObjectPtr goVolume4d= nullptr;
        if (!CheckGraphicObject_i(spSeriesGroup4d->m_strMainSeriesUid, GO_TYPE_GALLERY_VOLUME_D4, goVolume4d))
        //if (!CheckGraphicObject_i(imageUID, GO_TYPE_GALLERY_VOLUME_D4, goVolume4d))
        {
            TPS_LOG_DEV_ERROR<<"Failed to check volume4d graphic object of image: "<<imageUID;
            return false;
        }
        spVolume4dGO =std::dynamic_pointer_cast<Volume4dGraphicObject>(goVolume4d);
        if (spVolume4dGO == nullptr)
        {
            TPS_LOG_DEV_ERROR<<"Failed to check volume4d graphic object of image: "<<imageUID;
            return false;
        }
        spVolume4dGO->ClearAllVolumes();

        IGraphicObjectPtr goVol = nullptr;
        std::shared_ptr<GraphicObjVolume> spVolume = nullptr;
        GOConverterParameter para;
        for(int i = 0; i < spSeriesGroup4d->m_vecSeriesUid.size(); ++i)
        {
            para.data_source_map[IMAGE] = spSeriesGroup4d->m_vecSeriesUid[i];
            if(spSeriesGroup4d->m_vecSeriesUid[i] == imageUID)
            {
                goVol = goVolume;
            }
            else if(!CheckGraphicObject_i(spSeriesGroup4d->m_vecSeriesUid[i], GO_TYPE_GALLERY_VOLUME, goVol, false, &para)){
                TPS_LOG_DEV_ERROR<<"Failed to check volume graphic object of image: "<<spSeriesGroup4d->m_vecSeriesUid[i];
            }

            spVolume =std::dynamic_pointer_cast<GraphicObjVolume>(goVol);
            if (spVolume != nullptr)
            {
                spVolume4dGO->AddVolume(spVolume);
            }
        }
        spVolume =std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
        spVolume4dGO->SetCurrentVolume(spVolume);
        spVolume4dGO->SetMainVolume(spVolume);
    }

    //check mask volume graphic object
    IGraphicObjectPtr goMask = nullptr;
    if (!CheckGraphicObject_i(imageUID, GO_TYPE_GALLERY_MASK, goMask))
    {
        TPS_LOG_DEV_ERROR << "Failed to check mask volume graphic object of image: " << imageUID;
        return false;
    }

    //check mpr graphic object
    if (mSectionType == EMPTY_COMPONENT)
    {
        TPS_LOG_DEV_ERROR << "The section type is unknown.";
        return false;
    }
    IGraphicObjectPtr goMpr = nullptr;
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, mSectionType, mLocation);
    if (!CheckGraphicObject_i(imageUID, goType, goMpr))
    {
        TPS_LOG_DEV_ERROR << "Failed to check MPR graphic object of image: " << imageUID;
        return false;
    } 
    // crosshair GO
    IGraphicObjectPtr goCrosshair = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, mSectionType, mLocation);
    if (!CheckGraphicObject_i(imageUID, goType, goCrosshair))
    {
        TPS_LOG_DEV_ERROR << "Failed to check crosshair graphic object of image: " << imageUID;
        return false;
    }
    //// hotspot GO
    //IGraphicObjectPtr goHotSpots = nullptr;
    //if(!CheckGraphicObject_i(COMMON_BASE, GO_TYPE_HOT_SPOTS, goHotSpots))
    //{
    //    TPS_LOG_DEV_ERROR << "failed to get hot spots go!";
    //    return false;
    //}
    //auto hotSpots = std::dynamic_pointer_cast<HotSpotsGraphicObject>(goHotSpots);

    //// coldspot GO
    //if(!CheckGraphicObject_i(COMMON_BASE, GO_TYPE_COLD_SPOTS, goHotSpots))
    //{
    //    TPS_LOG_DEV_ERROR << "failed to get hot spots go!";
    //    return false;
    //}
    //auto coldSpots = std::dynamic_pointer_cast<HotSpotsGraphicObject>(goHotSpots);

    // RulerText GO
    IGraphicObjectPtr goMprRulerText = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(GALLERY_RULER_TEXT, mSectionType, mLocation);
    if (!CheckGraphicObject_i(imageUID, goType, goMprRulerText))
    {
        TPS_LOG_DEV_ERROR << "Failed to check MPR ruler text graphic object of image: " << imageUID;
        return false;
    }
    ////dummy graphic object
    //IGraphicObjectPtr goDummy = nullptr;
    //if (!CheckGraphicObject_i(boost::lexical_cast<std::string>(mWindowType),
    //    GO_TYPE_DUMMY, goDummy))
    //{
    //    TPS_LOG_DEV_ERROR << "Failed to check dummy graphic object.";
    //    return false;
    //}
    // Measure GO
    IGraphicObjectPtr goGalleryMeasure = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(GALLERY_MEASURE, mSectionType, mLocation);
    if (!CheckGraphicObject_i(imageUID, goType, goGalleryMeasure))
    {
        TPS_LOG_DEV_ERROR << "Failed to check measure graphic object of image: " << imageUID;
        return false;
    }
    // measureVal GO
    IGraphicObjectPtr goGalleryMeasureVal = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(GALLERY_MEASURE_VAL, mSectionType, mLocation);
    if (!CheckGraphicObject_i(imageUID, goType, goGalleryMeasureVal))
    {
        TPS_LOG_DEV_ERROR << "Failed to check measure value graphic object of image: " << imageUID;
        return false;
    }
    // Text GO
    IGraphicObjectPtr goGalleryText = nullptr;
    goType = GOTypeHelper::ComposeAsGOType(GALLERY_TEXT, mSectionType, mLocation);
    if (!CheckGraphicObject_i(imageUID, goType, goGalleryText))
    {
        TPS_LOG_DEV_ERROR << "Failed to check text graphic object of image: " << imageUID;
        return false;
    }

    //RTTI
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    auto mask = std::dynamic_pointer_cast<GraphicObjVolume>(goMask);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(goMpr);
    auto crosshair = std::dynamic_pointer_cast<CrosshairGraphicObject>(goCrosshair);
    auto mprRulerText = std::dynamic_pointer_cast<TpsTextOverlayGraphicObject>(goMprRulerText);
    //auto dummy = std::dynamic_pointer_cast<DummyGraphicObject>(goDummy);
    auto measure = std::dynamic_pointer_cast<GalleryMeasureGraphicObject>(goGalleryMeasure);
    auto measureVal = std::dynamic_pointer_cast<GalleryMeasureValGraphicObject>(goGalleryMeasureVal);
    auto text = std::dynamic_pointer_cast<GalleryTextGraphicObject>(goGalleryText);
    if (volume == nullptr 
        || mask == nullptr 
        || mpr == nullptr
        || crosshair == nullptr 
        || mprRulerText == nullptr 
        /*|| dummy == nullptr*/
        || measure == nullptr
        || measureVal == nullptr
        || text == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to dynamic cast IGraphicObjects.";
        return false;
    }     

    //Reset graphic objects of render strategy
    volume->SetDirty(true);
    mpr->SetDirty(true);

    if (m_spMprStrategy != nullptr)
    {
        m_spMprStrategy->SetVolumeGO(volume);
        m_spMprStrategy->SetMPRGO(mpr);
        //4DCT GO
        if (is4DSeries)
        {
            m_spMprStrategy->SetVolume4dGraphicObject(spVolume4dGO);
            m_spMprStrategy->Enable4DMode(true);
        }
        else
        {
           m_spMprStrategy->Enable4DMode(false);
        }
    }

    //crosshair strategy data
    if (m_spCrosshairRenderStrategy != nullptr)
    {
        m_spCrosshairRenderStrategy->SetCrosshairGraphicObject(crosshair);
    }

    //if (m_spMprHotSpotsRenderStrategy != nullptr)
    //{
    //    m_spMprHotSpotsRenderStrategy->SetMPRGraphicObject(mpr);
    //    m_spMprHotSpotsRenderStrategy->SetVolumeGraphicObject(volume);
    //    m_spMprHotSpotsRenderStrategy->SetHotSpotsGraphicObject(hotSpots);
    //    m_spMprHotSpotsRenderStrategy->SetColdSpotsGraphicObject(coldSpots);
    //}

    if (m_spScaleRulerStrategy != nullptr)
    {
        m_spScaleRulerStrategy->SetMprGraphicObject(mpr);
        m_spScaleRulerStrategy->SetVolumeGraphicObject(volume);
        m_spScaleRulerStrategy->SetRulerTextInfoGraphicObject(mprRulerText);
    }

    ////dummy render strategy
    //if (m_spDummyStrategy != nullptr)
    //{
    //    m_spDummyStrategy->SetDummyGraphicObject(dummy);
    //}

    //measure render strategy
    if(m_spMeasureAngleStrategy != nullptr)
    {
        m_spMeasureAngleStrategy->SetGalleryMeasureAngleGraphicObject(measure);
    }
    if(m_spMeasureDistanceStrategy != nullptr)
    {
        m_spMeasureDistanceStrategy->SetGalleryMeasureDistanceGraphicObject(measure);
    }
    if (m_spMeasureAngleValStrategy != nullptr)
    {
        m_spMeasureAngleValStrategy->SetMeasureValGraphicObject(measureVal);
    }

    //text render strategy
    if (m_spTextStrategy != nullptr)
    {
        m_spTextStrategy->SetTextGraphicObject(text);
    }

    return true;
}

void TpsGalleryMPRRenderController::RemoveDataSource_i(DATA_SOURCE_ID id)
{
    switch (id)
    {
    case IMAGE:
        if (m_spMprStrategy != nullptr)
        {
            m_spMprStrategy->SetVolumeGO(nullptr);
            m_spMprStrategy->SetMPRGO(nullptr);
            m_spMprStrategy->SetVolume4dGraphicObject(nullptr);
            m_spMprStrategy->Enable4DMode(false);
        }
        //crosshair strategy data
        if (m_spCrosshairRenderStrategy != nullptr)
        {
            m_spCrosshairRenderStrategy->SetCrosshairGraphicObject(nullptr);
        }
        //if (m_spMprHotSpotsRenderStrategy != nullptr)
        //{
        //    m_spMprHotSpotsRenderStrategy->SetMPRGraphicObject(nullptr);
        //    m_spMprHotSpotsRenderStrategy->SetVolumeGraphicObject(nullptr);

        //}
        if (m_spScaleRulerStrategy != nullptr)
        {
            m_spScaleRulerStrategy->SetMprGraphicObject(nullptr);
            m_spScaleRulerStrategy->SetVolumeGraphicObject(nullptr);
            m_spScaleRulerStrategy->SetRulerTextInfoGraphicObject(nullptr);
        }
        ////dummy render strategy
        //if (m_spDummyStrategy != nullptr)
        //{
        //    m_spDummyStrategy->SetDummyGraphicObject(nullptr);
        //}
        //measure render strategy
        if (m_spMeasureAngleStrategy != nullptr)
        {
            m_spMeasureAngleStrategy->SetGalleryMeasureAngleGraphicObject(nullptr);
        }
        if (m_spMeasureDistanceStrategy != nullptr)
        {
            m_spMeasureDistanceStrategy->SetGalleryMeasureDistanceGraphicObject(nullptr);
        }
        if (m_spMeasureAngleValStrategy != nullptr)
        {
            m_spMeasureAngleValStrategy->SetMeasureValGraphicObject(nullptr);
        }

        //text render strategy
        if (m_spTextStrategy != nullptr)
        {
            m_spTextStrategy->SetTextGraphicObject(nullptr);
        }
        break;
    default:
        break;
    }
}

// TODO now not use
bool TpsGalleryMPRRenderController::TransformPoint(
    const Mcsf::MedViewer3D::Point2D& ptIn, Mcsf::MedViewer3D::Point3D& ptOut)
{
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end())
    {
        TPS_LOG_DEV_ERROR << "The image data source is not ready.";
        return false;
    }

    std::string imageUID = mDataSourceMap[IMAGE];
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
	GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, mSectionType);
    std::string key = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(key);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The mpr graphic object is empty.";
        return false;
    }

    int width, height;
    mView->GetDisplaySize(width, height);
    double dRatioX = 1.0, dRatioY = 1.0;
    if (width >= height)
    {
        dRatioX = static_cast<double>(height) / width;
    }
    else
    {
        dRatioY = static_cast<double>(width) / height;
    }

    Point2D ptRes;
    ptRes.x = (ptIn.x - (1 - dRatioX) / 2.0) / dRatioX;
    ptRes.y = (ptIn.y - (1 - dRatioY) / 2.0) / dRatioY;

    Point3D ptInWorld;
    ptInWorld = mpr->GetWorldCoordinate(ptRes);

	std::string volumeKey = imageUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    if (volume == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get volume go!" << imageUID;
        return false;
    }
    Matrix4x4 matW2P = volume->GetWorld2Patient();
    ptOut = matW2P.Transform(ptInWorld);
    return true;
}

bool TpsGalleryMPRRenderController::TransformPoint(const Point3D& ptIn, Point2D& ptOut)
{
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end())
    {
        TPS_LOG_DEV_ERROR << "The image data source is not ready.";
        return false;
	}

	// mpr go
	std::string imageUID = mDataSourceMap[IMAGE];
	TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
	GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, mSectionType);
	std::string key = imageUID + "|" + GOTypeHelper::ToString(goType);
	auto go = mModelWarehouse->GetModelObject(key);
	auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
	if (mpr == nullptr)
    {
		TPS_LOG_DEV_ERROR << "The mpr graphic object is empty.";
		return false;
	}

	// volume go
	std::string volumeKey = imageUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
	auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
	auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
	if (volume == nullptr)
    {
		TPS_LOG_DEV_ERROR << "Failed to get volume go!" << imageUID;
		return false;
	}

	size_t dim[3];
	volume->GetDimension(dim);
	Mcsf::MedViewer3D::Matrix4x4 modelToVolume(dim[0], 0, 0, 0,
		0, dim[1], 0, 0,
		0, 0, dim[2], 0,
		-0.5, -0.5, -0.5, 1);
	Mcsf::MedViewer3D::Matrix4x4 matWorld2Volume = modelToVolume * volume->GetModel2World().Inverse();
	Mcsf::MedViewer3D::Matrix4x4 matPat2World = volume->GetWorld2Patient().Inverse();

	// 坐标映射
	Mcsf::MedViewer3D::Point3D center3D = matPat2World.Transform(ptIn);
	Mcsf::MedViewer3D::Point3D pt3dCrosshairCenterInVolume = matWorld2Volume.Transform(center3D);
	double dSliceX = pt3dCrosshairCenterInVolume.x;
	double dSliceY = pt3dCrosshairCenterInVolume.y;
	double dSliceZ = pt3dCrosshairCenterInVolume.z;
	dSliceX = (int)(dSliceX + 0.5);
	dSliceY = (int)(dSliceY + 0.5);
	dSliceZ = (int)(dSliceZ + 0.5);
	center3D = matWorld2Volume.Inverse().Transform(Mcsf::MedViewer3D::Point3D(dSliceX, dSliceY, dSliceZ));
	Point2D ptRes;
	ptRes = mpr->GetPlaneCoordinate(center3D, false);
	//printf("res: %.2f, %.2f\n", ptRes.x, ptRes.y);
	// modify again
	int width, height;
	mView->GetDisplaySize(width, height);
	double dRatioX = 1.0, dRatioY = 1.0;
	if (width >= height)
    {
		dRatioX = static_cast<double>(height) / width;
	}
	else
    {
		dRatioY = static_cast<double>(width) / height;
	}

	ptOut.x = ptRes.x*dRatioX + (1 - dRatioX) / 2.0;
	ptOut.y = ptRes.y*dRatioY + (1 - dRatioY) / 2.0;
	//printf("out: %.2f, %.2f\n", ptOut.x, ptOut.y);
	return true;
}

bool TpsGalleryMPRRenderController::SetCrosshairHittedModeForGallery(int mode)
{
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end())
    {
        TPS_LOG_DEV_ERROR << "The image data source is not ready.";
        return false;
    }
    std::string strSeriesUid = mDataSourceMap[IMAGE];
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, mSectionType);
    std::string strKey = strSeriesUid + "|" + GOTypeHelper::ToString(goType);
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    auto go = mModelWarehouse->GetModelObject(strKey);
    auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(go);
    if (spCrosshairGO == nullptr)
    {
        TPS_LOG_DEV_ERROR << "pCrosshairGO is null.";
        printf("\nspCrosshairGO is null.");
        return false;
    }
    spCrosshairGO->SetHittedMode((CrosshairGraphicObject::HITTED_MODE)mode);
    return true;
}

bool TpsGalleryMPRRenderController::GetCurrentMprSlice(int& slice, int& startSliceNumber, int& totalSlice) {
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end()){
        TPS_LOG_DEV_ERROR<<"The image data source is not ready.";
        return false;
    }

    std::string imageUID = mDataSourceMap[IMAGE];

    TpsImage3DEntity* image = nullptr;
    mDataRepository->GetGalleryImageManager()->GetImage3DData(imageUID, &image);
    int startSliceZ = image->GetStartSliceNumber();

    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, mSectionType);
    std::string key = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(key);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The mpr graphic object is empty.";
        return false;
    }

    std::string volumeKey = imageUID + "|"+ GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    if (volume == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get volume go!" << imageUID;
        return false;
    }
    TpsMprHelper::PlaneTypeInVol planeType;
    TpsMprHelper::GetSliceNumber(volume, mpr, slice, totalSlice, planeType);
    startSliceNumber = planeType == TpsMprHelper::Z ? startSliceZ : 1;
    return true;
}

FIRST_POSTFIX_COMPONENT TpsGalleryMPRRenderController::GetSectionType()
{
    return mSectionType;
}



//bool TpsGalleryMPRRenderController::ResetSeries4D(const std::string& seriesUID)
//{
//    return ResetSeries4D_i(seriesUID);
//}
//
//// TODO now not use
//bool TpsGalleryMPRRenderController::ResetSeries4D_i(const std::string& seriesUID)
//{
//
//    if (mSectionType == EMPTY_COMPONENT)
//    {
//        TPS_LOG_DEV_ERROR << "The section type is unknown.";
//        return false;
//    }
//
//    IGraphicObjectPtr goVolume = nullptr;
//
//
//    if (!CheckGraphicObject_i(seriesUID, GO_TYPE_VOLUME, goVolume))
//    {
//        TPS_LOG_DEV_ERROR << "Failed to check volume graphic object of image: " << seriesUID;
//        return false;
//    }
//
//    IGraphicObjectPtr goMask = nullptr;
//    if (!CheckGraphicObject_i(seriesUID, GO_TYPE_MASK, goMask))
//    {
//        TPS_LOG_DEV_ERROR << "Failed to check mask volume graphic object of image: " << seriesUID;
//        return false;
//    }
//
//    IGraphicObjectPtr goMpr = nullptr;
//    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSectionType, mLocation);
//    if (!CheckGraphicObject_i(seriesUID, goType, goMpr)){
//        TPS_LOG_DEV_ERROR<<"Failed to check MPR graphic object of image: "<<seriesUID;
//        return false;
//    }
//
//    auto volume = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(goVolume);
//    auto mask = std::dynamic_pointer_cast<Mcsf::MedViewer3D::GraphicObjVolume>(goMask);
//    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(goMpr);
//
//    if (volume == nullptr || mask == nullptr || mpr == nullptr )
//    {
//        TPS_LOG_DEV_ERROR << "Failed to dynamic cast IGraphicObjects.";
//        return false;
//    }
//
//    mpr->SetDirty(true);
//    mView->Render(true);
//
//    m_spMPRStrategy->SetVolumeGO(volume, mask);
//    m_spMPRStrategy->SetMPRGO(mpr);
//    return true;
//}



void TpsGalleryMPRRenderController::SetSectionType_i()
{

    switch (mWindowType)
    {
        case WINDOW_GALLERY_MPR_AXIAL:
            mSectionType = AXIAL;
            break;
        case WINDOW_GALLERY_MPR_SAGITTAL:
            mSectionType = SAGITTAL;
            break;
        case WINDOW_GALLERY_MPR_CORONAL:
            mSectionType = CORONAL;
            break;
        default:
            mSectionType = EMPTY_COMPONENT;
            break;
    }

    // default Value
    mLocation = EMPTY_COMPONENT;
}

void TpsGalleryMPRRenderController::RemoveGraphicObjects(const std::string& seriesUID)
{
    if (seriesUID.empty())
    {
        return;
    }

    //check volume graphic object
    RemoveGraphicObject_i(seriesUID, GO_TYPE_GALLERY_VOLUME);
    //check mask volume graphic object
    RemoveGraphicObject_i(seriesUID, GO_TYPE_GALLERY_MASK);

    //check mpr graphic object
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, mSectionType, mLocation);
    RemoveGraphicObject_i(seriesUID, goType);

    // crosshair GO
    goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, mSectionType, mLocation);
    RemoveGraphicObject_i(seriesUID, goType);

    goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR_CORNER_INFO, mSectionType, mLocation);
    RemoveGraphicObject_i(seriesUID, goType);

    goType = GOTypeHelper::ComposeAsGOType(GALLERY_RULER_TEXT, mSectionType, mLocation);
    RemoveGraphicObject_i(seriesUID, goType);   

    goType = GOTypeHelper::ComposeAsGOType(GALLERY_MEASURE_VAL, mSectionType, mLocation);
    RemoveGraphicObject_i(seriesUID, goType);   

    goType = GOTypeHelper::ComposeAsGOType(GALLERY_MEASURE, mSectionType, mLocation);
    RemoveGraphicObject_i(seriesUID, goType);

    goType = GOTypeHelper::ComposeAsGOType(GALLERY_TEXT, mSectionType, mLocation);
    RemoveGraphicObject_i(seriesUID, goType);

    auto spSeriesGroup4d = mDataRepository->GetGalleryImageManager()->GetSeriesGroup4d(seriesUID);
    if (spSeriesGroup4d != nullptr)
    {
        RemoveGraphicObject_i(spSeriesGroup4d->m_strMainSeriesUid, GO_TYPE_GALLERY_VOLUME_D4);
        for(int i = 0; i < spSeriesGroup4d->m_vecSeriesUid.size(); ++i){
            RemoveGraphicObject_i(spSeriesGroup4d->m_vecSeriesUid[i], GO_TYPE_GALLERY_VOLUME);
        }
    }
}

//void TpsGalleryMPRRenderController::RemoveGraphicObject_i(const std::string& seriesUID, GO_TYPE goType)
//{
//    std::string goTypeStr = GOTypeHelper::ToString(goType);
//    std::string key = seriesUID + "|" + goTypeStr;
//    auto go = mModelWarehouse->GetModelObject(key);
//    if (nullptr != go)
//    {
//        mModelWarehouse->RemoveModelObject(key);
//    }
//}

bool TpsGalleryMPRRenderController::GetCrosshairProjectionPosition(float& x2D, float& y2D)
{
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end())
    {
        TPS_LOG_DEV_ERROR << "The image data source is not ready.";
        return false;
    }

    // mpr go
    std::string imageUID = mDataSourceMap[IMAGE];
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, mSectionType);
    std::string key = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(key);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The mpr graphic object is empty.";
        return false;
    }

    // volume go
    std::string volumeKey = imageUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    if (volume == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get volume go!" << imageUID;
        return false;
    }

    // crosshair go
    goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, mSectionType);
    std::string strKey = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto goCrosshair = mModelWarehouse->GetModelObject(strKey);
    auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(goCrosshair);
    if (spCrosshairGO == nullptr)
    {
        TPS_LOG_DEV_ERROR << "[ERROR][CrosshairMovementOperation][ModifyGraphicObject]: "
            << "Failed to dynamic cast IGraphicObjects.";
        return false;
    }

    // world2volume
    size_t dim[3];
    volume->GetDimension(dim);
    Mcsf::MedViewer3D::Matrix4x4 modelToVolume(dim[0], 0, 0, 0,
        0, dim[1], 0, 0,
        0, 0, dim[2], 0,
        -0.5, -0.5, -0.5, 1);
    Mcsf::MedViewer3D::Matrix4x4 matWorld2Volume = modelToVolume * volume->GetModel2World().Inverse();

    Mcsf::MedViewer3D::Point3D center3D;
    spCrosshairGO->GetCrosshairPoint3d(&center3D); // in world
    Mcsf::MedViewer3D::Point3D pt3dCrosshairCenterInVolume = matWorld2Volume.Transform(center3D);
    double dSliceX = pt3dCrosshairCenterInVolume.x;
    double dSliceY = pt3dCrosshairCenterInVolume.y;
    double dSliceZ = pt3dCrosshairCenterInVolume.z;
    dSliceX = (int)(dSliceX + 0.5);
    dSliceY = (int)(dSliceY + 0.5);
    dSliceZ = (int)(dSliceZ + 0.5);
    center3D = matWorld2Volume.Inverse().Transform(Mcsf::MedViewer3D::Point3D(dSliceX, dSliceY, dSliceZ));
    Point2D ptRes;
    ptRes = mpr->GetPlaneCoordinate(center3D, false);
    //printf("res: %.2f, %.2f\n", ptRes.x, ptRes.y);
    // modify again
    int width, height;
    mView->GetDisplaySize(width, height);
    double dRatioX = 1.0, dRatioY = 1.0;
    if (width >= height)
    {
        dRatioX = static_cast<double>(height) / width;
    }
    else
    {
        dRatioY = static_cast<double>(width) / height;
    }

    x2D = ptRes.x*dRatioX + (1 - dRatioX) / 2.0;
    y2D = ptRes.y*dRatioY + (1 - dRatioY) / 2.0;

    return true;
}

//获取是直线上下左右的四个点，不一定是终点。
//x1,y1,x2,y2 是y上的两个点，x3,y3,x4,y4是x上的两点。
bool TpsGalleryMPRRenderController::GetCrossXYPoints(double& x1, double& y1, double& x2, double& y2, double& x3, double& y3, double& x4, double& y4)
{
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end())
    {
        TPS_LOG_DEV_ERROR << "The image data source is not ready.";
        return false;
    }

    int width, height;
    mView->GetDisplaySize(width, height);
    if (width <= 0 || height <= 0)
    {
        TPS_LOG_DEV_ERROR << "The view size is not correct.";
        return false;
    }

    // mpr go
    std::string imageUID = mDataSourceMap[IMAGE];
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, mSectionType);
    std::string key = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(key);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The mpr graphic object is empty.";
        return false;
    }

    // volume go
    std::string volumeKey = imageUID + "|" + GOTypeHelper::ToString(GO_TYPE_GALLERY_VOLUME);
    auto goVolume = mModelWarehouse->GetModelObject(volumeKey);
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goVolume);
    if (volume == nullptr)
    {
        TPS_LOG_DEV_ERROR << "Failed to get volume go!" << imageUID;
        return false;
    }

    // crosshair go
    goType = GOTypeHelper::ComposeAsGOType(GALLERY_CROSSHAIR, mSectionType);
    std::string strKey = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto goCrosshair = mModelWarehouse->GetModelObject(strKey);
    auto spCrosshairGO = std::dynamic_pointer_cast<CrosshairGraphicObject>(goCrosshair);
    if (spCrosshairGO == nullptr)
    {
        TPS_LOG_DEV_ERROR << "[ERROR][CrosshairMovementOperation][ModifyGraphicObject]: "
            << "Failed to dynamic cast IGraphicObjects.";
        return false;
    }

    Mcsf::MedViewer3D::Point3D point1, point2, point3, point4;
    Mcsf::MedViewer3D::Point2D pt1, pt2, pt3, pt4;
    spCrosshairGO->GetCrossFourPoints(&point1, &point2, &point3, &point4);
    pt1 = mpr->GetPlaneCoordinate(point1, false);
    pt2 = mpr->GetPlaneCoordinate(point2, false);
    pt3 = mpr->GetPlaneCoordinate(point3, false);
    pt4 = mpr->GetPlaneCoordinate(point4, false);

    //printf("res: %.2f, %.2f\n", ptRes.x, ptRes.y);
    // modify again
    double dRatioX = 1.0, dRatioY = 1.0;
    if (width >= height)
    {
        dRatioX = static_cast<double>(height) / width;
    }
    else
    {
        dRatioY = static_cast<double>(width) / height;
    }

    x1 = pt1.x*dRatioX + (1 - dRatioX) / 2.0;
    y1 = pt1.y*dRatioY + (1 - dRatioY) / 2.0;
    x2 = pt2.x*dRatioX + (1 - dRatioX) / 2.0;
    y2 = pt2.y*dRatioY + (1 - dRatioY) / 2.0;
    x3 = pt3.x*dRatioX + (1 - dRatioX) / 2.0;
    y3 = pt3.y*dRatioY + (1 - dRatioY) / 2.0;
    x4 = pt4.x*dRatioX + (1 - dRatioX) / 2.0;
    y4 = pt4.y*dRatioY + (1 - dRatioY) / 2.0;
    return true;
}

void TpsGalleryMPRRenderController::HideNonImageOverlay(bool isToHide)
{
    std::vector<TpsRSPtr> nonImageRSList;
    for (auto it = mStrategyList.begin(); it != mStrategyList.end(); ++it)
    {
        if (it->first == ClsView::IMAGE) continue;
        for (int i = 0; i < it->second.size(); ++i)
        {
            if (it->second[i] == nullptr) continue;
            nonImageRSList.push_back(it->second[i]);
        }
    }

    if (!nonImageRSList.empty()) this->SynchronizeRenderThread();
    for (int i = 0; i < nonImageRSList.size(); ++i)
    {
        nonImageRSList[i]->SetIsVisible(!isToHide);
    }
}

bool TpsGalleryMPRRenderController::GetMprViewDirecrion(Mcsf::MedViewer3D::Vector3D &vectorView, Mcsf::MedViewer3D::Vector3D &vectotUp) 
{
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end())
    {
        TPS_LOG_DEV_ERROR << "The image data source is not ready.";
        return false;
    }

    std::string imageUID = mDataSourceMap[IMAGE];
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, mSectionType);
    std::string key = imageUID + "|" + GOTypeHelper::ToString(goType);
    auto go = mModelWarehouse->GetModelObject(key);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    if (mpr == nullptr)
    {
        TPS_LOG_DEV_ERROR << "The mpr graphic object is empty.";
        return false;
    }

    mpr->GetViewDirection(&vectotUp, &vectorView);
    return true;
}

//根据windowType 和windowuid 来重置MPR底图
bool TpsGalleryMPRRenderController::ResetMPRObject()
{
    TpsSafeLock<ModelWarehouse> lock(mModelWarehouse);
    if (mDataSourceMap.find(IMAGE) == mDataSourceMap.end())
    {
        TPS_LOG_DEV_ERROR << "The image data source is not ready.";
        return false;
    }
    std::string strSeriesUid = mDataSourceMap[IMAGE];
    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(GALLERY_MPR, mSectionType, mLocation);
    std::string goTypeStr = GOTypeHelper::ToString(goType);
    std::string key = strSeriesUid + "|" + goTypeStr;
    auto go = mModelWarehouse->GetModelObject(key);
    auto spMprGO = std::dynamic_pointer_cast<MPRGraphicObject>(go);
    auto converter = mGOConverterFactory->GetConverter(goType);

    if (converter == nullptr)
    {
        TPS_LOG_DEV_ERROR << "[ERROR][TpsMPRRenderController][ResetMPRObject]: "
            << "Failed to get graphic object converter which type is: " << goType;
        return false;
    }
    if (!converter->Update(go))
    {
        TPS_LOG_DEV_ERROR << "[ERROR][TpsMPRRenderController][ResetMPRObject]: "
            << "Failed to update RT data to graphic object which key is: " << strSeriesUid;
        return false;
    }

    spMprGO->SetDirty(true);
    return true;
}

TPS_END_NAMESPACE  // end namespace tps
