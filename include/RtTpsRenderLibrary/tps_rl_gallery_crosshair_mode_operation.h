//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  shaosen.huang shaosen.huang@united-imaging.com
///
///  \file    tps_rl_gallery_crosshair_mode_operation.h
///  \brief   
///
///  \version 1.0
///  \date    Dec. 12, 2014
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef TPS_RL_GALLERY_CROSSHAIR_MODE_OPERATION_H_
#define TPS_RL_GALLERY_CROSSHAIR_MODE_OPERATION_H_

#include "tps_rl_defs.h"

#include "RtTpsFramework/tps_fw_operationbase.h"


TPS_BEGIN_NAMESPACE  // begin namespace tps

class TPS_RL_DECLSPEC TpsGalleryCrosshairModeOperation : public OperationBase{

public:
    //the constructor and destructor
    TpsGalleryCrosshairModeOperation(std::string& strImageUID, int nCrosshairMode, bool bVisible);
    virtual ~TpsGalleryCrosshairModeOperation();

    virtual bool ModifyGraphicObject() const;

private:
    std::string     m_strImageUID;
    int             m_nCrosshairMode;
    bool            m_bVisible;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsGalleryCrosshairModeOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif //TPS_RL_CROSSHAIR_MOVEMENT_OPERATION_H_
