////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_beamrenderstrategy.h
/// 
///  \brief class TpsBeamRenderStrategy declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/11
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_BEAMRENDERSTRATEGY_H
#define TPS_RL_BEAMRENDERSTRATEGY_H

#include "tps_rl_defs.h"

#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_render_strategy_base.h"
#include "RtTpsRenderLibrary/tps_rl_graphicobjecttypedefines.h"
#include <vector>
#include <list>
#include <map>

namespace Viewer3D = Mcsf::MedViewer3D;
namespace Mcsf {
    namespace MedViewer3D {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE
struct HitTestResult;
class TpsBeamGraphicObject;
class TpsSegmentGraphicObject;
class MPRGraphicObject;
class NormalGroupCollection;
struct ArcBeamEditCache;
class AntialiasedCirclePainter;
struct CombinationEdges;

typedef std::shared_ptr<TpsBeamGraphicObject> BeamGoPtr;
typedef std::shared_ptr<TpsSegmentGraphicObject> SegmentGoPtr;

struct Line
{
    Viewer3D::Point2D start;
    Viewer3D::Point2D end;
};
struct BeamHitData
{
    std::string uid;
    //arc or imrt
    int type;
    Line startLine;
    //if arc
    Line endLine;
};
class TPS_RL_DECLSPEC TpsBeamRenderStrategy : public TpsRenderStrategyBase
{
public:
    //////////////////////////////////////////////////////////////////////////
    // \brief constructor
    TpsBeamRenderStrategy();

    //////////////////////////////////////////////////////////////////////////
    // \brief destructor
    virtual ~TpsBeamRenderStrategy(void);

    virtual void Initialize();
    virtual void Finalize();
    virtual int  CacheChanges();
    virtual void Resize(int iWidth, int iHeight);

    //////////////////////////////////////////////////////////////////////////
    // \brief set related mpr graphic object
    // \param[in] shared pointer of mpr graphic object;
    void SetMprGraphicObject(std::shared_ptr<MPRGraphicObject>);

    //////////////////////////////////////////////////////////////////////////
    // \brief set related volume graphic object;
    // \param[in] shared pointer of volume graphic;
    void SetVolumeGraphicObject(std::shared_ptr<Viewer3D::GraphicObjVolume>);

    void SetSectionType(FIRST_POSTFIX_COMPONENT section);

    inline void SetNormalGroupCollection(std::shared_ptr<NormalGroupCollection> ngCollection){
        mNormalGroupCollection = ngCollection;
    }

    void HitTest(double x, double y, HitTestResult& hitRes);
protected:
    virtual void Render_i();

private:
    //////////////////////////////////////////////////////////////////////////
    // \brief calculate world to view transformation matrix based on MPR plane
    void CalculateWorld2ViewTM_i();

    //粗判断Mpr平面是否与Beam合并野相交
    bool IfMprIntersectsWithBeamCombinationFields_i(const BeamGoPtr& beam, 
        const Viewer3D::Matrix4x4& world2Beam, double sad) const;

    void AdjustMpr_i(Mcsf::MedViewer3D::Point3D* vertices) const;

    void DrawBeamLines_i(
        const BeamGoPtr& beam, 
        const SegmentGoPtr& segment, 
        const Viewer3D::Matrix4x4& patient2World,
        const Viewer3D::Matrix4x4& beam2Patient,
        const Viewer3D::Matrix4x4& beam2View,
        bool isActive, 
        bool isHittable, 
        bool isToDrawCentralLine, 
        double sad);
    void DrawAccessories_i(const BeamGoPtr& beam, const SegmentGoPtr& segment, double sad);
    void DrawWedge_i(const BeamGoPtr& beam, const SegmentGoPtr& segment);
    void DrawBlock_i(const BeamGoPtr& beam, const SegmentGoPtr& segment, double sad);
    void RenderIsoCenters_i();

    std::vector<Viewer3D::Point3D> ComputeBeamLine_i(
        const std::shared_ptr<TpsSegmentGraphicObject>& segment, 
        const Viewer3D::Matrix4x4& matBeam2View);

    bool ComputeIntersectionLineInView_i(const Viewer3D::Point3D& ptSource, 
        const Viewer3D::Vector3D& vecFirst, const Viewer3D::Vector3D& vecSecond, 
        Viewer3D::Point3D& start, Viewer3D::Point3D& end);

    bool IsInWindowRange_i(const Viewer3D::Point3D& point);

    void DrawArc_i(const std::shared_ptr<TpsBeamGraphicObject>& beamGo, 
        const Viewer3D::Matrix4x4& matPatientToWorld, bool hittable, double sad);

    void DrawEditModeArc_i(const std::shared_ptr<TpsBeamGraphicObject>& beamGo, 
        const Viewer3D::Matrix4x4& matPatientToWorld, double sad);

    void DrawArc_i(const std::vector<double>& startGantryAngles, 
        const std::vector<double>& endGantryAngles, 
        bool isClockwise, const Viewer3D::Matrix4x4& matStartGantryToWorld, 
        const Viewer3D::Matrix4x4& matEndGantryToWorld, double sad);

    void DrawCentralLine_i(const std::string& beamUid, 
        const Viewer3D::Matrix4x4& matPatientToWorld, 
        const Viewer3D::Matrix4x4& matBeamToPatient, 
        const std::string& isoCenterUid, bool isActive, bool hittable, double sad);

    void DrawArrow_i(const std::shared_ptr<TpsBeamGraphicObject>& beam, 
        const Viewer3D::Matrix4x4& matPatientToWorld, double sad);

    void DrawSingleWedge_i(Viewer3D::Point3D* vertices, 
        unsigned int* planeIndex, unsigned int planeBufferCount, 
        unsigned int* lineIndex, unsigned int lineBufferCount, double* color);
    bool HitLine_i(const Viewer3D::Point2D& point, const Line line);
private:
    std::shared_ptr<NormalGroupCollection>        mNormalGroupCollection;
    std::shared_ptr<MPRGraphicObject>             mMprGO;
    std::shared_ptr<Viewer3D::GraphicObjVolume>   mVolumeGO;

    FIRST_POSTFIX_COMPONENT                       mSectionType;
    Viewer3D::Matrix4x4                           mWorld2View;

    unsigned int mWedgeVBO[2];
    unsigned int mWedgeEBO;
    unsigned int mBlockVBO;
    unsigned int mBlockEBO;

    //hit test data
    std::list<BeamHitData>  mHitTestData;
    //rendering temp data
    struct IsoCenterTempData
    {
        float x;
        float y;
        bool isFilled;
        bool isActive;
    };
    std::map<std::string, IsoCenterTempData> mIsoCenterTempData;
    AntialiasedCirclePainter* mPointPainter;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsBeamRenderStrategy);
};
TPS_END_NAMESPACE
#endif
