#version 430
uniform sampler3D texVolume;
uniform sampler2D texFrontFace;
uniform sampler2D texBackFace;
uniform sampler1D texCT2Density;
uniform int iCT2DensityStart;
uniform vec3 vSpacing;
uniform float fSampleRate;
uniform float fRescaleIntercept;
uniform float fRescaleSlope;
uniform float fWaterAttenuation;
uniform sampler1D texDensity2CT;
uniform float fDensityStart;
uniform float fDensityInterval;
uniform mat4 matModel2Patent;

in vec2 vTexCoord;
out vec4 vFragColor;

float CT2Density(float ct)
{
   int texSize = textureSize(texCT2Density, 0);
   float texCoord = (ct - iCT2DensityStart + 0.5) / texSize;
   return texture(texCT2Density, texCoord).r;
}

float StoredValue2CT(float gray)
{
   return gray * fRescaleSlope + fRescaleIntercept;
}

float Density2CT(float density)
{
   float texCoord = (density - fDensityStart) / fDensityInterval + 0.5;
   int texSize = textureSize(texDensity2CT, 0);
   texCoord /= texSize;
   return texture(texDensity2CT, texCoord).r;
}

vec4 RayCasting(vec3 start, vec3 end)
{
    if (end == start) discard;
    vec3 vDirt = normalize(end - start);
    vec3 step = abs(vSpacing / vDirt);
    float minStep = fSampleRate * 0.5 * min(step.x, min(step.y, step.z));
    vDirt *= minStep;
    float fStepNum = floor(length(end - start) / minStep);
    float vStepLenInPat = length(matModel2Patent * vec4(vDirt, 0.0));

    vec3 point;
    float gray, ct, density;
    float sum = 0.0;
    for(float i = 0.0; i < fStepNum; ++i)
    {
        point = start + i * vDirt;
        gray = texture(texVolume, point).x;
        ct = StoredValue2CT(gray);
        density = CT2Density(ct);
        ct = Density2CT(density);
        sum += fWaterAttenuation * (1.0 + ct / 1000.0);
    }
    float intensity = clamp(1.0 - exp(-0.1 * sum * vStepLenInPat), 0, 1);
    return vec4(intensity, intensity, intensity, 1.0); 
}

void main()
{
    vec3 vStart = texture(texFrontFace, vTexCoord).xyz;
    vec3 vEnd = texture(texBackFace, vTexCoord).xyz;
    vFragColor = RayCasting(vStart, vEnd);
}