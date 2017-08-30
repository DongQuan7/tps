//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  LI Song  song.li@united-imaging.com
///
///  \file    tps_bl_cmd_volume_change_slice.h
///  \brief   Volume Change Slice command
///  \class   VolumeChangeSliceCmd
///
///  \version 1.0
///  \date    July 30, 2014
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_VOLUME_CHANGE_SLICE_H_
#define TPS_BL_CMD_VOLUME_CHANGE_SLICE_H_

#include "RtTpsFramework/tps_fw_command.h"

#include <vector>

TPS_BEGIN_NAMESPACE

class SimpleGuiResponser;

class VolumeChangeSliceCmd : public TpsCommand
{
public:
    VolumeChangeSliceCmd(const std::string& strSeriesUid, const std::string& strPlanUid, int SectionType, int nPageChange, std::shared_ptr<SimpleGuiResponser> spResponser,bool bIsCyclic);

    virtual ~VolumeChangeSliceCmd(void);

protected:
    virtual int PreExecute();
    virtual int Execute();
    virtual int PostExecute();

    VolumeChangeSliceCmd * Clone();

private:
    std::string     m_strSeriesUid;
    std::string     m_strPlanUid;
    int             m_nSectionType;             // CORONAL = 0x01, SAGITTAL = 0x02, AXIAL = 0x03
    int             m_nPageChange;              // 1: next slice; -1: previous slice
    std::shared_ptr<SimpleGuiResponser> m_spResponser;
    bool            m_bIsCyclic;
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_VOLUME_CHANGE_SLICE_H_
