////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_volume4d_graphicobject.cpp
/// 
///  \brief class Volume4dGraphicObejct implementation 
/// 
///  \version 1.0
/// 
///  \date    2015/07/16
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_volume4d_graphicobject.h"
#include "McsfMedViewer3DGraphicObject/mcsf_viewer3d_graphic_object_volume.h"

TPS_BEGIN_NAMESPACE


Volume4dGraphicObject::Volume4dGraphicObject()
    : m_spMainVolume(nullptr), m_spCurrentVolume(nullptr)
{
}

Volume4dGraphicObject::~Volume4dGraphicObject() {

}

bool Volume4dGraphicObject::Initialize() {
    return true;
}

bool Volume4dGraphicObject::Finalize() {
    return true;
}

void Volume4dGraphicObject::SetCurrentVolume(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spVolumeGO)
{
    m_spCurrentVolume = spVolumeGO;
}

void Volume4dGraphicObject::SetMainVolume(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spVolumeGO)
{
    m_spMainVolume = spVolumeGO;
}

void Volume4dGraphicObject::AddVolume(std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> spVolumeGO)
{
    m_vecVolumePtr.push_back(spVolumeGO);
}

std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> Volume4dGraphicObject::GetMainVolume() const
{
    return m_spMainVolume;
}

std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> Volume4dGraphicObject::GetCurrentVolume() const
{
    return m_spCurrentVolume;
}

void Volume4dGraphicObject::ClearAllVolumes()
{
	m_vecVolumePtr.clear();
	m_spMainVolume = nullptr;
	m_spCurrentVolume = nullptr;
}

TPS_END_NAMESPACE