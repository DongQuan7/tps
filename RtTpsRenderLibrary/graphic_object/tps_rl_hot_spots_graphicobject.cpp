////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_hotspots_graphicobject.cpp
/// 
///  \brief class CrosshairGraphicObejct implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/07/07
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_hot_spots_graphicobject.h"

TPS_BEGIN_NAMESPACE


HotSpotsGraphicObject::HotSpotsGraphicObject(): m_bVisible(true)
{ 

}

HotSpotsGraphicObject::~HotSpotsGraphicObject() 
{

}

bool HotSpotsGraphicObject::Initialize() {
    return true;
}

bool HotSpotsGraphicObject::Finalize() {
    return true;
}


HotColdContourPointer HotSpotsGraphicObject::GetVecPointsByNameAndPlanUid(const std::string& windowName, const std::string& planUid)
{
    if (planUid.empty())
        return nullptr;

    std::map<std::string, HotColdContourPointer>::iterator it;
    if (windowName == "Axial")
    {
        if (m_spVecPoints_Axial_Map.size() <= 0)
        {
            return nullptr;
        }
        else
        {
            it = m_spVecPoints_Axial_Map.find(planUid);
            if (it == m_spVecPoints_Axial_Map.end())
            {
                return nullptr;
            }
            return m_spVecPoints_Axial_Map[planUid];
        }
    }
    else if (windowName == "Sagittal")
    {
        if (m_spVecPoints_Sagittal_Map.size() <= 0)
        {
            return nullptr;
        }
        else
        {
            it = m_spVecPoints_Sagittal_Map.find(planUid);
            if (it == m_spVecPoints_Sagittal_Map.end())
            {
                return nullptr;
            }
            return m_spVecPoints_Sagittal_Map[planUid];
        }
    }
    else if (windowName == "Coronal")
    {
        if (m_spVecPoints_Coronal_Map.size() <= 0)
        {
            return nullptr;
        }
        else
        {
            it = m_spVecPoints_Coronal_Map.find(planUid);
            if (it == m_spVecPoints_Coronal_Map.end())
            {
                return nullptr;
            }
            return m_spVecPoints_Coronal_Map[planUid];
        }
    }
    else
    {
        return nullptr;
    }
}


void HotSpotsGraphicObject::ClearVecPointsByPlanUid(const std::string& planUid)
{
    if (planUid.empty()) return;

    std::map<std::string, HotColdContourPointer>::iterator it = m_spVecPoints_Axial_Map.find(planUid);
    if (it != m_spVecPoints_Axial_Map.end())
    {
        m_spVecPoints_Axial_Map.erase(it);
    }

    it = m_spVecPoints_Coronal_Map.find(planUid);
    if (it != m_spVecPoints_Coronal_Map.end())
    {
        m_spVecPoints_Coronal_Map.erase(it);
    }

    it = m_spVecPoints_Sagittal_Map.find(planUid);
    if (it != m_spVecPoints_Sagittal_Map.end())
    {
        m_spVecPoints_Sagittal_Map.erase(it);
    }
}

TPS_END_NAMESPACE