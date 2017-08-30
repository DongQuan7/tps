////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Miao chenfeng  chenfeng.miao@united-imaging.com
/// 
///  \file tps_rl_beamgraphicobject.h
/// 
///  \brief class TpsBeamGraphicObject declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/11
////////////////////////////////////////////////////////////////


#ifndef TPS_RL_BEAMGRAPHICOBJECT_H
#define TPS_RL_BEAMGRAPHICOBJECT_H

#include "tps_rl_defs.h"
#include "McsfMedViewer3DArithmetic/arithmetic.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_common_enums.h"
#include <list>
#include <vector>

namespace Viewer3D = Mcsf::MedViewer3D;

TPS_BEGIN_NAMESPACE
class TpsSegmentGraphicObject;
class TpsWedgeGraphicObject;
class ApplicatorGraphicObject;
class BlockGraphicObject;
class FluenceMapEntity;
struct StcMLC;

enum BEAM_TYPE
{
    IMRT,
    ARC
};

struct ArcBeamEditCache
{
    bool    _isClockwise;
    bool   _isSimpleArc;
    bool   _isToRotateStart;
    double _minAngle;
    double _maxAngle;
    double _cwRange;
    double _ccwRange;
    double _startGantry;
    double _arcLength;
    //std::vector<double> _gantryAngles;
    Viewer3D::Matrix4x4 _matStartGantry2Patient;
    Viewer3D::Matrix4x4 _matEndGantry2Patient;
    Viewer3D::Matrix4x4 _matPatient2OrgGantry;

    bool    _isEditingInfoInit;
    double  _maxArcLength;
    bool    _isCloseToFirstEndium;
    bool    _isOnEdge;
};

//以X方向叶片说明，start为这个Edge Y起始坐标，end为Y终止坐标，location是叶片x坐标值
struct Edge
{
    float start;
    float end;
    float location;
};

//合并后的叶片边缘，不包含闭合的叶片，包含正方向和负方向的边缘组，组内排序均是start从小到大
//需要注意：这里edge与edge之间并不一定会彼此相连，即上一个edge的end并非一定等于下一个edge的start
//因为子野形状存在多个孔的情况，
//所以在以后合成outline时需要小心处理（现在还没有显示combination field的需求）
struct CombinationEdges
{
    std::vector<Edge> negEdges;
    std::vector<Edge> posEdges;
};
struct Rectangle
{
    //x1, x2, y1, y2
    double boundings[4];
};
class TPS_RL_DECLSPEC TpsBeamGraphicObject : public TpsGraphicObjectBase
{
public:
    //the constructor and destructor
    TpsBeamGraphicObject();
    virtual ~TpsBeamGraphicObject();

    //////////////////////////////////////////////////////////////////////////
    //\brief virtual methods derived from GraphicObiBase;
    virtual bool Initialize();
    virtual bool Finalize();

    void SetBeamType(BEAM_TYPE type);

    BEAM_TYPE GetBeamType() const;

    //std::map<std::string,std::shared_ptr<TpsSegmentGraphicObject>> GetSegmentGoList();

    std::vector<std::shared_ptr<TpsSegmentGraphicObject>>  GetSegmentGoList() const; 

    void SetActiveSegmentIndex(int index);
    int GetActiveSegmentIndex() const;
    std::shared_ptr<TpsSegmentGraphicObject> GetActiveSegmentGo();
    void CorrectActiveSegment();

    void AddSegmentGo(std::shared_ptr<TpsSegmentGraphicObject>);

    void SetSegmentGos(const std::vector<std::shared_ptr<TpsSegmentGraphicObject>> &);

    bool DeleteSegmentGo(int index);

    void RemoveAllGos();

    void SetColorForMprWindow(const float color[4]);

    void GetColorForMprWindow(float color[4]);

    void SetColorForBevWindow(const float color[4]);

    void GetColorForBevWindow(float color[4]);

    void SetVisible(bool);

    bool GetVisible() const;

    int GetSegmentGoCount() const;

    //void SetCombinedSegmentGo(std::shared_ptr<TpsSegmentGraphicObject>);

    //std::shared_ptr<TpsSegmentGraphicObject> GetCombinedSegmentGo() const;

    // set/get wedge graphic object
    void SetPhysicalWedgeGO(std::shared_ptr<TpsWedgeGraphicObject> wedge);
    std::shared_ptr<TpsWedgeGraphicObject> GetPhysicalWedgeGO() const;

    void SetVirtualWedgeGO(std::shared_ptr<TpsWedgeGraphicObject> wedge);
    std::shared_ptr<TpsWedgeGraphicObject> GetVirtualWedgeGO() const;

    void SetApplicator(std::shared_ptr<ApplicatorGraphicObject> applicator);
    std::shared_ptr<ApplicatorGraphicObject> GetApplicator() const;

    void SetApertureBlock(std::shared_ptr<BlockGraphicObject> block);
    std::shared_ptr<BlockGraphicObject> GetApertureBlock() const;

    void SetShieldingBlock(std::shared_ptr<BlockGraphicObject> block);
    std::shared_ptr<BlockGraphicObject> GetShieldingBlock() const;

    inline void SetIsJoined(bool isJoined) { mIsJoined = isJoined; }
    inline bool IsJoined() const { return mIsJoined; }

    void SetWeight(float weight);
    float GetWeight() const;

    void SetMU(float mu);
    float GetMU() const;

    void SetName(const std::string& name);
    std::string GetName() const;

    RtDbDef::APERTURE_TYPE GetApertureType() const;
    void SetApertureType(RtDbDef::APERTURE_TYPE type);

    std::string GetUID() const;
    void SetUID(const std::string& uid);

    void SetEnergy(float energy);
    float GetEnergy() const;

    void SetFluenceMapEntity(std::shared_ptr<FluenceMapEntity> fluenceMap);
    std::shared_ptr<FluenceMapEntity> GetFluenceMapEntity() const;

    bool IsArcEditModeEnabled() const;
    void SetArcEditModeEnabled(bool isEnabled);
    ArcBeamEditCache& GetArcEditCache();

    std::string GetIsoCenterUID() const;
    void SetIsoCenterUID(const std::string& isoCenterUID);

    //需要注意的是：对于IMRT beam， CombinationEdges是所有子野形状的并集
    //            而对于ARC beam, 只是当前子野的形状（因为每个子野Gantry角度不同）
    bool UpdateCombinationFieldEdges();
    CombinationEdges GetCombinationFieldEdges() const;
    std::vector<Rectangle> GetCombinationFieldRects() const;

    void SetIECFixToPatientMatrix(const Viewer3D::Matrix4x4& matrix);
    Viewer3D::Matrix4x4 GetIECFixToPatientMatrix() const;
private:

    bool UpdateARCCombinationFieldEdges_i();
    bool UpdateIMRTCombinationFieldEdges_i();

    void CombineSingleLeaf_i(
    const std::vector<StcMLC*>& mlcShapes, 
        int leafNum, 
        std::vector<Edge>& negEdges, 
        std::vector<Edge>& posEdges) const;

    void ComputeLeafEdges_i(
        const std::vector<StcMLC*>& mlcShapes, 
        int leafNum, 
        std::vector<Edge>& negEdges, 
        std::vector<Edge>& posEdges, 
        float& leafWidth) const;
    
    enum CombineDirection
    {
        Negative,
        Positive
    };
    void CombineEdges_i(CombineDirection direction, 
        float leafWidth, std::vector<Edge>& edges) const;
    
    //将边缘线所表示的合并野分割成在Beam坐标系下的多个矩形区域
    void DevideCombinationFieldsToRectangles_i(int leafDirection);

    //没有考虑多孔情况，如果需要这个方法，需要添加相应代码
    std::vector<Mcsf::MedViewer3D::Point3D> ConnectEdgesToLine_i(
        const std::vector<Edge>& negEdges, 
        const std::vector<Edge>& posEdges, 
        int leafDirection);

    std::vector<std::shared_ptr<TpsSegmentGraphicObject>>              mSegmentGoList;
    //std::shared_ptr<TpsSegmentGraphicObject>                           mCombinedSegmentGo;
    float                                                 mColorForMprWindow[4];
    float                                                 mColorForBevWindow[4];
    bool                                                  mIsVisible;
    int                                                   mActiveSegmentGoIndex;
    BEAM_TYPE                                             mType;
    RtDbDef::APERTURE_TYPE                                mApertureType;

    bool                                                  mIsJoined;
    std::shared_ptr<TpsWedgeGraphicObject>             mPhysicalWedgeGo;
    std::shared_ptr<TpsWedgeGraphicObject>             mVirtualWedgeGo;
    std::shared_ptr<ApplicatorGraphicObject>           mApplicator;
    std::shared_ptr<BlockGraphicObject>                mApertureBlock;
    std::shared_ptr<BlockGraphicObject>                mShieldingBlock;
    std::shared_ptr<FluenceMapEntity>                  mFluenceMapEntity;

    float                       mWeight;
    float                       mMU;
    std::string                 mName;
    std::string                 mUID;
    float                       mEnergy;
    std::string                 mIsoCenterUID;

    //cache data for arc beam rotation on MPR plane
    bool                        mArcEditModeEnabled;
    ArcBeamEditCache            mArcBeamEditCache;
    
    //combination edges
    CombinationEdges mCombinationEdges;
    std::vector<Rectangle> mCombinationFieldsRects;

    Viewer3D::Matrix4x4 mIECFixToPatientMat;
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsBeamGraphicObject);
};
TPS_END_NAMESPACE

#endif
