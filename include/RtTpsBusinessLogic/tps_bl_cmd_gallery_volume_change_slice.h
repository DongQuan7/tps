//////////////////////////////////////////////////////////////////////////
/// \defgroup TPS of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  yuxuan.duan@united-imaging.com
///
///  \file    tps_bl_cmd_gallery_volume_change_slice.h
///  \brief   Volume Change Slice command
///  \class   VolumeChangeSliceCmd
///
///  \version 1.0
///  \date    Mar 13, 2017
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_BL_CMD_GALLERY_VOLUME_CHANGE_SLICE_H_
#define TPS_BL_CMD_GALLERY_VOLUME_CHANGE_SLICE_H_

#include "RtTpsFramework/tps_fw_command.h"

#include <vector>

TPS_BEGIN_NAMESPACE

class SimpleGuiResponser;

class GalleryVolumeChangeSliceCmd : public TpsCommand
{
public:
    GalleryVolumeChangeSliceCmd(const std::string& strSeriesUid, int SectionType, int nPageChange, std::shared_ptr<SimpleGuiResponser> spResponser);

    virtual ~GalleryVolumeChangeSliceCmd(void);

protected:
    virtual int PreExecute();
    virtual int Execute();
    virtual int PostExecute();

    GalleryVolumeChangeSliceCmd * Clone();

private:
    std::string     m_strSeriesUid;
    int             m_nSectionType;             // CORONAL = 0x01, SAGITTAL = 0x02, AXIAL = 0x03
    int             m_nPageChange;              // 1: next slice; -1: previous slice
    std::shared_ptr<SimpleGuiResponser> m_spResponser;
};

TPS_END_NAMESPACE

#endif //TPS_BL_CMD_GALLERY_VOLUME_CHANGE_SLICE_H_
