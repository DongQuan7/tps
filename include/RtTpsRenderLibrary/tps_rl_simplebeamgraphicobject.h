//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_rl_simplebeamgraphicobject.h
///  \brief  
///  \
///  class  TpsSimpleBeamGraphicObject
///  \version 1.0
///  \date  2014/11/24
///  \
//////////////////////////////////////////////////////////////////////////


#ifndef TPS_RL_SIMPLEBEAM_GRAPHICOBJECT_H
#define TPS_RL_SIMPLEBEAM_GRAPHICOBJECT_H

#include "tps_rl_defs.h"
#include "McsfMedViewer3DArithmetic/matrix_4x4.h"
#include "RtTpsFramework/tps_fw_graphicobjectbase.h"
#include "Mcsf3DArithmetic/mcsf_3d_matrix4x4.h"

namespace Viewer3D = Mcsf::MedViewer3D;

namespace RTFWK{
    class RTFWKDLDOMLCSetting;
};

TPS_BEGIN_NAMESPACE

class TpsSegmentGraphicObject;

class TPS_RL_DECLSPEC TpsSimpleBeamGraphicObject : public TpsGraphicObjectBase{
public:
    TpsSimpleBeamGraphicObject();
    virtual ~TpsSimpleBeamGraphicObject();

    typedef enum
    {
        IMRT,
        ARC
    }BEAM_TYPE;

    //////////////////////////////////////////////////////////////////////////
    //\brief virtual methods derived from GraphicObjectBase;
    virtual bool Initialize();
    virtual bool Finalize();

    //////////////////////////////////////////////////////////////////////////
    void SetBeamType(BEAM_TYPE type);
    BEAM_TYPE GetBeamType() const;

    //void SetGantryAngle(const float &);
    void SetGantryStartAngle(const float &);
    void SetGantryArcLength(const float &);
    void SetCollimatorAngle(const float &);
    void SetCouchAngle(const float &);

    //float GetGantryAngle() const;
    float GetGantryStartAngle() const;
    float GetGantryArcLength() const;
    float GetCollimatorAngle() const;
    float GetCouchAngle() const;

    void SetBeamToPatMatrix(const Viewer3D::Matrix4x4 &mat);
    void GetBeamToPatMatrix(Viewer3D::Matrix4x4 *) const;

    void SetGantryIsClockWise(bool isClockWise);
    bool GetGantryIsClockWise() const;
    std::vector<std::shared_ptr<TpsSegmentGraphicObject>> GetSegmentGoList() const;

    void CreateSimpleBeam();
    void AddSegmentGO(std::shared_ptr<TpsSegmentGraphicObject>);
    void SetActiveSegment(int index);
    int GetActiveSegmentIndex() const;

private:
    BEAM_TYPE                                          mType;
    float                                               mGantryStartAngle;
    float                                               mGantryArcLength;
    float                                               mCollimatorAngle;
    float                                               mCouchAngle;
    bool                                                mIsClockWise;
    RTFWK::RTFWKDLDOMLCSetting                       *mMlcSetting;

    int                                                 mActiveSegmentGoIndex;
    std::vector<std::shared_ptr<TpsSegmentGraphicObject>> mSegmentList;
    Viewer3D::Matrix4x4                               mBeam2PatMatrix;
    

    TPS_DISALLOW_COPY_AND_ASSIGN(TpsSimpleBeamGraphicObject);
};
TPS_END_NAMESPACE

#endif
