////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_series_table_cache_graphic_object_converter.cpp
/// 
///  \brief class SeriesTableCacheConverter declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/07/18
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_series_table_cache_graphic_object_converter.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsFramework/tps_fw_modelwarehouse.h"
#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include "RtTpsRenderLibrary/tps_rl_series_table_cache_graphic_object.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"
#include "RtTpsDataAccess/tps_da_tablemanager.h"
#include "RtTpsFramework/tps_fw_table.h"
#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"

#include "tps_logger.h"

using namespace Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE   // begin namespace tps

SeriesTableCacheConverter::SeriesTableCacheConverter(GO_TYPE goType)
    : TpsGOConverterBase(goType), mSeriesTable(nullptr), mRtTable(nullptr)
{

}

SeriesTableCacheConverter::~SeriesTableCacheConverter()
{

}

GraphicObjectPtr SeriesTableCacheConverter::CreateGraphicObject_i()
{
    return std::shared_ptr<SeriesTableCacheGraphicObject>(
        new SeriesTableCacheGraphicObject());
}

bool SeriesTableCacheConverter::PrepareSourceData_i(const GOConverterParameter& para)
{
    auto it = para.data_source_map.find(IMAGE);
    if (it == para.data_source_map.end())
    {
        TPS_LOG_DEV_ERROR<<"The IMAGE data source does not exist.";
        return false;
    }

    mSeriesTable = nullptr;
    mRtTable = nullptr;
    auto tblMng = mDataRepository->GetTableManager();
    mSeriesTable = tblMng->GetSeriesTable(it->second);
    if (mSeriesTable == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The series table does not exist.";
        return false;
    }

    auto tblUid = mSeriesTable->GetTableUid();
    mRtTable = tblMng->GetRtTable(tblUid);
    if (!tblUid.empty() && mRtTable == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The RtTable does not exist.";
        return false;
    }
    return true;
}

bool SeriesTableCacheConverter::Update_i(GraphicObjectPtr go)
{
    auto cacheGO = std::dynamic_pointer_cast<SeriesTableCacheGraphicObject>(go);
    if (cacheGO == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"Graphic object to be updated is null.";
        return false;
    }
    if (mSeriesTable == nullptr) return false;
    cacheGO->Clear();
    if (mRtTable == nullptr) return true;

    std::string imageUID = mSeriesTable->GetSeriesUid();
    auto goTmp = mModelWarehouse->GetModelObject(
        imageUID + "|" + GOTypeHelper::ToString(GO_TYPE_VOLUME));
    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(goTmp);
    if (volume == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get volume graphic object of image: "<<imageUID;
        return false;
    }

    auto section = GOTypeHelper::GetFirstPostfixComponent(mGOType);
    auto location = GOTypeHelper::GetSecondPostfixComponent(mGOType);

    GO_TYPE goType = GOTypeHelper::ComposeAsGOType(MPR, section, location);
    std::string mprKey = imageUID + "|" + GOTypeHelper::ToString(goType);
    goTmp = mModelWarehouse->GetModelObject(mprKey);
    auto mpr = std::dynamic_pointer_cast<MPRGraphicObject>(goTmp);
    if (mpr == nullptr){
        TPS_LOG_DEV_ERROR<<"Failed to get mpr graphic object with uid: "<<mprKey;
        return false;
    }

    auto center = mRtTable->GetCenter();
    auto matWorld2Pat = volume->GetWorld2Patient();
    auto matPat2Tbl = mSeriesTable->GetTableToPatient(center).Inverse();
    auto matTbl2TblVol = mRtTable->GetTableToMaskVolume();
    auto matWorld2TblVol = TpsArithmeticConverter::ConvertToMatrix4f(matWorld2Pat);
    matWorld2TblVol.Prepend(matPat2Tbl);
    matWorld2TblVol.Prepend(matTbl2TblVol);

    Point3D vertices[4];
    mpr->GetVertices(vertices);
    Mcsf::Point3f verticesF[4];
    for (int i=0; i<4; ++i){
        verticesF[i] = matWorld2TblVol.Transform(
            TpsArithmeticConverter::ConvertToPoint3f(vertices[i]));
    }

    unsigned int dim[3];
    mRtTable->GetMaskDim(dim);
    size_t stDim[3] = { dim[0], dim[1], dim[2] };

    //get expanded rect of mpr plane to involve all table volume in, calculate the matrix
    //form world to normalized plane c.s.
    Mcsf::Point3f boundingRectInVolume[4];
    Mcsf::Matrix4f matWorld2NP;
    SubVolumeHelperAPI::GetBoundingRectangle(stDim, verticesF, 
        boundingRectInVolume, matWorld2NP);
    unsigned int width = static_cast<unsigned int>(
        Mcsf::Vector3f(boundingRectInVolume[1] - boundingRectInVolume[0]).Magnitude() + 1.5);
    unsigned int height = static_cast<unsigned int>(
        Mcsf::Vector3f(boundingRectInVolume[3] - boundingRectInVolume[0]).Magnitude() + 1.5);
    matWorld2NP.Append(matWorld2TblVol);

    //cut sub-volume with expanded rect & get the mask cache
    unsigned char* buffer = new unsigned char[width * height];
    memset(buffer, 0, sizeof(unsigned char) * width * height);
    if (TPS_ER_SUCCESS != SubVolumeHelperAPI::SliceSubVolume(
        mRtTable->GetMask(), dim, boundingRectInVolume, width, height, buffer)){
        TPS_LOG_DEV_ERROR<<"Failed to slice volume.";
        return false;
    }
    /*#ifdef _DEBUG
        unsigned char* bmpBuffer = new unsigned char[width * height * 4];
        for (unsigned int j=0; j < width * height; ++j){
            bmpBuffer[4 * j] = buffer[j] * 255;
            bmpBuffer[4 * j + 1] = buffer[j] * 255;
            bmpBuffer[4 * j + 2] = buffer[j] * 255;
            bmpBuffer[4 * j + 3] = 255;
        }
        char fileName[256];
        sprintf_s(fileName, "d:\\temp\\contour_mask\\slice_table_result_%s.bmp", 
            GOTypeHelper::ToString(goType));
        SubVolumeHelperAPI::SaveToTrueColorBitmap(fileName, bmpBuffer, height, width);
        delete []bmpBuffer;
    #endif*/
    cacheGO->SetCache(buffer, width, height, TpsArithmeticConverter::ConvertToMatrix4x4(matWorld2NP));
    cacheGO->SetIsTableEmpty(false);
    TPS_DEL_ARRAY(buffer);

    double color[4];
    mSeriesTable->GetColor(color);
    cacheGO->SetColor(color);
    return true;
}
TPS_END_NAMESPACE  // end namespace tps