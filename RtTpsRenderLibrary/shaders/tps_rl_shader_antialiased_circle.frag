#version 430

uniform sampler1D sCenters;
uniform sampler1D sRadius;
uniform sampler1D sColors;
uniform usampler1D sIsFilled;
uniform int iCount;
uniform bool bDrawShadow;

out vec4 oFragColor;

void main()
{
    float coord, radius, dist, fw, alpha1, alpha2, alphaShadow;
    vec2 center;
    vec4 color;
    uint isFilled;
    float shadowWidth = 1.0;
    vec3 shadowColor = vec3(0.0, 0.0, 0.0);
    for(float i = 0; i < iCount; ++i)
    {
        coord = (i + 0.5) / iCount;
        radius = texture(sRadius, coord).x;
        center = texture(sCenters, coord).xy;
        color = texture(sColors, coord);
        isFilled = texture(sIsFilled, coord).x;

        shadowColor = (1 - color.xyz) * 0.1;

        dist = distance(gl_FragCoord.xy, center);
        fw = fwidth(dist);
        alpha1 = 1 - smoothstep(radius, radius + fw, dist);
        alphaShadow = 1 - smoothstep(radius + shadowWidth, radius + shadowWidth + fw, dist);
        if (alpha1 == 0 && (!bDrawShadow || alphaShadow == 0)) continue;
        if (isFilled != 0)
        {
            if (!bDrawShadow || alpha1 == 1.0)
            {
                oFragColor = vec4(color.xyz, alpha1);
            }
            else
            {
                oFragColor = vec4(mix(shadowColor, color.xyz, alpha1), alphaShadow);
            }
            return;
        }
        else
        {
            alpha2 = smoothstep(radius - fw, radius, dist);
            if (alpha2 == 0.0) continue;
            if (!bDrawShadow || alpha1 == 1.0)
            {
                oFragColor = vec4(color.xyz, min(alpha1, alpha2));
            }
            else
            {
                oFragColor = vec4(mix(shadowColor, color.xyz, min(alpha1, alpha2)), alphaShadow);
            }
            return;
        }
    }
    discard;
}