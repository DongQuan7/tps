//////////////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare, 2013
///  All rights reserved.
/// 
///  \author  Shangguan Xiaoqing  xiaoqing.shangguan@united-imaging.com
/// 
///  \file image_converter.cpp
/// 
///  \version 1.0
/// 
///  \date    2015/08/19
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "image3d_converter.h"

#include "segment_converter.h"
#include "RtTpsFramework/tps_fw_image3dentity.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_beamsegment.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_series.h"

#include "alg/RtAlgDataLayer/rt_alg_dl_object_image3d.h"

TPS_BEGIN_NAMESPACE;

bool Image3dConverter::ConvertToALG(const TpsImage3DEntity& image3d, std::shared_ptr<RTFWK::RTFWKDLDOImage3D>* algImage3d)
{
    algImage3d->reset(new RTFWK::RTFWKDLDOImage3D());
    (*algImage3d)->SetSeriesUID(image3d.GetUID());
    RtImage3DHeader* header = image3d.GetRtSeries()->get_header();
    RTFWK::RTFWKDLDOImage3DHeader algImageHeader;
    algImageHeader.m_iXDim = header->m_iXDim;
    algImageHeader.m_iYDim = header->m_iYDim;
    algImageHeader.m_dXSize = header->m_dXSize;
    algImageHeader.m_dYSize = header->m_dYSize;
    algImageHeader.m_iSliceCount = header->m_iSliceCount;
    algImageHeader.m_fSliceSpacing = header->m_fSliceSpacing;
    memcpy(algImageHeader.m_cPatientPosition, header->m_cPatientPosition, sizeof(char) * 6);
    algImageHeader.m_iBitsAllocated = header->m_iBitsAllocated;
    algImageHeader.m_dScaleIntercept = header->m_dScaleIntercept;
    algImageHeader.m_dScaleSlope = header->m_dScaleSlope;

    Mcsf::Vector3f imageXOrientation(header->m_ImageXOrientation.x, header->m_ImageXOrientation.y, header->m_ImageXOrientation.z);
    algImageHeader.m_ImageXOrientation = imageXOrientation;
    Mcsf::Vector3f imageYOrientation(header->m_ImageYOrientation.x, header->m_ImageYOrientation.y, header->m_ImageYOrientation.z);
    algImageHeader.m_ImageYOrientation = imageYOrientation;

    std::vector<RTFWK::RTFWKDLDOPerScanInfo> algPerScanInfoList;
    std::vector<RtImage3DPerScanInfo> tpsScanInfoList = header->m_vScanInfoList;
    std::vector<RtImage3DPerScanInfo>::iterator itScanInfo = tpsScanInfoList.begin();

    for (; itScanInfo != tpsScanInfoList.end(); ++itScanInfo)
    {
        RtImage3DPerScanInfo tpSscanInfo = *itScanInfo;
        RTFWK::RTFWKDLDOPerScanInfo algScanInfo;
        Mcsf::Point3f algImagePosistion(tpSscanInfo.m_ImagePosition.x, tpSscanInfo.m_ImagePosition.y, tpSscanInfo.m_ImagePosition.z);
        algScanInfo.m_ImagePosition = algImagePosistion;
        memcpy(algScanInfo.m_cSOPInstanceUID, tpSscanInfo.m_cSOPInstanceUID, 64);
        algScanInfo.m_sUID = tpSscanInfo.m_sUID;
        algScanInfo.m_uiFileOffset = tpSscanInfo.m_uiFileOffset;

        algPerScanInfoList.push_back(algScanInfo);
    }
    algImageHeader.m_vScanInfoList = algPerScanInfoList;
    (*algImage3d)->SetImage3DHeader(algImageHeader);

    // OriginalPoint
    const Mcsf::Point3f OriginalPoint = *image3d.GetSetupPoint();
    (*algImage3d)->SetOriginalPoint(OriginalPoint);


    // pImageData
    long lTotalSize = header->m_iXDim * header->m_iYDim * header->m_iSliceCount * (header->m_iBitsAllocated >> 3);
    char* pImageData = new char[lTotalSize];
    unsigned long ulSize(0);
    memcpy(pImageData, image3d.GetRtSeries()->get_imagedata(&ulSize), lTotalSize);
    (*algImage3d)->SetImageData(pImageData);

    DEL_ARRAY(pImageData);

    return true;
}



TPS_END_NAMESPACE