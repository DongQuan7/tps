////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  Qiangqiang ZHOU qiangqiang.zhou@united-imaging.com
/// 
///  \file tps_rl_ct2density_graphicobject.h
/// 
///  \brief class Ct2DensityGraphicObejct declaration 
/// 
///  \version 1.0
/// 
///  \date    July 24, 2014
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_CT2DENSITY_GRAPHICOBJECT_H_
#define TPS_RL_CT2DENSITY_GRAPHICOBJECT_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"

namespace Mcsf{
    namespace MedViewer3D{
        class Texture1D;
    }
}

TPS_BEGIN_NAMESPACE
class TPS_RL_DECLSPEC Ct2DensityGraphicObejct : 
    public TpsGraphicObjectBase{

public:
    Ct2DensityGraphicObejct();
    virtual ~Ct2DensityGraphicObejct();

    //////////////////////////////////////////////////////////////////////////
    void SetCt2Density(float* pCt2Density, int ct2DensitySize);
    void GetCt2Density(float** pCt2Denstiy, int* ct2DensitySize);

    void SetCT2DensityTableUid(const std::string& uid);
    std::string GetCT2DensityTableUid();

    void SetElectronEnergy(float electronEnergy);
    float GetElectronEnergy();

    std::shared_ptr<Mcsf::MedViewer3D::Texture1D> GetTexture();

    bool LoadTexture(float thresholdMin, float thresholdMax);

private:
    std::string                         mCt2DensityUid;
    float*                              mCt2Density;
    int                                 mCt2DensitySize;
    std::shared_ptr<Mcsf::MedViewer3D::Texture1D>   mTex1D;
    float                               mElectronEnergy;
};

TPS_END_NAMESPACE//TPS_RL_CT2DENSITY_GRAPHICOBJECT_H_

#endif
