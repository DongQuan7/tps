#version 430
uniform sampler2D texFluenceMap;
uniform sampler1D texLutValue;
uniform sampler1D texLutColor;
uniform float fWW;
uniform float fWL;
    
in vec4 oTexCoord;
out vec4 vFragColor;
void main()
{
    int iLutSize = textureSize(texLutValue, 0);
    if (iLutSize == 0) discard;
        
    float intensity = texture(texFluenceMap, oTexCoord.xy).r;
    float value = (intensity - fWL) / fWW + 0.5;
    int i = 0;
    float c1 = 0.0, c2 = 0.0, v1 = 0.0, v2 = 0.0;
    while (i < iLutSize)
    {
        c2 = (float(i) + 0.5) / iLutSize;
        v2 = texture(texLutValue, c2).r;
        if (value <= v2) break;
        c1 = c2;
        v1 = v2;
        i++;
    }

    float coordinate;
    if (i == 0)
    {
        coordinate = 0.5 / iLutSize;
    }
    else if (v1 == v2)
    {
        coordinate = c2;
    }
    else
    {
        coordinate = c1 + (value - v1) / (v2 - v1) / iLutSize;
    }
    vFragColor = vec4(texture(texLutColor, coordinate).rgb, 1.0);
}