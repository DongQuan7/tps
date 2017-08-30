//////////////////////////////////////////////////////////////////////////
///defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  yilong.cheng mailto:yilong.cheng@united-imaging.com
///
///  \file    tps_rl_TpsEasyPlanDrrRenderStrategy.h
///  \brief   DRR RenderStrategy
///
///  \version 1.0
///  \date   2014/09/29
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_RL_TPSEASYPLANDRRRENDERSTRATEGY_H_
#define TPS_RL_TPSEASYPLANDRRRENDERSTRATEGY_H_

#include "tps_rl_defs.h"

//ZHENGHE
#include "RtTpsFramework/tps_fw_render_strategy_base.h"
#include "Mcsf3DArithmetic/mcsf_3d_matrix3x3.h"
#include "Mcsf3DArithmetic/mcsf_3d_vector3f.h"

class Matrix3f;

TPS_BEGIN_NAMESPACE

class NormalGroupCollection;
class EasyPlanDrrGraphicObejct;
class EasyPlanDrrGraphicObjectCollection;

class TPS_RL_DECLSPEC TpsEasyPlanDrrRenderStrategy :public TpsRenderStrategyBase {    
public:  
    TpsEasyPlanDrrRenderStrategy(void);    
    virtual ~TpsEasyPlanDrrRenderStrategy(void);
    virtual int CacheChanges();
    virtual void Resize(int iWidth, int iHeight);
    virtual void Initialize();
    virtual void Finalize(); 
    void SetDRRGOCollection(std::shared_ptr<EasyPlanDrrGraphicObjectCollection> easyPlanDrrGoCollection);
    void SetNGGOCollection(std::shared_ptr<NormalGroupCollection> normalGroupGOCollection);

protected:
    virtual void Render_i();
private:
    void CreateImage(const std::shared_ptr<EasyPlanDrrGraphicObejct>& drrGO);
    void DrawTexture(const std::shared_ptr<EasyPlanDrrGraphicObejct>& drrGO);
    std::shared_ptr<EasyPlanDrrGraphicObejct> GetCurrentDrrGO() const;

private:
    std::shared_ptr<EasyPlanDrrGraphicObjectCollection>      mEasyPlanDrrGoCollection;
    std::shared_ptr<NormalGroupCollection>                   mNormalGroupGOCollection;
    int                                                      mImageWidth;
    int                                                      mImageHeight;
    Mcsf::Matrix3f mDrrRegistrationMatrix3f;
    Mcsf::Matrix3f mManualFusionMatrix3f;
    unsigned  int                                            mTextureID;


};

TPS_END_NAMESPACE

#endif  //TPS_RL_IGRTMPRRENDERSTRATEGY_H_
