////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_series_table_cache_graphic_object.h
/// 
///  \brief class SeriesTableCacheGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/07/18
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SERIES_TABLE_CACHE_GRAPHIC_OBJECT_H_
#define TPS_RL_SERIES_TABLE_CACHE_GRAPHIC_OBJECT_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class SeriesTableCacheGraphicObject
/// 
/// \brief *****
class TPS_RL_DECLSPEC SeriesTableCacheGraphicObject : public TpsGraphicObjectBase
{
public:
    //the constructor and destructor
    SeriesTableCacheGraphicObject();
    virtual ~SeriesTableCacheGraphicObject();

    void SetCache(unsigned char* mask, int width, int height, 
        const Mcsf::MedViewer3D::Matrix4x4& matWorldToNormalPlane);

    void GetCache(unsigned char*& mask, int& width, int& height, 
        Mcsf::MedViewer3D::Matrix4x4& matWorldToNormalPlane) const;

    void SetColor(const double* color);

    void GetColor(double* color) const;

    void Clear();

    bool IsTableEmpty() const;

    void SetIsTableEmpty(bool flag);
private:
    double mColor[4];
    unsigned char* mCache;
    int mWidth;
    int mHeight;
    Mcsf::MedViewer3D::Matrix4x4 mWorldToNormalPlaneTM;

    bool mIsTableEmpty;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(SeriesTableCacheGraphicObject);
};

TPS_END_NAMESPACE  // end namespace tps

#endif