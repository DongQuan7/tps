//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  zhiyuan.liu (mailto:zhiyuan.liu@united-imaging.com)
///
///  \file   tps_rl_easyplan_setpoirenderstrategy.h 
///  \brief   set easyplan poi in 2d image, see easy plan
///
///  \version 1.0
///  \date  2014/11/07  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_RL_SETISOCENTERRENDERSTRATEGY_H_
#define TPS_RL_SETISOCENTERRENDERSTRATEGY_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"

TPS_BEGIN_NAMESPACE

class TpsPoiGraphicObjectCollection;
class ITpsDataRepository;
class EasyPlanDrrGraphicObejct;

class TPS_RL_DECLSPEC TpsEasyPlanSetPoiRenderStrategy :
    public TpsRenderStrategyBase {
public:
    /////////////////////////////////////////////////////////////////
    ///  \brief TpsEasyPlanSetPoiRenderStrategy
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return       
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    TpsEasyPlanSetPoiRenderStrategy();
    /////////////////////////////////////////////////////////////////
    ///  \brief Destructor
    ///
    ///  \param[in]    
    ///  \param[out]   
    ///  \return       
    ///  \exceptions:   none  
    /////////////////////////////////////////////////////////////////
    ~TpsEasyPlanSetPoiRenderStrategy();

    void Initialize();
    void Finalize();
    int  CacheChanges();
    void Resize(int iWidth, int iHeight);
    void SetPixelSpaceData(float imagePlanPixelSpacingX, float imagePlanPixelSpacingY);

    void SetPoiGraphicObjectCollection(std::shared_ptr<TpsPoiGraphicObjectCollection>);
    void SetDrrGraphicObject(std::shared_ptr<EasyPlanDrrGraphicObejct>);

protected:
    virtual void Render_i();
private:
    std::shared_ptr<TpsPoiGraphicObjectCollection>       mPoiGraphicObejctColl;
    std::shared_ptr<EasyPlanDrrGraphicObejct>            mDrrGraphicObject;
    int                                                  mImageWidth;
    int                                                  mImageHeight;
    float                                                mImagePlanPixelSpacingX;
    float                                                mImagePlanPixelSpacingY;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsEasyPlanSetPoiRenderStrategy);
};
TPS_END_NAMESPACE

#endif