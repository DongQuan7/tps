#include "StdAfx.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_edit_roi_via_new_contour.h"

#include "RtTpsCertifiedLib/rt_tps_certifiedsmartcontour.h"
#include "RtTpsCertifiedLib/rt_tps_certifiedvoi.h"

#include "RtTpsDataAccess/tps_da_voimanager.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsRenderController/tps_rc_renderproxy.h"

#include "RtTpsFramework/tps_fw_roi_entity.h"
#include "RtTpsFramework/tps_fw_contour_set.h"
#include "RtTpsFramework/tps_fw_signed_distance_field.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
#include "RtTpsROIUtility/tps_ru_roi_shape_converter.h"
#include "RtTpsROIUtility/tps_ru_polygon_utility.h"
#include "RtTpsBusinessLogic/tps_bl_save_objects.h"
#include "RtTpsDataAccess/tps_da_entity_converter.h"

TPS_BEGIN_NAMESPACE

EditROIViaContourCmd::EditROIViaContourCmd(CertifiedVOIEditInfo& certifiedEditVOI) 
    : TpsVOIRedoUndoCacheCmd(certifiedEditVOI.mVOIUid)
    , mCertifiedSmartContour(certifiedEditVOI), mCertVOI(new CertifiedVOI())
{
}


EditROIViaContourCmd::~EditROIViaContourCmd(void) 
{
    if (mCertVOI) delete mCertVOI;
}

TpsRedoUndoCommand* EditROIViaContourCmd::Clone() 
{
    EditROIViaContourCmd* cmd = new EditROIViaContourCmd(mCertifiedSmartContour);
    cmd->SetDatabaseWrapper(mDatabaseWrapper);
    cmd->SetDataRepository(mDataRepository);
    cmd->SetLoadObjects(mLoadObjects);
    cmd->SetSaveObjects(mSaveObjects);
    cmd->SetRenderProxy(mRenderProxy);
    cmd->mResponser = mResponser;
    return cmd;
}

int EditROIViaContourCmd::Execute() 
{
    TPS_LOG_DEV_INFO<<"[INFO][EditROIViaContourCmd][Execute]: "<<"EditROIViaContourCmd execute!";

    string voiUid = mCertifiedSmartContour.mVOIUid;
    auto voiMng = mDataRepository->GetVoiManager();
    ROIEntity* roi = voiMng->GetROI(voiUid);
    if (roi == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The roi does not exist.";
        return TPS_ER_FAILURE;
    }

    if (mCertifiedSmartContour.mPtList.size() < 3)
    {
        TPS_LOG_DEV_ERROR<<"Points less than 3.";
        return TPS_ER_FAILURE;
    }

    std::vector<PT3> points;
    if (!mRenderProxy->TransformPoints(
        (WINDOW_TYPE)mCertifiedSmartContour.mWindowType, 
        mCertifiedSmartContour.mWindowUid, 
        mCertifiedSmartContour.mPtList, points))
    {
        TPS_LOG_DEV_ERROR<<"Failed to transform points.";
        return TPS_ER_FAILURE;
    }

    PT3 p0(points[0].x, points[0].y, points[0].z), p1, p2;
    VEC3 v0, v1, n;
    int i = 2;
    bool isValid = false;
    while(i < points.size())
    {
        p1 = PT3(points[i-1].x, points[i-1].y, points[i-1].z);
        p2 = PT3(points[i].x, points[i].y, points[i].z);
        v0 = p1 - p0;
        v1 = p2 - p1;
        if (!v0.IsParallel2(v1))
        {
            n = v0.CrossProduct(v1);
            n.Normalize();
            isValid = true;
            break;
        }
        ++i;
    }
    if (!isValid)
    {
        TPS_LOG_DEV_ERROR<<"Same line points.";
        return TPS_ER_FAILURE;
    }

    auto innerRoi = roi->GetInnerRoi();
    auto seriesUid = innerRoi->get_seriesuid();
    TpsImage3DEntity* image = nullptr;
    mDataRepository->GetImageDataManager()->GetImage3DData(seriesUid, &image);

    ContourSet* cs = roi->GetContourSet();
    auto sdf = roi->GetSDF();
    bool contour_parallel_2_cs = false;
    if (cs->IsEmpty())
    {
        std::vector<std::vector<PT3>> contours(1, points);
        SubVolumeHelperAPI::SetupContourSet(image, contours, roi);
    }
    else
    {
        //if plane is not primary, convert contour set
        contour_parallel_2_cs = cs->normal.IsParallel2(n);
        if (!contour_parallel_2_cs)
        {
            auto matPat2SDF = sdf->GetPatToSDF();
            auto nv = matPat2SDF.Transform(n);
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
                TPS_LOG_DEV_ERROR<<"Oblique contour.";
                return TPS_ER_FAILURE;
            }
            ROIShapeConverter::SDFToContourSet(*sdf, axis, *cs);
        }
        CombineNewContour_i(points, cs);
    }
    if (!ROIShapeConverter::ContourSetToSDF(*cs, *sdf, contour_parallel_2_cs))
    {
        TPS_LOG_DEV_ERROR<<"Failed to convert CS to SDF.";
        return TPS_ER_FAILURE;
    }

    //interpolation
    MAT4 matP2V = image->GetVolumeToPatient().Inverse();
    VEC3 N = matP2V.Transform(n);
    N.Normalize();
    if (fabs(fabs(N.z) - 1.0) <= 1e-6)
    {
        int z = TPS_ROUND(matP2V.Transform(p0).z);
        auto innerRoi = roi->GetInnerRoi();
        auto interFlags = innerRoi->get_interpolate();
        if (z >= 0 && z < interFlags.size())
        {
            interFlags[z] = false;
        }
        innerRoi->set_interpolate(interFlags);

        if (mCertifiedSmartContour.mIsInterpolate)
        {
            int size[3];
            sdf->GetSize(size);
            int start[3];
            sdf->GetStart(start);
            int valid_range[2] = 
            {
                TPS_MAX(0, start[2]), 
                TPS_MIN(interFlags.size() - 1, start[2] + size[2] - 1)
            }; 

            int pre = z, post = z;
            for (int i = z - 1; i >= valid_range[0]; --i)
            {
                if (!interFlags[i])
                {
                    pre = i;
                    break;
                }
            }
            for (int i = z + 1; i <= valid_range[1]; ++i)
            {
                if (!interFlags[i])
                {
                    post = i;
                    break;
                }
            }

            char* inter_flag_mask = new char[size[2] / 8 + 1];
            memset(inter_flag_mask, 0, (size[2] / 8 + 1) * sizeof(char));
            int idx;
            for (int i = pre + 1; i < z; ++i)
            {
                idx = i - start[2];
                inter_flag_mask[idx/8] |= (0x01 << (idx%8));
            }
            for (int i = z + 1; i < post; ++i)
            {
                idx = i - start[2];
                inter_flag_mask[idx/8] |= (0x01 << (idx%8));
            }
            ROIShapeConverter::InterpolateROI(inter_flag_mask, *sdf, *cs);
            TPS_DEL_ARRAY(inter_flag_mask);
        }
    }

    //update roi graphic object
    mRenderProxy->UpdateVOI(seriesUid, voiUid);
    mRenderProxy->Render();

    UpdateVOI_i(voiUid);
    return TPS_ER_SUCCESS;
}

void EditROIViaContourCmd::CombineNewContour_i(
    const std::vector<PT3>& points, 
    ContourSet* cs)
{
    if (points.size() < 3) return;

    int id = (int)TPS_ROUND(cs->pat_to_vol.Transform(points[0]).z);
    auto it = cs->page_contours.find(id);
    if (it == cs->page_contours.end())
    {
        PageContours* pc = new PageContours();
        pc->contour_list.push_back(Contour());
        pc->is_dirty = true;
        pc->slice_number = id;
        cs->page_contours[id] = pc;

        Contour& contour = pc->contour_list[0];
        contour.slice_number = id;
        contour.pt = points;
        return;
    }

    PageContours* pc = it->second;
    POLYGON_SET ps;
    ps.reserve(pc->contour_list.size());
    PT3 p_temp;
    int vc;
    for (int i = 0; i < pc->contour_list.size(); ++i)
    {
        vc = pc->contour_list[i].VertexCount();
        if (vc < 3) continue;

        ps.push_back(POLYGON());
        POLYGON& polygon = ps.back();
        polygon.reserve(vc);
        for (int j = 0; j < vc; ++j)
        {
            p_temp = cs->pat_to_vol.Transform(pc->contour_list[i].pt[j]);
            polygon.push_back(PT2(p_temp.x, p_temp.y));
        }
    }

    POLYGON polygon;
    polygon.reserve(points.size());
    for (int i = 0; i < points.size(); ++i)
    {
        p_temp = cs->pat_to_vol.Transform(points[i]);
        polygon.push_back(PT2(p_temp.x, p_temp.y));
    }
    ps = PolygonUtility::Logic(ps, polygon, PolygonUtility::UNION);

    pc->contour_list.clear();
    pc->contour_list.assign(ps.size(), Contour());
    MAT4 matV2P = cs->pat_to_vol.Inverse();
    for (int i = 0; i < ps.size(); ++i)
    {
        pc->contour_list[i].slice_number = id;
        pc->contour_list[i].pt.resize(ps[i].size());
        for (int j = 0; j < ps[i].size(); ++j)
        {
            pc->contour_list[i].pt[j] = matV2P.Transform(
                PT3(ps[i][j].x, ps[i][j].y, id));
        }
    }
    pc->is_dirty = true;
}

int EditROIViaContourCmd::Undo()
{
    if (!mRenderProxy->VoiRedoUndo(mPreState))
    {
        TPS_LOG_DEV_ERROR<<"Failed to execute render proxy voi redo undo.";
        return TPS_ER_FAILURE;
    }
    mRenderProxy->Render();
    UpdateVOI_i(mVoiUid);
    return TPS_ER_SUCCESS;
}

int EditROIViaContourCmd::Redo()
{
    if (!mRenderProxy->VoiRedoUndo(mPostState))
    {
        TPS_LOG_DEV_ERROR<<"Failed to execute render proxy voi redo undo.";
        return TPS_ER_FAILURE;
    }
    mRenderProxy->Render();
    UpdateVOI_i(mVoiUid);
    return TPS_ER_SUCCESS;
}

bool EditROIViaContourCmd::UpdateVOI_i(const std::string &voiUID)
{
    ROIEntity* roi = mDataRepository->GetVoiManager()->GetROI(voiUID);
    if (roi == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The roi does not exist.";
        return TPS_ER_FAILURE;
    }

    auto innerRoi = roi->GetInnerRoi();
    std::string imageUID = innerRoi->get_seriesuid();
    TpsImage3DEntity *image3D = nullptr;
    TpsImageDataManager* imageMng = mDataRepository->GetImageDataManager();
    if (!imageMng->GetImage3DData(imageUID, &image3D) || image3D == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Failed to get image 3d data.";
        return false;
    }

    //update voi statistical information
    RtCt2density* pCt2Density = imageMng->GetCT2DensityTableByImageUid(imageUID);
    if (!SubVolumeHelperAPI::UpdateVOIStatisticalInformation(pCt2Density, *image3D, roi))
    {
        TPS_LOG_DEV_ERROR<<"Failed to update statistical information.";
        return false;
    }

    if(mSaveObjects->SaveRoi(*roi) != TPS_ER_SUCCESS)
    {
        TPS_LOG_DEV_ERROR<<"SaveVOItoTMS failed VOI UID:"<<voiUID;
        return false;
    }

    EntityConverter::GetInstance()->ROIEntiyToCertifiedVOI(*roi, mCertVOI);
    if (mResponser != nullptr){
        mResponser->Response(*mCertVOI);
    }
    return true;
}

TPS_END_NAMESPACE