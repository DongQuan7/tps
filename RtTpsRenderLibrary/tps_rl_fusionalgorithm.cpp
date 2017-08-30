#include "StdAfx.h"

#include "RtTpsRenderLibrary/tps_rl_fusionalgorithm.h"

#include "McsfMPRVRAlgorithm/mcsf_gpu_algorithm_base.h"
#include "McsfViewer3DResource/mcsf_viewer3d_texture_3d.h"//Mcsf::MedViewer3D::Texture3D
#include "McsfMedViewer3DDataRepresentation/image_data.h"
#include "McsfMPRVRAlgorithm/mcsf_algo_step_base.h"

TPS_BEGIN_NAMESPACE // begin namespace tps

FusionGpuMprAlgorithm::FusionGpuMprAlgorithm()
    :GpuMprAlgorithm()
{

}

void FusionGpuMprAlgorithm::AddShader(GLenum uiShaderType, const char* pShaderTextPtr)
{
    m_vecShaderType.push_back(std::make_pair(uiShaderType, pShaderTextPtr));
}

void FusionGpuMprAlgorithm::GetShaderParamUniformID()
{
    if (m_vecShaderType.size()<1)
    {
        //LOG
        return;
    }
    unsigned int uiProgram = m_uiProgramID;//m_pGpuMprAlgorithm->GetProgramID();
    if (-1 == (m_iUniformIDModelMatrix = glGetUniformLocation(uiProgram, "mMatrix")))
    {
        printf("Can't get ID of uniform property 'mMatrix'!");
    }
    if (-1 == (m_iUniformIDVPMatrix = glGetUniformLocation(uiProgram, "vpMatrix")))
    {
        printf("Can't get ID of uniform property 'vpMatrix'!");
    }
    if (-1 == (m_iUniformIDW2MMatrix = glGetUniformLocation(uiProgram, "w2mMatrix")))
    {
        printf("Can't get ID of uniform property 'w2mMatrix'!");
    }
    if (-1 == (m_iUniformIDFusionMatrix = glGetUniformLocation(uiProgram, "FusionMatrix")))
    {
        printf("Can't get ID of uniform property 'FusionMatrix'!");
    }

    if (-1 == (m_iUniformIDStart = glGetUniformLocation(uiProgram, "inVecStart")))
    {
        printf("Can't get ID of uniform property 'inVecStart'!");
    }
    if (-1 == (m_iUniformIDEnd = glGetUniformLocation(uiProgram, "inVecEnd")))
    {
        printf("Can't get ID of uniform property 'inVecEnd'!");
    }
    if (-1 == (m_iAttribIDVertex = glGetAttribLocation(uiProgram, "inVertex")))
    {
        printf("Can't get ID of attribute 'inVertex'!");
    }

    //////////////////////////////////////////////////////////////////////////
    if (-1 == (m_iUniformIDWindowing = glGetUniformLocation(uiProgram, "vecWindowing")))
    {
        printf("Can't get ID of uniform property 'vecWindowing'!");
    }
    if (-1 == (m_iUniformIDInverted = glGetUniformLocation(uiProgram, "iInverted")))
    {
        printf("Can't get ID of uniform property 'iInverted'!");
    }
    //if (-1 == (m_iUnifromIDSteps = glGetUniformLocation(uiProgram, "fSteps")))
    //{
    //    printf("Can't get ID of uniform property 'fSteps'!");
    //}
    if (-1 == (m_iUniformIDPseudoColorTable = glGetUniformLocation(uiProgram, "sPseudoColorTable")))
    {
        printf("Can't get ID of uniform property 'sPseudoColorTable'!");
    }
    if (-1 == (m_iUniformIDPseudoColorTable2 = glGetUniformLocation(uiProgram, "sPseudoColorTable2")))
    {
        printf("Can't get ID of uniform property 'sPseudoColorTable2'!");
    }
    if (-1 == (m_iUniformIDPseudoColorTableSize = glGetUniformLocation(uiProgram, "fPseudoColorTableSize")))
    {
        printf("Can't get ID of uniform property 'fPseudoColorTableSize'!");
    }

    //////////////////////////////////////////////////////////////////////////
    m_iUniformIDDataVolume = glGetUniformLocation(uiProgram, "vDataVolume");
    if (-1 == m_iUniformIDDataVolume)
    {
        printf("Can't get ID of uniform property 'vDataVolume'! ");
    }
    m_iUniformIDDataTexShift = glGetUniformLocation(uiProgram, "vecDataTexShift");
    if (-1 == m_iUniformIDDataTexShift)
    {
        printf("Can't get ID of uniform property 'vecDataTexShift'! ");
    }
    m_iUniformIDDataTexShiftFusion = glGetUniformLocation(uiProgram, "vecDataTexShiftFusion");
    if (-1 == m_iUniformIDDataTexShiftFusion)
    {
        printf("Can't get ID of uniform property 'vecDataTexShiftFusion'! ");
    }
    m_iUniformIDFloatingFlag = glGetUniformLocation(uiProgram, "FloatingFlag");
    if (-1 == m_iUniformIDFloatingFlag)
    {
        printf("Can't get ID of uniform property 'FloatingFlag'! ");
    }
    //////////////////////////////////////////////////////////////////////////
    //if (-1 == (m_iUniformIDDataBrickVolume = glGetUniformLocation(uiProgram, "vDataBrickVolume")))
    //{
    //    printf("Can't get ID of uniform property 'vDataBrickVolume'");
    //}
    //if (-1 == (m_iUniformIDDataBrickTexShift = glGetUniformLocation(uiProgram, "vecDataBrickTexShift")))
    //{
    //    printf("Can't get ID of uniform property 'vecDataBrickTexShift'");
    //}

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    m_iUniformOriginalPoint = glGetUniformLocation(uiProgram, "OriginalPoint");
    if (-1 == m_iUniformOriginalPoint)
    {
        printf("Can't get ID of uniform m_iUniformOriginalPoint (OriginalPoint):");
    }
    m_iUniformOriginalPoint2 = glGetUniformLocation(uiProgram, "OriginalPoint2");
    if (-1 == m_iUniformOriginalPoint2)
    {
        printf("Can't get ID of uniform m_iUniformOriginalPoint (OriginalPoint2):");
    }


    m_iUniformDirection = glGetUniformLocation(uiProgram, "Direction");
    if (-1 == m_iUniformDirection)
    {
        printf("Can't get ID of uniform m_iUniformDirection (Direction):");
    }
    m_iUniformDirection2 = glGetUniformLocation(uiProgram, "Direction2");
    if (-1 == m_iUniformDirection2)
    {
        printf("Can't get ID of uniform m_iUniformDirection (Direction2):");
    }

    m_iUniformDisplaySize = glGetUniformLocation(uiProgram, "DisplaySize");
    if (-1 == m_iUniformDisplaySize)
    {
        printf("Can't get ID of uniform m_iUniformDisplaySize (DisplaySize):" );
    }

    m_iUniformVolumeSize = glGetUniformLocation(uiProgram, "VolumeSize");
    if (-1 == m_iUniformVolumeSize)
    {
        printf("Can't get ID of uniform m_iUniformVolumeSize (VolumeSize):"); 
    }

    // color table for MPR
    //m_iUniformColorTable = glGetUniformLocation(uiProgram, "ColorTable");
    m_iUniformWindowWidthLevel2 = glGetUniformLocation(uiProgram, "vecWindowing2");
    if (-1 == m_iUniformWindowWidthLevel2)
    {
        printf( "Can't get ID of uniform m_iUniformWindowWidthLevel2 (vecWindowing2):"); 
    }

    m_iUniformSecondVolume = glGetUniformLocation(uiProgram,"vDataVolume2");
    if (-1 == m_iUniformSecondVolume)
    {
        printf("Can't get ID of uniform m_iUniformSecondVolume (vDataVolume2):" );
    }

    m_iUniformVolume2Size = glGetUniformLocation(uiProgram,"Volume2Size");
    if (-1 == m_iUniformVolumeSize)
    {
        printf( "Can't get ID of uniform m_iUniformVolume2Size (Volume2Size):");
    }

    m_iUniformAlpha1 = glGetUniformLocation(uiProgram,"fAlpha1");
    if (-1 == m_iUniformAlpha1)
    {
        printf("Can't get ID of uniform m_iUniformAlpha1 (fAlpha1):");
    }
}

void FusionGpuMprAlgorithm::SetUniformValue()
{
    float fMatrix[16];
    Mcsf::MedViewer3D::Matrix4x4 matM2W = this->GetMode2World_Plane();
    for (int i = 0; i < 16; ++i)
    {
        fMatrix[i] = static_cast<float>(matM2W._m[i]);
    }
    glUniformMatrix4fv(m_iUniformIDModelMatrix, 1, false, fMatrix);

    matM2W = this->GetViewProjection();
    for (int i = 0; i < 16; ++i)
    {
        fMatrix[i] = static_cast<float>(matM2W._m[i]);
    }
    glUniformMatrix4fv(m_iUniformIDVPMatrix, 1, false, fMatrix);

    matM2W = this->GetWorld2Model_Volume();
    for (int i = 0; i < 16; ++i)
    {
        fMatrix[i] = static_cast<float>(matM2W._m[i]);
    }
    glUniformMatrix4fv(m_iUniformIDW2MMatrix, 1, false, fMatrix);

    Mcsf::MedViewer3D::Vector3D vec3D = this->GetPlaneToStartVector();
    glUniform4f(m_iUniformIDStart,static_cast<GLfloat>(vec3D.x), 
        static_cast<GLfloat>(vec3D.y), static_cast<GLfloat>(vec3D.z), 0.0f);
    vec3D = this->GetPlaneToEndVector();
    glUniform4f(m_iUniformIDEnd,static_cast<GLfloat>(vec3D.x), 
        static_cast<GLfloat>(vec3D.y), static_cast<GLfloat>(vec3D.z), 0.0f);

    //////////////////////////////////////////////////////////////////////////
    float fWindowWidth, fWindowLevel;
    this->GetWindowLevel(&fWindowWidth, &fWindowLevel);
    glUniform2f(m_iUniformIDWindowing, fWindowWidth, fWindowLevel);

    float fSteps = this->GetSteps();
    glUniform1f(m_iUnifromIDSteps, fSteps);

    int iInverted = this->GetColorInverted();
    glUniform1i(m_iUniformIDInverted, iInverted);

    //About pseudo color table
    int iTexPseudoTU = 13;
    glEnable(GL_TEXTURE_1D);
    glActiveTexture(GL_TEXTURE0 + iTexPseudoTU);
    glBindTexture(GL_TEXTURE_1D, this->GetPseudoColorTexID());
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glUniform1i(m_iUniformIDPseudoColorTable, iTexPseudoTU);

    iTexPseudoTU += 1;
    glEnable(GL_TEXTURE_1D+iTexPseudoTU);
    glActiveTexture(GL_TEXTURE0 + iTexPseudoTU);
    glBindTexture(GL_TEXTURE_1D, m_uiFusionColorID);
    glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glUniform1i(m_iUniformIDPseudoColorTable2, iTexPseudoTU);
    //glDisable(GL_TEXTURE_1D);

    glUniform1f(m_iUniformIDPseudoColorTableSize, static_cast<float>(this->GetPseudoColorTableSize()));

    //////////////////////////////////////////////////////////////////////////
    /// Volume Data Texture
    int volumeTU = 0;
    glActiveTexture(GL_TEXTURE0 + volumeTU);
    unsigned int uiVolumeTex = this->GetDataVolumeTexID();
    glBindTexture(GL_TEXTURE_3D, uiVolumeTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUniform1i(m_iUniformIDDataVolume, volumeTU);

    //Volume2
    typedef std::shared_ptr<Mcsf::MedViewer3D::Texture3D> Texture3DPtr;
    Texture3DPtr pTex3D2;
    if (nullptr != m_pVolumeDisplay)
    {
        pTex3D2 = m_pVolumeDisplay->Get3DTexID();
    }

    float BorderColor2[4] = { 0.0 };
    glActiveTexture(GL_TEXTURE1);
    //pTex3D2->Bind();
    pTex3D2->Bind();
    glEnable(GL_TEXTURE_1D);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR,  BorderColor2);
    glUniform1i(m_iUniformSecondVolume, 1);
    glDisable(GL_TEXTURE_1D);

    unsigned int uiDim[3];
    this->GetVolumeDim(uiDim);
    float fShift[3];
    fShift[0] = 0.5f / uiDim[0];
    fShift[1] = 0.5f / uiDim[1];
    fShift[2] = 0.5f / uiDim[2];

    glUniform3fv(m_iUniformIDDataTexShift, 1, fShift);
    glUniform1i(m_iUniformIDFloatingFlag, this->GetMprFloatingFlag());


    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    // Volume1
    Mcsf::MedViewer3D::Matrix4x4 matWorldToMainVolume = this->GetWorld2Model_Volume();
    Mcsf::MedViewer3D::Matrix4x4 matMainVolumeToDisplayVolume;
    matMainVolumeToDisplayVolume = this->GetFusionRegistrationMatrix();
    matMainVolumeToDisplayVolume = matMainVolumeToDisplayVolume.Inverse();
    //matMainVolumeToDisplayVolume.m[3][0] += 5.0f;
    for (int i = 0; i < 16; ++i)
    {
        fMatrix[i] = static_cast<float>(matMainVolumeToDisplayVolume._m[i]);
    }

    glUniformMatrix4fv(m_iUniformIDFusionMatrix, 1, false, fMatrix);

    // WindowWidthLevel
    /*double dWindowWidth = 0.0;
    double dWindowCenter = 0.0;*/
    //pMPR->GetFusionWindowLevel(m_vecFusionVolumeUID[0], dWindowWidth, dWindowCenter);

    //pVolume->RegulateWindowLevel(dWindowCenter, dWindowWidth);
    //glUniform2f(m_iUniformWindowWidthLevel,        static_cast<float>(dWindowWidth), static_cast<float>(dWindowCenter));

    // Steps
    //double dThickness = pMPR->GetThickness();

    //double dStepLength = std::min(
    //    matMainVolumeToWorld.Inverse().Transform(Vector3D(1, 0, 0)).Magnitude(),
    //    std::min(matMainVolumeToWorld.Inverse().Transform(Vector3D(0, 1, 0)).Magnitude(),
    //    matMainVolumeToWorld.Inverse().Transform(Vector3D(0, 0, 1)).Magnitude()));
    //double dStepNum = std::floor(dThickness * 0.5 / dStepLength);
    //glUniform2f(m_iUniformStepWidthNumber, 1,
    //    static_cast<float>(dStepNum));

    // OriginalPoint 
    Mcsf::MedViewer3D::Point3D aVertex[4];
    // pMPR->GetVertices(aVertex);
    aVertex[0] = m_vertexPoints[0];
    aVertex[1] = m_vertexPoints[1];
    aVertex[2] = m_vertexPoints[2];
    aVertex[3] = m_vertexPoints[3];

    Mcsf::MedViewer3D::Point3D ptOriginInVolume = matWorldToMainVolume.Transform(aVertex[0]);
    Mcsf::MedViewer3D::Point3D ptOriginInVolume2 = matMainVolumeToDisplayVolume.Transform(ptOriginInVolume);

    // Direction
    Mcsf::MedViewer3D::Vector3D vAxisX = aVertex[1] - aVertex[0];
    Mcsf::MedViewer3D::Vector3D vAxisY = aVertex[3] - aVertex[0];
    Mcsf::MedViewer3D::Vector3D vAxisZ = vAxisX.CrossProduct(vAxisY);
    vAxisZ.Normalize();
    //vAxisZ *= dThickness;
    Mcsf::MedViewer3D::Vector3D vAxixXInVolume = matWorldToMainVolume.Transform(vAxisX);
    Mcsf::MedViewer3D::Vector3D vAxisYInVolume = matWorldToMainVolume.Transform(vAxisY);
    Mcsf::MedViewer3D::Vector3D vAxisZInVolume = matWorldToMainVolume.Transform(vAxisZ);
    //if (dStepNum >= MCSF_3D_DOUBLE_ZERO)
    //{
    //    vAxisZInVolume *= (0.5 / dStepNum);
    //}

    float arrDirection[9] = { 0.0 };
    arrDirection[0] = static_cast<float>(vAxixXInVolume.x);
    arrDirection[1] = static_cast<float>(vAxixXInVolume.y);
    arrDirection[2] = static_cast<float>(vAxixXInVolume.z);
    arrDirection[3] = static_cast<float>(vAxisYInVolume.x);
    arrDirection[4] = static_cast<float>(vAxisYInVolume.y);
    arrDirection[5] = static_cast<float>(vAxisYInVolume.z);
    arrDirection[6] = static_cast<float>(vAxisZInVolume.x);
    arrDirection[7] = static_cast<float>(vAxisZInVolume.y);
    arrDirection[8] = static_cast<float>(vAxisZInVolume.z);

    Mcsf::MedViewer3D::Vector3D vAxixXInVolume2 = matMainVolumeToDisplayVolume.Transform(vAxixXInVolume);
    Mcsf::MedViewer3D::Vector3D vAxisYInVolume2 = matMainVolumeToDisplayVolume.Transform(vAxisYInVolume);
    Mcsf::MedViewer3D::Vector3D vAxisZInVolume2 = matMainVolumeToDisplayVolume.Transform(vAxisZInVolume);
    float arrDirection2[9] = { 0.0 };
    arrDirection2[0] = static_cast<float>(vAxixXInVolume2.x);
    arrDirection2[1] = static_cast<float>(vAxixXInVolume2.y);
    arrDirection2[2] = static_cast<float>(vAxixXInVolume2.z);
    arrDirection2[3] = static_cast<float>(vAxisYInVolume2.x);
    arrDirection2[4] = static_cast<float>(vAxisYInVolume2.y);
    arrDirection2[5] = static_cast<float>(vAxisYInVolume2.z);
    arrDirection2[6] = static_cast<float>(vAxisZInVolume2.x);
    arrDirection2[7] = static_cast<float>(vAxisZInVolume2.y);
    arrDirection2[8] = static_cast<float>(vAxisZInVolume2.z);

    // Original Point
    glUniform3f(m_iUniformOriginalPoint,
        static_cast<float>(ptOriginInVolume.x),
        static_cast<float>(ptOriginInVolume.y),
        static_cast<float>(ptOriginInVolume.z));

    glUniform3f(m_iUniformOriginalPoint2,
        static_cast<float>(ptOriginInVolume2.x),
        static_cast<float>(ptOriginInVolume2.y),
        static_cast<float>(ptOriginInVolume2.z));

    // Direction
    glUniform3fv(m_iUniformDirection, 3, arrDirection);
    glUniform3fv(m_iUniformDirection2, 3, arrDirection2);

    // DisplaySize
    int aDisplaySize[2] = { m_iWidth, m_iHeight };
    //   m_spMPR->GetDisplaySize(&aDisplaySize[0], &aDisplaySize[1]);
    glUniform2f(m_iUniformDisplaySize,
        static_cast<float>(aDisplaySize[0]),
        static_cast<float>(aDisplaySize[1]));

    // VolumeSize
    unsigned int aVolumeSize[3] = { 0 };
    this->GetVolumeDim(aVolumeSize);
    //pVolume->GetDimension(aVolumeSize);
    glUniform3f(m_iUniformVolumeSize,
        static_cast<float>(aVolumeSize[0]),
        static_cast<float>(aVolumeSize[1]),
        static_cast<float>(aVolumeSize[2]));

    size_t aVolumeSize2[3] = { 0 };
    //pVolume2->GetDimension(aVolumeSize2);
    m_pVolumeDisplay->GetDimension(aVolumeSize2);
    glUniform3f(m_iUniformVolume2Size,
        static_cast<float>(aVolumeSize2[0]),
        static_cast<float>(aVolumeSize2[1]),
        static_cast<float>(aVolumeSize2[2]));


    float fShiftFuison[3];
    fShiftFuison[0] = 0.5f / aVolumeSize2[0];
    fShiftFuison[1] = 0.5f / aVolumeSize2[1];
    fShiftFuison[2] = 0.5f / aVolumeSize2[2];

    glUniform3fv(m_iUniformIDDataTexShiftFusion, 1, fShiftFuison);

    // Channels
    //glUniform1i(m_iUniformChannelNum,
    //    static_cast<int>(pVolume->GetDataChannel()));
    //glUniform1i(m_iUniformChannelNum2,
    //    static_cast<int>(pVolume2->GetDataChannel()));

    //Blend factor
    float fAlpha1 = 0.5;
    //float fAlpha2 = 0.0;
    //fAlpha1 = pMPR->GetFusionFactor(m_vecFusionVolumeUID[0]);
    //fAlpha2 = pMPR->GetFusionFactor(m_vecFusionVolumeUID[1]);
    glUniform1f(m_iUniformAlpha1, fAlpha1);

    //XYZ RGB
    //float XYZ2RGB[9] = { 2.3707f, -0.9001f, -0.4706f, -0.5139f, 1.4253f, 0.0886f, 0.0053f, -0.0147f, 1.0094f };
    //glUniform3fv(m_iXYZ2RGB, 3, XYZ2RGB);

    //float RGB2XYZ[9] = {0.4887f, 0.3107f, 0.2006f, 0.1762f, 0.8130f, 0.0108f, 0.0000f, 0.0102f, 0.9898f };
    //glUniform3fv(m_iRGB2XYZ, 3, RGB2XYZ);

    // WindowWidthLevel
    double dWindowingWidth2(m_dWindowingWidth), dWindowingCenter2(m_dWindowingCenter);

    // pMPR->GetFusionWindowLevel(m_vecFusionVolumeUID[1], dWindowingWidth2, dWindowingCenter2);
    m_pVolumeDisplay->RegulateWindowLevel(dWindowingCenter2, dWindowingWidth2);
    glUniform2f(m_iUniformWindowWidthLevel2,
        static_cast<float>(dWindowingWidth2), static_cast<float>(dWindowingCenter2));

    matM2W = this->GetWorld2Model_Volume();

    Mcsf::MedViewer3D::Point3D ptModelInMain = matM2W.Transform(aVertex[0] + (aVertex[2] - aVertex[0]) * 0.5);
    Mcsf::MedViewer3D::Point3D ptVolumeInMain(ptModelInMain.x * aVolumeSize[0], 
        ptModelInMain.y * aVolumeSize[1], 
        ptModelInMain.z * aVolumeSize[2]);
    Mcsf::MedViewer3D::Point3D ptVolumeInSec = matMainVolumeToDisplayVolume.Transform(ptVolumeInMain);
    Mcsf::MedViewer3D::Point3D ptModelInSec(ptVolumeInSec.x / aVolumeSize2[0], 
        ptVolumeInSec.y / aVolumeSize2[1], 
        ptVolumeInSec.z / aVolumeSize2[2]);
    //printf("%f, %f, %f", ptModelInSec.x, ptModelInSec.y, ptModelInSec.z);
}

Mcsf::MedViewer3D::Matrix4x4 FusionGpuMprAlgorithm::GetFusionRegistrationMatrix() const
{
    return m_matRegistrationMatrix;
}

void FusionGpuMprAlgorithm::LinkProgram()
{
    m_uiProgramID = glCreateProgram();

    // Compile and Link Program
    auto it = m_vecSteps.begin();

    unsigned int uiShaderType = 0;
    unsigned int uiShaderID = 0;
    GLint compileStatus;

    std::vector<std::pair<GLenum, const char*>>::iterator iter;
    for (iter = m_vecShaderType.begin(); iter != m_vecShaderType.end(); ++iter)
    {
        uiShaderType = iter->first;

        uiShaderID = glCreateShader(uiShaderType);

        glShaderSource(uiShaderID, 1, const_cast<const GLchar**>(&iter->second), 0);

        glCompileShader(uiShaderID);

        glGetShaderiv(uiShaderID, GL_COMPILE_STATUS, &compileStatus);
        if (GL_TRUE != compileStatus)
        {
            GLint len = 0;
            glGetShaderiv(uiShaderID, GL_INFO_LOG_LENGTH, &len);
            if (len > 0)
            {
                char *pInfor = new char[len];
                glGetShaderInfoLog(uiShaderID, len, NULL, pInfor);
                printf("%s\n",pInfor);
                delete []pInfor;
            }
            throw std::exception("Shander compile Failed");
        }

        glAttachShader(m_uiProgramID, uiShaderID);

        m_uiShaderIDs.push_back(uiShaderID);

    }



    // Link Program
    glLinkProgram(m_uiProgramID);

    GLint bLinked;
    glGetProgramiv(m_uiProgramID, GL_LINK_STATUS, &bLinked);
    if (GL_TRUE != bLinked)
    {
        GLint iLen;
        glGetProgramiv(m_uiProgramID, GL_INFO_LOG_LENGTH, &iLen);
        if (iLen > 0)
        {
            char* pInfo = new char[iLen];
            glGetProgramInfoLog(m_uiProgramID, iLen, 0, pInfo);
            printf("Error: %s", pInfo);
            delete[] pInfo;
        }
        glDeleteProgram(m_uiProgramID);
        throw std::exception("Program Link Failed");
    }

    // Get uniform locations
    glUseProgram(m_uiProgramID);


    GetShaderParamUniformID();

    glUseProgram(0);
}

bool FusionGpuMprAlgorithm::InitVAO()
{
    /// Generate VBO
    // 1. Vertex VBO
    glGenBuffers(1, &m_uiArrayBufferVertex);
    // 2. Element VBO
    glGenBuffers(1, &m_uiArrayBufferElementIdx);

    GLfloat plane_vertice[12] = 
    {
        -0.5, -0.5, 0.0, 
        0.5, -0.5, 0.0, 
        0.5, 0.5, 0.0, 
        -0.5, 0.5, 0.0
    };

    GLuint plane_elements_triangle[6] = { 0, 1, 3, 1, 2, 3 };

    /// Bind VAO
    // 1. Generate VAO
    glGenVertexArrays(1, &m_uiVAOID);
    glBindVertexArray(m_uiVAOID);

    // 2. Bind vertex array buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_uiArrayBufferVertex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertice), plane_vertice, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // 3. Bind element id array buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiArrayBufferElementIdx);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_elements_triangle), plane_elements_triangle, GL_STATIC_DRAW);

    // 4. Bind vertex array to 0
    glBindVertexArray(0);

    return true;
}

void FusionGpuMprAlgorithm::ClearVAO()
{
    if (m_uiVAOID != 0)
    {
        glDeleteBuffers(1, &m_uiArrayBufferVertex);
        glDeleteBuffers(1, &m_uiArrayBufferElementIdx);
        glDeleteVertexArrays(1, &m_uiVAOID);
        m_uiVAOID = 0;
    }
}

void FusionGpuMprAlgorithm::DeletePrvPogram()
{
    if (0 == m_uiProgramID)
    {
        return;
    }
    auto it = m_uiShaderIDs.begin();
    for(;it != m_uiShaderIDs.end();++it)
    {
        unsigned int id = *(it);
        glDetachShader(m_uiProgramID, id);
        glDeleteShader(id);
    }

    glDeleteProgram(m_uiProgramID);

    m_uiShaderIDs.clear();
    if (!m_vecSteps.empty())
    {
        for (auto itr = m_vecSteps.begin(); itr != m_vecSteps.end(); ++itr)
        {
            delete *itr;
        }
        m_vecSteps.clear();
    }
    m_uiProgramID = 0;
}

void FusionGpuMprAlgorithm::Render()
{
    //Mcsf::MedViewer3D::GpuMprAlgorithm::Render();

    if (m_bShaderUpdated)
    {
        this->DeletePrvPogram();

        this->LinkProgram();
        m_bShaderUpdated = false;
    }

    if (0 == m_uiVAOID)
    {
        /// Generate CUBE VAO 
        this->InitVAO();
    }

    // enable stencil test in case of magic glass
    // the rendering of mpr inside magic glass should write the
    // stencil buffer and the fragments inside the magic glass
    // of this pass will fail during stencil test
    glStencilMask(0x01);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_GREATER, 0x01, 0x01);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // enable depth test to treat the mpr plane as an occluder
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glUseProgram(m_uiProgramID);

    //
    SetUniformValue();

    /// Draw VAO
    glBindVertexArray(m_uiVAOID);
    //glDrawElements(GL_TRIANGLE_STRIP, sizeof(unsigned int) * 4, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_TRIANGLES, sizeof(unsigned int) * 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glStencilMask(0xff);
}

void FusionGpuMprAlgorithm::SetVertices(Mcsf::MedViewer3D::Point3D& vertexPoints)
{
    m_vertexPoints = &vertexPoints;
}

void FusionGpuMprAlgorithm::SetVolumeDisplay(ImageDataPtr pVolumeDisplay)
{
    m_pVolumeDisplay = pVolumeDisplay;
}

void FusionGpuMprAlgorithm::SetDisplaySize(int iWidth, int iHeight)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;
}

void FusionGpuMprAlgorithm::SetFusionWindowLevel(double dWindowingWidth, double dWindowingCenter)
{
    m_dWindowingWidth = dWindowingWidth;
    m_dWindowingCenter = dWindowingCenter;
}

void FusionGpuMprAlgorithm::SetFusionRegistrationMatrix(Mcsf::MedViewer3D::Matrix4x4& matRegistrationMatrix)
{
    m_matRegistrationMatrix = matRegistrationMatrix;
}
void FusionGpuMprAlgorithm::SetPseudoColorFusionTexID(unsigned int uiID)
{
    m_uiFusionColorID = uiID;
}
TPS_END_NAMESPACE  // end namespace tps