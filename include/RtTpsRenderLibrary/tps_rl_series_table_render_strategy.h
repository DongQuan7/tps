////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_series_table_render_strategy.h
/// 
///  \brief class SeriesTableRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/07/18
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_SERIES_TABLE_RENDER_STRATEGY_H_
#define TPS_RL_SERIES_TABLE_RENDER_STRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_render_strategy_base.h"

#include <vector>

namespace Mcsf
{
    class Point2f;
    namespace MedViewer3D
    {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class MPRGraphicObject;
class SeriesTableCacheGraphicObject;
typedef std::shared_ptr<MPRGraphicObject> MPRGOPtr;
typedef std::shared_ptr<Mcsf::MedViewer3D::GraphicObjVolume> VolumeGOPtr;
typedef std::shared_ptr<SeriesTableCacheGraphicObject> SeriesTableCacheGOPtr;

/// \class SeriesTableRenderStrategy
/// 
/// \brief *****
class TPS_RL_DECLSPEC SeriesTableRenderStrategy : public TpsRenderStrategyBase
{
public:
    //the constructor and destructor
    SeriesTableRenderStrategy();
    virtual ~SeriesTableRenderStrategy();

    /// \fun    CacheChanges
    /// \brief  Store all changed items locally for later rendering.
    ///         This is necessary because the rendering is asynchronous.
    //virtual int CacheChanges() = 0;
    virtual int CacheChanges();

    /// \fun    Initialize
    /// \brief  For GPU implementation, any GL resources specific to the
    ///         strategy or the render context should be created in the function
    virtual void Initialize();

    /// \fun    Finalize
    /// \brief  For GPU implementation, the resources created in the Initialize
    ///         function should be cleaned up
    virtual void Finalize();

    //////////////////////////////////////////////////////////////////////////
    // \brief set volume graphic object
    // \param[in] volume  the volume graphic object pointer
    void SetVolumeGO(VolumeGOPtr volume);

    //////////////////////////////////////////////////////////////////////////
    // \brief set MPR graphic object
    // \param[in] mpr  the mpr graphic object pointer
    void SetMPRGO(MPRGOPtr mpr);

    void SetSeriesTableCacheGO(SeriesTableCacheGOPtr tableCache);

protected:
    virtual void Render_i();

    void CalcViewProjectionMatrix_i();

    void DrawFill(const std::vector<std::vector<Mcsf::Point2f>>& contours);
private:
    MPRGOPtr                           mMPRGO;
    VolumeGOPtr                        mVolumeGO;
    SeriesTableCacheGOPtr              mSeriesTableCacheGO;

    Mcsf::MedViewer3D::Matrix4x4       mViewProjectionMat;
    bool                               mNeedRecalcViewProj;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(SeriesTableRenderStrategy);
};

TPS_END_NAMESPACE  // end namespace tps

#endif