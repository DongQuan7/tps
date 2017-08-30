#ifndef DRR_GL_GENERATOR_H_
#define DRR_GL_GENERATOR_H_

#include "tps_rl_defs.h"

TPS_BEGIN_NAMESPACE
class GLProgram;
// begin namespace ***

/// \class DrrGLGenerator
/// 
/// \brief *****
class __declspec(dllexport) DrrGLGenerator {
public:
    //the constructor and destructor
    DrrGLGenerator();
    ~DrrGLGenerator();

    bool GenerateDrrImage(float* drrImage);

    // \brief initialize all gl resources
    void Initialize();

    // \brief clear all gl resources
    void Finialize();

    /// \brief  The resize method is only needed in gpu implementation
    ///         to reallocate any gpu resource according to the new
    ///         size of the screen
    /// \param[in] iWidth The width of the screen in pixels
    /// \param[in] iHeight The height of the screen in pixels
    void Resize(int iWidth, int iHeight);

    // \brief set viewport region to display drr image
    void SetViewportRegion(int startX, int startY, unsigned int sizeX, unsigned int sizeY);

    // \brief set model view projection matrix
    void SetMVP(float matrix[16]);

    // \brief set volume 3D texture ID
    void SetVolumeTexture(unsigned int texID);

    // \brief set ct 2 density table
    void SetCt2Density(int start, unsigned int size, unsigned int texID);

    // \brief set spacing in volume model c.s.
    void SetSpacing(float spacing[3]);

    // \brief set sample rate
    void SetSampleRate(float rate);

    // \breif set rescale paras
    void SetRescalePara(float slope, float intercept);

    // \brief set energy
    void SetEnergy(float energy);

    // \brief set model to patient matrix
    void SetModel2Patient(float matrix[16]);
private:
    bool Render_i();

    //map rendering result to [0.f, 1.f]
    void AdaptBuffer_i(float* drrImage);

    void LinearInterp_i(
        int iCountIn, const float fxIn[], const float fyIn[], 
        int iCountOut, const float *fxOut, float *fyOut);

    bool                mInitialized;

    int                 mWidth;
    int                 mHeight;
    float*              mResultBuffer;

    GLProgram*          mFaceProgram;
    GLProgram*          mDrrProgram;

    unsigned int        mVertexArrayID[2];
    unsigned int        mFaceBuffers[3];
    unsigned int        mDrrBuffers[3];
    //front face, back face, rendering result, depth_stencil buffer
    unsigned int        mTexID[4];
    unsigned int        mDensity2CtTexID;
    float               mDensity2CtStart;
    float               mDensity2CtInterval;
    unsigned int        mFaceLocations[3];
    unsigned int        mDrrLocations[16];
    unsigned int        mFboID;

    //parameters need to set
    //viewport region
    int                 mViewportStart[2];
    unsigned int        mViewportSize[2];
    //model view projection matrix, column priority
    float               mMvpMatrix[16];
    //volume 3d texture id
    unsigned int        mVolumeTexID;
    //ct 2 density table
    int                 mCt2DensityTableStart;
    unsigned int        mCt2DensityTableSize;
    unsigned int        mCt2DensityTexID;
    //spacing in volume model c.s.
    float               mSpacing[3];
    //sample rate
    float               mSampleRate;
    //rescale intercept
    float               mRescaleIntercept;
    //rescale slope
    float               mRescaleSlope;
    //energy
    float               mEnergy;
    //water attenuation
    float               mWaterAttenuation;
    //model to patient matrix, column priority
    float               mModel2PatientMatrix[16];
private:
    // the default copy and assign method are not allowed.
    DrrGLGenerator(const DrrGLGenerator&);
    void operator = (const DrrGLGenerator &);
};

// end namespace ***
TPS_END_NAMESPACE
#endif

