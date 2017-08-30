#include "StdAfx.h"

#include<string>
#include "gl/glew.h"
#include "RtTpsRenderLibrary/tps_rl_gl_program.h"
#include "RtTpsRenderLibrary/drr_gl_generator.h"
#include "../tps_rl_shaders.h"

TPS_BEGIN_NAMESPACE

DrrGLGenerator::DrrGLGenerator() : mInitialized(false), mWidth(0), mHeight(0), 
mFaceProgram(nullptr), mDrrProgram(nullptr), mSampleRate(1.f), 
mRescaleSlope(1.f), mRescaleIntercept(0.f), mEnergy(6.00f), mWaterAttenuation(0.0277f), 
mDensity2CtStart(0.f), mDensity2CtInterval(0.001f), mResultBuffer(nullptr){

}

DrrGLGenerator::~DrrGLGenerator(){
    if (mResultBuffer){
        delete[] mResultBuffer;
        mResultBuffer = nullptr;
    }
}

void DrrGLGenerator::Initialize(){
    if (mInitialized) return;
    //init programs
    mFaceProgram = new GLProgram();
    mFaceProgram->AttachShader(GLProgram::Vertex_Shader, Shaders::Get(IDR_SHADER_DrrFaceVert).c_str());
    mFaceProgram->AttachShader(GLProgram::Fragment_Shader, Shaders::Get(IDR_SHADER_DrrFaceFrag).c_str());
    mFaceProgram->Link();
    //mFaceProgram = new GLProgram(faceVertShader, faceFragShader);
    GLuint faceProgID = mFaceProgram->ID();
    mFaceLocations[0] = glGetAttribLocation(faceProgID, "vVertex");
    mFaceLocations[1] = glGetAttribLocation(faceProgID, "vColor");
    mFaceLocations[2] = glGetUniformLocation(faceProgID, "matMVP");

    mDrrProgram = new GLProgram();
    mDrrProgram->AttachShader(GLProgram::Vertex_Shader, Shaders::Get(IDR_SHADER_DrrVert).c_str());
    mDrrProgram->AttachShader(GLProgram::Fragment_Shader,Shaders::Get(IDR_SHADER_DrrFrag).c_str());
    mDrrProgram->Link();

    //mDrrProgram = new GLProgram(drrVertShader, drrFragShader);
    GLuint drrProgID = mDrrProgram->ID();
    mDrrLocations[0] = glGetAttribLocation(drrProgID, "vVertex");
    mDrrLocations[1] = glGetAttribLocation(drrProgID, "vInTexCoord");
    mDrrLocations[2] = glGetUniformLocation(drrProgID, "texVolume");
    mDrrLocations[3] = glGetUniformLocation(drrProgID, "texFrontFace");
    mDrrLocations[4] = glGetUniformLocation(drrProgID, "texBackFace");
    mDrrLocations[5] = glGetUniformLocation(drrProgID, "texCT2Density");
    mDrrLocations[6] = glGetUniformLocation(drrProgID, "iCT2DensityStart");
    mDrrLocations[7] = glGetUniformLocation(drrProgID, "vSpacing");
    mDrrLocations[8] = glGetUniformLocation(drrProgID, "fSampleRate");
    mDrrLocations[9] = glGetUniformLocation(drrProgID, "fRescaleIntercept");
    mDrrLocations[10] = glGetUniformLocation(drrProgID, "fRescaleSlope");
    mDrrLocations[11] = glGetUniformLocation(drrProgID, "fWaterAttenuation");
    mDrrLocations[12] = glGetUniformLocation(drrProgID, "texDensity2CT");
    mDrrLocations[13] = glGetUniformLocation(drrProgID, "fDensityStart");
    mDrrLocations[14] = glGetUniformLocation(drrProgID, "fDensityInterval");
    mDrrLocations[15] = glGetUniformLocation(drrProgID, "matModel2Patent");

    //VAO
    glGenVertexArrays(2, mVertexArrayID);
    //color object vao
    float cubeVert[] = {
        0.f, 0.f, 0.f, //0
        1.f, 0.f, 0.f, //1
        1.f, 1.f, 0.f, //2
        0.f, 1.f, 0.f, //3
        0.f, 0.f, 1.f, //4
        1.f, 0.f, 1.f, //5
        1.f, 1.f, 1.f, //6
        0.f, 1.f, 1.f  //7
    };
    unsigned short cubeIndex[] = {
        0, 1, 2, 3, //bottom
        7, 6, 5, 4, //top
        6, 7, 3, 2, //back
        0, 4, 5, 1, //front
        0, 3, 7, 4, //left
        1, 5, 6, 2  //right
    };

    glBindVertexArray(mVertexArrayID[0]);
    glGenBuffers(3, mFaceBuffers);

    glBindBuffer(GL_ARRAY_BUFFER, mFaceBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVert), cubeVert, GL_STATIC_DRAW);
    glVertexAttribPointer(mFaceLocations[0], 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(mFaceLocations[0]);

    glBindBuffer(GL_ARRAY_BUFFER, mFaceBuffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVert), cubeVert, GL_STATIC_DRAW);
    glVertexAttribPointer(mFaceLocations[1], 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(mFaceLocations[1]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFaceBuffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndex), cubeIndex, GL_STATIC_DRAW);

    //drr vao
    float planeVert[] = {
        -1.f, -1.f, 0.f, 
         1.f, -1.f, 0.f, 
         1.f,  1.f, 0.f, 
        -1.f,  1.f, 0.f};
    float texVert[] = {0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f};
    unsigned short planeIndex[] = {0, 1, 2, 0, 2, 3};
    glBindVertexArray(mVertexArrayID[1]);
    glGenBuffers(3, mDrrBuffers);
    glBindBuffer(GL_ARRAY_BUFFER, mDrrBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVert), planeVert, GL_STATIC_DRAW);
    glVertexAttribPointer(mDrrLocations[0], 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(mDrrLocations[0]);

    glBindBuffer(GL_ARRAY_BUFFER, mDrrBuffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texVert), texVert, GL_STATIC_DRAW);
    glVertexAttribPointer(mDrrLocations[1], 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(mDrrLocations[1]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mDrrBuffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndex), planeIndex, GL_STATIC_DRAW);
    glBindVertexArray(0);

    //FBO for front/back face rendering
    glGenTextures(4, mTexID);
    glBindTexture(GL_TEXTURE_2D, mTexID[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, mTexID[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, mTexID[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, mTexID[3]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &mFboID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFboID);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexID[0], 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mTexID[1], 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mTexID[2], 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mTexID[3], 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    //density to ct texture
    int templateCount = 33;
    const float templateDensity[33] = {
        0.001207f, 0.021787f, 0.052693f, 0.104161f, 0.155659f, 0.207141f, 0.258619f,
        0.413081f, 0.619019f, 0.773481f, 0.907339f, 0.943881f, 0.970671f, 0.997461f,
        1.000000f, 1.054400f, 1.072400f, 1.088040f, 1.135400f, 1.194600f,
        1.253800f, 1.313000f, 1.372200f, 1.431400f, 1.490600f, 1.549800f, 1.609000f,
        1.668200f, 1.727400f, 1.786600f, 1.845800f, 1.905000f, 1.920000f};
    const float templateCT[33] = {
        -1000.0f, -980.0f, -950.0f, -900.0f, -850.0f, -800.0f, -750.0f, 
        -600.0f, -400.0f, -250.0f, -120.0f, -83.0f, -53.0f, -23.0f, 0.0f, 
        18.0f, 80.0f, 120.0f, 200.0f, 300.0f, 400.0f, 500.0f, 600.0f, 700.0f, 
        800.0f, 900.0f, 1000.0f, 1100.0f,1200.0f, 1300.0f,1400.0f, 1500.0f, 1600.0f};
    int targetCount = 3000;
    float* targetDensity = new float[targetCount];
    for (int i = 0; i < targetCount; ++i){
        targetDensity[i] = mDensity2CtStart + mDensity2CtInterval * i;
    }
    float* ctBuffer = new float[targetCount];
    memset(ctBuffer, 0, sizeof(float) * targetCount);
    this->LinearInterp_i(templateCount, templateDensity, templateCT, 
        targetCount, targetDensity, ctBuffer);
    delete[] targetDensity;
    glGenTextures(1, &mDensity2CtTexID);
    glBindTexture(GL_TEXTURE_1D, mDensity2CtTexID);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_LUMINANCE32F_ARB, targetCount, 0, 
        GL_LUMINANCE, GL_FLOAT, ctBuffer);
    glBindTexture(GL_TEXTURE_1D, 0);
    delete[] ctBuffer;

    mInitialized = true;
}

void DrrGLGenerator::Finialize(){
    glDeleteBuffers(3, mFaceBuffers);
    glDeleteBuffers(3, mDrrBuffers);
    glDeleteVertexArrays(2, mVertexArrayID);
    glDeleteTextures(4, mTexID);
    glDeleteFramebuffers(1, &mFboID);
    glDeleteTextures(1, &mDensity2CtTexID);
    delete mFaceProgram;
    delete mDrrProgram;
    mInitialized = false;
}

bool DrrGLGenerator::GenerateDrrImage(float* drrImage){
    if (drrImage == nullptr){
        //LOG null ptr
        return false;
    }

    if (!mInitialized || mWidth < 0 || mHeight < 0 || mResultBuffer == nullptr){
        //LOG not initialized
        return false;
    }

    if (!Render_i()){
        //LOG render failed
        return false;
    }

    AdaptBuffer_i(drrImage);
    return true;
}

bool DrrGLGenerator::Render_i(){
    glPushAttrib(GL_ENABLE_BIT);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFboID);
    glViewport(mViewportStart[0], mViewportStart[1], 
        mViewportSize[0], mViewportSize[1]);
    glUseProgram(mFaceProgram->ID());
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
 
    //upload mvp matrix
    glUniformMatrix4fv(mFaceLocations[2], 1, GL_FALSE, mMvpMatrix);
 
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //render front face
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBindVertexArray(mVertexArrayID[0]);
    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
    //render back face
    glDrawBuffer(GL_COLOR_ATTACHMENT1);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glCullFace(GL_FRONT);
    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
    glDisable(GL_CULL_FACE);
    glUseProgram(0);

    //render drr
    glDrawBuffer(GL_COLOR_ATTACHMENT2);
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(mDrrProgram->ID());
    //set volume texture id
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, mVolumeTexID);
    glUniform1i(mDrrLocations[2], 0);
    //set front face texture 
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mTexID[0]);
    glUniform1i(mDrrLocations[3], 1);
    //set back face texture
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mTexID[1]);
    glUniform1i(mDrrLocations[4], 2);
    //set back face texture
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_1D, mCt2DensityTexID);
    glUniform1i(mDrrLocations[5], 3);
    glUniform1i(mDrrLocations[6], mCt2DensityTableStart);
    //set spacing
    glUniform3f(mDrrLocations[7], mSpacing[0], mSpacing[1], mSpacing[2]);
    //set down sample rate
    glUniform1f(mDrrLocations[8], mSampleRate);
    //set rescale intercept
    glUniform1f(mDrrLocations[9], mRescaleIntercept);
    //set rescale slope
    glUniform1f(mDrrLocations[10], mRescaleSlope);
    //set water attenuation
    glUniform1f(mDrrLocations[11], mWaterAttenuation);
    //set density to ct texture
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_1D, mDensity2CtTexID);
    glUniform1i(mDrrLocations[12], 4);
    glUniform1f(mDrrLocations[13], mDensity2CtStart);
    glUniform1f(mDrrLocations[14], mDensity2CtInterval);
    //set model to patient matrix
    glUniformMatrix4fv(mDrrLocations[15], 1, GL_FALSE, mModel2PatientMatrix);

    glBindVertexArray(mVertexArrayID[1]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    glFlush();
    glUseProgram(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

    memset(mResultBuffer, 0, sizeof(float) * mWidth * mHeight);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mFboID);
    //front buffer
    glReadBuffer(GL_COLOR_ATTACHMENT2);
    GLenum status = (GLenum)glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        //log 
        //printf("\nError when set read buffer.");
        return false;
    }
    glReadPixels(0, 0, mWidth, mHeight, GL_RED, GL_FLOAT, mResultBuffer);

    status = (GLenum)glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        //log
        //printf("\nError when glReadPixels.");
        return false;
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glPopAttrib();
    return true;
}

void DrrGLGenerator::AdaptBuffer_i(float* drrImage){
    float min(mResultBuffer[0]), max(mResultBuffer[0]);
    for (int i = 1; i < mWidth * mHeight; ++i){
        min = (mResultBuffer[i] < min) ? mResultBuffer[i] : min;
        max = (mResultBuffer[i] > max) ? mResultBuffer[i] : max;
    }

    if (min == max){
        memset(drrImage, 0, sizeof(float) * mWidth * mHeight);
        return;
    }
    float slope = 1.f / (max - min);
    float intercept = -1.f * min / (max - min);
    for (int i = 0; i < mWidth * mHeight; ++i){
        drrImage[i] = mResultBuffer[i] * slope + intercept;
    }
}

void DrrGLGenerator::Resize(int iWidth, int iHeight){
    if (iWidth * iHeight != mWidth * mHeight || mResultBuffer == nullptr){
        if (mResultBuffer) delete[] mResultBuffer;
        mResultBuffer = new float[iWidth * iHeight];
    }
    mWidth = iWidth;
    mHeight = iHeight;

    glBindTexture(GL_TEXTURE_2D, mTexID[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, mTexID[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, mTexID[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, mTexID[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, mWidth, mHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void DrrGLGenerator::SetViewportRegion(int startX, int startY, unsigned int sizeX, unsigned int sizeY){
    mViewportStart[0] = startX;
    mViewportStart[1] = startY;
    mViewportSize[0] = sizeX;
    mViewportSize[1] = sizeY;
}

void DrrGLGenerator::SetMVP(float matrix[16]){
    memcpy(mMvpMatrix, matrix, 16 * sizeof(float));
}

void DrrGLGenerator::SetVolumeTexture(unsigned int texID){
    mVolumeTexID = texID;
}

void DrrGLGenerator::SetCt2Density(int start, unsigned int size, unsigned int texID){
    mCt2DensityTableStart = start;
    mCt2DensityTableSize = size;
    mCt2DensityTexID = texID;
}

void DrrGLGenerator::SetSpacing(float spacing[3]){
    memcpy(mSpacing, spacing, 3 * sizeof(float));
}

void DrrGLGenerator::SetSampleRate(float rate){
    mSampleRate = rate;
}

void DrrGLGenerator::SetRescalePara(float slope, float intercept){
    mRescaleSlope = slope;
    mRescaleIntercept = intercept;
}

void DrrGLGenerator::SetEnergy(float energy){
    if (mEnergy != energy){
        mEnergy = energy;

        int iNum = 36;
        const float fEnergyRange[36] = {
            0.001f,0.0015f,0.002f,0.003f,0.004f,0.005f,0.006f,0.008f,
            0.010f,0.015f,0.02f,0.03f,0.04f,0.05f,0.06f,0.08f,
            0.10f,0.15f,0.20f,0.30f,0.40f,0.50f,0.60f,0.80f,
            1.00f,1.25f,1.50f,2.00f,3.00f,4.00f,5.00f,6.00f,8.00f,10.0f,15.0f,20.0f};
        const float fWaterAttCoeffRange[36] = {
                4078.0f,1376.0f, 617.3f, 192.9f, 82.78f, 42.58f, 24.64f, 10.37f,5.329f,
                1.673f, 0.8096f, 0.3756f, 0.2683f, 0.2269f, 0.2059f, 0.183700f, 0.1707f, 
                0.1505f, 0.137f, 0.1186f, 0.1061f, 0.09687f, 0.08956f, 0.07865f, 0.07072f, 
                0.06323f, 0.05754f, 0.04942f, 0.03969f, 0.03403f, 0.03031f, 0.0277f, 
                0.02429f, 0.02219f, 0.01941f, 0.01813f};
        this->LinearInterp_i(iNum, fEnergyRange, fWaterAttCoeffRange, 1, &mEnergy, &mWaterAttenuation);
    }
}

void DrrGLGenerator::SetModel2Patient(float matrix[16]){
    memcpy(mModel2PatientMatrix, matrix, 16 * sizeof(float));
}

void DrrGLGenerator::LinearInterp_i(
    int iCountIn, const float fxIn[], const float fyIn[], 
    int iCountOut, const float *fxOut, float *fyOut){

    // allocate local memory
    float *fSlope     = new float[iCountIn];
    float *fIntercept = new float[iCountIn];

    // solve the slope and intercept
    for (int i = 0;i < iCountIn - 1; i++)
    {
        fSlope[i] = (fyIn[i+1] - fyIn[i]) / (fxIn[i+1] - fxIn[i]);
        fIntercept[i] = fyIn[i] - fSlope[i] * fxIn[i];
    }
    fSlope[iCountIn - 1] = fSlope[iCountIn - 2];
    fIntercept[iCountIn - 1] = fIntercept[iCountIn - 2];

    // solve the corresponding points 
    int index = 0;
    for(int i = 0; i < iCountOut; i++)
    {
        while(index < iCountIn && fxIn[index] < fxOut[i]){
            index++;
        }
        index = index < 0 ? 0 : index;
        index = index >= iCountIn ? iCountIn - 1 : index;
        fyOut[i] = fxOut[i] * fSlope[index] + fIntercept[index];
    }

    delete[] fSlope;
    delete[] fIntercept;
}

TPS_END_NAMESPACE