//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_rl_simulator_mlcrenderstrategy.h 
///  \brief   render mlc shape in simulator 
///
///  \version 1.0
///  \date  2015/02/10  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_RL_SIMULATOR_MLCRENDERSTATEGY_H_
#define TPS_RL_SIMULATOR_MLCRENDERSTATEGY_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"

TPS_BEGIN_NAMESPACE

class NormalGroupCollection;

class TPS_RL_DECLSPEC TpsSimulatorMLCRenderStategy : public TpsRenderStrategyBase {
public:
    /////////////////////////////////////////////////////////////////
    ///  \brief Constructor
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    TpsSimulatorMLCRenderStategy();
    /////////////////////////////////////////////////////////////////
    ///  \brief Destruct
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    ~TpsSimulatorMLCRenderStategy();
    /////////////////////////////////////////////////////////////////
    ///  \brief Override method
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    virtual void Initialize();
    virtual void Finalize();
    virtual void Resize(int iWidth, int iHeight);
    virtual int CacheChanges();

    /////////////////////////////////////////////////////////////////
    ///  \brief Set segment GO, which contains mlc shape
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return	   
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    void SetNormalGroupCollection(std::shared_ptr<NormalGroupCollection> segmentGO );

protected:
    virtual void Render_i();

private:
    std::shared_ptr<NormalGroupCollection>                mNormGroupCollectionGO;

    TPS_DISALLOW_COPY_AND_ASSIGN(TpsSimulatorMLCRenderStategy);
};

TPS_END_NAMESPACE

#endif