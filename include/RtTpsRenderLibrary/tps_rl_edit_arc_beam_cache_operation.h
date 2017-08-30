////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_edit_arc_beam_cache_operation.h
/// 
///  \brief class EditArcBeamCache declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/12/11
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_EDIT_ARC_BEAM_CACHE_OPERATION_H_
#define TPS_RL_EDIT_ARC_BEAM_CACHE_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

struct ArcBeamEditCache;
/// \class EditArcBeamCacheOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC EditArcBeamCacheOperation : public OperationBase
{
public:
    //the constructor and destructor
    EditArcBeamCacheOperation(const std::string& imageUid, 
        const std::string& planUid, const std::string ngUid, 
        const std::string& beamUid, int sectionType,
        double preX, double preY, double curX, double curY, DISPLAY_SIZE displaySize);
    virtual ~EditArcBeamCacheOperation();

    virtual bool ModifyGraphicObject() const;

private:
    void CalcValidRotationRange_i(double start, double arcLength, ArcBeamEditCache& cache) const;
    bool BelongsToCircleSection_i(double start, double end, double value) const;
    bool IsFullReachablePosition_i(double start, double end, double value) const;

    std::string         mImageUid;
    std::string         mPlanUid;
    std::string         mNormGroupUid;
    std::string         mBeamUid;
    int                 mSectionType;
    double              mPreX;
    double              mPreY;
    double              mCurX;
    double              mCurY;
    DISPLAY_SIZE        mDisplaySize;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(EditArcBeamCacheOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif