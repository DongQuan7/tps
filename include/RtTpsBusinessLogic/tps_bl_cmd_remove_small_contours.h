////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  Wang jie  jie.wang@united-imaging.com
/// 
///  \file tps_bl_cmd_remove_small_contours.h
/// 
///  \brief class RemoveSmallContoursCmd declaration 
/// 
///  \version 1.0
/// 
///  \date    2015/11/10
////////////////////////////////////////////////////////////////

#ifndef TPS_BL_REMOVESMALLCONTOURS_CMD_H_
#define TPS_BL_REMOVESMALLCONTOURS_CMD_H_

#include "tps_bl_defs.h"
#include "RtTpsBusinessLogic/tps_bl_cmd_voi_redo_undo_subvolume.h"

#include "RtTpsCertifiedLib/rt_tps_certifiedselectcontourslices.h"
#include "RtTpsFramework/tps_fw_graphicobjectconverterbase.h"

namespace Mcsf{
    class Point2f;
}

TPS_BEGIN_NAMESPACE

class TPS_BL_DECLSPEC RemoveSmallContoursCmd : public TpsVOIRedoUndoSubVolumeCmd {
public:
    RemoveSmallContoursCmd(const std::string &voiUid, double area);
    virtual ~RemoveSmallContoursCmd();

protected:
    virtual int Execute();
    virtual TpsRedoUndoCommand* Clone();

private:
    double GetArea_i(const std::vector<Mcsf::Point2f> &contour, 
        double xSize, double ySize, int width, int height);
    bool UpdateVOI_i(const std::string &voiUID);
private:
    double                   mArea;
    int                      mRemoveCount;
private:
    TPS_DISALLOW_COPY_AND_ASSIGN(RemoveSmallContoursCmd);
};

TPS_END_NAMESPACE

#endif
