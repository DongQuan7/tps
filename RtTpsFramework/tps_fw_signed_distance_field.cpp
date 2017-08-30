#include "StdAfx.h"
#include "RtTpsFramework/tps_fw_signed_distance_field.h"

#include "gl/glew.h"

TPS_BEGIN_NAMESPACE

SignedDistanceField::SignedDistanceField() 
    : VolumeData<float>()
    , mIsoLevel(0.f)
    , mType(GL_FLOAT)
    , mFormat(GL_RED)
    , mInternalFormat(GL_R32F)
{
    mDefault = -1.0f;
}

SignedDistanceField::~SignedDistanceField()
{

}

SignedDistanceField::SignedDistanceField(const SignedDistanceField& sdf)
    : VolumeData<float>(sdf)
    , mIsoLevel(sdf.mIsoLevel)
    , mType(sdf.mType)
    , mFormat(sdf.mFormat)
    , mInternalFormat(sdf.mInternalFormat)
    , mMatPatToSDF(sdf.mMatPatToSDF)
{
    mDefault = sdf.mDefault;
}

SignedDistanceField& SignedDistanceField::operator=(const SignedDistanceField& sdf)
{
    auto base_ptr = (VolumeData<float>*)this;
    *base_ptr = sdf;
    mIsoLevel = sdf.mIsoLevel;
    mType = sdf.mType;
    mFormat = sdf.mFormat;
    mInternalFormat = sdf.mInternalFormat;
    mMatPatToSDF = sdf.mMatPatToSDF;
    return *this;
}

int SignedDistanceField::GetParasInVol(const MAT4& matP2V, int* start, int* size) const
{
    MAT4 sdf2vol = mMatPatToSDF.Inverse();
    sdf2vol.Prepend(matP2V);

    VEC3 vecZ = sdf2vol.Transform(VEC3(0, 0, 1));
    vecZ.Normalize();
    int axis = -1;
    if (fabs(fabs(vecZ.x) - 1) <= 1e-6)
    {
        axis = 0;
        start[0] = mStart[2]; start[1] = mStart[0]; start[2] = mStart[1];
        size[0] = mSize[2]; size[1] = mSize[0]; size[2] = mSize[1]; 
    }
    else if (fabs(fabs(vecZ.y) - 1) <= 1e-6)
    {
        axis = 1;
        start[0] = mStart[1]; start[1] = mStart[2]; start[2] = mStart[0];
        size[0] = mSize[1]; size[1] = mSize[2]; size[2] = mSize[0]; 
    }
    else if (fabs(fabs(vecZ.z) - 1) <= 1e-6)
    {
        axis = 2;
        memcpy(start, mStart, 3 * sizeof(int));
        memcpy(size, mSize, 3 * sizeof(int));
    }
    return axis;
}

float SignedDistanceField::GetValueInVol(const MAT4& matP2V, int x, int y, int z) const
{
    MAT4 sdf2vol = mMatPatToSDF.Inverse();
    sdf2vol.Prepend(matP2V);

    VEC3 vecZ = sdf2vol.Transform(VEC3(0, 0, 1));
    vecZ.Normalize();
    int p[3];
    if (fabs(fabs(vecZ.x) - 1) <= 1e-6)
    {
        p[0] = y; p[1] = z; p[2] = x;
    }
    else if (fabs(fabs(vecZ.y) - 1) <= 1e-6)
    {
        p[0] = z; p[1] = x; p[2] = y;
    }
    else if (fabs(fabs(vecZ.z) - 1) <= 1e-6)
    {
        p[0] = x; p[1] = y; p[2] = z;
    }
    else return mDefault;

    for (int i = 0; i < 3; ++i)
    {
        p[i] -= mStart[i];
    }
    if (p[0] < 0 || p[0] >= mSize[0] || 
        p[1] < 0 || p[1] >= mSize[1] || 
        p[2] < 0 || p[2] >= mSize[2] )
    {
        return mDefault;
    }

    return mData[p[2] * mSize[0] * mSize[1] + p[1] * mSize[0] + p[0]];
}

float SignedDistanceField::operator()(double x, double y, double z) const
{
    if (x < -0.5 || x > mSize[0] - 0.5 || 
        y < -0.5 || y > mSize[1] - 0.5 || 
        z < -0.5 || z > mSize[2] - 0.5 )
    {
        return mDefault;
    }

    int ix0 = floor(x);
    int iy0 = floor(y);
    int iz0 = floor(z);
    int ix1 = ix0 + 1;
    int iy1 = iy0 + 1;
    int iz1 = iz0 + 1;

    if (ix0 < 0) ix0 = 0;
    if (iy0 < 0) iy0 = 0;
    if (iz0 < 0) iz0 = 0;
    if (ix1 >= mSize[0]) ix1 = mSize[0] - 1;
    if (iy1 >= mSize[1]) iy1 = mSize[1] - 1;
    if (iz1 >= mSize[2]) iz1 = mSize[2] - 1;

    float dx = x - ix0;
    float dy = y - iy0;
    float dz = z - iz0;

    float v000 = mData[iz0 * mSize[0] * mSize[1] + iy0 * mSize[0] + ix0];
    float v100 = mData[iz0 * mSize[0] * mSize[1] + iy0 * mSize[0] + ix1];
    float v010 = mData[iz0 * mSize[0] * mSize[1] + iy1 * mSize[0] + ix0];
    float v110 = mData[iz0 * mSize[0] * mSize[1] + iy1 * mSize[0] + ix1];
    float v001 = mData[iz1 * mSize[0] * mSize[1] + iy0 * mSize[0] + ix0];
    float v101 = mData[iz1 * mSize[0] * mSize[1] + iy0 * mSize[0] + ix1];
    float v011 = mData[iz1 * mSize[0] * mSize[1] + iy1 * mSize[0] + ix0];
    float v111 = mData[iz1 * mSize[0] * mSize[1] + iy1 * mSize[0] + ix1];

    return (1 - dx) * (1 - dy) * (1 - dz) * v000 + 
        dx * (1 - dy) * (1 - dz) * v100 + 
        (1 - dx) * dy * (1 - dz) * v010 + 
        dx * dy * (1 - dz) * v110 +
        (1 - dx) * (1 - dy) * dz * v001 + 
        dx * (1 - dy) * dz * v101 + 
        (1 - dx) * dy * dz * v011 + 
        dx * dy * dz * v111;
}

TPS_END_NAMESPACE