////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_fusionchessgraphicobject.h
/// 
///  \brief class FusionChessGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/05/27
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_FUSIONCHESSGRAPHICOBJECT_H_
#define TPS_RL_FUSIONCHESSGRAPHICOBJECT_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "McsfMedViewer3DArithmetic/point3d.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

#define MIN_SQUARE_SIZE 0.1 //screen ratio

TPS_BEGIN_NAMESPACE  // begin namespace tps

typedef enum{
    NORMAL_CHESS = 0,
    ROW_ONLY,
    COLUMN_ONLY,
}CHESS_DISPLAY_MODE;

typedef enum{
    CHESS_IDLE,
    CHESS_TRANSLATE,
    CHANGE_SQUARE_SIZE,
}CHESS_EDIT_MODE;

/// \class TpsMprFusionGraphicObject
/// 
/// \brief *****
class TPS_RL_DECLSPEC FusionChessGraphicObject : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    FusionChessGraphicObject() : mChessCenter(0, 0, 0), mSquareSize(0.5f), 
        mDisplayMode(NORMAL_CHESS), mEditMode(CHESS_IDLE), mHitNum(HIT_NUM_NONE){}
    virtual ~FusionChessGraphicObject() {}

    //chess center is in world coordinate system
    inline void SetChessCenter(const Mcsf::MedViewer3D::Point3D& center){
        mChessCenter = center;
    }
    inline Mcsf::MedViewer3D::Point3D GetChessCenter() const{
        return mChessCenter;
    }
    //square size is length in world coordinate system
    inline void SetSquareSize(float size) { mSquareSize = size; }
    inline float GetSquareSize() const { return mSquareSize; }

    inline void SetDisplayMode(CHESS_DISPLAY_MODE mode) { mDisplayMode = mode; }
    inline CHESS_DISPLAY_MODE GetDisplayMode() const { return mDisplayMode; }

    inline void SetEditMode(CHESS_EDIT_MODE mode) { mEditMode = mode; }
    inline CHESS_EDIT_MODE GetEditMode() const { return mEditMode; }

    inline int IsHit() const { return mHitNum; }
    inline void SetHit(int hit){ mHitNum = hit; }
private:
    Mcsf::MedViewer3D::Point3D  mChessCenter;
    float                       mSquareSize;
    CHESS_DISPLAY_MODE          mDisplayMode;
    CHESS_EDIT_MODE             mEditMode;
    int             mHitNum;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(FusionChessGraphicObject);
};

TPS_END_NAMESPACE  // end namespace tps

#endif