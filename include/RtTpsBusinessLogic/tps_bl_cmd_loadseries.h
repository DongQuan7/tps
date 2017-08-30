//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  xi.chen (mailto:xi.chen@united-imaging.com)
///
///  \file   tps_bl_cmd_loadseries.h 
///  \brief   load series
///
///  \version 1.0
///  \date  2016/10/28  
///  \
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_LOADSERIES_H_
#define TPS_BL_CMD_LOADSERIES_H_

#include "tps_bl_defs.h"
#include "RtTpsFramework/tps_fw_command.h"

namespace Mcsf {
    class Matrix4f;
}

TPS_BEGIN_NAMESPACE;

class TpsImage3DEntity;
class ROIEntity;
class RtPoi;

class TPS_BL_DECLSPEC LoadSeriesCommand : public TpsCommand
{
public:
    LoadSeriesCommand(const std::string sSeriesUID);
    virtual ~LoadSeriesCommand();

    virtual void GetResult(TpsImage3DEntity*& image3DEntity,
        std::vector<ROIEntity*>& vVoiEntityList, std::vector<RtPoi*>& vRtPoiList);

protected:
    virtual int Execute();

private:
    int InitializeSeries();
    int InitializeVoi();
    int InitializePoi();

private:
    std::string msSeriesUID;
    TpsImage3DEntity* mpImage3DEntity;
    std::vector<ROIEntity*> mvVoiEntityList;
    std::vector<RtPoi*> mvRtPoiList;
};

TPS_END_NAMESPACE

#endif