////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_series_table_cache_graphic_object.cpp
/// 
///  \brief class SeriesTableCacheGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/07/18
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RtTpsRenderLibrary/tps_rl_series_table_cache_graphic_object.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

SeriesTableCacheGraphicObject::SeriesTableCacheGraphicObject() : 
    mCache(nullptr), mWidth(0), mHeight(0), mIsTableEmpty(true)
{
    memset(mColor, 0, sizeof(mColor));
}

SeriesTableCacheGraphicObject::~SeriesTableCacheGraphicObject()
{
    Clear();
}

void SeriesTableCacheGraphicObject::SetCache(
    unsigned char* mask, int width, int height, 
    const Mcsf::MedViewer3D::Matrix4x4& matWorldToNormalPlane)
{
    if (mCache == nullptr || width * height != mWidth * mHeight){
        delete[] mCache;
        mCache = new unsigned char[width * height];
    }

    mWidth = width;
    mHeight = height;
    memcpy(mCache, mask, width * height * sizeof(unsigned char));
    mWorldToNormalPlaneTM = matWorldToNormalPlane;
}

void SeriesTableCacheGraphicObject::GetCache(
    unsigned char*& mask, int& width, int& height, 
    Mcsf::MedViewer3D::Matrix4x4& matWorldToNormalPlane) const
{
    mask = mCache;
    width = mWidth;
    height = mHeight;
    matWorldToNormalPlane = mWorldToNormalPlaneTM;
}

void SeriesTableCacheGraphicObject::SetColor(const double* color)
{
    memcpy(mColor, color, sizeof(mColor));
}

void SeriesTableCacheGraphicObject::GetColor(double* color) const
{
    memcpy(color, mColor, sizeof(mColor));
}

void SeriesTableCacheGraphicObject::Clear(){

    if (mCache)
    {
        TPS_DEL_ARRAY(mCache);
    }

    mWidth = 0;
    mHeight = 0;
    mWorldToNormalPlaneTM.SetIdintity();
    memset(mColor, 0, sizeof(mColor));
    mIsTableEmpty = true;
}

bool SeriesTableCacheGraphicObject::IsTableEmpty() const
{
    return mIsTableEmpty;
}

void SeriesTableCacheGraphicObject::SetIsTableEmpty(bool flag)
{
    mIsTableEmpty = flag;
}
TPS_END_NAMESPACE  // end namespace tps