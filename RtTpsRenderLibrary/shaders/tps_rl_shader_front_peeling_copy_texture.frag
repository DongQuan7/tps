uniform sampler2DMS ColorTex;
uniform int iSampleCount;

void main(void)
{
    gl_FragColor = texelFetch(ColorTex, ivec2(gl_FragCoord.xy), gl_SampleID);
}