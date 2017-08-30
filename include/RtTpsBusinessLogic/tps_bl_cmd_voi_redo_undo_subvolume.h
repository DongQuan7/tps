////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
/// 
///  \author  GONG Maoliang  mailto:maoliang.gong@united-imaging.com
/// 
///  \file tps_bl_cmd_voi_redo_undo_subvolume.h
/// 
///  \brief class TpsVOIRedoUndoSubVolumeCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/09/17
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_CMD_VOI_REDO_UNDO_SUBVOLUME_H_
#define TPS_BL_CMD_VOI_REDO_UNDO_SUBVOLUME_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"
#include "RtTpsFramework/tps_fw_voiredoundocommandbase.h"

namespace McsfGeometry{
    class SubVolume;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps
struct CertifiedVOI;
/// \class TpsVOIRedoUndoSubVolumeCmd
/// 
/// \brief *****
class TPS_BL_DECLSPEC TpsVOIRedoUndoSubVolumeCmd : public TpsVOIRedoUndoCmdBase{
public:
    //the constructor and destructor
    TpsVOIRedoUndoSubVolumeCmd(const std::string& uid);
    virtual ~TpsVOIRedoUndoSubVolumeCmd();

    virtual int Undo();
    virtual int Redo();
    virtual bool Ready() const;
protected:
    virtual int PreExecute();

    std::unique_ptr<McsfGeometry::SubVolume> mSubvolume;
    CertifiedVOI* mCertVoi;
private:
    int RedoUndo_i();
    // the default copy and assign method are not allowed.
    TPS_DISALLOW_COPY_AND_ASSIGN(TpsVOIRedoUndoSubVolumeCmd);
};

TPS_END_NAMESPACE  // end namespace tps

#endif