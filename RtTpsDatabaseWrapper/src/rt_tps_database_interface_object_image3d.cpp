//////////////////////////////////////////////////////////////////////////
/// \defgroup Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2015
///  All rights reserved.
///
///  \author  ZHOU qiangqiang  mailto:qiangqiang.zhou@united-imaging.com
///
///  \file      rt_tps_database_interface_object_image3d.cpp
///  \brief     NO table in database
///
///  \version 1.0
///  \date    July 10,2015
///  \{
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_image3d.h"
#include <fstream>
#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"

RT_TPS_DATABASE_BEGIN_NAMESPACE

RtImage3DPerScanInfo::RtImage3DPerScanInfo()
    :m_ImagePosition(0.0, 0.0, 0.0), 
    m_uiFileOffset(0),
    m_sUID(""),
    m_SliceLocation(0.0f),
    m_BitsStored(0)
{
    memset(m_cSOPInstanceUID, 0, 64);
}

RtImage3DPerScanInfo::~RtImage3DPerScanInfo()
{

}

RtImage3DPerScanInfo::RtImage3DPerScanInfo(const RtImage3DPerScanInfo& oPerScanInfo)
    :m_ImagePosition(oPerScanInfo.m_ImagePosition),
    m_uiFileOffset(oPerScanInfo.m_uiFileOffset),
    m_sUID(oPerScanInfo.m_sUID),
    m_uiInstanceNumber(oPerScanInfo.m_uiInstanceNumber),
    m_SliceLocation(oPerScanInfo.m_SliceLocation),
    m_BitsStored(oPerScanInfo.m_BitsStored)
{
    memcpy(m_cSOPInstanceUID, oPerScanInfo.m_cSOPInstanceUID, 64);
}

RtImage3DPerScanInfo& RtImage3DPerScanInfo::operator = (const RtImage3DPerScanInfo& perScanInfo)
{
    if (this != &perScanInfo){
        this->m_ImagePosition = perScanInfo.m_ImagePosition;
        this->m_uiFileOffset = perScanInfo.m_uiFileOffset;
        memcpy(this->m_cSOPInstanceUID, perScanInfo.m_cSOPInstanceUID, sizeof(char) * 64); 
        this->m_sUID = perScanInfo.m_sUID;
        this->m_uiInstanceNumber = perScanInfo.m_uiInstanceNumber;
        this->m_SliceLocation = perScanInfo.m_SliceLocation;
        this->m_BitsStored = perScanInfo.m_BitsStored;
    }
    return *this;
}

//////////////////////////////////////////////////////////////////////////
RtImage3DHeader::RtImage3DHeader()
    :m_iSliceCount(0),
    m_iXDim(0),
    m_iYDim(0),
    m_dXSize(0.0),
    m_dYSize(0.0),
    m_bIsEqualSliceSpacing(false),
    m_fSliceSpacing(0),
    m_iDataType(0),
    m_iBitsAllocated(0),
    m_iWindowCenter(0),
    m_iWindowWidth(0),
    m_dScaleSlope(0.0),
    m_dScaleIntercept(0.0),
    m_sManufacturerModelName(""),
    m_sFrameOfReferenceUID(""),
    m_iInterCount(0),
    m_pInterpolation(nullptr)
{
    memset(m_cModality, 0, 6);
    memset(m_cPatientPosition, 0, 6);
}

RtImage3DHeader::~RtImage3DHeader()
{
    DEL_ARRAY(m_pInterpolation);
}

RtImage3DHeader::RtImage3DHeader(const RtImage3DHeader& oImage3DHeader)
    :m_iSliceCount(oImage3DHeader.m_iSliceCount),
    m_iXDim(oImage3DHeader.m_iXDim),
    m_iYDim(oImage3DHeader.m_iYDim),
    m_dXSize(oImage3DHeader.m_dXSize),
    m_dYSize(oImage3DHeader.m_dYSize),
    m_bIsEqualSliceSpacing(oImage3DHeader.m_bIsEqualSliceSpacing),
    m_fSliceSpacing(oImage3DHeader.m_fSliceSpacing),
    m_iDataType(oImage3DHeader.m_iDataType),
    m_iBitsAllocated(oImage3DHeader.m_iBitsAllocated), 
    m_ImageXOrientation(oImage3DHeader.m_ImageXOrientation),
    m_ImageYOrientation(oImage3DHeader.m_ImageYOrientation),
    m_iWindowCenter(oImage3DHeader.m_iWindowCenter),
    m_iWindowWidth(oImage3DHeader.m_iWindowWidth),
    m_dScaleSlope(oImage3DHeader.m_dScaleSlope),
    m_dScaleIntercept(oImage3DHeader.m_dScaleIntercept),
    m_sFrameOfReferenceUID(oImage3DHeader.m_sFrameOfReferenceUID),
    m_sManufacturerModelName(oImage3DHeader.m_sManufacturerModelName),
    m_tAcquisitionDateTime(oImage3DHeader.m_tAcquisitionDateTime),
    m_pInterpolation(nullptr),
    m_iInterCount(0)
{
    memcpy(m_cModality,oImage3DHeader.m_cModality,sizeof(char)*6);
    memcpy(m_cPatientPosition,oImage3DHeader.m_cPatientPosition,sizeof(char)*6);  
    this->m_vScanInfoList =  oImage3DHeader.m_vScanInfoList;
    bool* pInter = nullptr;
    int iCount = 0;
    oImage3DHeader.GetInterpolation(pInter, &iCount);
    this->SetInterpolation(pInter, iCount);
}

RtImage3DHeader RtImage3DHeader::operator= (const RtImage3DHeader& oImage3DHeader)
{
    if (this != &oImage3DHeader){
        this->m_iSliceCount = oImage3DHeader.m_iSliceCount;
        this->m_iXDim = oImage3DHeader.m_iXDim;
        this->m_iYDim = oImage3DHeader.m_iYDim;
        this->m_dXSize = oImage3DHeader.m_dXSize;
        this->m_dYSize = oImage3DHeader.m_dYSize;
        this->m_bIsEqualSliceSpacing = oImage3DHeader.m_bIsEqualSliceSpacing;
        this->m_fSliceSpacing = oImage3DHeader.m_fSliceSpacing;
        this->m_iDataType = oImage3DHeader.m_iDataType;
        this->m_iBitsAllocated = oImage3DHeader.m_iBitsAllocated;
        this->m_ImageXOrientation = oImage3DHeader.m_ImageXOrientation;
        this->m_ImageYOrientation = oImage3DHeader.m_ImageYOrientation;
        this->m_iWindowCenter = oImage3DHeader.m_iWindowCenter;
        this->m_iWindowWidth = oImage3DHeader.m_iWindowWidth;
        this->m_dScaleSlope = oImage3DHeader.m_dScaleSlope;
        this->m_dScaleIntercept = oImage3DHeader.m_dScaleIntercept;
        this->m_sFrameOfReferenceUID = oImage3DHeader.m_sFrameOfReferenceUID;
        this->m_sManufacturerModelName = oImage3DHeader.m_sManufacturerModelName;
        this->m_tAcquisitionDateTime = oImage3DHeader.m_tAcquisitionDateTime;

        memcpy(this->m_cModality,oImage3DHeader.m_cModality,sizeof(char)*6);
        memcpy(this->m_cPatientPosition,oImage3DHeader.m_cPatientPosition,sizeof(char)*6);
        this->m_vScanInfoList =  oImage3DHeader.m_vScanInfoList;
        bool* pInter = nullptr;
        int iCount = 0;
        oImage3DHeader.GetInterpolation(pInter, &iCount);
        this->SetInterpolation(pInter, iCount);
    }
    return *this;
}

void RtImage3DHeader::GetInterpolation(bool*& pInter, int* iInterCount) const
{
    pInter = m_pInterpolation;
    *iInterCount = m_iInterCount;
}

void RtImage3DHeader::SetInterpolation(const bool* pInter, const int iInterCount)
{
    DEL_ARRAY(m_pInterpolation);
    m_iInterCount = iInterCount;
    m_pInterpolation = new bool[iInterCount];
    memcpy(m_pInterpolation, pInter, sizeof(bool) * iInterCount);
}


RT_TPS_DATABASE_END_NAMESPACE