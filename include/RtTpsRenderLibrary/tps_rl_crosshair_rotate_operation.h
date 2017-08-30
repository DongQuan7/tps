//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
///
///  \author  LI Song song.li@united-imaging.com
///
///  \file    tps_rl_crosshair_rotate_operation.h
///  \brief   Crosshair rotate declaration
///
///  \version 1.0
///  \date    Jul. 21, 2014
///  
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_CROSSHAIR_ROTATE_OPERATION_H_
#define TPS_RL_CROSSHAIR_ROTATE_OPERATION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_operationbase.h"


TPS_BEGIN_NAMESPACE  // begin namespace tps

class TPS_RL_DECLSPEC CrosshairRotateOperation : public OperationBase{

public:
    //the constructor and destructor
    CrosshairRotateOperation(std::string& strImageUID, 
        FIRST_POSTFIX_COMPONENT strSectionType, double xStart, 
        double yStart, double xEnd, double yEnd, double rotateX, double rotateY,int hitMode,int crossMode);
    virtual ~CrosshairRotateOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string     m_strImageUID;
    FIRST_POSTFIX_COMPONENT m_strSectionType;
    double          m_dXStart;
    double          m_dYStart;
    double          m_dXEnd;
    double          m_dYEnd;
    double          m_dXRotateCenter;
    double          m_dYRotateCenter;
    int             m_hitMode;
    int             m_crossMode;
private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(CrosshairRotateOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_RL_CROSSHAIR_ROTATE_OPERATION_H_
