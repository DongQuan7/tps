#version 430

in vec4 vStart;
in vec4 vEnd;

uniform sampler3D sVolumeData;
uniform vec2 vWindowLevel;
uniform float fSteps;
uniform sampler1D sPseudoColorTable;
uniform uint uiPseudoColorTableSize;

//uniform int iAlgorithmType;
uniform bool isInverted;

void MPRCastingLoop(vec3 vStartPoint, vec3 vRayDir, 
    float fSteps, out float fSumGray, out float fNum);

vec3 MPRCasting()
{
    vec3 vTexRayDir = (vEnd - vStart).xyz / fSteps;
    vec3 rayCenter = (vEnd + vStart).xyz * 0.5;
    float halfSteps = fSteps * 0.5;
    float fSumGray, fNum;
    MPRCastingLoop(rayCenter, vTexRayDir, halfSteps, fSumGray, fNum);

    bool isOutOfVolume = fNum < 1.0;
    if (isOutOfVolume)
    {
        discard;
    }
    else
    {
        float fGray = fSumGray / fNum;
        fGray = clamp((fGray - vWindowLevel.y) / vWindowLevel.x + 0.5, 0.0, 1.0);
        if (isInverted) fGray = 1.0 - fGray;
        float fCoord = fGray + (0.5 - fGray) / uiPseudoColorTableSize;
        return texture(sPseudoColorTable, fCoord).xyz;
    }
}

void MPRCastingLoop(vec3 vStartPoint, vec3 vRayDir, 
    float fSteps, out float fSumGray, out float fNum)
{
    fSumGray = 0.0;
    fNum = 0.0;
    if (!(any(lessThan(vStartPoint, vec3(0.0, 0.0, 0.0))) || 
        any(greaterThan(vStartPoint, vec3(1.0, 1.0, 1.0)))))
    {
        fSumGray += texture(sVolumeData, vStartPoint).r;
        fNum++;
    }

    vec3 vEndPoint = vStartPoint;
    for (float i = 1.0; i < fSteps; ++ i)
    {
        vStartPoint -= vRayDir;
        if (!(any(lessThan(vStartPoint, vec3(0.0, 0.0, 0.0))) || 
            any(greaterThan(vStartPoint, vec3(1.0, 1.0, 1.0)))))
        {
            fSumGray += texture(sVolumeData, vStartPoint).r;
            fNum++;
        }
        vEndPoint += vRayDir;
        if (!(any(lessThan(vEndPoint, vec3(0.0, 0.0, 0.0))) || 
            any(greaterThan(vEndPoint, vec3(1.0, 1.0, 1.0)))))
        {
            fSumGray += texture(sVolumeData, vEndPoint).r;
            fNum++;
        }
    }
}

bool IsVolumeEdge()
{
    vec3 center = (vEnd + vStart).xyz * 0.5;
    vec3 volSize = vec3(textureSize(sVolumeData, 0));
    
    return (center.x >= 0.0 && center.x <= 1 / volSize.x) || 
           (center.y >= 0.0 && center.y <= 1 / volSize.y) || 
           (center.z >= 0.0 && center.z <= 1 / volSize.z) || 
           (center.x >= 1.0 - 1 / volSize.x && center.x <= 1.0) || 
           (center.y >= 1.0 - 1 / volSize.y && center.y <= 1.0) || 
           (center.z >= 1.0 - 1 / volSize.z && center.z <= 1.0);
}