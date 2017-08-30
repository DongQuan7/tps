////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_poi_renderstrategy.h
/// 
///  \brief class TpsPoiRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/11/20
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_poi_renderstrategy.h"

//#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_raii_utility.h"
#include "McsfMedViewer3D/McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_poigraphicobjectcollection.h"
#include "RtTpsRenderLibrary/tps_rl_antialiased_circle_painter.h"

TPS_BEGIN_NAMESPACE

TpsPoiRenderStrategy::TpsPoiRenderStrategy() : 
    m_spPoiGraphicObjectCollection(nullptr), m_spVolumeGraphicObject(nullptr),
    m_spMprGraphicObject(nullptr), m_pPointPainter(nullptr){

}
TpsPoiRenderStrategy::~TpsPoiRenderStrategy() {

}

void TpsPoiRenderStrategy::Initialize() {
    m_pPointPainter = new AntialiasedCirclePainter();
    m_pPointPainter->Initialize();
}

void TpsPoiRenderStrategy::Finalize(){
    if (m_pPointPainter){
        delete m_pPointPainter;
        m_pPointPainter = nullptr;
    }
}

void TpsPoiRenderStrategy::Render_i() {
    if(m_spPoiGraphicObjectCollection == nullptr || 
       m_spVolumeGraphicObject == nullptr || m_spMprGraphicObject == nullptr) {
        //TPS_LOG_DEV_INFO<<"TpsPoiRenderStrategy::Render: POI GO collection is NULL !";
        return;
    }
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);
    glViewport(0, 0, mWidth, mHeight);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, mWidth, 0, mHeight, -1.0f, 1.0f);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    std::map<std::string,std::shared_ptr<TpsPoiGraphicObject>> poiMap = 
        m_spPoiGraphicObjectCollection->GetPoiMap();
    size_t pointCount = poiMap.size();
    if (pointCount == 0) return;

    Mcsf::MedViewer3D::Matrix4x4 matWorld2Patient = m_spVolumeGraphicObject->GetWorld2Patient();
    Mcsf::MedViewer3D::Matrix4x4 matPatient2World = matWorld2Patient.Inverse();
    Mcsf::MedViewer3D::Point3D vertices[4];
    m_spMprGraphicObject->GetVertices(vertices);
    Mcsf::MedViewer3D::Plane plane(vertices[0],vertices[1],vertices[3]);

    float* centers = new float[2 * pointCount];
    float* radius = new float[pointCount];
    float* colors = new float[3 * pointCount];
    bool* isFilled = new bool[pointCount];
    float* setups = new float[2 * pointCount];
    float* setupColors = new float[3 * pointCount];
    bool* isSetupFilled = new bool[pointCount];

    Mcsf::MedViewer3D::Point2D center;
    Mcsf::MedViewer3D::Point3D ptInPat, ptInWorld;
    double dSpacingInWorld, dDistanceToPlane;
    float fColor[4];
    int realCount = 0;
    int setupCount = 0;
    float fRadiusInPixel = 2.0f;

    for(auto iter = poiMap.begin(); iter != poiMap.end(); ++iter) {
        std::shared_ptr<TpsPoiGraphicObject> spPoiGO = iter->second;
        if (!spPoiGO->IsVisible()) {
            continue;
        }

        ptInPat = spPoiGO->GetPosition();
        ptInWorld = matPatient2World.Transform(ptInPat);
        dSpacingInWorld = m_spMprGraphicObject->GetThickness(); //0.5 * GetSpacing_i();
        dDistanceToPlane = plane.DistancePointToPlane(ptInWorld);
        if (dDistanceToPlane > dSpacingInWorld * 0.5f) continue;

        center = m_spMprGraphicObject->GetPlaneCoordinate(ptInWorld, false);
        if (mWidth >= mHeight){
            center.x = (center.x - 0.5) * mHeight / mWidth + 0.5;
        }
        else{
            center.y = (center.y - 0.5) * mWidth / mHeight + 0.5;
        }
        center.x *= mWidth - 1;
        center.y *= mHeight - 1;
        spPoiGO->GetColor(fColor);

        //if it's setup point, draw the cross line
        auto poiType = spPoiGO->GetPoiType();
        if (poiType == RtDbDef::Setup){
            //setup
            setups[setupCount * 2] = center.x;
            setups[setupCount * 2 + 1] = center.y;
            //color
            memcpy(setupColors + setupCount * 3, fColor, 3 * sizeof(float));
            isSetupFilled[setupCount] = (dDistanceToPlane < 1e-5);
            setupCount++;
        }
        else{
            //center
            centers[realCount * 2] = center.x;
            centers[realCount * 2 + 1] = center.y;
            //color
            memcpy(colors + realCount * 3, fColor, 3 * sizeof(float));
            //is filled flag
            isFilled[realCount] = (dDistanceToPlane < 1e-5);
            //radius
            radius[realCount] = fRadiusInPixel;
            realCount++;
        }
    }

    //draw setup point cross
    if (setupCount){
        double sz = 6;
        double internalSz = 2;
        for (int i = 0; i < setupCount; ++i)
        {
            glColor3f(setupColors[i*3], setupColors[i*3 + 1], setupColors[i*3 + 2]);
            if (isSetupFilled[i])
            {
                glBegin(GL_LINES);
                glVertex3d(setups[i*2] - sz, setups[i*2 + 1], 0.0);
                glVertex3d(setups[i*2] + sz, setups[i*2 + 1], 0.0);
                glVertex3d(setups[i*2], setups[i*2 + 1] - sz, 0.0);
                glVertex3d(setups[i*2], setups[i*2 + 1] + sz, 0.0);
                glEnd();
            }
            else
            {
                glBegin(GL_LINES);
                glVertex3d(setups[i*2] - sz, setups[i*2 + 1], 0.0);
                glVertex3d(setups[i*2] - internalSz, setups[i*2 + 1], 0.0);
                glVertex3d(setups[i*2], setups[i*2 + 1] - sz, 0.0);
                glVertex3d(setups[i*2], setups[i*2 + 1] - internalSz, 0.0);
                glVertex3d(setups[i*2] + internalSz, setups[i*2 + 1], 0.0);
                glVertex3d(setups[i*2] + sz, setups[i*2 + 1], 0.0);
                glVertex3d(setups[i*2], setups[i*2 + 1] + internalSz, 0.0);
                glVertex3d(setups[i*2], setups[i*2 + 1] + sz, 0.0);
                glEnd();
            }
        }
    }

    if (realCount) {
         m_pPointPainter->SetParas(centers, radius, colors, isFilled, realCount);
         m_pPointPainter->Render();
    }

    delete[] centers; delete[] radius;
    delete[] colors; delete[] isFilled;
    delete[] setups; delete[] setupColors; delete[] isSetupFilled;
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void TpsPoiRenderStrategy::Resize(int width, int height) {
    mWidth = width;
    mHeight = height;
}

int TpsPoiRenderStrategy::CacheChanges() {
    if(m_spPoiGraphicObjectCollection == nullptr ) {
        return -1;
    }
    if(m_spMprGraphicObject == nullptr) {
        return -1;
    }
    if(m_spVolumeGraphicObject == nullptr) {
        return -1;
    }
    if(m_spPoiGraphicObjectCollection->IsDirty() ||
       m_spMprGraphicObject->IsDirty() ||
       m_spVolumeGraphicObject->IsDirty() ) {
        mDirty = true;
    }
    return 0;
}

void TpsPoiRenderStrategy::SetPoiGraphicObjectCollection(
    std::shared_ptr<TpsPoiGraphicObjectCollection> poiGoColl) {
    if (m_spPoiGraphicObjectCollection != poiGoColl){
        m_spPoiGraphicObjectCollection = poiGoColl;
        mDirty = true;
    }
}

void TpsPoiRenderStrategy::SetMprGraphicObject(std::shared_ptr<MPRGraphicObject> mprGo) {
    if (m_spMprGraphicObject != mprGo){
        m_spMprGraphicObject = mprGo;
        mDirty = true;
    }
}

void TpsPoiRenderStrategy::SetVolumeGraphicObject(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> volume) {
    if (m_spVolumeGraphicObject != volume){
        m_spVolumeGraphicObject = volume;
        mDirty = true;
    }
}
TPS_END_NAMESPACE