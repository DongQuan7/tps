#include "StdAfx.h"

#include "RtTpsFramework/tps_fw_table.h"

#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_tablecontour.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_seriestable.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_interface_object_voi.h"

#include "Mcsf3DArithmetic/mcsf_3d_arithmetic.h"

#include "McsfMedViewer3DArithmetic/point3d.h"

#include "tps_logger.h"
#include "tps_performance.h"


using namespace Mcsf::MedViewer3D;
TPS_BEGIN_NAMESPACE  // begin namespace tps

RtTable::RtTable() : mTableMask(nullptr)
{

}

RtTable::~RtTable()
{
    for (int i = 0; i < mTableContours.size(); ++i)
    {
        TPS_DEL_PTR(mTableContours[i]);
    }
    mTableContours.clear();

    TPS_DEL_ARRAY(mTableMask);
}

void RtTable::SetUid(const std::string& uid)
{
    mUid = uid;
}

std::string RtTable::GetUid() const
{
    return mUid;
}

bool RtTable::AddTableContour(RtTablecontour* contour)
{
    if (contour == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The contour to be added is empty.";
        return false;
    }

    for (int i = 0; i < mTableContours.size(); ++i)
    {
        if (contour == mTableContours[i])
        {
            return true;
        }
        else if (contour->get_uid() == mTableContours[i]->get_uid())
        {
            TPS_LOG_DEV_ERROR<<"The contour to be added already exists.";
            return false;
        }
    }
    mTableContours.push_back(contour);
    return true;
}

std::vector<RtTablecontour*> RtTable::GetTableContours() const
{
    return mTableContours;
}

void RtTable::SetName(const std::string& name)
{
    mName = name;
}

std::string RtTable::GetName() const
{
    return mName;
}

void RtTable::ClearTableMask()
{
    if (mTableMask) TPS_DEL_ARRAY(mTableMask);
    memset(mMaskDim, 0, 3 * sizeof(unsigned int));
    mTableToVolume.SetIdentity();
}

void RtTable::SetMaskDim(const unsigned int* dim)
{
    unsigned int count = dim[0] * dim[1] * dim[2];
    if (mTableMask == nullptr || count != mMaskDim[0] * mMaskDim[1] * mMaskDim[2])
    {
        mTableMask = new unsigned char[count];
    }
    memset(mTableMask, 0, count * sizeof(unsigned char));
    memcpy(mMaskDim, dim, 3 * sizeof(unsigned int));
}

void RtTable::GetMaskDim(unsigned int* dim) const
{
    memcpy(dim, mMaskDim, 3 * sizeof(unsigned int));
}

unsigned char* RtTable::GetMask() const
{
    return mTableMask;
}

Mcsf::Matrix4f RtTable::GetTableToMaskVolume() const
{
    return mTableToVolume;
}

void RtTable::SetTableToMaskVolume(const Mcsf::Matrix4f& matrix)
{
    mTableToVolume = matrix;
}

Mcsf::Point3f RtTable::GetCenter() const
{
    Mcsf::Point3f center(0.5f * (mMaskDim[0] - 1), 
        0.5f * (mMaskDim[0] - 1), 0.5f * (mMaskDim[0] - 1));
    return mTableToVolume.Inverse().Transform(center);
}

SeriesTable::SeriesTable()
{
    mSeriesTable = new RtSeriestable();
    this->ClearTransformation();
    //默认颜色透明度
    mColor[0] = 1.0;
    mColor[1] = 1.0;
    mColor[2] = 1.0;
    mColor[3] = 0.5;
}

SeriesTable::~SeriesTable()
{
    TPS_DEL_PTR(mSeriesTable);
}

void SeriesTable::CopyFrom(RtSeriestable* seriesTable)
{
    *mSeriesTable = *seriesTable;
}

void SeriesTable::SetSeriesUid(const std::string& seriesUid)
{
    mSeriesTable->set_seriesuid(seriesUid);
}

std::string SeriesTable::GetSeriesUid() const
{
    return mSeriesTable->get_seriesuid();
}

void SeriesTable::SetTableUid(const std::string& tableUid)
{
    mSeriesTable->set_tableuid(tableUid);
}

std::string SeriesTable::GetTableUid() const
{
    return mSeriesTable->get_tableuid();
}

double SeriesTable::GetDensity() const
{
    return mSeriesTable->get_averagedensity();
}

void SeriesTable::SetDensity(double density)
{
    mSeriesTable->set_averagedensity(density);
}

// 待修改
double SeriesTable::GetReferenceEDensity() const
{
    return mSeriesTable->get_overrideedensity();
}

void SeriesTable::SetReferenceEDensity(double density)
{
    mSeriesTable->set_overrideedensity(density);
}


void SeriesTable::SetInitLocation(const Mcsf::Matrix4f& mat)
{
    mInitLocation = mat;
}

void SeriesTable::SetOffset(const double* offset)
{
    mSeriesTable->set_offsetx(offset[0]);
    mSeriesTable->set_offsety(offset[1]);
    mSeriesTable->set_offsetz(offset[2]);
}

void SeriesTable::GetOffset(double* offset) const
{
    offset[0] = mSeriesTable->get_offsetx();
    offset[1] = mSeriesTable->get_offsety();
    offset[2] = mSeriesTable->get_offsetz();
}

void SeriesTable::SetRotationAngles(const double* angles)
{
    mSeriesTable->set_anglex(angles[0]);
    mSeriesTable->set_angley(angles[1]);
    mSeriesTable->set_anglez(angles[2]);
}

void SeriesTable::GetRotationAngles(double* angles)
{
    angles[0] = mSeriesTable->get_anglex();
    angles[1] = mSeriesTable->get_angley();
    angles[2] = mSeriesTable->get_anglez();
}

Mcsf::Matrix4f SeriesTable::GetTableToPatient(const Mcsf::Point3f& tblCenter) const
{
    //matrix org to center
    Mcsf::Matrix4f matToCenter = Mcsf::MakeTranslate(Mcsf::Vector3f(
        -tblCenter.GetX(), -tblCenter.GetY(), -tblCenter.GetZ()));
    //matrix translation
    Mcsf::Vector3f translation(mSeriesTable->get_offsetx(), 
        mSeriesTable->get_offsety(), mSeriesTable->get_offsetz());
    Mcsf::Matrix4f matTranslation = Mcsf::MakeTranslate(translation);
    //matrix rotation
    Mcsf::Matrix4f matRotation;
    matRotation = matRotation.RotationX(mSeriesTable->get_anglex() * PI / 180);
    matRotation.Append(matRotation.RotationY(mSeriesTable->get_angley() * PI / 180));
    matRotation.Append(matRotation.RotationZ(mSeriesTable->get_anglez() * PI / 180));

    Mcsf::Matrix4f tm = matToCenter;
    tm.Prepend(matRotation);
    tm.Prepend(matToCenter.Inverse());
    tm.Prepend(matTranslation);
    tm.Prepend(mInitLocation);
    return tm;
}

void SeriesTable::ClearTransformation()
{
    if (mSeriesTable)
    {
        mSeriesTable->set_offsetx(0.0);
        mSeriesTable->set_offsety(0.0);
        mSeriesTable->set_offsetz(0.0);
        mSeriesTable->set_anglex(0.0);
        mSeriesTable->set_angley(0.0);
        mSeriesTable->set_anglez(0.0);
        //默认0.2
        mSeriesTable->set_averagedensity(0.2f);
        mSeriesTable->set_overrideedensity(0.2f);
    }

    mInitLocation.SetIdentity();
}

void SeriesTable::SetColor(const double* color)
{
    memcpy(mColor, color, 4 * sizeof(double));
}

void SeriesTable::GetColor(double* color) const
{
    memcpy(color, mColor, 4 * sizeof(double));
}

RtSeriestable* SeriesTable::GetInnerTable() const
{
    return mSeriesTable;
}
TPS_END_NAMESPACE  // end namespace tps