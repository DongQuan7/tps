////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_da_fluence_map_entity.h
/// 
///  \brief class FluenceMapEntity declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/05
////////////////////////////////////////////////////////////////

#ifndef TPS_DA_FLUENCE_MAP_ENTITY_H_
#define TPS_DA_FLUENCE_MAP_ENTITY_H_

#include "tps_fw_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class FluenceMapEntity
/// 
/// \brief *****
class TPS_FW_DECLSPEC FluenceMapEntity{
public:
    //the constructor and destructor
    FluenceMapEntity();
    ~FluenceMapEntity();
    FluenceMapEntity(const FluenceMapEntity& fluenceMap);
    FluenceMapEntity& operator=(const FluenceMapEntity& fluenceMap);

    bool IsValid() const;
    void SetIntensityMap(float startX, float startY, float xSpacing, 
        float ySpacing, int width, int height, float* buffer);
    void Clear();

    //notice that this method will malloc memory & copy the map out
    //you need release the memory by yourself
    bool GetIntensifyBuffer(int& width, int& height, float*& buffer) const;

    bool GetSpacing(float& spacingX, float& spacingY) const;
    bool GetStart(float& startX, float& startY) const;
    bool GetMaxValue(float& max);
    bool GetMinValue(float& min);
private:
    bool    mIsValid;
    float   mSpacing[2];
    float   mStartPoint[2];
    int     mDim[2];
    float*  mIntensityBuffer;
    float   mMax;
    float   mMin;
};

TPS_END_NAMESPACE  // end namespace tps

#endif