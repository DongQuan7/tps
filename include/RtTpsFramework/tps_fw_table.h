////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2013
///  All rights reserved.
/// 
///  \author  zuo kai  kai.zuo@united-imaging.com
/// 
///  \file tps_da_tpstable.h
/// 
///  \brief class table declaration 
/// 
///  \version 1.0
/// 
///  \date    2013/12/07
////////////////////////////////////////////////////////////////

#ifndef TPS_DA_TPSTable_H_
#define TPS_DA_TPSTable_H_

#include "tps_fw_defs.h"
#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include <vector>

TPS_BEGIN_NAMESPACE  // begin namespace tps

class RtTablecontour;
class RtSeriestable;
class TPS_FW_DECLSPEC RtTable
{
public:
    RtTable();
    ~RtTable();

    void SetUid(const std::string& uid);
    std::string GetUid() const;

    bool AddTableContour(RtTablecontour* contour);
    std::vector<RtTablecontour*> GetTableContours() const;

    void SetName(const std::string& name);
    std::string GetName() const;

    void SetMaskDim(const unsigned int* dim);
    void GetMaskDim(unsigned int* dim) const;
    unsigned char* GetMask() const;

    void SetTableToMaskVolume(const Mcsf::Matrix4f& matrix);
    Mcsf::Matrix4f GetTableToMaskVolume() const;
    void ClearTableMask();

    Mcsf::Point3f GetCenter() const;
private:
    std::string                     mUid;
    std::string                     mName;
    std::vector<RtTablecontour*>    mTableContours;
    //病床的Mask数据
    unsigned char*                  mTableMask;
    unsigned int                    mMaskDim[3];
    //病床数据点所在坐标系到Mask Volume坐标系的变换关系
    Mcsf::Matrix4f                  mTableToVolume;
    //病床Volume中心点坐标
    Mcsf::Point3f                   mCenter;
};

class TPS_FW_DECLSPEC SeriesTable
{
public:
    SeriesTable();
    ~SeriesTable();

    void CopyFrom(RtSeriestable* seriesTable);

    void SetSeriesUid(const std::string& seriesUid);
    std::string GetSeriesUid() const;
    void SetTableUid(const std::string& tableUid);
    std::string GetTableUid() const;
    double GetDensity() const;
    void SetDensity(double density);

    double GetReferenceEDensity() const;
    void SetReferenceEDensity(double eDensity);

    void SetInitLocation(const Mcsf::Matrix4f& mat);
    void SetOffset(const double* offset);
    void GetOffset(double* offset) const;
    void SetRotationAngles(const double* angles);
    void GetRotationAngles(double* angles);
    Mcsf::Matrix4f GetTableToPatient(const Mcsf::Point3f& tblCenter) const;

    void ClearTransformation();

    void SetColor(const double* color);
    void GetColor(double* color) const;

    RtSeriestable* GetInnerTable() const;
private:
    RtSeriestable* mSeriesTable;
    
    //初始摆位变换
    Mcsf::Matrix4f mInitLocation;
    double mColor[4];
};
TPS_END_NAMESPACE  // end namespace tps

#endif