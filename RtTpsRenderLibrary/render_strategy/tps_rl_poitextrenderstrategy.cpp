
////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_poitextrenderstrategy.cpp
/// 
///  \brief class POITypeTextRenderStrategy definition 
/// 
///  \version 1.0
/// 
///  \date    2014/04/14
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"
#include "McsfMedViewer3DArithmetic/plane.h"
#include "RtTpsRenderLibrary/tps_rl_poitextrenderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsDataAccess/tps_da_imagedatamanager.h"
#include "RtTpsDataAccess/tps_da_poimanager.h"
#include "RtTpsDataAccess/tps_da_idatarepository.h"

//#include "McsfGeometry/mcsf_geometry_plane.h"

#include "tps_logger.h"


TPS_BEGIN_NAMESPACE
POITypeTextRenderStrategy::POITypeTextRenderStrategy() {

}

POITypeTextRenderStrategy::~POITypeTextRenderStrategy() {

}

void POITypeTextRenderStrategy::SetMprGraphicObject(std::shared_ptr<MPRGraphicObject> mprGo) {
    mMprGraphicObject = mprGo;
}

void POITypeTextRenderStrategy::SetVolumeGraphicObject(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volume) {
    mVolumeGraphicObject = volume;
}

void POITypeTextRenderStrategy::SetPoiGraphicObjectCollection(std::shared_ptr<TpsPoiGraphicObjectCollection> poiColl) {
    mPoiGraphicObejctColl = poiColl;
}

bool POITypeTextRenderStrategy::Render(unsigned char*& buffer, int bufWidth, int bufHeight) 
{
    using namespace Mcsf::MedViewer3D;
    
    if(mMprGraphicObject == nullptr ||
        mVolumeGraphicObject == nullptr ||
        mPoiGraphicObejctColl == nullptr) {
        //no POI sometimes
        TPS_LOG_TRACE_INFO<<"input go is nullptr!";
        return true;
    }

    if(nullptr == buffer || bufHeight <= 0 || bufWidth <=0) {
        TPS_LOG_DEV_ERROR<<"buffer==nullptr || bufWidth<=0 || bufHeight<=0";
        return false;
    }

    Matrix4x4 matWorld2Patient = mVolumeGraphicObject->GetWorld2Patient();
    Matrix4x4 matPatient2World = matWorld2Patient.Inverse();

    auto poiMap = mPoiGraphicObejctColl->GetPoiMap();


    for(auto itr = poiMap.begin(); itr != poiMap.end() ; ++itr) {
        std::shared_ptr<TpsPoiGraphicObject> poiGo = itr->second;

        if(!poiGo->IsVisible()) {
            continue;
        }
        Mcsf::MedViewer3D::Point3D pt = poiGo->GetPosition();
        Mcsf::MedViewer3D::Point3D ptInWorld = matPatient2World.Transform(pt);
        Mcsf::MedViewer3D::Point3D vertices[4];
        mMprGraphicObject->GetVertices(vertices);
        Mcsf::MedViewer3D::Plane  plane(vertices[0], vertices[1],vertices[3]);

        double maxSpacing = 0.5 * mMprGraphicObject->GetThickness(); //0.5 *GetSpacing_i();
        double actualLen = plane.DistancePointToPlane(ptInWorld);

        if(actualLen > maxSpacing) {
            continue;
        }

        Vector3D vecNormal = plane.GetNormal();
        Point3D ptRes = ptInWorld + vecNormal * actualLen;
        if(!plane.IsOnPlane(ptInWorld)) {
            ptRes = ptInWorld + vecNormal * (-1 * actualLen);
        }

        Point2D ptNormalizedPlane = mMprGraphicObject->GetPlaneCoordinate(ptRes, false);

        float nomalizedPlaneWidth = std::min(bufWidth, bufHeight);
        int ptScreenX = bufWidth * 0.5 + (ptNormalizedPlane.x - 0.5) * nomalizedPlaneWidth;
        int ptScreenY = bufHeight * 0.5 + (ptNormalizedPlane.y - 0.5) * nomalizedPlaneWidth;

        unsigned char* pStrBuffer = nullptr;
        int strBufHeight = 0;
        int strBufWidth = 0;

        std::string nameOfPoi = poiGo->GetPoiName();
        std::wstring wName(nameOfPoi.begin(), nameOfPoi.end());

        TpsPOIManager::POIPosInfo poi_pos;
        if(!mDataRepository->GetPoiManager()->GetPoiPosText(itr->first, poi_pos)){
            return false;
        }
        pStrBuffer = poi_pos.TxtBuffer;
        strBufWidth = poi_pos.TxtWidth;
        strBufHeight = poi_pos.TxtHeight;
        if (strBufHeight > bufHeight || strBufWidth > bufWidth){
            continue;
        }

        //int top = static_cast<int>(ptScreen.y * bufHeight);
        //int left = static_cast<int>(ptScreen.x * bufWidth) + 2;

        int top = static_cast<int>(ptScreenY + 0.8 * poi_pos.Counter * strBufHeight);
        int left = static_cast<int>(ptScreenX + 2);

        unsigned char* pDstBuffer = buffer + (top *bufWidth + left) *4;
        unsigned char *pSrcBuffer = pStrBuffer;

        for(int m=0; m<strBufHeight; ++m) {
            for(int n=0; n<strBufWidth; ++n) {
                if(pSrcBuffer[(m*strBufWidth+n)*4+3]!=0 &&
                    top+m <bufHeight && left+n <bufWidth &&
                    top+m >1 && left + n>1) {
                    pDstBuffer[(m*bufWidth+n)*4] = pSrcBuffer[(m*strBufWidth+n)*4];
                    pDstBuffer[(m*bufWidth+n)*4+1] = pSrcBuffer[(m*strBufWidth+n)*4+1];
                    pDstBuffer[(m*bufWidth+n)*4+2] = pSrcBuffer[(m*strBufWidth+n)*4+2];
                    pDstBuffer[(m*bufWidth+n)*4+3] = 255;
                }
            }
        }


    }

    return true;


}

double POITypeTextRenderStrategy::GetSpacing_i() {
    using namespace Mcsf::MedViewer3D;
    if(mVolumeGraphicObject == nullptr || mMprGraphicObject == nullptr) {
        return 0.0;
    }
    size_t dim[3];
    mVolumeGraphicObject->GetDimension(dim);
    double spacing[3];
    spacing[0] = 2.0 / (dim[0]-1);
    spacing[1] = 2.0 / (dim[1]-1);
    spacing[2] = 2.0 / (dim[2]-1);


    Point3D vertices[4];
    mMprGraphicObject->GetVertices(vertices);
    Vector3D vec1 = vertices[1] - vertices[0];
    Vector3D vec2 = vertices[3] - vertices[0];
    Vector3D vec = vec1.CrossProduct(vec2);
    vec.Normalize();
    if(std::abs(vec.x)>= std::abs(vec.y) && 
        std::abs(vec.x) >= std::abs(vec.z)) {
            return spacing[0];
    }
    else if(std::abs(vec.y) >= std::abs(vec.x) &&
        std::abs(vec.y) >= std::abs(vec.z)){
            return spacing[1];
    }
    else if(std::abs(vec.z) >= std::abs(vec.x) &&
        std::abs(vec.z) >= std::abs(vec.y)) {
            return spacing[2];
    }

    return 0.0;
}


TPS_END_NAMESPACE


