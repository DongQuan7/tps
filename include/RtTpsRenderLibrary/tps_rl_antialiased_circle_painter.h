////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_antialiased_circle_painter.h
/// 
///  \brief class AntialiasedCirclePainter declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/12/11
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_ANTIALIASED_CIRCLE_PAINTER_H_
#define TPS_RL_ANTIALIASED_CIRCLE_PAINTER_H_

#include "tps_rl_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class GLProgram;
/// \class AntialiasedCirclePainter
/// 
/// \brief *****
class TPS_RL_DECLSPEC AntialiasedCirclePainter{
public:
    //the constructor and destructor
    AntialiasedCirclePainter();
    ~AntialiasedCirclePainter();

    bool Initialize();

    //void SetMVPMatrix(const Mcsf::MedViewer3D::Matrix4x4& mvp);

    //the screen coordinate(pixel unit), centers(x, y), radius(x), colors(r, g, b)
    void SetParas(float* centers, float* radius, 
        float* colors, bool* isFilledFlag, int count, bool drawShadow = true);

    void Render();

private:
    bool                            mInitialized;

    int                             mCount;
    bool                            mDrawShadow;

    GLProgram*                      mProgram;
    unsigned int                    mVAOID;
    unsigned int                    mUniformLocations[7];
    unsigned int                    mTexID[4];
    unsigned int                    mBufferID[2];
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(AntialiasedCirclePainter);
};

TPS_END_NAMESPACE  // end namespace tps

#endif