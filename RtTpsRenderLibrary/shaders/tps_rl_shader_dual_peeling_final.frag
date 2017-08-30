uniform sampler2DRect FrontBlenderTex;

void main(void)
{
    gl_FragColor = texture(FrontBlenderTex, gl_FragCoord.xy);
}