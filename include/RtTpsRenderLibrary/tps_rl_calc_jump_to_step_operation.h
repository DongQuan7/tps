////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_rl_calc_jump_to_step_operation.h
/// 
///  \brief class CalcJumpToStep declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/08/03
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_CALC_JUMP_TO_STEP_OPERATION_H_
#define TPS_RL_CALC_JUMP_TO_STEP_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

namespace Mcsf
{
    namespace MedViewer3D
    {
        class GraphicObjVolume;
    }
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

class MPRGraphicObject;
/// \class CalcJumpToStepOperation
/// 
/// \brief *****
class TPS_RL_DECLSPEC CalcJumpToStepOperation
 : public OperationBase
{
public:
    //the constructor and destructor
    CalcJumpToStepOperation(const std::string& seriesUid, float x, float y, float z, 
        FIRST_POSTFIX_COMPONENT section, SECOND_POSTFIX_COMPONENT extension);
    virtual ~CalcJumpToStepOperation();

    virtual bool ModifyGraphicObject() const;

    int GetStep() const;
private:

    std::string mSeriesUid;
    float mX;
    float mY;
    float mZ;
    FIRST_POSTFIX_COMPONENT mSection;
    SECOND_POSTFIX_COMPONENT mExtension;

    mutable int mStep;
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(CalcJumpToStepOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif