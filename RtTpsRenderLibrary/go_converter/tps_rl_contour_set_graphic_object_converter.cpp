#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_contour_set_graphic_object_converter.h"
#include "RtTpsRenderLibrary/tps_rl_contour_set_graphic_object.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsFramework/tps_fw_contour_set.h"
#include "RtTpsFramework/tps_fw_contour.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_signed_distance_field.h"
#include "RtTpsFramework/tps_fw_composable_graphic_object.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsROIUtility/tps_ru_roi_shape_converter.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

using namespace Mcsf::MedViewer3D;

#define ONLY_PRIMARY_CONVERTION

TPS_BEGIN_NAMESPACE

ContourSetGOConverter::ContourSetGOConverter(GO_TYPE goType) : TpsGOConverterBase(goType)
{

}

ContourSetGOConverter::~ContourSetGOConverter()
{

}

GraphicObjectPtr ContourSetGOConverter::CreateGraphicObject_i()
{
    return std::shared_ptr<ContourSetGraphicObject>(new ContourSetGraphicObject());
}

bool ContourSetGOConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    mROI = mDataRepository->GetVoiManager()->GetROI(para.obj_uid);
    mSection = para.section;
    return mROI != nullptr;
}

bool ContourSetGOConverter::Update_i(GraphicObjectPtr go)
{
    auto csGO = std::dynamic_pointer_cast<ContourSetGraphicObject>(go);
    if (csGO == nullptr) return false;

    std::string imageUID = mROI->GetInnerRoi()->get_seriesuid();
    auto goTmp = mModelWarehouse->GetModelObject(
        imageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME));
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goTmp);
    if (volume == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get volume graphic object of image: "<<imageUID;
        return false;
    }

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSection);
    std::string mprKey = imageUID + "|" + GOTypeHelper::ToString(goType);
    goTmp = mModelWarehouse->GetModelObject(mprKey);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(goTmp);
    if (mpr == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get mpr graphic object with uid: "<<mprKey;
        return false;
    }
    Vector3D normal;
    mpr->GetNormal(&normal);
    auto matW2P = volume->GetWorld2Patient();
    normal = matW2P.Transform(normal);

    auto srs_cs = mROI->GetContourSet();
    auto cs = csGO->GetContourSet();
    if (srs_cs->normal.IsParallel2(normal))
    {
        *cs = *srs_cs;
    }
    else
    {
        auto sdf = mROI->GetSDF();
        auto matPat2SDF = sdf->GetPatToSDF();
        auto nv = matPat2SDF.Transform(normal);
        nv.Normalize();
        int axis = -1;
        for (int i = 0; i < 3; ++i)
        {
            if (fabs(fabs(Get(nv, i)) - 1.0) <= 1e-6)
            {
                axis = i;
                break;
            }
        }
        if (axis == -1)
        {
            //oblique case
        }
        else
        {
            ROIShapeConverter::SDFToContourSet(*sdf, axis, *cs);
        }
    }
    return true;
}

CSGOCollectionConverter::CSGOCollectionConverter(
    GO_TYPE goType, ContourSetGOConverter* converter) : 
    TpsGOConverterBase(goType), mCSGOConverter(converter)
{

}

CSGOCollectionConverter::~CSGOCollectionConverter()
{
    TPS_DEL_PTR(mCSGOConverter);
}

void CSGOCollectionConverter::SetModelWarehouse(ModelWarehouse* modelWarehouse)
{
    mModelWarehouse = modelWarehouse;
    if (mCSGOConverter)
    {
        mCSGOConverter->SetModelWarehouse(modelWarehouse);
    }
}

void CSGOCollectionConverter::SetDataRepository(ITpsDataRepository* dataRepository)
{
    mDataRepository = dataRepository;
    if (mCSGOConverter)
    {
        mCSGOConverter->SetDataRepository(dataRepository);
    }
}

GraphicObjectPtr CSGOCollectionConverter::CreateGraphicObject_i()
{
    return std::shared_ptr<ComposableGraphicObject>(new ComposableGraphicObject());
}

bool CSGOCollectionConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end()) return false;

    mROIList = mDataRepository->GetVoiManager()->GetROIBySeries(it->second);
    mSection = para.section;
    return true;
}

bool CSGOCollectionConverter::Update_i(GraphicObjectPtr go)
{
    auto composableGO = std::dynamic_pointer_cast<ComposableGraphicObject>(go);
    if (composableGO == nullptr) return false;

    if (mROIList.empty())
    {
        composableGO->DecomposeAll();
        return true;
    }

#ifdef ONLY_PRIMARY_CONVERTION
    ROIEntity* roi = mROIList[0];
    std::string imageUID = roi->GetInnerRoi()->get_seriesuid();
    auto goTmp = mModelWarehouse->GetModelObject(
        imageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME));
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goTmp);
    if (volume == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get volume graphic object of image: "<<imageUID;
        return false;
    }

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, mSection);
    std::string mprKey = imageUID + "|" + GOTypeHelper::ToString(goType);
    goTmp = mModelWarehouse->GetModelObject(mprKey);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(goTmp);
    if (mpr == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get mpr graphic object with uid: "<<mprKey;
        return false;
    }
    Vector3D normal;
    mpr->GetNormal(&normal);
    auto matW2P = volume->GetWorld2Patient();
    normal = matW2P.Transform(normal);

    auto srs_cs = roi->GetContourSet();
    if (!srs_cs->normal.IsParallel2(normal))
    {
        composableGO->DecomposeOne(mSection);
        return true;
    }
#endif

    auto components = composableGO->GetComponentsMap();
    auto it = components.find(mSection);
    std::shared_ptr<CSGOCollection> csCollGO = nullptr;
    if (it != components.end())
    {
        csCollGO = std::dynamic_pointer_cast<CSGOCollection>(it->second);
    }
    if (csCollGO == nullptr)
    {
        csCollGO.reset(new CSGOCollection());
        composableGO->ComposeOne(mSection, csCollGO);
    }
    csCollGO->Clear();

    GOConverterParameter para;
    para.section = mSection;
    CSGOPtr cs_go = nullptr;
    for (int i = 0; i < mROIList.size(); ++i)
    {
        para.obj_uid = mROIList[i]->GetUid();
        cs_go = std::dynamic_pointer_cast<ContourSetGraphicObject>(
            mCSGOConverter->Convert(para));
        if (cs_go == nullptr)
        {
            TPS_LOG_DEV_ERROR<<"Failed to convert contour set graphic object.";
            return false;
        }
        csCollGO->AddContourSet(mROIList[i]->GetUid(), cs_go);
    }
    if (cs_go)
    {
        csCollGO->SetNormal(cs_go->GetContourSet()->normal);
    }
    return true;
}

TPS_END_NAMESPACE