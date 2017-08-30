uniform sampler2DMS DepthTex;
uniform sampler2DMS ColorTex;
uniform int iSampleCount;

vec4 ShadeFragment();

void main(void)
{
    float depth = texelFetch(DepthTex, ivec2(gl_FragCoord.xy), gl_SampleID).r;
    if (gl_FragCoord.z <= depth)
    {
        discard;
    }

    //exclude already opaque pixel
    vec4 pre_color = texelFetch(ColorTex, ivec2(gl_FragCoord.xy), gl_SampleID);
    if (pre_color.a <= 0.0)
    {
        discard;
    }

    vec4 color = ShadeFragment();
    gl_FragDepth = gl_FragCoord.z;
    // under blending
    gl_FragColor.rgb = color.rgb * color.a * pre_color.a + pre_color.rgb;
    gl_FragColor.a = (1 - color.a) * pre_color.a;
}