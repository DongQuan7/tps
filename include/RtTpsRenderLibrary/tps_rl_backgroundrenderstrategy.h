

#ifndef TPS_RL_BACKGROUNDRENDERSTRATEGY_H_
#define TPS_RL_BACKGROUNDRENDERSTRATEGY_H_

#include "tps_rl_defs.h"

#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC BackgroundRenderStrategy :
    public TpsRenderStrategyBase {
public:
    BackgroundRenderStrategy();
    virtual ~BackgroundRenderStrategy();

    virtual void Initialize();
    virtual void Finalize();
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);
    virtual void SetDirty(bool);

protected:
    virtual void Render_i();

private:
    TPS_DISALLOW_COPY_AND_ASSIGN(BackgroundRenderStrategy);

};

TPS_END_NAMESPACE

#endif