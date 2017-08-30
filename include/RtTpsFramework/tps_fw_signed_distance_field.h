#ifndef TPS_FW_SIGNED_DISTANCE_FIELD_H_
#define TPS_FW_SIGNED_DISTANCE_FIELD_H_

#include "tps_fw_defs.h"

#include "RtTpsFramework/tps_fw_volume_data.h"
#include "RtTpsFramework/tps_fw_arithmetic.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

class TPS_FW_DECLSPEC SignedDistanceField : public VolumeData<float>
{
public:
    SignedDistanceField();
    ~SignedDistanceField();
    SignedDistanceField(const SignedDistanceField& sdf);
    SignedDistanceField& operator=(const SignedDistanceField& sdf);

    inline float GetIsoLevel() const { return mIsoLevel; }
    inline void SetIsoLevel(float level) { mIsoLevel = level; }

    inline void GetGLFormat(
        unsigned int& type, 
        unsigned int& format, 
        unsigned int& internalFormat)
    {
        type = mType;
        format = mFormat;
        internalFormat = mInternalFormat;
    }

    inline MAT4 GetPatToSDF() const { return mMatPatToSDF; }
    inline void SetPatToSDF(const MAT4& mat) { mMatPatToSDF = mat; }

    //return main axis, -1 means oblique
    int GetParasInVol(const MAT4& matP2V, int* start, int* size) const;
    float GetValueInVol(const MAT4& matP2V, int x, int y, int z) const;

    float operator()(double x, double y, double z) const;
private:
    float mIsoLevel;

    MAT4 mMatPatToSDF;

    //for GL
    unsigned int mType;
    unsigned int mFormat;
    unsigned int mInternalFormat;
};

TPS_END_NAMESPACE  // end namespace tps

#endif // tps_fw_signed_distance_field_h__
