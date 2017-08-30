#version 430

uniform sampler2D texDose;
uniform sampler1D texDisplayValue;
uniform sampler1D texDisplayColor;
uniform mat4 matScrToDoseTex;
uniform uint uiVisionMode;
uniform float fTransparency;

out vec4 vFragColor;

vec4 texture_bicubic(sampler2D texture, vec2 coord)
{
    vec2 voxel_dim = textureSize(texture, 0);
    vec2 coord_grid = coord * voxel_dim - 0.5;
    vec2 floor_coord = floor(coord_grid);
    vec2 fraction = coord_grid - floor_coord;
    vec2 one_frac = 1.0 - fraction;
    
    vec2 w0 = 1.0/6.0 * one_frac * one_frac * one_frac;
    vec2 w1 = 2.0/3.0 - 0.5 * fraction * fraction * (2.0 - fraction);
    vec2 w2 = 2.0/3.0 - 0.5 * one_frac * one_frac * (2.0 - one_frac);
    vec2 w3 = 1.0/6.0 * fraction * fraction * fraction;
    
    vec2 g0 = w0 + w1;
    vec2 g1 = w2 + w3;
    vec2 mult = 1.0 / voxel_dim;
    vec2 h0 = mult * ((w1 / g0) - 0.5 + floor_coord);
    vec2 h1 = mult * ((w3 / g1) + 1.5 + floor_coord);
    
    vec4 tex00 = texture2D(texture, h0);
    vec4 tex10 = texture2D(texture, vec2(h1.x, h0.y));
    vec4 tex01 = texture2D(texture, vec2(h0.x, h1.y));
    vec4 tex11 = texture2D(texture, h1);
    return mix(mix(tex11, tex01, g0.x), mix(tex10, tex00, g0.x), g0.y);
}

float TestNeighborhood(float dose, vec4 nbhdDose, float displayValue)
{
    if (displayValue == dose) return 1.0;
    float delta = displayValue - dose;
    float alpha = 0.0, temp;
    
    for (int i=0; i<4; ++i) {
        temp = nbhdDose[i] - dose;
        if (sign(temp) != sign(delta) || abs(temp) < abs(delta)) continue;
        //alpha = max(alpha, smoothstep(abs(temp), 0.0, abs(delta)));
        alpha += smoothstep(abs(temp), 0.0, abs(delta));
    }
    alpha = clamp(alpha, 0, 1);
    return alpha;
}

void main()
{
    if (uiVisionMode == -1) discard;
    int iLineCount = textureSize(texDisplayValue, 0);
    if (iLineCount == 0) discard;
    
    vec2 vTexCoor = (matScrToDoseTex * vec4(gl_FragCoord.xy, 0, 1)).xy;
    if (any(lessThanEqual(vTexCoor, vec2(0.0, 0.0))) ||
        any(greaterThanEqual(vTexCoor, vec2(1.0, 1.0)))) discard;
    //float fDose = texture(texDose, vTexCoor).r;
    float fDose = texture_bicubic(texDose, vTexCoor).r;

    bool frag_handled = false;
    //IF Vision Mode is COLORWASH, calculate the texture coordinate
    if (uiVisionMode == 1 || uiVisionMode == 2)
    {
        int i = 0;
        float c1 = 0.0, c2 = 0.0, v1 = 0.0, v2 = 0.0;
        while (i < iLineCount)
        {
            c2 = (float(i) + 0.5) / iLineCount;
            v2 = texture(texDisplayValue, c2).r;
            if (fDose <= v2) break;
            c1 = c2;
            v1 = v2;
            i++;
        }
        if (i == 0) discard;
        float c = v1 == v2 ? c2 : c1 + (fDose - v1) / (v2 - v1) / iLineCount;
        vFragColor = vec4(texture(texDisplayColor, c).rgb, fTransparency);
        frag_handled = true;
    }
    //IF Vision Mode is LINE, draw the iso-dose line
    if (uiVisionMode == 0 || uiVisionMode == 2) 
    {
        float detDose = fwidth(fDose);
        float value, alpha;
        vec4 color;
        for (int i=0; i<iLineCount; ++i)
        {
            value = texelFetch(texDisplayValue, i, 0).r;
            alpha = 1 - smoothstep(0.0, detDose, abs(fDose - value));
            //handle ZERO dose value
            if (alpha == 0.0 || abs(value) <= 1e-6) continue;
            color = texelFetch(texDisplayColor, i, 0);
            if (!frag_handled)
            {
                vFragColor = vec4(color.rgb, alpha);
                frag_handled = true;
            }
            else
            {
                color.a = 1.0;
                vFragColor = mix(vFragColor, color, alpha);
            }
        }
    }
    if (!frag_handled) discard;
}