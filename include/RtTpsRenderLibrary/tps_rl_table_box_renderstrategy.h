////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_table_box_renderstrategy.h
/// 
///  \brief class TableBoxRenderStrategy 
/// 
///  \version 1.0
/// 
///  \date    2014/9/25
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_TABLE_BOX_RENDERSTRATEGY_H_
#define TPS_RL_TABLE_BOX_RENDERSTRATEGY_H_

#include "tps_rl_defs.h"

#include <memory>
#include "RtTpsFramework/tps_fw_render_strategy_base.h"

TPS_BEGIN_NAMESPACE
class ManualRemoveTableGraphicObject;
class MPRGraphicObject;
class TPS_RL_DECLSPEC TableBoxRenderStrategy : public TpsRenderStrategyBase {
public:
    TableBoxRenderStrategy(std::string& strWindowType);
    virtual ~TableBoxRenderStrategy();

    virtual void Initialize();
    virtual void Finalize();
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);

    void SetGraphicObject(std::shared_ptr<ManualRemoveTableGraphicObject>);
    void SetMPRGraphicObject(std::shared_ptr<MPRGraphicObject>);

protected:
    virtual void Render_i();

private:
    std::string m_strWindowType;
    std::shared_ptr<ManualRemoveTableGraphicObject>  m_spTableBoxGraphicObejct;
    std::shared_ptr<MPRGraphicObject> m_spMPRGO;
    TPS_DISALLOW_COPY_AND_ASSIGN(TableBoxRenderStrategy);

};

TPS_END_NAMESPACE

#endif //TPS_RL_TABLE_BOX_RENDERSTRATEGY_H_