////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_bevbeamrenderstrategy.cpp
/// 
///  \brief class BevBeamRenderStrategy definition 
/// 
///  \version 1.0
/// 
///  \date    2014/03/06
////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_bevbeamrenderstrategy.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsRenderLibrary/tps_rl_beamgocollection.h"
#include "RtTpsRenderLibrary/tps_rl_beamgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_segmentgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_shaded_voi_graphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_normalgroupcollection.h"
#include "RtTpsRenderLibrary/tps_rl_camera_graphic_object.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
//#include <stdio.h>


TPS_BEGIN_NAMESPACE
BevBeamRenderStrategy::BevBeamRenderStrategy()
: mNormalGroupCollection(nullptr), mVolumeGo(nullptr), mCameraGO(nullptr){

}

BevBeamRenderStrategy::~BevBeamRenderStrategy() {

}

void BevBeamRenderStrategy::Initialize() {
 
}
void BevBeamRenderStrategy::Finalize() {

}
int BevBeamRenderStrategy::CacheChanges() {
    if(mVolumeGo == nullptr || 
       mNormalGroupCollection == nullptr ||
       mCameraGO == nullptr) {
        TPS_LOG_DEV_INFO<<"input go is null!";
        return -1;
    }
    if(mNormalGroupCollection->IsDirty() ||
        mVolumeGo->IsDirty() ||
        mCameraGO->IsDirty()) {
            mDirty = true;
    }
    return 0;
}

void BevBeamRenderStrategy::Resize(int width,int height) {
    mWidth = width;
    mHeight = height;
}

void BevBeamRenderStrategy::Render_i() {

    //using namespace Mcsf::MedViewer3D;
    //if(nullptr == mNormalGroupCollection  ||
    //    nullptr == mVolumeGo      ||
    //    nullptr == mCameraGO ) {
    //     TPS_LOG_DEV_WARNING<<"input go is null !";
    //     return;
    //}
  
    //std::string activeNGUid = mNormalGroupCollection->GetActiveNormalGroupUID();
    //auto activeNG = mNormalGroupCollection->GetNormalGroup(activeNGUid);
    //if (activeNG == nullptr){
    //    TPS_LOG_DEV_ERROR<<"There is no active normal group!";
    //    return;
    //}

    //std::map<std::string, std::shared_ptr<TpsBeamGraphicObject>> beamMap = activeNG->GetBeamMap();

    //auto activeBeam = activeNG->GetActiveBeamGo();

    //for(auto itr = beamMap.begin(); itr != beamMap.end(); ++itr) {

    //    std::shared_ptr<TpsBeamGraphicObject> beamGo = itr->second;
    //    if (beamGo == nullptr){
    //        TPS_LOG_DEV_ERROR<<"TpsBeamGraphicObj is null!";
    //        return;
    //    }
    //    if (!beamGo->GetVisible()) continue;
    //    std::shared_ptr<TpsSegmentGraphicObject> segment = beamGo->GetActiveSegmentGo();
    //    if(segment == nullptr) {
    //        TPS_LOG_DEV_ERROR<<"segment is null!";
    //        return;
    //    }

        ////for temp, copy from mlc render strategy
        //const float minJawPosition = -200.0f;
        //const float maxJawPosition =  200.0f;
        //const float distS2A = 1000.0f;

        //LEAF_DIRECTION leafDirection = segment->GetLeafDirection();
        //std::vector<Point3D> ptList;
        //StcMLC* pMLC = segment->GetMLC();
        //const int nLeaf = pMLC->m_nLeaf;
        //plunc::LEAF_POSITION_STC* pMlcLeaf = pMLC->m_pLeaf;
        //if(leafDirection == LEAF_DIRECTION_X)
        //{
        //    for (int i = 0; i < nLeaf; i++)
        //    {
        //        ptList.push_back(Point3D(minJawPosition, pMlcLeaf[i].min_w, -1 * distS2A));
        //        ptList.push_back(Point3D(minJawPosition, pMlcLeaf[i].max_w, -1 * distS2A));
        //        ptList.push_back(Point3D(pMlcLeaf[i].pos_1, pMlcLeaf[i].max_w, -1 * distS2A));
        //        ptList.push_back(Point3D(pMlcLeaf[i].pos_1, pMlcLeaf[i].min_w, -1 * distS2A));
        //        ptList.push_back(Point3D(maxJawPosition, pMlcLeaf[i].min_w, -1 * distS2A));
        //        ptList.push_back(Point3D(maxJawPosition, pMlcLeaf[i].max_w, -1 * distS2A));
        //        ptList.push_back(Point3D(pMlcLeaf[i].pos_2, pMlcLeaf[i].max_w, -1 * distS2A));
        //        ptList.push_back(Point3D(pMlcLeaf[i].pos_2, pMlcLeaf[i].min_w, -1 * distS2A));
        //    }
        //}
        //else
        //{
        //    for (int i = 0; i < nLeaf; i++)
        //    {
        //        ptList.push_back(Point3D(pMlcLeaf[i].min_w, minJawPosition, -1 * distS2A));
        //        ptList.push_back(Point3D(pMlcLeaf[i].max_w, minJawPosition, -1 * distS2A));
        //        ptList.push_back(Point3D(pMlcLeaf[i].max_w, pMlcLeaf[i].pos_1, -1 * distS2A));
        //        ptList.push_back(Point3D(pMlcLeaf[i].min_w, pMlcLeaf[i].pos_1, -1 * distS2A));
        //        ptList.push_back(Point3D(pMlcLeaf[i].min_w, maxJawPosition, -1 * distS2A));
        //        ptList.push_back(Point3D(pMlcLeaf[i].max_w, maxJawPosition, -1 * distS2A));
        //        ptList.push_back(Point3D(pMlcLeaf[i].max_w, pMlcLeaf[i].pos_2, -1 * distS2A));
        //        ptList.push_back(Point3D(pMlcLeaf[i].min_w, pMlcLeaf[i].pos_2, -1 * distS2A));
        //    }
        //}

        //Matrix4x4 matBeam2Pat;
        //segment->GetStartCollimatorToPatMatrix(&matBeam2Pat);
        //Point3D ptSource = matBeam2Pat.Transform(Point3D(0.0,0.0,0.0));

        //double scale = 0.3;
        //double oneMinusScale = 1 - scale;
        //for(std::vector<Point3D>::iterator itrr = ptList.begin(); itrr != ptList.end(); ++itrr) {
        //    Point3D ptTemp = matBeam2Pat.Transform(*itrr);
        //    *itrr = Point3D(scale*ptTemp.x + oneMinusScale*ptSource.x,scale*ptTemp.y + oneMinusScale*ptSource.y,scale*ptTemp.z +oneMinusScale*ptSource.z);
        //}
        //segment->Set3DSegmentPtList(ptList);
    //}
}

void BevBeamRenderStrategy::SetNormalGroupCollection(
    std::shared_ptr<NormalGroupCollection> normalGroupCollection){
    if (mNormalGroupCollection != normalGroupCollection){
        mNormalGroupCollection = normalGroupCollection;
        mDirty = true;
    }
}

void BevBeamRenderStrategy::SetCameraGO(std::shared_ptr<CameraGraphicObject> go) {
    if (mCameraGO != go){
        mCameraGO = go;
        mDirty = true;
    }
}

void BevBeamRenderStrategy::SetVolumeGraphicObject(std::shared_ptr<
    Mcsf::MedViewer3D::GraphicObjVolume> volumeGo) {
    if (mVolumeGo != volumeGo){
        mVolumeGo = volumeGo;
        mDirty = true;
    }
}

bool BevBeamRenderStrategy::IsBeamView_i() {
    if(nullptr == mNormalGroupCollection) {
        TPS_LOG_DEV_INFO<<"beam go collection is nullptr!";
        return false;
    }

    std::string activeNGUid = mNormalGroupCollection->GetActiveNormalGroupUID();
    auto activeNG = mNormalGroupCollection->GetNormalGroup(activeNGUid);
    if (activeNG == nullptr){
        TPS_LOG_DEV_ERROR<<"There is no active normal group!";
        return false;
    }
    std::shared_ptr<TpsBeamGraphicObject> beamGo = activeNG->GetActiveBeamGo();
    if(nullptr == beamGo) {
        TPS_LOG_DEV_ERROR<<"beamGo is null !";
        return false;
    }
    std::shared_ptr<TpsSegmentGraphicObject> segment = nullptr;
    segment = beamGo->GetActiveSegmentGo();
    if(nullptr == segment) {
        TPS_LOG_DEV_ERROR<<"segment do not exist!";
        return false;
    }
    return true;

}
TPS_END_NAMESPACE
