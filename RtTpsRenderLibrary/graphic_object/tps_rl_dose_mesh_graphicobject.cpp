//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  LI Song  song.li@united-imaging.com
///
///  \file tps_rl_dose_mesh_graphicobject.cpp
///  \brief class DoseMeshGraphicObject  
///
///  \version 1.0
///  \date    Jun. 8, 2015
///  \{
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_dose_mesh_graphicobject.h"

#include "McsfMedViewer3D/McsfMedViewer3DGraphicUtils/camera.h"
                                       
#include "RtTpsAlgorithmProxy/tps_ap_dose_mesh_helper.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE
DoseMeshGraphicObject::DoseMeshGraphicObject()
    :m_pStrips(nullptr), m_pAna(nullptr),
     mpVertexArray(nullptr), mpNormalArray(nullptr),
     mpIndexArray(nullptr){

    m_fRed = 1.0f;
    m_fGreen = 0.0f;
    m_fBlue = 0.0f;
    m_fAlpha = 0.5f;
    m_bValid = false;

    mspecf = 0.7f;
    mambf = 0.6f;
    mdifff = 0.4f;
    mshininess = 30.0f;
    msmoothness = 1;
    mDisplayMode = 0;
    museCameraLight = true;
    mdisplayVertexCube = false;
    mStartStripIndex = 0;
    mEndStripIndex = 1000;
}

DoseMeshGraphicObject::~DoseMeshGraphicObject() {
    // delete mesh when deleting the graphic object
    if (m_pStrips != nullptr)
    {
        delete m_pStrips;
        m_pStrips = nullptr;
    }

    if (m_pAna != nullptr)
    {
        for (int t = 0; t < m_pAna->contour_count; t++)
        {
            plunc::CONTOUR_STC* pCon = m_pAna->contours + t;
            free(pCon->x);
            free(pCon->y);
        }
        free(m_pAna->contours);
        delete m_pAna;
    }
    if(mpVertexArray != nullptr)
    {
        delete []mpVertexArray; mpVertexArray = nullptr;
    }
    if(mpNormalArray != nullptr)
    {
        delete []mpNormalArray; mpNormalArray = nullptr;
    }
    if(mpIndexArray != nullptr)
    {
        delete []mpIndexArray; mpIndexArray = nullptr;
    }
}

bool DoseMeshGraphicObject::Initialize() {
     return true;
}

bool DoseMeshGraphicObject::Finalize() {
    return true;
}


void DoseMeshGraphicObject::SetDoseMesh(STC_STRIPS* pMesh)
{
    if (m_pStrips != nullptr)
    {
        delete m_pStrips;
    }
    if(mpVertexArray != nullptr)
    {
        delete []mpVertexArray; mpVertexArray = nullptr;
    }
    if(mpNormalArray != nullptr)
    {
        delete []mpNormalArray; mpNormalArray = nullptr;
    }
    if(mpIndexArray != nullptr)
    {
        delete []mpIndexArray; mpIndexArray = nullptr;
    }
    m_pStrips = pMesh;
    GetElementArray();
}

void DoseMeshGraphicObject::SetDoseAnastruct(plunc::ANASTRUCT* pAna)
{
    if (m_pAna != nullptr)
    {
        for (int t = 0; t < m_pAna->contour_count; t++)
        {
            plunc::CONTOUR_STC* pCon = m_pAna->contours + t;
            free(pCon->x);
            free(pCon->y);
        }
        free(m_pAna->contours);
        delete m_pAna;
    }
    m_pAna = pAna;
}

void DoseMeshGraphicObject::SetDoseColor(float fRed, float fGreen, float fBlue, float fAlpha)
{
    m_fRed = fRed;
    m_fGreen = fGreen;
    m_fBlue = fBlue;
    m_fAlpha = fAlpha;
}

void DoseMeshGraphicObject::GetDoseColor(float* fRed, float* fGreen, float* fBlue, float* fAlpha)
{
    if (fRed) *fRed = m_fRed;
    if (fGreen) *fGreen = m_fGreen;
    if (fBlue) *fBlue = m_fBlue;
    if (fAlpha) *fAlpha = m_fAlpha;
}

void DoseMeshGraphicObject::GetElementArray()
{
    STC_STRIPS* pStrips = this->GetDoseMesh();
    if (pStrips == nullptr)
    {
        m_count = 0;
    }

    int nQuadPoint = 0;
    for (int cloop = 0; cloop < pStrips->nCount; cloop++) 
    {
        STC_STRIP* pStrip = pStrips->pStrip + cloop;
        if (pStrip == nullptr)
            continue;
        nQuadPoint += pStrip->count;
    }
    if (nQuadPoint <= 0)
        m_count = 0;

    mpVertexArray = new float[nQuadPoint * 3];
    mpNormalArray = new float[nQuadPoint * 3];
    int nVertexIndex = 0;
    int nNormalIndex = 0;

    STC_STRIP* pStrip = nullptr;
    plunc::PNT3D p1, n1;
    for (int cloop = 0; cloop < pStrips->nCount; cloop++) 
    {
        pStrip = pStrips->pStrip + cloop;
        for (int loop = 0; loop < pStrip->count; loop++) 
        {
            p1 = pStrip->dpt[loop];
            n1 = pStrip->norm[loop];
            mpNormalArray[nNormalIndex++] = n1.x;
            mpNormalArray[nNormalIndex++] = n1.y;
            mpNormalArray[nNormalIndex++] = n1.z;

            mpVertexArray[nVertexIndex++] = p1.x;
            mpVertexArray[nVertexIndex++] = p1.y;
            mpVertexArray[nVertexIndex++] = p1.z;
        }
    }
    m_count = nQuadPoint;
    if(m_count > 0)
    {
        mpIndexArray = new unsigned int[m_count];
        for (int i = 0; i < m_count; i++)
            mpIndexArray[i] = i;
    }
}

int DoseMeshGraphicObject::GetElementBuffer(float** pVertexArray, float** pNormalArray, unsigned int** pIndexArray)
{
    *pNormalArray = mpNormalArray;
    *pVertexArray = mpVertexArray;
    *pIndexArray = mpIndexArray;
    return m_count;
}

TPS_END_NAMESPACE
