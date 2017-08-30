////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_mprfusionrenderstrategy.h
/// 
///  \brief class FusionRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/05/08
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_MPRFUSIONRENDERSTRATEGY_H_
#define TPS_RL_MPRFUSIONRENDERSTRATEGY_H_

#include "tps_rl_defs.h"

#include "RtTpsRenderLibrary/tps_rl_mprrenderstrategy.h"
#include <vector>
#include <map>

namespace Mcsf
{
    namespace MedViewer3D
    {
        class Point2D;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

struct HitTestResult;
class FusionGraphicObject;
class FusionChessGraphicObject;
class AntialiasedCirclePainter;
typedef std::shared_ptr<FusionGraphicObject> FusionGoPtr;
typedef std::shared_ptr<FusionChessGraphicObject> ChessGoPtr;
/// \class FusionRenderStrategy
/// 
/// \brief *****
class TPS_RL_DECLSPEC FusionRenderStrategy : public MPRRenderStrategy
{
public:
    //the constructor and destructor
    FusionRenderStrategy();
    virtual ~FusionRenderStrategy();

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

    /// \fun    Resize
    /// \brief  The resize method is only needed in gpu implementation
    ///         to reallocate any gpu resource according to the new
    ///         size of the screen
    /// \param[in] iWidth The width of the screen in pixels
    /// \param[in] iHeight The height of the screen in pixels
    virtual void Resize(int iWidth, int iHeight);

    // \brief set fusion graphic object
    inline void SetFusionGraphicObject(FusionGoPtr fusionGo)
    {
        if (mFusionGO != fusionGo)
        {
            mFusionGO = fusionGo;
            mDirty = true;
        }
    }

    inline void SetFloatVolumeGO(VolumePtr volumeGo)
    {
        if (mFloatVolume != volumeGo)
        {
            mFloatVolume = volumeGo;
            mDirty = true;
        }
    }

    inline void SetFloatMprGO(MprGoPtr mprGo)
    {
        if (mFloatMpr != mprGo)
        {
            mFloatMpr = mprGo;
            mDirty = true;
        }
    }

    inline void SetChessGraphicObject(ChessGoPtr chessGo)
    {
        if (mChessGO != chessGo)
        {
            mChessGO = chessGo;
            mDirty = true;
        }
    }

    void HitTest(double x, double y, HitTestResult& hitRes);

protected:
    virtual void Render_i();

    bool PrepareFloatAlgorithm_i();
private:
    void RenderChess_i();
    void RenderBlend_i();

    bool WriteStencilForChess_i();
    bool PrepareChessBoard_i();
    bool RenderChessLine_i();
    void RenderHotSpots_i();
    void DrawCircle_i(float cx, float cy, float r, int num_segments);
    void CircleMidPoint_i(int x, int y, int radius);
    void CirclePlotPoints_i(int x, int y, int deltaX, int deltaY);
private:
    MprAlgorithm*    mFloatAlgorithm;
    AntialiasedCirclePainter* mCirclePainter;

    MprGoPtr            mFloatMpr;
    VolumePtr           mFloatVolume;
    FusionGoPtr         mFusionGO;
    ChessGoPtr          mChessGO;

    //chess board
    bool                mNeedRefreshChessBoard;
    //left, right, bottom, top
    float               mDisplayRectInChessModel[4];
    std::vector<float>  mHorizonalPts;
    std::vector<float>  mVerticalPts;
    float*              mVertexBuffer;
    unsigned int*       mIndexBuffer;
    int                 mIndexCount;
    //hit test data, <Hit test num, chess point(pixel)> map list
    std::map<int, Mcsf::MedViewer3D::Point2D> mHitTestData;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(FusionRenderStrategy);
};

TPS_END_NAMESPACE  // end namespace tps

#endif
