////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
/// 
///  \author  LI Song  song.li@united-imaging.com
/// 
///  \file tps_rl_preview_voi_operation.h
/// 
///  \brief class TpsPreviewVOIOperation declaration 
/// 
///  \version 1.0
/// 
///  \date    2014/06/27
////////////////////////////////////////////////////////////////

#ifndef TPS_RL_PREVIEW_VOI_OPERATION_H_
#define TPS_RL_PREVIEW_VOI_OPERATION_H_

#include "tps_rl_defs.h"
#include "RtTpsFramework/tps_fw_operationbase.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class ROIEntity;
/// \class TpsAddVOIOperation
/// 
/// \brief add a VOI graphic object into ModelWareHouse
class TPS_RL_DECLSPEC TpsPreviewVOIOperation
    : public OperationBase{
public:
    //the constructor and destructor
    //param[in] the UID of image which the VOI belongs to
    //param[in] the UID of the VOI to be added in database
    TpsPreviewVOIOperation(const std::string& imageUID, ROIEntity* pVOI);
    ~TpsPreviewVOIOperation();

    virtual bool ModifyGraphicObject() const;

private:
    mutable std::string m_strImageUID;
    mutable ROIEntity*  m_pVOI;

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsPreviewVOIOperation);
};

TPS_END_NAMESPACE  // end namespace tps

#endif