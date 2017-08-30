////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_landmarkgraphicobject.h
/// 
///  \brief class LandmarkGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/06/25
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_LANDMARKGRAPHICOBJECT_H_
#define TPS_RL_LANDMARKGRAPHICOBJECT_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsFramework/tps_fw_common_enums.h"

namespace Mcsf{
    namespace MedViewer3D{
        class Point3D;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class LandmarkGraphicObject
/// 
/// \brief *****
class TPS_RL_DECLSPEC LandmarkGraphicObject : public TpsGraphicObjectBase{
public:
    //the constructor and destructor
    LandmarkGraphicObject();
    virtual ~LandmarkGraphicObject();

    void SetImageUID(const std::string& fst, const std::string& snd);
    void GetImageUID(std::string& fst, std::string& snd) const;

    void SetMarkFlag(short flag);
    short GetMarkFlag() const;

    void Enable(bool isEnabled);
    bool IsEnabled() const;

    bool SetLandmarkPoint(int index, const Mcsf::MedViewer3D::Point3D& point);
    bool GetLandmarkPoint(int index, Mcsf::MedViewer3D::Point3D& point) const;

    void SetPointSize(int size);
    int GetPointSize() const;

    void SetSelectedIndex(int index);
    int GetSelectedIndex() const;
private:
    bool                            mIsEnabled;
    short                           mMarkFlag;
    Mcsf::MedViewer3D::Point3D      mLandmarkPoints[LANDMARK_POINT_PAIR_COUNT * 2];
    int                             mSelectedIndex;

    std::string         mFixedImageUID;
    std::string         mFloatImageUID;
    int                 mPointSize;//pixel size

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(LandmarkGraphicObject);
};

TPS_END_NAMESPACE  // end namespace tps

#endif