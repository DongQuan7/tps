////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_smooth_voi.h
/// 
///  \brief class SmoothVoiCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/10
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_SMOOTH_VOI_H_
#define TPS_BL_CMD_SMOOTH_VOI_H_

#include "tps_bl_defs.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_voi_redo_undo_subvolume.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class SmoothVoiCmd
/// 
/// \brief *****
class TPS_BL_DECLSPEC SmoothVoiCmd : public TpsVOIRedoUndoSubVolumeCmd{
public:
    //the constructor and destructor
    SmoothVoiCmd(const std::string& voiUid);
    virtual ~SmoothVoiCmd();

protected:
    virtual int Execute();
    virtual TpsRedoUndoCommand* Clone();

private:
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(SmoothVoiCmd);
};

TPS_END_NAMESPACE  // end namespace tps

#endif