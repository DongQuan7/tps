//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  LI Song song.li@united-imaging.com
///
///  \file    tps_rl_crosshair_hittest_operation.h
///  \brief   Crosshair Hit Test declaration
///
///  \version 1.0
///  \date    Jul. 10, 2014
///  
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_CROSSHAIR_HITTEST_OPERATION_H_
#define TPS_RL_CROSSHAIR_HITTEST_OPERATION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_operationbase.h"


TPS_BEGIN_NAMESPACE  // begin namespace tps

class TPS_RL_DECLSPEC CrosshairHitTestOperation : public OperationBase{

public:
    //the constructor and destructor
    CrosshairHitTestOperation(std::string strKey, double x, double y);
    virtual ~CrosshairHitTestOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string     m_strCrosshairGOKey;
    double          m_dX;
    double          m_dY;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(CrosshairHitTestOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_RL_CROSSHAIR_HITTEST_OPERATION_H_
