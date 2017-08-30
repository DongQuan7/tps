//////////////////////////////////////////////////////////////////////////
/// Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014 
/// All rights reserved. 
/// 
/// \author  Xi Chen xi.chen@united-imaging.com
//
/// \file    tps_rl_axialpageturn_operation.h
///
/// \brief   AxialPageTurnOperation
/// 
/// \version 1.0
/// 
/// \date    2015/05/04
/////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_AXIALPAGETURN_OPERATION_H_
#define TPS_RL_AXIALPAGETURN_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

namespace Mcsf{
    namespace MedViewer3D{
        class ClsGraphicObjectMPR;
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TpsTextOverlayGraphicObject;
    /// \class TpsAddVOIOperation
    /// 
    /// \brief add a VOI graphic object into ModelWareHouse
class TPS_RL_DECLSPEC AxialPageTurnOperation
    : public OperationBase{
public:
    //the constructor and destructor
    //param[in] the UID of image which the VOI belongs to
    //param[in] the UID of the VOI to be added in database
    AxialPageTurnOperation(const std::string& seriesUID);
    virtual ~AxialPageTurnOperation();

    virtual void SetTargetSliceNum(int sliceNum);

    virtual bool ModifyGraphicObject() const;

private:
    bool ConvertSlicePosAndThicknessToImageText_i(std::string sliceposValKey, std::string slicetotalValKey, std::string thicknessValKey, 
        double slicepos, size_t slicetotal, double thickness,
        std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo) const;
    void UpdatePosInPatientImageText_i(const std::string& sliceposValKey, double posInPat, 
        std::shared_ptr<TpsTextOverlayGraphicObject> mprCornerInfo) const;

    mutable std::string mSeriesUID;
    mutable int mTargetSliceNum;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(AxialPageTurnOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif // TPS_RL_AXIALPAGETURN_OPERATION_H_