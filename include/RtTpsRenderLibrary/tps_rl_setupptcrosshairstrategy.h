

#ifndef TPS_RL_SETUPPTCROSSHAIRSTRATEGY_H_
#define TPS_RL_SETUPPTCROSSHAIRSTRATEGY_H_

#include "tps_rl_defs.h"

#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"

TPS_BEGIN_NAMESPACE
class EditSetupPtGraphicObject;
class TPS_RL_DECLSPEC TpsSetupPtCrossHairStrategy :
public TpsRenderStrategyBase {
public:
    TpsSetupPtCrossHairStrategy();

    virtual ~TpsSetupPtCrossHairStrategy();

    virtual void Initialize();
    virtual void Finalize();
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);

    void SetGraphicObject(std::shared_ptr<EditSetupPtGraphicObject>);

protected:
    virtual void Render_i();

private:  
    std::shared_ptr<EditSetupPtGraphicObject>   mEditSetUpGo;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsSetupPtCrossHairStrategy);

};


TPS_END_NAMESPACE

#endif