////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_graphicobjectconverterfactory.cpp
/// 
///  \brief class TpsGOConverterFactory implementation 
/// 
///  \version 1.0
/// 
///  \date    2013/12/18
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjectconverterfactory.h"

//graphic object converters
#include "RtTpsRenderLibrary/tps_rl_volumegraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_volume4d_graphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_crosshair_graphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_beamcollectiongraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_beamtemplategraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_dosegraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_ct2density_graphicobjectcollectionconverter.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobejctcollectionconverter.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobejctconverter.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollectionconverter.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_goconverter.h"
#include "RtTpsRenderLibrary/tps_rl_dosegridboundarygraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_wedgegraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_cornerinfographicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_rulertextgraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollectionconverter.h"
#include "RtTpsRenderLibrary/tps_rl_fusiongraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_doselineobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_easyplandrrgraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_easyplanmeasuregraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_tablelineobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_chess_graphic_object_converter.h"
#include "RtTpsRenderLibrary/tps_rl_beam_view_camera_graphic_object_converter.h"
#include "RtTpsRenderLibrary/tps_rl_drr_graphic_object_converter.h"
#include "RtTpsRenderLibrary/tps_rl_applicator_graphic_object_converter.h"
#include "RtTpsRenderLibrary/tps_rl_block_graphic_object_converter.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_beamcollectiongraphicobjectconverter.h"
//#include "RtTpsRenderLibrary/tps_rl_bev_imagetext_graphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_surfacecamera_graphicobject_converter.h"
#include "RtTpsRenderLibrary/tps_rl_dose_mesh_gocollectionconverter.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrcollection_graphicobject_converter.h"

#include "RtTpsRenderLibrary/tps_rl_gallery_measuregraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_measurevaluegraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_camera_graphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_volumegraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_mprgraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_cornerinfographicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_crosshair_graphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_textgraphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_vr_graphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_gallery_colortable_graphicobjectconverter.h"
#include "RtTpsRenderLibrary/tps_rl_drrsettinggocollectionconverter.h"
#include "RtTpsRenderLibrary/tps_rl_series_table_cache_graphic_object_converter.h"

//graphic object for common converter
#include "RtTpsRenderLibrary/tps_rl_nudgeshapegraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_editsetuppoigraphicobject.h"
//#include "RtTpsRenderLibrary/tps_rl_bevwindowdisplaygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_surface_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_landmarkgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_manualremovetablegraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_hot_spots_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_dosegridboundarygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_statecontrol_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_contour_set_graphic_object_converter.h"
#include "RtTpsRenderLibrary/tps_rl_dosegridboundarygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_dummygraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_easyplan_drrgraphicobject.h"

//graphic object type defines
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"


TPS_BEGIN_NAMESPACE   // begin namespace tps

TpsGOConverterFactory::TpsGOConverterFactory(){

}

TpsGOConverterFactory::~TpsGOConverterFactory(){

}

TpsGOConverterBase* TpsGOConverterFactory::CreateConverter_i(GO_TYPE goType){

    TpsGOConverterBase* converter = nullptr;
    switch (goType) {
    case GO_TYPE_VOLUME:
        converter = new TpsVolumeGOConverter(goType);
        break;
    case GO_TYPE_VOLUME_D4:
        converter = new TpsVolume4dGOConverter(goType);
        break;
    case GO_TYPE_MASK:
        converter = new TpsMaskGOConverter(goType);
        break;
    case GO_TYPE_MPR_CORONAL:
    case GO_TYPE_MPR_SAGITTAL:
    case GO_TYPE_MPR_AXIAL:
    case GO_TYPE_MPR_AXIAL_PREV:
    case GO_TYPE_MPR_AXIAL_POST:  	
    case GO_TYPE_MPR_AXIAL_VOI_PREVIEW:
    case GO_TYPE_MPR_CORONAL_VOI_PREVIEW:
    case GO_TYPE_MPR_SAGITTAL_VOI_PREVIEW:
    case GO_TYPE_MPR_CORONAL_BEAM_PREVIEW:
    case GO_TYPE_MPR_SAGITTAL_BEAM_PREVIEW:
    case GO_TYPE_MPR_AXIAL_BEAM_PREVIEW:
        converter = new TpsMPRGOConverter(goType);
        break;
    case GO_TYPE_CROSSHAIR_CORONAL:
    case GO_TYPE_CROSSHAIR_SAGITTAL:
    case GO_TYPE_CROSSHAIR_AXIAL:
        converter = new TpsCrosshairGOConverter(goType);
        break;
    case GO_TYPE_BEAM_COLLECTION:
        converter = new TpsBeamCollectionGOConverter(goType, 
            new TpsBeamGOConverter(GO_TYPE_BEAM, 
            new TpsWedgeGOConverter(GO_TYPE_WEDGE),
            new ApplicatorGOConverter(GO_TYPE_APPLICATOR), 
            new BlockGOConverter(GO_TYPE_APERTURE_BLOCK),
            new BlockGOConverter(GO_TYPE_SHIELDING_BLOCK)));
        break;
    case GO_TYPE_POI_COLLECTION:
        converter = new PoiGraphicObjectCollectionConverter(goType, 
            new PoiGraphicObjectConverter(GO_TYPE_POI));
        break;
    case GO_TYPE_POI:
        converter = new PoiGraphicObjectConverter(goType);
        break;
    case GO_TYPE_MAX_POINT_COLLECTION:
        converter = new MaxPointGraphicObjectCollectionConverter(goType, 
            new MaxPointGraphicObjectConverter(GO_TYPE_MAX_POINT));
        break;
    case GO_TYPE_MAX_POINT:
        converter = new MaxPointGraphicObjectConverter(goType);
        break;
    case GO_TYPE_BEAM:
        converter = new TpsBeamGOConverter(goType, 
            new TpsWedgeGOConverter(GO_TYPE_WEDGE), 
            new ApplicatorGOConverter(GO_TYPE_APPLICATOR), 
            new BlockGOConverter(GO_TYPE_APERTURE_BLOCK),
            new BlockGOConverter(GO_TYPE_SHIELDING_BLOCK));
        break;
    case GO_TYPE_VOI_COLLECTION:
        //TpsVOIGOConverter* subconvert = new TpsVOIGOConverter(GO_TYPE_VOI);
        //if (subconvert!=nullptr) {
        //    subconvert->SetDataRepository(mDataRepository);
        //    subconvert->SetModelWarehouse(mModelWarehouse);
        //}
        converter = new TpsVOIGOCollectionConverter(goType, new TpsVOIGOConverter(GO_TYPE_VOI));
        break;
    case GO_TYPE_VOI_COLLECTION_PREVIEW:
        converter = new TpsGOCommonConverter<TpsVOIGOCollection>(goType);
        break;
    case GO_TYPE_VOI:
        converter = new TpsVOIGOConverter(goType);
        break;
    case GO_TYPE_SHADEDSURFCE:
        converter = new ShadedVoiGraphicObjectConverter(goType);
        break;
    case GO_TYPE_DOSE_MESH_COLLECTION:
        converter = new DoseMeshGraphicObjectCollectionConverter(goType);
        break;
    case GO_TYPE_ROI_CONTOUR_SET:
        converter = new ContourSetGOConverter(goType);
        break;
    case GO_TYPE_ROI_CONTOUR_SET_COLLECTION:
        converter = new CSGOCollectionConverter(goType, 
            new ContourSetGOConverter(GO_TYPE_ROI_CONTOUR_SET));
        break;
    case GO_TYPE_DRR:
        converter = new DRRGOConverter(goType);
        break;
    case GO_TYPE_CT2DENSITYCollection:
        converter = new Ct2DensityGOCollectionConverter(goType);
        break;
    case GO_TYPE_DOSE_CORONAL:
    case GO_TYPE_DOSE_SAGITTAL:
    case GO_TYPE_DOSE_AXIAL:
    case GO_TYPE_DOSE_AXIAL_PREV:
    case GO_TYPE_DOSE_AXIAL_POST:
        converter = new TpsDoseGOConverter(goType);
        break;
    case GO_TYPE_SETUP_POI:
        converter = new TpsGOCommonConverter<EditSetupPtGraphicObject>(goType);
        break;
    case GO_TYPE_NUDGE_CIRCLE:
        converter = new TpsGOCommonConverter<TpsNudgeShapeGraphicObject>(goType);
        break;
    case GO_TYPE_DOSE_GRID_BOUNDARY:
        converter = new TpsDoseGridBoundaryGOConverter(goType);
        break;
    case GO_TYPE_WEDGE:
        converter = new TpsWedgeGOConverter(goType);
        break;
    case GO_TYPE_VR:       
        converter = new TpsGOCommonConverter<ShadedSurfaceGraphicObject>(goType);
        break;
    case GO_TYPE_FUSION_CHESS_CORONAL:
    case GO_TYPE_FUSION_CHESS_SAGITTAL:
    case GO_TYPE_FUSION_CHESS_AXIAL:
        converter = new ChessGoConverter(goType);
        break;
//     case GO_TYPE_BEV_DISPLAY:
//         converter = new TpsGOCommonConverter<BEVDisplayGraphicObject>(goType);
//         break;
    case GO_TYPE_NORMALGROUP_COLLECTION:
        converter = new NormalGroupCollectionConverter(goType, 
            new TpsBeamCollectionGOConverter(GO_TYPE_BEAM_COLLECTION, 
            new TpsBeamGOConverter(GO_TYPE_BEAM, 
            new TpsWedgeGOConverter(GO_TYPE_WEDGE),
            new ApplicatorGOConverter(GO_TYPE_APPLICATOR), 
            new BlockGOConverter(GO_TYPE_APERTURE_BLOCK),
            new BlockGOConverter(GO_TYPE_SHIELDING_BLOCK))));
        break;
    case GO_TYPE_LANDMARK:
        converter = new TpsGOCommonConverter<LandmarkGraphicObject>(goType);
        break;
    case GO_TYPE_LIMIT_REGION:
    case GO_TYPE_FUSION_REGION_FIXED:
    case GO_TYPE_FUSION_REGION_FLOAT:
        converter = new TpsGOCommonConverter<TpsDoseGridBoundaryGraphicObject>(goType);
        break;
    case GO_TYPE_MPR_CORNERINFO_CORONAL:
    case GO_TYPE_MPR_CORNERINFO_SAGITTAL:
    case GO_TYPE_MPR_CORNERINFO_AXIAL:
    case GO_TYPE_MPR_CORNERINFO_AXIAL_PREV:
    case GO_TYPE_MPR_CORNERINFO_AXIAL_POST:
         converter = new TpsCornerInfoGOConverter(goType);
        break;
    case GO_TYPE_MPR_RULERTEXT_CORONAL:
    case GO_TYPE_MPR_RULERTEXT_SAGITTAL:
    case GO_TYPE_MPR_RULERTEXT_AXIAL:
    case GO_TYPE_GALLERY_RULE_TEXT_CORONAL:
    case GO_TYPE_GALLERY_RULE_TEXT_SAGITTAL:
    case GO_TYPE_GALLERY_RULE_TEXT_AXIAL:
    case GO_TYPE_MPR_RULERTEXT_AXIAL_PREV:
    case GO_TYPE_MPR_RULERTEXT_AXIAL_POST:
        converter = new TpsRulerTextGOConverter(goType);
        break;
//     case GO_TYPE_BEV_IMAGE_TEXT:
//         converter = new BevImageTextGOConverter(goType);
//         break;
    case GO_TYPE_MANUL_REMOVE_TABLE:
        converter = new TpsTableLineGOConverter(goType);
        break;
    case GO_TYPE_HOT_SPOTS:
    case GO_TYPE_COLD_SPOTS:
        converter = new TpsGOCommonConverter<HotSpotsGraphicObject>(goType);
        break;
    case GO_TYPE_FUSION:
        converter = new FusionGoConverter(goType);
        break;
    case GO_TYPE_VARYING_FUSION_GO:
        converter = new VaryingFusionGoConverter(goType);
        break;
    case GO_TYPE_BEV_DISPLAY_CONTROL:
        converter = new TpsGOCommonConverter<StateControlGraphicObject>(goType);
        break;
    case GO_TYPE_DUMMY:
        converter = new TpsGOCommonConverter<DummyGraphicObject>(goType);
        break;
    case GO_TYPE_BEV_CAMERA:
        converter = new BeamViewCameraGoConverter(goType);
        break;
    case GO_TYPE_SURFACE_CAMERA:
        converter = new SurfaceCameraGraphicObjectConverter(goType);
        break;
    case GO_TYPE_DOSE_LINE_SETTING:
        converter = new TpsDoseLineGOConverter(goType);
        break;
    case GO_TYPE_EASYPLAN_DRR:
        converter = new TpsEasyPlanDrrGOConverter(goType);
        break;
    case GO_TYPE_RT_IMAGE_COLLECTION:
        converter = new TpsEasyPlanDrrCollectionGOConverter(goType, new TpsEasyPlanDrrGOConverter(GO_TYPE_EASYPLAN_DRR));
        break;
    case GO_TYPE_BEAM_TEMPLATE_PREVIEW:
        converter = new TpsBeamTemplateGraphicObjectConverter(goType);
        break;
    case GO_TYPE_EASYPLAN_MEASURE:
        converter = new TpsEasyPlanDRRMeasureGOConverter(goType);
        break;
    case GO_TYPE_EASY_PLAN_BEAM_COLLECTION:
        converter = new TpsEasyPlanBeamCollectionGOConverter(goType);
        break;
    case GO_TYPE_GALLERY_VOLUME:
        converter = new TpsGalleryVolumeGOConverter(goType);
        break;
    case GO_TYPE_GALLERY_VOLUME_D4:
        converter = new TpsVolume4dGOConverter(goType);
        break;
    case GO_TYPE_GALLERY_MASK:
        converter = new TpsGalleryMaskGOConverter(goType);
        break;
    case GO_TYPE_GALLERY_MPR_CORONAL:
    case GO_TYPE_GALLERY_MPR_SAGITTAL:
    case GO_TYPE_GALLERY_MPR_AXIAL:
        converter = new TpsGalleryMPRGOConverter(goType);
        break;
    case GO_TYPE_GALLERY_MPR_CORNERINFO_CORONAL:
    case GO_TYPE_GALLERY_MPR_CORNERINFO_SAGITTAL:
    case GO_TYPE_GALLERY_MPR_CORNERINFO_AXIAL:
        converter = new TpsGalleryCornerInfoGOConverter(goType);
        break;
    case GO_TYPE_GALLERY_CROSSHAIR_CORONAL:
    case GO_TYPE_GALLERY_CROSSHAIR_SAGITTAL:
    case GO_TYPE_GALLERY_CROSSHAIR_AXIAL:
        converter = new TpsGalleryCrosshairGOConverter(goType);
        break;
    case GO_TYPE_GALLERY_MEASURE_CORONAL:
    case GO_TYPE_GALLERY_MEASURE_SAGITTAL:
    case GO_TYPE_GALLERY_MEASURE_AXIAL:
        converter = new TpsGalleryMeasureGOConverter(goType);
        break;
    case GO_TYPE_GALLERY_MEASURE_VAL_CORONAL:
    case GO_TYPE_GALLERY_MEASURE_VAL_SAGITTAL:
    case GO_TYPE_GALLERY_MEASURE_VAL_AXIAL:
        converter = new TpsGalleryMeasureValGOConverter(goType);
        break;

    case GO_TYPE_GALLERY_SURFACE_CAMERA:
        converter = new TpsGalleryCameraGoConverter(goType);
        break;

    case GO_TYPE_GALLERY_TEXT_CORONAL:
    case GO_TYPE_GALLERY_TEXT_SAGITTAL:
    case GO_TYPE_GALLERY_TEXT_AXIAL:
        converter = new TpsGalleryTextGOConverter(goType);
        break;

    case GO_TYPE_GALLERY_VR:
        converter = new TpsGalleryVRGOConverter(goType);
        break;
    case GO_TYPE_GALLERY_COLORTABLE:
        converter = new TpsGalleryColorTableGOConverter(goType);
        break;

    case GO_TYPE_APPLICATOR:
        converter = new ApplicatorGOConverter(goType);
        break;
    case GO_TYPE_APERTURE_BLOCK:
    case GO_TYPE_SHIELDING_BLOCK:
        converter = new BlockGOConverter(goType);
        break;
    case  GO_TYPE_DRRSETTING_COLLECTION:
        converter = new TpsDrrSettingGOCollectionConverter(goType);
        break;
    case GO_TYPE_SERIES_TABLE_CACHE_CORONAL:
    case GO_TYPE_SERIES_TABLE_CACHE_SAGITTAL:
    case GO_TYPE_SERIES_TABLE_CACHE_AXIAL:
        converter = new SeriesTableCacheConverter(goType);
        break;
    default:
        TPS_LOG_DEV_ERROR<<"Converter to graphic object: "
            <<goType<<" does not exist.";
    }

    if(nullptr != converter){
        converter->SetDataRepository(mDataRepository);
        converter->SetModelWarehouse(mModelWarehouse);
        mConverterMap[goType] = converter;
    }
    return converter;
}

TPS_END_NAMESPACE  // end namespace tps
