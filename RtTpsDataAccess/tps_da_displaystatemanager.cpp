////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_da_displaystatemanager.cpp
/// 
///  \brief class FusionSecondaryDataManager implementation 
/// 
///  \version 1.0
/// 
///  \date    2014/01/21
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsDataAccess/tps_da_displaystatemanager.h"

#include "RtTpsDataAccess/tps_da_idatarepository.h"
//#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps
	//Display attributes相关的初始值同DisplayAttributesSharedService.cs
	//如果修改需要相应修改
TpsDisplayStateManager::TpsDisplayStateManager()
    : mBEVViewType(BEV_VIEW),
    mBevEditTarget(BEV_EDIT_TARGET_MLC), 
	mBEVDisplayType(DRR),
    mMoveVOIChanged(false),
	mDoselineRenderMode(LINE),
	mDoselineTransparency(0.6f),
	mDoselineTransparency3D(0)
{
}

TpsDisplayStateManager::~TpsDisplayStateManager()
{

}

void TpsDisplayStateManager::SetBevViewType(BEV_VIEW_TYPE type)
{
    mBEVViewType = type;
}

BEV_VIEW_TYPE TpsDisplayStateManager::GetBevViewType() const
{
    return mBEVViewType;
}

void TpsDisplayStateManager::SetBevDisplayType(BEV_DISPLAY_TYPE type)
{
	mBEVDisplayType = type;
}

BEV_DISPLAY_TYPE TpsDisplayStateManager::GetBevDisplayType() const
{
	return mBEVDisplayType;
}

void TpsDisplayStateManager::SetDoselineRenderModeType(DOSELINE_RENDERMODE type)
{
	mDoselineRenderMode = type;
}

DOSELINE_RENDERMODE TpsDisplayStateManager::GetDoselineRenderModeType() const
{
	return mDoselineRenderMode;
}

float TpsDisplayStateManager::GetDoselineTransparency() const
{
	return mDoselineTransparency;
}

void TpsDisplayStateManager::SetDoselineTransparency(float target)
{
	mDoselineTransparency = target;
}

float TpsDisplayStateManager::GetDoselineTransparency3D() const
{
	return mDoselineTransparency3D;
}

void TpsDisplayStateManager::SetDoselineTransparency3D(float target)
{
	mDoselineTransparency3D = target;
}

void TpsDisplayStateManager::SetBevEditTarget(BEV_EDIT_TARGET target)
{
    mBevEditTarget = target;
}

BEV_EDIT_TARGET TpsDisplayStateManager::GetBevEditTarget() const
{
    return mBevEditTarget;
}

bool TpsDisplayStateManager::GetMoveVoiFlag() const
{
    return mMoveVOIChanged;
}

void TpsDisplayStateManager::SetMoveVoiFlag(bool flag)
{
    mMoveVOIChanged = flag;
}

int TpsDisplayStateManager::GetMprSection() const
{
	return mMprSection;
}

void TpsDisplayStateManager::SetMprSection(int flag)
{
	mMprSection = flag;
}

float TpsDisplayStateManager::GetMPRTransparency3D() const
{
	return mMPRTransparency3D;
}

void TpsDisplayStateManager::SetMPRTransparency3D(float target)
{
	mMPRTransparency3D = target;
}

TPS_END_NAMESPACE  // end namespace tps
