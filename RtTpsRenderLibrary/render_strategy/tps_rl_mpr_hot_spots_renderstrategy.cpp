////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_mascot_renderstrategy.cpp
/// 
///  \brief class MascotRenderStrategy implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/09/05
////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_mpr_hot_spots_renderstrategy.h"
#include "RtTpsRenderLibrary/tps_rl_mprgraphicobject.h"
#include "RtTpsRenderLibrary/tps_rl_hot_spots_graphicobject.h"

#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_raii_utility.h"            // for Scopped_SetValue
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"   // for GraphicObjVolume
#include "RtTpsDataAccess/tps_da_hot_cold_spots_manager.h"                      // for TpsHotColdSpotsManager
#include "RtTpsDataAccess/tps_da_idatarepository.h"

TPS_BEGIN_NAMESPACE

MprHotSpotsRenderStrategy::MprHotSpotsRenderStrategy()
    :m_spMPRGO(nullptr),
     m_spVolumeGO(nullptr),
     m_spHotSpotsGO(nullptr),
     m_spColdSpotsGO(nullptr){
}

MprHotSpotsRenderStrategy::MprHotSpotsRenderStrategy(std::shared_ptr<MPRGraphicObject> spMPRGO)
    :m_spMPRGO(spMPRGO), m_spHotSpotsGO(nullptr) {

}

void MprHotSpotsRenderStrategy::Initialize() {

}

MprHotSpotsRenderStrategy::~MprHotSpotsRenderStrategy() {

}

void MprHotSpotsRenderStrategy::SetName(std::string& strName) {
    m_strName = strName;
}

void MprHotSpotsRenderStrategy::SetCurrentPlanUid(std::string& planUid)
{
    m_strPlanUid = planUid;
}

void MprHotSpotsRenderStrategy::SetMPRGraphicObject(std::shared_ptr<MPRGraphicObject> spMPRGO) {
    if (m_spMPRGO != spMPRGO){
        m_spMPRGO = spMPRGO;
        mDirty = true;
    }
}

void MprHotSpotsRenderStrategy::SetVolumeGraphicObject(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spVolumeGO)
{
    if (m_spVolumeGO != spVolumeGO){
        m_spVolumeGO = spVolumeGO;
        mDirty = true;
    }
}

void MprHotSpotsRenderStrategy::SetHotSpotsGraphicObject(std::shared_ptr<HotSpotsGraphicObject> spHotSpotsGO) {
    if (m_spHotSpotsGO != spHotSpotsGO){
        m_spHotSpotsGO = spHotSpotsGO;
        mDirty = true;
    }
}

void MprHotSpotsRenderStrategy::SetColdSpotsGraphicObject(std::shared_ptr<HotSpotsGraphicObject> spColdSpotsGO) {
    if (m_spColdSpotsGO != spColdSpotsGO){
        m_spColdSpotsGO = spColdSpotsGO;
        mDirty = true;
    }
}

void MprHotSpotsRenderStrategy::Render_i() {
    if (m_spMPRGO == nullptr) {
        return;
    }

    std::shared_ptr<std::vector<Mcsf::MedViewer3D::Point3D>> spVecHotPoints = nullptr;
    std::shared_ptr<std::vector<Mcsf::MedViewer3D::Point3D>> spVecColdPoints = nullptr;

    if (m_spHotSpotsGO != nullptr) {
        if (m_spHotSpotsGO->Visible()) {
            spVecHotPoints = m_spHotSpotsGO->GetVecPointsByNameAndPlanUid(this->m_strName, m_strPlanUid);
        }
    }

    if (m_spColdSpotsGO != nullptr) {
        if (m_spColdSpotsGO->Visible()) {
             spVecColdPoints = m_spColdSpotsGO->GetVecPointsByNameAndPlanUid(this->m_strName, m_strPlanUid);
        }
    }

    if ((spVecHotPoints == nullptr || spVecHotPoints->size() == 0) && 
         (spVecColdPoints == nullptr || spVecColdPoints->size() == 0)) {
        return;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_uiFbo);

    float fScale = 1.0f;
    if (mWidth >= mHeight) {
        glViewport(0, -(mWidth - mHeight)/2, mWidth, mWidth);
        fScale = (float)mHeight / (float)mWidth;
    }
    else {
        glViewport(-(mHeight - mWidth)/2, 0, mHeight, mHeight);
        fScale =  (float)mWidth / (float)mHeight;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -0.1, 0.1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(fScale, fScale, 1.0f);

    Mcsf::MedViewer3D::Matrix4x4 matWorld2Patient = m_spVolumeGO->GetWorld2Patient();
    Mcsf::MedViewer3D::Matrix4x4 matPatient2World = matWorld2Patient.Inverse();
    Mcsf::MedViewer3D::Point3D ptInWorld;

    Mcsf::MedViewer3D::Point3D vertices[4];
    m_spMPRGO->GetVertices(vertices);
    Mcsf::MedViewer3D::Plane plane(vertices[0], vertices[1], vertices[3]);
    double dSpacingInWorld = m_spMPRGO->GetThickness(); //0.5 * GetSpacing_i();
        
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_LINES);


    // Plane coordinate:
    //                      ( 1, 1)
    //          ___________________
    //         |   |           |   |
    //         |   |    MPR    |   |
    //         |   |           |   |
    //         |___|___________|___|
    //          ( 0, 0)

    // Hot spots
    glColor4f(1.0f, 0.08f, 0.08f, 1.0);
    if (spVecHotPoints != nullptr) {
        for (int i = 0; i < spVecHotPoints->size(); i++) {
            ptInWorld = matPatient2World.Transform((*spVecHotPoints)[i]);
            double dDistanceToPlane = plane.DistancePointToPlane(ptInWorld);
            if (dDistanceToPlane <= dSpacingInWorld * 0.5f) {
                Mcsf::MedViewer3D::Point2D ptInPlane = m_spMPRGO->GetPlaneCoordinate(ptInWorld, false);
                Mcsf::MedViewer3D::Point2D ptInView;
                ptInView.x = 2.0f * ptInPlane.x - 1.0f;
                ptInView.y = 2.0f * ptInPlane.y - 1.0f;
                //斜线填充
                glVertex3f(ptInView.x - 0.01, ptInView.y - 0.01, 0);
                glVertex3f(ptInView.x + 0.01, ptInView.y + 0.01, 0);
            }
        }
    }
  
    //Cold Spots
    glColor4f(0.0f, 1.0, 1.0, 1.0);
    if (spVecColdPoints != nullptr) {
        for (int i = 0; i < spVecColdPoints->size(); i++) {
            ptInWorld = matPatient2World.Transform((*spVecColdPoints)[i]);
            double dDistanceToPlane = plane.DistancePointToPlane(ptInWorld);
            if (dDistanceToPlane <= dSpacingInWorld * 0.5f) {
                Mcsf::MedViewer3D::Point2D ptInPlane = m_spMPRGO->GetPlaneCoordinate(ptInWorld, false);
                Mcsf::MedViewer3D::Point2D ptInView;
                ptInView.x = 2.0f * ptInPlane.x - 1.0f;
                ptInView.y = 2.0f * ptInPlane.y - 1.0f;
                //斜线填充（与热区斜线的斜率向反）
                glVertex3f(ptInView.x - 0.01, ptInView.y + 0.01, 0);
                glVertex3f(ptInView.x + 0.01, ptInView.y - 0.01, 0);
            }
        }
    }

    glEnd();
    glFlush();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}


void MprHotSpotsRenderStrategy::Resize(int w, int h) {
    mHeight = h;
    mWidth = w;
}

void MprHotSpotsRenderStrategy::Finalize() {

}

int MprHotSpotsRenderStrategy::CacheChanges() {
    //m_bDirty = true;
    if (m_spMPRGO == nullptr)
    {
        return 0;
    }
    if (m_spMPRGO->IsDirty())
    {
        mDirty = true;
        return 0;
    }
    if (m_spHotSpotsGO != nullptr)
    {
        if (m_spHotSpotsGO->IsDirty())
        {
            mDirty = true;
            return 0;
        }       
    }

    if (m_spColdSpotsGO != nullptr)
    {
        if (m_spColdSpotsGO->IsDirty())
        {
            mDirty = true;
            return 0;
        }       
    }

    return 0;
}

TPS_END_NAMESPACE
