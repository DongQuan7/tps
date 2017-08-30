////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  WANG Jie  jie.wang@united-imaging.com
/// 
///  \file tps_bl_cmd_interpolate_voi.h
/// 
///  \brief class InterpolateVoiCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2017/06/14
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_INTERPOLATE_VOI_H_
#define TPS_BL_CMD_INTERPOLATE_VOI_H_

#include "tps_bl_defs.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_voi_redo_undo_subvolume.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps


/// \class InterpolateVoiCmd
/// 
/// \brief *****
class TPS_BL_DECLSPEC InterpolateVoiCmd : public TpsVOIRedoUndoSubVolumeCmd{
public:
    //the constructor and destructor
    InterpolateVoiCmd(const std::string& voiUid);
    virtual ~InterpolateVoiCmd();

protected:
    std::string                         mVOIUID;

    //virtual void AutoInterpolate_i() const;
    virtual int Execute();
    virtual TpsRedoUndoCommand* Clone();

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(InterpolateVoiCmd);
};

TPS_END_NAMESPACE  // end namespace tps

#endif