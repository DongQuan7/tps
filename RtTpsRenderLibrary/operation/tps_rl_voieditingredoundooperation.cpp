//////////////////////////////////////////////////////////////////
/////  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
/////  All rights reserved.
///// 
/////  \author  GONG Maoliang maoliang.gong@united-imaging.com
///// 
/////  \file tps_rl_voieditingredoundooperation.cpp
///// 
/////  \brief class TpsVOIEditingRedoUndoOperation implementation 
///// 
/////  \version 1.0
///// 
/////  \date    2014/04/22
//////////////////////////////////////////////////////////////////
//
#include "StdAfx.h"
//#include "RtTpsRenderLibrary/tps_rl_voieditingredoundooperation.h"
//
//#include "McsfGeometry/mcsf_geometry_sub_volume.h"
//#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
//#include "Mcsf3DArithmetic/mcsf_3d_point3f.h"
//
//#include "RtTpsDataAccess/tps_da_voimanager.h"
//#include "RtTpsFramework/tps_fw_roientity.h"
//#include "RtTpsFramework/tps_fw_mask.h"
//#include "RtTpsDataAccess/tps_da_idatarepository.h"
//#include "RtTpsFramework/tps_fw_voiedtingcache.h"
//#include "RtTpsFramework/tps_fw_modelwarehouse.h"
//#include "RtTpsFramework/tps_fw_arithmeticconverter.h"
//#include "RtTpsFramework/tps_fw_safelock.h"
//#include "RtTpsFramework/tps_fw_graphicobjecttypehelper.h"
//#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"
//#include "RtTpsFramework/tps_fw_graphic_object_converter_factory_base.h"
//#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
//#include "RtTpsRenderLibrary/tps_rl_voirenderingcachegraphicobject.h"
//#include "RtTpsRenderLibrary/tps_rl_voirenderingcachegocollection.h"
//#include "RtTpsRenderLibrary/tps_rl_voigraphicobjectcollection.h"
//#include "RtTpsRenderLibrary/tps_rl_voigraphicobject.h"
//#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
//#include "RtTpsAlgorithmProxy/tps_ap_subvolumehelper_api.h"
//
////#ifndef ENABLE_TEST
////#include <fstream>
////#define ENABLE_TEST
////#endif
//
//using namespace Mcsf::MedViewer3D;
//
//TPS_BEGIN_NAMESPACE   // begin namespace tps
//
//const static GO_TYPE aRenderingCacheCollectionGoType[5] = {
//    GO_TYPE_VOI_RENDERINGCACHE_COLLECTION_CORONAL,
//    GO_TYPE_VOI_RENDERINGCACHE_COLLECTION_SAGITTAL,
//    GO_TYPE_VOI_RENDERINGCACHE_COLLECTION_AXIAL,
//    GO_TYPE_VOI_RENDERINGCACHE_COLLECTION_AXIAL_PREV,
//    GO_TYPE_VOI_RENDERINGCACHE_COLLECTION_AXIAL_POST,
//};
//
//TpsVOIEditingRedoUndoOperation::TpsVOIEditingRedoUndoOperation(
//    std::shared_ptr<TpsVOIEditingCache> cache) : mCache(cache){
//
//}
//
//TpsVOIEditingRedoUndoOperation::~TpsVOIEditingRedoUndoOperation(){
//
//}
//
//bool TpsVOIEditingRedoUndoOperation::ModifyGraphicObject() const{
//
//    if (mCache == nullptr){
//        TPS_LOG_DEV_ERROR<<"The voi editing cache is empty.";
//        return false;
//    }
//
//    //TEST_PERFORMANCE_INIT;
//    //TEST_PERFORMANCE_BEGIN;
//    //get voi
//    VOIEntity* voi = nullptr;
//    std::string voiUID = mCache->GetVoiUid();
//    if (!mDataRepository->GetVoiManager()->GetVOI(voiUID, &voi) || voi == nullptr){
//        TPS_LOG_DEV_ERROR<<"Failed to get voi in voi manager with uid: "<<voiUID;
//        return false;
//    }
//    //get sub-volume
//    auto subVolume = voi->GetSubVolume();
//    if (subVolume == nullptr){
//        TPS_LOG_DEV_ERROR<<"The sub-volume to be updated is empty.";
//        return false;
//    }
//    //get volume graphic object
//    std::string sGoType = GOTypeHelper::ToString(GO_TYPE_VOLUME);
//    std::string imageUID = voi->GetSeriesUid();
//    std::string goKey = imageUID + "|"+ sGoType;
//    auto go = mModelWarehouse->GetModelObject(goKey);
//    auto volume = std::dynamic_pointer_cast<GraphicObjVolume>(go);
//    if(volume == nullptr) {
//        TPS_LOG_DEV_ERROR<<"Failed to get volume go with uid: "<<imageUID;
//        return false;
//    }
//    //get voi graphic object
//    sGoType = GOTypeHelper::ToString(GO_TYPE_VOI_COLLECTION);
//    goKey = imageUID + "|" + sGoType;
//    go = mModelWarehouse->GetModelObject(goKey);
//    auto voiCollection = std::dynamic_pointer_cast<TpsVOIGOCollection>(go);
//    if (voiCollection == nullptr){
//        TPS_LOG_DEV_ERROR<<"Failed to get voi collection of image: "<<imageUID;
//        return false;
//    }
//    auto voiGO = voiCollection->GetVOI(voiUID);
//    if (voiGO == nullptr){
//        TPS_LOG_DEV_ERROR<<"Failed to get voi with UID: "<<voiUID;
//        return false;
//    }
//
//    size_t dim[3];
//    volume->GetDimension(dim);
//    Mcsf::Matrix4f matPatient2Volume(
//        Mcsf::Vector4f(dim[0], 0, 0, 0), 
//        Mcsf::Vector4f(0, dim[1], 0, 0), 
//        Mcsf::Vector4f(0, 0, dim[2], 0), 
//        Mcsf::Vector4f(-0.5, -0.5, -0.5, 1));
//    matPatient2Volume.Append(TpsArithmeticConverter::ConvertToMatrix4f(
//        volume->GetModel2PatientMat().Inverse()));
//    Mcsf::Matrix4f matNP2V;
//
//    //get cache buffer
//    unsigned char* buffer = nullptr; int width, height;
//    Mcsf::Matrix4f matPatientToNormalPlane;
//    std::vector<PlaneCache*> cacheList;
//    mCache->GetCacheList(cacheList);
//    Mcsf::Point3f aVertices[4];
//    for (int i = 0; i < cacheList.size(); ++i){
//        buffer = cacheList[i]->buffer;
//        width = cacheList[i]->width;
//        height = cacheList[i]->height;
//        matPatientToNormalPlane = cacheList[i]->transformMatrix;
//        if (buffer == nullptr || width <= 0 || height <= 0){
//            TPS_LOG_DEV_ERROR<<"The cache is invalid.";
//            continue;
//        }
//        matNP2V = matPatient2Volume;
//        matNP2V.Append(matPatientToNormalPlane.Inverse());
//        float deltaX = 0.5 / width;
//        float deltaY = 0.5 / height;
//        aVertices[0] = matNP2V.Transform(Mcsf::Point3f(deltaX, deltaY, 0.0));
//        aVertices[1] = matNP2V.Transform(Mcsf::Point3f(1.0 - deltaX, deltaY, 0.0));
//        aVertices[2] = matNP2V.Transform(Mcsf::Point3f(1.0 - deltaX, 1.0 - deltaY, 0.0));
//        aVertices[3] = matNP2V.Transform(Mcsf::Point3f(deltaX, 1.0 - deltaY, 0.0));
//
//#ifdef ENABLE_TEST
//        unsigned char* bmpBuffer = new unsigned char[width * height * 4];
//        for (int j=0; j < width * height; ++j){
//            bmpBuffer[4 * j] = buffer[j] * 255;
//            bmpBuffer[4 * j + 1] = buffer[j] * 255;
//            bmpBuffer[4 * j + 2] = buffer[j] * 255;
//            bmpBuffer[4 * j + 3] = 255;
//        }
//        char fileName[256];
//        sprintf_s(fileName, "d:\\temp\\before_UpdateSubVolume.bmp");
//        SubVolumeHelperAPI::SaveToTrueColorBitmap(fileName, bmpBuffer, height, width);
//        delete []bmpBuffer;
//
//        fstream fs;
//        fs.open("D:/temp/plane_buffer.raw", ios::out|ios::binary|ios::trunc);
//        for (int i=0; i<width*height; ++i){
//            fs<<buffer[i];
//        }
//        fs.close();
//
//#endif
//
//        //update sub-volume
//        if (TPS_ER_SUCCESS != SubVolumeHelperAPI::UpdateSubVolume(aVertices, buffer, 
//            width, height, subVolume)){
//            TPS_LOG_DEV_ERROR<<"Failed to get mask from contours.";
//            return false;
//        }
//
//#ifdef ENABLE_TEST
//        TPSMask mask(width, height);
//        if (TPS_ER_SUCCESS != SubVolumeHelperAPI::SliceSubVolume(
//            *subVolume, aVertices, width, height, &mask)){
//                TPS_LOG_DEV_ERROR<<"Failed to slice sub-volume of voi: "<<voiUID;
//                //return false;
//        }
//
//        buffer = mask.GetMaskBuffer();
//        bmpBuffer = new unsigned char[width * height * 4];
//        for (int j=0; j < width * height; ++j){
//            bmpBuffer[4 * j] = buffer[j] * 255;
//            bmpBuffer[4 * j + 1] = buffer[j] * 255;
//            bmpBuffer[4 * j + 2] = buffer[j] * 255;
//            bmpBuffer[4 * j + 3] = 255;
//        }
//        char fileName1[256];
//        sprintf_s(fileName1, "d:\\temp\\after_UpdateSubVolume.bmp");
//        SubVolumeHelperAPI::SaveToTrueColorBitmap(fileName1, bmpBuffer, height, width);
//        delete []bmpBuffer;
//#endif
//    }
//
//    size_t sliceCount = mCache->GetSliceCount();
//    if (sliceCount == 0) return false;
//
//    //redo/undo interpolation flags
//    bool* interpolationFlags = mCache->GetInterpolationFlags();
//    if (interpolationFlags == nullptr){
//        TPS_LOG_DEV_ERROR<<"Interpolation flags buffer is invalid.";
//        return false;
//    }
//    std::vector<bool> vecFlags;
//    for (int i = 0; i < sliceCount; ++i){
//        vecFlags.push_back(interpolationFlags[i]);
//    }
//    voi->SetInterpolationFlag(vecFlags);
//    voiGO->SetSliceCount(sliceCount);
//    voiGO->SetInterpolateFlags(interpolationFlags);
//
//    //redo/undo slice dirty flags
//    char* dirtyFlags = mCache->GetDirtyFlags();
//    if (dirtyFlags == nullptr){
//        TPS_LOG_DEV_ERROR<<"Dirty flags buffer is empty.";
//        return false;
//    }
//    
//    {
//        TpsSafeLock<VOIEntity> lock(voi);
//        auto ana = voi->GetAnastruct();
//        if (ana == nullptr){
//            TPS_LOG_DEV_ERROR<<"Ana-struct of VOI is empty.";
//            return false;
//        }
//        if (sliceCount != ana->slice_count){
//            TPS_LOG_DEV_ERROR<<"Slice count is not equal.";
//            return false;
//        }
//        memcpy(ana->is_dirty, dirtyFlags, sliceCount * sizeof(char));
//    }
//    SubVolumeHelperAPI::UpdateDirtySlicesOfAnastruct(matPatient2Volume.Inverse(), voi);
//
//    //TEST_PERFORMANCE_END(First step);
//    //update other mask caches
//    Matrix4x4 matWorld2Volume(
//        dim[0], 0, 0, 0, /*|*/ 0, dim[1], 0, 0, 
//        0, 0, dim[2], 0, /*|*/ -0.5, -0.5, -0.5, 1);
//    matWorld2Volume.Append(volume->GetModel2World().Inverse());
//    //TEST_PERFORMANCE_BEGIN;
//    UpdateVoiMaskCaches_i(imageUID, voiUID, subVolume, matWorld2Volume);
//    //TEST_PERFORMANCE_END(Second step: UpdateVoiMaskCaches_i);
//    //set dirty
//    voiGO->SetMeshDataDirty(true);
//    voiGO->SetDirty(true);
//    voiCollection->UpdateVOI(voiUID);
//    voiCollection->SetDirty(true);
//    return true;
//}
//
//void TpsVOIEditingRedoUndoOperation::UpdateVoiMaskCaches_i(
//    const std::string& imageUID, const std::string& voiUID, 
//    McsfGeometry::SubVolume* subVolume, const Matrix4x4& matWorld2Volume) const{
//
//    std::string cacheCollectionKey;
//    std::shared_ptr<IGraphicObject> go = nullptr;
//    std::shared_ptr<TpsVOIRenderingCacheGOCollection> cacheCollection = nullptr;
//    std::shared_ptr<TpsVOIRenderingCacheGO> cache = nullptr;
//    unsigned char* buffer;
//    int width, height;
//    Mcsf::Matrix4f matNP2V;
//    Matrix4x4 matrix, matNormalPlaneToVolume;
//    for (int i=0; i<5; ++i){
//        //get cache collection graphic object
//        cacheCollectionKey = imageUID + "|" + 
//            GOTypeHelper::ToString(aRenderingCacheCollectionGoType[i]);
//        go = mModelWarehouse->GetModelObject(cacheCollectionKey);
//        cacheCollection = std::dynamic_pointer_cast<TpsVOIRenderingCacheGOCollection>(go);
//        if (cacheCollection == nullptr){
//            TPS_LOG_DEV_INFO<<"VOI rendering cache collection of does not exist, key: "<<cacheCollectionKey;
//            continue;
//        }
//        //get rendering cache
//        cache = cacheCollection->GetCache(voiUID);
//        if (cache == nullptr){
//            TPS_LOG_DEV_ERROR<<"VOI rendering cache of voi: "<<voiUID<<" does not exist.";
//            continue;
//        }
//        //get four vertices in volume c.s.
//        cache->GetMaskCache(buffer, width, height, matrix);
//        matNormalPlaneToVolume = matWorld2Volume;
//        matNormalPlaneToVolume.Append(matrix.Inverse());
//        matNP2V = TpsArithmeticConverter::ConvertToMatrix4f(matNormalPlaneToVolume);
//        float deltaX = 0.5 / width;
//        float deltaY = 0.5 / height;
//        Mcsf::Point3f aVertices[4] = {
//            matNP2V.Transform(Mcsf::Point3f(deltaX, deltaY, 0.0)),
//            matNP2V.Transform(Mcsf::Point3f(1.0 - deltaX, deltaY, 0.0)),
//            matNP2V.Transform(Mcsf::Point3f(1.0 - deltaX, 1.0 - deltaY, 0.0)),
//            matNP2V.Transform(Mcsf::Point3f(deltaX, 1.0 - deltaY, 0.0))
//        };
//        //slice sub-volume
//        TPSMask mask(width, height);
//        if (TPS_ER_SUCCESS != SubVolumeHelperAPI::SliceSubVolume(
//            *subVolume, aVertices, width, height, &mask)){
//                TPS_LOG_DEV_ERROR<<"Failed to slice sub-volume of voi: "<<voiUID;
//                return;
//        }
//
//        //#ifdef _DEBUG
//        //        buffer = mask.GetMaskBuffer();
//        //        unsigned char* bmpBuffer = new unsigned char[width * height * 4];
//        //        for (int j=0; j < width * height; ++j){
//        //            bmpBuffer[4 * j] = buffer[j] * 255;
//        //            bmpBuffer[4 * j + 1] = buffer[j] * 255;
//        //            bmpBuffer[4 * j + 2] = buffer[j] * 255;
//        //            bmpBuffer[4 * j + 3] = 255;
//        //        }
//        //        char fileName[256];
//        //        sprintf_s(fileName, "d:\\temp\\slice_subvolume_result_%s.bmp", aSections[i].c_str());
//        //        SubVolumeHelperAPI::SaveToTrueColorBitmap(fileName, bmpBuffer, height, width);
//        //        delete []bmpBuffer;
//        //#endif
//
//        //set the mask cache
//        cache->SetMaskCache(mask.GetMaskBuffer(), width, height, matrix);
//    }
//}
//TPS_END_NAMESPACE  // end namespace tps
